//$Id$
//------------------------------------------------------------------------------
//                               ShadowState
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
//#define DEBUG_SHADOW_STATE1
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

//      if (shadowModel == "Cylindrical")
//      {
//          // In this model, the spacecraft is in darkness if it is within
//          // bodyRadius of the sun-body line; otherwise it is lit
//          Real rperp[3];
//          rperp[0] = state[0] - rdotsun * unitsun[0];
//          rperp[1] = state[1] - rdotsun * unitsun[1];
//          rperp[2] = state[2] - rdotsun * unitsun[2];
//
//          mag = GmatMathUtil::Sqrt(rperp[0]*rperp[0] +
//                                   rperp[1]*rperp[1] +
//                                   rperp[2]*rperp[2]);
//          if (mag < bodyRad)
//          {
//              percentSun = 0.0;
//              lit        = false;
//              dark       = true;
//          }
//          else
//          {
//              percentSun = 1.0;
//              lit        = true;
//              dark       = false;
//          }
//      }
//      else if (shadowModel == "DualCone")
//      {
         
         //  Compute apparent quantities like body radii and distances between bodies
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

         if (sunRad >= satToSunDist)
         {
            lit = true;
            dark = false;
            percentSun = 1.0;
            return percentSun;
         }
         apparentSunRadius = GmatMathUtil::ASin(sunRad/satToSunDist);

         if (bodyRad >= satToBodyDist)
         {
            lit = false;
            dark = true;
            percentSun = 0.0;
            return percentSun;
         }
         apparentBodyRadius = GmatMathUtil::ASin(bodyRad/satToBodyDist);

         unitBodyToSat[0] = state[0]/satToBodyDist;
         unitBodyToSat[1] = state[1]/satToBodyDist;
         unitBodyToSat[2] = state[2]/satToBodyDist;
         unitSatToSun[0] = satToSunVec[0]/satToSunDist;
         unitSatToSun[1] = satToSunVec[1]/satToSunDist;
         unitSatToSun[2] = satToSunVec[2]/satToSunDist;
         apparentDistFromSunToBody = GmatMathUtil::ACos(-unitBodyToSat[0]*unitSatToSun[0]
         -unitBodyToSat[1]*unitSatToSun[1]-unitBodyToSat[2]*unitSatToSun[2]);
		 
		 if (apparentSunRadius + apparentBodyRadius <= apparentDistFromSunToBody)
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
            Real f = GmatMathUtil::Sqrt(force[0]*force[0] + force[1]*force[1] + force[2]*force[2]);
   #ifdef DEBUG_SHADOW_STATE1
         MessageInterface::ShowMessage("  force = %.12lf   apparentDistFromSunToBody = %.12lf     apparentSunRadius = %.12lf  apparentBodyRadius = %.12lf\n", f, apparentDistFromSunToBody, apparentSunRadius, apparentBodyRadius);
   #endif
            //percentSun = GetPercentSunInPenumbra(state, pcbrad, psunrad, force);
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


       //   THis is old code updated as part of fix to GMT-
       //Real s0, s2, lsc, l1, l2, c1, c2, sinf1, sinf2, tanf1, tanf2;
         //// Montenbruck and Gill, eq. 3.79
         //s0  = -state[0]*unitsun[0]  -
         //       state[1]*unitsun[1] -
         //       state[2]*unitsun[2];
         //s2  =  state[0]*state[0] +
         //       state[1]*state[1] +
         //       state[2]*state[2];

         //// Montenbruck and Gill, eq. 3.80
         //lsc = GmatMathUtil::Sqrt(s2 - s0*s0);

         //// Montenbruck and Gill, eq. 3.81
         //sinf1 = (sunRad + bodyRad) / mag;
         //sinf2 = (sunRad - bodyRad) / mag;

         //// Appropriate l1 and l2 temporarily
         //l1    = sinf1 * sinf1;
         //l2    = sinf2 * sinf2;
         //tanf1 = GmatMathUtil::Sqrt(l1 / (1.0 - l1));
         //tanf2 = GmatMathUtil::Sqrt(l2 / (1.0 - l2));

         //// Montenbruck and Gill, eq. 3.82
         //c1    = s0 + bodyRad / sinf1;    // ***
         //c2    = bodyRad / sinf2 - s0;       // Different sign from M&G

         //// Montenbruck and Gill, eq. 3.83
         //l1 = c1 * tanf1;
         //l2 = c2 * tanf2;

         //#ifdef DEBUG_SHADOW_STATE_2
         //   MessageInterface::ShowMessage("before FSS, rdotsun= %12.10f\n",
         //         rdotsun);
         //   MessageInterface::ShowMessage("   ss0 = %12.10f, s2 = %12.10f\n", s0, s2);
         //   MessageInterface::ShowMessage("   lsc = %12.10f\n", lsc);
         //   MessageInterface::ShowMessage("   sinf1 = %12.10f, sinf2 = %12.10f\n", sinf1, sinf2);
         //   MessageInterface::ShowMessage("   tanf1 = %12.10f, tanf2 = %12.10f\n", tanf1, tanf2);
         //   MessageInterface::ShowMessage("   c1 = %12.10f, c2 = %12.10f\n", c1, c2);
         //   MessageInterface::ShowMessage("   l1 = %12.10f, l2 = %12.10f\n", l1, l2);
         //#endif
         //if (lsc > l1)
         //{
         //   // Outside of the penumbral cone
         //   lit        = true;
         //   dark       = false;
         //   percentSun = 1.0;
         //   return percentSun;
         //   #ifdef DEBUG_SHADOW_STATE
         //      MessageInterface::ShowMessage(
         //            "ShadowState::FindShadowState **** Body is outside penumbral zone\n");
         //   #endif
         //}
         //else
         //{
         //   #ifdef DEBUG_SHADOW_STATE
         //      MessageInterface::ShowMessage(
         //            "ShadowState::FindShadowState sunSat = %12.10f  %12.10f  %12.10f\n",
         //            sunSat[0], sunSat[1], sunSat[2]);
         //      MessageInterface::ShowMessage("   sunRadius = %12.10f\n",
         //            sunRad);
         //      MessageInterface::ShowMessage("   bodyRadius = %12.10f, s2 = %12.10f, GmatMathUtil::Sqrt(s2) = %12.10f\n",
         //            bodyRad, s2, GmatMathUtil::Sqrt(s2));
         //   #endif


         //   Real sqrtS2 = GmatMathUtil::Sqrt(s2);
         //   if (sqrtS2 < bodyRad)
         //   {
         //      lit        = false;
         //      dark       = true;
         //      percentSun = 0.0;
         //      return percentSun;
         //   }
         //   Real pcbrad      = GmatMathUtil::ASin(bodyRad / sqrtS2);

         //   lit         = false;
         //   if (lsc < GmatMathUtil::Abs(l2))
         //   {
         //      // Inside umbral cone
         //      if (c2 >= 0.0)
         //      {
         //         // no annular ring
         //         percentSun = 0.0;
         //         dark       = true;
         //      }
         //      else
         //      {
         //         // annular eclipse
         //         percentSun = (psunrad*psunrad - pcbrad*pcbrad) /
         //                      (psunrad*psunrad);
         //         dark       = false;
         //      }
         //      return percentSun;
         //   }
         //   else
         //   {
         //      // In penumbra
         //      percentSun = GetPercentSunInPenumbra(state, pcbrad, psunrad, force);
         //      lit        = false;
         //      dark       = false;
         //   }
         //}
//      }
//      else
//      {
//         std::string errmsg = "ShadowState: unknown shadow model \"";
//         errmsg += shadowModel + "\".\n";
//         throw SolarSystemException(errmsg);
//      }
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
 * @param <unitSunToSat>   Sat-to-Sun unit vector
 * @param <force>    Force vector
 * @param <pcbrad>   Central body radius over distance
 * @param >psunrad>  Sun radius over distance
 *
 * NOTE: this code was adapted from original SRP Shadow code
 */
//------------------------------------------------------------------------------
Real ShadowState::GetPercentSunInPenumbra(Real *state,
                  Real pcbrad, Real psunrad, Real *unitSunToSat)
{
   Real mag = GmatMathUtil::Sqrt(state[0]*state[0] +
                                 state[1]*state[1] +
                                 state[2]*state[2]);

   // Montenbruck and Gill, eq. 3.87
   Real c = GmatMathUtil::ACos((-state[0]*unitSunToSat[0] -
                                state[1]*unitSunToSat[1] -
                                state[2]*unitSunToSat[2]) / mag);

   Real a2 = psunrad*psunrad;
   Real b2 = pcbrad*pcbrad;

   // Montenbruck and Gill, eq. 3.93
   Real x = (c*c + a2 - b2) / (2.0 * c);
   #ifdef DEBUG_SHADOW_STATE1
         MessageInterface::ShowMessage("   sqrt in penumbra calc = %12.10f,  x = %.12lf  c = %.12lf  a = %.12lf  b = %.12lf\n",a2 - x*x, x, c, psunrad, pcbrad);
   #endif

   Real y = GmatMathUtil::Sqrt(a2 - x*x);

   // Montenbruck and Gill, eq. 3.92
   Real area = a2*GmatMathUtil::ACos(x/psunrad) +
               b2*GmatMathUtil::ACos((c-x)/pcbrad)
               - c*y;

   // Montenbruck and Gill, eq. 3.94
   return 1.0 - area / (GmatMathConstants::PI * a2);
}

