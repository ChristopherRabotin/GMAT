//$Id$
//------------------------------------------------------------------------------
//                              SolarRadiationPressure
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
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
// SolarRadiationPressure::SolarRadiationPressure(const std::string &name)
//------------------------------------------------------------------------------
/**
 * 
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
   flux                (1367.0),                  // W/m^2, IERS 1996
   fluxPressure        (flux / GmatPhysicalConstants::c),   // converted to N/m^2
   srpModel            ("Spherical"),
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
   estimatingCr        (false),
   crEpsilonID         (-1),
   crEpsilonRow        (-1)
{
   parameterCount = SRPParamCount;
   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   objectTypeNames.push_back("SolarRadiationPressure");

   shadowState = new ShadowState();
}

//------------------------------------------------------------------------------
// SolarRadiationPressure(SolarRadiationPressure &srp)
//------------------------------------------------------------------------------
/**
 * 
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
   cbSunVector         (srp.cbSunVector),
   forceVector         (srp.forceVector),
   sunRadius           (srp.sunRadius),
   hasMoons            (srp.hasMoons),
   cr                  (srp.cr),
   area                (srp.area),
   mass                (srp.mass),
   flux                (srp.flux),           // W/m^2, IERS 1996
   fluxPressure        (srp.fluxPressure),   // converted to N/m^2
   srpModel            (srp.srpModel),
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
   estimatingCr        (srp.estimatingCr),
   crEpsilonID         (srp.crEpsilonID),
   crEpsilonRow        (srp.crEpsilonRow),
   crInitial           (srp.crInitial)
{
   parameterCount = SRPParamCount;
   shadowState = new ShadowState();
}

//------------------------------------------------------------------------------
// SolarRadiationPressure& operator=(SolarRadiationPressure &srp)
//------------------------------------------------------------------------------
/**
 * 
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
      cbSunVector  = srp.cbSunVector;
      forceVector  = srp.forceVector;
      sunRadius    = srp.sunRadius;
      hasMoons     = srp.hasMoons;
      cr           = srp.cr;
      area         = srp.area;
      mass         = srp.mass;
      scObjs       = srp.scObjs;
      flux         = srp.flux;           // W/m^2, IERS 1996
      fluxPressure = srp.fluxPressure;   // converted to N/m^2
      srpModel     = srp.srpModel;
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

      estimatingCr = srp.estimatingCr;
      crEpsilonID  = srp.crEpsilonID;
      crEpsilonRow = srp.crEpsilonRow;
      crInitial    = srp.crInitial;

      if (shadowState)
      {
         delete shadowState;
         shadowState        = NULL;
      }

      shadowState           = new ShadowState();
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
   if (id == SRP_MODEL)  return srpModel;

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
      if ((value != "Spherical") && (value != "SPADFile"))
      {
         ODEModelException odee("");
         odee.SetDetails(errorMessageFormat.c_str(),
                        value.c_str(),
                        "SrpModel", "\"Spherical\" or \"SPADFile\"");
         throw odee;
      }
      srpModel = value;
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
      theSun    = solarSystem->GetBody(SolarSystem::SUN_NAME);
      sunRadius = theSun->GetEquatorialRadius();
      
      if (!theSun)
         throw ODEModelException("Solar system does not contain the Sun for SRP force.");

      // Set the body to the force model origin, or Earth if origin isn't set
      if (forceOrigin)
         body = forceOrigin;
      else
         body = solarSystem->GetBody(SolarSystem::EARTH_NAME);

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
   
   theSun = solarSystem->GetBody(SolarSystem::SUN_NAME);
   
   if (!theSun)
      throw ODEModelException("Solar system does not contain the Sun for SRP force.");
   
   // Set the body to the force model origin, or Earth if origin isn't set
   if (forceOrigin)
      body = forceOrigin;
   else
      body = solarSystem->GetBody(SolarSystem::EARTH_NAME);

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

   Real ep = epoch + dt / GmatTimeConstants::SECS_PER_DAY;
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

   Integer i6, ix;
   Real sunSat[3];
   Rvector3 spadArea;
    
   if (fillCartesian)
   {
      for (Integer i = 0; i < satCount; ++i) 
      {
         #ifdef DEBUG_DERIVATIVE
            MessageInterface::ShowMessage(
                  "Filling Cartesian state for spacecraft %d\n", i);
         #endif
         i6 = cartesianStart + i*6;

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

         // Convert m/s^2 to km/s^2
         //distancefactor *= distancefactor * GmatMathConstants::M_TO_KM;
         distancefactor *= distancefactor;

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
                     sunRadius, bodyRadius, psunrad);
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
            
            if (srpModel == "Spherical")
            {
               #ifdef DEBUG_CR_UPDATES
                  MessageInterface::ShowMessage("SRP using Cr = %.12lf\n", cr[i]);
               #endif
               mag *= cr[i] * area[i];                                // Its current unit is [1/(m*s^2)]*(m^2) = m/s^2
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
            else  // SPADFile
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
         }
         else 
         {
            deriv[i6] = deriv[i6 + 1] = deriv[i6 + 2] = 
            deriv[i6 + 3] = deriv[i6 + 4] = deriv[i6 + 5] = 0.0;
         }
      }
   }

   if (fillSTM || fillAMatrix)
   {

      if (warnSRPMath && (percentSun < 1.0))
      {
         if (srpModel == "Spherical")
         {
            MessageInterface::ShowMessage("Warning: The orbit state transition "
                  "matrix does not currently contain SRP contributions from shadow "
                  "partial derivatives when using Spherical SRP.\n");

            warnSRPMath = false;
         }
      }

      Integer stmSize = stmRowCount * stmRowCount;
      Real *aTilde;
      aTilde = new Real[stmSize];

      Integer index = psm->GetSTMIndex(crID);
      if (index >= 0)
      {
         estimatingCr = true;
         crEpsilonRow = index;
      }

      Integer associate, element;
      for (Integer i = 0; i < stmCount; ++i)
      {
         #ifdef DEBUG_STM_MATRIX
            MessageInterface::ShowMessage("Filling STM for spacecraft %d\n", i);
         #endif
         Integer iStart = stmStart + i * stmRowCount*stmRowCount;
         associate = theState->GetAssociateIndex(iStart);

         // Calculate A-tilde
         for (Integer j = 0; j < stmRowCount; ++j)
         {
            ix = j * stmRowCount;
            for (Integer k = 0; k < stmRowCount; ++k)
               aTilde[ix+k] = 0.0;
         }

         // Build vector from Sun to the current spacecraft; (-s in math spec)
         sunSat[0] = state[ associate ] - cbSunVector[0];
         sunSat[1] = state[associate+1] - cbSunVector[1];
         sunSat[2] = state[associate+2] - cbSunVector[2];
         sunDistance = sqrt(sunSat[0]*sunSat[0] + sunSat[1]*sunSat[1] +
                           sunSat[2]*sunSat[2]);
         if (sunDistance == 0.0)
            sunDistance = 1.0;

         distancefactor = nominalSun / sunDistance;     // It has no unit (Km/Km)
         // Convert m/s^2 to km/s^2
         //distancefactor *= distancefactor * GmatMathConstants::M_TO_KM * GmatMathConstants::M_TO_KM;
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
                  sunRadius, bodyRadius, psunrad);
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
            if (srpModel == "Spherical")
            {
               /// @todo Make Cr value reporting more elegant
               static Real old_cr = 0.0;
               if ((old_cr != cr[i]))
               {
                  old_cr = cr[i];
                  //MessageInterface::ShowMessage("Cr = %.12lf   Cr0 = %.12lf\n", cr[i], crInitial[i]);
               }

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

               // VX term for estimating Cr
               if (estimatingCr)
                  aTilde[ix+crEpsilonRow] = deriv[i6 + 3] * crInitial[i] / cr[i];

               ix = stmRowCount * 4;
               aTilde[ix]     = mag * (    - 3.0 * sunSat[1] * sunSat[0] / sSquared);      // unit: 1/s^2
               aTilde[ix + 1] = mag * (1.0 - 3.0 * sunSat[1] * sunSat[1] / sSquared);      // unit: 1/s^2
               aTilde[ix + 2] = mag * (    - 3.0 * sunSat[1] * sunSat[2] / sSquared);      // unit: 1/s^2

               // VY term for estimating Cr
               if (estimatingCr)
                  aTilde[ix+crEpsilonRow] = deriv[i6 + 4] * crInitial[i] / cr[i];

               ix = stmRowCount * 5;
               aTilde[ix]     = mag * (    - 3.0 * sunSat[2] * sunSat[0] / sSquared);     // unit: 1/s^2
               aTilde[ix + 1] = mag * (    - 3.0 * sunSat[2] * sunSat[1] / sSquared);     // unit: 1/s^2
               aTilde[ix + 2] = mag * (1.0 - 3.0 * sunSat[2] * sunSat[2] / sSquared);     // unit: 1/s^2

               // VZ term for estimating Cr
               if (estimatingCr)
                  aTilde[ix+crEpsilonRow] = deriv[i6 + 5] * crInitial[i] / cr[i];

            }
            else // SPADFile
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
               ix = stmRowCount * 4;
               aTilde[ix]   = (accelPertX[4] - nominalAccel[4]) / dx;       // unit: 1/s^2
               aTilde[ix+1] = (accelPertY[4] - nominalAccel[4]) / dy;       // unit: 1/s^2
               aTilde[ix+2] = (accelPertZ[4] - nominalAccel[4]) / dz;       // unit: 1/s^2
               ix = stmRowCount * 5;
               aTilde[ix]   = (accelPertX[5] - nominalAccel[5]) / dx;       // unit: 1/s^2
               aTilde[ix+1] = (accelPertY[5] - nominalAccel[5]) / dy;       // unit: 1/s^2
               aTilde[ix+2] = (accelPertZ[5] - nominalAccel[5]) / dz;       // unit: 1/s^2

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
         }
         else
         {
            ix = stmRowCount * 3;
            aTilde[ix] = aTilde[ix+1] = aTilde[ix+2] = 0.0;
            ix = stmRowCount * 4;
            aTilde[ix] = aTilde[ix+1] = aTilde[ix+2] = 0.0;
            ix = stmRowCount * 5;
            aTilde[ix] = aTilde[ix+1] = aTilde[ix+2] = 0.0;
         }

         for (Integer j = 0; j < stmRowCount; ++j)
         {
            for (Integer k = 0; k < stmRowCount; ++k)
            {
               element = j * stmRowCount + k;
               deriv[iStart+element] = aTilde[element];
            }
         }
      }

      for (Integer i = 0; i < aMatrixCount; ++i)
      {
         #ifdef DEBUG_A_MATRIX
            MessageInterface::ShowMessage(
                  "Filling A-matrix for spacecraft %d\n", i);
         #endif
         Integer iStart = aMatrixStart + i * stmRowCount*stmRowCount;
         associate = theState->GetAssociateIndex(iStart);

         // Calculate A-tilde
         for (Integer j = 0; j < stmRowCount; ++j)
         {
            ix = j * stmRowCount;
            for (Integer k = 0; k < stmRowCount; ++k)
               aTilde[ix+k] = 0.0;
         }

         // Build vector from Sun to the current spacecraft; (-s in math spec)
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
                  sunRadius, bodyRadius, psunrad);
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
            if (srpModel == "Spherical")
            {
               // All of the common terms for C_s
               mag = percentSun * cr[i] * fluxPressure * area[i] * distancefactor /
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
            else // SPADFile
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
         }
         else
         {
            ix = stmRowCount * 3;
            aTilde[ix] = aTilde[ix+1] = aTilde[ix+2] = 0.0;
            ix = stmRowCount * 4;
            aTilde[ix] = aTilde[ix+1] = aTilde[ix+2] = 0.0;
            ix = stmRowCount * 5;
            aTilde[ix] = aTilde[ix+1] = aTilde[ix+2] = 0.0;
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
               deriv[iStart+element] = aTilde[element];
               #ifdef DEBUG_A_MATRIX
                  MessageInterface::ShowMessage("  %le  ", deriv[i6+element]);
               #endif
            }
            #ifdef DEBUG_A_MATRIX
               MessageInterface::ShowMessage("\n");
            #endif
         }
      }

	  delete [] aTilde;
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

   Real cr, area, mass;
   Real distancefactor = 1.0, mag;
   bool inSunlight = true, inShadow = false;

   cr   = sc->GetRealParameter("Cr");
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

   distancefactor = nominalSun / sunDistance;
   // Convert m/s^2 to km/s^2
   distancefactor *= distancefactor * GmatMathConstants::M_TO_KM;

   // Test shadow condition for current spacecraft (only if body isn't Sol)
   if (!bodyIsTheSun)
   {
      psunrad = asin(sunRadius / sunDistance);
      std::string shModel = "DualCone";
      if (shadowModel == CYLINDRICAL_MODEL) shModel = "Cylindrical";
      percentSun = shadowState->FindShadowState(inSunlight, inShadow,
            shModel, state, cbSunVector, sunSat, forceVector,
            sunRadius, bodyRadius, psunrad);
//      FindShadowState(inSunlight, inShadow, state);
   }
   else
   {
       inSunlight= true;
       inShadow = false;
       percentSun = 1.0;
   }

   Rvector3 spadArea;
   if (!inShadow)
   {
      if (srpModel == "Spherical")
      {
         // Montenbruck and Gill, eq. 3.75
         mag = percentSun * cr * fluxPressure * area * distancefactor /
                             mass;
         dv[0] = dv[1] = dv[2] = 0.0;
         dv[3] = mag * forceVector[0];
         dv[4] = mag * forceVector[1];
         dv[5] = mag * forceVector[2];
      }
      else   // SPADFile
      {
         // Montenbruck and Gill, eq. 3.75
         mag = percentSun * fluxPressure * distancefactor /
                             mass;
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
         dv[3] = mag * spadArea[0];
         dv[4] = mag * spadArea[1];
         dv[5] = mag * spadArea[2];
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
   sunSat[0] = state[0] - cbSunVector[0];
   sunSat[1] = state[1] - cbSunVector[1];
   sunSat[2] = state[2] - cbSunVector[2];
   Real sunDistance = sqrt(sunSat[0]*sunSat[0] + sunSat[1]*sunSat[1] +
                           sunSat[2]*sunSat[2]);
   if (sunDistance == 0.0)
      sunDistance = 1.0;

   Real distancefactor = nominalSun / sunDistance;
   // Convert m/s^2 to km/s^2
   distancefactor *= distancefactor * GmatMathConstants::M_TO_KM;

   Real mag = percentSun * fluxPressure * distancefactor / mass[scID];

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
   result[3] = mag * spadArea[0];
   result[4] = mag * spadArea[1];
   result[5] = mag * spadArea[2];

   #ifdef DEBUG_SPAD_ACCEL
      MessageInterface::ShowMessage("--- AT END, result = %12.16le  %12.16le  %12.16le  %12.16le  %12.16le  %12.16le\n",
            result[0],result[1],result[2],result[3],result[4],result[5]);
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
 * @param i ID for the spacecraft
 * @param parmName name of the Spacecraft parameter 
 * @param parm Parameter value
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
        if (parmNumber < cr.size())
        {
           cr[i] = parm;
           if (crInitial[i] == -99999999.9999)
              crInitial[i] = parm;
        }
        else
        {
           cr.push_back(parm);
           crInitial.push_back(parm);
        }

        if (parmID >= 0)
           crID = parmID;
    }
    if (parmName == "SRPArea")
    {
       if (parmNumber < area.size())
            area[i] = parm;
        else
            area.push_back(parm);
        if (parmID >= 0)
            areaID = parmID;
    }
    if (parmName == "CrEpsilon")
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
   if ((parmName == "Cr") || (parmName == ""))
   {
      cr.clear();
      crInitial.clear();
   }
   if ((parmName == "CrEpsilon") || (parmName == ""))
      crEpsilon.clear();
   if ((parmName == "SRPArea") || (parmName == ""))
      area.clear();
   if ((parmName == "scObjs")  || (parmName == ""))
      scObjs.clear();
}

//------------------------------------------------------------------------------
// void PhysicalModel::SetSpaceObject(const Integer i, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft pointers to the force model.
 *
 * @param i   ID for the spacecraft
 * @param obj pointer to the Spacecraft
 */
//------------------------------------------------------------------------------
void SolarRadiationPressure::SetSpaceObject(const Integer i, GmatBase *obj)
{
   unsigned parmNumber = (unsigned)(i);

   if (parmNumber < scObjs.size())
      scObjs[i] = obj;
   else
      scObjs.push_back(obj);
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
//      if (srpModel == "Spherical")
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
 * @param id State Element ID for the derivative type
 * @param index Starting index in the state vector for this type of derivative
 * @param quantity Number of objects that supply this type of data
 * @param sizeOfType For sizable types, the size to use.  For example, for STM,
 *                   this is the number of rows or columns in the STM
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::SetStart(Gmat::StateElementId id, Integer index, 
                      Integer quantity, Integer sizeOfType)
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
         stmRowCount = Integer(sqrt((Real)sizeOfType));
         retval = true;
         break;
         
      case Gmat::ORBIT_A_MATRIX:
         aMatrixCount = quantity;
         aMatrixStart = index;
         fillAMatrix = true;
         stmRowCount = Integer(sqrt((Real)sizeOfType));
         retval = true;
         break;

      default:
         break;
   }
   
   return retval;
}

