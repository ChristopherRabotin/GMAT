//$Header$
//------------------------------------------------------------------------------
//                                DragForce
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/29
//
/**
 * Drag force modeling
 */
//------------------------------------------------------------------------------


#include "DragForce.hpp"
#include "ForceModelException.hpp"

DragForce::DragForce(const std::string &name) :
    PhysicalModel          (Gmat::PHYSICAL_MODEL, "DragForce", name),
    sun                    (NULL),
    centralBody            (NULL),
    useExternalAtmosphere  (true),
    atmosphereType         (""),  
    atmos                  (NULL),
    density                (NULL),
    prefactor              (NULL),
    bodyName               ("Earth"),
    dataType               ("Constant"),
    fluxFile               (""),
    fluxF107               (180.0),
    fluxF107A              (120.0),
    ap                     (40.0),
    atmosphereModelID      (parameterCount),
    centralBodyID          (parameterCount+1),
    sourceTypeID           (parameterCount+2),
    fluxFileID             (parameterCount+3),
    fluxID                 (parameterCount+4),
    averageFluxID          (parameterCount+5),
    magneticIndexID        (parameterCount+6)
{
    dimension = 6;
    parameterCount+=7;
    
    /// @todo Remove hard coded central body parms in DragForce
    // Sun location at MJD 21545.0, from the SLP file:
    sunLoc[0] =  2.65e+07; 
    sunLoc[1] = -1.32757e+08;
    sunLoc[2] = -5.75566e+07;
    
    cbLoc[0]  = 0.0;
    cbLoc[1]  = 0.0;
    cbLoc[2]  = 0.0;
    
    angVel[0]      = 0.0;
    angVel[1]      = 0.0;
    angVel[2]      = 7.29211585530e-5;
}


DragForce::~DragForce()
{
    if (!useExternalAtmosphere && atmos)
        delete atmos;
        
    if (density)
        delete [] density;
        
    if (prefactor)
        delete [] prefactor;
}


DragForce::DragForce(const DragForce& df) :
    PhysicalModel           (df),
    sun                     (NULL),
    centralBody             (NULL),
    useExternalAtmosphere   (true),
    atmosphereType          (df.atmosphereType),  
    atmos                   (NULL),
    density                 (NULL),
    prefactor               (NULL),
    bodyName                (df.bodyName),
    dataType                (df.dataType),
    fluxFile                (df.fluxFile),
    fluxF107                (df.fluxF107),
    fluxF107A               (df.fluxF107A),
    ap                      (df.ap),
    atmosphereModelID       (parameterCount),
    centralBodyID           (parameterCount+1),
    sourceTypeID            (parameterCount+2),
    fluxFileID              (parameterCount+3),
    fluxID                  (parameterCount+4),
    averageFluxID           (parameterCount+5),
    magneticIndexID         (parameterCount+6)
{
    parameterCount += 7;
    dimension = df.dimension;

    /// @todo Remove hard coded central body parms in DragForce
    sunLoc[0] = 138276412034.25;
    sunLoc[1] = -71626341186.98;
    sunLoc[2] =    119832241.16;
    
    cbLoc[0]  = 0.0;
    cbLoc[1]  = 0.0;
    cbLoc[2]  = 0.0;
    
    angVel[0]      = 0.0;
    angVel[1]      = 0.0;
    angVel[2]      = 7.29211585530e-5;
}


DragForce& DragForce::operator=(const DragForce& df)
{
    if (this == &df)
        return *this;
        
    PhysicalModel::operator=(df);        
    return *this;
}

 
bool DragForce::GetComponentMap(Integer * map, Integer order) const
{
    Integer i6;

    if (order != 1)
        throw ForceModelException("Drag supports 1st order equations of motion only");

    // Calculate how many spacecraft are in the model
    Integer satCount = (Integer)(dimension / 6);
    for (Integer i = 0; i < satCount; i++) 
    {
        i6 = i * 6;

        map[ i6 ] = i6 + 3;
        map[i6+1] = i6 + 4;
        map[i6+2] = i6 + 5;
        map[i6+3] = -1;
        map[i6+4] = -1;
        map[i6+5] = -1;
    }

    return true;
}

//------------------------------------------------------------------------------
// void SetSatelliteParameter(const Integer i, const std::string parmName, 
//                            const Real parm)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft parameters to the force model.
 * 
 * For drag modeling, this method is used to set or update C_d, area, and mass.
 * 
 * @param i ID for the spacecraft
 * @param parmName name of the Spacecraft parameter 
 * @param parm Parameter value
 */
//------------------------------------------------------------------------------
void DragForce::SetSatelliteParameter(const Integer i, 
                                      const std::string parmName, 
                                      const Real parm)
{
    unsigned parmNumber = (unsigned)(i+1);
    
    if (parmName == "Mass")
        if (parmNumber < mass.size())
            mass[i] = parm;
        else
            mass.push_back(parm);
    if (parmName == "CoefficientDrag")
        if (parmNumber < dragCoeff.size())
            dragCoeff[i] = parm;
        else
            dragCoeff.push_back(parm);
    if (parmName == "IncidentArea")
        if (parmNumber < area.size())
            area[i] = parm;
        else
            area.push_back(parm);
}


//------------------------------------------------------------------------------
// void SetSatelliteParameter(const Integer i, const std::string parmName, 
//                            const std::string parm)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft parameters to the force model.
 * 
 * For drag modeling, this method is used to set the body with the atmosphere.
 * 
 * @param i ID for the spacecraft
 * @param parmName name of the Spacecraft parameter 
 * @param parm Parameter value
 */
//------------------------------------------------------------------------------
void DragForce::SetSatelliteParameter(const Integer i, 
                                      const std::string parmName, 
                                      const std::string parm)
{
    unsigned parmNumber = (unsigned)(i+1);
    
    if (parmName == "ReferenceBody")
        if (parmNumber < mass.size())
            dragBody[i] = parm;
        else
            dragBody.push_back(parm);
}


bool DragForce::Initialize(void)
{
    bool retval = PhysicalModel::Initialize();
    
    if (retval) {
       satCount = dimension / 6;
       if (satCount <= 0)
           throw ForceModelException("Drag called with dimension zero");
           
       density   = new Real[satCount];
       prefactor = new Real[satCount];
   
       if (mass.size() > 0)
           BuildPrefactors();
       else
           if (mass.size() == 0) 
               for (Integer i = 0; i < satCount; ++i) {
                   prefactor[i] = -0.5 * 2.2 * 15.0 / 875.0;   // Dummy up the product
               }
           
       // Set the atmosphere model
       if (solarSystem) {
           sun = solarSystem->GetBody(SolarSystem::SUN_NAME);
           if (!sun)
               throw ForceModelException("The Sun is not in solar system");
           
           std::string bodyName;
           if (dragBody.size() > 0)
               bodyName = dragBody[0];
           else
               bodyName = "Earth";
           centralBody = solarSystem->GetBody(bodyName);
   
           if (!centralBody)
               throw ForceModelException("Central body (for Drag) not in solar system");
           if (useExternalAtmosphere) {
               atmos = centralBody->GetAtmosphereModel();
           }
           else {
               if (atmosphereType == "BodyDefault")
                  atmos = centralBody->GetAtmosphereModel();
               else
                  atmos = centralBody->GetAtmosphereModel(atmosphereType);
               if (!atmos)
                  throw ForceModelException("Atmosphere model not defined");
           }
               
           if (atmos)  {
               atmos->SetSunVector(sunLoc);
               atmos->SetCentralBodyVector(cbLoc);
           }
           else {
              if (atmosphereType != "BodyDefault") {
                 std::string msg = "Could not create ";
                 msg += atmosphereType;
                 msg += " atmosphere model";
                 throw ForceModelException(msg);
              }
           }
       }
    }
    
    return retval;
}


void DragForce::BuildPrefactors(void) 
{
    for (Integer i = 0; i < satCount; ++i) {
        if (mass.size() < (unsigned)(i+1))
            throw ForceModelException("Spacecraft not set correctly");
        if (mass[i] <= 0.0) {
            std::string errorMsg = "Spacecraft ";
            errorMsg += i;
            errorMsg += " has non-physical mass; Drag modeling cannot be used.";
            throw ForceModelException(errorMsg); 
        }
        prefactor[i] = -0.5 * dragCoeff[i] * area[i] / mass[i];
    }
}


bool DragForce::GetDerivatives(Real *state, Real dt, Integer order)
{
    Integer i, i6;
    Real vRelative[3], vRelMag, factor;
    
    GetDensity(state);
    
    for (i = 0; i < satCount; ++i) {
        i6 = i * 6;

        // v_rel = v - w x R
        vRelative[0] = state[i6+3] - 
                       (angVel[1]*state[i6+2] - angVel[2]*state[i6+1]);
        vRelative[1] = state[i6+4] - 
                       (angVel[2]*state[ i6 ] - angVel[0]*state[i6+2]);
        vRelative[2] = state[i6+5] - 
                       (angVel[0]*state[i6+1] - angVel[1]*state[ i6 ]);
        vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] + 
                       vRelative[2]*vRelative[2]);
        
        factor = prefactor[i] * density[i];
        if (order == 1) 
        {
            // Do dv/dt first, in case deriv = state
            deriv[3+i6] = factor * vRelMag * vRelative[0];
            deriv[4+i6] = factor * vRelMag * vRelative[1];
            deriv[5+i6] = factor * vRelMag * vRelative[2];
            // dr/dt = v
            deriv[i6]   = state[3+i6];
            deriv[1+i6] = state[4+i6];
            deriv[2+i6] = state[5+i6];
        }
        else 
        {
            // Feed accelerations to corresponding components directly for RKN
            deriv[ i6 ] = factor * vRelMag * vRelative[0];
            deriv[1+i6] = factor * vRelMag * vRelative[1];
            deriv[2+i6] = factor * vRelMag * vRelative[2];
            deriv[3+i6] = 0.0; 
            deriv[4+i6] = 0.0; 
            deriv[5+i6] = 0.0; 
        }
    }
    
    return true;
}


std::string DragForce::GetParameterText(const Integer id) const
{
    if (id == atmosphereModelID)
       return "AtmosphereModel";
    
    if (id == centralBodyID)
       return "AtmosphereBody";
    
    if (id == sourceTypeID)
       return "InputSource";
    
    if (id == fluxFileID)
       return "SolarFluxFile";
    
    if (id == fluxID)
       return "SolarFlux";
    
    if (id == averageFluxID)
       return "AverageSolarFlux";
    
    if (id == magneticIndexID)
       return "MagneticIndex";
    
    return PhysicalModel::GetParameterText(id);
}


Integer DragForce::GetParameterID(const std::string &str) const
{
    if (str == "AtmosphereModel")
       return atmosphereModelID;
       
    if (str == "AtmosphereBody")
       return centralBodyID;
    
    if (str == "InputSource")
       return  sourceTypeID;
    
    if (str == "SolarFluxFile")
       return  fluxFileID;
    
    if (str == "SolarFlux")
       return  fluxID;
    
    if (str == "AverageSolarFlux")
       return  averageFluxID;
    
    if (str == "MagneticIndex")
       return  magneticIndexID;
    
    return PhysicalModel::GetParameterID(str);
}


Gmat::ParameterType DragForce::GetParameterType(const Integer id) const
{
    if (id == atmosphereModelID)
       return Gmat::STRING_TYPE;
    
    if (id == centralBodyID)
       return Gmat::STRING_TYPE;
    
    if (id == sourceTypeID)
       return Gmat::STRING_TYPE;    // "File" or "Constant" for now
    
    if (id == fluxFileID)
       return Gmat::STRING_TYPE;
    
    if (id == fluxID)
       return Gmat::REAL_TYPE;
    
    if (id == averageFluxID)
       return Gmat::REAL_TYPE;
    
    if (id == magneticIndexID)
       return Gmat::REAL_TYPE;

    return PhysicalModel::GetParameterType(id);
}


std::string DragForce::GetParameterTypeString(const Integer id) const
{
    if ( (id == atmosphereModelID) ||
         (id == centralBodyID) ||
         (id == sourceTypeID) ||
         (id == fluxFileID) ||
         (id == fluxID) ||
         (id == averageFluxID) ||
         (id == magneticIndexID) )
       return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];

    return PhysicalModel::GetParameterTypeString(id);
}


Integer DragForce::GetIntegerParameter(const Integer id) const
{
    return PhysicalModel::GetIntegerParameter(id);
}


Integer DragForce::SetIntegerParameter(const Integer id, const Integer value)
{
    return PhysicalModel::SetIntegerParameter(id, value);
}


Real DragForce::GetRealParameter(const Integer id) const
{
    if (id == fluxID)
       return fluxF107;
    
    if (id == averageFluxID)
       return fluxF107A;
    
    if (id == magneticIndexID)
       return ap;

    return PhysicalModel::GetRealParameter(id);
}


Real DragForce::SetRealParameter(const Integer id, const Real value)
{
    if (id == fluxID) {
       if (value >= 0.0)
          fluxF107 = value;
       return fluxF107;
    }
    
    if (id == averageFluxID) {
       if (value >= 0.0)
          fluxF107A = value;
       return fluxF107A;
    }
    
    if (id == magneticIndexID) {
       if (value >= 0.0)
          ap = value;
       return ap;
    }

    return PhysicalModel::SetRealParameter(id, value);
}


std::string DragForce::GetStringParameter(const Integer id) const
{
   if (id == atmosphereModelID) 
      return atmosphereType;

   if (id == centralBodyID)
      return bodyName;
    
   if (id == sourceTypeID)
      return dataType;
    
   if (id == fluxFileID)
      return fluxFile;
    
   return PhysicalModel::GetStringParameter(id);
}


bool DragForce::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == atmosphereModelID) {
      atmosphereType = value;

      if ((value == "") || (value == "BodyDefault"))
         useExternalAtmosphere = true;
      else {
         if (!useExternalAtmosphere)
            delete atmos;
         atmos = NULL;
         useExternalAtmosphere = false;
      }

      return true;
   }
   
   if (id == centralBodyID) {
      if (value == "")
         return false;
      bodyName = value;
      return true;
   }
      
    
   if (id == sourceTypeID) {     // "File" or "Constant" for now
      if ((value != "File") && (value != "Constant"))
         return false;
      dataType = value;
      return true;
   }
    
   if (id == fluxFileID) {
      fluxFile = value;
      return true;
   }
    
   return PhysicalModel::SetStringParameter(id, value);
}

void DragForce::GetDensity(Real *state)
{
    Real now = 21545.0;
    
    // Give it a default value if the atmosphere model is not set
    if (!atmos) {
        for (Integer i = 0; i < satCount; ++i)
            density[i] = 4.0e-13;
    }
    else {
        if (atmos) {
            if (sun && centralBody) {
                // Update the Sun vector
                now = epoch + elapsedTime / 86400.0; 
                Rvector sunV = sun->GetState(now);
                Rvector cbV  = centralBody->GetState(now);
                
                sunLoc[0] = sunV[0];
                sunLoc[1] = sunV[1];
                sunLoc[2] = sunV[2];
                cbLoc[0]  = cbV[0];
                cbLoc[1]  = cbV[1];
                cbLoc[2]  = cbV[2];
            }
        }
        atmos->Density(state, density, now, satCount);
    }
}
