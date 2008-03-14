//$Id$
//------------------------------------------------------------------------------
//                              SolarRadiationPressure
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
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
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
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

//#define DEBUG_SRP_ORIGIN
//#define DEBUG_SOLAR_RADIATION_PRESSURE
//#define DEBUG_SOLAR_RADIATION_PRESSURE_TIMESHADOW

#define SPEED_OF_LIGHT 299792458.0

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
   useAnalytic         (true),
   shadowModel         (CONICAL_MODEL),
   vectorModel         (SUN_PARALLEL),
   bodyRadius          (6378.14),
   cbSunVector         (NULL),
   forceVector         (NULL),
   sunRadius           (6.96e5),
   hasMoons            (false),
   flux                (1367.0),                  // W/m^2, IERS 1996
   fluxPressure        (flux / SPEED_OF_LIGHT),   // converted to N/m^2
   sunDistance         (149597870.691),
   nominalSun          (149597870.691),
   bodyIsTheSun        (false)
{
   parameterCount = SRPParamCount;
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
   bodyID              (srp.bodyID)
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
   if (this == &srp)
      return *this;

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
//         throw ForceModelException(
//            "Attempting to access C_r for SRP force before any spacecraft "
//            "parameters were set.");
//   if (id == AREA)
//      if (area.size() > 0)
//         return area[0];
//      else
//         throw ForceModelException(
//            "Attempting to access area for SRP force before any spacecraft "
//            "parameters were set.");
//   if (id == MASS)
//      if (mass.size() > 0)
//         return mass[0];
//      else
//         throw ForceModelException(
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
      fluxPressure = flux / SPEED_OF_LIGHT;
      return flux;
   }
   if (id == FLUX_PRESSURE)
   {
      fluxPressure = value;
      flux = fluxPressure * SPEED_OF_LIGHT;
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
// bool SolarRadiationPressure::Initialize(void)
//------------------------------------------------------------------------------
/**
 * 
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::Initialize()
{
   bool retval = PhysicalModel::Initialize();

   if (!retval)
      return false;

   if (solarSystem) 
   {
      theSun = solarSystem->GetBody(SolarSystem::SUN_NAME);
      
      if (!theSun)
         throw ForceModelException("Solar system does not contain the Sun for SRP force.");

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
         throw ForceModelException("Central body not set for SRP force.");

      if (body->GetName() == "Sun")         
         bodyIsTheSun = true;
      else
         bodyIsTheSun = false;
      bodyRadius = body->GetEquatorialRadius();

      if (forceVector)
         delete [] forceVector;
   
      forceVector = new Real[dimension];
      
      if (!cbSunVector)
         cbSunVector = new Real[3];

      initialized = true;
   }
   
   #ifdef DEBUG_SRP_ORIGIN
      shadowModel = 0;  // CONICAL_MODEL
   #endif
   
   return initialized;
}

//------------------------------------------------------------------------------
// bool SolarRadiationPressure::SetCentralBody()
//------------------------------------------------------------------------------
/**
 * 
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::SetCentralBody()
{
   if (!solarSystem)
      throw ForceModelException("Solar system not set in call to SRP::SetCentralBody.");
   
   theSun = solarSystem->GetBody(SolarSystem::SUN_NAME);
   
   if (!theSun)
      throw ForceModelException("Solar system does not contain the Sun for SRP force.");
   
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
      throw ForceModelException("Central body not set for SRP force.");

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
bool SolarRadiationPressure::GetDerivatives(Real *state, Real dt, Integer order)
{
    if (!initialized)
        return false;
        
    Real satCount = dimension / 6;

    if (!theSun)
       throw ForceModelException("The Sun is not set in SRP::GetDerivatives");

    if (!body)
       throw ForceModelException(
          "The central body is not set in SRP::GetDerivatives");
    
    if (!cbSunVector)
       throw ForceModelException(
          "The sun vector is not initialized in SRP::GetDerivatives");

    if (cr.size() != satCount)
    {
       std::stringstream msg;
       msg << "Mismatch between satellite count (" << satCount 
           << ") and radiation coefficient count (" << cr.size() << ")"; 
       throw ForceModelException(msg.str());
    }

    if (area.size() != satCount)
    {
       std::stringstream msg;
       msg << "Mismatch between satellite count (" << satCount   
           << ") and area count (" << area.size() << ")"; 
       throw ForceModelException(msg.str());
    }

    if (mass.size() != satCount)
    {
       std::stringstream msg;
       msg << "Mismatch between satellite count (" << satCount   
           << ") and mass count (" << mass.size() << ")"; 
       throw ForceModelException(msg.str());
    }

    Real distancefactor = 1.0, mag = 0.0;
    bool inSunlight = true, inShadow = false;

    Real ep = epoch + dt / 86400.0;
    sunrv = theSun->GetState(ep);
    
    // Rvector6 is initialized to all 0.0's; only change it if the body is not 
    // the Sun
    if (!bodyIsTheSun)
       cbrv = body->GetState(ep);
    cbSunVector[0] = sunrv[0] - cbrv[0];
    cbSunVector[1] = sunrv[1] - cbrv[1];
    cbSunVector[2] = sunrv[2] - cbrv[2];

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

    for (Integer i = 0; i < satCount; ++i) 
    {
        i6 = i*6;

        // Build vector from the Sun to the current spacecraft
        sunSat[0] = state[i6] - cbSunVector[0];
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
        // Factor of 0.001 converts m/s^2 to km/s^2
        distancefactor *= distancefactor * 0.001;
      
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
// void SolarRadiationPressure::FindShadowState(bool &lit, bool &dark, Real *state)
//------------------------------------------------------------------------------
/**
 * Determines lighting conditions at the input location
 * 
 * @param <lit>   Indicates if the spoacecraft is in full sunlight
 * @param <dark>  Indicates if the spacecarft is in umbra
 * @param <state> Current spacecraft state
 * 
 * \todo: Currently implemented for one spacecraft, state vector arranges as (x, y, z)
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
// Real SolarRadiationPressure::ShadowFunction(Real * state)
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
                                          const Real parm)
{
    unsigned parmNumber = (unsigned)(i);

    #ifdef DEBUG_SOLAR_RADIATION_PRESSURE
         std::stringstream msg;
         msg << "Setting satellite parameter " << parmName << " for Spacecraft " 
             << i << " to " << parm << "\n";
         MessageInterface::ShowMessage(msg.str());
    #endif
    
    if (parmName == "DryMass")
        if (parmNumber < mass.size())
            mass[i] = parm;
        else
            mass.push_back(parm);
    if (parmName == "Cr")
        if (parmNumber < cr.size())
            cr[i] = parm;
        else
            cr.push_back(parm);
    if (parmName == "SRPArea")
        if (parmNumber < area.size())
            area[i] = parm;
        else
            area.push_back(parm);
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
   if ((parmName == "DryMass") || (parmName == ""))
      mass.clear();
   if ((parmName == "Cr") || (parmName == ""))
      cr.clear();
   if ((parmName == "SRPArea") || (parmName == ""))
      area.clear();
}
