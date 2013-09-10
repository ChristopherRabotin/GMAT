//$Id$
//------------------------------------------------------------------------------
//                                DragForce
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "ODEModelException.hpp"
#include "MessageInterface.hpp"
#include "CoordinateSystem.hpp"
#include "TimeTypes.hpp"

#include <sstream>                 // for <<
#include <cmath>

// Uncomment to generate drag model data for debugging:
//#define DEBUG_DRAGFORCE_DENSITY
//#define DEBUG_DRAGFORCE_PARAM
//#define DEBUG_DRAGFORCE_REFOBJ
//#define DEBUG_ANGVEL
//#define DEBUG_FIRST_CALL

//#define DUMP_DERIVATIVE
//#define DUMP_DENSITY
//#define DUMP_INITIAL_STATE_DERIVATIVES_ONLY


//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif


#ifdef DEBUG_DRAGFORCE_DENSITY
   #include <fstream>
   
   std::ofstream dragdata;
#endif

#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
#endif



//---------------------------------
// static data
//---------------------------------
const std::string
DragForce::PARAMETER_TEXT[DragForceParamCount - PhysicalModelParamCount] =
{
   "AtmosphereModel",               // ATMOSPHERE_MODEL
   "AtmosphereBody",                // ATMOSPHERE_BODY
   "InputSource",                   // SOURCE_TYPE
   "SolarFluxFile",                 // FLUX_FILE
   "F107",                          // FLUX
   "F107A",                         // AVERAGE_FLUX
   "MagneticIndex",                 // MAGNETIC_INDEX
   "FixedCoordinateSystem",         // FIXED_COORD_SYSTEM  (Read-only parameter)
   "AngularMomentumUpdateInterval", // W_UPDATE_INTERVAL (in days, Read-only)
   "KpToApMethod",                  // KP2AP_METHOD (Read-only)
};

const Gmat::ParameterType
DragForce::PARAMETER_TYPE[DragForceParamCount - PhysicalModelParamCount] =
{
   Gmat::OBJECT_TYPE,   // "AtmosphereModel"
   Gmat::STRING_TYPE,   // "AtmosphereBody",
   Gmat::STRING_TYPE,   // "InputSource", ("File" or "Constant" for now)
   Gmat::STRING_TYPE,   // "SolarFluxFile",
   Gmat::REAL_TYPE,     // "F107",
   Gmat::REAL_TYPE,     // "F107A",
   Gmat::REAL_TYPE,     // "MagneticIndex",
   Gmat::STRING_TYPE,   // "FixedCoordinateSystem"
   Gmat::REAL_TYPE,     // "AngularMomentumUpdateInterval"
   Gmat::INTEGER_TYPE,  // "KpToApMethod"
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
   PhysicalModel           (Gmat::PHYSICAL_MODEL, "DragForce", name),
   sun                     (NULL),
   centralBody             (NULL),
   angVel                  (NULL),
   useExternalAtmosphere   (true),
   atmosphereType          (""),
   atmos                   (NULL),
   internalAtmos           (NULL),
   density                 (NULL),
   prefactor               (NULL),
   firedOnce               (false),
   hasWindModel            (false),
   satCount                (1),
   orbitDimension          (0),
   dragState               (NULL),
   wUpdateInterval         (0.02),      // 0.02 = 28.8 minutes
   wUpdateEpoch            (-1.0),              // Force update if not set to fixed w
   massID                  (-1),
   cdID                    (-1),
   areaID                  (-1),
   F107ID                  (-1),
   F107AID                 (-1),
   KPID                    (-1),
   //bodyName               ("Earth"),
   dataType                ("Constant"),
   fluxFile                (""),
   fluxF107                (150.0),
   fluxF107A               (150.0),
   kp                      (3.0),
   cartIndex               (0),
   fillCartesian           (false),
   cbFixed                 (NULL),
   internalCoordSystem     (NULL),
   kpApConversion          (0)
{
   dimension = 6;
   parameterCount = DragForceParamCount;
   objectTypeNames.push_back("DragForce");

   
   // Default Sun location, from the SLP file at MJD GmatTimeConstants::MJD_OF_J2000:
   sunLoc[0] =  2.65e+07;
   sunLoc[1] = -1.32757e+08;
   sunLoc[2] = -5.75566e+07;
   
   cbLoc[0]  = 0.0;
   cbLoc[1]  = 0.0;
   cbLoc[2]  = 0.0;
   
   density = new Real[1];

//   // Nominal Earth angular velocity
//   angVel[0]      = 0.0;
//   angVel[1]      = 0.0;
//   angVel[2]      = 7.29211585530e-5;
   
   ap = CalculateAp(kp);
   
   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   
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
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (internalAtmos, "internalAtmos", "DragForce::~DragForce()",
          "deleting internal atmosphere model", this);
      #endif
      delete internalAtmos;
      internalAtmos = NULL;
   }
   
   if (density)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (density, "density", "DragForce::Initialize()",
          "deleting density[satCount]", this);
      #endif
      delete [] density;
   }
   
   if (dragState)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (dragState, "dragState", "DragForce::Initialize()",
          "deleting dragState[orbitDimension]", this);
      #endif
      delete [] dragState;
   }
   
   if (prefactor)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (prefactor, "prefactor", "DragForce::Initialize()",
          "deleting prefactor[satCount]", this);
      #endif
      delete [] prefactor;
   }
   
//   if (cbFixed)
//      delete cbFixed;

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
   angVel                  (NULL),
   useExternalAtmosphere   (df.useExternalAtmosphere),
   atmosphereType          (df.atmosphereType),
   atmos                   (NULL),
   density                 (NULL),
   prefactor               (NULL),
   firedOnce               (false),
   hasWindModel            (df.hasWindModel),
   satCount                (df.satCount),
   dragBody                (df.dragBody),
   dragState               (NULL),
   wUpdateInterval         (df.wUpdateInterval),
   wUpdateEpoch            (df.wUpdateEpoch),
   massID                  (df.massID),
   cdID                    (df.cdID),
   areaID                  (df.areaID),
   //bodyName                (df.bodyName),
   dataType                (df.dataType),
   fluxFile                (df.fluxFile),
   fluxF107                (df.fluxF107),
   fluxF107A               (df.fluxF107A),
   kp                      (df.kp),
   cartIndex               (df.cartIndex),
   fillCartesian           (df.fillCartesian),
   cbFixed                 (NULL),
   internalCoordSystem     (NULL),
   kpApConversion          (df.kpApConversion)
{
   internalAtmos = NULL;
   if (df.internalAtmos)
   {
      internalAtmos = (AtmosphereModel*)df.internalAtmos->Clone();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (internalAtmos, internalAtmos->GetName(), "DragForce::DragForce(copy)",
          "internalAtmos = (AtmosphereModel*)df.internalAtmos->Clone()", this);
      #endif
   }
   
   if (df.cbFixed)
//      cbFixed = (CoordinateSystem*)(df.cbFixed->Clone());      // Any other initialization needed?
      cbFixed = (CoordinateSystem*)(df.cbFixed);

   parameterCount += 7;
   dimension = df.dimension;
   orbitDimension = df.orbitDimension;

   sunLoc[0] = df.sunLoc[0];
   sunLoc[1] = df.sunLoc[1];
   sunLoc[2] = df.sunLoc[2];
   
   cbLoc[0]  = df.cbLoc[0];
   cbLoc[1]  = df.cbLoc[1];
   cbLoc[2]  = df.cbLoc[2];
   
   density = new Real[1];
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
   
   massID                = df.massID;
   cdID                  = df.cdID;
   areaID                = df.areaID;
   
   if (internalAtmos != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (internalAtmos, "internalAtmos", "DragForce::operator=()",
          "deleting internal atmosphere model");
      #endif
      delete internalAtmos;
   }
   
   internalAtmos = NULL;
   
   if (df.internalAtmos)
   {
      internalAtmos = (AtmosphereModel*)df.internalAtmos->Clone();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (internalAtmos, internalAtmos->GetName(), "DragForce::operator=()",
          "internalAtmos = (AtmosphereModel*)df.internalAtmos->Clone()", this);
      #endif
   }
   
   if (df.cbFixed)
   {
//      if (cbFixed)
//         delete cbFixed;
//      cbFixed = (CoordinateSystem*)(df.cbFixed->Clone());
      cbFixed = (CoordinateSystem*)(df.cbFixed);
   }

   atmos                 = NULL;
   angVel                = NULL;
//   density               = NULL;
   density = new Real[1];
   prefactor             = NULL;
   firedOnce             = false;
   hasWindModel          = df.hasWindModel;
   //bodyName              = df.bodyName;
   satCount              = df.satCount;
   dragBody              = df.dragBody;
   dragState             = df.dragState;
   wUpdateInterval       = df.wUpdateInterval;
   wUpdateEpoch          = df.wUpdateEpoch;
   dataType              = df.dataType;
   fluxFile              = df.fluxFile;
   fluxF107              = df.fluxF107;
   fluxF107A             = df.fluxF107A;
   kpApConversion        = df.kpApConversion;
   kp                    = df.kp;
   ap                    = CalculateAp(kp);
   
   dimension = df.dimension;
   orbitDimension = df. orbitDimension;

   sunLoc[0] = df.sunLoc[0];
   sunLoc[1] = df.sunLoc[1];
   sunLoc[2] = df.sunLoc[2];
   
   cbLoc[0]  = df.cbLoc[0];
   cbLoc[1]  = df.cbLoc[1];
   cbLoc[2]  = df.cbLoc[2];

   ap = CalculateAp(kp);
   
   area.clear();
   mass.clear();
   dragCoeff.clear();
   
   cartIndex = df.cartIndex;
   fillCartesian = df.fillCartesian;

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
      throw ODEModelException(
         "Drag supports 1st order equations of motion only");

   // Calculate how many spacecraft are in the model
   for (Integer i = 0; i < satCount; i++)
   {
      i6 = i * 6 + cartIndex;

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
                                      const Real parm,
                                      const Integer parmID)
{
   unsigned parmNumber = (unsigned)(i+1);

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Setting satellite parameter " << parmName
               << " for Spacecraft " << i << " to " << parm << "\n";
   #endif
    
   if (parmName == "Mass")
   {
      if (parmNumber < mass.size())
         mass[i] = parm;
      else
         mass.push_back(parm);
      if (parmID >= 0)
         massID = parmID;
   }
   if (parmName == "Cd")
   {
      if (parmNumber < dragCoeff.size())
         dragCoeff[i] = parm;
      else
         dragCoeff.push_back(parm);
      if (parmID >= 0)
         cdID = parmID;
   }
   if (parmName == "DragArea")
   {
      if (parmNumber < area.size())
         area[i] = parm;
      else
         area.push_back(parm);
      if (parmID >= 0)
         areaID = parmID;
   }
}


//------------------------------------------------------------------------------
// void SetSatelliteParameter(const Integer i, const Integer parmID,
//                            const Real parm)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft parameters to the force model.
 *
 * For drag modeling, this method is used to set or update C_d, area, and mass.
 *
 * @param i ID for the spacecraft
 * @param parmID id of the Spacecraft parameter
 * @param parm Parameter value
 */
//------------------------------------------------------------------------------
void DragForce::SetSatelliteParameter(const Integer i,
                                      const Integer parmID,
                                      const Real parm)
{
   unsigned parmNumber = (unsigned)(i+1);

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Setting satellite parameter ID " << parmID
               << " for Spacecraft " << i << " to " << parm << "\n";
   #endif

   if (parmID == massID)
   {
      if (parmNumber < mass.size())
         mass[i] = parm;
      else
         mass.push_back(parm);
   }
   if (parmID == cdID)
   {
      if (parmNumber < dragCoeff.size())
         dragCoeff[i] = parm;
      else
         dragCoeff.push_back(parm);
   }
   if (parmID == areaID)
   {
      if (parmNumber < area.size())
         area[i] = parm;
      else
         area.push_back(parm);
   }
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
      const std::string parmName, const std::string parm)
{
   unsigned parmNumber = (unsigned)(i+1);

   if (parmName == "ReferenceBody")
   {
      if (parmNumber < mass.size())
         dragBody[i] = parm;
      else
         dragBody.push_back(parm);
   }
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
   if ((parmName == "Mass") || (parmName == ""))
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
      orbitDimension = 6 * satCount;
      
      if (dragState)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (dragState, "dragState", "DragForce::Initialize()",
             "deleting dragState[orbitDimension]", this);
         #endif
         delete [] dragState;
      }
      
      dragState = new Real[orbitDimension];
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (dragState, "dragState", "DragForce::Initialize()",
          "dragState = new Real[orbitDimension]", this);
      #endif
      
      if (satCount <= 0)
         throw ODEModelException("Drag called with orbit dimension zero");

      if (density)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (density, "density", "DragForce::Initialize()",
             "deleting density[satCount]", this);
         #endif
         delete [] density;
      }
      
      if (prefactor)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (prefactor, "prefactor", "DragForce::Initialize()",
             "deleting prefactor[satCount]", this);
         #endif
         delete [] prefactor;
      }
      
      // Set up density even if only used for a parameter
      if (satCount > 0)
         density   = new Real[satCount];
      else
         density   = new Real[1];
      prefactor = new Real[satCount];
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (density, "density", "DragForce::Initialize()",
          "density = new Real[satCount]", this);
      MemoryTracker::Instance()->Add
         (prefactor, "prefactor", "DragForce::Initialize()",
          "prefactor = new Real[satCount]", this);
      #endif
      
      // Set the atmosphere model
      if (solarSystem)
      {
         sun = solarSystem->GetBody(SolarSystem::SUN_NAME);
         if (!sun)
            throw ODEModelException("The Sun is not in solar system");
           
         std::string bodyName;

         // Drag currently requires that the drag body be the Earth.  When other
         // drag models are implemented, remove this block and test.
         for (StringArray::iterator i = dragBody.begin(); i != dragBody.end(); 
              ++i)
         {
            if ((*i) != "Earth" && (*i) != "Mars")
               throw ODEModelException(
                  "Drag modeling only works at the Earth or Mars in current "
                  "GMAT builds.");
         }
         
         if (dragBody.size() > 0)
            bodyName = dragBody[0];
         else
            bodyName = "Earth";
         centralBody = solarSystem->GetBody(bodyName);
   
         if (!centralBody)
            throw ODEModelException(
               "Central body (for Drag) not in solar system");
         if (useExternalAtmosphere)
         {
            atmos = centralBody->GetAtmosphereModel();
         }
         else
         {
            std::string modelBodyIsUsing =
               centralBody->GetAtmosphereModelType();
            
            // Density from the body
            if (modelBodyIsUsing == "Undefined")
            {
               #ifdef DEBUG_DRAGFORCE_DENSITY
               MessageInterface::ShowMessage
                  ("   Setting atmosphereType<%p><%s>'%s' to body '%s'\n",
                   internalAtmos, internalAtmos ? internalAtmos->GetTypeName().c_str() :
                   "NULL", internalAtmos ? internalAtmos->GetName().c_str() : "NULL",
                   centralBody->GetName().c_str());
               #endif
               
               if (internalAtmos)
               {
                  AtmosphereModel *amCloned =
                        (AtmosphereModel*)internalAtmos->Clone();
                  #ifdef DEBUG_MEMORY
                  MemoryTracker::Instance()->Add
                     (amCloned, amCloned->GetName(), "DragForce::Initialize()",
                      "amCloned = (AtmosphereModel*)internalAtmos->Clone()", this);
                  #endif
                  
                  centralBody->SetAtmosphereModelType(atmosphereType);
                  centralBody->SetAtmosphereModel(amCloned);
               }
            }
            
            if ((atmosphereType == "BodyDefault") ||
                (atmosphereType == modelBodyIsUsing))
               atmos = centralBody->GetAtmosphereModel();
            else
               atmos = internalAtmos;
            if (!atmos)
               throw ODEModelException("Atmosphere model not defined");
            
         }
         
         if (atmos)
         {
            atmos->SetSunVector(sunLoc);
            atmos->SetCentralBodyVector(cbLoc);
            atmos->SetCentralBody(centralBody);
            atmos->SetSolarSystem(solarSystem);
            atmos->SetUpdateParameters(wUpdateInterval, wUpdateEpoch);
            atmos->SetKpApConversionMethod(kpApConversion);

            angVel = atmos->GetAngularVelocity();
            hasWindModel = atmos->HasWindModel();

            F107ID = atmos->GetParameterID("F107");
            F107AID = atmos->GetParameterID("F107A");
            KPID = atmos->GetParameterID("MagneticIndex");

            if (fluxFile == "")
            {
               if (F107ID < 0)
                  throw ODEModelException("Atmosphere model initialization is "
                        "incomplete");
               atmos->SetRealParameter(F107ID, fluxF107);
               atmos->SetRealParameter(F107AID, fluxF107A);
               atmos->SetRealParameter(KPID, kp);
            }

			if (cbFixed != NULL)										// made changes by TUAN NGUYEN
               atmos->SetFixedCoordinateSystem(cbFixed);				// made changes by TUAN NGUYEN
            if (internalCoordSystem != NULL)							// made changes by TUAN NGUYEN
               atmos->SetInternalCoordSystem(internalCoordSystem);		// made changes by TUAN NGUYEN
			atmos->Initialize();										// made changes by TUAN NGUYEN		Note: it needs to initialize before use. Fixed bug GMT-4124
         }
         else
         {
            if (atmosphereType != "BodyDefault")
            {
               std::string msg = "Could not create ";
               msg += atmosphereType;
               msg += " atmosphere model";
               throw ODEModelException(msg);
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
      throw ODEModelException(
         "Cannot use drag force: force model origin not set.");
    
   for (Integer i = 0; i < satCount; ++i)
   {
      if (mass.size() < (unsigned)(i+1))
         throw ODEModelException("Spacecraft not set correctly");
      if (mass[i] <= 0.0)
      {
         std::string errorMsg = "Spacecraft ";
         errorMsg += i;
         errorMsg += " has non-physical mass; Drag modeling cannot be used.";
         throw ODEModelException(errorMsg);
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
   memcpy(dragState, &(state[cartIndex]),  orbitDimension * sizeof(Real));
   if (forceOrigin != centralBody)
   {
      throw ODEModelException(
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
bool DragForce::GetDerivatives(Real *state, Real dt, Integer order, 
      const Integer id)
{
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Entered DragForce::GetDerivatives()\n";
   #endif

   Integer i, i6, j6;
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
                  throw ODEModelException(
                     "Cannot use drag force: force model origin not set.");
    
               #ifdef DEBUG_DRAGFORCE_DENSITY
                   dragdata << "Using default prefactors for " << satCount
                            << " Spacecraft\n";
               #endif
               prefactor[i] = -0.5 * 2.2 * 15.0 / 875.0; // Dummy up the product
            }
               
      firedOnce = true;
   }
    
   // First translate to the drag body from the force model origin
   Real now = epoch + (elapsedTime + dt) / GmatTimeConstants::SECS_PER_DAY;
   TranslateOrigin(state, now);

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "density[0] = " << density[0] << "\n";
   #endif

   #ifdef DEBUG_DRAGFORCE_EPOCH
      MessageInterface::ShowMessage("Drag epoch = %16.11lf\n", now);
   
   #endif

   #ifdef DEBUG_ANGVEL
      MessageInterface::ShowMessage("Ang Vel Vector: [%.12le %.12le "
            "%.12le]\n", angVel[0], angVel[1], angVel[2]);
   #endif

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Looking up density\n";
   #endif
   GetDensity(dragState, now);
   Real wind[6];

   #ifdef DUMP_DENSITY
      bool writeDensity = true;

      #ifdef DUMP_INITIAL_STATE_DERIVATIVES_ONLY
         if (dt != 0.0)
            writeDensity = false;
      #endif

      if (writeDensity)
      {
         MessageInterface::ShowMessage("Density:  %.12lf", now);

         for (Integer m = 0; m < satCount; ++m)
            MessageInterface::ShowMessage("  %.15le", density[m]);
         MessageInterface::ShowMessage("\n");
      }
   #endif

   if (fillCartesian)
   {
   
      for (i = 0; i < satCount; ++i)
      {
         i6 = i * 6;
         j6 = cartIndex + i * 6;
         #ifdef DEBUG_DRAGFORCE_DENSITY
            dragdata << "Spacecraft " << (i+1) << ": ";
         #endif
   
         
         if (hasWindModel)
         {
            // v_rel = v - w x R
            atmos->Wind(&(dragState[i6]), wind, now, 1);
            vRelative[0] = dragState[i6+3] - wind[3];
            vRelative[1] = dragState[i6+4] - wind[4];
            vRelative[2] = dragState[i6+5] - wind[5];
            vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] +
                           vRelative[2]*vRelative[2]);

            #ifdef COMPAREAPPROACHES
               Real vRel[3];
               vRel[0] = dragState[i6+3] -
                              (angVel[1]*dragState[i6+2] - angVel[2]*dragState[i6+1]);
               vRel[1] = dragState[i6+4] -
                              (angVel[2]*dragState[ i6 ] - angVel[0]*dragState[i6+2]);
               vRel[2] = dragState[i6+5] -
                              (angVel[0]*dragState[i6+1] - angVel[1]*dragState[ i6 ]);
               MessageInterface::ShowMessage("v-Local wind: %lf %lf %lf\n",
                     dragState[i6+3] - wind[3], dragState[i6+4] - wind[4], dragState[i6+5] - wind[5]);
               MessageInterface::ShowMessage("VRel: %lf %lf %lf\n",
                     vRelative[0], vRelative[1], vRelative[2]);
            #endif
         }
         else
         {
            // v_rel = v - w x R
            vRelative[0] = dragState[i6+3] -
                           (angVel[1]*dragState[i6+2] - angVel[2]*dragState[i6+1]);
            vRelative[1] = dragState[i6+4] -
                           (angVel[2]*dragState[ i6 ] - angVel[0]*dragState[i6+2]);
            vRelative[2] = dragState[i6+5] -
                           (angVel[0]*dragState[i6+1] - angVel[1]*dragState[ i6 ]);
            vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] +
                           vRelative[2]*vRelative[2]);
         }

         factor = prefactor[i] * density[i];
   
         #ifdef DEBUG_DERIVATIVES_FOR_SPACECRAFT
            MessageInterface::ShowMessage("GD4P Drag: Prefactor = %.12lf, "
               "density = %.12le\n", prefactor[i], density[i]);
         #endif

         if (order == 1)
         {
            // Do dv/dt first, in case deriv = state
            deriv[3+j6] = factor * vRelMag * vRelative[0];// - a_indirect[0];
            deriv[4+j6] = factor * vRelMag * vRelative[1];// - a_indirect[1];
            deriv[5+j6] = factor * vRelMag * vRelative[2];// - a_indirect[2];

            // dr/dt = v term not built from drag force
            deriv[j6]   = 
            deriv[1+j6] = 
            deriv[2+j6] = 0.0;
   
            #ifdef DEBUG_DRAGFORCE_DENSITY
//               for (Integer m = 0; m < satCount; ++m)
//                  dragdata << "   Drag Accel: "
//                           << deriv[3+i6] << "  "
//                           << deriv[4+i6] << "  "
//                           << deriv[5+i6] << "\n";
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
            deriv[ j6 ] = factor * vRelMag * vRelative[0];// - a_indirect[0];
            deriv[1+j6] = factor * vRelMag * vRelative[1];// - a_indirect[1];
            deriv[2+j6] = factor * vRelMag * vRelative[2];// - a_indirect[2];
            deriv[3+j6] = 0.0;
            deriv[4+j6] = 0.0;
            deriv[5+j6] = 0.0;
   
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
   }

   //#ifdef DUMP_DERIVATIVE


   #ifdef DEBUG_SHOW_Force
      static int iter = 0;
      if (iter == 0)
         MessageInterface::ShowMessage("Drag dv: [%le %le %le]\n", deriv[3],
               deriv[4], deriv[5]);
      if (++iter == 16)
         iter = 0;
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// Rvector6 GetDerivativesForSpacecraft(Spacecraft* sc)
//------------------------------------------------------------------------------
/**
 * Retrieves the Cartesian state vector of derivatives w.r.t. time
 *
 * @param sc The spacecraft that holds the state vector
 *
 * @return The derivative vector
 */
//------------------------------------------------------------------------------
Rvector6 DragForce::GetDerivativesForSpacecraft(Spacecraft* sc)
{
   Rvector6 dv;

   if (atmos == NULL)
      Initialize();

   Real vRelative[3], vRelMag, factor, mass, cd, area;
   mass = sc->GetRealParameter("TotalMass");
   cd   = sc->GetRealParameter("Cd");
   area = sc->GetRealParameter("DragArea");

   // Note: Prefactor is scaled to account for density in kg / m^3
   Real prefactor = -500.0 * cd * area / mass;

   // First translate to the drag body from the force model origin
   Real *j2kState = sc->GetState().GetState();
   Real state[6];
   Real now = sc->GetEpoch();

   BuildModelState(now, state, j2kState);

   Real dens = 0.0;
   if (atmos != NULL)
   {
      atmos->Density(state, &dens, now, 1);
      if (angVel == NULL)
         angVel = atmos->GetAngularVelocity();
   }
   else
      throw ODEModelException("Atmospheric model is NULL in the DragForce");

   if (hasWindModel)
   {
      Real wind[6];

      // v_rel = v - w x R
      atmos->Wind(&(state[0]), wind, now, 1);
      vRelative[0] = state[3] - wind[3];
      vRelative[1] = state[4] - wind[4];
      vRelative[2] = state[5] - wind[5];
      vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] +
                     vRelative[2]*vRelative[2]);
   }
   else
   {
      // v_rel = v - w x R
      vRelative[0] = state[3] -
                     (angVel[1]*state[2] - angVel[2]*state[1]);
      vRelative[1] = state[4] -
                     (angVel[2]*state[0] - angVel[0]*state[2]);
      vRelative[2] = state[5] -
                     (angVel[0]*state[1] - angVel[1]*state[0]);
      vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] +
                     vRelative[2]*vRelative[2]);
   }

   factor = prefactor * dens;

   #ifdef DEBUG_DERIVATIVES_FOR_SPACECRAFT
      MessageInterface::ShowMessage("GD4S Drag: Prefactor = %.12lf, "
         "density = %.12le\n", prefactor, dens);
   #endif

   // Do dv/dt first, in case deriv = state
   dv[3] = factor * vRelMag * vRelative[0];// - a_indirect[0];
   dv[4] = factor * vRelMag * vRelative[1];// - a_indirect[1];
   dv[5] = factor * vRelMag * vRelative[2];// - a_indirect[2];

   // dr/dt = v term not built from drag force
   dv[0] =
   dv[1] =
   dv[2] = 0.0;

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage("Drag Accel: [%le %le %le]\n", dv[3],
               dv[4], dv[5]);
   #endif

   return dv;
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
   
   if (id == ATMOSPHERE_BODY || id == SOURCE_TYPE ||
       id == FIXED_COORD_SYSTEM || id == W_UPDATE_INTERVAL ||
       id == KP2AP_METHOD)
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

   if (id == W_UPDATE_INTERVAL)
      return wUpdateInterval;

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
//   ODEModelException fme;
   if (id == FLUX)
   {
      if (value >= 0.0)
      {
         fluxF107 = value;
         if (atmos)
         {
            if (F107ID < 0)
            {
               F107ID = atmos->GetParameterID("F107");
               F107AID = atmos->GetParameterID("F107A");
               KPID = atmos->GetParameterID("MagneticIndex");
            }
            atmos->SetRealParameter(F107ID, fluxF107);
         }
      }
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ODEModelException(
            "The value of \"" + buffer.str() + "\" for field \"F107(Solar Flux)\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Real Number >= 0.0]. ");
      }
      return fluxF107;
   }
    
   if (id == AVERAGE_FLUX)
   {
      if (value >= 0.0)
      {
         fluxF107A = value;
         if (atmos)
         {
            if (F107AID < 0)
            {
               F107ID = atmos->GetParameterID("F107");
               F107AID = atmos->GetParameterID("F107A");
               KPID = atmos->GetParameterID("MagneticIndex");
            }
            atmos->SetRealParameter(F107AID, fluxF107A);
         }
      }
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ODEModelException(
            "The value of \"" + buffer.str() + "\" for field \"F107A(Average Solar Flux)\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Real Number >= 0.0]. ");
      }
      return fluxF107A;
//      if ((value < 0.0) || (value > 500.0))
//         throw ODEModelException(
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
         if (atmos)
         {
            if (KPID < 0)
            {
               F107ID = atmos->GetParameterID("F107");
               F107AID = atmos->GetParameterID("F107A");
               KPID = atmos->GetParameterID("MagneticIndex");
            }
            atmos->SetRealParameter(KPID, kp);
         }
      }
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ODEModelException(
            "The value of \"" + buffer.str() + "\" for field \"Magnetic Index\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [0.0 <= Real Number <= 9.0]. ");
      }
      return kp;
//      if ((value < 0.0) || (value > 9.0))
//         throw ODEModelException(
//            "The magnetic index (Kp) must be between 0 and 9");
//
//      kp = value;
//      ap = CalculateAp(kp);
//
//      return kp;
   }

   if (id == W_UPDATE_INTERVAL)
   {
      wUpdateInterval = value;
      return wUpdateInterval;
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
    
   if (id == FIXED_COORD_SYSTEM)
      return bodyName + "Fixed";

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
//------------------------------------------------------------------------------
bool DragForce::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_DRAGFORCE_PARAM
   MessageInterface::ShowMessage
      ("DragForce::SetStringParameter() '%s' entered, id=%d, value=%s\n",
       GetName().c_str(), id, value.c_str());
   #endif
   
   if (id == ATMOSPHERE_MODEL)
   {
      atmosphereType = value;
      
      if ((value == "") || (value == "BodyDefault"))
         useExternalAtmosphere = true;
      else
      {
         if (!useExternalAtmosphere && atmos != NULL)
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (atmos, atmos->GetName(), "DragForce::SetStringParameter()",
                "deleting atmosphere model");
            #endif
            delete atmos;
         }
         atmos = NULL;
         useExternalAtmosphere = false;
      }
      
      return true;
   }
   
   if (id == ATMOSPHERE_BODY)
   {
      if (value == "")
         return false;

      // Added Mars for drag body. We added MarsGRAM atmosphere model.
      // Drag currently requires that the drag body be the Earth.  When other
      // drag models are implemented, remove this block and test.
      if (value != "Earth" && value != "Mars")
         throw ODEModelException(
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
         throw ODEModelException(
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


Integer DragForce::GetIntegerParameter(const Integer id) const
{
   if (id == KP2AP_METHOD)
      return kpApConversion;

   return PhysicalModel::GetIntegerParameter(id);
}

Integer DragForce::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == KP2AP_METHOD)
   {
      kpApConversion = value;
      if (atmos)
         atmos->SetKpApConversionMethod(kpApConversion);
      return kpApConversion;
   }

   return PhysicalModel::SetIntegerParameter(id, value);
}

Integer DragForce::GetIntegerParameter(const Integer id,
      const Integer index) const
{
   return PhysicalModel::GetIntegerParameter(id, index);
}

Integer DragForce::SetIntegerParameter(const Integer id, const Integer value,
      const Integer index)
{
   return PhysicalModel::SetIntegerParameter(id, value, index);
}

Integer DragForce::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

Integer DragForce::SetIntegerParameter(const std::string &label,
      const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}

Integer DragForce::GetIntegerParameter(const std::string &label,
      const Integer index) const
{
   return GetIntegerParameter(GetParameterID(label), index);
}

Integer DragForce::SetIntegerParameter(const std::string &label,
      const Integer value, const Integer index)
{
   return SetIntegerParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const Gmat::ObjectType type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the HarmonicField class.
 *
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* DragForce::GetRefObject(const Gmat::ObjectType type,
                                      const std::string &name)
{
   switch (type)
   {
      case Gmat::COORDINATE_SYSTEM:
         { // Set scope for local variable
            std::string fixedCSName = bodyName + "Fixed";
            if ((cbFixed) && (name == fixedCSName))
               return cbFixed;
         }
         break;

      default:
         break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return PhysicalModel::GetRefObject(type, name);
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
//------------------------------------------------------------------------------
bool DragForce::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name)
{
   #ifdef DEBUG_DRAGFORCE_REFOBJ
   MessageInterface::ShowMessage
      ("DragForce::SetRefObject() <%p>'%s' entered, obj=<%p>'%s', name='%s'\n",
       this, GetName().c_str(), obj, obj ? obj->GetName().c_str() : "NULL",
       name.c_str());
   #endif
   
   if (obj == NULL)
      return false;
   
   if (type == Gmat::ATMOSPHERE)
   {
      if (obj->GetType() != Gmat::ATMOSPHERE)
         throw ODEModelException("DragForce::SetRefObject: AtmosphereModel "
                                   "type set incorrectly.");      
      SetInternalAtmosphereModel((AtmosphereModel*)obj);
      return true;
   }

   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (obj->GetType() != Gmat::COORDINATE_SYSTEM)
         throw ODEModelException("DragForce::SetRefObject: Coordinate System "
                                 "type set incorrectly.");

      if (((CoordinateSystem*)(obj))->AreAxesOfType("BodyFixedAxes"))
      {
         cbFixed = (CoordinateSystem*)(obj);
         if (internalAtmos != NULL)
            internalAtmos->SetFixedCoordinateSystem(cbFixed);
      }
      else
      {
         internalCoordSystem = (CoordinateSystem*)(obj);
         if (internalAtmos != NULL)
            internalAtmos->SetInternalCoordSystem(internalCoordSystem);
      }

      return true;
   }

   return PhysicalModel::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// virtual Integer GetOwnedObjectCount()
//------------------------------------------------------------------------------
Integer DragForce::GetOwnedObjectCount()
{
   // for now only 1 internal object
   if (internalAtmos != NULL)
      return 1;
   else
      return 0;
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetOwnedObject(Integer whichOne)
//------------------------------------------------------------------------------
GmatBase* DragForce::GetOwnedObject(Integer whichOne)
{
   // for now only 1 internal object
   return internalAtmos;
}


//------------------------------------------------------------------------------
// bool SetInternalAtmosphereModel(AtmosphereModel* atm)
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
   #ifdef DEBUG_DRAGFORCE_REFOBJ
   MessageInterface::ShowMessage
      ("DragForce::SetInternalAtmosphereModel() entered, atm=<%p>, "
       "internalAtmos=<%p>\n", atm, internalAtmos);
   #endif
   
   if (atm == NULL)
   {
      #ifdef DEBUG_DRAGFORCE_REFOBJ
      MessageInterface::ShowMessage
         ("DragForce::SetInternalAtmosphereModel() returning false, atm is NULL\n");
      #endif
      return false;
   }
   
   if (internalAtmos)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (internalAtmos, "internalAtmos", "DragForce::SetInternalAtmosphereModel()",
          "deleting internal atmosphere model");
      #endif
      delete internalAtmos;
   }
   
   internalAtmos = atm;

   if (cbFixed != NULL)
      internalAtmos->SetFixedCoordinateSystem(cbFixed);
   if (internalCoordSystem != NULL)
      internalAtmos->SetInternalCoordSystem(internalCoordSystem);
   
   #ifdef DEBUG_DRAGFORCE_REFOBJ
   MessageInterface::ShowMessage
      ("DragForce::SetInternalAtmosphereModel() returning true, internalAtmos=<%p>\n",
       internalAtmos);
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// AtmosphereModel* GetInternalAtmosphereModel()
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
// bool SupportsDerivative(Gmat::StateElementId id)
//------------------------------------------------------------------------------
/**
 * Function used to determine if the physical model supports derivative 
 * information for a specified type.
 * 
 * @param id State Element ID for the derivative type
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool DragForce::SupportsDerivative(Gmat::StateElementId id)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage(
            "DragForce checking for support for id %d\n", id);
   #endif
      
   if (id == Gmat::CARTESIAN_STATE)
      return true;
   
   if (id == Gmat::ORBIT_STATE_TRANSITION_MATRIX)
   {
      MessageInterface::ShowMessage("Warning: The orbit state transition "
            "matrix does not currently contain drag contributions.\n");
      return false;
   }
   
   return PhysicalModel::SupportsDerivative(id);
}


//------------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//------------------------------------------------------------------------------
/**
 * Function used to set the start point and size information for the state 
 * vector, so that the derivative information can be placed in the correct place 
 * in the derivative vector.
 * 
 * @param id State Element ID for the derivative type
 * @param index Starting index in the state vector for this type of derivative
 * @param quantity Number of objects that supply this type of data
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool DragForce::SetStart(Gmat::StateElementId id, Integer index, 
                      Integer quantity)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage("DragForce setting start data for id = %d"
            " to index %d; %d objects identified\n", id, index, quantity);
   #endif
   
   bool retval = false;
   
   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
         satCount = quantity;
         cartIndex = index;
         fillCartesian = true;
         retval = true;
         break;
         
//      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
//         stmCount = quantity;
//         stmIndex = index;
//         fillSTM = true;
//         retval = true;
//         break;
         
      default:
         break;
   }
   
   return retval;
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
 *
 * @return The first computed density value
 */
//------------------------------------------------------------------------------
Real DragForce::GetDensity(Real *state, Real when, Integer count)
{
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Entered DragForce::GetDensity()\n";
   #endif

   if (count == -1)
      count = satCount;

   // Give it a default value if the atmosphere model is not set
   if (!atmos)
   {
      for (Integer i = 0; i < count; ++i)
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
      }

      #ifdef DEBUG_DRAGFORCE_DENSITY
         dragdata << "Calling atmos->Density() on " << atmos->GetTypeName()
                  << "\n";
      #endif
      atmos->Density(state, density, when, count);
      #ifdef DEBUG_DRAGFORCE_DENSITY
         dragdata << "Returned from atmos->Density()\n";
      #endif
        
      #ifdef DEBUG_DRAGFORCE_DENSITY
         for (Integer m = 0; m < count; ++m)
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
   return density[0];
}


//------------------------------------------------------------------------------
// bool IsUnique(const std::string &forBody)
//------------------------------------------------------------------------------
/**
 * Specifies if a force is the one and only force, optionally at a given body.
 *
 * @param forBody The body at which the force must be unique, if body specific
 *
 * @return true if the force should be unique in the ODEModel.
 */
//------------------------------------------------------------------------------
bool DragForce::IsUnique(const std::string& forBody)
{
   if (bodyName == forBody)
      return true;
   return false;
}

//------------------------------------------------------------------------------
// Real CalculateAp(Real kp)
//------------------------------------------------------------------------------
/**
 * Converts Kp values to Ap values.  If the atmosphere model is set, the
 * calculation is performed using the AtmosphereModel::ConvertKpToAp method.
 * If not, the value is calculated using Vallado (2nd Edition), eq 8-31:
 *
 *    \f[a_p = \exp\left(\frac{k_p + 1.6} {1.75}\right)\f]
 *
 * @param <kp> The planetary index that is converted.
 *
 * @return The corresponding (approximate) planetary amplitude, Ap.
 */
//------------------------------------------------------------------------------
Real DragForce::CalculateAp(Real kp)
{
   Real newAp = 0.0;

   if (atmos)
      newAp = atmos->ConvertKpToAp(kp);
   else
      newAp = exp((kp + 1.6) / 1.75);

   return newAp;
}

