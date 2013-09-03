//$Id$
//------------------------------------------------------------------------------
//                         StateConversionUtil
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Wendy Shoan, GSFC/GSSB
// Created: 2011.11.02
//
/**
 * Implementation of the namespace containing methods to convert between
 * orbit state representations.
 *
 */
//------------------------------------------------------------------------------

#include <sstream>                 // for <<
#include "gmatdefs.hpp"
#include "StateConversionUtil.hpp"
#include "GmatDefaults.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"

//#define DEBUG_EQUINOCTIAL
//#define DEBUG_STATE_CONVERSION
//#define DEBUG_STATE_CONVERSION_SQRT
//#define DEBUG_KEPLERIAN
//#define DEBUG_KEPLERIAN_TA
//#define DEBUG_KEPLERIAN_EA
//#define DEBUG_KEPLERIAN_HA
//#define DEBUG_KEPLERIAN_SMA
//#define DEBUG_KEPLERIAN_ECC
//#define DEBUG_KEPLERIAN_INC
//#define DEBUG_KEPLERIAN_RAAN
//#define DEBUG_KEPLERIAN_AOP
//#define DEBUG_KEPLERIAN_AM
//#define DEBUG_KEPLERIAN_ECC_VEC
//#define DEBUG_CART_TO_KEPL
//#define DEBUG_MOD_KEPLERIAN
//#define DEBUG_ANOMALY
//#define DEBUG_ANOMALY_MA
//#define DEBUG_MODKEP_TO_KEP
//#define DEBUG_CONVERT_ERRORS
//#define DEBUG_SC_CONVERT_VALIDATE

using namespace GmatMathUtil;
using namespace GmatMathConstants;

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const Real         StateConversionUtil::ORBIT_TOL          = 1.0E-10;
const Real         StateConversionUtil::ORBIT_TOL_SQ       = 1.0E-20;
const Real         StateConversionUtil::SINGULAR_TOL       = .001;
const Real         StateConversionUtil::INFINITE_TOL       = 1.0E-30;
const Real         StateConversionUtil::PARABOLIC_TOL      = 1.0E-7;
const Real         StateConversionUtil::MU_TOL             = 1.0E-15;
const Real         StateConversionUtil::EQUINOCTIAL_TOL    = 1.0E-5;
const Real         StateConversionUtil::ANGLE_TOL          = 0.0;

const Integer      StateConversionUtil::MAX_ITERATIONS     = 75;

const Real         StateConversionUtil::DEFAULT_MU         =
                   GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH];

const std::string  StateConversionUtil::STATE_TYPE_TEXT[StateTypeCount] =
{
   "Cartesian",
   "Keplerian",
   "ModifiedKeplerian",
   "SphericalAZFPA",
   "SphericalRADEC",
   "Equinoctial",
   "ModifiedEquinoctial", // Modified by M.H.
   "Delaunay",
   "Planetodetic"
};

const bool         StateConversionUtil::REQUIRES_CB_ORIGIN[StateTypeCount] =
{
   false,
   true,
   true,
   false,
   false,
   true,
   // Modified by M.H.
   true,
   true,
   true
};

const bool         StateConversionUtil::REQUIRES_FIXED_CS[StateTypeCount] =
{
   false,
   false,
   false,
   false,
   false,
   false,
   false,
   false,
   true
};


const std::string StateConversionUtil::ANOMALY_LONG_TEXT[AnomalyTypeCount] =
{
   "True Anomaly", "Mean Anomaly", "Eccentric Anomaly", "Hyperbolic Anomaly",
};

const std::string StateConversionUtil::ANOMALY_SHORT_TEXT[AnomalyTypeCount] =
{
   "TA", "MA", "EA", "HA",
};



//------------------------------------------------------------------------------
// static methods
//------------------------------------------------------------------------------


//---------------------------------------------------------------------------
//  Rvector6 Convert(const Rvector6 &state,
//                   const std::string &fromType,
//                   const std::string &toType,
//                   const std::string &fromType,
//                   const std::string &toType,
//                   Real mu         = GmatSolarSystemDefaults::PLANET_MU[
//                                     GmatSolarSystemDefaults::EARTH],
//                   Real flattening = GmatSolarSystemDefaults::PLANET_FLATTENING[
//                                     GmatSolarSystemDefaults::EARTH],
//                   Real eqRadius   = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[
//                                     GmatSolarSystemDefaults::EARTH],
//                   const std::string &anomalyType = "TA")
//                   const std::string &anomalyType = "TA")
//---------------------------------------------------------------------------
/**
 * Converts state from fromType to toType.
 *
 * @param <state>       state to convert
 * @param <fromType>    state type to convert from
 * @param <toType>      state type to convert to
 * @param <mu>          gravitational constant for the central body
 * @param <flattening>  flattening coefficient for the central body
 * @param <eqRadius>    equatorial radius for the central body
 * @param <anomalyType> anomaly type string if toType is Mod/Keplerian
 *
 * @return Converted states from the specific element type
 */
//---------------------------------------------------------------------------
//Rvector6 StateConversionUtil::Convert(Real              mu,
//                                      const Rvector6    &state,
//                                      const std::string &fromType,
//                                      const std::string &toType,
//                                      const std::string &anomalyType)
Rvector6 StateConversionUtil::Convert(const Rvector6 &state,
                              const std::string &fromType, const std::string &toType,
                              Real mu,
                              Real flattening,
                              Real eqRadius,
                              const std::string &anomalyType)
{
   #ifdef DEBUG_STATE_CONVERSION
   MessageInterface::ShowMessage
      ("StateConversionUtil::Convert() fromType=%s, toType=%s, anomalyType=%s\n"
       "   state=%s\n", fromType.c_str(), toType.c_str(), anomalyType.c_str(),
       state.ToString(13).c_str());
   #endif

   if (fromType == toType)
      return state;

   Rvector6 outState;

   try
   {
      // Determine the input state type
      if (fromType == "Cartesian")
      {
         if (toType == "Keplerian" || toType == "ModifiedKeplerian")
         {
            Rvector6 kepl = CartesianToKeplerian(mu, state, anomalyType);

            if (toType == "ModifiedKeplerian")
               outState = KeplerianToModKeplerian(kepl);
            else
               outState = kepl;
         }
         else if (toType == "SphericalAZFPA")
         {
            outState = CartesianToSphericalAZFPA(state);
         }
         else if (toType == "SphericalRADEC")
         {
            outState = CartesianToSphericalRADEC(state);
         }
         else if (toType == "Equinoctial")
         {
            outState = CartesianToEquinoctial(state, mu);
         }
         else if (toType == "ModifiedEquinoctial") // Modified by M.H.
         {
            outState = CartesianToModEquinoctial(state, mu);
         }
         else if (toType == "Delaunay") // Modified by M.H.
         {
            Rvector6 kepl = CartesianToKeplerian(mu, state, anomalyType); 
            outState = KeplerianToDelaunay(kepl, mu); 
         }
         else if (toType == "Planetodetic") // Modified by M.H.
         {
            outState = CartesianToPlanetodetic(state);
         }
         else
         {
            throw UtilityException
               ("Cannot convert the state from \"Cartesian\" to \"" + toType +
                "\". \"" + toType + "\" is an unknown State Type\n");
         }

      } // fromType == "Cartesian"
      else if (fromType == "Keplerian")
      {
         if (toType == "Cartesian")
         {
            outState = KeplerianToCartesian(mu, state, anomalyType);
         }
         else if (toType == "ModifiedKeplerian")
         {
            outState = KeplerianToModKeplerian(state);
         }
         else if (toType == "SphericalAZFPA")
         {
            Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
            outState           = CartesianToSphericalAZFPA(cartesian);
         }
         else if (toType == "SphericalRADEC")
         {
            Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
            outState           = CartesianToSphericalRADEC(cartesian);
         }
         else if (toType == "Equinoctial")
         {
            Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
            outState           = CartesianToEquinoctial(cartesian, mu);
         }
         else if (toType == "ModifiedEquinoctial") // Modified by M.H.
         {
            Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
            outState = CartesianToModEquinoctial(cartesian, mu);
         }
         else if ( toType == "Delaunay" )// Modified by M.H.
         {
            outState = KeplerianToDelaunay(state, mu);
         }
         else if (toType == "Planetodetic") // Modified by M.H.
         {
            Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
            outState = CartesianToPlanetodetic(cartesian);
         }
         else
         {
            throw UtilityException
               ("Cannot convert the state from \"Keperian\" to \"" + toType +
                "\". \"" + toType + " is an unknown State Type\n");
         }
      } // fromType == "Keplerian"
      else if (fromType == "ModifiedKeplerian")
      {
         Rvector6 keplerian = ModKeplerianToKeplerian(state);

         if (toType == "Cartesian")
         {
            outState = KeplerianToCartesian(mu, keplerian, anomalyType);
         }
         else if (toType == "Keplerian")
         {
            outState = keplerian;
         }
         else if (toType == "SphericalAZFPA")
         {
            Rvector6 cartesian = KeplerianToCartesian(mu, keplerian, anomalyType);
            outState           = CartesianToSphericalAZFPA(cartesian);
         }
         else if (toType == "SphericalRADEC")
         {
            Rvector6 cartesian = KeplerianToCartesian(mu, keplerian, anomalyType);
            outState           = CartesianToSphericalRADEC(cartesian);
         }
         else if (toType == "Equinoctial")
         {
            Rvector6 cartesian = KeplerianToCartesian(mu, keplerian, anomalyType);
            outState           = CartesianToEquinoctial(cartesian, mu);
         }
         else if (toType == "ModifiedEquinoctial") // Modified by M.H.
         {
            Rvector6 cartesian = KeplerianToCartesian(mu, keplerian, anomalyType);
            outState = CartesianToModEquinoctial(cartesian, mu);
         }
         else if (toType == "Delaunay") // Modified by M.H.
         {
            outState = KeplerianToDelaunay(keplerian, mu);
         }
         else if (toType == "Planetodetic") // Modified by M.H.
         {
            Rvector6 cartesian = KeplerianToCartesian(mu, keplerian, anomalyType);
            outState           = CartesianToPlanetodetic(cartesian);
         }
         else
         {
            throw UtilityException
               ("Cannot convert the state from \"ModKeplerian\" to \"" + toType +
                "\". \"" + toType + " is an unknown State Type\n");
         }
      } // fromType == "ModKeplerian"
      else if (fromType == "SphericalAZFPA")
      {
         Rvector6 cartesian = SphericalAZFPAToCartesian(state);
         
         if (toType == "Cartesian")
         {
            outState = cartesian;
         }
         else if (toType == "Keplerian")
         {
            outState           = CartesianToKeplerian(mu, cartesian, anomalyType);
         }
         else if (toType == "ModifiedKeplerian")
         {
            Rvector6 keplerian = CartesianToKeplerian(mu, cartesian, anomalyType);
            outState           = KeplerianToModKeplerian(keplerian);
         }
         else if (toType == "SphericalRADEC")
         {
            outState           = CartesianToSphericalRADEC(cartesian);
         }
         else if (toType == "Equinoctial")
         {
            outState           = CartesianToEquinoctial(cartesian, mu);
         }
         else if (toType == "ModifiedEquinoctial") // Modified by M.H.
         {
            outState = CartesianToModEquinoctial(cartesian,mu);
         }
         else if (toType == "Delaunay") // Modified by M.H.
         {
            Rvector6 keplerian = CartesianToKeplerian(mu, cartesian, anomalyType);
            outState           = KeplerianToDelaunay(keplerian,mu);
         }
         else if (toType == "Planetodetic") // Modified by M.H.
         {
            outState           = CartesianToPlanetodetic(cartesian);
         }
         else
         {
            throw UtilityException
               ("Cannot convert the state from \"SphericalAZFPA\" to \"" + toType +
                "\". \"" + toType + " is an unknown State Type\n");
         }
      } // fromType == "SphericalAZFPA"
      else if (fromType == "SphericalRADEC")
      {
         Rvector6 cartesian = SphericalRADECToCartesian(state);
         
         if (toType == "Cartesian")
         {
            outState = cartesian;
         }
         else if (toType == "Keplerian")
         {
            outState           = CartesianToKeplerian(mu, cartesian, anomalyType);
         }
         else if (toType == "ModifiedKeplerian")
         {
            Rvector6 keplerian = CartesianToKeplerian(mu, cartesian, anomalyType);
            outState           = KeplerianToModKeplerian(keplerian);
         }
         else if (toType == "SphericalAZFPA")
         {
            outState           = CartesianToSphericalAZFPA(cartesian);
         }
         else if (toType == "Equinoctial")
         {
            outState           = CartesianToEquinoctial(cartesian, mu);
         }
         else if (toType == "ModifiedEquinoctial") // Modified by M.H.
         {
            outState = CartesianToModEquinoctial(cartesian,mu);
         }
         else if (toType == "Delaunay") // Modified by M.H.
         {
            Rvector6 keplerian = CartesianToKeplerian(mu, cartesian, anomalyType);
            outState = KeplerianToDelaunay(keplerian, mu);
         }
         else if (toType == "Planetodetic") // Modified by M.H.
         {
            outState           = CartesianToPlanetodetic(cartesian);
         }
         else
         {
            throw UtilityException
               ("Cannot convert the state from \"SphericalRADEC\" to \"" + toType +
                "\". \"" + toType + " is an unknown State Type\n");
         }
      } // fromType == "SphericalRADEC"
      else if (fromType == "Equinoctial")
      {
         Rvector6 cartState = EquinoctialToCartesian(state, mu);

         if (toType == "Cartesian")
         {
            outState = cartState;
         }
         else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
         {
            Rvector6 kepl = CartesianToKeplerian(mu, state, anomalyType);

            if (toType == "ModifiedKeplerian")
               outState =  KeplerianToModKeplerian(kepl);
            else
               outState = kepl;
         }
         else if (toType == "SphericalAZFPA")
         {
            outState =  CartesianToSphericalAZFPA(cartState);
         }
         else if (toType == "SphericalRADEC")
         {
            outState = CartesianToSphericalRADEC(cartState);
         }
         else if (toType == "ModifiedEquinoctial") // Modified by M.H.
         {
            outState = CartesianToModEquinoctial(cartState, mu);
         }
         else if (toType == "Delaunay") // Modified by M.H.
         {
            Rvector6 keplerian = CartesianToKeplerian(mu, cartState, anomalyType);
            outState = KeplerianToDelaunay(keplerian, mu);
         }
         else if (toType == "Planetodetic") // Modified by M.H.
         {
            outState           = CartesianToPlanetodetic(cartState);
         }
         else
         {
            throw UtilityException
               ("Cannot convert the state from \"Equinoctial\" to \"" + toType +
                "\". \"" + toType + " is Unknown State Type\n");
         }
      }  // fromType == "Equinoctial"
      else if (fromType == "ModifiedEquinoctial") // Modified by M.H.
      {
         Rvector6 cartState = ModEquinoctialToCartesian(state, mu);
         
         if (toType == "Cartesian")
         {
            outState = cartState;
         }
         else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
         {
            Rvector6 kepl = CartesianToKeplerian(mu, state, anomalyType);

            if (toType == "ModifiedKeplerian")
               outState =  KeplerianToModKeplerian(kepl);
            else
               outState = kepl;
         }
         else if (toType == "SphericalAZFPA")
         {
            outState =  CartesianToSphericalAZFPA(cartState);
         }
         else if (toType == "SphericalRADEC")
         {
            outState = CartesianToSphericalRADEC(cartState);
         }
         else if (toType == "Equinoctial") 
         {
            outState = CartesianToEquinoctial(cartState, mu);
         }
         else if (toType == "Delaunay") // Modified by M.H.
         {
            Rvector6 keplerian = CartesianToKeplerian(mu, cartState, anomalyType);
            outState = KeplerianToDelaunay(keplerian, mu);
         }
         else if (toType == "Planetodetic") // Modified by M.H.
         {
            outState = CartesianToPlanetodetic(cartState);
         }
         else
         {
            throw UtilityException
               ("Cannot convert the state from \"Equinoctial\" to \"" + toType +
                "\". \"" + toType + " is Unknown State Type\n");
         }
      }  // fromType == "ModifiedEquinoctial"
      else if (fromType == "Delaunay") // Modified by M.H.
      {
         Rvector6 kepl = DelaunayToKeplerian(state, mu);
         Rvector6 cart = CartesianToKeplerian(mu, state, anomalyType);
         if (toType == "Cartesian")
         {
            outState = cart;
         }
         else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
         {
            if (toType == "ModifiedKeplerian")
               outState =  KeplerianToModKeplerian(kepl);
            else
               outState = kepl;
         }
         else if (toType == "SphericalAZFPA")
         {
            outState =  CartesianToSphericalAZFPA(cart);
         }
         else if (toType == "SphericalRADEC")
         {
            outState = CartesianToSphericalRADEC(cart);
         }
         else if (toType == "Equinoctial") 
         {
            outState = CartesianToEquinoctial(cart, mu);
         }
         else if (toType == "ModifiedEquinoctial") // Modified by M.H.
         {
            outState = CartesianToModEquinoctial(cart, mu);
         }
         else if (toType == "Planetodetic") // Modified by M.H.
         {
            outState = CartesianToPlanetodetic(cart);
         }
         else
         {
            throw UtilityException
               ("Cannot convert the state from \"Delaunay\" to \"" + toType +
                "\". \"" + toType + " is Unknown State Type\n");
         }
      }  // fromType == "Delaunay"
      
      else if (fromType == "Planetodetic") // Modified by M.H.
      {
         Rvector6 cart = PlanetodeticToCartesian(state);
         
         if (toType == "Cartesian")
         {
            outState = cart;
         }
         else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
         {
            Rvector6 kepl = CartesianToKeplerian(mu, cart, anomalyType);
            
            if (toType == "ModifiedKeplerian")
               outState =  KeplerianToModKeplerian(kepl);
            else
               outState = kepl;
         }
         else if (toType == "SphericalAZFPA")
         {
            outState =  CartesianToSphericalAZFPA(cart);
         }
         else if (toType == "SphericalRADEC")
         {
            outState = CartesianToSphericalRADEC(cart);
         }
         else if (toType == "Equinoctial") 
         {
            outState = CartesianToEquinoctial(cart, mu);
         }
         else if (toType == "ModifiedEquinoctial") // Modified by M.H.
         {
            outState = CartesianToModEquinoctial(cart, mu);
         }
         else if (toType == "Delaunay") // Modified by M.H.
         {
            Rvector6 kepl = CartesianToKeplerian(mu, cart, anomalyType);
            outState = KeplerianToDelaunay(kepl, mu);
         }
         else
         {
            throw UtilityException
               ("Cannot convert the state from \"Planetodetic\" to \"" + toType +
                "\". \"" + toType + " is Unknown State Type\n");
         }
      }  // fromType == "Planetodetic"
      else
      {
         throw UtilityException
            ("StateConversionUtil::Convert() Cannot convert the state \"" +
             fromType + "\" to \"" + toType + "\". \"" + fromType +
             " is an unknown State Type\n");
      }
   }
   catch(UtilityException &ue)
   {
      throw ue;
   }
   
   #ifdef DEBUG_STATE_CONVERSION
   MessageInterface::ShowMessage
      ("StateConversionUtil::Convert() returning \n   %s\n", outState.ToString(13).c_str());
   #endif
   
   return outState;
}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 CartesianToModEquinoctial(const Rvector6& cartesian, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to ModifiedEquinoctial.
 *
 * @param <cartesian>     Cartesian state
 * @param <mu>            Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Cartesian to ModifiedEquinoctial
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToModEquinoctial(const Rvector6& cartesian, const Real& mu)
{
   //#ifdef DEBUG_MODEQUINOCTIAL
      //MessageInterface::ShowMessage("Converting from Cartesian to ModEquinoctial: \n");
      //MessageInterface::ShowMessage("   input Cartesian is: %s\n", cartesian.ToString().c_str());
      //MessageInterface::ShowMessage("   mu is:              %12.10f\n", mu);
   //#endif
   
   Real p_mee, f_mee, g_mee, h_mee, k_mee, L_mee; // modequinoctial elements
   Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
   Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
   Real rMag = pos.GetMagnitude();
   Real vMag = vel.GetMagnitude();
   
   if (rMag <= 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Modified Equinoctial - position vector is zero vector.\n");
   }
   
   if (mu < MU_TOL)
   {
      throw UtilityException("Cannot convert from Cartesian to Modified Equinoctial - gravitational constant is zero.\n");
   }
   
   Rvector3 hVec = Cross(pos, vel);
   Real hMag = hVec.GetMagnitude();
   
   Rvector3 rHat, vHat, hHat;
   if ( rMag == 0 )
   {
      rHat[0] = 0; rHat[1] = 0; rHat[2] = 0;
   }
   else 
   {
      rHat = pos.GetUnitVector();
   }
   
   if ( hMag == 0 )
   {
      hHat[0] = 0; hHat[1] = 0; hHat[2] = 0;
      vHat[0] = 0; vHat[1] = 0; vHat[2] = 0;
   }
   else
   {
      hHat = hVec.GetUnitVector();
      Real dotpv = pos[0]*vel[0] + pos[1]*vel[1] + pos[2]*vel[2];
      vHat = ( rMag*vel - dotpv/rMag*pos )/hMag;
   }
   
   Rvector3 eVec=   CartesianToEccVector(mu, pos, vel);
   
   p_mee = (hMag * hMag) /mu;
   
   if ( p_mee < 0 )
   {
      throw UtilityException("Semi-latus rectum has to be greater than 0.\n");
   }
   
   Real j = 1;
   Real denom = ( 1 + hHat[2]*j );

   if ( Abs(denom) < 1.0E-7 )
   {
      std::string warn = "Warning: Singularity may occur during calculate Modified Equinoctial element h and k.";
      MessageInterface::PopupMessage(Gmat::WARNING_, warn);
   }
   else if ( Abs(denom ) < 1.0E-16 )
   {
      throw UtilityException("Singularity occurs during calculate Modified Equinoctial element h and k.\n");
   }
   
   // Define modequinoctial coordinate system
   Rvector3 f;
   f[0]      =   1.0 - ((hHat[0] * hHat[0]) / denom);
   f[1]      = - (hHat[0] * hHat[1]) / denom;
   f[2]      = - hHat[0]*j ;
   f         = f.GetUnitVector();
   
   Rvector3 g = Cross(hHat,f).GetUnitVector();
   
   f_mee =   eVec * f;
   g_mee =   eVec * g;
   k_mee =   hHat[0] / denom ;
   h_mee = - hHat[1] / denom ;

   // Calculate True longitude
   Real sinl = rHat[1] - vHat[0];
   Real cosl = rHat[0] + vHat[1];

   L_mee = atan2(sinl,cosl);
   
   while ( L_mee > TWO_PI )
   {
      L_mee -= TWO_PI;
   }
   while ( L_mee < 0 )
   {
      L_mee += TWO_PI;
   }
   
   L_mee = L_mee * DEG_PER_RAD;
   
   return Rvector6(p_mee, f_mee, g_mee, h_mee, k_mee, L_mee);
   
}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 ModEquinoctialToCartesian(const Rvector6& modequinoctial, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from ModifiedEquinoctial to Cartesian.
 *
 * @param <modequinoctial>     Modified Equinoctial state
 * @param <mu>              Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Modified Equinoctial to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::ModEquinoctialToCartesian(const Rvector6& modequinoctial, const Real& mu)
{
   //#ifdef DEBUG_MODEQUINOCTIAL
      //MessageInterface::ShowMessage("Converting from Modified Equinoctial to Cartesian: \n");
      //MessageInterface::ShowMessage("   input Modified Equinoctial is: %s\n", modequinoctial.ToString().c_str());
      //MessageInterface::ShowMessage("   mu is:              %12.10f\n", mu);
   //#endif
   Rvector3 pos;
   Rvector3 vel; // Cartesian elements

   Real p_mee= modequinoctial[0];   // Semi-latus rectum
   Real f_mee= modequinoctial[1];   // projection of eccentricity vector onto x
   Real g_mee= modequinoctial[2];   // projection of eccentricity vector onto y
   Real h_mee= modequinoctial[3];   // projection of N onto x
   Real k_mee= modequinoctial[4];   // projection of N onto y
   Real L_mee= modequinoctial[5]*RAD_PER_DEG;// True longitude

   Real j = 1; // regrograde factor

   if (mu < MU_TOL)
   {
      throw UtilityException("Cannot convert from Modified Equinoctial to Cartesian - gravitational constant is zero.\n");
   }
   
   if ( p_mee < 0 )
   {
      throw UtilityException("Cannot convert from Modified Equinoctial to Cartesian: Semi-latus rectum has to be greater than 0");
   }
   
   Real r= p_mee / (1 + f_mee*Cos(L_mee) + g_mee * Sin(L_mee));
   Real X1= r * Cos(L_mee);
   Real Y1= r * Sin(L_mee);
   
   Real dotX1, dotY1;
   if ( p_mee == 0)
   {
      dotX1= 0;
      dotY1= 0;
   }
   else
   {
      dotX1= -Sqrt(mu/p_mee) * ( g_mee + Sin(L_mee) );
      dotY1= Sqrt(mu/p_mee) * ( f_mee + Cos(L_mee) );
   }
   
   Real alpha2= h_mee*h_mee - k_mee*k_mee;
   Real s2= 1 + h_mee*h_mee + k_mee*k_mee;
   
   Rvector3 f_hat, g_hat;
   f_hat[0] = (1 + alpha2)/s2;
   f_hat[1] = (2 * k_mee * h_mee)/s2;
   f_hat[2] = (-2 * k_mee * j)/s2;
   
   g_hat[0] = ( 2 * k_mee * h_mee * j ) /s2;
   g_hat[1] = ( (1 - alpha2) * j )/s2;
   g_hat[2] = ( 2 * h_mee ) /s2;
   
   pos = X1 * f_hat + Y1 * g_hat;
   vel = dotX1 * f_hat + dotY1 * g_hat;
   
   return Rvector6(pos[0], pos[1], pos[2], vel[0], vel[1], vel[2] );
}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 KeplerianToDelaunay(const Rvector6& keplerian, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from Keplerian to Delaunay
 *
 * @param <keplerian>Keplerian state
 * @param <mu>              Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Keplerian to Delaunay
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::KeplerianToDelaunay(const Rvector6& keplerian, const Real& mu)
{
	Real sma= keplerian[0];
	Real ecc= keplerian[1];
	Real inc= keplerian[2]*RAD_PER_DEG;
	Real ta= keplerian[5]*RAD_PER_DEG;

	if ( ecc < 0.0)
	{
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Eccentricity (" << ecc << ") cannot be less than 0.0.";
      errmsg << " The sign of the eccentricity has been changed.\n";
      MessageInterface::ShowMessage(errmsg.str());

      ecc *= -1.0;
	}

   if ((sma > 0.0) && (ecc > 1.0))
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Semimajor axis (" << sma << ") cannot be positive if"; 
      errmsg << " eccentricity (" << ecc << ") is greater than 1.0.";
      errmsg << " The sign of the semimajor axis has been changed.";
      errmsg << " If changing orbit from hyperbolic to elliptic, set eccentricity first.\n" << std::endl;
      MessageInterface::ShowMessage(errmsg.str());
      sma *= -1.0;
   }
   
   if ((sma < 0.0) && (ecc < 1.0))
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Semimajor axis (" << sma << ") cannot be negative if ";
      errmsg << " eccentricity (" << ecc << ") is less than 1.0.";
      errmsg << " The sign of the semimajor axis has been changed.";
      errmsg << " If changing orbit from elliptic to hyperbolic, set eccentricity first.\n" << std::endl;
      MessageInterface::ShowMessage(errmsg.str());
      
      sma *= -1.0;
   }
   
   if (mu < MU_TOL)
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "Gravitational constant (" << mu << ") is too small to convert"; 
      errmsg << " from Keplerian to Cartesian state." << std::endl;
      throw UtilityException(errmsg.str());
   }
   else
   {
      // Test that radius of periapsis is not too small.
      Real absA1E = Abs(sma * (1.0 - ecc));
      if (absA1E < SINGULAR_TOL)
      {
         std::stringstream errmsg("");
         errmsg.precision(16);
         errmsg << "A nearly singular conic section was encountered while converting from" ;
         errmsg << "  the Keplerian elements to the Cartesian state. The radius of periapsis(" ;
         errmsg << absA1E << ") must be greater than 1 meter." << std::endl;
         throw UtilityException(errmsg.str());
      }
      //  Verify that orbit is not too close to a parabola which results in undefined SMA
      Real oneMinusE = Abs(1.0 - ecc);
      if (oneMinusE < PARABOLIC_TOL)
      {
         std::stringstream errmsg("");
         errmsg.precision(16);
         errmsg << "A nearly parabolic orbit";
         errmsg << " (ECC = " << ecc << ") was encountered";
         errmsg << " while converting from the Keplerian elements to" ;
         errmsg << " the Cartesian state.";
         errmsg << " The Keplerian elements are";
         errmsg << " undefined for a parabolic orbit." << std::endl;
         throw UtilityException(errmsg.str());
      }
   }
   
   if ( ecc > 1.0 )
   {
      #ifdef DEBUG_CONVERT_ERRORS
      MessageInterface::ShowMessage("Attempting to test for impossible TA:  ecc = %12.10f\n", ecc);
      #endif
      
      Real possible = PI - ACos(1.0/ecc);
      
      while ( ta > PI )ta -= TWO_PI;
      while ( ta < PI)ta += TWO_PI;
      
      if ( Abs(ta ) >= possible )
      {
         possible *= DEG_PER_RAD;
         std::stringstream errmsg;
           errmsg.precision(12);
           errmsg << "\nError: The TA value is not physically possible for a hyperbolic orbit ";
           errmsg << "with the input values of SMA and ECC (or RadPer and RadApo).\nThe allowed values are: ";
           errmsg << "[" << -possible << " < TA < " << possible << " (degrees)]\nor equivalently: ";
           errmsg << "[TA < " << possible << " or TA > " << (360.0 - possible) << " (degrees)]\n";
           throw UtilityException(errmsg.str());
      }
   }
   
   Real L_dela= Sqrt(mu * sma);
   Real G_dela= L_dela * Sqrt(1 - ecc*ecc);
   Real H_dela= G_dela * Cos(inc);
   Real ll_dela= TrueToMeanAnomaly(ta,ecc) * DEG_PER_RAD;
   Real gg_dela= keplerian[4];
   Real hh_dela= keplerian[3];

   return Rvector6( ll_dela, gg_dela, hh_dela, L_dela, G_dela, H_dela );

}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 DelaunayToKeplerian(const Rvector6& delaunay, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from Delaunay to Keplerian
 *
 * @param <keplerian>Delaunay state
 * @param <mu>              Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Delaunay to Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::DelaunayToKeplerian(const Rvector6& delaunay, const Real& mu)
{
   Real L_dela= delaunay[3];
   Real G_dela= delaunay[4];
   Real H_dela= delaunay[5];
   Real ll_dela= delaunay[0]*RAD_PER_DEG;
   
   Real sma= L_dela*L_dela / mu;
   Real ecc= Sqrt ( 1 - (G_dela/L_dela)*(G_dela/L_dela) );
   Real inc= ACos(H_dela / G_dela) * DEG_PER_RAD;
   Real aop= delaunay[1];
   Real raan= delaunay[2];
   Real ta= MeanToTrueAnomaly(ll_dela, ecc) * DEG_PER_RAD;
   
   return Rvector6( sma, ecc, inc, raan, aop, ta );
}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 CartesianToPlanetodetic(const Rvector6& cartesian)
//------------------------------------------------------------------------------
/**
 * Converts from Planetocentric to Cartesian.
 *
 * @param <cartesian>     Cartesian state
 *
 * @return Spacecraft orbit state converted from Cartesian to Planetodetic
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToPlanetodetic(const Rvector6& cartesian)
{
   // Convert Cartesian state to Planetocentric state
   Rvector6 planetocentric= CartesianToSphericalAZFPA(cartesian);
   
   Real Req = 6378.1363; // equatorial radius
   Real f = 0.0033527; // flattening coefficients
   
   //Real Req = GetCentralBody()->GetEquatorialRadius();
   //Real f = GetCentralBody()->GetFlattening();
   
   Real rMag = planetocentric[0];
   Real lon = planetocentric[1]; // longitude
   Real latg = planetocentric[2] * RAD_PER_DEG; // planetocentric latitude
   Real vMag = planetocentric[3];
   Real azi = planetocentric[4];
   Real vfpa = planetocentric[5]; // vertical flight path angle
   Real hfpa = 90 - vfpa;// horizontal flight path angle
   
   
   // Convert planetocentric latitude to Planetodetic latitude
   Real r_z = cartesian[2];
   Real r_xy = Sqrt(cartesian[0]*cartesian[0] + cartesian[1]*cartesian[1]);
   Real latd = latg;
   Real e2 = 2*f - f*f;
   Real tol = 1;
   
   while( tol >= 1e-13)
   {
      Real latd_old = latd;
      Real C = Req / Sqrt( 1 - e2*Sin(latd_old)*Sin(latd_old) );
      latd = ATan( (r_z + C*e2*Sin(latd_old))/r_xy );
      
      tol = Abs(latd - latd_old);
   }
   
   return Rvector6(rMag, lon, latd * DEG_PER_RAD, vMag, azi, hfpa);
   
}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 PlanetodeticToCartesian(const Rvector6& planetodetic)
//------------------------------------------------------------------------------
/**
 * Converts from Planetodetic to Cartesian.
 *
 * @param <planetodetic>     Planetodetic state
 *
 * @return Spacecraft orbit state converted from Planetodetic to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::PlanetodeticToCartesian(const Rvector6& planetodetic)
{
   Real Req = 6378.1363;
   Real f = 0.0033527;
   
   Real rMag = planetodetic[0];
   Real lon = planetodetic[1] * RAD_PER_DEG;
   Real latd = planetodetic[2] * RAD_PER_DEG;
   Real vMag = planetodetic[3];
   Real azi = planetodetic[4];
   Real hfpa = planetodetic[5];
   
   Real vfpa = 90 - hfpa;
   
// convert plantodetic latitude to planetocentric latitude
   Real e2 = 2*f - f*f;
   Real tol = 1;
   Real latg = latd;
   
   while(tol >= 1e-13)
   {
      Real latg_old = latg;
      
      Real x = rMag * Cos(latg_old) * Cos(lon);
      Real y = rMag * cos(latg_old) * Sin(lon);
      
      Real r_xy = Sqrt(x*x + y*y);
      Real alt = r_xy/Cos(latd) - Req/Sqrt(1-e2*Sin(latd)*Sin(latd));
      
      Real Sin2 = Sin(2*latd);
      Real Sin4 = Sin(4*latd);
      Real h_hat = alt/Req;
      Real denom = ( h_hat + 1);
      
      latg = latd + (-Sin2/denom)*f + ( (-Sin2)/(2*denom*denom) + (1/(4*denom*denom) + 1/(4*denom))*Sin4 )*f*f;
      
      tol = Abs(latg - latg_old);
   }
   
   Rvector6 planetocentric;
   planetocentric[0] =  rMag; 
   planetocentric[1] = lon*DEG_PER_RAD;
   planetocentric[2] = latg*DEG_PER_RAD;
   planetocentric[3] = vMag;
   planetocentric[4] = azi;
   planetocentric[5] = vfpa;
   
   // convert planetocentric to cartesian state
   Rvector6 cartesian = SphericalAZFPAToCartesian(planetocentric);
   
   return cartesian;
}

//---------------------------------------------------------------------------
//  Rvector6 Convert(Real mu,
//                   Real *state,
//                   const std::string &fromType,
//                   const std::string &toType,
//                   Real mu         = GmatSolarSystemDefaults::PLANET_MU[
//                                     GmatSolarSystemDefaults::EARTH],
//                   Real flattening = GmatSolarSystemDefaults::PLANET_FLATTENING[
//                                     GmatSolarSystemDefaults::EARTH],
//                   Real eqRadius   = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[
//                                     GmatSolarSystemDefaults::EARTH],
//                   const std::string &anomalyType = "TA")
//---------------------------------------------------------------------------
/**
 * Converts from fromType to toType.
 *
 * @param <state>       state to convert
 * @param <fromType>    state type to convert from
 * @param <toType>      state type to convert to
 * @param <mu>          gravitational constant for the central body
 * @param <flattening>  flattening coefficient for the central body
 * @param <eqRadius>    equatorial radius for the central body
 * @param <anomalyType> anomaly type string if toType is Mod/Keplerian
 *
 * @return Converted states from the specific element type
 */
//---------------------------------------------------------------------------
//Rvector6 StateConversionUtil::Convert(Real mu,
//                                      const Real *state,
//                                      const std::string &fromType,
//                                      const std::string &toType,
//                                      const std::string &anomalyType)
Rvector6 StateConversionUtil::Convert(const Real *state,
                              const std::string &fromType, const std::string &toType,
                              Real mu,
                              Real flattening,
                              Real eqRadius,
                              const std::string &anomalyType)
{
   Rvector6 newState;
   newState.Set(state[0], state[1], state[2], state[3], state[4], state[5]);

   if (fromType == toType)
      return newState;

   return Convert(newState, fromType, toType, mu, flattening, eqRadius, anomalyType);
}

//------------------------------------------------------------------------------
// Rvector6 CartesianToKeplerian(Real mu, const Rvector3 &pos,
//                               const Rvector3 &vel,
//                               AnomalyType anomalyType)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to Keplerian.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <anomalyType>   Anomaly type
 *
 * @return Spacecraft orbit state converted from Cartesian to Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToKeplerian(Real mu, const Rvector3 &pos,
                                                   const Rvector3 &vel,
                                                   AnomalyType anomalyType)
{
   #ifdef DEBUG_KEPLERIAN
   MessageInterface::ShowMessage("CartesianToKeplerian() ");
   MessageInterface::ShowMessage("                   pos = %s and  vel = %s\n", pos.ToString().c_str(), vel.ToString().c_str());
   #endif

   Real tfp, ma;
   Real p[3], v[3];
   for (unsigned int ii = 0; ii < 3; ii++)
   {
      p[ii] = pos[ii];
      v[ii] = vel[ii];
   }

   Real     kepOut[6];
   kepOut[0] = kepOut[1] = kepOut[2] = kepOut[3] = kepOut[4] = kepOut[5] = 0.0;
   Integer retval = ComputeCartToKepl(mu, p, v, &tfp, kepOut, &ma);
   if (retval != 0)
   {
      // only non-zero retval is 2, which did indicate a zero mu; ignore for now, as we did before
   }

   Real anomaly = kepOut[5];
   Real sma = kepOut[0];
   Real ecc = kepOut[1];
   Real ta  = kepOut[5];

   if (anomalyType != TA)
   {
      anomaly = ConvertFromTrueAnomaly(anomalyType, ta, ecc);
   }
   Rvector6 kep(sma, ecc, kepOut[2], kepOut[3], kepOut[4], anomaly);

   #ifdef DEBUG_KEPLERIAN
   MessageInterface::ShowMessage("returning %s\n", kep.ToString().c_str());
   #endif

   return kep;
}


//------------------------------------------------------------------------------
// Rvector6 CartesianToKeplerian(Real mu, const Rvector3 &pos,
//                               const Rvector3 &vel,
//                               const std::string &anomalyType = "TA")
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to Keplerian.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <anomalyType>   Anomaly type
 *
 * @return Spacecraft orbit state converted from Cartesian to Keplerian
 */
//---------------------------------------------------------------------------
 Rvector6 StateConversionUtil::CartesianToKeplerian(Real mu, const Rvector3 &pos,
                                                   const Rvector3 &vel,
                                                   const std::string &anomalyType)
{
   AnomalyType type = GetAnomalyType(anomalyType);
   return CartesianToKeplerian(mu, pos, vel, type);
}


//------------------------------------------------------------------------------
// Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state
//                               AnomalyType anomalyType)
//------------------------------------------------------------------------------
 /**
  * Converts from Cartesian to Keplerian.
  *
  * @param <mu>            Gravitational constant for the central body
  * @param <state>         Cartesian state
  * @param <anomalyType>   Anomaly type
  *
  * @return Spacecraft orbit state converted from Cartesian to Keplerian
  */
 //---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToKeplerian(Real mu, const Rvector6 &state,
                                                   AnomalyType anomalyType)
{
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);
   return CartesianToKeplerian(mu, pos, vel, anomalyType);
}


//------------------------------------------------------------------------------
// Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state
//                               const std::string &anomalyType = "TA")
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to Keplerian.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <state>         Cartesian state
 * @param <anomalyType>   Anomaly type
 *
 * @return Spacecraft orbit state converted from Cartesian to Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToKeplerian(Real mu, const Rvector6 &state,
                                                   const std::string &anomalyType)
{
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);
   return CartesianToKeplerian(mu, pos, vel, anomalyType);
}

//------------------------------------------------------------------------------
// Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state, Real *ma)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to Keplerian.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <state>         Cartesian state
 * @param <ma>            Mean Anomaly
 *
 * @return Spacecraft orbit state converted from Cartesian to Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToKeplerian(Real mu, const Rvector6 &state, Real *ma)
{
   #ifdef DEBUG_CART_TO_KEPL
      MessageInterface::ShowMessage("StateConversionUtil::CartesianToKeplerian called ... \n");
   #endif

   Real     kepl[6];
   Real     r[3];
   Real     v[3];
   Real     tfp;
   Integer  ret;
   Integer  errorCode;

   for (int i=0; i<6; i++)
      kepl[i] = 0.0;

   if(mu < MU_TOL)
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "Gravitational constant (" << mu << ") is too small to convert"; 
      errmsg << " from Keplerian to Cartesian state." << std::endl;
      throw UtilityException(errmsg.str());
   }
   else
   {
      state.GetR(r);
      state.GetV(v);


      if (IsRvValid(r,v))
      {
         errorCode = ComputeCartToKepl(mu, r, v, &tfp, kepl, ma);

         switch (errorCode)
         {
         case 0: // no error
            ret = 1;
            break;
         case 2:
            throw UtilityException
               ("Gravity constant too small for conversion to Keplerian elements\n");
         default:
            throw UtilityException
               ("Unable to convert Cartesian elements to Keplerian\n");
         }
      }
      else
      {
         std::stringstream ss;
         ss << state;
         throw UtilityException
            ("Invalid Cartesian elements:\n" +
             ss.str());
      }
   }

   Rvector6 keplVec = Rvector6(kepl[0], kepl[1], kepl[2], kepl[3], kepl[4], kepl[5]);
   return keplVec;
}


//------------------------------------------------------------------------------
// Rvector6 KeplerianToCartesian(Real mu, const Rvector6 &state,
//                               AnomalyType anomalyType)
//------------------------------------------------------------------------------
/**
 * Converts from Keplerian to Cartesian.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <state>         Keplerian state
 * @param <anomalyType>   Anomaly Type
 *
 * @return Spacecraft orbit state converted from Keplerian to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::KeplerianToCartesian(Real mu, const Rvector6 &state,
                                                   AnomalyType anomalyType)
{
   Integer   ret = 1;
   Real      temp_r[3];
   Real      temp_v[3];
   Real      kepl[6];
   Rvector6  cartVec;

   for (int i=0; i<6; i++)
      kepl[i] = state[i];

   //  These checks test for invalid combination of ECC and SMA. -SPH
   if (kepl[1] < 0.0)
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Eccentricity (" << kepl[1] << ") cannot be less than 0.0.";
      errmsg << " The sign of the eccentricity has been changed.\n";
      MessageInterface::ShowMessage(errmsg.str());

      kepl[1] *= -1.0;
   }
   if ((kepl[0] > 0.0) && (kepl[1] > 1.0))
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Semimajor axis (" << kepl[0] << ") cannot be positive if"; 
      errmsg << " eccentricity (" << kepl[1] << ") is greater than 1.0.";
      errmsg << " The sign of the semimajor axis has been changed.";
      errmsg << " If changing orbit from hyperbolic to elliptic, set eccentricity first.\n" << std::endl;
      MessageInterface::ShowMessage(errmsg.str());
      kepl[0] *= -1.0;
   }
   if ((kepl[0] < 0.0) && (kepl[1] < 1.0))
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Semimajor axis (" << kepl[0] << ") cannot be negative if ";
      errmsg << " eccentricity (" << kepl[1] << ") is less than 1.0.";
      errmsg << " The sign of the semimajor axis has been changed.";
      errmsg << " If changing orbit from elliptic to hyperbolic, set eccentricity first.\n" << std::endl;
      MessageInterface::ShowMessage(errmsg.str());
      
      kepl[0] *= -1.0;
      ret = 1;
   }

   //  These checks test for invalid mu, singular conic sections,
   //  or numerical edge conditions -SPH
   if (ret)
   {
      //  Test that mu is not too small to avoid divide by zero.
      if (mu < MU_TOL)
      {
         std::stringstream errmsg("");
         errmsg.precision(16);
         errmsg << "Gravitational constant (" << mu << ") is too small to convert"; 
         errmsg << " from Keplerian to Cartesian state." << std::endl;
         throw UtilityException(errmsg.str());
      }
      else
      {
         // Test that radius of periapsis is not too small.
         Real absA1E = Abs(kepl[0] * (1.0 - kepl[1]));
         if (absA1E < SINGULAR_TOL)
         {
            std::stringstream errmsg("");
            errmsg.precision(16);
            errmsg << "A nearly singular conic section was encountered while converting from"; 
            errmsg << "  the Keplerian elements to the Cartesian state. The radius of periapsis("; 
            errmsg << absA1E << ") must be greater than 1 meter." << std::endl;
            throw UtilityException(errmsg.str());
         }
         //  Verify that orbit is not too close to a parabola which results in undefined SMA
         Real oneMinusE = Abs(1.0 - kepl[1]);
         if (oneMinusE < PARABOLIC_TOL)
         {
            std::stringstream errmsg("");
            errmsg.precision(16);
            errmsg << "A nearly parabolic orbit";
            errmsg << " (ECC = " << kepl[1] << ") was encountered";
            errmsg << " while converting from the Keplerian elements to"; 
            errmsg << " the Cartesian state.";
            errmsg << " The Keplerian elements are";
            errmsg << " undefined for a parabolic orbit." << std::endl;
            throw UtilityException(errmsg.str());
         }
         //  Verify that if orbit is hyperbolic, TA is realistic.
         if (kepl[1] > 1.0)
         {
            #ifdef DEBUG_CONVERT_ERRORS
               MessageInterface::ShowMessage("Attempting to test for impossible TA:  ecc = %12.10f\n", kepl[1]);
            #endif
            Real possible  = PI - ACos(1.0 / kepl[1]);
            Real taM       = kepl[5]  * RAD_PER_DEG;
            while (taM > PI)   taM -= TWO_PI;
            while (taM < -PI)  taM += TWO_PI;
            if (Abs(taM) >= possible)
            {
               possible *= DEG_PER_RAD;
               std::stringstream errmsg;
               errmsg.precision(12);
               errmsg << "\nError: The TA value is not physically possible for a hyperbolic orbit ";
               errmsg << "with the input values of SMA and ECC (or RadPer and RadApo).\nThe allowed values are: ";
               errmsg << "[" << -possible << " < TA < " << possible << " (degrees)]\nor equivalently: ";
               errmsg << "[TA < " << possible << " or TA > " << (360.0 - possible) << " (degrees)]\n";
               throw UtilityException(errmsg.str());
            }
         }
         //  Verify that position is not too large for the machine
         Real infCheck  = 1.0 + kepl[1] * Cos(kepl[5] * RAD_PER_DEG);
         if (infCheck < INFINITE_TOL)
         {
            std::string errmsg = "A near infinite radius was encountered while converting from ";
            errmsg += "the Keplerian elements to the Cartesian state.\n";
            throw UtilityException(errmsg);
         }
        
         // if the return code from a call to compute_kepl_to_cart is greater than zero, there is an error
         Integer errorCode = ComputeKeplToCart(mu, kepl, temp_r, temp_v, anomalyType);
         if (errorCode > 0)
         {
            if (errorCode == 2)
            {
               std::stringstream errmsg("");
               errmsg.precision(16);
               errmsg << "A nearly parabolic orbit (ECC = " << kepl[1] << ") was encountered";
               errmsg << " while converting from the Keplerian elements to the Cartesian state.";
               errmsg << " The Keplerian elements are undefined for a parabolic orbit." << std::endl;
               throw UtilityException(errmsg.str());
            }
            else
            {
               //  Hope we never hit this.  This is a last resort, something went wrong.
               throw UtilityException
               ("Unable to convert Keplerian elements to Cartesian state.\n");
            }
         }
         else
         {
            // build the state vector
            cartVec = Rvector6(temp_r[0], temp_r[1], temp_r[2],
                               temp_v[0], temp_v[1], temp_v[2]);
         }
      }
   }

   return cartVec;
}

//------------------------------------------------------------------------------
// Rvector6 KeplerianToCartesian(Real mu, const Rvector6 &state,
//                               const std::string &anomalyType = "TA")
//------------------------------------------------------------------------------
/**
 * Converts from Keplerian to Cartesian.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <state>         Keplerian state
 * @param <anomalyType>   Anomaly Type
 *
 * @return Spacecraft orbit state converted from Keplerian to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::KeplerianToCartesian(Real mu, const Rvector6 &state,
                                                   const std::string &anomalyType)
{
   AnomalyType type = GetAnomalyType(anomalyType);
   return KeplerianToCartesian(mu, state, type);
}



//------------------------------------------------------------------------------
// Rvector6 CartesianToEquinoctial(const Rvector6& cartesian, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to Equinoctial.
 *
 * @param <cartesian>     Cartesian state
 * @param <mu>            Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Cartesian to Equinoctial
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToEquinoctial(const Rvector6& cartesian, const Real& mu)
{
   #ifdef DEBUG_EQUINOCTIAL
      MessageInterface::ShowMessage("Converting from Cartesian to Equinoctial: \n");
      MessageInterface::ShowMessage("   input Cartesian is: %s\n", cartesian.ToString().c_str());
      MessageInterface::ShowMessage("   mu is:              %12.10f\n", mu);
   #endif
   Real sma, h, k, p, q, lambda; // equinoctial elements

   Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
   Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
   Real r = pos.GetMagnitude();
   Real v = vel.GetMagnitude();

   if (r <= 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Equinoctial - position vector is zero vector.\n");
   }
   if (mu < MU_TOL)
   {
      throw UtilityException("Cannot convert from Cartesian to Equinoctial - gravitational constant is zero.\n");
   }

   Rvector3 eVec = ( ((v*v - mu/r) * pos) - ((pos * vel) * vel) ) / mu;
   Real e = eVec.GetMagnitude();

   // Check for a near parabolic or hyperbolic orbit.
   if ( e > 1.0 - GmatOrbitConstants::KEP_ECC_TOL)
   {
      std::string errmsg =
            "Cannot convert from Cartesian to Equinoctial - the orbit is either parabolic or hyperbolic.\n";
      throw UtilityException(errmsg);
   }

   Real xi  = (v * v / 2.0) - (mu / r);
   sma      = - mu / (2.0 * xi);

   // Check to see if the conic section is nearly singular
   if (Abs(sma * (1.0 - e)) < .001)
   {
      #ifdef DEBUG_EQUINOCTIAL
         MessageInterface::ShowMessage(
               "Equinoctial ... failing check for singular conic section ... e = %12.10f, sma = %12,10f\n",
               e, sma);
      #endif
      std::string errmsg =
         "Cannot convert from Cartesian to Equinoctial: The state results in a singular conic section with radius of periapsis less than 1 m.\n";
      throw UtilityException(errmsg);
   }

   Rvector3 am = Cross(pos, vel).GetUnitVector();
   Real inc = ACos((am[2]), GmatOrbitConstants::KEP_TOL);
   if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   {
      throw UtilityException
         ("Error in conversion to Equinoctial elements: "
          "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   }

   Integer j = 1;  // always 1, unless inclination is exactly 180 degrees

   // Define equinoctial coordinate system
   Rvector3 f;
   f[0]      =   1.0 - ((am[0] * am[0]) / (1.0 + Pow(am[2], j)));
   f[1]      = - (am[0] * am[1]) / (1.0 + Pow(am[2], j));
   f[2]      = - Pow(am[0], j);
   f         = f.GetUnitVector();

   Rvector3 g = Cross(am,f).GetUnitVector();

   h =   eVec * g;
   k =   eVec * f;
   p =   am[0] / (1.0 + Pow(am[2], j));
   q = - am[1] / (1.0 + Pow(am[2], j));

   // Calculate mean longitude
   // First, calculate true longitude
   Real X1      = pos * f;
   Real Y1      = pos * g;
   #ifdef DEBUG_STATE_CONVERSION_SQRT
      MessageInterface::ShowMessage("About to call Sqrt from CartesianToEquinoctial\n");
      MessageInterface::ShowMessage("h = %12.10f,  k = %12.10f\n", h, k);
   #endif
   Real tmpSqrt = Sqrt(1.0 - (h * h) - (k * k));
   Real beta    = 1.0 / (1.0 + tmpSqrt);
   Real cosF    = k + ((1.0 - k*k*beta) * X1 - (h * k * beta * Y1)) /
                  (sma * tmpSqrt);
   Real sinF    = h + ((1.0 - h * h * beta) * Y1 - (h * k * beta * X1)) /
                  (sma * tmpSqrt);
   Real F       = ATan2(sinF, cosF);
   // limit F to a positive value
   while (F < 0.0) F += TWO_PI;
   lambda       = (F + (h * cosF) - (k * sinF)) * DEG_PER_RAD;

   return Rvector6(sma, h, k, p, q, lambda);
}

//------------------------------------------------------------------------------
// Rvector6 EquinoctialToCartesian(const Rvector6& equinoctial, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from Equinoctial to Cartesian.
 *
 * @param <equinoctial>     Equinoctial state
 * @param <mu>              Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Equinoctial to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::EquinoctialToCartesian(const Rvector6& equinoctial, const Real& mu)
{
   Real sma    = equinoctial[0];   // semi major axis
   Real h      = equinoctial[1];   // projection of eccentricity vector onto y
   Real k      = equinoctial[2];   // projection of eccentricity vector onto x
   Real p      = equinoctial[3];   // projection of N onto y
   Real q      = equinoctial[4];   // projection of N onto x
   Real lambda = equinoctial[5]*RAD_PER_DEG;   // mean longitude

   // Check for eccentricity out-of-range
   Real e = Sqrt((h * h) + (k * k));
   Real oneMinusEps = 1.0 - GmatOrbitConstants::ECC_RANGE_TOL;
   if (e > oneMinusEps )
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Equinoctial to Cartesian elements: ";
      errmsg << "Values of EquinoctialH and EquinoctialK result in eccentricity of ";
      errmsg << e << " and eccentricity must be less than " << oneMinusEps << std::endl;
      throw UtilityException(errmsg.str());
   }
   
   // Use mean longitude to find true longitude
   Real prevF;
   Real fF;
   Real fPrimeF;
   Real F = lambda;      // first guess is mean longitude
   do {
      prevF   = F;
      fF      = F + h * Cos(F) - k * Sin(F) - lambda;
      fPrimeF = 1.0 - h * Sin(F) - k * Cos(F);
      F       = prevF - (fF/fPrimeF);
   } while (Abs(F-prevF) >= ORBIT_TOL);

   // Adjust true longitude to be between 0 and two-pi
   while (F < 0) F += TWO_PI;

   #ifdef DEBUG_STATE_CONVERSION_SQRT
      MessageInterface::ShowMessage("About to call Sqrt from EquinoctialToCartesian\n");
      MessageInterface::ShowMessage("h = %12.10f,  k = %12.10f\n", h, k);
   #endif
   Real tmpSqrt;
   try
   {
      tmpSqrt = Sqrt(1.0 - (h * h) - (k * k));
   }
   catch (UtilityException &ue)
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Equinoctial to Cartesian elements: ";
      errmsg << "Values of EquinoctialH and EquinoctialK result in eccentricity of ";
      errmsg << e << " and eccentricity must be less than " << oneMinusEps << std::endl;
      throw UtilityException(errmsg.str());
   }
   Real beta    = 1.0 / (1.0 + tmpSqrt);

   #ifdef DEBUG_STATE_CONVERSION_SQRT
      MessageInterface::ShowMessage("About to call Sqrt from EquinoctialToCartesian (2)\n");
      MessageInterface::ShowMessage("mu = %12.10f,  sma = %12.10f\n", mu, sma);
   #endif
   Real n    = Sqrt(mu/(sma * sma * sma));
   Real cosF = Cos(F);
   Real sinF = Sin(F);
   Real r    = sma * (1.0 - (k * cosF) - (h * sinF));

   if (r <= 0.0)
   {
      throw UtilityException("Error in conversion from Equinoctial to Cartesian elements: Cannot convert state because RMAG <= 0.\n");
   }

   // Calculate the cartesian components expressed in the equinoctial coordinate system

   Real X1    = sma * (((1.0 - (h * h * beta)) * cosF) + (h * k * beta * sinF) - k);
   Real Y1    = sma * (((1.0 - (k * k * beta)) * sinF) + (h * k * beta * cosF) - h);
   Real X1Dot = ((n * sma * sma) / r) * ((h * k * beta * cosF) -
                (1.0 - (h * h * beta)) * sinF);
   Real Y1Dot = ((n * sma * sma) / r) * ((1.0 - (k * k * beta)) * cosF -
                (h * k * beta * sinF));

   // assumption in conversion from equinoctial to cartesian
   Integer j = 1;  // always 1, unless inclination is exactly 180 degrees

   // Compute Q matrix
   Rmatrix33 Q(1.0 - (p * p) + (q * q),   2.0 * p * q * j,                2.0 * p,
               2.0 * p * q,               (1.0 + (p * p) - (q * q)) * j, -2.0 * q,
              -2.0 * p * j,               2.0 * q,                       (1.0 - (p * p) - (q * q)) * j);

   Rmatrix33 Q2 = (1.0 / (1.0 + (p * p) + (q * q))) * Q;
   Rvector3  f(Q2(0,0), Q2(1,0), Q2(2,0));
   Rvector3  g(Q2(0,1), Q2(1,1), Q2(2,1));
   f = f.GetUnitVector();
   g = g.GetUnitVector();

   Rvector3 pos = (X1 * f) + (Y1 * g);
   Rvector3 vel = (X1Dot * f) + (Y1Dot * g);

   return Rvector6(pos, vel);
}

//------------------------------------------------------------------------------
// Rvector6 CartesianToSphericalAZFPA(const Rvector6& cartesian)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to SphericalAZFPA.
 *
 * @param <cartesian>     Cartesian state
 *
 * @return Spacecraft orbit state converted from Cartesian to SphericalAZFPA
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToSphericalAZFPA(const Rvector6& cartesian)
{
   // Calculate the magnitude of the position vector, right ascension, and declination
   Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
   Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
   Real    rMag   = pos.GetMagnitude();

   if (rMag < 1e-10)
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Cartesian to SphericalAZFPA: ";
      errmsg << "Spherical elements are undefined because RMAG (" << rMag;
      errmsg << ") is less than 1e-10." << std::endl;
      throw UtilityException(errmsg.str());
   }
   
   Real    lambda = ATan2(pos[1], pos[0]);
   Real    delta  = ASin(pos[2] / rMag);

   // Calculate magnitude of the velocity vector
   Real   vMag    = vel.GetMagnitude();

   if (vMag < 1e-10)
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Cartesian to SphericalAZFPA: ";
      errmsg << "Spherical elements are undefined because VMAG (" << vMag;
      errmsg << ") is less than 1e-10." << std::endl;
      throw UtilityException(errmsg.str());
   }
   
   // Calculate the vertical flight path angle. rMag = 0 or vMag = 0 is trapped above. 
   Real   psi     = ACos((pos * vel) / (rMag * vMag));

   //Calculate the azimuth angle
   // First, calculate basis (column) vectors of Fl expressed in Fi
   Rvector3 x(Cos(delta) * Cos(lambda),  Cos(delta) * Sin(lambda), Sin(delta));
   Rvector3 y(Cos(lambda + PI_OVER_TWO), Sin(lambda + PI_OVER_TWO), 0.0);
   Rvector3 z(-Sin(delta) * Cos(lambda), -Sin(delta) * Sin(lambda), Cos(delta));
   // Create the transformation matrix from Fi (the frame in which the cartesian state is expressed)
   // to Fl (local frame, where z is a unit vector that points north); Rli is the tranpose of the
   // matrix created by the three column vectors    Rli = [x y z]T
   Rmatrix33 Rli( x[0], x[1], x[2],
                  y[0], y[1], y[2],
                  z[0], z[1], z[2] );

   // Compute the velocity in the local frame
   Rvector3  vLocal = Rli * vel;

   //Compute the flight path azimuth angle
   Real alphaF     = ATan2(vLocal[1], vLocal[2]);

   return Rvector6(rMag, lambda * DEG_PER_RAD, delta  * DEG_PER_RAD,
         vMag, alphaF * DEG_PER_RAD, psi    * DEG_PER_RAD);  // existing code expects this order
}

//------------------------------------------------------------------------------
// Rvector6 SphericalAZFPAToCartesian(const Rvector6& spherical)
//------------------------------------------------------------------------------
/**
 * Converts from SphericalAZFPA to Cartesian.
 *
 * @param <spherical>     SphericalAZFPA state
 *
 * @return Spacecraft orbit state converted from SphericalAZFPA to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::SphericalAZFPAToCartesian(const Rvector6& spherical)
{
   #ifdef DEBUG_STATE_CONVERSION
      MessageInterface::ShowMessage("Entering SphericalAZFPAToCartesian: spherical = %s\n",
            spherical.ToString().c_str());
   #endif
   Real     rMag    = spherical[0]; // magnitude of the position vector
   Real     lambda  = spherical[1] * RAD_PER_DEG; // right ascension
   Real     delta   = spherical[2] * RAD_PER_DEG; // declination
   Real     vMag    = spherical[3]; // magnitude of the velocity vector
   Real     alphaF  = spherical[4] * RAD_PER_DEG; // flight path azimuth   *** existing code expects this order
   Real     psi     = spherical[5] * RAD_PER_DEG; // vertical flight path angle

   // Compute the position
   Rvector3 pos(rMag * Cos(delta) * Cos(lambda),
                rMag * Cos(delta) * Sin(lambda),
                rMag * Sin(delta) );

   Real     sinDelta  = Sin(delta);
   Real     cosDelta  = Cos(delta);
   Real     sinLambda = Sin(lambda);
   Real     cosLambda = Cos(lambda);
   Real     sinPsi    = Sin(psi);
   Real     cosPsi    = Cos(psi);
   Real     sinAlphaF = Sin(alphaF);
   Real     cosAlphaF = Cos(alphaF);

   Real vx, vy, vz;

   vx = vMag * ( (cosPsi * cosDelta * cosLambda) -
        sinPsi * ((sinAlphaF * sinLambda) + (cosAlphaF * sinDelta * cosLambda)) );
   vy = vMag * ( (cosPsi * cosDelta * sinLambda) +
        sinPsi * ((sinAlphaF * cosLambda) - (cosAlphaF * sinDelta * sinLambda)) );
   vz = vMag * ( (cosPsi * sinDelta) + (sinPsi * cosAlphaF * cosDelta) );
   Rvector3 vel(vx, vy, vz);

   return Rvector6(pos, vel);
}

//------------------------------------------------------------------------------
// Rvector6 CartesianToSphericalRADEC(const Rvector6& cartesian)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to SphericalRADEC.
 *
 * @param <cartesian>     Cartesian state
 *
 * @return Spacecraft orbit state converted from Cartesian to SphericalRADEC
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToSphericalRADEC(const Rvector6& cartesian)
{
   // Calculate the magnitude of the position vector, right ascension, and declination
   Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
   Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
   Real    rMag   = pos.GetMagnitude();

   if (rMag < 1e-10)
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Cartesian to SphericalRADEC: ";
      errmsg << "Spherical elements are undefined because RMAG (" << rMag;
      errmsg << ") is less than 1e-10." << std::endl;
      throw UtilityException(errmsg.str());
   }
   
   Real    lambda = ATan2(pos[1], pos[0]);
   Real    delta  = ASin(pos[2] / rMag);

   // Calculate magnitude of the velocity vector
   Real   vMag    = vel.GetMagnitude();

   if (vMag < 1e-10)
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Cartesian to SphericalRADEC: ";
      errmsg << "Spherical elements are undefined because VMAG (" << vMag;
      errmsg << ") is less than 1e-10." << std::endl;
      throw UtilityException(errmsg.str());
   }
   
   // Compute right ascension of velocity
   Real   lambdaV = ATan2(vel[1], vel[0]);

   // Compute the declination of velocity
   Real   deltaV  = ASin(vel[2] / vMag);

   return Rvector6(rMag, lambda  * DEG_PER_RAD, delta  * DEG_PER_RAD,
                   vMag, lambdaV * DEG_PER_RAD, deltaV * DEG_PER_RAD);
}

//------------------------------------------------------------------------------
// Rvector6 SphericalRADECToCartesian(const Rvector6& spherical)
//------------------------------------------------------------------------------
/**
 * Converts from SphericalRADEC to Cartesian.
 *
 * @param <spherical>     SphericalRADEC state
 *
 * @return Spacecraft orbit state converted from SphericalRADEC to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::SphericalRADECToCartesian(const Rvector6& spherical)
{
   Real     rMag    = spherical[0]; // magnitude of the position vector
   Real     lambda  = spherical[1] * RAD_PER_DEG; // right ascension
   Real     delta   = spherical[2] * RAD_PER_DEG; // declination
   Real     vMag    = spherical[3]; // magnitude of the velocity vector
   Real     lambdaV = spherical[4] * RAD_PER_DEG; // right ascension of velocity
   Real     deltaV  = spherical[5] * RAD_PER_DEG; // declination of velocity

   #ifdef DEBUG_STATE_CONVERSION
      MessageInterface::ShowMessage(
            "Entering SphericalRADECToCartesian with state %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
            spherical[0],spherical[1],spherical[2],spherical[3],spherical[4],spherical[5]);
   #endif
   // Compute the position
   Rvector3 pos(rMag * Cos(delta) * Cos(lambda),
                rMag * Cos(delta) * Sin(lambda),
                rMag * Sin(delta) );

   // Compute the velocity
   Real     vx = vMag   * Cos(lambdaV) * Cos(deltaV);
   Real     vy = vx     * Tan(lambdaV);
   Real     vz = vMag   * Sin(deltaV);
   Rvector3 vel(vx, vy, vz);

   #ifdef DEBUG_STATE_CONVERSION
      MessageInterface::ShowMessage(
            "Exiting SphericalRADECToCartesian and returning state %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
            pos[0],pos[1],pos[2],vel[0],vel[1],vel[2]);
   #endif
   return Rvector6(pos, vel);
}

//------------------------------------------------------------------------------
//  Rvector6 KeplerianToModKeplerian(const Rvector6& keplerian)
//------------------------------------------------------------------------------
/**
 * Converts from Keplerian to Modified Keplerian.
 *
 * @param <keplerian>     Keplerian state
 *
 * @return Spacecraft orbit state converted from Keplerian to Modified Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::KeplerianToModKeplerian(const Rvector6& keplerian)
{
   #ifdef DEBUG_MOD_KEPLERIAN
   MessageInterface::ShowMessage
      ("KeplerianToModKeplerian() keplerian =\n   %s\n", keplerian.ToString().c_str());
   #endif

   Real a = keplerian[0];    // Semi-major axis
   Real e = keplerian[1];    // eccentricity

   // Check for  exactly parabolic orbit or infinite semi-major axis
   // then send the error message
   if ( a == 1 || GmatMathUtil::IsInf(a))
      throw UtilityException("StateConversionUtil::KeplerianToModKeplerian: "
                             "Parabolic orbits cannot be entered in Keplerian "
                             "or Modified Keplerian format");
   
   // Check for invalid eccentricity then send the error message
   if (e < 0.0)
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Eccentricity (" << e << ") cannot be less than 0.0.";
      errmsg << " The sign of the eccentricity has been changed.\n";
      MessageInterface::ShowMessage(errmsg.str());
      
      e *= -1.0;
   }
   
   // Check for inconsistent semi-major axis and  eccentricity
   // then send the error message
   if ((a > 0.0) && (e > 1.0))
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Semimajor axis (" << a << ") cannot be positive if"; 
      errmsg << " eccentricity (" << a << ") is greater than 1.0.";
      errmsg << " The sign of the semimajor axis has been changed.";
      errmsg << " If changing orbit from hyperbolic to elliptic, set eccentricity first.\n" << std::endl;
      MessageInterface::ShowMessage(errmsg.str());
      
      a *= -1.0;
   }
   if ((a < 0.0) && (e < 1.0))
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Semimajor axis (" << a << ") cannot be negative if ";
      errmsg << " eccentricity (" << e << ") is less than 1.0.";
      errmsg << " The sign of the semimajor axis has been changed.";
      errmsg << " If changing orbit from elliptic to hyperbolic, set eccentricity first.\n" << std::endl;
      MessageInterface::ShowMessage(errmsg.str());
      
      a *= -1.0;
   }
   
   // Test that radius of periapsis is not too small.
   Real absA1E = Abs(a * (1.0 - e));
   if (absA1E < SINGULAR_TOL)
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "A nearly singular conic section was encountered while converting from"; 
      errmsg << "  the Keplerian elements to the Cartesian state. The radius of periapsis("; 
      errmsg << absA1E << ") must be greater than 1 meter." << std::endl;
      throw UtilityException(errmsg.str());
   }
   //  Verify that orbit is not too close to a parabola which results in undefined SMA
   Real oneMinusE = Abs(1.0 - e);
   if (oneMinusE < PARABOLIC_TOL)
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "A nearly parabolic orbit";
      errmsg << " (ECC = " << e << ") was encountered";
      errmsg << " while converting from the Keplerian elements to"; 
      errmsg << " the Cartesian state.";
      errmsg << " The Keplerian elements are";
      errmsg << " undefined for a parabolic orbit." << std::endl;
      throw UtilityException(errmsg.str());
   }
   
   
   // Check for parabolic orbit to machine precision
   // then send the error message
   if ( GmatMathUtil::Abs(e - 1) < 2*GmatRealConstants::REAL_EPSILON)
   {
      std::string errmsg =
            "Error in conversion from Keplerian to ModKeplerian state: ";
      errmsg += "The state results in an orbit that is nearly parabolic.\n";
      throw UtilityException(errmsg);
   }
   // Check for a singular conic section
   if (GmatMathUtil::Abs(a*(1 - e) < .001))
   {
      throw UtilityException
         ("StateConversionUtil: Error in conversion from Keplerian to ModKeplerian state: "
          "The state results in a singular conic section with radius of periapsis less than 1 m.\n");
   }

   // Convert into radius of periapsis and apoapsis
   Real radPer = a*(1.0 - e);
   Real radApo = a*(1.0 + e);


   #ifdef DEBUG_MOD_KEPLERIAN
   Rvector6 modkepl = Rvector6(radPer, radApo, keplerian[2], keplerian[3],
                               keplerian[4], keplerian[5]);
   MessageInterface::ShowMessage
      ("KeplerianToModKeplerian() returning\n   %s\n", modkepl.ToString().c_str());
   #endif

   // return new Modified Keplerian
   return Rvector6(radPer, radApo, keplerian[2], keplerian[3],
                   keplerian[4], keplerian[5]);
}


//------------------------------------------------------------------------------
//  Rvector6 ModKeplerianToKeplerian(const Rvector6& modKeplerian)
//------------------------------------------------------------------------------
/**
 * Converts from Modified Keplerian to Keplerian.
 *
 * @param <modKeplerian>     Modified Keplerian state
 *
 * @return Spacecraft orbit state converted from Modified Keplerian to Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::ModKeplerianToKeplerian(const Rvector6& modKeplerian)
{
   #ifdef DEBUG_MODKEP_TO_KEP
      MessageInterface::ShowMessage("Entering ModKepToKep, radPer = %12.10f, radApo = %12.10f\n",
            modKeplerian[0], modKeplerian[1]);
   #endif
   Real radPer = modKeplerian[0];     // Radius of Periapsis
   Real radApo = modKeplerian[1];     // Radius of Apoapsis

   // Check validity
   if (IsEqual(radApo, 0.0, .001))
      throw UtilityException("StateConversionUtil::ModKeplerianToKeplerian: "
                             "Radius of Apoapsis must not be zero");

   if (radApo < radPer && radApo > 0.0)
      throw UtilityException("StateConversionUtil::ModKeplerianToKeplerian: If RadApo < RadPer then RadApo must be negative.  "
                             "If setting Modified Keplerian State, set RadApo before RadPer to avoid this issue.");

   if (radPer <= 0.0)
      throw UtilityException("StateConversionUtil::ModKeplerianToKeplerian: "
                             "Radius of Periapsis must be greater than zero");

   if (IsEqual(radPer, 0.0, .001))
      throw UtilityException("StateConversionUtil::ModKeplerianToKeplerian: "
                             "Parabolic orbits are not currently supported."
                             "RadPer must be greater than zero");

   // Compute the division between them
   Real rpbyra = radPer/radApo;

   // compute the eccentricity and semi-major axis
   Real e = (1.0 - rpbyra)/(1.0 + rpbyra);
   Real a = radPer/(1.0 - e);

   // Return the classic Keplerian
   return Rvector6(a, e, modKeplerian[2], modKeplerian[3],
                   modKeplerian[4], modKeplerian[5]);
}

//------------------------------------------------------------------------------
// Real TrueToMeanAnomaly(Real taRadians, Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Computes mean anomaly from true anomaly
 *
 * @param <taRadians>     True anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <modBy2Pi>      Flag specifying whether or not to mod the result by two pi
 *
 * @return Mean anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::TrueToMeanAnomaly(Real taRadians, Real ecc, bool modBy2Pi)
{
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("TrueToMeanAnomaly() ta=%f, ecc=%f\n", taRadians, ecc);
   #endif

   Real ma = 0.0;

   if (ecc < (1.0 - GmatOrbitConstants::KEP_TOL))
   {
      Real ea = TrueToEccentricAnomaly(taRadians, ecc);
      ma = ea - ecc * Sin(ea);
      // Only mod it to be between 0 and twopi when the orbit is elliptical
      if (ma < 0.0)
         ma = ma + TWO_PI;
      if (modBy2Pi)
      {
         while (ma > TWO_PI)
            ma -= TWO_PI;
      }
   }
   else if (ecc > (1.0 + GmatOrbitConstants::KEP_TOL))
   {
      Real ha = TrueToHyperbolicAnomaly(taRadians, ecc);
      ma = ecc * Sinh(ha) - ha;
   }
   else
   {
      std::string warn =
         "Warning: Orbit is near parabolic in mean anomaly calculation.  ";
      warn += "Setting MA = 0\n";
      MessageInterface::PopupMessage(Gmat::WARNING_, warn);
      ma = 0.0;
   }

   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage("TrueToMeanAnomaly() returning %f\n", ma);
   #endif

   return ma;
}


//------------------------------------------------------------------------------
// Real TrueToEccentricAnomaly(Real taRadians, Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Computes eccentric anomaly from true anomaly
 *
 * @param <taRadians>     True anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <modBy2Pi>      Flag specifying whether or not to mod the result by two pi
 *
 * @return Eccentric anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::TrueToEccentricAnomaly(Real taRadians, Real ecc, bool modBy2Pi)
{
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("TrueToEccentricAnomaly() ta=%f, ecc=%f\n", taRadians, ecc);
   #endif

   Real ea = 0.0;

   if (ecc <= (1.0 - GmatOrbitConstants::KEP_ANOMALY_TOL))
   {
      Real cosTa = Cos(taRadians);
      Real eccCosTa = ecc * cosTa;
      if (eccCosTa == -1.0)
      {
         throw UtilityException("StateConversionUtil::TrueToEccentricAnomaly - error converting - divide by zero.\n");
      }
      Real sinEa = (Sqrt(1.0 - ecc*ecc) * Sin(taRadians)) / (1.0 + eccCosTa);
      Real cosEa = (ecc + cosTa) / (1.0 + eccCosTa);
      ea = ATan2(sinEa, cosEa);
   }

   if (ea < 0.0)
      ea = ea + TWO_PI;
   if (modBy2Pi)
   {
      while (ea > TWO_PI)
         ea -= TWO_PI;
   }

   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage("TrueToEccentricAnomaly() returning %f\n", ea);
   #endif

   return ea;
}


//------------------------------------------------------------------------------
// Real TrueToHyperbolicAnomaly(Real taRadians, Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/**
 * Converts true anomaly to hyperbolic anomaly.
 *
 * @param  taRadians  True anomaly in radians
 * @param  ecc        Eccentricity
 * @param  modBy2Pi   Flag specifying whether or not to mod the result by two pi
 *
 * @return  Hyperbolic anomaly in radians, 0.0 if eccentricity is less than 1.0 + KEP_TOL
 *
 * @exception  UtilityException is throw if "eccentricity * cos(trueAnomaly)" is -1.
 */
//------------------------------------------------------------------------------
Real StateConversionUtil::TrueToHyperbolicAnomaly(Real taRadians, Real ecc, bool modBy2Pi)
{
   #ifdef DEBUG_ANOMALY
      MessageInterface::ShowMessage("TrueToHyperbolicAnomaly() ta=%f, ecc=%f\n", taRadians, ecc);
   #endif

   Real ha = 0.0;

   if (ecc >= (1.0 + GmatOrbitConstants::KEP_TOL))
   {
      Real cosTa = Cos(taRadians);
      Real eccCosTa = ecc * cosTa;
      if (eccCosTa == -1.0)
      {
         throw UtilityException("StateConversionUtil::TrueToEccentricAnomaly - error converting - divide by zero.\n");
      }
      Real sinhHa = (Sin(taRadians) * Sqrt(ecc*ecc - 1.0)) / (1.0 + eccCosTa);
      ha = ASinh(sinhHa);
   }
#ifdef DEBUG_ANOMALY
   else
   {
      MessageInterface::ShowMessage("TrueToHyperbolicAnomaly() ecc < (1.0 + KEP_TOL), returning ha = 0.0\n");
   }
#endif

   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage("TrueToHyperbolicAnomaly() returning %f\n", ha);
   #endif
   if (modBy2Pi)
   {
      while (ha > TWO_PI)
         ha -= TWO_PI;
      while (ha < 0.0)
         ha += TWO_PI;
   }

   return ha;
}


//------------------------------------------------------------------------------
// Real MeanToTrueAnomaly(Real maRadians, Real ecc, Real tol = 1.0e-08)
//------------------------------------------------------------------------------
/*
 * Computes true anomaly from mean anomaly
 *
 * @param <maRadians>     Mean anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <tol>           Tolerance value
 *
 * @return True anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::MeanToTrueAnomaly(Real maRadians, Real ecc, Real tol)
{
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("MeanToTrueAnomaly() maRadians=%f, ecc=%f\n", maRadians, ecc);
   #endif

   Real ta;
   Integer iter;
   Integer ret;

   ret = ComputeMeanToTrueAnomaly(maRadians, ecc, tol, &ta, &iter);

   if (ret == 0)
   {
      #ifdef DEBUG_ANOMALY
      MessageInterface::ShowMessage("MeanToTrueAnomaly() returning %f\n", ta);
      #endif

      return ta;
   }

   throw UtilityException("MeanToTrueAnomaly() Error converting "
                          " Mean Anomaly to True Anomaly\n");
}

//------------------------------------------------------------------------------
// Real EccentricToTrueAnomaly(Real eaRadians, Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Computes true anomaly from eccentric anomaly
 *
 * @param <eaRadians>     Eccentric anomaly in radians
 * @param <ecc>           Eccentricity
 * @param  <modBy2Pi>     Flag specifying whether or not to mod the result by two pi
 *
 * @return True anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::EccentricToTrueAnomaly(Real eaRadians, Real ecc, bool modBy2Pi)
{
   Real e2            = ecc * ecc;
   Real cosE          = Cos(eaRadians);
   Real oneMinusECosE = 1.0 - ( ecc * cosE);
   if (oneMinusECosE == 0.0)
   {
      throw UtilityException("StateConversionUtil::EccentricToTrueAnomaly - error converting - divide by zero.\n");
   }

   Real sinTA = (Sqrt(1.0 - e2) * Sin(eaRadians)) / oneMinusECosE;
   Real cosTA = (cosE - ecc) / oneMinusECosE;
   Real ta    =  ATan2(sinTA, cosTA);
   if (modBy2Pi)
   {
      while (ta)
         ta -= TWO_PI;
   }
   return ta;
}

//------------------------------------------------------------------------------
// Real HyperbolicToTrueAnomaly(Real haRadians, Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Computes true anomaly from hyperbolic anomaly
 *
 * @param <eaRadians>     Hyperbolic anomaly in radians
 * @param <ecc>           Eccentricity
 * @param  <modBy2Pi>     Flag specifying whether or not to mod the result by two pi
 *
 * @return True anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::HyperbolicToTrueAnomaly(Real haRadians, Real ecc, bool modBy2Pi)
{
   Real e2             = ecc * ecc;
   Real coshH          = Cosh(haRadians);
   Real oneMinusECoshH = 1.0 - (ecc * coshH);
   if (oneMinusECoshH == 0.0)
   {
      throw UtilityException("StateConversionUtil::HyperbolicToTrueAnomaly - error converting - divide by zero.\n");
   }

   Real sinTA = (Sqrt(e2 - 1.0) * Sinh(haRadians)) / oneMinusECoshH;
   Real cosTA = (coshH - ecc) / oneMinusECoshH;
   Real ta    = ATan2(sinTA, cosTA);

   if (modBy2Pi)
   {
      while (ta)
         ta -= TWO_PI;
   }
   return ta;
}

//------------------------------------------------------------------------------
// Real ConvertFromTrueAnomaly(const std::string toType, Real taRadians,
//                                    Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Converts from true anomaly to specified anomaly type
 *
 * @param <toType>        Anomaly type to convert to
 * @param <taRadians>     True anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <modBy2Pi>      Flag specifying whether or not to mod the result by two pi
 *
 * @return  anomaly of specified type
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::ConvertFromTrueAnomaly(const std::string toType, Real taRadians, Real ecc, bool modBy2Pi)
{
   AnomalyType anomType = GetAnomalyType(toType);
   return ConvertFromTrueAnomaly(anomType, taRadians, ecc, modBy2Pi);
}

//------------------------------------------------------------------------------
// Real ConvertFromTrueAnomaly(AnomalyType toType, Real taRadians,
//                                    Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Converts from true anomaly to specified anomaly type
 *
 * @param <toType>        Anomaly type to convert to
 * @param <taRadians>     True anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <modBy2Pi>      Flag specifying whether or not to mod the result by two pi
 *
 * @return  anomaly of specified type
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::ConvertFromTrueAnomaly(AnomalyType toType,
                          Real taRadians, Real ecc, bool modBy2Pi)
{
   switch (toType)
   {
   case TA:
      return taRadians;
   case MA:
      return TrueToMeanAnomaly(taRadians, ecc, modBy2Pi);
   case EA:
      return TrueToEccentricAnomaly(taRadians, ecc, modBy2Pi);
   case HA:
      return TrueToHyperbolicAnomaly(taRadians, ecc, modBy2Pi);
   default:
      throw UtilityException("ERROR converting to true anomaly - unknown anomaly type.\n");
   }
}

//------------------------------------------------------------------------------
// Real ConvertToTrueAnomaly(const std::string fromType, Real taRadians,
//                                  Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Converts to true anomaly from input anomaly type
 *
 * @param <fromType>      Anomaly type to convert from
 * @param <taRadians>     Anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <modBy2Pi>      Flag specifying whether or not to mod the result by two pi
 *
 * @return  true anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::ConvertToTrueAnomaly(const std::string fromType, Real taRadians, Real ecc, bool modBy2Pi)
{
   AnomalyType anomType = GetAnomalyType(fromType);
   return ConvertToTrueAnomaly(anomType, taRadians, ecc, modBy2Pi);
}

//------------------------------------------------------------------------------
// Real ConvertToTrueAnomaly(AnomalyType fromType, Real taRadians,
//                                  Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Converts to true anomaly from input anomaly type
 *
 * @param <fromType>      Anomaly type to convert from
 * @param <taRadians>     Anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <modBy2Pi>      Flag specifying whether or not to mod the result by two pi
 *
 * @return  true anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::ConvertToTrueAnomaly(AnomalyType fromType, Real taRadians, Real ecc, bool modBy2Pi)
{
   switch (fromType)
   {
   case TA:
      return taRadians;
   case MA:
      return MeanToTrueAnomaly(taRadians, ecc);
   case EA:
      return EccentricToTrueAnomaly(taRadians, ecc, modBy2Pi);
   case HA:
      return HyperbolicToTrueAnomaly(taRadians, ecc, modBy2Pi);
   default:
      throw UtilityException("ERROR converting to true anomaly - unknown anomaly type.\n");
   }
}


//------------------------------------------------------------------------------
// Real CartesianToTA(Real mu, const Rvector3 &pos,
//                           const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
/*
 * Computes true anomaly from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  true anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToTA(Real mu, const Rvector3 &pos,
                                        const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_TA
   MessageInterface::ShowMessage
      ("CartesianToTA() mu=%f, inRadian=%d\n   pos =%s\n   vel =%s\n", mu, inRadian,
       pos.ToString().c_str(), vel.ToString().c_str());
   #endif

   Rvector3 eVec = CartesianToEccVector(mu, pos, vel);
   Real inc = CartesianToINC(mu, pos, vel, true);
   if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   {
      throw UtilityException
         ("Error in conversion to Keplerian state: "
          "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   }
   Real ecc = eVec.GetMagnitude();
   Real rMag = pos.GetMagnitude();
   Real ta = 0.0;

   if (rMag == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to TA - position vector is a zero vector\n");
   }

   #ifdef DEBUG_KEPLERIAN_TA
   MessageInterface::ShowMessage
      ("   eVec=%s,\n   inc=%f, ecc=%f, rMag=%f, ta=%f\n",
       eVec.ToString().c_str(), inc, ecc, rMag, ta);
   #endif

   // Case 1:  Non-circular, Inclined Orbit
   if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   {
      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   Case 1:  Non-circular, Inclined Orbit\n");
      #endif

      Real temp = (eVec*pos) / (ecc*rMag);
      ta = ACos(temp, GmatOrbitConstants::KEP_TOL);

      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   ACos(%+.16f) = %+.16f\n", temp, ACos(temp, GmatOrbitConstants::KEP_TOL));
      MessageInterface::ShowMessage("   ACos(%+.16f) = %+.16f\n", temp, ta);
      #endif

      // Fix quadrant
      if ((pos*vel) < 0.0)
      {
         ta = TWO_PI - ta;
         #ifdef DEBUG_KEPLERIAN_TA
         MessageInterface::ShowMessage("   after quadrant fix, ta=%f\n", ta);
         #endif
      }
   }
   // Case 2: Non-circular, Equatorial Orbit
   else if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   {
      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   Case 2: Non-circular, Equatorial Orbit\n");
      #endif

      //ta = ACos((eVec*pos) / (ecc*rMag));
      ta = ACos(((eVec*pos) / (ecc*rMag)), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if ((pos*vel) < 0.0)
         ta = TWO_PI - ta;
   }
   // Case 3: Circular, Inclined Orbit
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   {
      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   Case 3: Circular, Inclined Orbit\n");
      #endif

      Rvector3 nVec =  CartesianToDirOfLineOfNode(pos, vel);
      Real nMag = nVec.GetMagnitude();
      if (nMag == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to TA - n vector is a zero vector.\n");
      }
      //ta = ACos((nVec*pos) / (nMag*rMag));
      ta = ACos(((nVec*pos) / (nMag*rMag)), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if ((pos[2]) < 0.0)
         ta = TWO_PI - ta;
   }
   // Case 4: Circular, Equatorial Orbit
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   {
      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   Case 4: Circular, Equatorial Orbit\n");
      #endif

      //ta = ACos(pos[0] / rMag);
      ta = ACos((pos[0] / rMag), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if ((pos[1]) < 0.0)
         ta = TWO_PI - ta;
   }

   // Convert 2pi to 0
   ta = Mod(ta, TWO_PI);

   #ifdef DEBUG_KEPLERIAN_TA
   MessageInterface::ShowMessage("CartesianToTA() returning %f\n", ta);
   #endif

   if (inRadian)
      return ta;
   else
      return ta * DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Real CartesianToMA(Real mu, const Rvector3 &pos,
//                           const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
/*
 * Computes mean anomaly from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  mean anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToMA(Real mu, const Rvector3 &pos,
                                        const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_ANOMALY_MA
   MessageInterface::ShowMessage("CartesianToMA() ");
   #endif

   Real ta  = CartesianToTA(mu, pos, vel, true);
   Real ecc = CartesianToECC(mu, pos, vel);
   Real ma  = TrueToMeanAnomaly(ta, ecc);

   #ifdef DEBUG_ANOMALY_MA
   MessageInterface::ShowMessage("returning %f\n", ma);
   #endif

   if (inRadian)
      return ma;
   else
      return ma * DEG_PER_RAD;
}



//------------------------------------------------------------------------------
// Real CartesianToEA(Real mu, const Rvector3 &pos,
//                           const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
/*
 * Computes eccentric anomaly from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  eccentric anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToEA(Real mu, const Rvector3 &pos,
                                        const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_EA
   MessageInterface::ShowMessage("CartesianToEA() ");
   #endif

   Real ta = CartesianToTA(mu, pos, vel, true);
   Real ecc = CartesianToECC(mu, pos, vel);
   Real ea = TrueToEccentricAnomaly(ta, ecc);

   #ifdef DEBUG_KEPLERIAN_EA
   MessageInterface::ShowMessage("returning %f\n", ea);
   #endif

   if (inRadian)
      return ea;
   else
      return ea * DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Real CartesianToHA(Real mu, const Rvector3 &pos,
//                           const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
/*
 * Computes hyperbolic anomaly from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  hyperbolic anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToHA(Real mu, const Rvector3 &pos,
                                        const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_HA
   MessageInterface::ShowMessage("CartesianToHA() ");
   #endif

   Real ta = CartesianToTA(mu, pos, vel, true);
   Real ecc = CartesianToECC(mu, pos, vel);
   Real ha = TrueToHyperbolicAnomaly(ta, ecc);

   #ifdef DEBUG_KEPLERIAN_HA
   MessageInterface::ShowMessage("returning %f\n", ha);
   #endif

   if (inRadian)
      return ha;
   else
      return ha * DEG_PER_RAD;

}



//------------------------------------------------------------------------------
// Real CartesianToSMA(Real mu, const Rvector3 &pos,
//                     const Rvector3 &vel)
//------------------------------------------------------------------------------
/*
 * Computes semimajor axis from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 *
 * @return  SMA
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToSMA(Real mu, const Rvector3 &pos,
                                         const Rvector3 &vel)
{
   #ifdef DEBUG_KEPLERIAN_SMA
   MessageInterface::ShowMessage("CartesianToSMA() ");
   #endif

   Real rMag = pos.GetMagnitude(); // ||r||
   Real vMag = vel.GetMagnitude(); // ||v||

   if (rMag == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to SMA - position vector is a zero vector.\n");
   }

   Real zeta = 0.5*(vMag*vMag) - mu/rMag;

   if (zeta == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to SMA - computed zeta is zero.\n");
   }

   // check if the orbit is near parabolic
   Real ecc = CartesianToECC(mu, pos, vel);
   if ((Abs(1.0 - ecc)) <= GmatOrbitConstants::KEP_ECC_TOL)
   {

      throw UtilityException
         ("Error in conversion from Cartesian to Keplerian state: "
          "The state results in an orbit that is nearly parabolic.\n");
   }

   Real sma = -mu/(2*zeta);

   // Check for a singular conic section
   if (Abs(sma*(1 - ecc) < .001))
   {
      throw UtilityException
         ("Error in conversion from Cartesian to Keplerian state: "
          "The state results in a singular conic section with radius of periapsis less than 1 m.\n");
   }


   #ifdef DEBUG_KEPLERIAN_SMA
   MessageInterface::ShowMessage("returning %f\n", sma);
   #endif

   return sma;
}

//------------------------------------------------------------------------------
// Real CartesianToECC(Real mu, const Rvector3 &pos,
//                            const Rvector3 &vel)
//------------------------------------------------------------------------------
/*
 * Computes eccentricity from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 *
 * @return  Eccentricity
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToECC(Real mu, const Rvector3 &pos,
                                         const Rvector3 &vel)
{
   #ifdef DEBUG_KEPLERIAN_ECC
   MessageInterface::ShowMessage("CartesianToECC() ");
   #endif

   Rvector3 eVec = CartesianToEccVector(mu, pos, vel);
   Real eMag = eVec.GetMagnitude(); // ||e||

   #ifdef DEBUG_KEPLERIAN_ECC
   MessageInterface::ShowMessage("returning %f\n", eMag);
   #endif

   return eMag;
}


//------------------------------------------------------------------------------
// Real CartesianToINC(Real mu, const Rvector3 &pos,
//                            const Rvector3 &vel)
//------------------------------------------------------------------------------
/*
 * Computes inclination from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  inclination
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToINC(Real mu, const Rvector3 &pos,
                                         const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_INC
   MessageInterface::ShowMessage("CartesianToINC() ");
   #endif

   // check if the orbit is near parabolic
   Real eMag = CartesianToECC(mu, pos, vel);
   if ((Abs(1.0 - eMag)) <= GmatOrbitConstants::KEP_ZERO_TOL)
   {

      throw UtilityException
      ("Error in conversion from Cartesian to Keplerian state: "
       "The state results in an orbit that is nearly parabolic.\n");
   }

   Rvector3 hVec = Cross(pos, vel);
   Real hMag = hVec.GetMagnitude();

   if (hMag == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to INC - angular momentum is a zero vector.\n");
   }

   Real inc = ACos((hVec[2] / hMag), GmatOrbitConstants::KEP_TOL);
   if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   {
      throw UtilityException
         ("Error in conversion to Keplerian state: "
          "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   }

   #ifdef DEBUG_KEPLERIAN_INC
   MessageInterface::ShowMessage("returning %f\n", inc);
   #endif

   if (inRadian)
      return inc;
   else
      return inc * DEG_PER_RAD;
}

//------------------------------------------------------------------------------
// Real CartesianToRAAN(Real mu, const Rvector3 &pos,
//                             const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
/*
 * Computes right ascension of the ascending node from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  RAAN
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToRAAN(Real mu, const Rvector3 &pos,
                                          const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_RAAN
   MessageInterface::ShowMessage("CartesianToRAAN() ");
   #endif

   Real ecc = CartesianToECC(mu, pos, vel);
   Real inc = CartesianToINC(mu, pos, vel, true);
   if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   {
      throw UtilityException
         ("Error in conversion to Keplerian state: "
          "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   }
   Real raan = 0.0;

   // Case 1:  Non-circular, Inclined Orbit
   if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   {
      Rvector3 nVec =  CartesianToDirOfLineOfNode(pos, vel);
      Real nMag = nVec.GetMagnitude();
      if (nMag == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to RAAN - n vector is a zero vector.\n");
      }
      raan = ACos((nVec[0] / nMag), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if (nVec[1] < 0.0)
         raan = TWO_PI - raan;
   }
   // Case 2: Non-circular, Equatorial Orbit
   else if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   {
      raan = 0.0;
   }
   // Case 3: Circular, Inclined Orbit
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   {
      Rvector3 nVec =  CartesianToDirOfLineOfNode(pos, vel);
      Real nMag = nVec.GetMagnitude();
      if (nMag == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to RAAN - n vector is a zero vector.\n");
      }
      raan = ACos((nVec[0] / nMag), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if (nVec[1] < 0.0)
         raan = TWO_PI - raan;
   }
   // Case 4: Circular, Equatorial Orbit
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   {
      raan = 0.0;
   }

   #ifdef DEBUG_KEPLERIAN_RAAN
   MessageInterface::ShowMessage("returning %f\n", raan);
   #endif

   // Convert 2pi to 0
   raan = Mod(raan, TWO_PI);

   if (inRadian)
      return raan;
   else
      return raan * DEG_PER_RAD;

}


//------------------------------------------------------------------------------
// Real CartesianToAOP(Real mu, const Rvector3 &pos,
//                            const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
/*
 * Computes argument of periapsis from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  AOP
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToAOP(Real mu, const Rvector3 &pos,
                                         const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_AOP
   MessageInterface::ShowMessage("CartesianToAOP() ");
   #endif

   Rvector3 eVec = CartesianToEccVector(mu, pos, vel);
   Real inc = CartesianToINC(mu, pos, vel, true);
   if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   {
      throw UtilityException
         ("Error in conversion to Keplerian state: "
          "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   }
   Real ecc = eVec.GetMagnitude();
   Real aop = 0.0;

   // Case 1:  Non-circular, Inclined Orbit
   if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   {
      Rvector3 nVec =  CartesianToDirOfLineOfNode(pos, vel);
      Real nMag = nVec.GetMagnitude();
      if ((nMag == 0.0) || (ecc == 0.0))
      {
         throw UtilityException("Cannot convert from Cartesian to AOP - n vector is a zero vector or eccentricity is zero.\n");
      }
      aop = ACos(((nVec*eVec) / (nMag*ecc)), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if (eVec[2] < 0.0)
         aop = TWO_PI - aop;
   }
   // Case 2: Non-circular, Equatorial Orbit
   else if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   {
      if (ecc == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to AOP - eccentricity is zero.\n");
      }
      aop = ACos((eVec[0] / ecc), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if (eVec[1] < 0.0)
         aop = TWO_PI - aop;
   }
   // Case 3: Circular, Inclined Orbit
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   {
      aop = 0.0;
   }
   // Case 4: Circular, Equatorial Orbit
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   {
      aop = 0.0;
   }

   #ifdef DEBUG_KEPLERIAN_AOP
   MessageInterface::ShowMessage("returning %f\n", aop);
   #endif

   // Convert 2pi to 0
   aop = Mod(aop, TWO_PI);

   if (inRadian)
      return aop;
   else
      return aop * DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Rvector3 CartesianToEccVector(Real mu, const Rvector3 &pos,
//                                      const Rvector3 &vel)
//------------------------------------------------------------------------------
/*
 * Computes eccentricity vector from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 *
 * @return  ECC vector
 */
//---------------------------------------------------------------------------
Rvector3 StateConversionUtil::CartesianToEccVector(Real mu, const Rvector3 &pos,
                                                   const Rvector3 &vel)
{
   Real rMag = pos.GetMagnitude();
   Real vMag = vel.GetMagnitude();
   if ((mu == 0.0) || (rMag == 0.0))
   {
      throw UtilityException("Cannot convert from Cartesian to EccVector - position vector is a zero vector or mu is zero.\n");
   }

   Rvector3 eVec = ((vMag*vMag - mu/rMag)*pos - (pos*vel)*vel) / mu;

   #ifdef DEBUG_ECC_VEC
   MessageInterface::ShowMessage
      ("CartesianToEccVector() returning\n   %s\n", eVec.ToString().c_str());
   #endif

   return eVec;
}

//------------------------------------------------------------------------------
// Rvector3 CartesianToDirOfLineOfNode(const Rvector3 &pos, const Rvector3 &vel)
//------------------------------------------------------------------------------
/*
 * Computes direction of line of nodes from input cartesian state
 *
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 *
 * @return  Dir of line of nodes
 */
//---------------------------------------------------------------------------
Rvector3 StateConversionUtil::CartesianToDirOfLineOfNode(const Rvector3 &pos, const Rvector3 &vel)
{
   Rvector3 hVec = Cross(pos, vel);
   Rvector3 nVec = Cross(Rvector3(0, 0, 1), hVec);
   return nVec;
}

//------------------------------------------------------------------------------
// Rvector6 CartesianToAngularMomentum(Real mu, const Rvector3 &pos,
//                                            const Rvector3 &vel)
//------------------------------------------------------------------------------
/**
 * CartesianTos angular momentum, its magnitude, and other related data.
 *
 * @param <mu>  gravitational constant of the central body
 * @param <pos> input position vector
 * @param <vel> input velocity vector
 *
 * @return [0] Angular momentum x unit vector
 *         [1] Angular momentum y unit vector
 *         [2] Angular momentum z unit vector
 *         [3] Angular momentum magnitude
 *         [4] velocity magnitude squared
 *         [5] Orbit parameter
 */
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToAngularMomentum(Real mu, const Rvector3 &pos,
                                                         const Rvector3 &vel)
{
   #ifdef DEBUG_KEPLERIAN_AM
   MessageInterface::ShowMessage("CartesianToAngularMomentum() ");
   #endif

   Real vMag = vel.GetMagnitude();
   Real vMagSq = vMag*vMag;
   if (mu == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Angular Momentum - mu is zero.\n");
   }

   Rvector3 hVec = Cross(pos, vel);
   Real hMag = Sqrt(hVec * hVec);
   Real orbParam = (hMag*hMag) / mu;
   hVec.Normalize();

   Rvector6 h(hVec[0], hVec[1], hVec[2], hMag, vMagSq, orbParam);

   #ifdef DEBUG_KEPLERIAN_AM
   MessageInterface::ShowMessage("returning %s\n", h.ToString().c_str());
   #endif

   return h;
}


//------------------------------------------------------------------------------
// bool ValidateValue(const std::string& label,          Real value,
//                    const std::string &compareTo = "", Real compareValue = 0.0)
//------------------------------------------------------------------------------
/**
 * Validates an input value for the specified orbit element.  When requested, also
 * validates the value against another input value, for coupled quantities.
 *
 * @param <label>        Orbit element label
 * @param <value>        Input value
 * @param <compareTo>    Name of coupled orbit element to validate against
 * @param <compareValue> Value of coupled orbit element to validate against
 *
 * @return true if value is allowed/valid; false otherwise
 */
//------------------------------------------------------------------------------
bool StateConversionUtil::ValidateValue(const std::string &label,       Real value,
                                        const std::string &errorMsgFmt, Integer dataPrecision,
                                        const std::string &compareTo,   Real compareValue)
{
   #ifdef DEBUG_SC_CONVERT_VALIDATE
      MessageInterface::ShowMessage(
            "Entering SCU::ValidateValue with label = %s, value = %le, errorMsgFmt = %s, dataPrecision = %d, compareTo = %s, compareValue = %le\n",
            label.c_str(), value, errorMsgFmt.c_str(), dataPrecision, compareTo.c_str(), compareValue);
   #endif

   std::string labelUpper   = GmatStringUtil::ToUpper(label);
   std::string compareUpper = GmatStringUtil::ToUpper(compareTo);
   #ifdef DEBUG_SC_CONVERT_VALIDATE
      MessageInterface::ShowMessage(
            "SCU::ValidateValue:  labelUpper = %s, compareUpper = %s\n", labelUpper.c_str(), compareUpper.c_str());
   #endif

   // these are only limited by the upper and lower limits of the compiler's Real type
   if ((labelUpper == "X")                     || (labelUpper == "Y")                  || (labelUpper == "Z")                        ||
       (labelUpper == "VX")                    || (labelUpper == "VY")                 || (labelUpper == "VZ")                       ||
       (labelUpper == "AOP")                   || (labelUpper == "AZI")                || (labelUpper == "EquinoctialP")             ||
       (labelUpper == "EquinoctialQ")          || (labelUpper == "RA")                 || (labelUpper == "RAAN")                     ||
       (labelUpper == "RAV")                   || (labelUpper == "TA"))
   {
      return true;
   }
   else if (labelUpper == "RADAPO")
   {
      if (GmatMathUtil::Abs(value) < .001)
      {
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "RadApo", "Real Number >= 1 meter");
         throw ue;
      }
      if (compareUpper == "RADPER")
      {
         if ((value > 0.0) && (value < compareValue))
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "RadApo", "Real Number < 0.0 if RadApo < RadPer");
            throw ue;
         }
      }
   }
   else if (labelUpper == "RADPER")
   {
      if (GmatMathUtil::Abs(value) < .001)
      {
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "RadPer", "Real Number >= 1 meter");
         throw ue;
      }
      else if (IsEqual(value, 0.0, .001))
      {
         UtilityException ue;
         ue.SetDetails("Parabolic orbits are not currently supported.  Radius of Periapsis must be greater than zero");
         throw ue;
      }
   }
   else if (labelUpper == "ECC")
   {
      if (IsEqual(value, 1.0, PARABOLIC_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "Real Number != 1";
         if (PARABOLIC_TOL != 0.0)
            rangeMsg << " (tolerance = " << PARABOLIC_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "ECC", rangeMsg.str().c_str());
         throw ue;
      }
      if (compareUpper == "SMA")
      {
         if (((value > 1.0 + PARABOLIC_TOL) || (value < 0.0 - PARABOLIC_TOL)) && (compareValue > 0.0))
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "ECC", "0 < Real Number < 1 when SMA > 0");
            throw ue;
         }
         if ((value <= 1.0 - PARABOLIC_TOL) && (compareValue < 0.0))
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "ECC", "Real Number > 1 when SMA < 0");
            throw ue;
         }
      }
   }
   else if (labelUpper == "SMA")
   {
      if (IsEqual(value, 0.0, SINGULAR_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "Real Number != 0";
         if (SINGULAR_TOL != 0.0)
            rangeMsg << " (tolerance = " << SINGULAR_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "SMA", rangeMsg.str().c_str());
         throw ue;
      }
      if (compareUpper == "ECC")
      {
         if ((value < 0.0) && (compareValue < 1.0 - SINGULAR_TOL) && (compareValue > 0.0 + SINGULAR_TOL))
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "SMA", "Real Number > 0 when 0 < ECC < 1");
            throw ue;
         }
         else if ((value > 0.0) && (compareValue > 1.0 + SINGULAR_TOL))
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "SMA", "Real Number < 0 when ECC > 1");
            throw ue;
         }
      }
   }
   else if (labelUpper == "INC")
   {
      if ((value < 0.0 - ANGLE_TOL) || (value > 180.0 + ANGLE_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "0.0 <= Real Number <= 180.0";
         if (ANGLE_TOL != 0.0)
            rangeMsg << " (tolerance = " << ANGLE_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "INC", rangeMsg.str().c_str());
         throw ue;
      }
   }
   else if (labelUpper == "RMAG")
   {
      if (value < 1.0e-10)
      {
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "RMAG", "Real Number > 1.0e-10");
         throw ue;
      }
   }
   else if (labelUpper == "VMAG")
   {
      if (value < 1.0e-10)
      {
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "VMAG", "Real Number > 1.0e-10");
         throw ue;
      }
   }
   else if (labelUpper == "DEC")
   {
      if ((value < -90.0 - ANGLE_TOL) || (value > 90.0 + ANGLE_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "-90.0 <= Real Number <= 90.0";
         if (ANGLE_TOL != 0.0)
            rangeMsg << " (tolerance = " << ANGLE_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "DEC", rangeMsg.str().c_str());
         throw ue;
      }
   }
   else if (labelUpper == "DECV")
   {
      if ((value < -90.0 - ANGLE_TOL) || (value > 90.0 + ANGLE_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "-90.0 <= Real Number <= 90.0";
         if (ANGLE_TOL != 0.0)
            rangeMsg << " (tolerance = " << ANGLE_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "DECV", rangeMsg.str().c_str());
         throw ue;
      }
   }
   else if (labelUpper == "FPA")
   {
      if ((value < 0 - ANGLE_TOL) || (value > 180 + ANGLE_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "0.0 <= Real Number <= 180.0";
         if (ANGLE_TOL != 0.0)
            rangeMsg << " (tolerance = " << ANGLE_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "FPA", rangeMsg.str().c_str());
         throw ue;
      }
   }
   else if (labelUpper == "EQUINOCTIALK")
   {
      if ((value < -1.0 + EQUINOCTIAL_TOL) || (value > 1.0 - EQUINOCTIAL_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "-1 < Real Number < 1";
         if (EQUINOCTIAL_TOL != 0.0)
            rangeMsg << " (tolerance = " << EQUINOCTIAL_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "EquinoctialK", rangeMsg.str().c_str());
         throw ue;
      }
      if (compareUpper == "EQUINOCTIALH")
      {
         if (GmatMathUtil::Sqrt(value * value + compareValue * compareValue) > 1.0 - EQUINOCTIAL_TOL)
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "EquinoctialK", "Sqrt(EquinoctialH^2 + EquinoctialK^2) < 0.99999");
            throw ue;
         }
      }
   }
   else if (labelUpper == "EQUINOCTIALH")
   {
      if ((value < -1.0 + EQUINOCTIAL_TOL) || (value > 1.0 - EQUINOCTIAL_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "-1 < Real Number < 1";
         if (EQUINOCTIAL_TOL != 0.0)
            rangeMsg << " (tolerance = " << EQUINOCTIAL_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "EquinoctialH", rangeMsg.str().c_str());
         throw ue;
      }
      if (compareUpper == "EQUINOCTIALK")
      {
         if (GmatMathUtil::Sqrt(value * value + compareValue * compareValue) > 1.0 - EQUINOCTIAL_TOL)
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "EquinoctialH", "Sqrt(EquinoctialH^2 + EquinoctialK^2) < 0.99999");
            throw ue;
         }
      }
   }
   else if (labelUpper == "MLONG")
   {
      if ((value < -360.0 - ANGLE_TOL) || (value > 360.0 + ANGLE_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "-360.0 <= Real Number <= 360.0";
         if (ANGLE_TOL != 0.0)
            rangeMsg << " (tolerance = " << ANGLE_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "MLONG", rangeMsg.str().c_str());
         throw ue;
      }
   }
   return true;
}

//------------------------------------------------------------------------------
// const std::string* GetStateTypeList()
//------------------------------------------------------------------------------
/**
 * Returns the list of state types
 *
 * @return list of valid state types
 */
//------------------------------------------------------------------------------
const std::string* StateConversionUtil::GetStateTypeList()
{
   return STATE_TYPE_TEXT;
}

//------------------------------------------------------------------------------
// bool RequiresCelestialBodyOrigin(const std::string &type)
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the specified state type
 * requires a celestial body origin
 *
 * @param <type> State type
 *
 * @return flag indicating whether or not the specified state type
 * requires a celestial body origin
 */
//------------------------------------------------------------------------------
bool StateConversionUtil::RequiresCelestialBodyOrigin(const std::string &type)
{
   for (unsigned int ii = 0; ii < StateTypeCount; ii++)
   {
      if (type == STATE_TYPE_TEXT[ii])  return REQUIRES_CB_ORIGIN[ii];
   }
   return false;  // by default
}

//------------------------------------------------------------------------------
// bool RequiresFixedCoordinateSystem(const std::string &type)
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the specified state type
 * requires a fixed coordinate system
 *
 * @param <type> State type
 *
 * @return flag indicating whether or not the specified state type
 * requires a fixed coordinate system
 */
//------------------------------------------------------------------------------
bool StateConversionUtil::RequiresFixedCoordinateSystem(const std::string &type)
{
   for (unsigned int ii = 0; ii < StateTypeCount; ii++)
   {
      if (type == STATE_TYPE_TEXT[ii])  return REQUIRES_FIXED_CS[ii];
   }
   return false;  // by default
}

//------------------------------------------------------------------------------
// AnomalyType GetAnomalyType(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Returns the AnomalyType corresponding to the specified string
 *
 * @param <typeStr> Anomaly type string
 *
 * @return anomaly type
 */
//------------------------------------------------------------------------------
StateConversionUtil::AnomalyType StateConversionUtil::GetAnomalyType(const std::string &typeStr)
{
   #ifdef DEBUG_ANOMALY
      MessageInterface::ShowMessage("GetAnomalyType (%s) called.\n", typeStr.c_str());
   #endif
   for (int i=0; i<AnomalyTypeCount; i++)
   {
      if (typeStr == ANOMALY_LONG_TEXT[i])
         return (AnomalyType) i;
   }

   for (int i=0; i<AnomalyTypeCount; i++)
   {
      if (typeStr == ANOMALY_SHORT_TEXT[i])
         return (AnomalyType) i;
   }

   throw UtilityException
      ("Invalid Anomaly Type \"" + typeStr + "\"\nAllowed "
       "are \"TA\", \"MA\", \"EA\", \"HA\" or \n\"True Anomaly\", "
       "\"Mean Anomaly\", \"Eccentric Anomaly\", \"Hyperbolic Anomaly\"");
}

//------------------------------------------------------------------------------
// bool IsValidAnomalyType(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not the input string is a valid anomaly type
 *
 * @param <typeStr> Anomaly type string
 *
 * @return flag indicating whether or not the input string is a valid anomaly type
 */
//------------------------------------------------------------------------------
bool StateConversionUtil::IsValidAnomalyType(const std::string &anomType)
{
   for (int i=0; i<AnomalyTypeCount; i++)
   {
      if (anomType == ANOMALY_LONG_TEXT[i])
         return true;
   }

   for (int i=0; i<AnomalyTypeCount; i++)
   {
      if (anomType == ANOMALY_SHORT_TEXT[i])
         return true;
   }

   return false;
}

//------------------------------------------------------------------------------
// bool IsRvValid(Real r[3], Real v[3])
//------------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not the input position/velocity is valid
 *
 * @param <r>   Position
 * @param <v>   Velocity
 *
 * @return flag indicating whether or not the input position/velocity is valid
 */
//------------------------------------------------------------------------------
bool StateConversionUtil::IsRvValid(Real r[3], Real v[3])
{
   if ( ((r[0]*r[0] + r[1]*r[1] + r[2]*r[2]) > ORBIT_TOL_SQ) &&
        ((v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) > ORBIT_TOL_SQ) )
      return true;
   else
      return false;

}

//------------------------------------------------------------------------------
// const std::string* GetLongTypeNameList()
//------------------------------------------------------------------------------
/**
 * Returns list of long anomaly names
 *
 * @return list of long anomaly names
 */
//------------------------------------------------------------------------------
const std::string* StateConversionUtil::GetLongTypeNameList()
{
   return ANOMALY_LONG_TEXT;
}

//------------------------------------------------------------------------------
// std::string GetAnomalyShortText(const std::string &anomalyType)
//------------------------------------------------------------------------------
/**
 * Returns the short anomaly name for the specified anomaly type
 *
 * @param <anomalyType>  anomaly type
 *
 * @return corresponding short anomaly name
 */
//------------------------------------------------------------------------------
std::string StateConversionUtil::GetAnomalyShortText(const std::string &type)
{
   if (type == "True Anomaly" || type == "TA")
      return "TA";
   else if (type == "Mean Anomaly" || type == "MA")
      return "MA";
   else if (type == "Eccentric Anomaly" || type == "EA")
      return "EA";
   else if (type == "Hyperbolic Anomaly" || type == "HA")
      return "HA";
   else
      throw UtilityException
         ("Invalid Anomaly Type \"" + type + "\"\nAllowed "
          "are \"TA\", \"MA\", \"EA\", \"HA\" or \n\"True Anomaly\", "
          "\"Mean Anomaly\", \"Eccentric Anomaly\", \"Hyperbolic Anomaly\"");
}

//------------------------------------------------------------------------------
// std::string GetAnomalyLongText(const std::string &type)
//------------------------------------------------------------------------------
/**
 * Returns the long anomaly name for the specified anomaly type
 *
 * @param <anomalyType>  anomaly type
 *
 * @return corresponding long anomaly name
 */
//------------------------------------------------------------------------------
std::string StateConversionUtil::GetAnomalyLongText(const std::string &type)
{
   if (type == "True Anomaly" || type == "TA")
      return "True Anomaly";
   else if (type == "Mean Anomaly" || type == "MA")
      return "Mean Anomaly";
   else if (type == "Eccentric Anomaly" || type == "EA")
      return "Eccentric Anomaly";
   else if (type == "Hyperbolic Anomaly" || type == "HA")
      return "Hyperbolic Anomaly";
   else
      throw UtilityException
         ("Invalid Anomaly Type \"" + type + "\"\nAllowed "
          "are \"TA\", \"MA\", \"EA\", \"HA\" or \n\"True Anomaly\", "
          "\"Mean Anomaly\", \"Eccentric Anomaly\", \"Hyperbolic Anomaly\"");

}


//------------------------------------------------------------------------------
// Integer ComputeCartToKepl(Real grav, Real r[3], Real v[3], Real *tfp,
//                           Real elem[6], Real *ma)
//------------------------------------------------------------------------------
/*
 * @param <grav> input gravitational constant
 * @param <r>    input position vector in cartesian coordinate
 * @param <v>    input velocity vector in cartesian coordinate
 * @param <tfp>  output time from periapsis
 * @param <elem> output six keplerian elements
 *               (1) = semimajor axis
 *               (2) = eccentricity
 *               (3) = inclination (deg)
 *               (4) = longitude of ascending node (deg)
 *               (5) = argument of periapsis (deg)
 *               (6) = true anomaly (deg)
 * @param <ma>   output mean anomaly (deg)
 *
 * @return       Error code
 */
//------------------------------------------------------------------------------
Integer StateConversionUtil::ComputeCartToKepl(Real grav,    Real r[3], Real v[3], Real *tfp,
                                               Real elem[6], Real *ma)
{
   #ifdef DEBUG_CART_TO_KEPL
      MessageInterface::ShowMessage("StateConversionUtil::ComputeCartToKepl called ... \n");
      MessageInterface::ShowMessage("   grav = %12.10f\n", grav);
      MessageInterface::ShowMessage("   KEP_ECC_TOL = %12.10f\n", GmatOrbitConstants::KEP_ECC_TOL);
   #endif

   if (Abs(grav) < 1E-30)
      return(2);

   Rvector3 pos(r[0], r[1], r[2]);
   Rvector3 vel(v[0], v[1], v[2]);

   // eqn 4.1
   Rvector3 angMomentum = Cross(pos, vel);

   // eqn 4.2
   Real h = angMomentum.GetMagnitude();
   #ifdef DEBUG_CART_TO_KEPL
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, pos = %12.14f  %12.14f  %12.14f \n", pos[0], pos[1], pos[2]);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, vel = %12.14f  %12.14f  %12.14f \n", vel[0], vel[1], vel[2]);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, angMomentum = %12.10f  %12.10f  %12.10f\n",
            angMomentum[0], angMomentum[1], angMomentum[2]);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, h = %12.10f\n", h);
   #endif

   // eqn 4.3
   Rvector3 v3(0.0,0.0,1.0);
   Rvector3 nodeVec = Cross(v3, angMomentum);

   // eqn 4.4
   Real n = nodeVec.GetMagnitude();

   // eqn 4.5 - 4.6
   Real posMag = pos.GetMagnitude();
   Real velMag = vel.GetMagnitude();
   if (posMag == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Keplerian - position vector is a zero vector.\n");
   }
   if (grav == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Keplerian - mu is zero.\n");
   }

   // eqn 4.7 - 4.8
   Rvector3 eccVec = (1/grav)*((velMag*velMag - grav/posMag)*pos - (pos * vel) * vel);
   Real e = eccVec.GetMagnitude();

   // eqn 4.9
   Real zeta = 0.5*velMag*velMag - (grav/posMag);
   if (zeta == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Keplerian - computed zeta is zero.\n");
   }

   #ifdef DEBUG_CART_TO_KEPL
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, nodeVec = %12.10f  %12.10f  %12.10f \n",
            nodeVec[0], nodeVec[1], nodeVec[2]);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, n = %12.10f\n", n);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, posMag = %12.10f\n", posMag);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, velMag = %12.10f\n", velMag);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, eccVec = %12.10f  %12.10f  %12.10f \n", eccVec[0], eccVec[1], eccVec[2]);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, e = %12.10f\n", e);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, zeta = %12.10f\n", zeta);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, Abs(1.0 - e) = %12.10f\n", Abs(1.0 - e));
   #endif

   if ((Abs(1.0 - e)) <= GmatOrbitConstants::KEP_ECC_TOL)
   {
      std::string errmsg = "Warning: GMAT does not support parabolic orbits ";
      errmsg += "in conversion from Cartesian to Keplerian state.\n";
      throw UtilityException(errmsg);
   }

   // eqn 4.10
   Real sma = -grav/(2*zeta);
   #ifdef DEBUG_CART_TO_KEPL
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, sma = %12.10f\n", sma);
   #endif


   if (Abs(sma*(1 - e)) < .001)
   {
      throw UtilityException
      ("Error in conversion from Cartesian to Keplerian state: "
       "The state results in a singular conic section with radius of periapsis less than 1 m.\n");

   }
   // eqn 4.11
   if (h == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Keplerian - angular momentum is zero.\n");
   }
   Real i = ACos( angMomentum.Get(2)/h );
   if (i >= PI - GmatOrbitConstants::KEP_TOL)
   {
      throw UtilityException
         ("Error in conversion to Keplerian state: "
          "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   }
   Real raan         = 0.0;
   Real argPeriapsis = 0.0;
   Real trueAnom     = 0.0;

   if ( e >= 1E-11 && i >= 1E-11 )  // CASE 1: Non-circular, Inclined Orbit
   {
      if (n == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to Keplerian - line-of-nodes vector is a zero vector.\n");
      }
      raan = ACos( nodeVec.Get(0)/n );
      if (nodeVec.Get(1) < 0)
         raan = TWO_PI - raan;

      argPeriapsis = ACos( (nodeVec*eccVec)/(n*e) );
      if (eccVec.Get(2) < 0)
         argPeriapsis = TWO_PI - argPeriapsis;

      trueAnom = ACos( (eccVec*pos)/(e*posMag) );
      if (pos*vel < 0)
         trueAnom = TWO_PI - trueAnom;
   }
   if ( e >= 1E-11 && i < 1E-11 )  // CASE 2: Non-circular, Equatorial Orbit
   {
      if (e == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to Keplerian - eccentricity is zero.\n");
      }
      raan = 0;
      argPeriapsis = ACos(eccVec.Get(0)/e);
      if (eccVec.Get(1) < 0)
         argPeriapsis = TWO_PI - argPeriapsis;

      trueAnom = ACos( (eccVec*pos)/(e*posMag) );
      if (pos*vel < 0)
         trueAnom = TWO_PI - trueAnom;
   }
   if ( e < 1E-11 && i >= 1E-11 )  // CASE 3: Circular, Inclined Orbit
   {
      if (n == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to Keplerian - line-of-nodes vector is a zero vector.\n");
      }
      raan = ACos( nodeVec.Get(0)/n );
      if (nodeVec.Get(1) < 0)
         raan = TWO_PI - raan;

      argPeriapsis = 0;

      trueAnom = ACos( (nodeVec*pos)/(n*posMag) );
      if (pos.Get(2) < 0)
         trueAnom = TWO_PI - trueAnom;
   }
   if ( e < 1E-11 && i < 1E-11 )  // CASE 4: Circular, Equatorial Orbit
   {
      raan = 0;
      argPeriapsis = 0;
      trueAnom = ACos(pos.Get(0)/posMag);
      if (pos.Get(1) < 0)
         trueAnom = TWO_PI - trueAnom;
   }

   elem[0] = sma;
   elem[1] = e;
   elem[2] = i*DEG_PER_RAD;
   elem[3] = raan*DEG_PER_RAD;
   elem[4] = argPeriapsis*DEG_PER_RAD;
   elem[5] = trueAnom*DEG_PER_RAD;

   return 0;
}


//------------------------------------------------------------------------------
// Integer ComputeKeplToCart(Real grav, Real elem[6], Real r[3],
//                           Real v[3], AnomalyType anomalyType)
//------------------------------------------------------------------------------
/**
 * Converts Kelperian to Cartesian
 *
 * @param <grav>        Gravitational constant for the central body
 * @param <elem>        Keplerian elements
 * @param <r>           Output Cartesian position
 * @param <v>           Output Cartesian velocity
 * @param <anomalyType> Anomaly type
 *
 * @return Error code
 */
//------------------------------------------------------------------------------
Integer StateConversionUtil::ComputeKeplToCart(Real grav, Real elem[6], Real r[3],
                                               Real v[3], AnomalyType anomalyType)
{
   Real sma = elem[0],
        ecc = elem[1],
        inc = elem[2]*RAD_PER_DEG,
       raan = elem[3]*RAD_PER_DEG,
        per = elem[4]*RAD_PER_DEG,
       anom = elem[5]*RAD_PER_DEG;

   // **************
   // taken from old code above; necessary to avoid crash, but not in spec
   // if input keplerian anomaly is MA, convert to TA  <<<< what about HA or EA?????   wcs 2011.11.23 ????
   if (anomalyType == MA)
   {
      Real ta;
      Integer iter;
      Integer ret = ComputeMeanToTrueAnomaly(anom, ecc, 1E-8, &ta, &iter);

      if (ret > 0)
         return ret;
      else
         anom = ta;
   }
   // ***************

   // radius near infinite
   #ifdef DEBUG_KEPL_TO_CART
      MessageInterface::ShowMessage("ecc = %12.10f, anom = %12.10f, Cos(anom) = %12.10f, 1+ecc*Cos(anom) = %12.10f\n",
            ecc, anom, Cos(anom), (1+ecc*Cos(anom)));
   #endif

   // eqn 4.24; semilatus rectum
   Real p = sma*(1 - ecc*ecc);

   // orbit parabolic
   if (Abs(p) < INFINITE_TOL)
   {
        return 2;
   }

   Real onePlusECos = 1 + ecc*Cos(anom);
   if (onePlusECos < ORBIT_TOL)
   {
      std::string warn = "Warning: The orbital radius is large in the conversion from ";
      warn += "Keplerian elements to Cartesian state and the state may be near a singularity, ";
      warn += "causing numerical errors in the conversion.\n";
      MessageInterface::PopupMessage(Gmat::WARNING_, warn);
   }
   // eqn 4.25; radius
   Real rad = p/onePlusECos;

   // eqn 4.26 - 4.28
   Real cosPerAnom    = Cos(per + anom);
   Real sinPerAnom    = Sin(per + anom);
   Real cosInc        = Cos(inc);
   Real sinInc        = Sin(inc);
   Real cosRaan       = Cos(raan);
   Real sinRaan       = Sin(raan);
   Real sqrtGravP     = Sqrt(grav/p);
   Real cosAnomPlusE  = Cos(anom) + ecc;
   Real sinAnom       = Sin(anom);
   Real cosPer        = Cos(per);
   Real sinPer        = Sin(per);

   r[0] = rad * (cosPerAnom * cosRaan - cosInc * sinPerAnom * sinRaan),
   r[1] = rad * (cosPerAnom * sinRaan + cosInc * sinPerAnom * cosRaan),
   r[2] = rad * sinPerAnom  * sinInc;

   v[0] = sqrtGravP * cosAnomPlusE*(-sinPer*cosRaan-cosInc*sinRaan*cosPer)
                    - sqrtGravP*sinAnom*(cosPer*cosRaan-cosInc*sinRaan*sinPer);

   v[1] = sqrtGravP * cosAnomPlusE*(-sinPer*sinRaan+cosInc*cosRaan*cosPer)
                    - sqrtGravP*sinAnom*(cosPer*sinRaan+cosInc*cosRaan*sinPer);

   v[2] = sqrtGravP * (cosAnomPlusE*sinInc*cosPer - sinAnom*sinInc*sinPer);

   return 0;
}

//------------------------------------------------------------------------------
// Integer ComputeMeanToTrueAnomaly(Real maRadians, Real ecc, Real tol,
//                                  Real *ta, Integer *iter)
//------------------------------------------------------------------------------
/*
 * @param <maRadians>   input elliptical, hyperbolic or mean anomaly in radians
 * @param <ecc>         input eccentricity
 * @param <tol>         input tolerance for accuracy
 * @param <ta>          output true anomaly in radians
 * @param <iter>        output number of iterations
 *
 * @return              error code
 */
//------------------------------------------------------------------------------
Integer StateConversionUtil::ComputeMeanToTrueAnomaly(Real maRadians, Real ecc, Real tol,
                                                      Real *ta,     Integer *iter)
{
   Real temp, temp2;
   Real rm,   e,     e1,  e2,  c,  f,  f1,   f2,    g;
   Real ztol = 1.0e-30;
   int done;

   rm = maRadians;
   *iter = 0;

   if (ecc <= 1.0)
   {
      //---------------------------------------------------------
      // elliptical orbit
      //---------------------------------------------------------

      e2 = rm + ecc * Sin(rm);
      done = 0;

      while (!done)
      {
         *iter = *iter + 1;
         temp = 1.0 - ecc * Cos(e2);
         if (temp == 0.0)
         {
            throw UtilityException("Cannot convert Mean to True Anomaly - computed temp is zero.\n");
         }

         if (Abs(temp) < ztol)
            return (3);

         e1 = e2 - (e2 - ecc * Sin(e2) - rm)/temp;

         if (Abs(e2-e1) < tol)
         {
            done = 1;
            e2 = e1;
         }

         if (!done)
         {
            *iter = *iter + 1;
            temp = 1.0 - ecc * Cos(e1);

            if (Abs(temp) < ztol)
               return (4);

            e2 = e1 - (e1 - ecc * Sin(e1) - rm)/temp;

            if( Abs(e1-e2) < tol)
               done = 1;
         }
      }

      e = e2;

      if (e < 0.0)
         e = e + TWO_PI;

      c = Abs(e - GmatMathConstants::PI);

      if (c >= 1.0e-08)
      {
         temp  = 1.0 - ecc;

         if (Abs(temp)< ztol)
            return (5);

         temp2 = (1.0 + ecc)/temp;

         if (temp2 <0.0)
            return (6);

         f = Sqrt(temp2);
         g = Tan(e/2.0);
         *ta = 2.0 * ATan(f*g);

      }
      else
      {
         *ta = e;
      }

      if( *ta < 0.0)
         *ta = *ta + GmatMathConstants::TWO_PI;

   }
   else
   {
      //---------------------------------------------------------
      // hyperbolic orbit
      //---------------------------------------------------------

      if (rm > PI)
         rm = rm - TWO_PI;

      f2 = ecc * Sinh(rm) - rm;
      done = 0;

      while (!done)
      {
         *iter = *iter + 1;
         temp = ecc * Cosh(f2) - 1.0;

         if (Abs(temp) < ztol)
            return (7);

         f1 = f2 - (ecc * Sinh(f2) - f2 - rm) / temp;

         if (Abs(f2-f1) < tol)
         {
            done = 1;
            f2 = f1;
         }

         if (!done)
         {
            *iter = *iter + 1;
            temp = ecc * Cosh(f1) - 1.0;

            if (Abs(temp) < ztol)
               return (8);

            f2 = f1 - (ecc * Sinh(f1) - f1 - rm) / temp;

            if ( Abs(f1-f2) < tol)
               done = 1;
         }

         if (*iter > 1000)
         {
            throw UtilityException
               ("ComputeMeanToTrueAnomaly() "
                "Caught in infinite loop numerical argument "
                "out of domain for sinh() and cosh()\n");
         }
      }

      f = f2;
      temp = ecc - 1.0;

      if (Abs(temp) < ztol)
         return (9);

      temp2 = (ecc + 1.0) / temp;

      if (temp2 <0.0)
         return (10);

      e = Sqrt(temp2);
      g = Tanh(f/2.0);
      *ta = 2.0 * ATan(e*g);

      if (*ta < 0.0)
         *ta = *ta + GmatMathConstants::TWO_PI;
   }

   return (0);

} // end ComputeMeanToTrueAnomaly()
