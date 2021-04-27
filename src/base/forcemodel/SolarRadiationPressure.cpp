//$Id$
//------------------------------------------------------------------------------
//                              SolarRadiationPressure
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: D.Conway
// Created: 2003/03/14
// Modified: 2003/03/30 Waka Waktola GSFC Code 583
//           Updated for GMAT style, standards.
///
/**
 * This is the SolarRadiationPressure class.
 *
 * @note original prolog information included at end of file prolog.
 */
// *** File Name : SolarRadiationPressure.cpp
// *** Created   : February 27, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
// ***                                                                    ***
// ***  This software is subject to the Software Usage Agreement described***
// ***  by NASA Case Number GSC-14735-1.  The Software Usage Agreement    ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// **************************************************************************

#include "SolarRadiationPressure.hpp"
#include <sstream>                      // For stringstream
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "GmatDefaults.hpp"
#include "PropagationStateManager.hpp"

#define NPLATE_ANALYTICAL_SOLUTION                    // made changes by TUAN NGUYEN

//#define DEBUG_SRP_ORIGIN
//#define DEBUG_SOLAR_RADIATION_PRESSURE
//#define DEBUG_SOLAR_RADIATION_PRESSURE_TIMESHADOW
//#define DEBUG_A_MATRIX
//#define DEBUG_STM_MATRIX

//#define DEBUG_SPAD_DATA
//#define DEBUG_SPAD_ACCEL
//#define DEBUG_SHADOW_STATE
//#define DEBUG_SHADOW_STATE_2

//#define IGNORE_SHADOWS


//---------------------------------
// static data
//---------------------------------
const std::string
SolarRadiationPressure::PARAMETER_TEXT[SRPParamCount - PhysicalModelParamCount] =
{
    "UseAnalytic",
    "ShadowModel",
    "VectorModel",
    "BodyRadius",
    "SunRadius",
    "HasMoons",
    "CR",
    "Area",
    "Mass",
    "Flux",
    "Flux_Pressure",
    "SRPModel",
    "Sun_Distance",
    "Nominal_Sun",
    "PSunRad",
    "PCBrad",
    "PercentSun",
};

const Gmat::ParameterType
SolarRadiationPressure::PARAMETER_TYPE[SRPParamCount - PhysicalModelParamCount] =
{
   Gmat::BOOLEAN_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::ENUMERATION_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
};

const Real SolarRadiationPressure::FLUX_LOWER_BOUND          = 1200.0;
const Real SolarRadiationPressure::FLUX_UPPER_BOUND          = 1450.0;
const Real SolarRadiationPressure::FLUX_PRESSURE_LOWER_BOUND = 4.33e-6;
const Real SolarRadiationPressure::FLUX_PRESSURE_UPPER_BOUND = 4.84e-6;
const Real SolarRadiationPressure::NOMINAL_SUN_LOWER_BOUND   = 135.0e6;
const Real SolarRadiationPressure::NOMINAL_SUN_UPPER_BOUND   = 165.0e6;

           
//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// SolarRadiationPressure(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 * 
 * @param name Name of the force object
 */
//------------------------------------------------------------------------------
SolarRadiationPressure::SolarRadiationPressure(const std::string &name) :
   PhysicalModel       (Gmat::PHYSICAL_MODEL, "SolarRadiationPressure", name),   
   theSun              (NULL),
   shadowState         (NULL),
   useAnalytic         (true),
   shadowModel         (CONICAL_MODEL),
   vectorModel         (SUN_PARALLEL),
   bodyRadius          (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]),
   cbSunVector         (NULL),
   forceVector         (NULL),
   sunRadius           (GmatSolarSystemDefaults::STAR_EQUATORIAL_RADIUS),
   hasMoons            (false),
   flux                (1367.0),                  // W/m, IERS 1996
   fluxPressure        (flux / GmatPhysicalConstants::c),   // converted to N/m^2
   srpShapeModel       ("Spherical"),
   srpShapeModelIndex  (ShapeModel::SPHERICAL_MODEL),    // made changes by TUAN NGUYEN
   sunDistance         (149597870.691),
   nominalSun          (149597870.691),
   bodyIsTheSun        (false),
   psunrad             (0.0),
   pcbrad              (0.0),
   percentSun          (1.0),
   warnSRPMath         (true),
   bodyID              (-1),
   satCount            (0),
   massID              (-1),
   crID                (-1),
   areaID              (-1),
   //estimatingCr        (false),                       // made changes by TUAN NGUYEN
   crEpsilonID         (-1)//,                          // made changes by TUAN NGUYEN
   //crEpsilonRow        (-1)                           // made changes by TUAN NGUYEN
{
   parameterCount = SRPParamCount;
   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   objectTypeNames.push_back("SolarRadiationPressure");

   shadowState = new ShadowState();
   isConservative = false;
   hasMassJacobian = true;
}

//------------------------------------------------------------------------------
// SolarRadiationPressure(SolarRadiationPressure &srp)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 * 
 * @param srp The force copied to this one.
 */
//------------------------------------------------------------------------------
SolarRadiationPressure::SolarRadiationPressure(const SolarRadiationPressure &srp) :
   PhysicalModel       (srp),  
   theSun              (NULL),
   shadowState         (NULL),
   useAnalytic         (srp.useAnalytic),
   shadowModel         (srp.shadowModel),
   vectorModel         (srp.vectorModel),
   bodyRadius          (srp.bodyRadius),
   cbSunVector         (NULL),
   forceVector         (NULL),
   sunRadius           (srp.sunRadius),
   hasMoons            (srp.hasMoons),
   cr                  (srp.cr),
   area                (srp.area),
   mass                (srp.mass),
   flux                (srp.flux),           // W/m, IERS 1996
   fluxPressure        (srp.fluxPressure),   // converted to N/m^2
   srpShapeModel       (srp.srpShapeModel),
   srpShapeModelIndex  (srp.srpShapeModelIndex),         // made changes by TUAN NGUYEN
   sunDistance         (srp.sunDistance),
   nominalSun          (srp.nominalSun),
   bodyIsTheSun        (srp.bodyIsTheSun),
   psunrad             (srp.psunrad),
   pcbrad              (srp.pcbrad),
   percentSun          (srp.percentSun),
   warnSRPMath         (srp.warnSRPMath),
   bodyID              (srp.bodyID),
   satCount            (srp.satCount),
   massID              (srp.massID),
   crID                (srp.crID),
   areaID              (srp.areaID),
   //estimatingCr        (srp.estimatingCr),             // made changes by TUAN NGUYEN
   crEpsilonID         (srp.crEpsilonID),
   //crEpsilonRow        (srp.crEpsilonRow),
   crEpsilon           (srp.crEpsilon)              // made changes by TUAN NGUYEN
   //crInitial           (srp.crInitial)            // made changes by TUAN NGUYEN
{
   parameterCount = SRPParamCount;
   shadowState = new ShadowState();

   isInitialized = false;
}

//------------------------------------------------------------------------------
// SolarRadiationPressure& operator=(SolarRadiationPressure &srp)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param srp The force passing values to this one
 *
 * @return This SRP force, set to match the srp input model
 */
//------------------------------------------------------------------------------
SolarRadiationPressure& SolarRadiationPressure::operator=(const SolarRadiationPressure &srp)
{
   if (this != &srp)
   {
      PhysicalModel::operator=(srp);
      theSun       = NULL;
      useAnalytic  = srp.useAnalytic;
      shadowModel  = srp.shadowModel;
      vectorModel  = srp.vectorModel;
      bodyRadius   = srp.bodyRadius;
      cbSunVector  = NULL;
      forceVector  = NULL;
      sunRadius    = srp.sunRadius;
      hasMoons     = srp.hasMoons;
      cr           = srp.cr;
      area         = srp.area;
      mass         = srp.mass;
      //scObjs       = srp.scObjs;                              // made changes by TUAN NGUYEN
      flux         = srp.flux;           // W/m^2, IERS 1996
      fluxPressure = srp.fluxPressure;   // converted to N/m^2
      srpShapeModel = srp.srpShapeModel;
      srpShapeModelIndex = srp.srpShapeModelIndex;              // made changes by TUAN NGUYEN
      sunDistance  = srp.sunDistance;
      nominalSun   = srp.nominalSun;
      bodyIsTheSun = srp.bodyIsTheSun;
      psunrad      = srp.psunrad;
      pcbrad       = srp.pcbrad;
      percentSun   = srp.percentSun;
      warnSRPMath  = srp.warnSRPMath;
      bodyID       = srp.bodyID;
   
      satCount     = srp.satCount;
      massID       = srp.massID;
      crID         = srp.crID;
      areaID       = srp.areaID;

      //estimatingCr = srp.estimatingCr;                    // made changes by TUAN NGUYEN
      crEpsilonID  = srp.crEpsilonID;
      //crEpsilonRow = srp.crEpsilonRow;
      crEpsilon    = srp.crEpsilon;
      //crInitial    = srp.crInitial;                  // made changes by TUAN NGUYEN

      if (shadowState)
      {
         delete shadowState;
         shadowState        = NULL;
      }

      shadowState           = new ShadowState();

      isInitialized = false;
}
   
   return *this;
}

//------------------------------------------------------------------------------
// SolarRadiationPressure::~SolarRadiationPressure(void)
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SolarRadiationPressure::~SolarRadiationPressure()
{ 
    if ((useAnalytic) && (cbSunVector))
        delete [] cbSunVector;
    if (forceVector)
        delete [] forceVector;
    if (shadowState)
       delete shadowState;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the SolarRadiationPressure.
 *
 * @return clone of the SolarRadiationPressure.
 *
 */
//------------------------------------------------------------------------------
GmatBase* SolarRadiationPressure::Clone() const
{
   return (new SolarRadiationPressure(*this));
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string SolarRadiationPressure::GetParameterText(const Integer id) const
{
   if (id >= USE_ANALYTIC && id < SRPParamCount)
      return SolarRadiationPressure::PARAMETER_TEXT[id - PhysicalModelParamCount];
   else
      return PhysicalModel::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer SolarRadiationPressure::GetParameterID(const std::string &str) const
{
   for (int i = USE_ANALYTIC; i < SRPParamCount; i++)
   {
      if (str == SolarRadiationPressure::PARAMETER_TEXT[i - PhysicalModelParamCount])
         return i;
   }
   return PhysicalModel::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType SolarRadiationPressure::GetParameterType(const Integer id) const
{
   if (id >= USE_ANALYTIC && id < SRPParamCount)
      return PARAMETER_TYPE[id - PhysicalModelParamCount];
   else
      return PhysicalModel::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string SolarRadiationPressure::GetParameterTypeString(const Integer id) const
{
   if (id >= USE_ANALYTIC && id < SRPParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
    else
      return PhysicalModel::GetParameterTypeString(id);
}

// All read only for now
bool SolarRadiationPressure::IsParameterReadOnly(const Integer id) const
{
   if ((id == FLUX) || (id == NOMINAL_SUN) || (id == SRP_MODEL))
      return false;
   
   return true;
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real SolarRadiationPressure::GetRealParameter(const Integer id) const
{
   if (id == BODY_RADIUS)
      return bodyRadius;
   if (id == SUN_RADIUS)
      return sunRadius;
//   if (id == CR)
//      if (cr.size() > 0)
//         return cr[0];
//      else
//         throw ODEModelException(
//            "Attempting to access C_r for SRP force before any spacecraft "
//            "parameters were set.");
//   if (id == AREA)
//      if (area.size() > 0)
//         return area[0];
//      else
//         throw ODEModelException(
//            "Attempting to access area for SRP force before any spacecraft "
//            "parameters were set.");
//   if (id == MASS)
//      if (mass.size() > 0)
//         return mass[0];
//      else
//         throw ODEModelException(
//            "Attempting to access mass for SRP force before any spacecraft "
//            "parameters were set.");
   if (id == FLUX)
      return flux;
   if (id == FLUX_PRESSURE)
      return fluxPressure;
   if (id == SUN_DISTANCE)
      return sunDistance;
   if (id == NOMINAL_SUN)
      return nominalSun;
   if (id == PSUNRAD)
      return psunrad;
   if (id == PCBRAD)
      return pcbrad;
   if (id == PERCENT_SUN)
      return percentSun;

   return PhysicalModel::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real SolarRadiationPressure::SetRealParameter(const Integer id, const Real value)
{
   if (id == BODY_RADIUS)
   {
      bodyRadius = value;
      return bodyRadius;
   }
   if (id == SUN_RADIUS)
   {
      sunRadius = value;
      return sunRadius;
   }
   if (id == FLUX)
   {
      if ((value <= FLUX_LOWER_BOUND) || (value >= FLUX_UPPER_BOUND))
      {
         std::stringstream errmsg;
         errmsg << "*** Error *** The value of " << value
                << " for field " << GetParameterText(id)
                << " on object \"" << instanceName
                <<  "\" is not an allowed value.\n"
                << "The allowed values are: ["
                << FLUX_LOWER_BOUND << " < Real number < " << FLUX_UPPER_BOUND << "]";
         throw ODEModelException(errmsg.str());
      }
      flux = value;
      fluxPressure = flux / GmatPhysicalConstants::c;
      return flux;
   }
   if (id == FLUX_PRESSURE)
   {
      if ((value <= FLUX_PRESSURE_LOWER_BOUND) || (value >= FLUX_PRESSURE_UPPER_BOUND))
      {
         std::stringstream errmsg;
         errmsg << "*** Error *** The value of " << value
                << " for field " << GetParameterText(id)
                << " on object \"" << instanceName
                <<  "\" is not an allowed value.\n"
                << "The allowed values are: ["
                << FLUX_PRESSURE_LOWER_BOUND << " < Real number < " << FLUX_PRESSURE_UPPER_BOUND << "]";
         throw ODEModelException(errmsg.str());
      }
      fluxPressure = value;
      flux = fluxPressure * GmatPhysicalConstants::c;
      return fluxPressure;
   }
   if (id == SUN_DISTANCE)
   {
      sunDistance = value;
      return sunDistance;
   }
   if (id == NOMINAL_SUN)
   {
      if ((value <= NOMINAL_SUN_LOWER_BOUND) || (value >= NOMINAL_SUN_UPPER_BOUND))
      {
         std::stringstream errmsg;
         errmsg << "*** Error *** The value of " << value
                << " for field " << GetParameterText(id)
                << " on object \"" << instanceName
                <<  "\" is not an allowed value.\n"
                << "The allowed values are: ["
                << NOMINAL_SUN_LOWER_BOUND << " < Real number < " << NOMINAL_SUN_UPPER_BOUND << "]";
         throw ODEModelException(errmsg.str());
      }
      nominalSun = value;
      return nominalSun;
   }
   if (id == PSUNRAD)
   {
      psunrad = value;
      return psunrad;
   }
   if (id == PCBRAD)
   {
      pcbrad = value;
      return pcbrad;
   }
   if (id == PERCENT_SUN)
   {
      percentSun = value;
      return percentSun;
   }
       
   return PhysicalModel::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::GetBooleanParameter(const Integer id) const
{
    if (id == USE_ANALYTIC)
        return useAnalytic;
    if (id == HAS_MOONS)
        return hasMoons;

    return PhysicalModel::GetBooleanParameter(id);
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == USE_ANALYTIC)
   {
      useAnalytic = value;
      return useAnalytic;
   }
   if (id == HAS_MOONS)
   {
      hasMoons = value;
      return hasMoons;
   }
       
   return PhysicalModel::SetBooleanParameter(id, value);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer SolarRadiationPressure::GetIntegerParameter(const Integer id) const
{
   if (id == SHADOW_MODEL)
      return shadowModel;
   if (id == VECTOR_MODEL)
      return vectorModel;

   return PhysicalModel::GetIntegerParameter(id);
}

//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer SolarRadiationPressure::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == SHADOW_MODEL)
   {
      shadowModel = value;
      return shadowModel;
   }
   if (id == VECTOR_MODEL)
   {
      vectorModel = value;
      return vectorModel;
   }
       
   return PhysicalModel::SetIntegerParameter(id, value);
}

std::string SolarRadiationPressure::GetStringParameter(const Integer id) const
{
   if (id == SRP_MODEL)  return srpShapeModel;

   return PhysicalModel::GetStringParameter(id);
}

std::string SolarRadiationPressure::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

bool SolarRadiationPressure::SetStringParameter(const Integer id,
                                                const std::string &value)
{
   if (id == SRP_MODEL)
   {
      Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
      if ((value == "NPlate") && (runmode != GmatGlobal::TESTING && runmode != GmatGlobal::TESTING_NO_PLOTS))
      {
         ODEModelException odee("");
         odee.SetDetails(errorMessageFormat.c_str(),
                        value.c_str(),
                        "SrpModel", "\"Spherical\" or \"SPADFile\"");
         throw odee;
      }

      if ((value != "Spherical") && (value != "SPADFile") && (value != "NPlate"))         // made changes by TUAN NGUYEN
      {
         ODEModelException odee("");
         odee.SetDetails(errorMessageFormat.c_str(),
                        value.c_str(),
                        "SrpModel", "\"Spherical\", \"SPADFile\", or \"NPlate\"");       // made changes by TUAN NGUYEN
         throw odee;
      }
      srpShapeModel = value;

      // Set value for srpShapeModelIndex                                                // made changes by TUAN NGUYEN
      if (value == "Spherical")                                                          // made changes by TUAN NGUYEN
         srpShapeModelIndex = ShapeModel::SPHERICAL_MODEL;                               // made changes by TUAN NGUYEN
      else if (value == "SPADFile")                                                      // made changes by TUAN NGUYEN
         srpShapeModelIndex = ShapeModel::SPAD_FILE_MODEL;                               // made changes by TUAN NGUYEN
      else if (value == "NPlate")                                                        // made changes by TUAN NGUYEN
         srpShapeModelIndex = ShapeModel::NPLATE_MODEL;                                  // made changes by TUAN NGUYEN

      return true;
   }

   return PhysicalModel::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const char *value)
//------------------------------------------------------------------------------
bool SolarRadiationPressure::SetStringParameter(const std::string &label,
                                                const char *value)
{
   return SetStringParameter(GetParameterID(label), std::string(value));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool SolarRadiationPressure::SetStringParameter(const std::string &label,
                                                const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool SolarRadiationPressure::Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the SRP model for use
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::Initialize()
{
   bool retval = PhysicalModel::Initialize();

   #ifdef DEBUG_SOLAR_RADIATION_PRESSURE
      MessageInterface::ShowMessage("Flux = %.12le, Flux Pressure = %.12le, "
            "c = %16.3lf or %16.3lf\n", flux, fluxPressure,
            GmatPhysicalConstants::c, flux/fluxPressure);
   #endif

   if (!retval)
      return false;

   if (solarSystem) 
   {
      theSun    = solarSystem->GetBody(GmatSolarSystemDefaults::SUN_NAME);
      sunRadius = theSun->GetEquatorialRadius();
      
      if (!theSun)
         throw ODEModelException("Solar system does not contain the Sun for SRP force.");

      // Set the body to the force model origin, or Earth if origin isn't set
      if (forceOrigin)
         body = forceOrigin;
      else
         body = solarSystem->GetBody(GmatSolarSystemDefaults::EARTH_NAME);

      #ifdef DEBUG_SRP_ORIGIN
         MessageInterface::ShowMessage("SRP body is %s\n", 
            body->GetName().c_str());
      #endif

      if (!body)
         throw ODEModelException("Central body not set for SRP force.");

      if (body->GetName() == "Sun")         
         bodyIsTheSun = true;
      else
         bodyIsTheSun = false;
      bodyRadius = body->GetEquatorialRadius();

      if (forceVector)
         delete [] forceVector;
   
      forceVector = new Real[dimension];
      
      if (cbSunVector)
         delete [] cbSunVector;
      
      cbSunVector = new Real[3];

      // if (srpShapeModel == "Spherical")                              // made changes by TUAN NGUYEN
      if (srpShapeModelIndex == ShapeModel::SPHERICAL_MODEL)            // made changes by TUAN NGUYEN
         hasTimeJacobian = true;
      
      isInitialized = true;
   }
   
   #ifdef DEBUG_SRP_ORIGIN
      shadowModel = 0;  // CONICAL_MODEL
   #endif
   
   return isInitialized;
}

//------------------------------------------------------------------------------
// bool SolarRadiationPressure::SetCentralBody()
//------------------------------------------------------------------------------
/**
 * Sets the up the Sun and central body for use
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::SetCentralBody()
{
   if (!solarSystem)
      throw ODEModelException("Solar system not set in call to SRP::SetCentralBody.");
   
   theSun = solarSystem->GetBody(GmatSolarSystemDefaults::SUN_NAME);
   
   if (!theSun)
      throw ODEModelException("Solar system does not contain the Sun for SRP force.");
   
   // Set the body to the force model origin, or Earth if origin isn't set
   if (forceOrigin)
      body = forceOrigin;
   else
      body = solarSystem->GetBody(GmatSolarSystemDefaults::EARTH_NAME);

   #ifdef DEBUG_SRP_ORIGIN
      MessageInterface::ShowMessage("SRP body is %s\n", 
         body->GetName().c_str());
   #endif

   if (!body)
      throw ODEModelException("Central body not set for SRP force.");

   bodyRadius = body->GetEquatorialRadius();

   #ifdef DEBUG_SRP_ORIGIN
      MessageInterface::ShowMessage("SRP central body set to %s\n", 
         body->GetName().c_str());
      shadowModel = 0;  // CONICAL_MODEL
   #endif
   
   
   return true;
}

//------------------------------------------------------------------------------
// bool SolarRadiationPressure::GetDerivatives(Real *state,Real dt,Integer order)
//------------------------------------------------------------------------------
/**
 * Calculates the SRP accelerations
 * 
 * @param <state> The state vector for the spacecraft
 * @param <dt>    Epoch offset of the state
 * @param <order> The order of derivative needed
 * 
 * @return true on success, false if an error was encountered
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::GetDerivatives(Real *state, Real dt, Integer order, 
      const Integer id)
{
   if (!isInitialized)
      return false;
   
   if (!theSun)
      throw ODEModelException("The Sun is not set in SRP::GetDerivatives");
   
   if (!body)
      throw ODEModelException(
         "The central body is not set in SRP::GetDerivatives");
   
   if (!cbSunVector)
      throw ODEModelException(
         "The sun vector is not initialized in SRP::GetDerivatives");
   
   if ((Integer)cr.size() != satCount)
   {
      std::stringstream msg;
      msg << "Mismatch between satellite count (" << satCount 
          << ") and radiation coefficient count (" << cr.size() << ")"; 
      throw ODEModelException(msg.str());
   }
   
   if ((Integer)area.size() != satCount)
   {
      std::stringstream msg;
      msg << "Mismatch between satellite count (" << satCount   
          << ") and area count (" << area.size() << ")"; 
      throw ODEModelException(msg.str());
   }
   
   if ((Integer)mass.size() != satCount)
   {
      std::stringstream msg;
      msg << "Mismatch between satellite count (" << satCount   
          << ") and mass count (" << mass.size() << ")"; 
      throw ODEModelException(msg.str());
   }
   
   if ((Integer)scObjs.size() != satCount)
   {
      std::stringstream msg;
      msg << "Mismatch between satellite count (" << satCount
          << ") and object count (" << scObjs.size() << ")";
      throw ODEModelException(msg.str());
   }
   
   Real distancefactor = 1.0, mag, magInitial, sSquared;
   bool inSunlight = true, inShadow = false;

   Real ep = epoch + (elapsedTime + dt) / GmatTimeConstants::SECS_PER_DAY;
   sunrv = theSun->GetState(ep);
   
   // Rvector6 is initialized to all 0.0's; only change it if the body is not 
   // the Sun
   if (!bodyIsTheSun)
   {
      cbrv = body->GetState(ep);
      cbSunVector[0] = sunrv[0] - cbrv[0];
      cbSunVector[1] = sunrv[1] - cbrv[1];
      cbSunVector[2] = sunrv[2] - cbrv[2];
   }
   else
   {
      cbSunVector[0] = 0.0;
      cbSunVector[1] = 0.0;
      cbSunVector[2] = 0.0;
   }

   #ifdef DEBUG_SRP_ORIGIN
      bool showData = false;
      if (shadowModel == 0) 
      {
         MessageInterface::ShowMessage("SRP[%s] Sun Position: %lf %lf %lf\n", 
            body->GetName().c_str(), cbSunVector[0], cbSunVector[1], 
            cbSunVector[2]);
         showData = true;
      }
   #endif
   
   if (order > 2)
      return false;
   
   //Integer i6, ix;          // made changes by TUAN NGUYEN
   Integer ix;                // made changes by TUAN NGUYEN
   Real sunSat[3];
   Rvector3 spadArea;
   Real appSunRad, appBodyRad, appDistFromSunToBody;
    
   Rvector3 scReflectance;    // made changes by TUAN NGUYEN
   
   if (fillCartesian)
   {
      for (Integer i = 0; i < satCount; ++i) 
      {
         #ifdef DEBUG_DERIVATIVE
            MessageInterface::ShowMessage(
                  "Filling Cartesian state for spacecraft %d\n", i);
         #endif
         Integer i6 = cartesianStart + i*6;                                  // made changes by TUAN NGUYEN
         
         // Build vector from the Sun to the current spacecraft
         sunSat[0] = state[ i6 ] - cbSunVector[0];
         sunSat[1] = state[i6+1] - cbSunVector[1];
         sunSat[2] = state[i6+2] - cbSunVector[2];
         sunDistance = sqrt(sunSat[0]*sunSat[0] + sunSat[1]*sunSat[1] + 
                           sunSat[2]*sunSat[2]);
         if (sunDistance == 0.0)
            sunDistance = 1.0;
         // Make a unit vector for the force direction
         forceVector[0] = sunSat[0] / sunDistance;
         forceVector[1] = sunSat[1] / sunDistance;
         forceVector[2] = sunSat[2] / sunDistance;
        
         distancefactor = nominalSun / sunDistance;        // it has no unit (Km/Km)
         distancefactor *= distancefactor;                 // (1AU/rs)^2

         #ifdef DEBUG_SRP_ORIGIN
            if (shadowModel == 0) 
               shadowModel = CONICAL_MODEL;
         #endif
         
         // Test shadow condition for current spacecraft (only if body isn't Sol)
         if (!bodyIsTheSun)
         {
            psunrad = asin(sunRadius / sunDistance);
            if (sunRadius < sunDistance)                                             // This change avoids ASIN() error
            {
               std::string shModel = "DualCone";
               if (shadowModel == CYLINDRICAL_MODEL) shModel = "Cylindrical";

               #ifdef DEBUG_SHADOW_STATE
                  MessageInterface::ShowMessage("before FSS, sunRadius= %12.10f, psunrad = %12.10f\n",
                     sunRadius, psunrad);
                  MessageInterface::ShowMessage("   state       = %12.10f %12.10f %12.10f\n",
                     state[i6], state[i6+1], state[i6+2]);
                  MessageInterface::ShowMessage("   cbSunVector = %12.10f %12.10f %12.10f\n",
                     cbSunVector[0], cbSunVector[1], cbSunVector[2]);
                  MessageInterface::ShowMessage("   sunSat      = %12.10f %12.10f %12.10f\n",
                     sunSat[0], sunSat[1], sunSat[2]);
                  MessageInterface::ShowMessage("   forceVector = %12.10f %12.10f %12.10f\n",
                     forceVector[0], forceVector[1], forceVector[2]);
               #endif

               percentSun = shadowState->FindShadowState(inSunlight, inShadow,
                     shModel, &state[i6], cbSunVector, sunSat, forceVector,
                     sunRadius, bodyRadius, psunrad, appSunRad, appBodyRad,
                     appDistFromSunToBody);
            }
            else
            {
               inSunlight = true;
               inShadow = false;
               percentSun = 1.0;
            }
         }
         else
         {
             inSunlight= true;
             inShadow = false;
             percentSun = 1.0;
         }
         
         #ifdef IGNORE_SHADOWS
            percentSun = 1.0;
         #endif

         if (!inShadow) 
         {
            // Montenbruck and Gill, eq. 3.75
            #ifdef DEBUG_SPAD_DATA
            MessageInterface::ShowMessage(" \n------> now using spad data for %s\n",
                  scObjs.at(i)->GetName().c_str());
            MessageInterface::ShowMessage("   nominalSun     = %12.10f Km\n", nominalSun);
            MessageInterface::ShowMessage("   sunDistance    = %12.10f Km\n", sunDistance);
            MessageInterface::ShowMessage("   state          = %12.10f  %12.10f  %12.10f Km\n",
                  state[i6], state[i6+1], state[i6+2]);
            MessageInterface::ShowMessage("   cbSunVector    = %12.10f  %12.10f  %12.10f Km\n",
                  cbSunVector[0], cbSunVector[1], cbSunVector[2]);
            MessageInterface::ShowMessage("   sunSat    = %12.10f  %12.10f  %12.10f Km\n",
                  sunSat[0], sunSat[1], sunSat[2]);
            MessageInterface::ShowMessage("   sunDistance    = %12.10f Km\n", sunDistance);
            MessageInterface::ShowMessage("   percentSun     = %12.10f\n", percentSun);
            MessageInterface::ShowMessage("   fluxPressure   = %12.10f N/m^2\n", fluxPressure);
            MessageInterface::ShowMessage("   distancefactor = %12.10f\n", distancefactor);
            MessageInterface::ShowMessage("   mass           = %12.10f Kg\n", mass[i]);
            #endif
            mag = percentSun * fluxPressure * distancefactor /
                                mass[i];                               // its current unit is (N/m^2)/Kg = 1/(m*s^2)
            
            // if (srpShapeModel == "Spherical")                                                   // made changes by TUAN NGUYEN
            if (srpShapeModelIndex == ShapeModel::SPHERICAL_MODEL)                                 // made changes by TUAN NGUYEN
            {
               #ifdef DEBUG_CR_UPDATES
                  MessageInterface::ShowMessage("SRP using Cr = %.12lf\n", cr[i]);
               #endif
               
               mag *= cr[i] * area[i];                             // Its current unit is [1/(m*s^2)]*(m^2) = m/s^2   // made changes by TUAN NGUYEN
               mag = mag*GmatMathConstants::M_TO_KM;                  // convert m/s^2 to Km/s^2
               
               if (order == 1)
               {
                  deriv[i6] = deriv[i6 + 1] = deriv[i6 + 2] = 0.0;
                  deriv[i6 + 3] = mag * forceVector[0];               // Unit: Km/s^2
                  deriv[i6 + 4] = mag * forceVector[1];               // Unit: Km/s^2
                  deriv[i6 + 5] = mag * forceVector[2];               // Unit: Km/s^2
               }
               else
               {
                  deriv[ i6 ] = mag * forceVector[0];                 // Unit: Km/s^2
                  deriv[i6+1] = mag * forceVector[1];                 // Unit: Km/s^2
                  deriv[i6+2] = mag * forceVector[2];                 // Unit: Km/s^2
                  deriv[i6 + 3] = deriv[i6 + 4] = deriv[i6 + 5] = 0.0;
               }
            }
            // else if (srpShapeModel == "SPADFile")  // SPADFile   // made changes by TUAN NGUYEN
            else if (srpShapeModelIndex == ShapeModel::SPAD_FILE_MODEL)  // SPADFile   // made changes by TUAN NGUYEN
            {
               #ifdef DEBUG_SPAD_DATA
                  MessageInterface::ShowMessage("in SRP, using SPAD file and mag = %12.16le \n", mag);
               #endif
               if (!scObjs.at(i)->IsOfType("Spacecraft"))
               {
                  std::stringstream msg;
                  msg << "Satellite " << scObjs.at(i)->GetName();
                  msg << " is not of type Spacecraft.  SPAD SRP area cannot ";
                  msg << "be obtained.\n";
                  throw ODEModelException(msg.str());
               }
               Rvector3 sunSC(sunSat[0], sunSat[1], sunSat[2]);  // need to modify for performance?
               spadArea = ((Spacecraft*) scObjs.at(i))->GetSPADSRPArea(ep, sunSC);
               #ifdef DEBUG_SPAD_DATA
                  MessageInterface::ShowMessage("in SRP, SPAD area from spacecraft (which calls filereader) = %12.10f  %12.10f  %12.10f\n",
                        spadArea[0],spadArea[1],spadArea[2]);
               #endif
               if (order == 1)
               {
                  deriv[i6] = deriv[i6 + 1] = deriv[i6 + 2] = 0.0;
                  deriv[i6 + 3] = mag * spadArea[0] * GmatMathConstants::M_TO_KM;                  // unit: [1/(m*s^2)]*(m^2)*(Km/m) = Km/s^2
                  deriv[i6 + 4] = mag * spadArea[1] * GmatMathConstants::M_TO_KM;                  // unit: [1/(m*s^2)]*(m^2)*(Km/m) = Km/s^2
                  deriv[i6 + 5] = mag * spadArea[2] * GmatMathConstants::M_TO_KM;                  // unit: [1/(m*s^2)]*(m^2)*(Km/m) = Km/s^2
               }
               else
               {
                  deriv[i6]     = mag * spadArea[0] * GmatMathConstants::M_TO_KM;                  // unit: [1/(m*s^2)]*(m^2)*(Km/m) = Km/s^2
                  deriv[i6 + 1] = mag * spadArea[1] * GmatMathConstants::M_TO_KM;                  // unit: [1/(m*s^2)]*(m^2)*(Km/m) = Km/s^2
                  deriv[i6 + 2] = mag * spadArea[2] * GmatMathConstants::M_TO_KM;                  // unit: [1/(m*s^2)]*(m^2)*(Km/m) = Km/s^2
                  deriv[i6 + 3] = deriv[i6 + 4] = deriv[i6 + 5] = 0.0;
               }
               #ifdef DEBUG_SPAD_DATA
                  MessageInterface::ShowMessage(
                        "Using SPAD, deriv = %12.16le  %12.16le  %12.16le  %12.16le  %12.16le  %12.16le\n",
                        deriv[ i6 ], deriv[ i6+1 ], deriv[ i6+2 ],
                        deriv[ i6+3 ], deriv[ i6+4 ], deriv[ i6+5 ]);
               #endif
            }
            // else if (srpShapeModel == "NPlate")  // NPlate   // made changes by TUAN NGUYEN
            else if (srpShapeModelIndex == ShapeModel::NPLATE_MODEL)  // NPlate   // made changes by TUAN NGUYEN
            {
               #ifdef DEBUG_NPLATE_DATA
                  MessageInterface::ShowMessage("in SRP, using N-Plates file and mag = %12.16le \n", mag);
               #endif
               if (!scObjs.at(i)->IsOfType("Spacecraft"))
               {
                  std::stringstream msg;
                  msg << "Satellite " << scObjs.at(i)->GetName();
                  msg << " is not of type Spacecraft.  N-Plates SRP area cannot ";
                  msg << "be obtained.\n";
                  throw ODEModelException(msg.str());
               }

               // Get spacecraft's refectance vector in inertial frame in Eq.1 SRP N-Plates MathSpec
               Rvector3 sunSC(sunSat[0], sunSat[1], sunSat[2]);
               ///@todo: It needs to verify epochGT at this point to make sure input a correct value
               GmatTime gtep = GmatTime(ep);
               scReflectance = ((Spacecraft*)scObjs.at(i))->GetNPlateSRPReflectance(sunSC, gtep);          // unit: m^2

               #ifdef DEBUG_NPLATE_DATA
                  MessageInterface::ShowMessage("in SRP, N-Plates reflectance from spacecraft (which calls filereader) = %12.10f  %12.10f  %12.10f\n",
                     scReflectance[0], scReflectance[1], scReflectance[2]);
                  MessageInterface::ShowMessage(" dot(s,A) = %le. It should be a negative number.\n", sunSat[0] * scReflectance[0] + sunSat[1] * scReflectance[1] + sunSat[2] * scReflectance[2]);
               #endif
               if (order == 1)
               {
                  deriv[i6] = deriv[i6 + 1] = deriv[i6 + 2] = 0.0;
                  // Eq.1 SRP N-Plates MathSpec. Spacecraft's reflection vector has opposite direction of SRP force. Therefore, -mag is used instead of mag.
                  deriv[i6 + 3] = (-mag) * scReflectance[0] * GmatMathConstants::M_TO_KM;                  // unit: [1/(m*s^2)]*(m^2)*(Km/m) = Km/s^2
                  deriv[i6 + 4] = (-mag) * scReflectance[1] * GmatMathConstants::M_TO_KM;                  // unit: [1/(m*s^2)]*(m^2)*(Km/m) = Km/s^2
                  deriv[i6 + 5] = (-mag) * scReflectance[2] * GmatMathConstants::M_TO_KM;                  // unit: [1/(m*s^2)]*(m^2)*(Km/m) = Km/s^2
               }
               else
               {
                  // Eq.1 SRP N-Plates MathSpec. Spacecraft's reflection vector has opposite direction of SRP force. Therefore, -mag is used instead of mag.
                  deriv[i6] = (-mag) * scReflectance[0] * GmatMathConstants::M_TO_KM;                      // unit: [1/(m*s^2)]*(m^2)*(Km/m) = Km/s^2
                  deriv[i6 + 1] = (-mag) * scReflectance[1] * GmatMathConstants::M_TO_KM;                  // unit: [1/(m*s^2)]*(m^2)*(Km/m) = Km/s^2
                  deriv[i6 + 2] = (-mag) * scReflectance[2] * GmatMathConstants::M_TO_KM;                  // unit: [1/(m*s^2)]*(m^2)*(Km/m) = Km/s^2
                  deriv[i6 + 3] = deriv[i6 + 4] = deriv[i6 + 5] = 0.0;
               }
            }
            else                                  // made changes by TUAN NGUYEN
            {
               throw ODEModelException("Error: SolarRadiationPressure SRP Model was not specified. It needs to set to Spherical, SPADFile, or NPlate.\n");  // made changes by TUAN NGUYEN
            }
         }
         else 
         {
            deriv[i6] = deriv[i6 + 1] = deriv[i6 + 2] = 
            deriv[i6 + 3] = deriv[i6 + 4] = deriv[i6 + 5] = 0.0;
         } // endif/else !inShadow
      } // endfor i = 0->satcount
   }  // endif fillCartesian
   
   
   if (fillSTM || fillAMatrix)
   {
      if (warnSRPMath && (percentSun < 1.0))
      {
         // if ((srpShapeModel == "Spherical") || (srpShapeModel == "SPADFile") || (srpShapeModel == "NPlate"))   // made changes by TUAN NGUYEN
         if ((srpShapeModelIndex == ShapeModel::SPHERICAL_MODEL) || (srpShapeModelIndex == ShapeModel::SPAD_FILE_MODEL) || (srpShapeModelIndex == ShapeModel::NPLATE_MODEL))   // made changes by TUAN NGUYEN
         {
            MessageInterface::ShowMessage("Warning: The orbit state transition "
                  "matrix does not currently contain SRP contributions from shadow "
                  "partial derivatives when using " + srpShapeModel + " SRP.\n");

            warnSRPMath = false;
         }
      }
      
      
      //Integer index = psm->GetSTMIndex(crID);                                 // made changes by TUAN NGUYEN
      //if (index >= 0)                                                         // made changes by TUAN NGUYEN
      //{                                                                       // made changes by TUAN NGUYEN
      //   estimatingCr = true;                                                 // made changes by TUAN NGUYEN
      //   crEpsilonRow = index;                                                // made changes by TUAN NGUYEN
      //}                                                                       // made changes by TUAN NGUYEN
      
      stmRowCount = 0;
      Integer aTildeRowCount = 0;                                               // made changes by TUAN NGUYEN
      Integer associate, element;
      Integer iStart = stmStart;                                                // made changes by TUAN NGUYEN
      for (Integer i = 0; i < stmCount; ++i)
      {
         Integer crEpsilonRow = psm->GetSTMIndex(crID, scObjs[i]) - stmRowCount;   // made changes by TUAN NGUYEN

         StringArray sfs = scObjs[i]->GetStringArrayParameter("SolveFors");        // made changes by TUAN NGUYEN
         bool estimatingCr = false;                                                // made changes by TUAN NGUYEN
         for (Integer j = 0; j < sfs.size(); ++j)                                  // made changes by TUAN NGUYEN
         {                                                                         // made changes by TUAN NGUYEN
            //if (GmatGlobal::Instance()->GetDebug())
            //   MessageInterface::ShowMessage("sfs[%d] = <%s>\n", j, sfs[j].c_str());
            if ((sfs[j] == "Cr")|| (sfs[j] == "SPADSRPScaleFactor"))               // made changes by TUAN NGUYEN
            {                                                                      // made changes by TUAN NGUYEN
               estimatingCr = true;                                                // made changes by TUAN NGUYEN
               break;                                                              // made changes by TUAN NGUYEN
            }                                                                      // made changes by TUAN NGUYEN
         }                                                                         // made changes by TUAN NGUYEN

         // with thout this line, i6 always points to the last spacrcraft's state  // made changes by TUAN NGUYEN
         Integer i6 = cartesianStart + i * 6;                                      // made changes by TUAN NGUYEN

         // Get size of spacecraft's STM
         Spacecraft *sc = (Spacecraft*)scObjs[i];
         
         // Create aTilde matrix
         stmRowCount = sc->GetIntegerParameter("FullSTMRowCount");
         //if (GmatGlobal::Instance()->GetDebug())
         //   MessageInterface::ShowMessage("stmRowCount = %d   estimatingCr = %s\n", stmRowCount, (estimatingCr?"true":"false"));

         //Integer stmSize = stmRowCount * stmRowCount;                            // made changea by TUAN NGUYEN
         //Real *aTilde = new Real[stmSize];                                       // made changea by TUAN NGUYEN
         aTildeRowCount = stmRowCount;                                             // made changea by TUAN NGUYEN
         Real *aTilde = new Real[stmRowCount*stmRowCount];                         // made changea by TUAN NGUYEN
         for (Integer j = 0; j < stmRowCount; ++j)
         {
            ix = j * stmRowCount;
            for (Integer k = 0; k < stmRowCount; ++k)
               aTilde[ix + k] = 0.0;
         }

         #ifdef DEBUG_STM_MATRIX
            MessageInterface::ShowMessage("Filling STM for spacecraft %d\n", i);
         #endif
         // Integer iStart = stmStart + i * stmRowCount*stmRowCount;           // made changes by TUAN NGUYEN
         
         // Calculate A-tilde
         // Build vector from Sun to the current spacecraft; (-s in math spec)
         associate = theState->GetAssociateIndex(iStart);
         sunSat[0] = state[ associate ] - cbSunVector[0];
         sunSat[1] = state[associate+1] - cbSunVector[1];
         sunSat[2] = state[associate+2] - cbSunVector[2];
         sunDistance = sqrt(sunSat[0]*sunSat[0] + sunSat[1]*sunSat[1] +
                           sunSat[2]*sunSat[2]);
         if (sunDistance == 0.0)
            sunDistance = 1.0;

         distancefactor = nominalSun / sunDistance;     // It has no unit (Km/Km)
         distancefactor *= distancefactor;

         #ifdef DEBUG_SRP_ORIGIN
            if (shadowModel == 0)
               shadowModel = CONICAL_MODEL;
         #endif
         
         // Test shadow condition for current spacecraft (only if body isn't Sol)
         if (!bodyIsTheSun)
         {
            psunrad = asin(sunRadius / sunDistance);
            if (sunRadius < sunDistance)                                               // This change avoids ASIN() error
            {
               std::string shModel = "DualCone";
               if (shadowModel == CYLINDRICAL_MODEL) shModel = "Cylindrical";
               percentSun = shadowState->FindShadowState(inSunlight, inShadow,
                  // shModel, &state[i6], cbSunVector, sunSat, forceVector,
                  shModel, &state[associate], cbSunVector, sunSat, forceVector,
                  sunRadius, bodyRadius, psunrad, appSunRad, appBodyRad,
                  appDistFromSunToBody);
               //FindShadowState(inSunlight, inShadow, &state[i6]);
            }
            else
            {
               inSunlight = true;
               inShadow = false;
               percentSun = 1.0;
            }
         }
         else
         {
            inSunlight= true;
            inShadow = false;
            percentSun = 1.0;
         }

         #ifdef IGNORE_SHADOWS
            percentSun = 1.0;
         #endif
         
         if (!inShadow)
         {
            //if (srpShapeModel == "Spherical")                                        // made changes by TUAN NGUYEN
            if (srpShapeModelIndex == ShapeModel::SPHERICAL_MODEL)                     // made changes by TUAN NGUYEN
            {
               // All of the common terms for C_s
               mag = percentSun * cr[i] * fluxPressure * area[i] * distancefactor / 
                  (mass[i] * sunDistance);                              // its unit is (N/m^2)*(m^2)/(Kg*Km) = m/(Km*s^2)
               mag = mag*GmatMathConstants::M_TO_KM;                    // convert m/(Km*s^2) to Km/(Km*s^2) = 1/s^2

               sSquared = sunDistance * sunDistance;

               // Math spec terms for SRP C submatrix of the A-matrix
               ix = stmRowCount * 3;
               aTilde[ix]     = mag * (1.0 - 3.0 * sunSat[0] * sunSat[0] / sSquared);       // unit: 1/s^2
               aTilde[ix + 1] = mag * (    - 3.0 * sunSat[0] * sunSat[1] / sSquared);       // unit: 1/s^2
               aTilde[ix + 2] = mag * (    - 3.0 * sunSat[0] * sunSat[2] / sSquared);       // unit: 1/s^2

               // ax term for estimating Cr
               if (estimatingCr)
               {
                  aTilde[ix + crEpsilonRow] = deriv[i6 + 3] / (1 + crEpsilon[i]);          // made changes by TUAN NGUYEN
               }

               ix = stmRowCount * 4;
               aTilde[ix]     = mag * (    - 3.0 * sunSat[1] * sunSat[0] / sSquared);      // unit: 1/s^2
               aTilde[ix + 1] = mag * (1.0 - 3.0 * sunSat[1] * sunSat[1] / sSquared);      // unit: 1/s^2
               aTilde[ix + 2] = mag * (    - 3.0 * sunSat[1] * sunSat[2] / sSquared);      // unit: 1/s^2

               // ay term for estimating Cr
               if (estimatingCr)
               {
                  aTilde[ix + crEpsilonRow] = deriv[i6 + 4] / (1 + crEpsilon[i]);          // made changes by TUAN NGUYEN
               }

               ix = stmRowCount * 5;
               aTilde[ix]     = mag * (    - 3.0 * sunSat[2] * sunSat[0] / sSquared);     // unit: 1/s^2
               aTilde[ix + 1] = mag * (    - 3.0 * sunSat[2] * sunSat[1] / sSquared);     // unit: 1/s^2
               aTilde[ix + 2] = mag * (1.0 - 3.0 * sunSat[2] * sunSat[2] / sSquared);     // unit: 1/s^2

               // az term for estimating Cr
               if (estimatingCr)
               {
                  aTilde[ix + crEpsilonRow] = deriv[i6 + 5] / (1 + crEpsilon[i]);          // made changes by TUAN NGUYEN
               }

            }
            // else if (srpShapeModel == "SPADFile")  // SPADFile  // made changes by TUAN NGUYEN
            else if (srpShapeModelIndex == ShapeModel::SPAD_FILE_MODEL)  // SPADFile  // made changes by TUAN NGUYEN
            {
               Real     nominalState[3];
               nominalState[0] = state[associate];
               nominalState[1] = state[associate+1];
               nominalState[2] = state[associate+2];
               Real     posMag = GmatMathUtil::Sqrt(
                                 nominalState[0] * nominalState[0] +
                                 nominalState[1] * nominalState[1] +
                                 nominalState[2] * nominalState[2]);
               Rvector6 nominalAccel, accelPertX, accelPertY, accelPertZ;
               Real     theState[3];
               Real     dx = posMag * 1e-4;  // guesses by SPH
               Real     dy = posMag * 1e-4;
               Real     dz = posMag * 1e-4;
               nominalAccel    = ComputeSPADAcceleration(i, ep, nominalState, cbSunVector);    // expected unit: Km/s^2 
               #ifdef DEBUG_SPAD_DATA
                   MessageInterface::ShowMessage(
                         "In STM section, using SPAD, nominalAccel = %le  %le  %le  %12.16le  %12.16le  %12.16le\n",
                         nominalAccel[0], nominalAccel[1], nominalAccel[2],
                         nominalAccel[3], nominalAccel[4], nominalAccel[5]);
                   MessageInterface::ShowMessage("    dx  %12.10f,  dy = %12.10f, dz = %12.10f\n", dx, dy, dz);
                #endif
               // Compute acceleration perturbed in X
               theState[0]     = nominalState[0] + dx;
               theState[1]     = nominalState[1];
               theState[2]     = nominalState[2];
               // @todo Modify the spacecraft state with each perturbation, to make sure attitude is correct
               // Must save current sc state first, then reset after perturbations <<<<<<<<<<<<<<<<<<TBD
               // BUT question - how to reset the state (stat variable is passed in)???
               accelPertX      = ComputeSPADAcceleration(i, ep, theState, cbSunVector);      // expected unit: Km/s^2
               // Compute acceleration perturbed in Y
               theState[0]     = nominalState[0];
               theState[1]     = nominalState[1] + dy;
               theState[2]     = nominalState[2];
               accelPertY      = ComputeSPADAcceleration(i, ep, theState, cbSunVector);      // expected unit: Km/s^2
               // Compute acceleration perturbed in Z
               theState[0]     = nominalState[0];
               theState[1]     = nominalState[1];
               theState[2]     = nominalState[2] + dz;
               accelPertZ      = ComputeSPADAcceleration(i, ep, theState, cbSunVector);     // expected unit: Km/s^2
               // Fill in aTilde
               ix = stmRowCount * 3;
               aTilde[ix]   = (accelPertX[3] - nominalAccel[3]) / dx;       // unit: 1/s^2
               aTilde[ix+1] = (accelPertY[3] - nominalAccel[3]) / dy;       // unit: 1/s^2
               aTilde[ix+2] = (accelPertZ[3] - nominalAccel[3]) / dz;       // unit: 1/s^2

               // ax term for estimating Cr                                                      // made changes by TUAN NGUYEN
               if (estimatingCr)                                                                 // made changes by TUAN NGUYEN
               {
                  aTilde[ix + crEpsilonRow] = deriv[i6 + 3] / (1 + crEpsilon[i]);                // made changes by TUAN NGUYEN
               }
               ix = stmRowCount * 4;
               aTilde[ix]   = (accelPertX[4] - nominalAccel[4]) / dx;       // unit: 1/s^2
               aTilde[ix+1] = (accelPertY[4] - nominalAccel[4]) / dy;       // unit: 1/s^2
               aTilde[ix+2] = (accelPertZ[4] - nominalAccel[4]) / dz;       // unit: 1/s^2

               // ay term for estimating Cr                                                      // made changes by TUAN NGUYEN
               if (estimatingCr)                                                                 // made changes by TUAN NGUYEN
               {
                  aTilde[ix + crEpsilonRow] = deriv[i6 + 4] / (1 + crEpsilon[i]);                // made changes by TUAN NGUYEN
               }

               ix = stmRowCount * 5;
               aTilde[ix]   = (accelPertX[5] - nominalAccel[5]) / dx;       // unit: 1/s^2
               aTilde[ix+1] = (accelPertY[5] - nominalAccel[5]) / dy;       // unit: 1/s^2
               aTilde[ix+2] = (accelPertZ[5] - nominalAccel[5]) / dz;       // unit: 1/s^2

               // az term for estimating Cr                                                      // made changes by TUAN NGUYEN
               if (estimatingCr)                                                                 // made changes by TUAN NGUYEN
               {
                  aTilde[ix + crEpsilonRow] = deriv[i6 + 5] / (1 + crEpsilon[i]);                // made changes by TUAN NGUYEN
               }
               #ifdef DEBUG_SPAD_DATA
                  MessageInterface::ShowMessage("    accelPertX          = %12.16le  %12.16le  %12.16le  %12.16le  %12.16le  %12.16le\n",
                        accelPertX[0], accelPertX[1], accelPertX[2],accelPertX[3],accelPertX[4],accelPertX[5]);
                  MessageInterface::ShowMessage("    accelPertY          = %12.16le  %12.16le  %12.16le  %12.16le  %12.16le  %12.16le\n",
                        accelPertY[0], accelPertY[1], accelPertY[2],accelPertY[3],accelPertY[4],accelPertY[5]);
                  MessageInterface::ShowMessage("    accelPertZ          = %12.16le  %12.16le  %12.16le  %12.16le  %12.16le  %12.16le\n",
                        accelPertZ[0], accelPertZ[1], accelPertZ[2],accelPertZ[3],accelPertZ[4],accelPertZ[5]);
                  MessageInterface::ShowMessage(
                        "In STM section, resulting aTilde[18]  = %12.16le\n", aTilde[18]);
                  MessageInterface::ShowMessage(
                        "                resulting aTilde[19]  = %12.16le\n", aTilde[19]);
                  MessageInterface::ShowMessage(
                        "                resulting aTilde[20]  = %12.16le\n", aTilde[20]);
                  MessageInterface::ShowMessage(
                        "                resulting aTilde[24]  = %12.16le\n", aTilde[24]);
                  MessageInterface::ShowMessage(
                        "                resulting aTilde[25]  = %12.16le\n", aTilde[25]);
                  MessageInterface::ShowMessage(
                        "                resulting aTilde[26]  = %12.16le\n", aTilde[26]);
                  MessageInterface::ShowMessage(
                        "                resulting aTilde[30]  = %12.16le\n", aTilde[30]);
                  MessageInterface::ShowMessage(
                        "                resulting aTilde[31]  = %12.16le\n", aTilde[31]);
                  MessageInterface::ShowMessage(
                        "                resulting aTilde[32]  = %12.16le\n", aTilde[32]);
                #endif
            }
            // else if (srpShapeModel == "NPlate")  // NPlate  // made changes by TUAN NGUYEN
            else if (srpShapeModelIndex == ShapeModel::NPLATE_MODEL)  // NPlate  // made changes by TUAN NGUYEN
            {
               Rvector3 sunSC(sunSat[0], sunSat[1], sunSat[2]);

               // Calculate K
               mag = percentSun * fluxPressure * distancefactor / mass[i];     // its unit is (N/m^2)/Kg = (Kg*m/s^2)/(m^2*Kg) = (m/s^2)/(m^2) 
               mag = mag * GmatMathConstants::M_TO_KM;                         // convert (m/s^2)/(m^2) to (Km/s^2)/(m^2)
               Real K = -mag;                                                  // unit: (Km/s^2)/(m^2)

               // Calculate A and dA/dX
               // Reflectance onply depends on spacecarft's attitude at a given epoch and the Sun light direction
               GmatTime gtep2 = GmatTime(ep);
               Rvector3 A = ((Spacecraft*)(scObjs[i]))->GetNPlateSRPReflectance(sunSC, gtep2);                             //unit: m^2     // reflectance in inertial frame
               std::vector<Rvector3> dAdX = ((Spacecraft*)(scObjs[i]))->GetNPlateSRPReflectanceDerivative(sunSC, gtep2);   //unit: m^2/km  // derivative of reflectance in inertial frame
               
               // Calculate dK/dX
               Rvector dKdX;                                                          // unit: [(km/s^2)/(m^2)]/km = 1/s^2/m^2
               dKdX.SetSize(dAdX.size());
               for (Integer col = 3; col < dKdX.GetSize(); ++col)
                  dKdX[col] = 0.0;                     // dKdv = 0 and dK/dCp = 0
               Real term = 2.0 * K / (sunDistance * sunDistance);                     // unit: (Km/s^2)/(m^2)/Km^2 = 1/s^2/m^2/Km
               Rvector3 rs(-sunSat[0], -sunSat[1], -sunSat[2]);        // The Sun vector. It points from spacecraft to the Sun
               dKdX[0] = term * rs[0];            // It is dK/dx                // unit: 1/s^2/m^2
               dKdX[1] = term * rs[1];            // It is dK/dy                // unit: 1/s^2/m^2
               dKdX[2] = term * rs[2];            // It is dK/dz                // unit: 1/s^2/m^2               

               // Calculate dKdX*A + K*dAdX:
               // When running RunSimulator command, stmRowCount = 6 always, but dAdX.size > 6. So we need to resize aTilde matrix as need
               Integer ix0, ix1, ix2;
               aTildeRowCount = dAdX.size();
               if (stmRowCount < aTildeRowCount)
               {
                  // Resize of aTilde matrix if it needs
                  Real* temp = new Real[aTildeRowCount*aTildeRowCount];
                  for (Integer row = 0; row < aTildeRowCount; ++row)
                  {
                     for (Integer col = 0; col < aTildeRowCount; ++col)
                        temp[row * aTildeRowCount + col] = 0.0;
                  }
                  delete [] aTilde;
                  aTilde = temp;

                  // Specify starting index of 4th, 5th, and 6th row of A-matrix 
                  ix0 = aTildeRowCount * 3;          // start index of row 4th
                  ix1 = aTildeRowCount * 4;          // start index of row 5th
                  ix2 = aTildeRowCount * 5;          // start index of row 6th
               }
               else
               {
                  ix0 = stmRowCount * 3;             // start index of row 4th
                  ix1 = stmRowCount * 4;             // start index of row 5th
                  ix2 = stmRowCount * 5;             // start index of row 6th
               }

               Rvector3 vec;
               for (Integer col = 0; col < dAdX.size(); ++col)
               {
                  // Eq.8 in SRP N-Plates MathSpec: dF/dX = d(K.A)/dX = [dK/dX]*A + K*[dA/dX]; where: A is relectance in inertial frame
                  vec = dKdX[col] * A + K * dAdX[col];                              // unit: (1/s^2/m^2)*(m^2) + (Km/s^2)/(m^2)*(m^2/Km) = 1/s^2
                  aTilde[ix0 + col] = vec[0];        // aTilde(3,col)               // unit: 1/s^2
                  aTilde[ix1 + col] = vec[1];        // aTilde(4,col)               // unit: 1/s^2
                  aTilde[ix2 + col] = vec[2];        // aTilde(5,col)               // unit: 1/s^2
               }
               
               #ifdef DEBUG_NPLATE_DATA
                  //if (GmatGlobal::Instance()->GetDebug())
                  //{
                     MessageInterface::ShowMessage("In STM section, aTilde = [\n");
                     for (Integer row = 0; row < stmRowCount; ++row)
                     {
                        for (Integer col = 0; col < stmRowCount; ++col)
                           MessageInterface::ShowMessage("%12.16le   ", aTilde[row*stmRowCount + col]);
                        MessageInterface::ShowMessage("\n");
                     }
                     MessageInterface::ShowMessage("]\n");
                  //}
               #endif
            }
            else                                  // made changes by TUAN NGUYEN
            {
               throw ODEModelException("Error: SolarRadiationPressure SRP Model was not specified. It needs to set to Spherical, SPADFile, or NPlate.\n");  // made changes by TUAN NGUYEN
            }
         }
         else
         {
            // in shadow, it has no SRP force so acceleration equals 0. Thus da/dX = Zero
            ix = stmRowCount * 3;
            for (Integer j = 0; j < 3*stmRowCount; ++j)                // made changes by TUAN NGUYEN
               aTilde[ix + j] = 0.0;                                   // made changes by TUAN NGUYEN
         }
         
         for (Integer j = 0; j < stmRowCount; ++j)
         {
            for (Integer k = 0; k < stmRowCount; ++k)
            {
               element = j * stmRowCount + k;
               if (stmRowCount == aTildeRowCount)                                     // made changes by TUAN NGUYEN
                  deriv[iStart + element] = aTilde[element]; 
               else                                                                   // made changes by TUAN NGUYEN
                  deriv[iStart + element] = aTilde[j * aTildeRowCount + k];           // made changes by TUAN NGUYEN
            }
         }

         //if (GmatGlobal::Instance()->GetDebug())
         //{
         //   MessageInterface::ShowMessage("aTilde for scObj[%d] <%s> srpShapeModel <%s> = [  stmRowCount = %d\n", i, scObjs[i]->GetName().c_str(), srpShapeModel.c_str(), stmRowCount);
         //   for (Integer j = 0; j < stmRowCount; ++j)
         //   {
         //      for (Integer k = 0; k < stmRowCount; ++k)
         //      {
         //         element = j * stmRowCount + k;
         //         MessageInterface::ShowMessage("%.12le   ", aTilde[element]);
         //      }
         //      MessageInterface::ShowMessage("\n");
         //   }
         //   MessageInterface::ShowMessage("]\n\n");
         //}
         
         // increment index to the next spacecraft's STM matrix
         iStart = iStart + stmRowCount*stmRowCount;                               // made changes by TUAN NGUYEN
         delete [] aTilde;                                                        // made changes by TUAN NGUYEN
      }

      
      iStart = aMatrixStart;                                                      // made changes by TUAN NGUYEN
      stmRowCount = 0;                                                            // made changes by TUAN NGUYEN
      aTildeRowCount = 0;                                                         // made changes by TUAN NGUYEN
      for (Integer i = 0; i < aMatrixCount; ++i)
      {
         Integer crEpsilonRow = psm->GetSTMIndex(crID, scObjs[i]) - stmRowCount;   // made changes by TUAN NGUYEN

         StringArray sfs = scObjs[i]->GetStringArrayParameter("SolveFors");        // made changes by TUAN NGUYEN
         bool estimatingCr = false;                                                // made changes by TUAN NGUYEN
         for (Integer j = 0; j < sfs.size(); ++j)                                  // made changes by TUAN NGUYEN
         {                                                                         // made changes by TUAN NGUYEN
            if (sfs[j] == "Cr")                                                    // made changes by TUAN NGUYEN
            {                                                                      // made changes by TUAN NGUYEN
               estimatingCr = true;                                                // made changes by TUAN NGUYEN
               break;                                                              // made changes by TUAN NGUYEN
            }                                                                      // made changes by TUAN NGUYEN
         }                                                                         // made changes by TUAN NGUYEN

         // Get size of spacecraft's STM
         Spacecraft *sc = (Spacecraft*)scObjs[i];
         
         // Define size of A tilde matrix
         stmRowCount = sc->GetIntegerParameter("FullSTMRowCount");
         // Integer stmSize = stmRowCount * stmRowCount;                           // made changes by TUAN NGUYEN
         // Real *aTilde = new Real[stmSize];                                      // made changes by TUAN NGUYEN
         aTildeRowCount = stmRowCount;                                             // made changes by TUAN NGUYEN
         Real *aTilde = new Real[stmRowCount*stmRowCount];                         // made changes by TUAN NGUYEN
         for (Integer j = 0; j < stmRowCount; ++j)
         {
            ix = j * stmRowCount;
            for (Integer k = 0; k < stmRowCount; ++k)
               aTilde[ix + k] = 0.0;
         }

         #ifdef DEBUG_A_MATRIX
            MessageInterface::ShowMessage(
                  "Filling A-matrix for spacecraft %d\n", i);
         #endif
         
         // Integer iStart = aMatrixStart + i * stmRowCount*stmRowCount;          // made changes by TUAN NGUYEN
         
         // Calculate A-tilde
         // Build vector from Sun to the current spacecraft; (-s in math spec)
         associate = theState->GetAssociateIndex(iStart);
         sunSat[0] = state[ associate ] - cbSunVector[0];
         sunSat[1] = state[associate+1] - cbSunVector[1];
         sunSat[2] = state[associate+2] - cbSunVector[2];
         sunDistance = sqrt(sunSat[0]*sunSat[0] + sunSat[1]*sunSat[1] +
                           sunSat[2]*sunSat[2]);
         if (sunDistance == 0.0)
            sunDistance = 1.0;

         distancefactor = nominalSun / sunDistance;                              // It has no unit due to Km/Km
         // Convert m/s^2 to km/s^2
         // distancefactor *= distancefactor * GmatMathConstants::M_TO_KM;
         distancefactor *= distancefactor;

         #ifdef DEBUG_SRP_ORIGIN
            if (shadowModel == 0)
               shadowModel = CONICAL_MODEL;
         #endif
         
         // Test shadow condition for current spacecraft (only if body isn't Sol)
         if (!bodyIsTheSun)
         {
            psunrad = asin(sunRadius / sunDistance);
            if (sunRadius < sunDistance)                                                     // this change avoids ASIN() error
            {
               std::string shModel = "DualCone";
               if (shadowModel == CYLINDRICAL_MODEL) shModel = "Cylindrical";
               percentSun = shadowState->FindShadowState(inSunlight, inShadow,
                  //shModel, &state[i6], cbSunVector, sunSat, forceVector,
                  shModel, &state[associate], cbSunVector, sunSat, forceVector,
                  sunRadius, bodyRadius, psunrad, appSunRad, appBodyRad,
                  appDistFromSunToBody);
               //            FindShadowState(inSunlight, inShadow, &state[i6]);
            }
            else
            {
               inSunlight = true;
               inShadow = false;
               percentSun = 1.0;
            }
         }
         else
         {
            inSunlight= true;
            inShadow = false;
            percentSun = 1.0;
         }

         #ifdef IGNORE_SHADOWS
            percentSun = 1.0;
         #endif

         
         if (!inShadow)
         {
            //if (srpShapeModel == "Spherical")                                          // made changes by TUAN NGUYEN
            if (srpShapeModelIndex == ShapeModel::SPHERICAL_MODEL)                       // made changes by TUAN NGUYEN
            {
               // All of the common terms for C_s
               mag = percentSun * cr[i] * fluxPressure * area[i] * distancefactor /                                                     // made changes by TUAN NGUYEN
                  (mass[i] * sunDistance);                                               // its unit is (N/m^2)*(m^2)/(Kg*Km) = m/(Km*s^2)
               mag = mag * GmatMathConstants::M_TO_KM;                                   // convert m/(Kg*s^2) to Km/(Km*s^2) = 1/s^2

               sSquared = sunDistance * sunDistance;

               // Math spec terms for SRP C submatrix of the A-matrix
               ix = stmRowCount*3;
               aTilde[ix]   = mag * (1.0 - 3.0 * sunSat[0]*sunSat[0] / sSquared);
               aTilde[ix+1] = mag * (    - 3.0 * sunSat[0]*sunSat[1] / sSquared);
               aTilde[ix+2] = mag * (    - 3.0 * sunSat[0]*sunSat[2] / sSquared);
               ix = stmRowCount*4;
               aTilde[ix]   = mag * (    - 3.0 * sunSat[1]*sunSat[0] / sSquared);
               aTilde[ix+1] = mag * (1.0 - 3.0 * sunSat[1]*sunSat[1] / sSquared);
               aTilde[ix+2] = mag * (    - 3.0 * sunSat[1]*sunSat[2] / sSquared);
               ix = stmRowCount*5;
               aTilde[ix]   = mag * (    - 3.0 * sunSat[2]*sunSat[0] / sSquared);
               aTilde[ix+1] = mag * (    - 3.0 * sunSat[2]*sunSat[1] / sSquared);
               aTilde[ix+2] = mag * (1.0 - 3.0 * sunSat[2]*sunSat[2] / sSquared);
            }
            //else if (srpShapeModel == "SPADFile") // SPADFile    // made changes by TUAN NGUYEN
            else if (srpShapeModelIndex == ShapeModel::SPAD_FILE_MODEL) // SPADFile    // made changes by TUAN NGUYEN
            {
               Real     nominalState[3];
               nominalState[0] = state[associate];
               nominalState[1] = state[associate+1];
               nominalState[2] = state[associate+2];
               Real     posMag = GmatMathUtil::Sqrt(
                                 nominalState[0] * nominalState[0] +
                                 nominalState[1] * nominalState[1] +
                                 nominalState[2] * nominalState[2]);
               Rvector6 nominalAccel, accelPertX, accelPertY, accelPertZ;
               Real     theState[3];
               Real     dx = posMag * 1e-4;  // guesses by SPH
               Real     dy = posMag * 1e-4;
               Real     dz = posMag * 1e-4;
               nominalAccel    = ComputeSPADAcceleration(i, ep, nominalState, cbSunVector);       // expected unit: Km/s^2
               // Compute acceleration perturbed in X
               theState[0]     = nominalState[0] + dx;
               theState[1]     = nominalState[1];
               theState[2]     = nominalState[2];
               accelPertX      = ComputeSPADAcceleration(i, ep, theState, cbSunVector);          // expected unit: Km/s^2
               // Compute acceleration perturbed in Y
               theState[0]     = nominalState[0];
               theState[1]     = nominalState[1] + dy;
               theState[2]     = nominalState[2];
               accelPertY      = ComputeSPADAcceleration(i, ep, theState, cbSunVector);          // expected unit: Km/s^2
               // Compute acceleration perturbed in Z
               theState[0]     = nominalState[0];
               theState[1]     = nominalState[1];
               theState[2]     = nominalState[2] + dz;
               accelPertZ      = ComputeSPADAcceleration(i, ep, theState, cbSunVector);          // expected unit: Km/s^2

               // Fill in aTilde
               ix = stmRowCount*3;
               aTilde[ix]     = (accelPertX[3] - nominalAccel[3]) / dx;          // unit: 1/s^2
               aTilde[ix + 1] = (accelPertY[3] - nominalAccel[3]) / dy;          // unit: 1/s^2
               aTilde[ix + 2] = (accelPertZ[3] - nominalAccel[3]) / dz;          // unit: 1/s^2
               ix = stmRowCount*4;
               aTilde[ix]     = (accelPertX[4] - nominalAccel[4]) / dx;          // unit: 1/s^2
               aTilde[ix + 1] = (accelPertY[4] - nominalAccel[4]) / dy;          // unit: 1/s^2
               aTilde[ix + 2] = (accelPertZ[4] - nominalAccel[4]) / dz;          // unit: 1/s^2
               ix = stmRowCount*5;
               aTilde[ix]     = (accelPertX[5] - nominalAccel[5]) / dx;          // unit: 1/s^2
               aTilde[ix + 1] = (accelPertY[5] - nominalAccel[5]) / dy;          // unit: 1/s^2
               aTilde[ix + 2] = (accelPertZ[5] - nominalAccel[5]) / dz;          // unit: 1/s^2
            }
            // else if (srpShapeModel == "NPlate") // NPlate    // made changes by TUAN NGUYEN
            else if (srpShapeModelIndex == ShapeModel::NPLATE_MODEL) // NPlate    // made changes by TUAN NGUYEN
            {
               Rvector3 sunSC(sunSat[0], sunSat[1], sunSat[2]);

               // Calculate K
               mag = percentSun * fluxPressure * distancefactor / mass[i];     // its unit is (N/m^2)/Kg = (Kg*m/s^2)/(m^2*Kg) = (m/s^2)/(m^2) 
               mag = mag * GmatMathConstants::M_TO_KM;                         // convert (m/s^2)/(m^2) to (Km/s^2)/(m^2)
               Real K = -mag;                                                  // unit: (Km/s^2)/(m^2)

               // Calculate dK/dX
               Rvector dKdX;
               dKdX.SetSize(stmRowCount);
               for (Integer col = 3; col < dKdX.GetSize(); ++col)
                  dKdX[col] = 0.0;                     // dKdv = 0 and dK/dCp = 0
               Real term = 2.0 * K / (sunDistance * sunDistance);
               Rvector3 rs(-sunSat[0], -sunSat[1], -sunSat[2]);          // the Sun vector. It points from spacecraft to the Sun
               dKdX[0] = term * rs[0];              // It is dK/dx
               dKdX[1] = term * rs[1];              // It is dK/dy
               dKdX[2] = term * rs[2];              // It is dK/dz

               // Calculate A and dA/dX
               GmatTime gtep3(ep);
               Rvector3 A = ((Spacecraft*)(scObjs[i]))->GetNPlateSRPReflectance(sunSC, gtep3);
               std::vector<Rvector3> dAdX = ((Spacecraft*)(scObjs[i]))->GetNPlateSRPReflectanceDerivative(sunSC, gtep3);

               // Calculate dKdX*A + K*dAdX:
               // aTilde matrix needs to resize when stmRowCount < dAdX.size()
               aTildeRowCount = dAdX.size();
               Integer ix0, ix1, ix2;
               if (stmRowCount < aTildeRowCount)
               {
                  Real* temp = new Real[aTildeRowCount*aTildeRowCount];
                  for (Integer row = 0; row < aTildeRowCount; ++row)
                  {
                     for (Integer col = 0; col < aTildeRowCount; ++col)
                     {
                        temp[row*aTildeRowCount + col] = 0.0;
                     }
                  }
                  delete [] aTilde;
                  aTilde = temp;

                  ix0 = aTildeRowCount * 3;          // start index of row 4th
                  ix1 = aTildeRowCount * 4;          // start index of row 5th
                  ix2 = aTildeRowCount * 5;          // start index of row 6th
               }
               else
               {
                  ix0 = stmRowCount * 3;             // start index of row 4th
                  ix1 = stmRowCount * 4;             // start index of row 5th
                  ix2 = stmRowCount * 5;             // start index of row 6th
               }

               Rvector3 vec;
               for (Integer col = 0; col < dAdX.size(); ++col)
               {
                  vec = dKdX[col] * A + K * dAdX[col];
                  aTilde[ix0 + col] = vec[0];        // aTilde(3,col)
                  aTilde[ix1 + col] = vec[1];        // aTilde(4,col)
                  aTilde[ix2 + col] = vec[2];        // aTilde(5,col)
               }
            }
            else
            {
               throw ODEModelException("Error: SolarRadiationPressure SRP Model was not specified. It needs to set to Spherical, SPADFile, or NPlate.\n");  // made changes by TUAN NGUYEN
            }
         }
         else
         {
            ix = stmRowCount * 3;
            for (Integer j = 0; j < 3*stmRowCount; ++j)
               aTilde[ix + j] = 0.0;
         }
         
         #ifdef DEBUG_A_MATRIX
            MessageInterface::ShowMessage(
                  "A-Matrix contribution[%d] from SRP:\n", i);
         #endif
         for (Integer j = 0; j < stmRowCount; ++j)
         {
            for (Integer k = 0; k < stmRowCount; ++k)
            {
               element = j * stmRowCount + k;
               if (stmRowCount == aTildeRowCount)                                 // made changes by TUAN NGUYEN
                  deriv[iStart+element] = aTilde[element];
               else                                                               // made changes by TUAN NGUYEN
                  deriv[iStart + element] = aTilde[j * aTildeRowCount + k];       // made changes by TUAN NGUYEN
               #ifdef DEBUG_A_MATRIX
                  MessageInterface::ShowMessage("  %le  ", deriv[iStart+element]);
               #endif
            }
            #ifdef DEBUG_A_MATRIX
               MessageInterface::ShowMessage("\n");
            #endif
         }

         // increment index to the next spacecraft's STM matrix
         iStart = iStart + stmRowCount * stmRowCount;                              // made changes by TUAN NGUYEN
         delete [] aTilde;                                                         // made changes by TUAN NGUYEN
      }
      
   }

   Integer i6;
   // if (fillMassJacobian && srpShapeModel == "Spherical")                        // made changes by TUAN NGUYEN
   if (fillMassJacobian && srpShapeModelIndex == ShapeModel::SPHERICAL_MODEL)      // made changes by TUAN NGUYEN
   {
      for (Integer i = 0; i < satCount; ++i)
      {
         i6 = i * 6;
         for (Integer k = 0; k < 6; ++k)
            massJacobian[i6+k] = -deriv[i6+k] / mass[i];
      }
   }

   // if (fillTimeJacobian && srpShapeModel == "Spherical")                        // made changes by TUAN NGUYEN
   if (fillTimeJacobian && srpShapeModelIndex == ShapeModel::SPHERICAL_MODEL)      // made changes by TUAN NGUYEN
   {
      for (Integer i = 0; i < satCount; ++i)
      {
         // Currently, this i6 value is used to determine the associated rv 
         // state vector to send to the FindShadowState method, but this may
         // cause issues in the future when multiple spacecraft are used with
         // state transition matrices since an associated index is not taken
         // from the GetAssociateIndex state method
         i6 = cartesianStart + i * 6;

         std::string shModel = "DualCone";
         if (shadowModel == CYLINDRICAL_MODEL) shModel = "Cylindrical";

         Rvector3 scToSunVec;
         scToSunVec[0] = cbSunVector[0] - state[i6];
         scToSunVec[1] = cbSunVector[1] - state[i6 + 1];
         scToSunVec[2] = cbSunVector[2] - state[i6 + 2];
         Real scToSunVecMag = scToSunVec.GetMagnitude();

         Rvector3 cbSunVelVec;
         if (!bodyIsTheSun)
         {
            percentSun = shadowState->FindShadowState(inSunlight, inShadow,
               shModel, &state[i6], cbSunVector, sunSat, forceVector,
               sunRadius, bodyRadius, psunrad, appSunRad, appBodyRad,
               appDistFromSunToBody);

            cbSunVelVec[0] = sunrv[3] - cbrv[3];
            cbSunVelVec[1] = sunrv[4] - cbrv[4];
            cbSunVelVec[2] = sunrv[5] - cbrv[5];
         }
         else
         {
            percentSun = 1.0;
            inSunlight = true;
            inShadow = false;

            cbSunVelVec[0] = 0.0;
            cbSunVelVec[1] = 0.0;
            cbSunVelVec[2] = 0.0;
         }

         // Currently it is assumed the occulting body and the central body
         // are considered the same.  Should this change, the difference
         // between the occulting body's velocity and central body's velocity
         // will have to be taken here
         Rvector3 cbOccVelVec;
         cbOccVelVec[0] = 0.0;
         cbOccVelVec[1] = 0.0;
         cbOccVelVec[2] = 0.0;

         Real dpdt = 0;

         // If this is a penumbra or antumbra case, dp/dt will not be zero and
         // requires further calculations
         if (percentSun != 0.0 && percentSun != 1.0)
         {
            Rvector3 bodyToSC;
            bodyToSC[0] = state[0];
            bodyToSC[1] = state[1];
            bodyToSC[2] = state[2];
            Real bodyToSCMag = bodyToSC.GetMagnitude();

            // dR'_sun/dt
            Real dAppSunRaddt = -sunRadius / (pow(scToSunVecMag, 3) *
               GmatMathUtil::Sqrt(1 - pow(sunRadius, 2) /
               pow(scToSunVecMag, 2))) * (scToSunVec[0] * cbSunVelVec[0] +
               scToSunVec[1] * cbSunVelVec[1] + scToSunVec[2] * cbSunVelVec[2]);

            // dR'_body/dt
            Real dAppBodyRaddt = -bodyRadius / (pow(bodyToSCMag, 3) *
               GmatMathUtil::Sqrt(1 - pow(bodyRadius, 2) / pow(bodyToSCMag, 2))) *
               (bodyToSC[0] * -cbOccVelVec[0] + bodyToSC[1] * -cbOccVelVec[1] +
               bodyToSC[2] * -cbOccVelVec[2]);

            if ((GmatMathUtil::Abs(appSunRad - appBodyRad) <
               appDistFromSunToBody) && (appDistFromSunToBody <
               (appSunRad + appBodyRad)))
            {
               // Penumbra case
               Real c1 = (appDistFromSunToBody * appDistFromSunToBody +
                  appSunRad * appSunRad - appBodyRad * appBodyRad) /
                  (2.0 * appDistFromSunToBody);
               Real c2 = sqrt(appSunRad * appSunRad - c1 * c1);

               Real overlapA = appSunRad * appSunRad *
                  GmatMathUtil::ACos(c1 / appSunRad) + appBodyRad * appBodyRad *
                  GmatMathUtil::ACos((appDistFromSunToBody - c1) / appBodyRad) -
                  appDistFromSunToBody * c2;

               // s x d
               Rvector3 scrossd;
               scrossd[0] = bodyToSC[1] * scToSunVec[2] -
                  bodyToSC[2] * scToSunVec[1];
               scrossd[1] = bodyToSC[2] * scToSunVec[0] -
                  bodyToSC[0] * scToSunVec[2];
               scrossd[2] = bodyToSC[0] * scToSunVec[1] -
                  bodyToSC[1] * scToSunVec[0];
               Real scrossdMag = scrossd.GetMagnitude();

               // d(s x d)/dt
               Rvector3 dscrossddt;
               dscrossddt[0] = -cbOccVelVec[1] * scToSunVec[2] +
                  bodyToSC[1] * cbSunVelVec[2] + cbOccVelVec[2] *
                  scToSunVec[1] - bodyToSC[2] * cbSunVelVec[1];
               dscrossddt[1] = -cbOccVelVec[2] * scToSunVec[0] +
                  bodyToSC[2] * cbSunVelVec[0] + cbOccVelVec[0] *
                  scToSunVec[2] - bodyToSC[0] * cbSunVelVec[2];
               dscrossddt[2] = -cbOccVelVec[0] * scToSunVec[1] +
                  bodyToSC[0] * cbSunVelVec[1] + cbOccVelVec[1] *
                  scToSunVec[0] - bodyToSC[1] * cbSunVelVec[0];

               // d||s x d||/dt
               Real dcrossmagdt = (scrossd[0] * dscrossddt[0] +
                  scrossd[1] * dscrossddt[1] + scrossd[2] * dscrossddt[2]) /
                  scrossdMag;

               // s_transpose * d
               Real sTimesD = bodyToSC[0] * scToSunVec[0] +
                  bodyToSC[1] * scToSunVec[1] + bodyToSC[2] * scToSunVec[2];

               // d(s_transpose * d)/dt
               Real dsTimesDdt = bodyToSC[0] * cbSunVelVec[0] +
                  bodyToSC[1] * cbSunVelVec[1] + bodyToSC[2] * cbSunVelVec[2] +
                  scToSunVec[0] * -cbOccVelVec[0] + scToSunVec[1] *
                  -cbOccVelVec[1] + scToSunVec[2] * -cbOccVelVec[2];

               // dD'/dt
               Real dAppDistdt = 1.0 / (pow(scrossdMag, 2) + pow(-sTimesD, 2)) *
                  (-sTimesD * dcrossmagdt - scrossdMag * -dsTimesDdt);

               // dc1/dt
               Real dc1dt = 0.5 * (-1.0 / pow(appDistFromSunToBody, 2) *
                  (pow(appDistFromSunToBody, 2) + pow(appSunRad, 2) -
                  pow(appBodyRad, 2)) * dAppDistdt + 2.0 /
                  appDistFromSunToBody * (appDistFromSunToBody * dAppDistdt +
                  appSunRad * dAppSunRaddt - appBodyRad * dAppBodyRaddt));

               // dc2/dt
               Real dc2dt = 1.0 / c2 * (appSunRad * dAppSunRaddt - c1 * dc1dt);

               // d(c1/R'_sun)/dt
               Real dc1AppSunRaddt = 1.0 / appSunRad * dc1dt - c1 /
                  pow(appSunRad, 2) * dAppSunRaddt;

               // d(atan2[c2, D'-c1])/dt
               Real datan2dt = 1.0 / (pow(c2, 2) +
                  pow(appDistFromSunToBody - c1, 2)) *
                  ((appDistFromSunToBody - c1) * dc2dt -
                  c2 * (dAppDistdt - dc1dt));

               // dA/dt
               Real dAreadt = 2.0 * appSunRad * acos(c1 / appSunRad) *
                  dAppSunRaddt - pow(appSunRad, 2) /
                  GmatMathUtil::Sqrt(1.0 - pow(c1, 2) / pow(appSunRad, 2)) *
                  dc1AppSunRaddt + 2.0 * appBodyRad *
                  atan2(c2, appDistFromSunToBody - c1) * dAppBodyRaddt +
                  pow(appBodyRad, 2) * datan2dt - appDistFromSunToBody *
                  dc2dt - c2 * dAppDistdt;

               // dp/dt
               dpdt = 100.0 / GmatMathConstants::PI * (1.0 / pow(appSunRad, 2) *
                  dAreadt - 2.0 * overlapA / (pow(appSunRad, 3)) *
                  dAppSunRaddt);
            }
            else
            {
               // Antumbra case

               // dp/dt
               dpdt = 100.0 * (2.0 * appBodyRad / pow(appSunRad, 2) *
                  dAppBodyRaddt - 2.0 * pow(appBodyRad, 2) /
                  pow(appSunRad, 3) * dAppSunRaddt);
            }
         }

         Rvector3 sunToSCVec = -scToSunVec;
         Real sunToSCVecMag = sunToSCVec.GetMagnitude();
         Real elementCoef = fluxPressure * pow(149597871.0, 2) * cr[i] * area[i] /
            (mass[i] * pow(sunToSCVecMag, 3));

         for (Integer i = 0; i < stmRowCount; ++i)
            timeJacobian[i] = 0.0;
         
         // Calculate the SRP time Jacobian
         Real timeJacElement;
         for (Integer ii = 0; ii < 3; ++ii)
         {
            for (Integer jj = 0; jj < 3; ++jj)
            {
               if (ii == jj)
               {
                  timeJacElement = 1.0 - 3.0 * sunToSCVec[ii] *
                     sunToSCVec[jj] / pow(sunToSCVecMag, 2);
               }
               else
               {
                  timeJacElement = -3.0 * sunToSCVec[ii] *
                     sunToSCVec[jj] / pow(sunToSCVecMag, 2);
               }

               timeJacobian[i6 + ii + 3] += percentSun *
                  elementCoef * timeJacElement * -cbSunVelVec[jj] / 1000.0;
            }
            timeJacobian[i6 + ii + 3] += elementCoef * sunToSCVec[ii] * -dpdt /
               100000.0;
         }
      }
   }

    
   #ifdef DEBUG_SOLAR_RADIATION_PRESSURE    
      MessageInterface::ShowMessage(
         "SRP Parameters:\n   SunVec = [%16le %16le %16le]\n   posVec = "
         "[%16le %16le %16le]\n", cbSunVector[0], cbSunVector[1], 
         cbSunVector[2], state[0], state[1], state[2]);
      MessageInterface::ShowMessage(
         "   epoch = %16le\n   nomSun = %16le\n   sunDist = %16le\n   %16le percent\n",
         ep, nominalSun, sunDistance, percentSun);
   #endif
   
   #ifdef DEBUG_SOLAR_RADIATION_PRESSURE_TIMESHADOW    
      MessageInterface::ShowMessage("   %16.12le      %16.12le\n", ep, percentSun);
   #endif

   #ifdef DEBUG_SRP_ORIGIN
      if (showData)
      {
         MessageInterface::ShowMessage(
            "SRP Parameters at %s:\n   SunVec = [%16le %16le %16le]\n   posVec"
            " = [%16le %16le %16le]\n", body->GetName().c_str(), cbSunVector[0], 
            cbSunVector[1], cbSunVector[2], state[0], state[1], state[2]);
         MessageInterface::ShowMessage(
            "   epoch = %16le\n   nomSun = %16le\n   sunDist = %16le\n   "
            "percent = %16le\n   cr = %16le\n   fluxP = %16le\n   area = %16le"
            "\n   mass = %16le\n   distFac = %16le\n   forceVector = [%16le "
            "%16le %16le]\n   magnitude = %16le\n\n", ep, nominalSun, 
            sunDistance, percentSun, cr[0], fluxPressure, area[0], mass[0], 
            distancefactor, forceVector[0], forceVector[1], forceVector[2], 
            mag);
      }
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
/**
 * Retrieves the Cartesian state vector of derivatives w.r.t. time
 *
 * @param sc The spacecraft that holds the state vector
 *
 * @return The derivative vector
 */
//------------------------------------------------------------------------------
Rvector6 SolarRadiationPressure::GetDerivativesForSpacecraft(Spacecraft *sc)
{
   Rvector6 dv;

   if (!theSun)
      throw ODEModelException("The Sun is not set in SRP::GetDerivatives");

   if (!body)
      throw ODEModelException(
         "The central body is not set in SRP::GetDerivatives");

   if (!cbSunVector)
      throw ODEModelException(
         "The sun vector is not initialized in SRP::GetDerivatives");

   //Real cr, area, mass;
   Rvector cr;
   Real area, mass;

   Real distancefactor = 1.0, mag;
   bool inSunlight = true, inShadow = false;

   // if (srpShapeModel == "Spherical")                         // made changes by TUAN NGUYEN
   if (srpShapeModelIndex == ShapeModel::SPHERICAL_MODEL)       // made changes by TUAN NGUYEN
   {
      cr.SetSize(1);                                            // made changes by TUAN NGUYEN
      cr[0] = sc->GetRealParameter("Cr");                       // made changes by TUAN NGUYEN
   }
   // else if (srpShapeModel == "SPADFile")                     // made changes by TUAN NGUYEN
   else if (srpShapeModelIndex == ShapeModel::SPAD_FILE_MODEL)  // made changes by TUAN NGUYEN
   {
      cr.SetSize(1);                                            // made changes by TUAN NGUYEN
      cr[0] = sc->GetRealParameter("SPADSRPScaleFactor");       // made changes by TUAN NGUYEN
   }
   // else if (srpShapeModel == "NPlate")                       // made changes by TUAN NGUYEN
   else if (srpShapeModelIndex == ShapeModel::NPLATE_MODEL)     // made changes by TUAN NGUYEN
   {
      // do nothing here
   }
   else                                                         // made changes by TUAN NGUYEN
      throw ODEModelException("Error: SRP model is not specified in SolarRadiationPressure object. The value should be Spherical, SPADFile, or NPlate.\n");  // made changes by TUAN NGUYEN

   area = sc->GetRealParameter("SRPArea");
   mass = sc->GetRealParameter("TotalMass");

   Real ep = sc->GetEpoch();
   GmatTime epGT = sc->GetEpochGT();
   if (hasPrecisionTime)
      sunrv = theSun->GetState(epGT);
   else
      sunrv = theSun->GetState(ep);

   Real *j2kState = sc->GetState().GetState();
   Real state[6];

   if (hasPrecisionTime)
      BuildModelStateGT(epGT, state, j2kState);
   else
      BuildModelState(ep, state, j2kState);

   // Rvector6 is initialized to all 0.0's; only change it if the body is not
   // the Sun
   if (!bodyIsTheSun)
   {
      if (hasPrecisionTime)
         cbrv = body->GetState(epGT);
      else
         cbrv = body->GetState(ep);

      cbSunVector[0] = sunrv[0] - cbrv[0];
      cbSunVector[1] = sunrv[1] - cbrv[1];
      cbSunVector[2] = sunrv[2] - cbrv[2];
   }
   else
   {
      cbSunVector[0] = 0.0;
      cbSunVector[1] = 0.0;
      cbSunVector[2] = 0.0;
   }

   Real sunSat[3];

   // Build vector from the Sun to the current spacecraft
   sunSat[0] = state[0] - cbSunVector[0];
   sunSat[1] = state[1] - cbSunVector[1];
   sunSat[2] = state[2] - cbSunVector[2];
   sunDistance = sqrt(sunSat[0]*sunSat[0] + sunSat[1]*sunSat[1] +
                     sunSat[2]*sunSat[2]);
   if (sunDistance == 0.0)
      sunDistance = 1.0;
   // Make a unit vector for the force direction
   forceVector[0] = sunSat[0] / sunDistance;
   forceVector[1] = sunSat[1] / sunDistance;
   forceVector[2] = sunSat[2] / sunDistance;

   distancefactor = nominalSun / sunDistance;                 // It has no unit (Km/Km)
   distancefactor *= distancefactor;                          // made changes by TUAN NGUYEN

   // Test shadow condition for current spacecraft (only if body isn't Sol)
   if (!bodyIsTheSun)
   {
      psunrad = asin(sunRadius / sunDistance);
      std::string shModel = "DualCone";
      Real appSunRad, appBodyRad, appDistFromSunToBody;
      if (shadowModel == CYLINDRICAL_MODEL) shModel = "Cylindrical";
      percentSun = shadowState->FindShadowState(inSunlight, inShadow,
            shModel, state, cbSunVector, sunSat, forceVector,
            sunRadius, bodyRadius, psunrad, appSunRad, appBodyRad,
            appDistFromSunToBody);
//      FindShadowState(inSunlight, inShadow, state);
   }
   else
   {
       inSunlight= true;
       inShadow = false;
       percentSun = 1.0;
   }

   Rvector3 spadArea;

   // It is spacecraft's reflectance vector in N-paltes model              // made changes by TUAN NGUYEN
   Rvector3 nplatesReflectance;                                            // made changes by TUAN NGUYEN

   if (!inShadow)
   {
      // if (srpShapeModel == "Spherical")                                 // made changes by TUAN NGUYEN
      if (srpShapeModelIndex == ShapeModel::SPHERICAL_MODEL)               // made changes by TUAN NGUYEN
      {
         // Montenbruck and Gill, eq. 3.75
         mag = percentSun * cr[0] * fluxPressure * area * distancefactor /              // made changes by TUAN NGUYEN
                             mass;                                         // its unit is (N/m^2)(m^2)/Kg = m/(s^2)     // made changes by TUAN NGUYEN
         mag = mag * GmatMathConstants::M_TO_KM;                           // convert m/(s^2) to Km/(s^2)               // made changes by TUAN NGUYEN

         dv[0] = dv[1] = dv[2] = 0.0;
         dv[3] = mag * forceVector[0];
         dv[4] = mag * forceVector[1];
         dv[5] = mag * forceVector[2];
      }
      // else if (srpShapeModel == "SPADFile")                     // SPADFile    // made changes by TUAN NGUYEN
      else if (srpShapeModelIndex == ShapeModel::SPAD_FILE_MODEL)  // SPADFile    // made changes by TUAN NGUYEN
      {
         // Montenbruck and Gill, eq. 3.75
         mag = percentSun * fluxPressure * distancefactor /
                             mass;                                      // its unit is (N/m^2)/Kg = (1/m^2).(m/s^2)    // made changes by TUAN NGUYEN
         mag = mag * GmatMathConstants::M_TO_KM;                        // convert (1/m^2).(m/s^2) to (1/m^2).(Km/s^2) // made changes by TUAN NGUYEN

         #ifdef DEBUG_SPAD_DATA
         MessageInterface::ShowMessage(" \n------> now using spad data (per SC) for %s\n",
               sc->GetName().c_str());
         MessageInterface::ShowMessage("   nominalSun     = %12.10f\n", nominalSun);
         MessageInterface::ShowMessage("   sunDistance    = %12.10f\n", sunDistance);
         MessageInterface::ShowMessage("   state          = %12.10f  %12.10f  %12.10f\n",
               state[0], state[1], state[2]);
         MessageInterface::ShowMessage("   cbSunVector    = %12.10f  %12.10f  %12.10f\n",
                cbSunVector[0], cbSunVector[1], cbSunVector[2]);
         MessageInterface::ShowMessage("   sunSat    = %12.10f  %12.10f  %12.10f\n",
               sunSat[0], sunSat[1], sunSat[2]);
         MessageInterface::ShowMessage("   percentSun     = %12.10f\n", percentSun);
         MessageInterface::ShowMessage("   fluxPressure   = %12.10f\n", fluxPressure);
         MessageInterface::ShowMessage("   distancefactor = %12.10f\n", distancefactor);
         MessageInterface::ShowMessage("   mass           = %12.10f\n", mass);
         #endif
         Rvector3 sunSC(sunSat[0],sunSat[1],sunSat[2]);
         if (hasPrecisionTime)
            spadArea = sc->GetSPADSRPArea(epGT.GetMjd(), sunSC);
         else
            spadArea = sc->GetSPADSRPArea(ep,sunSC);

         #ifdef DEBUG_SPAD_DATA
            MessageInterface::ShowMessage("in SRP, SPAD area (per SC) from file = %12.10f  %12.10f  %12.10f\n",
                  spadArea[0],spadArea[1],spadArea[2]);
         #endif
         dv[0] = dv[1] = dv[2] = 0.0;
         dv[3] = mag * spadArea[0];         // Its unit is (1/m^2).(Km/s^2).(m^2) = Km/s^2 
         dv[4] = mag * spadArea[1];         // Its unit is (1/m^2).(Km/s^2).(m^2) = Km/s^2
         dv[5] = mag * spadArea[2];         // Its unit is (1/m^2).(Km/s^2).(m^2) = Km/s^2
      }
      // else if (srpShapeModel == "NPlate")                    // NPlate    // made changes by TUAN NGUYEN
      else if (srpShapeModelIndex == ShapeModel::NPLATE_MODEL)  // NPlate    // made changes by TUAN NGUYEN
      {
         // Eq.1 N-Plates SRP MathSpec: -K/mass 
         mag = percentSun * fluxPressure * distancefactor /
                             mass;                                      // its unit is (N/m^2)/Kg = (1/m^2).(m/s^2)    // made changes by TUAN NGUYEN
         mag = mag * GmatMathConstants::M_TO_KM;                        // convert (1/m^2).(m/s^2) to (1/m^2).(Km/s^2) // made changes by TUAN NGUYEN

         #ifdef DEBUG_NPLATE_DATA
         MessageInterface::ShowMessage(" \n------> now using N-plates data (per SC) for %s\n",
            sc->GetName().c_str());
         MessageInterface::ShowMessage("   nominalSun     = %12.10f\n", nominalSun);
         MessageInterface::ShowMessage("   sunDistance    = %12.10f\n", sunDistance);
         MessageInterface::ShowMessage("   state          = %12.10f  %12.10f  %12.10f\n",
            state[0], state[1], state[2]);
         MessageInterface::ShowMessage("   cbSunVector    = %12.10f  %12.10f  %12.10f\n",
            cbSunVector[0], cbSunVector[1], cbSunVector[2]);
         MessageInterface::ShowMessage("   sunSat    = %12.10f  %12.10f  %12.10f\n",
            sunSat[0], sunSat[1], sunSat[2]);
         MessageInterface::ShowMessage("   percentSun     = %12.10f\n", percentSun);
         MessageInterface::ShowMessage("   fluxPressure   = %12.10f\n", fluxPressure);
         MessageInterface::ShowMessage("   distancefactor = %12.10f\n", distancefactor);
         MessageInterface::ShowMessage("   mass           = %12.10f\n", mass);
         #endif
         Rvector3 sunSC(sunSat[0], sunSat[1], sunSat[2]);
         Rvector6 scState(state[0], state[1], state[2], state[3], state[4], state[5]);

         if (hasPrecisionTime)
            nplatesReflectance = sc->GetNPlateSRPReflectance(sunSC, epGT);                    // made changes by TUAN NGUYEN
         else
         {
            GmatTime gtep4(ep);
            nplatesReflectance = sc->GetNPlateSRPReflectance(sunSC, gtep4);            // made changes by TUAN NGUYEN
         }

         #ifdef DEBUG_NPLATE_DATA
         MessageInterface::ShowMessage("in SRP, N-Plates reflectance (per SC) from file = %12.10f  %12.10f  %12.10f\n",
            nplatesReflectance[0], nplatesReflectance[1], nplatesReflectance[2]);
         #endif

         dv[0] = dv[1] = dv[2] = 0.0;
         // Eq.1. in SRP N-Plates MathSpec. Spacecraft's reflectance vector has opposite deriection to SRP force. Therefore, -mag is used instead of mag.
         dv[3] = (-mag) * nplatesReflectance[0];         // Its unit is (1/m^2).(Km/s^2).(m^2) = Km/s^2 
         dv[4] = (-mag) * nplatesReflectance[1];         // Its unit is (1/m^2).(Km/s^2).(m^2) = Km/s^2
         dv[5] = (-mag) * nplatesReflectance[2];         // Its unit is (1/m^2).(Km/s^2).(m^2) = Km/s^2
      }
      else                                  // made changes by TUAN NGUYEN
      {
         throw ODEModelException("Error: SolarRadiationPressure SRP Model was not specified. It needs to set to Spherical, SPADFile, or NPlate.\n");  // made changes by TUAN NGUYEN
      }
   }
   else
   {
      dv[0] = dv[1] = dv[2] =
      dv[3] = dv[4] = dv[5] = 0.0;
   }

   return dv;
}


////------------------------------------------------------------------------------
//// void FindShadowState(bool &lit, bool &dark, Real *state)
////------------------------------------------------------------------------------
///**
// * Determines lighting conditions at the input location
// *
// * @param <lit>   Indicates if the spoacecraft is in full sunlight
// * @param <dark>  Indicates if the spacecarft is in umbra
// * @param <state> Current spacecraft state
// *
// * \todo: Currently implemented for one spacecraft, state vector arranges as
// * (x, y, z)
// */
////------------------------------------------------------------------------------
//void SolarRadiationPressure::FindShadowState(bool &lit, bool &dark, Real *state)
//{
//    Real mag = sqrt(cbSunVector[0]*cbSunVector[0] +
//                      cbSunVector[1]*cbSunVector[1] +
//                      cbSunVector[2]*cbSunVector[2]);
//
//    Real unitsun[3];
//    unitsun[0] = cbSunVector[0] / mag;
//    unitsun[1] = cbSunVector[1] / mag;
//    unitsun[2] = cbSunVector[2] / mag;
//
//    double rdotsun = state[0]*unitsun[0] +
//                     state[1]*unitsun[1] +
//                     state[2]*unitsun[2];
//
//    if (rdotsun > 0.0) // Sunny side of central body is always fully lit
//    {
//        lit = true;
//        dark = false;
//        percentSun = 1.0;
//        return;
//    }
//
//    if (shadowModel == CYLINDRICAL_MODEL)
//    {
//        // In this model, the spacecraft is in darkness if it is within
//        // bodyRadius of the sun-body line; otherwise it is lit
//        Real rperp[3];
//        rperp[0] = state[0] - rdotsun * unitsun[0];
//        rperp[1] = state[1] - rdotsun * unitsun[1];
//        rperp[2] = state[2] - rdotsun * unitsun[2];
//
//        mag = sqrt(rperp[0]*rperp[0] + rperp[1]*rperp[1] + rperp[2]*rperp[2]);
//        if (mag < bodyRadius)
//        {
//            percentSun = 0.0;
//            lit = false;
//            dark = true;
//        }
//        else
//        {
//            percentSun = 1.0;
//            lit = true;
//            dark = false;
//        }
//    }
//    else if (shadowModel == CONICAL_MODEL)
//    {
//        Real s0, s2, lsc, l1, l2, c1, c2, sinf1, sinf2, tanf1, tanf2;
//
//        // Montenbruck and Gill, eq. 3.79
//        s0 = -state[0]*unitsun[0] - state[1]*unitsun[1] - state[2]*unitsun[2];
//        s2 = state[0]*state[0] + state[1]*state[1] + state[2]*state[2];
//
//        // Montenbruck and Gill, eq. 3.80
//        lsc = sqrt(s2 - s0*s0);
//
//        // Montenbruck and Gill, eq. 3.81
//        sinf1 = (sunRadius + bodyRadius) / mag;
//        sinf2 = (sunRadius - bodyRadius) / mag;
//
//        // Appropriate l1 and l2 temporarily
//        l1 = sinf1 * sinf1;
//        l2 = sinf2 * sinf2;
//        tanf1 = sqrt(l1 / (1.0 - l1));
//        tanf2 = sqrt(l2 / (1.0 - l2));
//
//        // Montenbruck and Gill, eq. 3.82
//        c1 = s0 + bodyRadius / sinf1;
//        c2 = bodyRadius / sinf2 - s0;       // Different sign from M&G
//
//        // Montenbruck and Gill, eq. 3.83
//        l1 = c1 * tanf1;
//        l2 = c2 * tanf2;
//
//        if (lsc > l1)
//        {
//            // Outside of the penumbral cone
//            lit = true;
//            dark = false;
//            percentSun = 1.0;
//            return;
//        }
//        else
//        {
//            lit = false;
//            if (lsc < fabs(l2))
//            {
//                // Inside umbral cone
//                if (c2 >= 0.0)
//                {
//                    // no annular ring
//                    percentSun = 0.0;
//                    dark = true;
//                }
//                else
//                {
//                    // annular eclipse
//                    pcbrad = asin(bodyRadius / sqrt(s2));
//                    percentSun = (psunrad*psunrad - pcbrad*pcbrad) /
//                                 (psunrad*psunrad);
//                    dark = false;
//                }
//
//                return;
//            }
//            // In penumbra
//            pcbrad = asin(bodyRadius / sqrt(s2));
//            percentSun = ShadowFunction(state);
//            lit = false;
//            dark = false;
//        }
//    }
//}


//------------------------------------------------------------------------------
// bool IsUnique(const std::string &forBody)
//------------------------------------------------------------------------------
/**
 * Specifies if the SRP force is the one and only SRP force allowed
 *
 * @param forBody The body at which the force must be unique (unused for SRP)
 *
 * @return true for SRP.
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::IsUnique(const std::string& forBody)
{
   return true;
}


////------------------------------------------------------------------------------
//// Real ShadowFunction(Real * state)
////------------------------------------------------------------------------------
///**
// * Calculates %lit when in penumbra.
// *
// * @param <state> The current spacecraft state
// *
// * @return the multiplier used when the satellite is partially lit
// */
////------------------------------------------------------------------------------
//Real SolarRadiationPressure::ShadowFunction(Real * state)
//{
//    Real mag = sqrt(state[0]*state[0] +
//                      state[1]*state[1] +
//                      state[2]*state[2]);
//
//    // Montenbruck and Gill, eq. 3.87
//    Real c = acos((state[0]*forceVector[0] +
//                     state[1]*forceVector[1] +
//                     state[2]*forceVector[2]) / mag);
//
//    Real a2 = psunrad*psunrad;
//    Real b2 = pcbrad*pcbrad;
//
//    // Montenbruck and Gill, eq. 3.93
//    Real x = (c*c + a2 - b2) / (2.0 * c);
//    Real y = sqrt(a2 - x*x);
//
//    // Montenbruck and Gill, eq. 3.92
//    Real area = a2*acos(x/psunrad) + b2*acos((c-x)/pcbrad) - c*y;
//
//    // Montenbruck and Gill, eq. 3.94
//    return 1.0 - area / (M_PI * a2);
//}

//---------------------------------------------------------------------------
// Rvector6 ComputeSPADAcceleration(Integer scID, Real ep,
//                                  Real *state, Real *cbSun)
//---------------------------------------------------------------------------
Rvector6 SolarRadiationPressure::ComputeSPADAcceleration(Integer scID,
                                 Real ep, Real *state, Real *cbSun)
{
   #ifdef DEBUG_SPAD_ACCEL
      MessageInterface::ShowMessage("Entering ComputeSPADAcceleration with inputs:\n");
      MessageInterface::ShowMessage("    scID           = %d\n", scID);
      MessageInterface::ShowMessage("    ep             = %12.16le\n", ep);
      MessageInterface::ShowMessage("    state          = %12.16le  %12.16le  %12.16le\n",
            state[0], state[1], state[2]);
      MessageInterface::ShowMessage("    cbSun          = %12.16le  %12.16le  %12.16le\n",
            cbSun[0], cbSun[1], cbSun[2]);
   #endif
   Rvector6 result;
   Rvector3 spadArea;
   Real sunSat[3];
   sunSat[0] = state[0] - cbSunVector[0];           // ???? it needs to change cbSunVector[0] to cbSun[0]
   sunSat[1] = state[1] - cbSunVector[1];           // ???? it needs to change cbSunVector[0] to cbSun[1]
   sunSat[2] = state[2] - cbSunVector[2];           // ???? it needs to change cbSunVector[0] to cbSun[2]
   Real sunDistance = sqrt(sunSat[0]*sunSat[0] + sunSat[1]*sunSat[1] +
                           sunSat[2]*sunSat[2]);
   if (sunDistance == 0.0)
      sunDistance = 1.0;

   Real distancefactor = nominalSun / sunDistance;            // It has no unit (Km/Km)
   distancefactor *= distancefactor;                                      // made changes by TUAN NGUYEN

   Real mag = percentSun * fluxPressure * distancefactor / mass[scID];    // Its unit is (N/m^2)/Kg = (1/m^2).(m/s^2)  // made changes by TUAN NGUYEN
   // Convert m/s^2 to km/s^2
   mag = mag  * GmatMathConstants::M_TO_KM;                               // Its unit is (1/m^2).(Km/s^2) // made changes by TUAN NGUYEN

   #ifdef DEBUG_SPAD_ACCEL
      MessageInterface::ShowMessage("--- Computed in ComputeSPADAcceleration:\n");
      MessageInterface::ShowMessage("    sunSat         = %12.16le  %12.16le  %12.16le\n",
            sunSat[0], sunSat[1], sunSat[2]);
      MessageInterface::ShowMessage("    sunDistance    = %12.16le\n",
            sunDistance);
      MessageInterface::ShowMessage("    percentSun     = %12.16le\n", percentSun);
      MessageInterface::ShowMessage("    fluxPressure   = %12.16le\n", fluxPressure);
      MessageInterface::ShowMessage("    distancefactor = %12.16le\n", distancefactor);
      MessageInterface::ShowMessage("    mass           = %12.16le\n", mass[scID]);
      MessageInterface::ShowMessage("    mag            = %12.16le\n", mag);
   #endif

   Rvector3 sunSC(sunSat[0], sunSat[1], sunSat[2]);  // need to modify for performance?
   spadArea = ((Spacecraft*) scObjs.at(scID))->GetSPADSRPArea(ep, sunSC);
   #ifdef DEBUG_SPAD_ACCEL
      MessageInterface::ShowMessage("    SPAD Area      = %12.16le  %12.16le  %12.16le\n",
            spadArea[0],spadArea[1],spadArea[2]);
   #endif

   result[0] = result[1] = result[2] = 0.0;
   result[3] = mag * spadArea[0];     // Its unit is (1/m^2).(Km/s^2).(m^2) = Km/s^2
   result[4] = mag * spadArea[1];     // Its unit is (1/m^2).(Km/s^2).(m^2) = Km/s^2
   result[5] = mag * spadArea[2];     // Its unit is (1/m^2).(Km/s^2).(m^2) = Km/s^2

   #ifdef DEBUG_SPAD_ACCEL
      MessageInterface::ShowMessage("--- AT END, result = %12.16le  %12.16le  %12.16le  %12.16le  %12.16le  %12.16le\n",
            result[0],result[1],result[2],result[3],result[4],result[5]);
   #endif

   return result;
}


//---------------------------------------------------------------------------
// Rvector6 ComputeNPlateAcceleration(Integer scID, Real ep,
//                                  Real *state, Real *cbSun)
//---------------------------------------------------------------------------
/**
* This function is use to compute spacecraft acceleration associate with 
* N-plates SRP force.
*
* @param scID     the ID of a spacecraft
* @param state    State of the spacecraft in central body MJ2000Eq coordinate system
* @param cbSun    the Sun's coordinate in central body MJ2000Eq coordinate system
* 
* @return         the last 3 elements in Rvector6 contains acceleration
*/
//---------------------------------------------------------------------------
Rvector6 SolarRadiationPressure::ComputeNPlateAcceleration(Integer scID, Real ep,
   Rvector6 state, Real *cbSun)
{
#ifdef DEBUG_NPLATE_ACCEL
   MessageInterface::ShowMessage("Entering ComputeNPlateAcceleration with inputs:\n");
   MessageInterface::ShowMessage("    scID           = %d\n", scID);
   MessageInterface::ShowMessage("    state          = %12.16le  %12.16le  %12.16le  %12.16le  %12.16le  %12.16le\n",
      state[0], state[1], state[2], state[3], state[4], state[5]);
   MessageInterface::ShowMessage("    cbSun          = %12.16le  %12.16le  %12.16le\n",
      cbSun[0], cbSun[1], cbSun[2]);
#endif
   Rvector6 result;
   Rvector3 nplatesReflectance;

   Rvector3 sunSC;
   sunSC[0] = state[0] - cbSun[0];
   sunSC[1] = state[1] - cbSun[1];
   sunSC[2] = state[2] - cbSun[2];

   Real sunDistance = sunSC.GetMagnitude();
   if (sunDistance == 0.0)
      sunDistance = 1.0;

   Real distancefactor = nominalSun / sunDistance;            // It has no unit (Km/Km)
   distancefactor *= distancefactor;

   Real mag = percentSun * fluxPressure * distancefactor / mass[scID];    // Its unit is (N/m^2)/Kg = (1/m^2).(m/s^2)
    // Convert m/s^2 to km/s^2
   mag = mag * GmatMathConstants::M_TO_KM;                               // Its unit is (1/m^2).(Km/s^2)

#ifdef DEBUG_NPLATE_ACCEL
   MessageInterface::ShowMessage("--- Computed in ComputeNPlateAcceleration:\n");
   MessageInterface::ShowMessage("    sunSC         = %12.16le  %12.16le  %12.16le\n",
      sunSC[0], sunSC[1], sunSC[2]);
   MessageInterface::ShowMessage("    sunDistance    = %12.16le\n",
      sunDistance);
   MessageInterface::ShowMessage("    percentSun     = %12.16le\n", percentSun);
   MessageInterface::ShowMessage("    fluxPressure   = %12.16le\n", fluxPressure);
   MessageInterface::ShowMessage("    distancefactor = %12.16le\n", distancefactor);
   MessageInterface::ShowMessage("    mass           = %12.16le\n", mass[scID]);
   MessageInterface::ShowMessage("    mag            = %12.16le\n", mag);
#endif
   
   GmatTime gtep5(ep);
   nplatesReflectance = ((Spacecraft*)scObjs.at(scID))->GetNPlateSRPReflectance(sunSC, gtep5);  // unit: m^2
#ifdef DEBUG_NPLATE_ACCEL
   MessageInterface::ShowMessage("    N-plates reflectance      = %12.16le  %12.16le  %12.16le\n",
      nplatesReflectance[0], nplatesReflectance[1], nplatesReflectance[2]);
#endif

   result[0] = result[1] = result[2] = 0.0;
   // Eq.1 in N-Plates SRP MathSpec
   result[3] = (-mag) * nplatesReflectance[0];     // Its unit is (1/m^2).(Km/s^2).(m^2) = Km/s^2
   result[4] = (-mag) * nplatesReflectance[1];     // Its unit is (1/m^2).(Km/s^2).(m^2) = Km/s^2
   result[5] = (-mag) * nplatesReflectance[2];     // Its unit is (1/m^2).(Km/s^2).(m^2) = Km/s^2

#ifdef DEBUG_SPAD_ACCEL
   MessageInterface::ShowMessage("--- AT END, result = %12.16le  %12.16le  %12.16le  %12.16le  %12.16le  %12.16le\n",
      result[0], result[1], result[2], result[3], result[4], result[5]);
#endif

   return result;
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves eumeration symbols of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return list of enumeration symbols
 */
//---------------------------------------------------------------------------
const StringArray& SolarRadiationPressure::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   switch (id)
   {
   case SRP_MODEL:
      enumStrings.clear();
      enumStrings.push_back("Spherical");
      enumStrings.push_back("SPADFile");
      enumStrings.push_back("NPlate");         // made changes by TUAN NGUYEN
      return enumStrings;
   default:
      return PhysicalModel::GetPropertyEnumStrings(id);
   }
}

//------------------------------------------------------------------------------
// void SetSatelliteParameter(const Integer i, const std::string parmName, 
//                            const Real parm)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft parameters to the SRP force.
 * 
 * @param i           ID for the spacecraft
 * @param parmName    name of the Spacecraft parameter 
 * @param parm        Parameter value
 * @param parmID      ID for the parameter
 */
//------------------------------------------------------------------------------
void SolarRadiationPressure::SetSatelliteParameter(const Integer i, 
                                          const std::string parmName, 
                                          const Real parm,
                                          const Integer parmID)
{
    unsigned parmNumber = (unsigned)(i);

    #ifdef DEBUG_SOLAR_RADIATION_PRESSURE
         std::stringstream msg;
         msg << "Setting satellite parameter " << parmName << " for Spacecraft " 
             << i << " to " << parm << "\n";
         MessageInterface::ShowMessage(msg.str());
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

    if (parmName == "Cr")
    {
       // if ((srpShapeModel == "Spherical")|| (srpShapeModel == "NPlate"))                                            // made changes by TUAN NGUYEN
       if ((srpShapeModelIndex == ShapeModel::SPHERICAL_MODEL) || (srpShapeModelIndex == ShapeModel::NPLATE_MODEL))    // made changes by TUAN NGUYEN
       {
          if (parmNumber < cr.size())
             cr[i] = parm;
          else
             cr.push_back(parm);

          if (parmID >= 0)
             crID = parmID;
       }
    }

    if (parmName == "SPADSRPScaleFactor")          // made changes by TUAN NGUYEN
    {                                              // made changes by TUAN NGUYEN
       // if (srpShapeModel == "SPADFile")                               // made changes by TUAN NGUYEN
       if (srpShapeModelIndex == ShapeModel::SPAD_FILE_MODEL)            // made changes by TUAN NGUYEN
       {                                           // made changes by TUAN NGUYEN
          if (parmNumber < cr.size())              // made changes by TUAN NGUYEN
             cr[i] = parm;                         // made changes by TUAN NGUYEN
          else                                     // made changes by TUAN NGUYEN
             cr.push_back(parm);                   // made changes by TUAN NGUYEN

          if (parmID >= 0)                         // made changes by TUAN NGUYEN
             crID = parmID;                        // made changes by TUAN NGUYEN
       }                                           // made changes by TUAN NGUYEN
    }                                              // made changes by TUAN NGUYEN

    if (parmName == "SRPArea")
    {
       if (parmNumber < area.size())
            area[i] = parm;
        else
            area.push_back(parm);
        if (parmID >= 0)
            areaID = parmID;
    }
    if (parmName == "Cr_Epsilon")                  // made changes by TUAN NGUYEN
    {
        if (parmNumber < crEpsilon.size())
            crEpsilon[i] = parm;
        else
            crEpsilon.push_back(parm);
        if (parmID >= 0)
           crEpsilonID = parmID;
    }
}


//------------------------------------------------------------------------------
// void SetSatelliteParameter(const Integer i, Integer parmID, const Real parm)
//------------------------------------------------------------------------------
/**
 * Sets the parameters for a specific Spacecraft
 *
 * @param i The Spacecraft's index
 * @param parmID The ID of the parameter that is being set
 * @param parm The new parameter value
 */
//------------------------------------------------------------------------------
void SolarRadiationPressure::SetSatelliteParameter(const Integer i,
                                   Integer parmID, const Real parm)
{
   unsigned parmNumber = (unsigned)(i);

   #ifdef DEBUG_SOLAR_RADIATION_PRESSURE
        std::stringstream msg;
        msg << "Setting satellite parameter " << parmID << " for Spacecraft "
            << i << " to " << parm << "\n";
        MessageInterface::ShowMessage(msg.str());
   #endif

   if (parmID == massID)
   {
       if (parmNumber < mass.size())
           mass[i] = parm;
       else
           mass.push_back(parm);
   }
   if (parmID == crID)
   {
      if (parmNumber < cr.size())
         cr[i] = parm;
      else
         cr.push_back(parm);
   }
   if (parmID == areaID)
   {
      if (parmNumber < area.size())
         area[i] = parm;
      else
         area.push_back(parm);
   }
   if (parmID == crEpsilonID)
   {
      if (parmNumber < crEpsilon.size())
         crEpsilon[i] = parm;
      else
         crEpsilon.push_back(parm);
   }
}


//------------------------------------------------------------------------------
// void ClearSatelliteParameters(const std::string parmName)
//------------------------------------------------------------------------------
/**
 * Resets the PhysicalModel to receive a new set of satellite parameters.
 *
 * @param parmName name of the Spacecraft parameter.  The empty string clear all
 *                 of the satellite parameters for the PhysicalModel.
 */
//------------------------------------------------------------------------------
void SolarRadiationPressure::ClearSatelliteParameters(
                                                   const std::string parmName)
{
   #ifdef DEBUG_SOLAR_RADIATION_PRESSURE
      std::stringstream msg;
      msg << "Clearing satellite parameter " << parmName << "\n";
      MessageInterface::ShowMessage(msg.str());
   #endif
   if ((parmName == "Mass") || (parmName == ""))
      mass.clear();

   if ((parmName == "Cr") || (parmName == "SPADSRPScaleFactor") || (parmName == ""))   // made changes by TUAN NGUYEN
      cr.clear();
   
   if ((parmName == "Cr_Epsilon") || (parmName == ""))     // made changes by TUAN NGUYEN
      crEpsilon.clear();
   if ((parmName == "SRPArea") || (parmName == ""))
      area.clear();
   if ((parmName == "scObjs")  || (parmName == ""))
      scObjs.clear();
}

////////------------------------------------------------------------------------------
//////// void PhysicalModel::SetSpaceObject(const Integer i, GmatBase *obj)
////////------------------------------------------------------------------------------
///////**
////// * Passes spacecraft pointers to the force model.
////// *
////// * @param i   ID for the spacecraft
////// * @param obj pointer to the Spacecraft
////// */
////////------------------------------------------------------------------------------
//////void SolarRadiationPressure::SetSpaceObject(const Integer i, GmatBase *obj)
//////{
//////   unsigned parmNumber = (unsigned)(i);
//////
//////   if (parmNumber < scObjs.size())
//////      scObjs[i] = obj;
//////   else
//////      scObjs.push_back(obj);
//////}

//------------------------------------------------------------------------------
// bool SolarRadiationPressure::AttitudeAffectsDynamics()
//------------------------------------------------------------------------------
/**
 * Detects if a the SolarRadiationPressure is dependant on its attitude
 *
 * @return true if the model's dynamics are affected by its attitude,
 *         false if it does not
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::AttitudeAffectsDynamics()
{
   bool retValue = false;                                                   // made changes by TUAN NGUYEN
   // if ((srpShapeModel == "SPADFile") || (srpShapeModel == "NPlate"))                                                // made changes by TUAN NGUYEN
   if ((srpShapeModelIndex == ShapeModel::SPAD_FILE_MODEL) || (srpShapeModelIndex == ShapeModel::NPLATE_MODEL))        // made changes by TUAN NGUYEN
      retValue = true;                                                      // made changes by TUAN NGUYEN

   return retValue;                                                         // made changes by TUAN NGUYEN
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
bool SolarRadiationPressure::SupportsDerivative(Gmat::StateElementId id)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage(
            "SolarRadiationPressure checking for support for id %d\n", id);
   #endif
      
   if (id == Gmat::CARTESIAN_STATE)
      return true;
   
   if (id == Gmat::ORBIT_STATE_TRANSITION_MATRIX)
   {
//      if (srpShapeModel == "Spherical")
//      {
//         MessageInterface::ShowMessage("Warning: The orbit state transition "
//               "matrix does not currently contain SRP contributions from shadow "
//               "partial derivatives when using Spherical SRP.\n");
//      }
      return true;
   }
   
   if (id == Gmat::ORBIT_A_MATRIX)
      return true;

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
 * @param id           State Element ID for the derivative type
 * @param index        Starting index in the state vector for this type of derivative
 * @param quantity     Number of objects that supply this type of data
 * @param totalSize    For sizable types, the size to use.  For example, for STM,
 *                     this is the number of elements of STM                         // made changes by TUAN NGUYEN
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::SetStart(Gmat::StateElementId id, Integer index, 
                        Integer quantity, Integer totalSize)                       // made changes by TUAN NGUYEN
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage("SolarRadiationPressure setting start data "
            "for id = %d to index %d; %d objects identified\n", id, index, 
            quantity);
   #endif
   
   bool retval = false;
   
   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
         satCount = quantity;
         cartesianCount = quantity;
         cartesianStart = index;
         fillCartesian = true;
         retval = true;
         break;
         
      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
         stmCount = quantity;
         stmStart = index;
         fillSTM = true;
         totalSTMSize = totalSize;                         // made changes by TUAN NGUYEN
         retval = true;
         break;
         
      case Gmat::ORBIT_A_MATRIX:
         aMatrixCount = quantity;
         aMatrixStart = index;
         fillAMatrix = true;
         totalSTMSize = totalSize;                         // made changes by TUAN NGUYEN
         retval = true;
         break;

      default:
         break;
   }
   
   return retval;
}


