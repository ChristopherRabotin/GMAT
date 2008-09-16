//$Header$ 
//------------------------------------------------------------------------------
//                              StateConverter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
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
#include "Equinoctial.hpp"
#include "CoordUtil.hpp"
#include "Keplerian.hpp"
#include "ModKeplerian.hpp"
#include "Equinoctial.hpp"
#include "SphericalAZFPA.hpp"
#include "SphericalRADEC.hpp"
#include "MessageInterface.hpp"
#include "UtilityException.hpp"

//#define DEBUG_STATE_CONVERTER 1

//-------------------------------------
// static data
//-------------------------------------

const Real StateConverter::DEFAULT_MU = 0.3986004415e+06;
const std::string StateConverter::STATE_TYPE_TEXT[StateTypeCount] =
{
   "Cartesian",
   "Keplerian",
   "ModifiedKeplerian",
   "SphericalAZFPA",
   "SphericalRADEC",
   "Equinoctial",
};


//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  StateConverter()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 *
 */
StateConverter::StateConverter()
{
//    mStateType = "Cartesian";
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
//    mStateType = newType;
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
//    mStateType = newType;
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
//    mStateType = stateConverter.mStateType;
   mMu   = stateConverter.mMu; 
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
        
//    mStateType = converter.mStateType;
   mMu   = converter.mMu; 
   
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
   #if DEBUG_STATE_CONVERTER
      MessageInterface::ShowMessage("\nStateConverter::SetMu(cs) enters...\n");
   #endif
      
   // Check for empty coordinate system then stop process
   if (coordSys == NULL)
      return false;
   
   // Get the coordinate system's origin
   SpacePoint *origin = coordSys->GetOrigin();
   
   #if DEBUG_STATE_CONVERTER
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
   
   #if DEBUG_STATE_CONVERTER
      MessageInterface::ShowMessage("...mMu = %f before "
        "StateConverter::SetMu() exits\n\n", mMu);
   #endif
      
   return true;
}


// //---------------------------------------------------------------------------
// //  bool StateConverter::SetMu(const SolarSystem *solarSystem, 
// //                             const std::string &body)
// //---------------------------------------------------------------------------
// /**
//  * Set the mu from the Celestial body's gravitational constant
//  *
//  * @return true if successful; otherwise, return false
//  *
//  */
// //---------------------------------------------------------------------------
// bool StateConverter::SetMu(SolarSystem *solarSystem, 
//                            const std::string &body)
// {
//    if (solarSystem == NULL) return false;

//    CelestialBody *centralBody = solarSystem->GetBody(body);
//    if (centralBody == NULL) return false;

//    // Get the gravitational constant and set new value for mu
//    mMu = centralBody->GetGravitationalConstant();

//    return true;
// }


//---------------------------------------------------------------------------
// Rvector6 FromCartesian(const Rvector6 &state, const std::string &toType,
//                        const std::string &anomalyType = "TA")
//---------------------------------------------------------------------------
Rvector6 StateConverter::FromCartesian(const Rvector6 &state,
                                       const std::string &toType,
                                       const std::string &anomalyType)
{
   #if DEBUG_STATE_CONVERTER
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
   
   #if DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromCartesian() returning \n   %s\n",
       outState.ToString(13).c_str());
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
   #if DEBUG_STATE_CONVERTER
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
      outState = KeplerianToSphericalAZFPA(state, mMu, anomaly);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = KeplerianToSphericalRADEC(state, mMu, anomaly);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"Keperian\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   #if DEBUG_STATE_CONVERTER
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
   #if DEBUG_STATE_CONVERTER
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
      outState = KeplerianToSphericalAZFPA(keplerian, mMu, anomaly);
   }
   else if (toType == "SphericalRADEC")
   {
      Rvector6 keplerian = ModKeplerianToKeplerian(state);
      outState = KeplerianToSphericalRADEC(keplerian, mMu, anomaly);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"ModKeplerian\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   #if DEBUG_STATE_CONVERTER
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
   #if DEBUG_STATE_CONVERTER
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
      outState = SphericalAZFPAToKeplerian(state, mMu, anomaly);
   }
   else if (toType == "ModifiedKeplerian")
   {
      Rvector6 keplerian = SphericalAZFPAToKeplerian(state, mMu, anomaly);
      outState = KeplerianToModKeplerian(keplerian);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = AZFPA_To_RADECV(state);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"SphericalAZFPA\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   #if DEBUG_STATE_CONVERTER
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
   #if DEBUG_STATE_CONVERTER
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
      outState = SphericalRADECToKeplerian(state, mMu, anomaly);
   }
   else if (toType == "ModifiedKeplerian")
   {
      Rvector6 keplerian = SphericalRADECToKeplerian(state, mMu, anomaly);
      outState = KeplerianToModKeplerian(keplerian);
   }
   else if (toType == "SphericalAZFPA")
   {
      outState = RADECV_To_AZFPA(state);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"SphericalRADEC\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   #if DEBUG_STATE_CONVERTER
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
   #if DEBUG_STATE_CONVERTER
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

   #if DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::FromEquinoctial() returning \n   %s\n",
       outState.ToString(13).c_str());
   #endif
   
   return outState;
   
}


// //---------------------------------------------------------------------------
// //  Rvector6 Convert(const Rvector6 &state, const std::string &fromType,
// //                   const std::string &toType,
// //                   Anomaly::AnomalyType anomalyType = Anomaly::TA)
// //---------------------------------------------------------------------------
// /**
//  * Converts state from fromType to toType.
//  *
//  * @param <state> state to convert
//  * @param <fromType>  state type to convert from
//  * @param <toType> state type to convert to
//  * @param <anomalyType> anomaly type string if toType is Mod/Keplerian
//  *
//  * @return Converted states from the specific element type 
//  */
// //---------------------------------------------------------------------------
// Rvector6 StateConverter::Convert(const Rvector6 &state,   
//                                  const std::string &fromType,
//                                  const std::string &toType,
//                                  Anomaly::AnomalyType anomalyType)
// {
//    if (fromType == "Cartesian" && toType == "Keplerian")
//    {
//       return Keplerian::CartesianToKeplerian(mMu, state, anomalyType);
//    }
//    else
//    {
//       Anomaly tempAnomaly;
//       tempAnomaly.SetType(anomalyType);
//       Real tempState[6];
//       for (int i=0; i<6; i++)
//          tempState[i] = state.Get(i);
//       return Convert(tempState, fromType, toType, tempAnomaly);
//    }
// }


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
   #if DEBUG_STATE_CONVERTER
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

   #if DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
      ("StateConverter::Convert() returning \n   %s\n", outState.ToString(13).c_str());
   #endif
   
   return outState;
}


//---------------------------------------------------------------------------
//  Rvector6 Convert(const Rvector6 &state, const std::string &fromType,
//                   const std::string &toType, Anomaly &anomaly)
//---------------------------------------------------------------------------
/**
 * Assignment operator for StateConverter structures.
 *
 * @param <state> Element states 
 * @param <fromType>  Element Type 
 * @param <toType> Element Type
 *
 * @return Converted states from the specific element type 
 */
//---------------------------------------------------------------------------
Rvector6 StateConverter::Convert(const Rvector6 &state,   
                                 const std::string &fromType,
                                 const std::string &toType,
                                 Anomaly &anomaly)
{
   #if DEBUG_STATE_CONVERTER
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


// //---------------------------------------------------------------------------
// //  Rvector6 StateConverter::Convert(const Real *state,   
// //                                   const std::string &toType)
// //---------------------------------------------------------------------------
// /**
//  * Converts state from internal state type to requested state type
//  *
//  * @param <state>  input state
//  * @param <toType> state type to convert state to
//  *
//  * @return Converted states from the specific element type 
//  */
// //---------------------------------------------------------------------------
// Rvector6 StateConverter::Convert(const Real *state,
//                                  const std::string &toType)
// {
//    Anomaly tempAnomaly;
//    return Convert(state, mStateType, toType, tempAnomaly);
// }


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

