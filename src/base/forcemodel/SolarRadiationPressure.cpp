//$Header$
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
// Modification History      : 2/27/2003 - D. Conway, Thinking Systems, Inc.
//                             Added class to test discontinuities in the
//                             force model
//
//                           : 03/30/2004 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//
//                           : 04/16/2004 - W. Waktola, Missions Applications Branch
//                              Additions:
//                                - GetParameterText()
//                                - GetParameterID()
//                                - GetParameterType()
//                                - GetParameterTypeString()
//                                - GetRealParameter()
//                                - SetRealParameter()
//                                - GetBooleanParameter()
//                                - SetBooleanParameter()
//                                - GetIntegerParameter()
//                                - SetIntegerParameter()
//
//                           : 04/19/2004 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Modified code to use the GMAT SolarSystem model
// **************************************************************************

#include "SolarRadiationPressure.hpp"



//---------------------------------
// static data
//---------------------------------
const std::string
SolarRadiationPressure::PARAMETER_TEXT[SRPParamCount] =
{
    "UseAnalytic",
    "ShadowModel",
    "VectorModel",
    "Body", //loj: 5/28/04 added to match with enum/ this will be removed when PhysicalModel has body
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
SolarRadiationPressure::PARAMETER_TYPE[SRPParamCount] =
{
   Gmat::BOOLEAN_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::STRING_TYPE, //loj: 5/28/04 added to match with enum
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
   cr                  (1.8),
   area                (5.0),
   mass                (500.0),
   flux                (1367.0),               // W/m^2, IERS 1996
   fluxPressure        (flux / 299792458.0),   // converted to N/m^2
   sunDistance         (149597870.691),
   nominalSun          (149597870.691)
{
//    SRPParamCount = VECTORTYPE;  //waw: 04/19/04
}

//------------------------------------------------------------------------------
// SolarRadiationPressure(SolarRadiationPressure &srp)
//------------------------------------------------------------------------------
/**
 * 
 */
//------------------------------------------------------------------------------
SolarRadiationPressure::SolarRadiationPressure(const SolarRadiationPressure &srp) :
   PhysicalModel     (srp)
{

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
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string SolarRadiationPressure::GetParameterText(const Integer id) const
{
   if (id >= USE_ANALYTIC && id < SRPParamCount)
      return SolarRadiationPressure::PARAMETER_TEXT[id];
   else
      return GmatBase::GetParameterText(id);
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
      if (str == SolarRadiationPressure::PARAMETER_TEXT[i])
         return i;
   }
   return GmatBase::GetParameterID(str);
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
      return PARAMETER_TYPE[id];
   else
      return GmatBase::GetParameterType(id);
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
      return GmatBase::GetParameterTypeString(id);
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
    if (id == CR)
        return cr;
    if (id == AREA)
        return area;
    if (id == MASS)
        return mass;
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

    return GmatBase::GetRealParameter(id);
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
   if (id == CR)
   {
      cr = value;
      return cr;
   }
   if (id == AREA)
   {
      area = value;
      return area;
   }
   if (id == MASS)
   {
      mass = value;
      return mass;
   }
   if (id == FLUX)
   {
      flux = value;
      return flux;
   }
   if (id == FLUX_PRESSURE)
   {
      fluxPressure = value;
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
       
   return GmatBase::SetRealParameter(id, value);
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

    return GmatBase::GetBooleanParameter(id);
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
       
   return GmatBase::SetBooleanParameter(id, value);
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

   return GmatBase::GetIntegerParameter(id);
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
       
   return GmatBase::SetIntegerParameter(id, value);
}

//------------------------------------------------------------------------------
// bool SolarRadiationPressure::Initialize(void)
//------------------------------------------------------------------------------
/**
 * 
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::Initialize(void)
{
   bool retval = PhysicalModel::Initialize();

   if (!retval)
      return false;

   if (solarSystem) 
   {
      theSun = solarSystem->GetBody(SolarSystem::SUN_NAME);
      
      if (!theSun)
         throw ForceModelException("Solar system does not contain the Sun for SRP force.");

      /// @todo: Update to get the central body for solar radiation pressure
      theCentralBody = solarSystem->GetBody(SolarSystem::EARTH_NAME);

      if (!theCentralBody)
         throw ForceModelException("Central body not set for SRP force.");
         
      bodyRadius = theCentralBody->GetEquatorialRadius();

      if (forceVector)
         delete [] forceVector;
   
      forceVector = new Real[dimension];
      
      if (!cbSunVector)
         cbSunVector = new Real[3];

      initialized = true;
   }
   
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
   
   // DJC: Changed to use the Earth as the SRP central body for now
   /// @todo: Update to get the central body for solar radiation pressure
//   theCentralBody = theSun->GetCentralBody();
   theCentralBody = solarSystem->GetBody(SolarSystem::EARTH_NAME);
   
   if (!theCentralBody)
      throw ForceModelException("Central body not set for SRP force.");
   
   bodyRadius = theCentralBody->GetEquatorialRadius();

   return true;
}

//------------------------------------------------------------------------------
// bool SolarRadiationPressure::GetDerivatives(Real *state,Real dt,Integer order)
//------------------------------------------------------------------------------
/**
 * Calculates the SRP accelerations 
 */
//------------------------------------------------------------------------------
bool SolarRadiationPressure::GetDerivatives(Real *state, Real dt, Integer order)
{
    if (!initialized)
        return false;

    if (!theSun)
       throw ForceModelException("The Sun is not set in SRP::GetDerivatives");
    
    if (!theCentralBody)
       throw ForceModelException("The central body is not set in SRP::GetDerivatives");
    
    if (!cbSunVector)
       throw ForceModelException("The sun vector is not initialized in SRP::GetDerivatives");

    Real distancefactor = 1.0, mag = 0.0;

    bool inSunlight = true, inShadow = false;
    
    Real ep = epoch + dt / 86400.0;
    Rvector6 sunrv = theSun->GetState(ep);
    Rvector6 cbrv = theCentralBody->GetState(ep);
    cbSunVector[0] = sunrv[0] - cbrv[0];
    cbSunVector[1] = sunrv[1] - cbrv[1];
    cbSunVector[2] = sunrv[2] - cbrv[2];

    FindShadowState(inSunlight, inShadow, state);

    if (order > 2)
        return false;

    // The following is an approximation -- the vector and sun distance both
    // are set to the central body rather than the satellite.
    for (Integer i = 0; i < dimension/6; ++i) 
    {
        distancefactor = nominalSun / sunDistance;
        distancefactor *= distancefactor;
      
        if (!inShadow) 
        {
            mag = percentSun * cr * fluxPressure * area / 
                                 mass * distancefactor;

            if (order == 1) 
            {
                deriv[i*6] = deriv[i*6 + 1] = deriv[i*6 + 2] = 0.0;
                deriv[i*6 + 3] = mag * forceVector[0];
                deriv[i*6 + 4] = mag * forceVector[1];
                deriv[i*6 + 5] = mag * forceVector[2];
            }
            else 
            {
                deriv[ i*6 ] = mag * forceVector[0];
                deriv[i*6+1] = mag * forceVector[1];
                deriv[i*6+2] = mag * forceVector[2];
                deriv[i*6 + 3] = deriv[i*6 + 4] = deriv[i*6 + 5] = 0.0;
            }
        }
        else 
        {
            deriv[i*6] = deriv[i*6 + 1] = deriv[i*6 + 2] = 
            deriv[i*6 + 3] = deriv[i*6 + 4] = deriv[i*6 + 5] = 0.0;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// void SolarRadiationPressure::FindShadowState(bool &lit, bool &dark, Real *state)
//------------------------------------------------------------------------------
/**
 * Determines lighting conditions at the input location
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
        BuildForceVector(state);
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
            BuildForceVector(state);
            percentSun = 1.0;
            lit = true;
            dark = false;
        }
    }
    else if (shadowModel == CONICAL_MODEL) 
    {
        Real s0, s2, lsc, l1, l2, c1, c2, sinf1, sinf2, tanf1, tanf2;

        s0 = -state[0]*unitsun[0] - state[1]*unitsun[1] - state[2]*unitsun[2];
        s2 = state[0]*state[0] + state[1]*state[1] + state[2]*state[2];
        lsc = sqrt(s2 - s0*s0);

        sinf1 = (sunRadius + bodyRadius) / mag;
        sinf2 = (sunRadius - bodyRadius) / mag;

        // Appropriate l1 and l2 temporarily
        l1 = sinf1 * sinf1;
        l2 = sinf2 * sinf2;
        tanf1 = sqrt(l1 / (1.0 - l1));
        tanf2 = sqrt(l2 / (1.0 - l2));
        
        c1 = s0 + bodyRadius / sinf1;
        c2 = bodyRadius / sinf2 - s0;       // Different sign from M&G

        l1 = c1 * tanf1;
        l2 = c2 * tanf2;

        if (lsc > l1) 
        {
            // Outside of the penumbral cone
            lit = true;
            dark = false;
            percentSun = 1.0;
            BuildForceVector(state);
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
                    BuildForceVector(state);
                    pcbrad = asin(bodyRadius / sqrt(s2));
                    percentSun = (psunrad*psunrad - pcbrad*pcbrad) / 
                                 (psunrad*psunrad);
                    dark = false;
                }
                
                return;
            }
            BuildForceVector(state);
            pcbrad = asin(bodyRadius / sqrt(s2));
            percentSun = ShadowFunction(state);
            lit = false;
            dark = false;
        }
    }
}

//------------------------------------------------------------------------------
// void SolarRadiationPressure::BuildForceVector(Real * state)
//------------------------------------------------------------------------------
/**
 * 
 */
//------------------------------------------------------------------------------
void SolarRadiationPressure::BuildForceVector(Real * state)
{
    // Vector from the spacecraft to the Sun
    forceVector[0] = cbSunVector[0] - state[0]; 
    forceVector[1] = cbSunVector[1] - state[1]; 
    forceVector[2] = cbSunVector[2] - state[2]; 

    Real mag = - sqrt(forceVector[0]*forceVector[0] + 
                        forceVector[1]*forceVector[1] + 
                        forceVector[2]*forceVector[2]);

    // Point the force directly away from the Sun.  This gets modified to sail.
    forceVector[0] /= mag;
    forceVector[1] /= mag;
    forceVector[2] /= mag;

    psunrad = asin(sunRadius / mag);
}

//------------------------------------------------------------------------------
// Real SolarRadiationPressure::ShadowFunction(Real * state)
//------------------------------------------------------------------------------
/**
 * Returns the multiplier used when the satellite is partially lit
 */
//------------------------------------------------------------------------------
Real SolarRadiationPressure::ShadowFunction(Real * state)
{
    Real mag = sqrt(state[0]*state[0] + 
                      state[1]*state[1] + 
                      state[2]*state[2]);
    Real c = acos((state[0]*forceVector[0] + 
                     state[1]*forceVector[1] + 
                     state[2]*forceVector[2]) / mag);

    Real a2 = psunrad*psunrad;
    Real b2 = pcbrad*pcbrad;

    Real x = (c*c + a2 - b2) / (2.0 * c);
    Real y = sqrt(a2 - x*x);

    Real area = a2*acos(x/psunrad) + b2*acos((c-x)/pcbrad) - c*y;

    return 1.0 - area / (M_PI * a2);
}

