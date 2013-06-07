//$Id$
//------------------------------------------------------------------------------
//                              SolarRadiationPressure
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

//#define DEBUG_SRP_ORIGIN
//#define DEBUG_SOLAR_RADIATION_PRESSURE
//#define DEBUG_SOLAR_RADIATION_PRESSURE_TIMESHADOW
//#define DEBUG_A_MATRIX

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
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
};
           
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
   sunDistance         (149597870.691),
   nominalSun          (149597870.691),
   bodyIsTheSun        (false),
   psunrad             (0.0),
   pcbrad              (0.0),
   percentSun          (1.0),
   bodyID              (-1),
   satCount            (0),
   massID              (-1),
   crID                (-1),
   areaID              (-1)
{
   parameterCount = SRPParamCount;
   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   objectTypeNames.push_back("SolarRadiationPressure");
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
   sunDistance         (srp.sunDistance),
   nominalSun          (srp.nominalSun),
   bodyIsTheSun        (srp.bodyIsTheSun),
   psunrad             (srp.psunrad),
   pcbrad              (srp.pcbrad),
   percentSun          (srp.percentSun),
   bodyID              (srp.bodyID),
   satCount            (srp.satCount),
   massID              (srp.massID),
   crID                (srp.crID),
   areaID              (srp.areaID)
{
   parameterCount = SRPParamCount;
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
      flux         = srp.flux;           // W/m^2, IERS 1996
      fluxPressure = srp.fluxPressure;   // converted to N/m^2
      sunDistance  = srp.sunDistance;
      nominalSun   = srp.nominalSun;
      bodyIsTheSun = srp.bodyIsTheSun;
      psunrad      = srp.psunrad;
      pcbrad       = srp.pcbrad;
      percentSun   = srp.percentSun;
      bodyID       = srp.bodyID;
   
      satCount      = srp.satCount;
      massID        = srp.massID;
      crID          = srp.crID;
      areaID        = srp.areaID;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// SolarRadiationPressure::~SolarRadiationPressure(void)
//------------------------------------------------------------------------------
/**
 * 
 */
//------------------------------------------------------------------------------
SolarRadiationPressure::~SolarRadiationPressure(void)
{ 
    if ((useAnalytic) && (cbSunVector))
        delete [] cbSunVector;
    if (forceVector)
        delete [] forceVector;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the SolarRadiationPressure.
 *
 * @return clone of the SolarRadiationPressure.
 *
 */
//------------------------------------------------------------------------------
GmatBase* SolarRadiationPressure::Clone(void) const
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
   if ((id == FLUX) || (id == NOMINAL_SUN))
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
      flux = value;
      fluxPressure = flux / GmatPhysicalConstants::c;
      return flux;
   }
   if (id == FLUX_PRESSURE)
   {
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
      sunRadius = theSun->GetEquatorialRadius();;
      
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

   Real distancefactor = 1.0, mag, sSquared;
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

   Integer i6;
   Real sunSat[3];
    
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
        
         distancefactor = nominalSun / sunDistance;
         // Convert m/s^2 to km/s^2
         distancefactor *= distancefactor * GmatMathConstants::M_TO_KM;
      
         #ifdef DEBUG_SRP_ORIGIN
            if (shadowModel == 0) 
               shadowModel = CONICAL_MODEL;
         #endif
        
         // Test shadow condition for current spacecraft (only if body isn't Sol)
         if (!bodyIsTheSun)
         {
            psunrad = asin(sunRadius / sunDistance);
            FindShadowState(inSunlight, inShadow, &state[i6]);
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
            mag = percentSun * cr[i] * fluxPressure * area[i] * distancefactor / 
                                mass[i];

            if (order == 1) 
            {
               deriv[i6] = deriv[i6 + 1] = deriv[i6 + 2] = 0.0;
               deriv[i6 + 3] = mag * forceVector[0];
               deriv[i6 + 4] = mag * forceVector[1];
               deriv[i6 + 5] = mag * forceVector[2];
            }
            else 
            {
               deriv[ i6 ] = mag * forceVector[0];
               deriv[i6+1] = mag * forceVector[1];
               deriv[i6+2] = mag * forceVector[2];
               deriv[i6 + 3] = deriv[i6 + 4] = deriv[i6 + 5] = 0.0;
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
      Real aTilde[36];
      Integer associate, element;
      for (Integer i = 0; i < stmCount; ++i)
      {
         #ifdef DEBUG_STM_MATRIX
            MessageInterface::ShowMessage("Filling STM for spacecraft %d\n", i);
         #endif
         i6 = stmStart + i * 36;
         associate = theState->GetAssociateIndex(i6);

         // Calculate A-tilde
         aTilde[ 0] = aTilde[ 1] = aTilde[ 2] =
         aTilde[ 3] = aTilde[ 4] = aTilde[ 5] =
         aTilde[ 6] = aTilde[ 7] = aTilde[ 8] =
         aTilde[ 9] = aTilde[10] = aTilde[11] =
         aTilde[12] = aTilde[13] = aTilde[14] =
         aTilde[15] = aTilde[16] = aTilde[17] =
         aTilde[21] = aTilde[22] = aTilde[23] =
         aTilde[27] = aTilde[28] = aTilde[29] =
         aTilde[33] = aTilde[34] = aTilde[35] = 0.0;

         // Build vector from Sun to the current spacecraft; (-s in math spec)
         sunSat[0] = state[ associate ] - cbSunVector[0];
         sunSat[1] = state[associate+1] - cbSunVector[1];
         sunSat[2] = state[associate+2] - cbSunVector[2];
         sunDistance = sqrt(sunSat[0]*sunSat[0] + sunSat[1]*sunSat[1] +
                           sunSat[2]*sunSat[2]);
         if (sunDistance == 0.0)
            sunDistance = 1.0;

         distancefactor = nominalSun / sunDistance;
         // Convert m/s^2 to km/s^2
         distancefactor *= distancefactor * GmatMathConstants::M_TO_KM * GmatMathConstants::M_TO_KM;

         #ifdef DEBUG_SRP_ORIGIN
            if (shadowModel == 0)
               shadowModel = CONICAL_MODEL;
         #endif

         // Test shadow condition for current spacecraft (only if body isn't Sol)
         if (!bodyIsTheSun)
         {
            psunrad = asin(sunRadius / sunDistance);
            FindShadowState(inSunlight, inShadow, &state[i6]);
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
            // All of the common terms for C_s
            mag = percentSun * cr[i] * fluxPressure * area[i] * distancefactor /
                                (mass[i] * sunDistance);
            sSquared = sunDistance * sunDistance;

            // Math spec terms for SRP C submatrix of the A-matrix
            aTilde[18] = mag * (1.0 - 3.0 * sunSat[0]*sunSat[0] / sSquared);
            aTilde[19] = mag * (    - 3.0 * sunSat[0]*sunSat[1] / sSquared);
            aTilde[20] = mag * (    - 3.0 * sunSat[0]*sunSat[2] / sSquared);
            aTilde[24] = mag * (    - 3.0 * sunSat[1]*sunSat[0] / sSquared);
            aTilde[25] = mag * (1.0 - 3.0 * sunSat[1]*sunSat[1] / sSquared);
            aTilde[26] = mag * (    - 3.0 * sunSat[1]*sunSat[2] / sSquared);
            aTilde[30] = mag * (    - 3.0 * sunSat[2]*sunSat[0] / sSquared);
            aTilde[31] = mag * (    - 3.0 * sunSat[2]*sunSat[1] / sSquared);
            aTilde[32] = mag * (1.0 - 3.0 * sunSat[2]*sunSat[2] / sSquared);
         }
         else
         {
            aTilde[18] = aTilde[19] = aTilde[20] =
            aTilde[24] = aTilde[25] = aTilde[26] =
            aTilde[30] = aTilde[31] = aTilde[32] = 0.0;
         }

         for (Integer j = 0; j < 6; ++j)
         {
            for (Integer k = 0; k < 6; ++k)
            {
               element = j * 6 + k;
               deriv[i6+element] = aTilde[element];
            }
         }
      }

      for (Integer i = 0; i < aMatrixCount; ++i)
      {
         #ifdef DEBUG_A_MATRIX
            MessageInterface::ShowMessage(
                  "Filling A-matrix for spacecraft %d\n", i);
         #endif
         i6 = aMatrixStart + i * 36;
         associate = theState->GetAssociateIndex(i6);

         // Calculate A-tilde
         aTilde[ 0] = aTilde[ 1] = aTilde[ 2] =
         aTilde[ 3] = aTilde[ 4] = aTilde[ 5] =
         aTilde[ 6] = aTilde[ 7] = aTilde[ 8] =
         aTilde[ 9] = aTilde[10] = aTilde[11] =
         aTilde[12] = aTilde[13] = aTilde[14] =
         aTilde[15] = aTilde[16] = aTilde[17] =
         aTilde[21] = aTilde[22] = aTilde[23] =
         aTilde[27] = aTilde[28] = aTilde[29] =
         aTilde[33] = aTilde[34] = aTilde[35] = 0.0;

         // Build vector from Sun to the current spacecraft; (-s in math spec)
         sunSat[0] = state[ associate ] - cbSunVector[0];
         sunSat[1] = state[associate+1] - cbSunVector[1];
         sunSat[2] = state[associate+2] - cbSunVector[2];
         sunDistance = sqrt(sunSat[0]*sunSat[0] + sunSat[1]*sunSat[1] +
                           sunSat[2]*sunSat[2]);
         if (sunDistance == 0.0)
            sunDistance = 1.0;

         distancefactor = nominalSun / sunDistance;
         // Convert m/s^2 to km/s^2
         distancefactor *= distancefactor * GmatMathConstants::M_TO_KM;

         #ifdef DEBUG_SRP_ORIGIN
            if (shadowModel == 0)
               shadowModel = CONICAL_MODEL;
         #endif

         // Test shadow condition for current spacecraft (only if body isn't Sol)
         if (!bodyIsTheSun)
         {
            psunrad = asin(sunRadius / sunDistance);
            FindShadowState(inSunlight, inShadow, &state[i6]);
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
            // All of the common terms for C_s
            mag = percentSun * cr[i] * fluxPressure * area[i] * distancefactor /
                                (mass[i] * sunDistance);
            sSquared = sunDistance * sunDistance;

            // Math spec terms for SRP C submatrix of the A-matrix
            aTilde[18] = mag * (1.0 - 3.0 * sunSat[0]*sunSat[0] / sSquared);
            aTilde[19] = mag * (    - 3.0 * sunSat[0]*sunSat[1] / sSquared);
            aTilde[20] = mag * (    - 3.0 * sunSat[0]*sunSat[2] / sSquared);
            aTilde[24] = mag * (    - 3.0 * sunSat[1]*sunSat[0] / sSquared);
            aTilde[25] = mag * (1.0 - 3.0 * sunSat[1]*sunSat[1] / sSquared);
            aTilde[26] = mag * (    - 3.0 * sunSat[1]*sunSat[2] / sSquared);
            aTilde[30] = mag * (    - 3.0 * sunSat[2]*sunSat[0] / sSquared);
            aTilde[31] = mag * (    - 3.0 * sunSat[2]*sunSat[1] / sSquared);
            aTilde[32] = mag * (1.0 - 3.0 * sunSat[2]*sunSat[2] / sSquared);
         }
         else
         {
            aTilde[18] = aTilde[19] = aTilde[20] =
            aTilde[24] = aTilde[25] = aTilde[26] =
            aTilde[30] = aTilde[31] = aTilde[32] = 0.0;
         }

         #ifdef DEBUG_A_MATRIX
            MessageInterface::ShowMessage(
                  "A-Matrix contribution[%d] from SRP:\n", i);
         #endif
         for (Integer j = 0; j < 6; ++j)
         {
            for (Integer k = 0; k < 6; ++k)
            {
               element = j * 6 + k;
               deriv[i6+element] = aTilde[element];
               #ifdef DEBUG_A_MATRIX
                  MessageInterface::ShowMessage("  %le  ", deriv[i6+element]);
               #endif
            }
            #ifdef DEBUG_A_MATRIX
               MessageInterface::ShowMessage("\n");
            #endif
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

   Real cr, area, mass;
   Real distancefactor = 1.0, mag;
   bool inSunlight = true, inShadow = false;

   cr   = sc->GetRealParameter("Cr");
   area = sc->GetRealParameter("SRPArea");
   mass = sc->GetRealParameter("TotalMass");

   Real ep = sc->GetEpoch();
   sunrv = theSun->GetState(ep);

   Real *j2kState = sc->GetState().GetState();
   Real state[6];

   BuildModelState(ep, state, j2kState);

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
      FindShadowState(inSunlight, inShadow, state);
   }
   else
   {
       inSunlight= true;
       inShadow = false;
       percentSun = 1.0;
   }

   if (!inShadow)
   {
      // Montenbruck and Gill, eq. 3.75
      mag = percentSun * cr * fluxPressure * area * distancefactor /
                          mass;

      dv[0] = dv[1] = dv[2] = 0.0;
      dv[3] = mag * forceVector[0];
      dv[4] = mag * forceVector[1];
      dv[5] = mag * forceVector[2];
   }
   else
   {
      dv[0] = dv[1] = dv[2] =
      dv[3] = dv[4] = dv[5] = 0.0;
   }

   return dv;
}


//------------------------------------------------------------------------------
// void FindShadowState(bool &lit, bool &dark, Real *state)
//------------------------------------------------------------------------------
/**
 * Determines lighting conditions at the input location
 * 
 * @param <lit>   Indicates if the spoacecraft is in full sunlight
 * @param <dark>  Indicates if the spacecarft is in umbra
 * @param <state> Current spacecraft state
 * 
 * \todo: Currently implemented for one spacecraft, state vector arranges as
 * (x, y, z)
 */
//------------------------------------------------------------------------------
void SolarRadiationPressure::FindShadowState(bool &lit, bool &dark, Real *state)
{
    Real mag = sqrt(cbSunVector[0]*cbSunVector[0] + 
                      cbSunVector[1]*cbSunVector[1] + 
                      cbSunVector[2]*cbSunVector[2]);

    Real unitsun[3];
    unitsun[0] = cbSunVector[0] / mag;
    unitsun[1] = cbSunVector[1] / mag;
    unitsun[2] = cbSunVector[2] / mag;

    double rdotsun = state[0]*unitsun[0] + 
                     state[1]*unitsun[1] + 
                     state[2]*unitsun[2];

    if (rdotsun > 0.0) // Sunny side of central body is always fully lit
    {    
        lit = true;
        dark = false;
        percentSun = 1.0;
        return;
    }

    if (shadowModel == CYLINDRICAL_MODEL) 
    {
        // In this model, the spacecraft is in darkness if it is within 
        // bodyRadius of the sun-body line; otherwise it is lit
        Real rperp[3];
        rperp[0] = state[0] - rdotsun * unitsun[0];
        rperp[1] = state[1] - rdotsun * unitsun[1];
        rperp[2] = state[2] - rdotsun * unitsun[2];

        mag = sqrt(rperp[0]*rperp[0] + rperp[1]*rperp[1] + rperp[2]*rperp[2]);
        if (mag < bodyRadius) 
        {
            percentSun = 0.0;
            lit = false;
            dark = true;
        }
        else 
        {
            percentSun = 1.0;
            lit = true;
            dark = false;
        }
    }
    else if (shadowModel == CONICAL_MODEL) 
    {
        Real s0, s2, lsc, l1, l2, c1, c2, sinf1, sinf2, tanf1, tanf2;

        // Montenbruck and Gill, eq. 3.79
        s0 = -state[0]*unitsun[0] - state[1]*unitsun[1] - state[2]*unitsun[2];
        s2 = state[0]*state[0] + state[1]*state[1] + state[2]*state[2];

        // Montenbruck and Gill, eq. 3.80
        lsc = sqrt(s2 - s0*s0);

        // Montenbruck and Gill, eq. 3.81
        sinf1 = (sunRadius + bodyRadius) / mag;
        sinf2 = (sunRadius - bodyRadius) / mag;

        // Appropriate l1 and l2 temporarily
        l1 = sinf1 * sinf1;
        l2 = sinf2 * sinf2;
        tanf1 = sqrt(l1 / (1.0 - l1));
        tanf2 = sqrt(l2 / (1.0 - l2));
        
        // Montenbruck and Gill, eq. 3.82
        c1 = s0 + bodyRadius / sinf1;
        c2 = bodyRadius / sinf2 - s0;       // Different sign from M&G

        // Montenbruck and Gill, eq. 3.83
        l1 = c1 * tanf1;
        l2 = c2 * tanf2;

        if (lsc > l1) 
        {
            // Outside of the penumbral cone
            lit = true;
            dark = false;
            percentSun = 1.0;
            return;
        }
        else 
        {
            lit = false;
            if (lsc < fabs(l2)) 
            {
                // Inside umbral cone
                if (c2 >= 0.0) 
                { 
                    // no annular ring
                    percentSun = 0.0;
                    dark = true;
                }
                else 
                {
                    // annular eclipse
                    pcbrad = asin(bodyRadius / sqrt(s2));
                    percentSun = (psunrad*psunrad - pcbrad*pcbrad) / 
                                 (psunrad*psunrad);
                    dark = false;
                }
                
                return;
            }
            // In penumbra
            pcbrad = asin(bodyRadius / sqrt(s2));
            percentSun = ShadowFunction(state);
            lit = false;
            dark = false;
        }
    }
}


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


//------------------------------------------------------------------------------
// Real ShadowFunction(Real * state)
//------------------------------------------------------------------------------
/**
 * Calculates %lit when in penumbra.
 * 
 * @param <state> The current spacecraft state
 * 
 * @return the multiplier used when the satellite is partially lit
 */
//------------------------------------------------------------------------------
Real SolarRadiationPressure::ShadowFunction(Real * state)
{
    Real mag = sqrt(state[0]*state[0] + 
                      state[1]*state[1] + 
                      state[2]*state[2]);
    
    // Montenbruck and Gill, eq. 3.87
    Real c = acos((state[0]*forceVector[0] + 
                     state[1]*forceVector[1] + 
                     state[2]*forceVector[2]) / mag);

    Real a2 = psunrad*psunrad;
    Real b2 = pcbrad*pcbrad;

    // Montenbruck and Gill, eq. 3.93
    Real x = (c*c + a2 - b2) / (2.0 * c);
    Real y = sqrt(a2 - x*x);

    // Montenbruck and Gill, eq. 3.92
    Real area = a2*acos(x/psunrad) + b2*acos((c-x)/pcbrad) - c*y;

    // Montenbruck and Gill, eq. 3.94
    return 1.0 - area / (M_PI * a2);
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
            cr[i] = parm;
        else
            cr.push_back(parm);
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
        msg << "Setting satellite parameter " << parmName << " for Spacecraft "
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
      cr.clear();
   if ((parmName == "SRPArea") || (parmName == ""))
      area.clear();
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
      MessageInterface::ShowMessage("Warning: The orbit state transition "
            "matrix does not currently contain SRP contributions from shadow "
            "partial derivatives.\n");
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
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::SetStart(Gmat::StateElementId id, Integer index, 
                      Integer quantity)
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
         retval = true;
         break;
         
      case Gmat::ORBIT_A_MATRIX:
         aMatrixCount = quantity;
         aMatrixStart = index;
         fillAMatrix = true;
         retval = true;
         break;

      default:
         break;
   }
   
   return retval;
}

