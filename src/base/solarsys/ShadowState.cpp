//$Id$
//------------------------------------------------------------------------------
//                               ShadowState
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2014.05.06
//
/**
 * Base class implementation for the ShadowState.
 */
//------------------------------------------------------------------------------

#include "ShadowState.hpp"
#include "SolarSystem.hpp"
#include "GmatConstants.hpp"
#include "GmatDefaults.hpp"
#include "SolarSystemException.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SHADOW_STATE
//#define DEBUG_SHADOW_STATE_INIT
//#define DEBUG_SHADOW_STATE_SUN_VECTOR
//#define DEBUG_SHADOW_STATE_2


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
ShadowState::ShadowState() :
   solarSystem  (NULL),
   sun          (NULL)//,
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ShadowState::ShadowState(const ShadowState &copy) :
   solarSystem  (NULL),
   sun          (NULL)//,
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ShadowState& ShadowState::operator=(const ShadowState &copy)
{
   if (&copy != this)
   {
      solarSystem  = NULL;
      sun          = NULL;
   }

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ShadowState::~ShadowState()
{
   solarSystem  = NULL;
   sun          = NULL;
}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void ShadowState::SetSolarSystem(SolarSystem *ss)
{
   #ifdef DEBUG_SHADOW_STATE_INIT
      MessageInterface::ShowMessage("Setting solar system on ShadowState\n");
   #endif
   solarSystem  = ss;
   sun          = solarSystem->GetBody(GmatSolarSystemDefaults::SUN_NAME);
}



//------------------------------------------------------------------------------
// Real FindShadowState(Real forEpoch, bool &lit, bool &dark,
//                      const std::string &shadowModel, Real *state, Real *cbSun,
//                      Real *sunSat, Real *force, Real sunRad,
//                      Real bodyRad, Real psunrad)
//------------------------------------------------------------------------------
/**
 * Determines lighting conditions at the input location
 *
 * @param <forEpoch> Time at which to compute the ShadowState
 * @param <lit>      Indicates if the spoacecraft is in full sunlight
 * @param <dark>     Indicates if the spacecraft is in umbra
 * @param >shadowModel> specifies ShadowModel ("Cylindrical" or "DualCone")
 * @param <state>    Current spacecraft state relative to its origin
 * @param <cbSun>    Origin-to-Sun vector
 * @param <sunSat>   Sun-to-SC vector
 * @param <force>    Force vector
 * @param <sunRad>   Sun radius
 * @param <bodyRad>  Radius of body
 * @param <psunrad>  Computed quantity related to sun radius and distance
 *
 * NOTE: this code was adapted from original SRP Shadow code
 */
//------------------------------------------------------------------------------
Real ShadowState::FindShadowState(bool &lit, bool &dark,
                          const std::string &shadowModel, Real *state, Real *cbSun,
                          Real *sunSat, Real *force, Real sunRad,
                          Real bodyRad, Real psunrad)
{
   Real      percentSun    = 1;   // default is full sun
   Real mag = GmatMathUtil::Sqrt(cbSun[0]*cbSun[0] +
                                 cbSun[1]*cbSun[1] +
                                 cbSun[2]*cbSun[2]);

   Real unitsun[3];
   unitsun[0] = cbSun[0] / mag;
   unitsun[1] = cbSun[1] / mag;
   unitsun[2] = cbSun[2] / mag;

   Real rdotsun = state[0]*unitsun[0] +
                  state[1]*unitsun[1] +
                  state[2]*unitsun[2];

   if (rdotsun > 0.0) // Sunny side of central body is always fully lit
   {
       lit        = true;
       dark       = false;
       percentSun = 1.0;
      #ifdef DEBUG_SHADOW_STATE
         MessageInterface::ShowMessage(
               "ShadowState::FindShadowState **** Body is fully lit\n");
      #endif
   }
   else
   {
         //  Compute apparent quantities like body radii and distances
         Real apparentSunRadius, apparentBodyRadius, satToSunDist;
         Real satToBodyDist, apparentDistFromSunToBody;
         Real satToSunVec[3], unitSatToSun[3], unitBodyToSat[3];
         satToSunVec[0] = -sunSat[0];
         satToSunVec[1] = -sunSat[1];
         satToSunVec[2] = -sunSat[2];

         satToSunDist = GmatMathUtil::Sqrt(satToSunVec[0]*satToSunVec[0]+
		   satToSunVec[1]*satToSunVec[1]+satToSunVec[2]*satToSunVec[2]);
         satToBodyDist = GmatMathUtil::Sqrt(state[0]*state[0]+
         state[1]*state[1]+state[2]*state[2]);

         apparentSunRadius = GmatMathUtil::ASin(sunRad/satToSunDist);
         apparentBodyRadius = GmatMathUtil::ASin(bodyRad/satToBodyDist);
         unitBodyToSat[0] = state[0]/satToBodyDist;
         unitBodyToSat[1] = state[1]/satToBodyDist;
         unitBodyToSat[2] = state[2]/satToBodyDist;
         unitSatToSun[0] = satToSunVec[0]/satToSunDist;
         unitSatToSun[1] = satToSunVec[1]/satToSunDist;
         unitSatToSun[2] = satToSunVec[2]/satToSunDist;
         apparentDistFromSunToBody = GmatMathUtil::ACos(-unitBodyToSat[0]*unitSatToSun[0]
         -unitBodyToSat[1]*unitSatToSun[1]-unitBodyToSat[2]*unitSatToSun[2]);

       if ((rdotsun <= 0.0)  && (satToBodyDist <= bodyRad))
       {
          //  Unphysical case where s/c is on shadow side but inside the body
          //  Need to trap here to avoid singularities in mathematics later
          lit        = false;
          dark       = true;
          percentSun = 0.0;
          return percentSun;
       }
		 if (apparentDistFromSunToBody >= apparentSunRadius + apparentBodyRadius)
       {
          // This is the full sun light case
          lit        = true;
          dark       = false;
          percentSun = 1.0;
          return percentSun;
         }
		 else if (apparentDistFromSunToBody<=apparentBodyRadius-apparentSunRadius)
		 {
		    // This is the umbra case
			 lit        = false;
          dark       = true;
          percentSun = 0.0;
          return percentSun;
		 }
		 else if (  (GmatMathUtil::Abs(apparentSunRadius-apparentBodyRadius)<apparentDistFromSunToBody)  && 
		     (apparentDistFromSunToBody < apparentSunRadius + apparentBodyRadius) )
		 {
		    // This is the penumbra case
			   Real pcbrad = GmatMathUtil::ASin(bodyRad/satToBodyDist);
			   Real psunrad = GmatMathUtil::ASin(sunRad/satToSunDist);
            Real unitSunToSat[3];

            percentSun = GetPercentSunInPenumbra(state, pcbrad, psunrad, unitSatToSun);
            lit        = false;
            dark       = false;
			   return percentSun;
		 }
		 else
		 {
			// This is the anteumbra case
		    lit        = false;
          dark       = false;
          percentSun =  1 - apparentBodyRadius*apparentBodyRadius/apparentSunRadius*apparentSunRadius;
			return percentSun;
		 }
   }
   return percentSun;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Real GetPercentSunInPenumbra(Real *state,
//                              Real *sunSat, Real *force,
//                              Real pcbrad, Real psunrad)
//------------------------------------------------------------------------------
/**
 * Determines the percent sun given the input epoch, state, sun vector,
 * and radii.
 *
 * @param <state>    Current spacecraft state relative to its origin
 * @param <sunSat>   Sat-to-Sun vector
 * @param <unitSatToSun>  Unit vector from spacecraft to sun
 * @param <pcbrad>   Central body radius over distance
 * @param >psunrad>  Sun radius over distance
 *
 * NOTE: this code was adapted from original SRP Shadow code
 */
//------------------------------------------------------------------------------
Real ShadowState::GetPercentSunInPenumbra(Real *state,
                  Real pcbrad, Real psunrad, Real *unitSatToSun)
{
   Real mag = GmatMathUtil::Sqrt(state[0]*state[0] +
                                 state[1]*state[1] +
                                 state[2]*state[2]);

   // Montenbruck and Gill, eq. 3.87
   Real c = GmatMathUtil::ACos((-state[0]*unitSatToSun[0] -
                                state[1]*unitSatToSun[1] -
                                state[2]*unitSatToSun[2]) / mag);

   Real a2 = psunrad*psunrad;
   Real b2 = pcbrad*pcbrad;

   // Montenbruck and Gill, eq. 3.93
   Real x = (c*c + a2 - b2) / (2.0 * c);
   #ifdef DEBUG_SHADOW_STATE
         MessageInterface::ShowMessage("   sqrt in penumbra calc = %12.10f",a2 - x*x);
   #endif

   Real y = GmatMathUtil::Sqrt(a2 - x*x);

   // Montenbruck and Gill, eq. 3.92
   Real area = a2*GmatMathUtil::ACos(x/psunrad) +
               b2*GmatMathUtil::ACos((c-x)/pcbrad)
               - c*y;

   // Montenbruck and Gill, eq. 3.94
   return 1.0 - area / (GmatMathConstants::PI * a2);
}

