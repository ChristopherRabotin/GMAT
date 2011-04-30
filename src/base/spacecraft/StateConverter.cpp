//$Id$ 
//------------------------------------------------------------------------------
//                              StateConverter
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
// Author:  Joey Gurganus
// Created: 2004/03/22
//
/**
 * Implements the StateConverter class
 */
//------------------------------------------------------------------------------

#include "StateConverter.hpp"
#include "SpacePoint.hpp"
//#include "Equinoctial.hpp"
#include "CoordUtil.hpp"
#include "Keplerian.hpp"
#include "ModKeplerian.hpp"
//#include "Equinoctial.hpp"
//#include "SphericalAZFPA.hpp"
//#include "SphericalRADEC.hpp"
#include "MessageInterface.hpp"
#include "UtilityException.hpp"
#include "GmatConstants.hpp"
#include "GmatDefaults.hpp"
#include "RealUtilities.hpp"

using namespace GmatMathUtil;
using namespace GmatMathConstants;

//#define DEBUG_STATE_CONVERTER 1
//#define DEBUG_STATE_CONVERTER_SQRT

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const Real StateConverter::DEFAULT_MU = GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH];
const std::string StateConverter::STATE_TYPE_TEXT[StateTypeCount] =
{
   "Cartesian",
   "Keplerian",
   "ModifiedKeplerian",
   "SphericalAZFPA",
   "SphericalRADEC",
   "Equinoctial",
};

const bool StateConverter::REQUIRES_CB_ORIGIN[StateTypeCount] =
{
      false,
      true,
      true,
      false,
      false,
      true,
};
//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  StateConverter()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 */
//---------------------------------------------------------------------------
StateConverter::StateConverter()
{
   mMu = DEFAULT_MU;
}


//---------------------------------------------------------------------------
//  StateConverter(const std::string &newTtype)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <newType> Element type of coordinate system.
 *
 */
//---------------------------------------------------------------------------
StateConverter::StateConverter(const std::string &newType) 
{
   mMu = DEFAULT_MU;
}


//---------------------------------------------------------------------------
//  StateConverter(const std::string &newTtype, const Real newMu)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <newType> Element type of coordinate system.
 *
 */
//---------------------------------------------------------------------------
StateConverter::StateConverter(const std::string &newType, const Real newMu) 
{
   mMu = newMu;
}


//---------------------------------------------------------------------------
//  StateConverter(const StateConverter &stateConverter)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base StateConverter structures.
 *
 * @param <stateConverter> The original that is being copied.
 */
//---------------------------------------------------------------------------
StateConverter::StateConverter(const StateConverter &stateConverter) 
{
   mMu = stateConverter.mMu; 
}


//---------------------------------------------------------------------------
//  ~StateConverter()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
StateConverter::~StateConverter()
{
}

//---------------------------------------------------------------------------
//  StateConverter& operator=(const StateConverter &a)
//---------------------------------------------------------------------------
/**
 * Assignment operator for StateConverter structures.
 *
 * @param <a> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
StateConverter& StateConverter::operator=(const StateConverter &converter)
{
   // Don't do anything if copying self
   if (&converter == this)
      return *this;
   
   mMu = converter.mMu; 
   
   return *this;
}


//---------------------------------------------------------------------------
//  bool StateConverter::SetMu(const CoordinateSystem *cs)
//---------------------------------------------------------------------------
/**
 * Set the mu from the central body of the given coordinate system.
 *
 * @param <cs>  Given coordinate system
 * @return true if successful; otherwise, return false
 *
 */
//---------------------------------------------------------------------------
bool StateConverter::SetMu(const CoordinateSystem *coordSys)
{
   #ifdef DEBUG_STATE_CONVERTER
      MessageInterface::ShowMessage("\nStateConverter::SetMu(cs) enters...\n");
   #endif
      
   // Check for empty coordinate system then stop process
   if (coordSys == NULL)
      return false;
   
   // Get the coordinate system's origin
   SpacePoint *origin = coordSys->GetOrigin();
   
   #ifdef DEBUG_STATE_CONVERTER
      std::string typeName = origin->GetTypeName();
      MessageInterface::ShowMessage("...Origin type is '%s'.", typeName.c_str());
      
      if (origin->IsOfType(Gmat::CELESTIAL_BODY))
         MessageInterface::ShowMessage("It is CelestialBody.\n");
      else
         MessageInterface::ShowMessage(
            "CoordSys origin is not a CelestialBody.\n ");
   #endif
      
   // Check if it is Celestial Body then get the mu; 
   // Otherwise, it sets mu to zero
   if (origin->IsOfType(Gmat::CELESTIAL_BODY))
      mMu = ((CelestialBody *)origin)->GetGravitationalConstant(); 
   else
      mMu = 0.0;
   
   #ifdef DEBUG_STATE_CONVERTER
      MessageInterface::ShowMessage("...mMu = %f before "
        "StateConverter::SetMu() exits\n\n", mMu);
   #endif
      
   return true;
}


//---------------------------------------------------------------------------
// Rvector6 FromCartesian(const Rvector6 &state, const std::string &toType,
//                        const std::string &anomalyType = "TA")
//---------------------------------------------------------------------------
Rvector6 StateConverter::FromCartesian(const Rvector6 &state,
                                       const std::string &toType,
                                       const std::string &anomalyType)
{
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromCartesian() toType=%s, anomalyType=%s\n"
       "   state=%s\n", toType.c_str(), anomalyType.c_str(),
       state.ToString(13).c_str());
   #endif

   if (toType == STATE_TYPE_TEXT[CARTESIAN])
      return state;
   
   Rvector6 outState;
   
   if (toType == "Keplerian" || toType == "ModifiedKeplerian") 
   {
      Rvector6 kepl = Keplerian::CartesianToKeplerian(mMu, state, anomalyType);
      
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
      outState = CartesianToEquinoctial(state, mMu);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"Cartesian\" to \"" + toType +
          "\". \"" + toType + "\" is Unknown State Type\n");
   }
   
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromCartesian() returning (%s)\n   %s\n",
       toType.c_str(), outState.ToString(13).c_str());
   #endif

   return outState;
}


//---------------------------------------------------------------------------
// Rvector6 FromKeplerian(const Rvector6 &state, const std::string &toType,
//                        const std::string &anomalyType = "TA")
//---------------------------------------------------------------------------
Rvector6 StateConverter::FromKeplerian(const Rvector6 &state,
                                       const std::string &toType,
                                       const std::string &anomalyType)
{
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromKeplerian() toType=%s, anomalyType=%s\n"
       "   state=%s\n", toType.c_str(), anomalyType.c_str(),
       state.ToString(13).c_str());
   #endif
   
   if (toType == STATE_TYPE_TEXT[MOD_KEPLERIAN])
      return state;
   
   Anomaly anomaly;
   anomaly.Set(state[0], state[1], state[5], anomalyType);
   Rvector6 outState;
   
   if (toType == "Cartesian")
   {
      //outState = CoordUtil::KeplerianToCartesian(state, mMu, anomaly);
      outState = Keplerian::KeplerianToCartesian(mMu, state, anomalyType);
   }
   else if (toType == "ModifiedKeplerian")
   {
      outState = KeplerianToModKeplerian(state); 
   }
   else if (toType == "SphericalAZFPA")
   {
      Rvector6 cartesian = Keplerian::KeplerianToCartesian(mMu, state, anomalyType);
      outState           = CartesianToSphericalAZFPA(cartesian);
   }
   else if (toType == "SphericalRADEC")
   {
      Rvector6 cartesian = Keplerian::KeplerianToCartesian(mMu, state, anomalyType);
      outState           = CartesianToSphericalRADEC(cartesian);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"Keperian\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromKeplerian() returning \n   %s\n",
       outState.ToString(13).c_str());
   #endif
   
   return outState;
}


//---------------------------------------------------------------------------
// Rvector6 FromModKeplerian(const Rvector6 &state, const std::string &toType,
//                           const std::string &anomalyType = "TA")
//---------------------------------------------------------------------------
Rvector6 StateConverter::FromModKeplerian(const Rvector6 &state,
                                          const std::string &toType,
                                          const std::string &anomalyType)
{
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromModKeplerian() toType=%s, anomalyType=%s\n"
       "   state=%s\n", toType.c_str(), anomalyType.c_str(),
       state.ToString(13).c_str());
   #endif
   
   if (toType == STATE_TYPE_TEXT[KEPLERIAN])
      return state;
   
   Anomaly anomaly;
   anomaly.Set(state[0], state[1], state[5], anomalyType);
   Rvector6 outState;
   
   if (toType == "Cartesian")
   {
      Rvector6 keplerian = ModKeplerianToKeplerian(state);
      outState = CoordUtil::KeplerianToCartesian(keplerian, mMu, anomaly);
   }
   else if (toType == "Keplerian")
   {
      outState = ModKeplerianToKeplerian(state); 
   }
   else if (toType == "SphericalAZFPA")
   {
      Rvector6 keplerian = ModKeplerianToKeplerian(state);
      Rvector6 cartesian = Keplerian::KeplerianToCartesian(mMu, keplerian, anomalyType);
      outState           = CartesianToSphericalAZFPA(cartesian);
   }
   else if (toType == "SphericalRADEC")
   {
      Rvector6 keplerian = ModKeplerianToKeplerian(state);
      Rvector6 cartesian = Keplerian::KeplerianToCartesian(mMu, keplerian, anomalyType);
      outState           = CartesianToSphericalRADEC(cartesian);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"ModKeplerian\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromModKeplerian() returning \n   %s\n",
       outState.ToString(13).c_str());
   #endif
   
   return outState;
}


//---------------------------------------------------------------------------
// Rvector6 FromSphericalAZFPA(const Rvector6 &state, const std::string &toType,
//                             const std::string &anomalyType = "TA")
//---------------------------------------------------------------------------
Rvector6 StateConverter::FromSphericalAZFPA(const Rvector6 &state,
                                            const std::string &toType,
                                            const std::string &anomalyType)
{
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromSphericalAZFPA() toType=%s, anomalyType=%s\n"
       "   state=%s\n", toType.c_str(), anomalyType.c_str(),
       state.ToString(13).c_str());
   #endif
   
   if (toType == STATE_TYPE_TEXT[SPH_AZFPA])
      return state;
   
   Anomaly anomaly;
   anomaly.Set(state[0], state[1], state[5], anomalyType);
   Rvector6 outState;
      
   if (toType == "Cartesian")
   {
      outState = SphericalAZFPAToCartesian(state);
   }
   else if (toType == "Keplerian")
   {
      Rvector6 cartesian = SphericalAZFPAToCartesian(state);
      outState           = Keplerian::CartesianToKeplerian(mMu, cartesian, anomalyType);
   }
   else if (toType == "ModifiedKeplerian")
   {
      Rvector6 cartesian = SphericalAZFPAToCartesian(state);
      Rvector6 keplerian = Keplerian::CartesianToKeplerian(mMu, cartesian, anomalyType);
//      Rvector6 keplerian = SphericalAZFPAToKeplerian(state, mMu, anomaly);
      outState           = KeplerianToModKeplerian(keplerian);
   }
   else if (toType == "SphericalRADEC")
   {
      Rvector6 cartesian = SphericalAZFPAToCartesian(state);
      outState           = CartesianToSphericalRADEC(cartesian);
//      outState = AZFPA_To_RADECV(state);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"SphericalAZFPA\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromSphericalAZFPA() returning \n   %s\n",
       outState.ToString(13).c_str());
   #endif
   
   return outState;
}


//---------------------------------------------------------------------------
// Rvector6 FromSphericalRADEC(const Rvector6 &state, const std::string &toType,
//                             const std::string &anomalyType = "TA")
//---------------------------------------------------------------------------
Rvector6 StateConverter::FromSphericalRADEC(const Rvector6 &state,
                                            const std::string &toType,
                                            const std::string &anomalyType)
{
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromSphericalRADEC() toType=%s, anomalyType=%s\n"
       "   state=%s\n", toType.c_str(), anomalyType.c_str(),
       state.ToString(13).c_str());
   #endif
   
   if (toType == STATE_TYPE_TEXT[SPH_RADEC])
      return state;
   
   Anomaly anomaly;
   anomaly.Set(state[0], state[1], state[5], anomalyType);
   Rvector6 outState;
   
   if (toType == "Cartesian")
   {
      outState = SphericalRADECToCartesian(state);
   }
   else if (toType == "Keplerian")
   {
      Rvector6 cartesian = SphericalRADECToCartesian(state);
      outState           = Keplerian::CartesianToKeplerian(mMu, cartesian, anomalyType);
//      outState = SphericalRADECToKeplerian(state, mMu, anomaly);
   }
   else if (toType == "ModifiedKeplerian")
   {
      Rvector6 cartesian = SphericalRADECToCartesian(state);
      Rvector6 keplerian = Keplerian::CartesianToKeplerian(mMu, cartesian, anomalyType);
      outState           = KeplerianToModKeplerian(keplerian);
//      Rvector6 keplerian = SphericalRADECToKeplerian(state, mMu, anomaly);
//      outState = KeplerianToModKeplerian(keplerian);
   }
   else if (toType == "SphericalAZFPA")
   {
      Rvector6 cartesian = SphericalRADECToCartesian(state);
      outState           = CartesianToSphericalAZFPA(cartesian);
//      outState = RADECV_To_AZFPA(state);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"SphericalRADEC\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromSphericalRADEC() returning \n   %s\n",
       outState.ToString(13).c_str());
   #endif
   
   return outState;
}


//---------------------------------------------------------------------------
// Rvector6 FromEquinoctial(const Rvector6 &state, const std::string &toType,
//                          const std::string &anomalyType = "TA")
//---------------------------------------------------------------------------
Rvector6 StateConverter::FromEquinoctial(const Rvector6 &state,
                                         const std::string &toType,
                                         const std::string &anomalyType)
{
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromEquinoctial() toType=%s, anomalyType=%s\n"
       "   state=%s\n", toType.c_str(), anomalyType.c_str(),
       state.ToString(13).c_str());
   #endif
   
   if (toType == STATE_TYPE_TEXT[EQUINOCTIAL])
      return state;
   
   Anomaly anomaly;
   anomaly.Set(state[0], state[1], state[5], anomalyType);
   
   Rvector6 cartState = EquinoctialToCartesian(state, mMu);
   Rvector6 outState;
   
   if (toType == "Cartesian")
   {
      outState = cartState;
   }
   else if (toType == "Keplerian" || toType == "ModifiedKeplerian") 
   {
      Rvector6 kepl =
         CoordUtil::CartesianToKeplerian(cartState, mMu, anomaly);
      
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
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"Equinoctial\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }

   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromEquinoctial() returning \n   %s\n",
       outState.ToString(13).c_str());
   #endif
   
   return outState;
   
}


//---------------------------------------------------------------------------
//  Rvector6 Convert(const Rvector6 &state, const std::string &fromType,
//                   const std::string &toType,
//                   const std::string &anomalyType = "TA")
//---------------------------------------------------------------------------
/**
 * Converts state from fromType to toType.
 *
 * @param <state> state to convert
 * @param <fromType>  state type to convert from
 * @param <toType> state type to convert to
 * @param <anomalyType> anomaly type string if toType is Mod/Keplerian
 *
 * @return Converted states from the specific element type 
 */
//---------------------------------------------------------------------------
Rvector6 StateConverter::Convert(const Rvector6 &state,   
                                 const std::string &fromType,
                                 const std::string &toType,
                                 const std::string &anomalyType)
{
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::Convert() fromType=%s, toType=%s, anomalyType=%s\n"
       "   state=%s\n", fromType.c_str(), toType.c_str(), anomalyType.c_str(),
       state.ToString(13).c_str());
   #endif
   
   if (fromType == toType)
      return state;

   Rvector6 outState;
   
   try
   {
      // Determine the input of state type
      if (fromType == "Cartesian") 
      {
         outState = FromCartesian(state, toType, anomalyType);
      }
      else if (fromType == "Keplerian")
      {       
         outState = FromKeplerian(state, toType, anomalyType);
      }
      else if (fromType == "ModifiedKeplerian")
      {
         outState = FromModKeplerian(state, toType, anomalyType);
      }
      else if (fromType == "SphericalAZFPA")
      {
         outState = FromSphericalAZFPA(state, toType, anomalyType);
      }
      else if (fromType == "SphericalRADEC")
      {       
         outState = FromSphericalRADEC(state, toType, anomalyType);
      }
      else if (fromType == "Equinoctial")
      {
         outState = FromEquinoctial(state, toType, anomalyType);
      }
      else
      {
         throw UtilityException
            ("StateConverter::Convert() Cannot convert the state \"" +
             fromType + "\" to \"" + toType + "\". \"" + fromType +
             " is Unknown State Type\n");
      }
   }
   catch(UtilityException &ue)
   {
      throw ue;
   }

   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::Convert() returning \n   %s\n", outState.ToString(13).c_str());
   #endif
   
   return outState;
}


//---------------------------------------------------------------------------
// Rvector6 Convert(const Rvector6 &state, const std::string &fromType,
//                  const std::string &toType, Anomaly &anomaly)
//---------------------------------------------------------------------------
/**
 * Assignment operator for StateConverter structures.
 *
 * @param <state>     Element states 
 * @param <fromType>  Element Type 
 * @param <toType>    Element Type
 * @param <anomaly>   Anomaly
 *
 * @return Converted states from the specific element type 
 */
//---------------------------------------------------------------------------
Rvector6 StateConverter::Convert(const Rvector6 &state,   
                                 const std::string &fromType,
                                 const std::string &toType,
                                 Anomaly &anomaly)
{
   #ifdef DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
       ("StateConverter::Convert() fromType=%s, toType=%s, state=\n%s\n",
        fromType.c_str(), toType.c_str(), state.ToString(13).c_str());
   MessageInterface::ShowMessage
       ("Anomaly info-> a: %f, e: %f, type:%s, value: %f\n",
        anomaly.GetSMA(),anomaly.GetECC(), anomaly.GetTypeString().c_str(),
        anomaly.GetValue());
   #endif
   
   // Check if both are the same then return the state with no conversion 
   if (fromType == toType)
      return state;
   
   std::string anomalyType = anomaly.GetTypeString();
   return Convert(state, fromType, toType, anomalyType);
   
}


//---------------------------------------------------------------------------
//  Rvector6 StateConverter::Convert(Real *state,   
//                                   const std::string &fromType,
//                                   const std::string &toType,
//                                   Anomaly &anomaly)
//---------------------------------------------------------------------------
/**
 * Assignment operator for StateConverter structures.
 *
 * @param <state>     Element states 
 * @param <fromType>  Element Type converted from
 * @param <toType>    Element Type converted to
 * @param <anomaly>   Anomaly 
 *
 * @return Converted states from the specific element type 
 */
//---------------------------------------------------------------------------
Rvector6 StateConverter::Convert(const Real *state,   
                                 const std::string &fromType,
                                 const std::string &toType,
                                 Anomaly &anomaly)
{
   Rvector6 newState;
   newState.Set(state[0], state[1], state[2], state[3], state[4], state[5]);
   
   if (fromType == toType)
      return newState;
   
   std::string anomalyType = anomaly.GetTypeString();
   return Convert(newState, fromType, toType, anomalyType);
}


//------------------------------------------------------------------------------
// static const std::string* GetStateTypeList()
//------------------------------------------------------------------------------
const std::string* StateConverter::GetStateTypeList()
{
   return STATE_TYPE_TEXT;
}

//------------------------------------------------------------------------------
// bool RequiresCelestialBodyOrigin(const std::string &type)
//------------------------------------------------------------------------------
bool StateConverter::RequiresCelestialBodyOrigin(const std::string &type)
{
   for (unsigned int ii = 0; ii < StateTypeCount; ii++)
   {
      if (type == STATE_TYPE_TEXT[ii])  return REQUIRES_CB_ORIGIN[ii];
   }
   return false;  // by default
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none


//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
Rvector6 StateConverter::CartesianToEquinoctial(const Rvector6& cartesian, const Real& mu)
{
   Real sma, h, k, p, q, lambda; // equinoctial elements

   Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
   Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
   Real r = pos.GetMagnitude();
   Real v = vel.GetMagnitude();

   Rvector3 eVec = ( ((v*v - mu/r) * pos) - ((pos * vel) * vel) ) / mu;
   Real e = eVec.GetMagnitude();

   // Check for a near parabolic or hyperbolic orbit.
   if ( e > 1.0 - GmatOrbitConstants::KEP_ECC_TOL)
   {
      std::string errmsg = 
            "Error: Cannot convert to Equinoctial elements because the orbit is either parabolic or hyperbolic.\n";
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
            "Error: The state results in a singular conic section with radius of periapsis less than 1 m.\n";
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
   #ifdef DEBUG_STATE_CONVERTER_SQRT
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

Rvector6 StateConverter::EquinoctialToCartesian(const Rvector6& equinoctial, const Real& mu)
{
   Real sma    = equinoctial[0];   // semi major axis
   Real h      = equinoctial[1];   // projection of eccentricity vector onto y
   Real k      = equinoctial[2];   // projection of eccentricity vector onto x
   Real p      = equinoctial[3];   // projection of N onto y
   Real q      = equinoctial[4];   // projection of N onto x
   Real lambda = equinoctial[5]*RAD_PER_DEG;   // mean longitude

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
   } while (Abs(F-prevF) >= 1.0e-10);
//   } while (Abs(F-prevF) >= EQ_TOLERANCE);

   // Adjust true longitude to be between 0 and two-pi
   while (F < 0) F += TWO_PI;

   #ifdef DEBUG_STATE_CONVERTER_SQRT
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
      std::string errmsg = "Error converting from Equinoctial to Cartesian.  Message received from Utility is: ";
      errmsg += ue.GetFullMessage() + "\n";
      throw UtilityException(errmsg);
   }
   Real beta    = 1.0 / (1.0 + tmpSqrt);

   #ifdef DEBUG_STATE_CONVERTER_SQRT
      MessageInterface::ShowMessage("About to call Sqrt from EquinoctialToCartesian (2)\n");
      MessageInterface::ShowMessage("mu = %12.10f,  sma = %12.10f\n", mu, sma);
   #endif
   Real n    = Sqrt(mu/(sma * sma * sma));
   Real cosF = Cos(F);
   Real sinF = Sin(F);
   Real r    = sma * (1.0 - (k * cosF) - (h * sinF));

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

Rvector6 StateConverter::CartesianToSphericalAZFPA(const Rvector6& cartesian)
{
   // Calculate the magnitude of the position vector, right ascension, and declination
   Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
   Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
   Real    rMag   = pos.GetMagnitude();
   Real    lambda = ATan2(pos[1], pos[0]);
   Real    delta  = ASin(pos[2] / rMag);

   // Calculate magnitude of the velocity vector
   Real   vMag    = vel.GetMagnitude();

   // Calculate the vertical flight path angle
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
//         vMag, psi    * DEG_PER_RAD, alphaF * DEG_PER_RAD);
}

Rvector6 StateConverter::SphericalAZFPAToCartesian(const Rvector6& spherical)
{
   Real     rMag    = spherical[0]; // magnitude of the position vector
   Real     lambda  = spherical[1] * RAD_PER_DEG; // right ascension
   Real     delta   = spherical[2] * RAD_PER_DEG; // declination
   Real     vMag    = spherical[3]; // magnitude of the velocity vector
//   Real     psi     = spherical[4] * RAD_PER_DEG; // vertical flight path angle
//   Real     alphaF  = spherical[5] * RAD_PER_DEG; // flight path azimuth
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

Rvector6 StateConverter::CartesianToSphericalRADEC(const Rvector6& cartesian)
{
   // Calculate the magnitude of the position vector, right ascension, and declination
   Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
   Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
   Real    rMag   = pos.GetMagnitude();
   Real    lambda = ATan2(pos[1], pos[0]);
   Real    delta  = ASin(pos[2] / rMag);

   // Calculate magnitude of the velocity vector
   Real   vMag    = vel.GetMagnitude();

   // Compute right ascension of velocity
   Real   lambdaV = ATan2(vel[1], vel[0]);

   // Compute the declination of velocity
   Real   deltaV  = ASin(vel[2] / vMag);

   return Rvector6(rMag, lambda  * DEG_PER_RAD, delta  * DEG_PER_RAD,
                   vMag, lambdaV * DEG_PER_RAD, deltaV * DEG_PER_RAD);
}

Rvector6 StateConverter::SphericalRADECToCartesian(const Rvector6& spherical)
{
   Real     rMag    = spherical[0]; // magnitude of the position vector
   Real     lambda  = spherical[1] * RAD_PER_DEG; // right ascension
   Real     delta   = spherical[2] * RAD_PER_DEG; // declination
   Real     vMag    = spherical[3]; // magnitude of the velocity vector
   Real     lambdaV = spherical[4] * RAD_PER_DEG; // right ascension of velocity
   Real     deltaV  = spherical[5] * RAD_PER_DEG; // declination of velocity

   #ifdef DEBUG_STATE_CONVERTER
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

   #ifdef DEBUG_STATE_CONVERTER
      MessageInterface::ShowMessage(
            "Exiting SphericalRADECToCartesian and returning state %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
            pos[0],pos[1],pos[2],vel[0],vel[1],vel[2]);
   #endif
   return Rvector6(pos, vel);
}

