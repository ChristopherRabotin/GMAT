//$Id$
//------------------------------------------------------------------------------
//                                  OrbitData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/09
//
/**
 * Implements Orbit related data class.
 *   VelApoapsis, VelPeriapsis, Apoapsis, Periapsis, OrbitPeriod,
 *   RadApoapsis, RadPeriapais, C3Energy, Energy
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "OrbitData.hpp"
#include "ParameterException.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "Linear.hpp"
#include "Keplerian.hpp"         // for Cartesian to Keplerian elements
#include "AngleUtil.hpp"
//#include "SphericalRADEC.hpp"    // for friend CartesianToSphericalRADEC/AZFPA()
#include "ModKeplerian.hpp"      // for friend KeplerianToModKeplerian()
//#include "Equinoctial.hpp"
#include "CelestialBody.hpp"
#include "StringUtil.hpp"        // for ToString()
#include "MessageInterface.hpp"


//#define DEBUG_ORBITDATA_INIT
//#define DEBUG_ORBITDATA_CONVERT
//#define DEBUG_ORBITDATA_RUN
//#define DEBUG_CLONE
//#define DEBUG_MA
//#define DEBUG_HA
//#define DEBUG_ORBITDATA_OBJREF_EPOCH

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

CoordinateConverter OrbitData::mCoordConverter = CoordinateConverter();

const std::string
OrbitData::VALID_OBJECT_TYPE_LIST[OrbitDataObjectCount] =
{
   "Spacecraft",
   "SolarSystem",
   "CoordinateSystem",
   "SpacePoint"
};


const Real OrbitData::ORBIT_DATA_TOLERANCE     = 2.0e-10;

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// OrbitData(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
OrbitData::OrbitData(const std::string &name)
   : RefData(name),
   stateTypeId (-1)
{
   mCartState = Rvector6::RVECTOR6_UNDEFINED;
   mKepState = Rvector6::RVECTOR6_UNDEFINED;
   mModKepState = Rvector6::RVECTOR6_UNDEFINED;
   mSphRaDecState = Rvector6::RVECTOR6_UNDEFINED;
   mSphAzFpaState = Rvector6::RVECTOR6_UNDEFINED;
   mCartEpoch = 0.0;
   mGravConst = 0.0;
   
   mSpacecraft = NULL;
   mSolarSystem = NULL;
   mOrigin = NULL;
   mInternalCoordSystem = NULL;
   mOutCoordSystem = NULL;

   firstTimeEpochWarning = false;
}


//------------------------------------------------------------------------------
// OrbitData(const OrbitData &data)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <data> the OrbitData object being copied.
 */
//------------------------------------------------------------------------------
OrbitData::OrbitData(const OrbitData &data)
   : RefData(data),
   stateTypeId (data.stateTypeId)
{
   #ifdef DEBUG_CLONE
   MessageInterface::ShowMessage
      ("OrbitData::OrbitData copy constructor called\n");
   #endif
   
   mCartState = data.mCartState;
   mKepState = data.mKepState;
   mModKepState = data.mModKepState;
   mSphRaDecState = data.mSphRaDecState;
   mSphAzFpaState = data.mSphAzFpaState;
   mCartEpoch = data.mCartEpoch;
   mGravConst = data.mGravConst;
   
   mSpacecraft = data.mSpacecraft;
   mSolarSystem = data.mSolarSystem;
   mOrigin = data.mOrigin;
   mInternalCoordSystem = data.mInternalCoordSystem;
   mOutCoordSystem = data.mOutCoordSystem;

   firstTimeEpochWarning = data.firstTimeEpochWarning;
}


//------------------------------------------------------------------------------
// OrbitData& operator= (const OrbitData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
OrbitData& OrbitData::operator= (const OrbitData &right)
{
   if (this == &right)
      return *this;
   
   RefData::operator=(right);
   
   mCartState = right.mCartState;
   mKepState = right.mKepState;
   mModKepState = right.mModKepState;
   mSphRaDecState = right.mSphRaDecState;
   mSphAzFpaState = right.mSphAzFpaState;
   mCartEpoch = right.mCartEpoch;
   mGravConst = right.mGravConst;
   
   mSpacecraft = right.mSpacecraft;
   mSolarSystem = right.mSolarSystem;
   mOrigin = right.mOrigin;
   mInternalCoordSystem = right.mInternalCoordSystem;
   mOutCoordSystem = right.mOutCoordSystem;
   
   stateTypeId = right.stateTypeId;
   
   firstTimeEpochWarning = right.firstTimeEpochWarning;

   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
OrbitData::~OrbitData()
{
   #ifdef DEBUG_ORBITDATA_DESTRUCTOR
   MessageInterface::ShowMessage("OrbitData::~OrbitData()\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetReal(Integer item, Real rval)
//------------------------------------------------------------------------------
void OrbitData::SetReal(Integer item, Real rval)
{
   #if DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::SetReal() item=%d, rval=%f\n", item, rval);
   #endif
   
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   if (mSpacecraft == NULL)
   {
      MessageInterface::ShowMessage
         ("*** INTERNAL ERROR *** Cannot find Spacecraft object so returning %f\n",
          GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
   }
   
   CoordinateSystem *satCS =
      (CoordinateSystem*)mSpacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, "");
   
   #if DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("   Parameter CS Origin = '%s'\n", mOutCoordSystem->GetOriginName().c_str());
   MessageInterface::ShowMessage
      ("   Sat       CS Origin = '%s'\n", satCS->GetOriginName().c_str());
   #endif
   
   if (mOutCoordSystem != satCS)
   {
      ParameterException pe;
      pe.SetDetails("Currently GMAT cannot set %s; the spacecraft '%s' "
                    "requires values to be in the '%s' coordinate system (setting "
                    "values in different coordinate systems will be implemented in "
                    "future builds)",  mName.c_str(), mSpacecraft->GetName().c_str(),
                    satCS->GetName().c_str());
      throw pe;
   }
   
   
   switch (item)
   {
   case PX:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("X"), rval);
      break;
   case PY:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("Y"), rval);
      break;
   case PZ:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("Z"), rval);
      break;
   case VX:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("VX"), rval);
      break;
   case VY:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("VY"), rval);
      break;
   case VZ:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("VZ"), rval);
      break;
   default:
      throw ParameterException("OrbitData::SetReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// void SetRvector6(const Rvector6 &val)
//------------------------------------------------------------------------------
void OrbitData::SetRvector6(const Rvector6 &val)
{
   #if DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::SetRvector6() entered, rval=[%s]\n", rval.ToString().c_str());
   #endif
   
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   if (mSpacecraft == NULL)
   {
      MessageInterface::ShowMessage
         ("*** INTERNAL ERROR *** Cannot find Spacecraft object so returning %f\n",
          GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
   }
   
   mSpacecraft->SetState(val);
}


//------------------------------------------------------------------------------
// Rvector6 GetCartState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetCartState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   mCartEpoch = mSpacecraft->GetEpoch();
   mCartState.Set(mSpacecraft->GetState().GetState());
   
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetCartState() '%s' mCartState=\n   %s\n",
       mSpacecraft->GetName().c_str(), mCartState.ToString().c_str());
   #endif
   
   // if origin dependent parameter, the relative position/velocity is computed in
   // the parameter calculation, so just return prop state.
   if (mOriginDep)
      return mCartState;
   
   if (mInternalCoordSystem == NULL || mOutCoordSystem == NULL)
   {
      MessageInterface::ShowMessage
         ("OrbitData::GetCartState() Internal CoordSystem or Output CoordSystem is NULL.\n");
      
      throw ParameterException
         ("OrbitData::GetCartState() internal or output CoordinateSystem is NULL.\n");
   }
   
   //-----------------------------------------------------------------
   // convert to output CoordinateSystem
   //-----------------------------------------------------------------
   if (mInternalCoordSystem->GetName() != mOutCoordSystem->GetName())
   {
      #ifdef DEBUG_ORBITDATA_CONVERT
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() mOutCoordSystem:%s(%s), Axis addr=%d\n",
             mOutCoordSystem->GetName().c_str(),
             mOutCoordSystem->GetTypeName().c_str(),
             mOutCoordSystem->GetRefObject(Gmat::AXIS_SYSTEM, ""));
         if (mOutCoordSystem->AreAxesOfType("ObjectReferencedAxes"))
               MessageInterface::ShowMessage("OrbitData::GetCartState() <-- "
                     "mOutCoordSystem IS of type ObjectReferencedAxes!!!\n");
         else
            MessageInterface::ShowMessage("OrbitData::GetCartState() <-- "
                  "mOutCoordSystem IS NOT of type ObjectReferencedAxes!!!\n");
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() <-- Before convert: mCartEpoch=%f\n"
                  "state = %s\n", mCartEpoch, mCartState.ToString().c_str());
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() <-- firstTimeEpochWarning = %s\n",
             (firstTimeEpochWarning? "true" : "false"));

      #endif

      if ((mOutCoordSystem->AreAxesOfType("ObjectReferencedAxes")) && !firstTimeEpochWarning)
      {
         GmatBase *objRefOrigin = mOutCoordSystem->GetOrigin();
         if (objRefOrigin->IsOfType("Spacecraft"))
         {
            std::string objRefScName = ((Spacecraft*) objRefOrigin)->GetName();
            if (objRefScName != mSpacecraft->GetName())
            {
               // Get the epochs of the spacecraft to see if they are different
               Real scEpoch   = mSpacecraft->GetRealParameter("A1Epoch");
               Real origEpoch = ((Spacecraft*) objRefOrigin)->GetRealParameter("A1Epoch");
               #ifdef DEBUG_ORBITDATA_OBJREF_EPOCH
                  MessageInterface::ShowMessage("obj ref cs sc epoch = %12.10f\n", origEpoch);
                  MessageInterface::ShowMessage("   and the sc epoch = %12.10f\n", scEpoch);
               #endif
               if (!GmatMathUtil::IsEqual(scEpoch, origEpoch, ORBIT_DATA_TOLERANCE))
               {
                  std::string errmsg = "Warning:  In Coordinate System \"";
                  errmsg += mOutCoordSystem->GetName() + "\", \"";
                  errmsg += mSpacecraft->GetName() + "\" and \"";
                  errmsg += objRefScName + "\" have different epochs.\n";
//                  MessageInterface::PopupMessage(Gmat::WARNING_, errmsg);
                  MessageInterface::ShowMessage(errmsg);
                  firstTimeEpochWarning = true;
               }
            }
         }
      }
      
      try
      {
         #ifdef DEBUG_ORBITDATA_CONVERT
            MessageInterface::ShowMessage("    --> Converting from %s to %s\n\n",
                  mInternalCoordSystem->GetName().c_str(),
                  mOutCoordSystem->GetName().c_str());
         #endif
         mCoordConverter.Convert(A1Mjd(mCartEpoch), mCartState, mInternalCoordSystem,
                                 mCartState, mOutCoordSystem, true);
         #ifdef DEBUG_ORBITDATA_CONVERT
            MessageInterface::ShowMessage
               ("OrbitData::GetCartState() --> After  convert: mCartEpoch=%f\n"
                "state = %s\n", mCartEpoch, mCartState.ToString().c_str());
         #endif
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() Failed to convert to %s coordinate system.\n   %s\n",
             mOutCoordSystem->GetName().c_str(), e.GetFullMessage().c_str());
         std::string errmsg = "OrbitData::GetCartState() Failed to convert to " ;
         errmsg += mOutCoordSystem->GetName() + " coordinate system.\n";
         errmsg += "Message: " + e.GetFullMessage() + "\n";
         throw ParameterException(errmsg);
      }
   }
   
   return mCartState;
}


//------------------------------------------------------------------------------
// Rvector6 GetKepState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetKepState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();

   #ifdef DEBUG_ORBITDATA_STATE
   MessageInterface::ShowMessage("rbitData::GetKepState() from SC mKepState=%s\n",
                                  mKepState.ToString().c_str());
   #endif
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   mKepState = Keplerian::CartesianToKeplerian(mGravConst, state);

   #ifdef DEBUG_ORBITDATA_KEP_STATE
   MessageInterface::ShowMessage("OrbitData::GetKepState() mKepState=%s\n",
                                 mKepState.ToString().c_str());
   #endif
   
   return mKepState;
}


//------------------------------------------------------------------------------
// Rvector6 GetModKepState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetModKepState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
      
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Anomaly anomaly;
   Rvector6 kepState = CoordUtil::CartesianToKeplerian(state, mGravConst, anomaly);
   mModKepState = KeplerianToModKeplerian(kepState);
   
   return mModKepState;
}


//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetSphRaDecState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetSphRaDecState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
//   mSphRaDecState = CartesianToSphericalRADEC(state);
   mSphRaDecState = stateConverter.FromCartesian(state, "SphericalRADEC");

   #ifdef DEBUG_ORBITDATA_STATE
   MessageInterface::ShowMessage
      ("OrbitData::GetSphRaDecState() mSphRaDecState=\n   %s\n",
       mSphRaDecState.ToString().c_str());
   #endif
   
   return mSphRaDecState;
}


//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetSphAzFpaState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetSphAzFpaState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   //   mSphAzFpaState = CartesianToSphericalAZFPA(state);
   mSphAzFpaState = stateConverter.FromCartesian(state, "SphericalAZFPA");

   
   return mSphAzFpaState;
}


//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetEquinState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetEquinState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
//   Rvector6 mEquinState = CartesianToEquinoctial(state, mGravConst);
   stateConverter.SetMu(mGravConst);
   Rvector6 mEquinState = stateConverter.FromCartesian(state, "Equinoctial");
   
   return mEquinState;
}


//------------------------------------------------------------------------------
// Real GetCartReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Cartesian element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetCartReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetCartReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();
   if (item >= PX && item <= VZ)
      return mCartState[item];
   else
      throw ParameterException("OrbitData::GetCartReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
}


//------------------------------------------------------------------------------
// Real GetKepReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Keplerian element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetKepReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetKepReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();

   if (mOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < GmatOrbitConstants::KEP_ZERO_TOL)
      throw ParameterException
         ("OrbitData::GetKepReal(" + GmatRealUtil::ToString(item) +
          ") position vector is zero. pos: " + pos.ToString() + " vel: " +
          vel.ToString());
   
   switch (item)
   {
   case SMA:
      return Keplerian::CartesianToSMA(mGravConst, pos, vel);
   case ECC:
      return Keplerian::CartesianToECC(mGravConst, pos, vel);
   case INC:
      return Keplerian::CartesianToINC(mGravConst, pos, vel);
   case TA:
      return Keplerian::CartesianToTA(mGravConst, pos, vel);
   case EA:
      return Keplerian::CartesianToEA(mGravConst, pos, vel);
   case MA:
   {
      #ifdef DEBUG_MA
      MessageInterface::ShowMessage("In OrbitData, computing MA -------\n");
      #endif
      return Keplerian::CartesianToMA(mGravConst, pos, vel);
   }
   case HA:
   {
      #ifdef DEBUG_HA
      MessageInterface::ShowMessage("In OrbitData, computing HA -------\n");
      #endif
      return Keplerian::CartesianToHA(mGravConst, pos, vel);
   }
   case RAAN:
      return Keplerian::CartesianToRAAN(mGravConst, pos, vel);
   case RADN:
      {
         Real raanDeg = Keplerian::CartesianToRAAN(mGravConst, pos, vel);
         return AngleUtil::PutAngleInDegRange(raanDeg + 180, 0.0, 360.0);
      }
   case AOP:
      return Keplerian::CartesianToAOP(mGravConst, pos, vel);
      
   default:
      throw ParameterException("OrbitData::GetKepReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetOtherKepReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives other Keplerian element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetOtherKepReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetOtherKepReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();
   
   if (mOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);
   
   Real sma = Keplerian::CartesianToSMA(mGravConst, pos, vel);   
   Real ecc = Keplerian::CartesianToECC(mGravConst, pos, vel);
   
   if (GmatMathUtil::Abs(1.0 - ecc) <= GmatOrbitConstants::KEP_ECC_TOL)
   {
      throw ParameterException
         ("In OrbitData, Error in conversion to Keplerian state: "
          "The state results in an orbit that is nearly parabolic.\n");
   } 

  if (sma*(1 - ecc) < .001)
   {
      throw ParameterException
         ("In OrbitData, Error in conversion to Keplerian state: "
          "The state results in a singular conic section with radius of periapsis less than 1 m.\n");
   } 
   
   Real grav = mGravConst;
   
   switch (item)
   {
   case MM:
      if (ecc < (1.0 - GmatOrbitConstants::KEP_ECC_TOL))      // Ellipse
         return Sqrt(grav / (sma*sma*sma));
      else if (ecc > (1.0 + GmatOrbitConstants::KEP_ECC_TOL)) // Hyperbola 
         return Sqrt(-(grav / (sma*sma*sma)));
      else                         
         return 2.0 * Sqrt(grav); // Parabola
   case VEL_APOAPSIS:
      if ( (ecc < 1.0 - GmatOrbitConstants::KEP_ECC_TOL) || (ecc > 1.0 + GmatOrbitConstants::KEP_ECC_TOL))  //Ellipse and Hyperbola
         return Sqrt( (grav/sma)*((1-ecc)/(1+ecc)) );  
      else
         return 0.0; // Parabola  
   case VEL_PERIAPSIS:
      return Sqrt( (grav/sma)*((1+ecc)/(1-ecc)) );
   case ORBIT_PERIOD:
      if (sma < 0.0)
         return 0.0;
      else
         return GmatMathConstants::TWO_PI * Sqrt((sma * sma * sma)/ grav);
   case RAD_APOAPSIS:
           if ( (ecc < 1.0 - GmatOrbitConstants::KEP_ECC_TOL) || (ecc > 1.0 + GmatOrbitConstants::KEP_ECC_TOL)) //Ellipse and Hyperbola
         return sma * (1.0 + ecc);
          else
                 return 0.0;   // Parabola
   case RAD_PERIAPSIS:
      return sma * (1.0 - ecc);
   case C3_ENERGY:
      return -grav / sma;
   case ENERGY:
      return -grav / (2.0 * sma);
   default:
      throw ParameterException
         ("OrbitData::GetOtherKepReal() Unknown parameter ID: " +
          GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetSphRaDecReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Spherical element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetSphRaDecReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetSphRaDecReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetSphRaDecState();

   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetSphRaDecReal() item=%d state=%s\n",
       item, state.ToString().c_str());
   #endif
   
   switch (item)
   {
   case RD_RMAG:
      {
         // if orgin is "Earth" just return default
         if (mOrigin->GetName() == "Earth")
            return mSphRaDecState[RD_RMAG];
         else
            return GetPositionMagnitude(mOrigin);
      }
   case RD_RRA:
   case RD_RDEC:
   case RD_VMAG:
   case RD_RAV:
   case RD_DECV:
      return mSphRaDecState[item];
   default:
      throw ParameterException
         ("OrbitData::GetSphRaDecReal() Unknown parameter name: " +
          GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetSphAzFpaReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Spherical element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetSphAzFpaReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetSphAzFpaReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetSphAzFpaState();

   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetSphAzFpaReal() item=%s state=%s\n",
       item, state.ToString().c_str());
   #endif

   if (item >= AF_RMAG && item <= AF_FPA)
      return mSphAzFpaState[item];
   else
   {
      throw ParameterException("OrbitData::GetSphAzFpaReal() Unknown parameter ID: " +
                               GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetAngularReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Computes angular related parameter.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetAngularReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetAngularReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();

   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetAngularReal() item=%d state=%s\n",
       item, state.ToString().c_str());
   #endif
   
   Rvector3 pos;
   Rvector3 vel;
   
   pos = Rvector3(state[0], state[1], state[2]);
   vel = Rvector3(state[3], state[4], state[5]);
   
   Rvector3 hVec3 = Cross(pos, vel);
   Real     h     = Sqrt(hVec3 * hVec3);
   
   Real grav = mGravConst;

   switch (item)
   {
   case SEMILATUS_RECTUM:
      {
         if (mOrigin->GetName() != "Earth")
         {
            state = GetRelativeCartState(mOrigin);
            pos = Rvector3(state[0], state[1], state[2]);
            vel = Rvector3(state[3], state[4], state[5]);
            hVec3 = Cross(pos, vel);
            h = Sqrt(hVec3 * hVec3);
         }
         
         if (h < GmatOrbitConstants::KEP_TOL)
            return 0.0;
         else
            return (h / grav) * h;      // B M W; eq. 1.6-1
      }
   case HMAG:
      {
         if (mOrigin->GetName() != "Earth")
         {
            state = GetRelativeCartState(mOrigin);
            pos = Rvector3(state[0], state[1], state[2]);
            vel = Rvector3(state[3], state[4], state[5]);
            hVec3 = Cross(pos, vel);
            h = Sqrt(hVec3 * hVec3);
         }
         
         return h; 
      }
   case HX:
      return hVec3[0];
   case HY:
      return hVec3[1];
   case HZ:
      return hVec3[2];
   default:
      throw ParameterException
         ("OrbitData::GetAngularReal() Unknown parameter ID: " +
          GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetOtherAngleReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Computes other angle related parameters.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetOtherAngleReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetOtherAngleReal() item=%d\n", item);
   #endif
   
   Rvector6 state;
   if (mOrigin->GetName() != "Earth")
      state = GetRelativeCartState(mOrigin);
   else
      state = GetCartState();
   Rvector3 pos = Rvector3(state[0], state[1], state[2]);
   Rvector3 vel = Rvector3(state[3], state[4], state[5]);

   switch (item)
   {
   case BETA_ANGLE:
      {
         // compute orbit normal unit vector
         Rvector3 hVec3 = Cross(pos, vel);
         hVec3.Normalize();
         
         // compute sun unit vector from the origin
         Rvector3 sunPos = (mSolarSystem->GetBody(SolarSystem::SUN_NAME))->
            GetMJ2000Position(mCartEpoch);
         Rvector3 originPos = mOrigin->GetMJ2000Position(mCartEpoch);
         Rvector3 originToSun = sunPos - originPos;
         originToSun.Normalize();
         
         // Math Spec change: eq. 4.99 on 2006/12/11
         //Real betaAngle = ACos(hVec3*originToSun) * DEG_PER_RAD;
         Real betaAngle = ASin(hVec3*originToSun) * GmatMathConstants::DEG_PER_RAD;
         return betaAngle;
      }
   case HYPERBOLIC_RLA:
   case HYPERBOLIC_DLA:
      {
         // Compute the eccentricity vector
         Real     r     = pos.GetMagnitude();
         Real     v     = vel.GetMagnitude();
         Rvector3 e     = ((((v * v) - mGravConst / r) * pos) - (pos * vel) *vel) / mGravConst;
         Real     ecc   = e.GetMagnitude();
         if (Abs(ecc) < 1.0 + GmatOrbitConstants::KEP_ECC_TOL)
            return GmatMathConstants::QUIET_NAN;

         // Compute orbit normal unit vector
         Rvector3 hVec3 = Cross(pos, vel);
         Real     h     = hVec3.GetMagnitude();

         // Compute C3
         Real     C3    = v * v - (2.0 * mGravConst) / r;
         Real     s_1   = 1.0 / (1.0 + C3 * (h / mGravConst) * (h / mGravConst));
         Rvector3 s     = s_1 * ((Sqrt(C3) / mGravConst) * Cross(hVec3, e) - e);
         if (item == HYPERBOLIC_RLA)
            return ATan2(s[0], s[1]) * GmatMathConstants::DEG_PER_RAD;
         else // DLA
            return ASin(s[2]) * GmatMathConstants::DEG_PER_RAD;
      }
   default:
      throw ParameterException
         ("OrbitData::GetOtherAngleReal() Unknown parameter ID: " +
          GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetEquiReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves Equinoctial element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetEquinReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetEquinReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();

   if (mOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < GmatOrbitConstants::KEP_ZERO_TOL)
      throw ParameterException
         ("OrbitData::GetEquiReal(" + GmatRealUtil::ToString(item) +
          ") position vector is zero. pos: " + pos.ToString() + " vel: " +
          vel.ToString());
   
   switch (item)
   {
   case EQ_SMA:
      return Keplerian::CartesianToSMA(mGravConst, pos, vel);
   case EY:
      {
         Rvector6 equiState = GetEquinState();
         return equiState[1];
      }
   case EX:
      {
         Rvector6 equiState = GetEquinState();
         return equiState[2];
      }
   case NY:
      {
         Rvector6 equiState = GetEquinState();
         return equiState[3];
      }
   case NX:
      {
         Rvector6 equiState = GetEquinState();
         return equiState[4];
      }
   case MLONG:
      {
         Rvector6 equiState = GetEquinState();
         return equiState[5];
      }
      
   default:
      throw ParameterException("OrbitData::GetEquinReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// const Rmatrix66& GetStmRmat66(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Spacecraft Rmatrix66 data.
 */
//------------------------------------------------------------------------------
const Rmatrix66& OrbitData::GetStmRmat66(Integer item)
{
   #ifdef DEBUG_SCDATA_GET
   MessageInterface::ShowMessage
      ("OrbitData::GetStmRmat66() entered, mSpacecraft=<%p>'%s'\n",
       mSpacecraft, mSpacecraft ? mSpacecraft->GetName().c_str() : "NULL");
   #endif
   
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   switch (item)
   {
   case ORBIT_STM:
      {
         mSTM = mSpacecraft->GetRmatrixParameter("OrbitSTM");
         return mSTM;
      }
   default:
      // otherwise, there is an error   
      throw ParameterException
         ("OrbitData::GetStmRmat66() Unknown parameter id: " +
          GmatStringUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// const Rmatrix33& GetStmRmat33(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Spacecraft Rmatrix33 data.
 */
//------------------------------------------------------------------------------
const Rmatrix33& OrbitData::GetStmRmat33(Integer item)
{
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   mSTM = mSpacecraft->GetRmatrixParameter("OrbitSTM");
   
   switch (item)
   {
   case ORBIT_STM_A:
      mSTMSubset = mSTM.UpperLeft();
      return mSTMSubset;
   case ORBIT_STM_B:
      mSTMSubset = mSTM.UpperRight();
      return mSTMSubset;
   case ORBIT_STM_C:
      mSTMSubset = mSTM.LowerLeft();
      return mSTMSubset;
   case ORBIT_STM_D:
      mSTMSubset = mSTM.LowerRight();
      return mSTMSubset;
   default:
      // otherwise, there is an error   
      throw ParameterException
         ("OrbitData::GetStmRmat33() Unknown parameter id: " +
          GmatStringUtil::ToString(item));
   }
}

//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* OrbitData::GetValidObjectList() const
{
   return VALID_OBJECT_TYPE_LIST;
}


//------------------------------------------------------------------------------
// bool ValidateRefObjects(GmatBase *param)
//------------------------------------------------------------------------------
/**
 * Validates reference objects for given parameter
 */
//------------------------------------------------------------------------------
bool OrbitData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<OrbitDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == OrbitDataObjectCount)
      return true;
   else
      return false;
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystem()
//------------------------------------------------------------------------------
SolarSystem* OrbitData::GetSolarSystem()
{
   return mSolarSystem;
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordSys()
//------------------------------------------------------------------------------
CoordinateSystem* OrbitData::GetInternalCoordSys()
{
   return mInternalCoordSystem;
}


//------------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/*
 * @param <cs> internal coordinate system what parameter data is representing.
 */
//------------------------------------------------------------------------------ 
void OrbitData::SetInternalCoordSys(CoordinateSystem *cs)
{
   mInternalCoordSystem = cs;
}



//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetRelativeCartState(SpacePoint *origin)
//------------------------------------------------------------------------------
/**
 * Computes spacecraft cartesian state from the given origin.
 *
 * @param <origin> origin pointer
 *
 * @return spacecraft state from the given origin.
 */
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetRelativeCartState(SpacePoint *origin)
{
   // get spacecraft state
   Rvector6 scState = GetCartState();
   
   // get origin state
   Rvector6 originState = origin->GetMJ2000State(mCartEpoch);
   
   #ifdef DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage
         ("OrbitData::GetRelativeCartState() origin=%s, state=%s\n",
          origin->GetName().c_str(), originState.ToString().c_str());
   #endif
      
   // return relative state
   return scState - originState;
}


//------------------------------------------------------------------------------
// Real OrbitData::GetPositionMagnitude(SpacePoint *origin)
//------------------------------------------------------------------------------
/**
 * Computes position magnitude from the given origin.
 *
 * @param <origin> origin pointer
 *
 * @return position magnitude from the given origin.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetPositionMagnitude(SpacePoint *origin)
{
   // get spacecraft position
   Rvector6 scState = GetCartState();
   Rvector3 scPos = Rvector3(scState[0], scState[1], scState[2]);

   // get origin position
   Rvector6 originState = origin->GetMJ2000State(mCartEpoch);
   Rvector3 originPos = Rvector3(originState[0], originState[1], originState[2]);

   // get relative position magnitude
   Rvector3 relPos = scPos - originPos;
   
   #ifdef DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage
         ("OrbitData::GetPositionMagnitude() scPos=%s, originPos=%s, relPos=%s\n",
          scPos.ToString().c_str(), originPos.ToString().c_str(),
          relPos.ToString().c_str());
   #endif
   
   return relPos.GetMagnitude();
}


// The inherited methods from RefData
//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void OrbitData::InitializeRefObjects()
{
   #ifdef DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage("OrbitData::InitializeRefObjects() entered.\n");
   #endif
   
   mSpacecraft =
      (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   
   if (mSpacecraft == NULL)
   {
      #ifdef DEBUG_ORBITDATA_INIT
      MessageInterface::ShowMessage
         ("OrbitData::InitializeRefObjects() Cannot find spacecraft: " +
          GetRefObjectName(Gmat::SPACECRAFT) + ".\n" +
          "Make sure Spacecraft is set to any internal parameters.\n");
      #endif
      
      throw ParameterException
         ("Cannot find spacecraft: " + GetRefObjectName(Gmat::SPACECRAFT));
   }
   
   if (stateTypeId == -1)
      stateTypeId = mSpacecraft->GetParameterID("DisplayStateType");
   
   mSolarSystem =
      (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   
   if (mSolarSystem == NULL)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() Cannot find SolarSystem object\n");
   
   if (mInternalCoordSystem == NULL)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() Cannot find internal "
          "CoordinateSystem object\n");
   
   //-----------------------------------------------------------------
   // if dependent body name exist and it is a CelestialBody,
   // it is origin dependent parameter, set new gravity constant.
   //-----------------------------------------------------------------
   std::string originName =
      FindFirstObjectName(GmatBase::GetObjectType(VALID_OBJECT_TYPE_LIST[SPACE_POINT]));
   
   mOriginDep = false;
   
   if (originName != "")
   {
      #ifdef DEBUG_ORBITDATA_INIT
      MessageInterface::ShowMessage
         ("OrbitData::InitializeRefObjects() getting originName: %s pointer.\n",
          originName.c_str());
      #endif
      
      mOrigin =
         (SpacePoint*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACE_POINT]);
      
      if (!mOrigin)
         throw InvalidDependencyException
            (" is a central body dependent parameter.");
      
      // override gravity constant if origin is CelestialBody
      if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
         mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();

      mOriginDep = true;
   }
   //-----------------------------------------------------------------
   // It is CoordinateSystem dependent parameter.
   // Set Origin to CoordinateSystem origin and mu to origin mu.
   //-----------------------------------------------------------------
   else
   {
      mOutCoordSystem =
         (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
      
      if (mOutCoordSystem == NULL)
      {
         #ifdef DEBUG_ORBITDATA_INIT
         MessageInterface::ShowMessage
            ("OrbitData::InitializeRefObjects() Cannot find output "
             "CoordinateSystem object\n");
         #endif
         
         throw ParameterException
            ("OrbitData::InitializeRefObjects() Cannot find coordinate system.\n");
      }
      
      
      // Set origin to out coordinate system origin for CoordinateSystem
      // dependent parameter
      mOrigin = mOutCoordSystem->GetOrigin();
      
      if (!mOrigin)
      {
         #ifdef DEBUG_ORBITDATA_INIT
         MessageInterface::ShowMessage
            ("OrbitData::InitializeRefObjects() origin not found: " +
             mOutCoordSystem->GetOriginName() + "\n");
         #endif
      
         throw ParameterException
            ("OrbitData::InitializeRefObjects() The origin of CoordinateSystem \"" +
             mOutCoordSystem->GetOriginName() + "\" is NULL");
      }
      
      // get gravity constant if out coord system origin is CelestialBody
      if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
         mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();
   }
   
   #ifdef DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage
      ("OrbitData::InitializeRefObjects() exiting, mOrigin.Name=%s, mGravConst=%f, "
       "mOriginDep=%d\n",  mOrigin->GetName().c_str(), mGravConst, mOriginDep);
   MessageInterface::ShowMessage
      ("   mSpacecraft=<%p> '%s', mSolarSystem=<%p>, mOutCoordSystem=<%p>, mOrigin=<%p>\n",
       mSpacecraft, mSpacecraft->GetName().c_str(),mSolarSystem, mOutCoordSystem, mOrigin);
   #endif
}


//------------------------------------------------------------------------------
// virtual bool IsValidObjectType(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Checks reference object type.
 *
 * @return return true if object is valid object, false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitData::IsValidObjectType(Gmat::ObjectType type)
{
   for (int i=0; i<OrbitDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;

}


