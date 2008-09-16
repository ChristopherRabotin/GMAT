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
#include "StringUtil.hpp"     // for ToString()
#include "ForceModelException.hpp"
#include "MessageInterface.hpp"

// Uncomment to generate drag model data for debugging:
//#define DEBUG_DRAGFORCE_DENSITY
//#define DEBUG_DRAGFORCE_PARAM

#ifdef DEBUG_DRAGFORCE_DENSITY
   #include <fstream>
   
   std::ofstream dragdata;
#endif


//---------------------------------
// static data
//---------------------------------
const std::string
DragForce::PARAMETER_TEXT[DragForceParamCount - PhysicalModelParamCount] =
{
   "AtmosphereModel",   // ATMOSPHERE_MODEL
   "AtmosphereBody",    // ATMOSPHERE_BODY
   "InputSource",       // SOURCE_TYPE
   "SolarFluxFile",     // FLUX_FILE
   "F107",              // FLUX
   "F107A",             // AVERAGE_FLUX
   "MagneticIndex",     // MAGNETIC_INDEX
};

const Gmat::ParameterType
DragForce::PARAMETER_TYPE[DragForceParamCount - PhysicalModelParamCount] =
{
   Gmat::STRING_TYPE,   // "AtmosphereModel"
   Gmat::STRING_TYPE,   // "AtmosphereBody",
   Gmat::STRING_TYPE,   // "InputSource", ("File" or "Constant" for now)
   Gmat::STRING_TYPE,   // "SolarFluxFile",
   Gmat::REAL_TYPE,     // "F107",
   Gmat::REAL_TYPE,     // "F107A",
   Gmat::REAL_TYPE,     // "MagneticIndex",
};

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
   internalAtmos          (NULL),
   density                (NULL),
   prefactor              (NULL),
   firedOnce              (false),
   satCount               (1),
   dragState              (NULL),
   //bodyName               ("Earth"),
   dataType               ("Constant"),
   fluxFile               (""),
   fluxF107               (150.0),
   fluxF107A              (150.0),
   kp                     (3.0)
{
   dimension = 6;
   parameterCount = DragForceParamCount;
   
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
   
   ap = CalculateAp(kp);
   
   #ifdef DEBUG_DRAGFORCE_DENSITY
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
   //if (!useExternalAtmosphere && atmos)
   //   delete atmos;
   if (internalAtmos)
      delete internalAtmos;
        
   if (density)
      delete [] density;
      
   if (dragState)
      delete [] dragState;
        
   if (prefactor)
      delete [] prefactor;

   #ifdef DEBUG_DRAGFORCE_DENSITY
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
   useExternalAtmosphere   (df.useExternalAtmosphere),
   atmosphereType          (df.atmosphereType),
   atmos                   (NULL),
   density                 (NULL),
   prefactor               (NULL),
   firedOnce               (false),
   satCount                (df.satCount),
   dragBody                (df.dragBody),
   dragState               (NULL),
   //bodyName                (df.bodyName),
   dataType                (df.dataType),
   fluxFile                (df.fluxFile),
   fluxF107                (df.fluxF107),
   fluxF107A               (df.fluxF107A),
   kp                      (df.kp)
{
   if (useExternalAtmosphere)
   {
      internalAtmos = NULL;
   }
   else
   {
      internalAtmos = (AtmosphereModel*)df.internalAtmos->Clone();
   }
   
   parameterCount += 7;
   dimension = df.dimension;

   sunLoc[0] = df.sunLoc[0];
   sunLoc[1] = df.sunLoc[1];
   sunLoc[2] = df.sunLoc[2];
   
   cbLoc[0]  = df.cbLoc[0];
   cbLoc[1]  = df.cbLoc[1];
   cbLoc[2]  = df.cbLoc[2];
   
   angVel[0]      = df.angVel[0];
   angVel[1]      = df.angVel[1];
   angVel[2]      = df.angVel[2];

   ap = CalculateAp(kp);
   
   area.clear();
   mass.clear();
   dragCoeff.clear();
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

   sun                   = NULL;
   centralBody           = NULL;
   useExternalAtmosphere = df.useExternalAtmosphere;
   atmosphereType        = df.atmosphereType;

   if (internalAtmos != NULL)
      delete internalAtmos;

   if (useExternalAtmosphere)
      internalAtmos = NULL;
   else
      internalAtmos = (AtmosphereModel*)df.atmos->Clone();

   atmos                 = NULL;
   density               = NULL;
   prefactor             = NULL;
   firedOnce             = false;
   //bodyName              = df.bodyName;
   satCount              = df.satCount;
   dragBody              = df.dragBody;
   dragState             = df.dragState;
   dataType              = df.dataType;
   fluxFile              = df.fluxFile;
   fluxF107              = df.fluxF107;
   fluxF107A             = df.fluxF107A;
   kp                    = df.kp;
   ap                    = CalculateAp(kp);
   
   dimension = df.dimension;

   sunLoc[0] = df.sunLoc[0];
   sunLoc[1] = df.sunLoc[1];
   sunLoc[2] = df.sunLoc[2];
   
   cbLoc[0]  = df.cbLoc[0];
   cbLoc[1]  = df.cbLoc[1];
   cbLoc[2]  = df.cbLoc[2];
   
   angVel[0]      = df.angVel[0];
   angVel[1]      = df.angVel[1];
   angVel[2]      = df.angVel[2];

   ap = CalculateAp(kp);
   
   area.clear();
   mass.clear();
   dragCoeff.clear();

   return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the DragForce.
 *
 * @return clone of the DragForce.
 *
 */
//------------------------------------------------------------------------------
GmatBase* DragForce::Clone() const
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
      throw ForceModelException(
         "Drag supports 1st order equations of motion only");

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

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Setting satellite parameter " << parmName
               << " for Spacecraft " << i << " to " << parm << "\n";
   #endif
    
   if (parmName == "DryMass")
      if (parmNumber < mass.size())
         mass[i] = parm;
      else
         mass.push_back(parm);
   if (parmName == "Cd")
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
// void ClearSatelliteParameters(const std::string parmName)
//------------------------------------------------------------------------------
/**
 * Resets the DragForce to receive a new set of satellite parameters.
 *
 * @param parmName name of the Spacecraft parameter.  The empty string clear all
 *                 of the satellite parameters for the PhysicalModel.
 */
//------------------------------------------------------------------------------
void DragForce::ClearSatelliteParameters(const std::string parmName)
{
   if ((parmName == "DryMass") || (parmName == ""))
      mass.clear();
   if ((parmName == "Cd") || (parmName == ""))
      dragCoeff.clear();
   if ((parmName == "DragArea") || (parmName == ""))
      area.clear();
}


//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Allocates memory and performs other drag force initialization.
 */
//------------------------------------------------------------------------------
bool DragForce::Initialize()
{
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Entered DragForce::Initialize()\n";
   #endif

   bool retval = PhysicalModel::Initialize();
    
   if (retval)
   {
      satCount = dimension / 6;
      
      if (dragState)
         delete [] dragState;\
      dragState = new Real[dimension];
      
      if (satCount <= 0)
         throw ForceModelException("Drag called with dimension zero");
           
      density   = new Real[satCount];
      prefactor = new Real[satCount];
   
      // Set the atmosphere model
      if (solarSystem)
      {
         sun = solarSystem->GetBody(SolarSystem::SUN_NAME);
         if (!sun)
            throw ForceModelException("The Sun is not in solar system");
           
         std::string bodyName;
         // Drag currently requires that the drag body be the Earth.  When other
         // drag models are implemented, remove this block and test.
         for (StringArray::iterator i = dragBody.begin(); i != dragBody.end(); 
              ++i)
            if ((*i) != "Earth")
               throw ForceModelException(
                  "Drag modeling only works at the Earth in current GMAT "
                  "builds.");
         
         if (dragBody.size() > 0)
            bodyName = dragBody[0];
         else
            bodyName = "Earth";
         centralBody = solarSystem->GetBody(bodyName);
   
         if (!centralBody)
            throw ForceModelException(
               "Central body (for Drag) not in solar system");
         if (useExternalAtmosphere)
         {
            atmos = centralBody->GetAtmosphereModel();
         }
         else
         {
            std::string modelBodyIsUsing =
               centralBody->GetAtmosphereModelType();
            
            //loj: 6/8/05 Added if block for AtmosDensity parameter getting
            // Density from the body
            if (modelBodyIsUsing == "Undefined")
            {
               #ifdef DEBUG_DRAGFORCE_DENSITY
               MessageInterface::ShowMessage
                  ("===> Setting atmosphereType=%s addr:%d to body:%s\n",
                   atmosphereType.c_str(), internalAtmos,
                   centralBody->GetName().c_str());
               #endif
               
               centralBody->SetAtmosphereModelType(atmosphereType);
               centralBody->SetAtmosphereModel(
                  (AtmosphereModel*)internalAtmos->Clone());
            }
            
            //if (atmosphereType == "BodyDefault")
            if ((atmosphereType == "BodyDefault") ||
                (atmosphereType == modelBodyIsUsing))
               atmos = centralBody->GetAtmosphereModel();
            else
               atmos = internalAtmos;
            if (!atmos)
               throw ForceModelException("Atmosphere model not defined");
            
         }
               
         if (atmos)
         {
            atmos->SetSunVector(sunLoc);
            atmos->SetCentralBodyVector(cbLoc);
            atmos->SetCentralBody(centralBody);
            atmos->SetSolarSystem(solarSystem);
         }
         else
         {
            if (atmosphereType != "BodyDefault")
            {
               std::string msg = "Could not create ";
               msg += atmosphereType;
               msg += " atmosphere model";
               throw ForceModelException(msg);
            }
         }
      }
   }
    
   firedOnce = false;

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Leaving DragForce::Initialize()\n";
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void BuildPrefactors() 
//------------------------------------------------------------------------------
/**
 * Builds drag prefactors prior to modeling the force.
 * 
 * The drag prefactor is given by
 * 
 *     \f[F_d = -\frac{1}{2} \frac{C_d A}{m} \f]
 * 
 * The atmospheric model classes provide densities in kg/m^3.  Since we need
 * accelerations in km/s^2, there is an extra factor of 1000 in the prefactor
 * to account for the units needed in the for the accelerations.  Areas are 
 * expressed in units of m^2, and the mass is in kg.
 */
//------------------------------------------------------------------------------
void DragForce::BuildPrefactors() 
{
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Building prefactors for " << satCount <<" Spacecraft\n";
   #endif
   
   if (!forceOrigin)
      throw ForceModelException(
         "Cannot use drag force: force model origin not set.");
    
   for (Integer i = 0; i < satCount; ++i)
   {
      if (mass.size() < (unsigned)(i+1))
         throw ForceModelException("Spacecraft not set correctly");
      if (mass[i] <= 0.0)
      {
         std::string errorMsg = "Spacecraft ";
         errorMsg += i;
         errorMsg += " has non-physical mass; Drag modeling cannot be used.";
         throw ForceModelException(errorMsg);
      }
        
      // Note: Prefactor is scaled to account for density in kg / m^3
      prefactor[i] = -500.0 * dragCoeff[i] * area[i] / mass[i];

      #ifdef DEBUG_DRAGFORCE_DENSITY
         dragdata << "Prefactor data\n   Spacecraft "
                  << i
                  << "\n      Cd = " << dragCoeff[i]
                  << "\n      Area = " << area[i]
                  << "\n      Mass = " << mass[i]
                  << "\n      Prefactor = " << prefactor[i]  << "\n";

         MessageInterface::ShowMessage(
            "Prefactor data\n   Spacecraft %d\n      Cd = %lf\n      "
            "Area = %lf\n      Mass = %lf\n      Prefactor = %lf\n", i,
            dragCoeff[i], area[i], mass[i], prefactor[i]);
      #endif
    }
}


//------------------------------------------------------------------------------
// void TranslateOrigin(const Real *state, const Real now)
//------------------------------------------------------------------------------
/**
 * Used when the force model origin is not coincident with the drag body.
 * 
 * @param state The state vector, in MJ2000Eq coordinates centered at the force
 *              model origin.
 * @param now   The epoch of the calculations, in A.1 Modified Julian format.
 *
 * @note The current implementation throws if the force model origin is not the
 *       same as the body producing drag.
 */
//------------------------------------------------------------------------------
void DragForce::TranslateOrigin(const Real *state, const Real now)
{
   memcpy(dragState, state, dimension * sizeof(Real));
   if (forceOrigin != centralBody)
   {
      throw ForceModelException(
         "DragForce::TranslateOrigin: Drag forces only work when the force "
         "model origin is the same as the body with the atmosphere producing "
         "drag in the current GMAT build.");
      Rvector6 cbrv = centralBody->GetMJ2000State(now);
      Rvector6 forv = forceOrigin->GetMJ2000State(now);
      Rvector6 delta = cbrv - forv;

      Integer i6;
      for (Integer i = 0; i < satCount; ++i)
      {
         i6 = i * 6;  //stateSize;
         dragState[i6]   -= delta[0];
         dragState[i6+1] -= delta[1];
         dragState[i6+2] -= delta[2];
         dragState[i6+3] -= delta[3];
         dragState[i6+4] -= delta[4];
         dragState[i6+5] -= delta[5];
      }
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
//------------------------------------------------------------------------------
bool DragForce::GetDerivatives(Real *state, Real dt, Integer order)
{
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Entered DragForce::GetDerivatives()\n";
   #endif

   Integer i, i6;
   Real vRelative[3], vRelMag, factor;

   if (!firedOnce)
   {
      if (mass.size() > 0)
         BuildPrefactors();
      else
         if (mass.size() == 0)
            for (Integer i = 0; i < satCount; ++i)
            {
               if (!forceOrigin)
                  throw ForceModelException(
                     "Cannot use drag force: force model origin not set.");
    
               #ifdef DEBUG_DRAGFORCE_DENSITY
                   dragdata << "Using default prefactors for " << satCount
                            << " Spacecraft\n";
               #endif
               prefactor[i] = -0.5 * 2.2 * 15.0 / 875.0; // Dummy up the product
            }
               
      firedOnce = true;
   }
    
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Looking up density\n";
   #endif

   // First translate to the drag body from the force model origin
   Real now = epoch + (elapsedTime + dt) / 86400.0;
   TranslateOrigin(state, now);
   GetDensity(dragState, now);

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "density[0] = " << density[0] << "\n";
   #endif
   
   #ifdef DEBUG_DRAGFORCE_EPOCH
      MessageInterface::ShowMessage("Drag epoch = %16.11lf\n", now);
   #endif
   
   for (i = 0; i < satCount; ++i)
   {
      i6 = i * 6;
      #ifdef DEBUG_DRAGFORCE_DENSITY
         dragdata << "Spacecraft " << (i+1) << ": ";
      #endif

      // v_rel = v - w x R
      vRelative[0] = dragState[i6+3] -
                     (angVel[1]*dragState[i6+2] - angVel[2]*dragState[i6+1]);
      vRelative[1] = dragState[i6+4] -
                     (angVel[2]*dragState[ i6 ] - angVel[0]*dragState[i6+2]);
      vRelative[2] = dragState[i6+5] -
                     (angVel[0]*dragState[i6+1] - angVel[1]*dragState[ i6 ]);
      vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] +
                     vRelative[2]*vRelative[2]);
        
      factor = prefactor[i] * density[i];

      if (order == 1)
      {
         // Do dv/dt first, in case deriv = state
         deriv[3+i6] = factor * vRelMag * vRelative[0];// - a_indirect[0];
         deriv[4+i6] = factor * vRelMag * vRelative[1];// - a_indirect[1];
         deriv[5+i6] = factor * vRelMag * vRelative[2];// - a_indirect[2];
         // dr/dt = v
         deriv[i6]   = state[3+i6];
         deriv[1+i6] = state[4+i6];
         deriv[2+i6] = state[5+i6];

         #ifdef DEBUG_DRAGFORCE_DENSITY
            for (Integer m = 0; m < satCount; ++m)
               dragdata << "   Drag Accel: "
                        << deriv[3+i6] << "  "
                        << deriv[4+i6] << "  "
                        << deriv[5+i6] << "\n";
            for (Integer m = 0; m < satCount; ++m)
            {
               MessageInterface::ShowMessage(
                  "   Position:   %16.9le  %16.9le  %16.9le\n",
                  state[i6], state[i6+1], state[i6+2]);
               MessageInterface::ShowMessage(
                  "   Velocity:   %16.9le  %16.9le  %16.9le\n",
                  state[i6+3], state[i6+4], state[i6+5]);
               MessageInterface::ShowMessage(
                  "   Drag Accel: %16.9le  %16.9le  %16.9le\n",
                  deriv[3+i6], deriv[4+i6], deriv[5+i6]);
               MessageInterface::ShowMessage(
                  "   Density:    %16.9le\n", density[i]);
            }
         #endif
      }
      else
      {
         // Feed accelerations to corresponding components directly for RKN
         deriv[ i6 ] = factor * vRelMag * vRelative[0];// - a_indirect[0];
         deriv[1+i6] = factor * vRelMag * vRelative[1];// - a_indirect[1];
         deriv[2+i6] = factor * vRelMag * vRelative[2];// - a_indirect[2];
         deriv[3+i6] = 0.0;
         deriv[4+i6] = 0.0;
         deriv[5+i6] = 0.0;

         #ifdef DEBUG_DRAGFORCE_DENSITY
            for (Integer m = 0; m < satCount; ++m)
               dragdata << "   Accel: "
                        << deriv[i6] << "  "
                        << deriv[1+i6] << "  "
                        << deriv[2+i6] << "\n";
            for (Integer m = 0; m < satCount; ++m)
            {
               MessageInterface::ShowMessage(
                  "   Position:   %16.9le  %16.9le  %16.9le\n",
                  state[i6], state[i6+1], state[i6+2]);
               MessageInterface::ShowMessage(
                  "    Velocity:   %16.9le  %16.9le  %16.9le\n",
                  state[i6+3], state[i6+4], state[i6+5]);
               MessageInterface::ShowMessage(
                  "   Drag Accel: %16.9le  %16.9le  %16.9le\n",
                  deriv[i6], deriv[1+i6], deriv[2+i6]);
               MessageInterface::ShowMessage("   Density:    %16.9le\n",
                  density[i]);
            }
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
//------------------------------------------------------------------------------
std::string DragForce::GetParameterText(const Integer id) const
{
   if (id >= PhysicalModelParamCount && id < DragForceParamCount)
      return PARAMETER_TEXT[id - PhysicalModelParamCount];
   else
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
//------------------------------------------------------------------------------
Integer DragForce::GetParameterID(const std::string &str) const
{
   for (int i = PhysicalModelParamCount; i < DragForceParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - PhysicalModelParamCount])
         return i;
   }
   
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
//------------------------------------------------------------------------------
Gmat::ParameterType DragForce::GetParameterType(const Integer id) const
{
   if (id >= PhysicalModelParamCount && id < DragForceParamCount)
      return PARAMETER_TYPE[id - PhysicalModelParamCount];
   else
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
//------------------------------------------------------------------------------
std::string DragForce::GetParameterTypeString(const Integer id) const
{
   if (id >= PhysicalModelParamCount && id < DragForceParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatBase::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
bool DragForce::IsParameterReadOnly(const Integer id) const
{
   if (id == FLUX || id == AVERAGE_FLUX || id == MAGNETIC_INDEX)
   {
      if (atmosphereType == "Exponential")
         return true;
      else
         return false;
   }
   
   if (id == ATMOSPHERE_BODY || SOURCE_TYPE)
      return true;
   
   return PhysicalModel::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
bool DragForce::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
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
//------------------------------------------------------------------------------
Real DragForce::GetRealParameter(const Integer id) const
{
   if (id == FLUX)
      return fluxF107;
    
   if (id == AVERAGE_FLUX)
      return fluxF107A;
    
   if (id == MAGNETIC_INDEX)
      return kp;

   return PhysicalModel::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real DragForce::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
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
//------------------------------------------------------------------------------
Real DragForce::SetRealParameter(const Integer id, const Real value)
{
//   ForceModelException fme;
   if (id == FLUX)
   {
      if (value >= 0.0)
         fluxF107 = value;
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ForceModelException(
            "The value of \"" + buffer.str() + "\" for field \"F107(Solar Flux)\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Real Number >= 0.0]. ");
      }
      return fluxF107;
//      if ((value < 0.0) || (value > 500.0))
//         throw ForceModelException(
//            "The solar flux (F10.7) must be between 0 and 500, and is usually "
//            "between 50 and 400");
//      if ((value < 50.0) || (value > 400.0))
//         MessageInterface::ShowMessage(
//            "Warning: The solar flux (F10.7) usually falls "
//            "between 50 and 400\n");
//
//      fluxF107 = value;
//      return fluxF107;
   }
    
   if (id == AVERAGE_FLUX)
   {
      if (value >= 0.0)
         fluxF107A = value;
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ForceModelException(
            "The value of \"" + buffer.str() + "\" for field \"F107A(Average Solar Flux)\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Real Number >= 0.0]. ");
      }
      return fluxF107A;
//      if ((value < 0.0) || (value > 500.0))
//         throw ForceModelException(
//            "The average solar flux (F10.7A) must be between 0 and 500, and is "
//            "usually between 50 and 400");
//      if ((value < 50.0) || (value > 400.0))
//         MessageInterface::ShowMessage(
//            "Warning: The average solar flux (F10.7A) usually falls "
//            "between 50 and 400\n");
//
//      fluxF107A = value;
//      return fluxF107A;
   }
    
   if (id == MAGNETIC_INDEX)
   {
      if ((value >= 0.0) && (value <= 9.0))
      {
         kp = value;
         ap = CalculateAp(kp);
      }
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ForceModelException(
            "The value of \"" + buffer.str() + "\" for field \"Magnetic Index\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Real Number >= 0.0]. ");
      }
      return kp;
//      if ((value < 0.0) || (value > 9.0))
//         throw ForceModelException(
//            "The magnetic index (Kp) must be between 0 and 9");
//
//      kp = value;
//      ap = CalculateAp(kp);
//
//      return kp;
   }

   return PhysicalModel::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, Real value)
//------------------------------------------------------------------------------
Real DragForce::SetRealParameter(const std::string &label, Real value)
{
   return SetRealParameter(GetParameterID(label), value);
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
//------------------------------------------------------------------------------
std::string DragForce::GetStringParameter(const Integer id) const
{
   if (id == ATMOSPHERE_MODEL) 
      return atmosphereType;

   if (id == ATMOSPHERE_BODY)
      return bodyName;
   
   if (id == SOURCE_TYPE)
      return dataType;
    
   if (id == FLUX_FILE)
      return fluxFile;
    
   return PhysicalModel::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string DragForce::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
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
#ifdef DEBUG_DRAGFORCE_PARAM
   MessageInterface::ShowMessage
      ("DragForce::SetStringParameter() id=%d, value=%s\n", id, value.c_str());
#endif
   
   if (id == ATMOSPHERE_MODEL)
   {
      atmosphereType = value;

      if ((value == "") || (value == "BodyDefault"))
         useExternalAtmosphere = true;
      else
      {
         if (!useExternalAtmosphere)
            delete atmos;
         atmos = NULL;
         useExternalAtmosphere = false;
      }

      return true;
   }
   
   if (id == ATMOSPHERE_BODY)
   {
      if (value == "")
         return false;
      
      // Drag currently requires that the drag body be the Earth.  When other
      // drag models are implemented, remove this block and test.
      if (value != "Earth")
         throw ForceModelException(
            "Drag models only function at the Earth in this build of GMAT.");
      bodyName = value;
      return true;
   }
      
    
   if (id == SOURCE_TYPE)      // "File" or "Constant" for now
   {
      if ((value != "File") && (value != "Constant"))
         return false;
      dataType = value;
      return true;
   }
    
   if (id == FLUX_FILE)
   {
      fluxFile = value;
      if (!internalAtmos)
         throw ForceModelException(
            "Cannot set flux file: Atmosphere Model undefined");
         
      internalAtmos->SetSolarFluxFile(fluxFile);
      internalAtmos->SetNewFileFlag(true);
      return true;
   }
    
   return PhysicalModel::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool DragForce::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
#ifdef DEBUG_DRAGFORCE_PARAM
   MessageInterface::ShowMessage(
      "DragForce::SetStringParameter() label=%s, value=%s\n", label.c_str(),
      value.c_str());
#endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// void SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                              const std::string &name)
//------------------------------------------------------------------------------
/**
 * Accessor used to set the AtmosphereModel.
 * 
 * @param obj The model.
 * @param type Type of object passed in.
 * @param name Name of the object.
 */
bool DragForce::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name)
{
   if (type == Gmat::ATMOSPHERE)
   {
      if (obj->GetType() != Gmat::ATMOSPHERE)
         throw ForceModelException("DragForce::SetRefObject: AtmosphereModel "
                                   "type set incorrectly.");
      SetInternalAtmosphereModel((AtmosphereModel*)obj);
      return true;
   }
   return PhysicalModel::SetRefObject(obj, type, name);
}
   

//------------------------------------------------------------------------------
// bool DragForce::SetInternalAtmosphereModel(AtmosphereModel* atm)
//------------------------------------------------------------------------------
/**
 * Sets the internal atmosphere model for the DragForce.
 *
 * @param atm AtmosphereModel to use when useExternalAtmosphere is false.
 *
 * @return flag indicating success of the operation.
 */
//------------------------------------------------------------------------------
bool DragForce::SetInternalAtmosphereModel(AtmosphereModel* atm)
{
   if (internalAtmos)
      delete internalAtmos;

   internalAtmos = atm;
   return true;
}

//------------------------------------------------------------------------------
// AtmosphereModel* DragForce::GetInternalAtmosphereModel()
//------------------------------------------------------------------------------
/**
 * Gets the internal atmosphere model for the DragForce.
 *
 * @return pointer to the internal atmosphere model used by the DragForce
 *         object when useExternalAtmosphere is set to false.
 */
//------------------------------------------------------------------------------
AtmosphereModel* DragForce::GetInternalAtmosphereModel()
{
   return internalAtmos;
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
//------------------------------------------------------------------------------
void DragForce::GetDensity(Real *state, Real when)
{
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Entered DragForce::GetDensity()\n";
   #endif

   // Give it a default value if the atmosphere model is not set
   if (!atmos)
   {
      for (Integer i = 0; i < satCount; ++i)
         density[i] = 4.0e-13;
   }
   else
   {
      if (atmos)
      {
         if (sun && centralBody)
         {
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
         if (fluxFile == "")
         {
            atmos->SetRealParameter(atmos->GetParameterID("F107"), fluxF107);
            atmos->SetRealParameter(atmos->GetParameterID("F107A"),
                                    fluxF107A);
            atmos->SetRealParameter(atmos->GetParameterID("MagneticIndex"), kp);
         }
      }

      #ifdef DEBUG_DRAGFORCE_DENSITY
         dragdata << "Calling atmos->Density() on " << atmos->GetTypeName()
                  << "\n";
      #endif
      atmos->Density(state, density, when, satCount);
      #ifdef DEBUG_DRAGFORCE_DENSITY
         dragdata << "Returned from atmos->Density()\n";
      #endif
        
      #ifdef DEBUG_DRAGFORCE_DENSITY
         for (Integer m = 0; m < satCount; ++m)
            dragdata << "   Epoch: " << when
                     << "   State: "
                     << state[m*6] << "  "
                     << state[m*6+1] << "  "
                     << state[m*6+2] << "    Density: "
                     << density[m] << "\n";
      #endif
   }
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Leaving DragForce::GetDensity()\n";
   #endif
}


//------------------------------------------------------------------------------
// Real CalculateAp(Real kp)
//------------------------------------------------------------------------------
/**
 * Converts Kp values to Ap values using Vallado, eq 8-31.
 *
 * This routine uses Valaldo 8-31:
 *
 *    \f[a_p = \exp\left(\frac{k_p + 1.6} {1.75}\right)\f]
 *
 * to convert Kp values to Ap values.
 *
 * @param <kp> The planetary index that is converted.
 *
 * @return The corresponding (approximate) planetary amplitude, Ap.
 */
//------------------------------------------------------------------------------
Real DragForce::CalculateAp(Real kp)
{
   return exp((kp + 1.6) / 1.75);
}
