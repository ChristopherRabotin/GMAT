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

// Uncomment to generate drag model data for debugging:
// #define     DEBUG      

#ifdef DEBUG
   #include <fstream>
   
   std::ofstream dragdata;
#endif


//------------------------------------------------------------------------------
// DragForce(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor for the DragForce.
 * 
 * @param name Optional name for this force component.
 */
//------------------------------------------------------------------------------
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
    fluxF107               (230.0),
    fluxF107A              (210.0),
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
    
    // Default Sun location, from the SLP file at MJD 21545.0:
    sunLoc[0] =  2.65e+07; 
    sunLoc[1] = -1.32757e+08;
    sunLoc[2] = -5.75566e+07;
    
    cbLoc[0]  = 0.0;
    cbLoc[1]  = 0.0;
    cbLoc[2]  = 0.0;
    
    // Nominal Earth angular velocity
    angVel[0]      = 0.0;
    angVel[1]      = 0.0;
    angVel[2]      = 7.29211585530e-5;

    #ifdef DEBUG
         dragdata.open("DragData.csv");
         dragdata << "Atmospheric drag parameters\n";
    #endif
}


//------------------------------------------------------------------------------
// ~DragForce()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
DragForce::~DragForce()
{
    if (!useExternalAtmosphere && atmos)
        delete atmos;
        
    if (density)
        delete [] density;
        
    if (prefactor)
        delete [] prefactor;

    #ifdef DEBUG
         dragdata.close();
    #endif
}


//------------------------------------------------------------------------------
// DragForce(const DragForce& df)
//------------------------------------------------------------------------------
/**
 * Copy Constructor.
 * 
 * @param df The DragForce instance used to set parameters for this clone.
 */
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
// DragForce& operator=(const DragForce& df)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param df DragForce instance used as a template for this copy.
 * 
 * @return A reference to this class, with members set to match the template.
 */
//------------------------------------------------------------------------------
DragForce& DragForce::operator=(const DragForce& df)
{
    if (this == &df)
        return *this;
        
    PhysicalModel::operator=(df);        
    return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the DragForce.
 *
 * @return clone of the DragForce.
 *
 */
//------------------------------------------------------------------------------
GmatBase* DragForce::Clone(void) const
{
   return (new DragForce(*this));
}

//------------------------------------------------------------------------------
// bool GetComponentMap(Integer * map, Integer order) const
//------------------------------------------------------------------------------
/**
 * Mapping function for integrators of different orders.
 * 
 * Provides a map between the incoming state and the location of the 
 * corresponding derivative information.  The resulting data, passed back in the 
 * array map, is used to determine where the derivative data is stored for a
 * given component of the input state.
 * 
 * @param map     Array used for the mapping
 * @param order   Order of the requesting class (usually an integrator)
 */
//------------------------------------------------------------------------------
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
    
    if (parmName == "DryMass")
        if (parmNumber < mass.size())
            mass[i] = parm;
        else
            mass.push_back(parm);
    if (parmName == "CoefficientDrag")
        if (parmNumber < dragCoeff.size())
            dragCoeff[i] = parm;
        else
            dragCoeff.push_back(parm);
    if (parmName == "DragArea")
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


//------------------------------------------------------------------------------
// void Initialize(void)
//------------------------------------------------------------------------------
/**
 * Allocates memory and performs other drag force initialization.
 */
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
// void BuildPrefactors(void) 
//------------------------------------------------------------------------------
/**
 * Builds drag prefactors prior to modeling the force.
 * 
 * The drag prefactor is given by
 * 
 *     \f[F_d = -{1 \over 2} {{C_d A} \over {m}} \f]
 * 
 * The atmospheric model classes provide densities in kg/m^3.  Since we need
 * accelerations in km/s^2, there is an extra factor of 1000 in the prefactor
 * to account for the units needed in the for the accelerations.  Areas are 
 * expressed in units of m^2, and the mass is in kg.
 */
//------------------------------------------------------------------------------
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
        
        // Note: Prefactor is scaled to account for density in kg / m^3
        prefactor[i] = -500.0 * dragCoeff[i] * area[i] / mass[i];

       #ifdef DEBUG
            dragdata << "Prefactor data\n   Spacecraft "
                     << i 
                     << "\n      Cd = " << dragCoeff[i] 
                     << "\n      Area = " << area[i] 
                     << "\n      Mass = " << mass[i]
                     << "\n      Prefactor = " << prefactor[i]  << "\n";
       #endif
    }
}


//------------------------------------------------------------------------------
// bool GetDerivatives(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Method invoked to calculate accelerations due to drag
 * 
 * This method calculates the acceleration due to drag using the formula
 * 
 * \f[\vec a = - {1\over 2} {{C_d A} \over {m}} \rho v_{rel}^2 \hat v_{rel}\f]
 *
 * @param dt            Additional time increment for the derivatitive 
 *                      calculation; defaults to 0.
 * @param state         Pointer to the current state data.  This can differ
 *                      from the PhysicalModel state if the subscribing
 *                      integrator samples other state values during 
 *                      propagation.  (For example, the Runge-Kutta integrators 
 *                      do this during the stage calculations.)
 * @param order         The order of the derivative to be taken (first 
 *                      derivative, second derivative, etc)
 *
 * @return              true if the call succeeds, false on failure.
 */
bool DragForce::GetDerivatives(Real *state, Real dt, Integer order)
{
    Integer i, i6;
    Real vRelative[3], vRelMag, factor;
    
    GetDensity(state, epoch + (elapsedTime + dt) / 86400.0);
    
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

            #ifdef DEBUG
               for (Integer m = 0; m < satCount; ++m)
                  dragdata << "   Accel: " 
                           << deriv[3+i6] << "  "
                           << deriv[4+i6] << "  "
                           << deriv[5+i6] << "\n";
            #endif
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

            #ifdef DEBUG
               for (Integer m = 0; m < satCount; ++m)
                  dragdata << "   Accel: " 
                           << deriv[i6] << "  "
                           << deriv[1+i6] << "  "
                           << deriv[2+i6] << "\n";
            #endif
        }
    }
    
    return true;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor for the string names for the parameters
 *
 * @param      id    ID for the parameter of interest.
 * 
 * @return     text identifier for the parameter.
 */
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


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Accessor for the string IDs for the parameters
 *
 * @param      str    text identifier for the parameter.
 * 
 * @return     ID for the parameter of interest.
 */
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


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor for the data types for the parameters
 *
 * @param      id    ID for the parameter of interest.
 * 
 * @return     data type for the parameter of interest.
 */
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


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor for the string description of the data types for the parameters
 *
 * @param      id    ID for the parameter of interest.
 * 
 * @return     string describing the data type for the parameter of interest.
 */
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


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Read accessor for the floating point parameters
 *
 * @param      id    ID for the parameter of interest.
 * 
 * @return     current value for the parameter.
 */
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


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Write accessor for the floating point parameters
 *
 * @param      id    ID for the parameter of interest.
 * @param      value The new value for the parameter.
 * 
 * @return     current (new) value for the parameter.
 */
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


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Read accessor for the text parameters
 *
 * @param      id    ID for the parameter of interest.
 * 
 * @return     current value for the parameter.
 */
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


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Write accessor for the text parameters.
 *
 * @param      id    ID for the parameter of interest.
 * @param      value The new value for the parameter.
 * 
 * @return     true if the parameter was set, false if the call failed.
 */
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


//------------------------------------------------------------------------------
// void DragForce::GetDensity(Real *state)
//------------------------------------------------------------------------------
/**
 * Accessor for the density calculation in the embedded AtmosphereModel.
 * 
 * @param state Cartesian position/velocity state specifying where the density
 *              is needed.
 * @param when  TAI Modified Julian epoch for the calculation
 */
void DragForce::GetDensity(Real *state, Real when)
{
    // Give it a default value if the atmosphere model is not set
    if (!atmos) {
        for (Integer i = 0; i < satCount; ++i)
            density[i] = 4.0e-13;
    }
    else {
        if (atmos) {
            if (sun && centralBody) {
                // Update the Sun vector
                Rvector sunV = sun->GetState(when);
                Rvector cbV  = centralBody->GetState(when);
                
                sunLoc[0] = sunV[0];
                sunLoc[1] = sunV[1];
                sunLoc[2] = sunV[2];
                cbLoc[0]  = cbV[0];
                cbLoc[1]  = cbV[1];
                cbLoc[2]  = cbV[2];
            }
        }
        atmos->Density(state, density, when, satCount);
        
        #ifdef DEBUG
            for (Integer m = 0; m < satCount; ++m)
               dragdata << "   Epoch: " << when 
                        << "   State: " 
                        << state[m*6] << "  "
                        << state[m*6+1] << "  "
                        << state[m*6+2] << "    Density: "
                        << density[m];
        #endif
    }
}
