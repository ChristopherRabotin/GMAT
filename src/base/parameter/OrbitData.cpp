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
#include "CalculationUtilities.hpp"
#include "GmatConstants.hpp"
#include "Linear.hpp"
#include "AngleUtil.hpp"
#include "CelestialBody.hpp"
#include "StringUtil.hpp"        // for ToString()
#include "MessageInterface.hpp"
#include "StateConversionUtil.hpp"


//#define DEBUG_ORBITDATA_SET
//#define DEBUG_ORBITDATA_INIT
//#define DEBUG_ORBITDATA_CONVERT
//#define DEBUG_ORBITDATA_RUN
//#define DEBUG_CLONE
//#define DEBUG_MA
//#define DEBUG_HA
//#define DEBUG_ORBITDATA_OBJREF_EPOCH
//#define DEBUG_ORBITDATA_OBJNAME
//#define DEBUG_BROUWER_LONG

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

const std::string OrbitData::VALID_ANGLE_PARAM_NAMES[HYPERBOLIC_DLA - SEMILATUS_RECTUM + 1] =
{
   "SemilatusRectum",
   "HMag",
   "HX",
   "HY",
   "HZ",
   "BetaAngle",
   "RLA",
   "DLA"
};

const std::string OrbitData::VALID_OTHER_ORBIT_PARAM_NAMES[ENERGY - MM + 1] =
{
   "MeanMotion",
   "VelApoapsis",
   "VelPeriapsis",
   "OrbitPeriod",
   "C3Energy",
   "Energy"
};


//---------------------------------
// public methods
//---------------------------------

//LOJ: NEW_PARAMETER: Added paramOwnerType to use in InitializeRefObjects()
//------------------------------------------------------------------------------
// OrbitData(const std::string &name = "", const Gmat::ObjectType paramOwnerType = Gmat::SPACECRAFT)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
OrbitData::OrbitData(const std::string &name, Gmat::ObjectType paramOwnerType,
                     GmatParam::DepObject depObj, bool isSettable)
   : RefData(name, paramOwnerType, depObj, isSettable),
   stateTypeId (-1)
{
   mCartEpoch     = 0.0;
   mGravConst     = 0.0;
   
   mSpacecraft    = NULL;
   mSpacePoint    = NULL;
   mSolarSystem   = NULL;
   mOrigin        = NULL;
   mInternalCS    = NULL;
   mParameterCS   = NULL;
   
   mIsParamOriginDep     = false;
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
   
   mCartEpoch     = data.mCartEpoch;
   mGravConst     = data.mGravConst;
   mFlattening    = data.mFlattening;
   mEqRadius      = data.mEqRadius;
   
   mSpacecraft = data.mSpacecraft;
   mSpacePoint = data.mSpacePoint;
   mSolarSystem = data.mSolarSystem;
   mOrigin = data.mOrigin;
   mInternalCS = data.mInternalCS;
   mParameterCS = data.mParameterCS;

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
   
   mCartEpoch     = right.mCartEpoch;
   mGravConst     = right.mGravConst;
   mFlattening    = right.mFlattening;
   mEqRadius      = right.mEqRadius;
   
   mSpacecraft = right.mSpacecraft;
   mSpacePoint = right.mSpacePoint;
   mSolarSystem = right.mSolarSystem;
   mOrigin = right.mOrigin;
   mInternalCS = right.mInternalCS;
   mParameterCS = right.mParameterCS;
   
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
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("\nOrbitData::SetReal() '%s' entered, item=%d, rval=%f\n", mActualParamName.c_str(), item, rval);
   #endif
   
   if (mSpacePoint == NULL)
      InitializeRefObjects();
   
   // Check for null SpacePoint pointer again
   if (mSpacePoint == NULL)
   {      
      if (mSpacePoint == NULL)
         throw ParameterException
            ("OrbitData::SetReal() Cannot set Parameter " + GmatRealUtil::ToString(item) +
             ". SpacePoint object is NULL\n");
   }
   
   CoordinateSystem *paramOwnerCS = NULL;
   
   if ((mSpacePoint->IsOfType(Gmat::SPACECRAFT)) ||
       (mSpacePoint->IsOfType("Spacecraft")))
   {
      mSpacecraft = (Spacecraft*)mSpacePoint;
      paramOwnerCS = 
         (CoordinateSystem*)mSpacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, "");
   }
   else
   {
      paramOwnerCS = mInternalCS;
   }
   
   #ifdef DEBUG_ORBITDATA_SET
   DebugOutputData(paramOwnerCS);
   #endif
   
   bool convertOriginBeforeSet = false;
   
   // Check if origin is different from spacecraft CS origin
   if (mIsParamOriginDep && mOrigin)
   {
      // Now allow setting value to origin dependent Parameter
      // that has different origin than Spacecraft CS origin (GMT-4020 Fix)
      if (paramOwnerCS->GetOriginName() != mOrigin->GetName())
      {
         if (mSpacecraft != NULL && mIsParamSettable) 
         {
            if (mParamDepObj == GmatParam::ORIGIN)
               convertOriginBeforeSet = true;
         }
         else
         {
            // Should not get here, so throw a exception
            ParameterException pe;
            pe.SetDetails("The Parameter %s is not settable)", mActualParamName.c_str());
            throw pe;
         }
         #if 0
         ParameterException pe;
         pe.SetDetails("Currently GMAT cannot set %s; the spacecraft '%s' "
                       "requires values to be in the '%s' origin (setting "
                       "values in different origin will be implemented in "
                       "future builds)",  mActualParamName.c_str(),
                       mSpacecraft->GetName().c_str(), paramOwnerCS->GetOriginName().c_str());
         throw pe;
         #endif
      }
   }
   
   bool convertCSBeforeSet = false;
   
   // Check for different coordinate system for Spacecraft owend Parameter(2013.03.28)
   if (!mIsParamOriginDep && mParameterCS != NULL && paramOwnerCS != NULL)
   {
      if (mParameterCS->GetName() != paramOwnerCS->GetName())
      {
         // Now allow setting value to coordinate system dependent Parameter
         // that has different coordinate system than Spacecraft (GMT-4020 Fix)
         if (mSpacecraft != NULL && mIsParamSettable) 
         {
            if (mParamDepObj == GmatParam::COORD_SYS)
               convertCSBeforeSet = true;
         }
         else
         {
            // Should not get here, so throw a exception
            ParameterException pe;
            pe.SetDetails("The Parameter %s is not settable)", mActualParamName.c_str());
            throw pe;
         }
         
         // Old code should be removed
         #if 0
         ParameterException pe;
         pe.SetDetails("Currently GMAT cannot set %s; the spacecraft '%s' "
                       "requires values to be in the '%s' coordinate system (setting "
                       "values in different coordinate systems will be implemented in "
                       "future builds)",  mActualParamName.c_str(),
                       mSpacecraft->GetName().c_str(), paramOwnerCS->GetName().c_str());
         throw pe;
         #endif
      }
   }
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("   convertOriginBeforeSet=%d, convertCSBeforeSet=%d\n", convertOriginBeforeSet, convertCSBeforeSet);
   #endif
   
   //=================================================================
   // Convert state to Parameter Origin or CS first before set value (LOJ: GMT-4020 fix)
   //=================================================================
   if (convertOriginBeforeSet)
   {
      Rvector6 cartStateInParamOrigin = GetCartStateInParameterOrigin(item, rval);
      Rvector6 cartStateNew = cartStateInParamOrigin + mOrigin->GetMJ2000State(mCartEpoch);
      
      // Need to set whole state in internal CS
      mSpacecraft->SetState(cartStateNew);
      
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("OrbitData::SetReal() leaving, item set to non-ParameterOwner Origin, item=%d, rval=%f\n", item, rval);
      #endif
      return;
   }
   else if (convertCSBeforeSet)
   {
      Rvector6 cartStateInParamCS = GetCartStateInParameterCS(item, rval);
      Real currEpoch = mSpacePoint->GetEpoch();
      Rvector6 cartStateNew;
      
      // Convert to internal CS
      mCoordConverter.Convert(A1Mjd(currEpoch), cartStateInParamCS, mParameterCS,
                              cartStateNew, mInternalCS, false);
      
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   from paramCS to internalCS = \n   %s", cartStateNew.ToString().c_str());
      #endif
      
      // Need to set whole state in internal CS
      mSpacecraft->SetState(cartStateNew);
      
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("OrbitData::SetReal() leaving, item set to non-ParameterOwner CS, item=%d, rval=%f\n", item, rval);
      #endif
      return;
   }
   //=================================================================
   else
   {
      // Do the direct setting
      SetRealParameters(item, rval);
   }
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::SetReal() '%s' leaving, item=%d, rval=%f\n", mActualParamName.c_str(), item, rval);
   #endif
}


//------------------------------------------------------------------------------
// void SetRvector6(const Rvector6 &val)
//------------------------------------------------------------------------------
void OrbitData::SetRvector6(const Rvector6 &val)
{
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::SetRvector6() entered, rval=[%s]\n", val.ToString().c_str());
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
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::SetRvector6() leaving, rval=[%s]\n", val.ToString().c_str());
   #endif
}


//------------------------------------------------------------------------------
// Rvector6 GetCartState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetCartState()
{
   //LOJ: NEW_PARAMETER: Change mSpacecraft to mSpacePoint here
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("--- is mSpacePoint NULL?  %s\n",
         (mSpacePoint? "NO" : "YES, it's NULL"));
   MessageInterface::ShowMessage("--- is mSpacecraft NULL?  %s\n",
         (mSpacecraft? "NO" : "YES, it's NULL"));
   MessageInterface::ShowMessage("--- is mSolarSystem NULL?  %s\n",
         (mSolarSystem? "NO" : "YES, it's NULL"));
   MessageInterface::ShowMessage("OrbitData::GetCartState() for '%s'\n",
       mSpacePoint->GetName().c_str());
   MessageInterface::ShowMessage("Is it a Spacecraft?  %s\n",
         (mSpacePoint->IsOfType("Spacecraft")? "YES!" : "no"));
   MessageInterface::ShowMessage
      ("In GetCartState, mParamOwnerType = %d, mParamTypeName=%s, mParamOwnerName=%s, mParamDepName=%s\n",
       mParamOwnerType, mParamTypeName.c_str(), mParamOwnerName.c_str(), mParamDepName.c_str());
   #endif
   
   if (mSolarSystem == NULL || mSpacePoint == NULL ||
      (mSpacePoint->IsOfType("Spacecraft") && mSpacecraft == NULL))
      InitializeRefObjects();
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("InitializeRefObjects done ...\n");
   MessageInterface::ShowMessage("--- is mSpacePoint NULL now?  %s\n",
         (mSpacePoint? "NO" : "YES, it's NULL"));
   MessageInterface::ShowMessage("--- is mSpacecraft NULL now?  %s\n",
         (mSpacecraft? "NO" : "YES, it's NULL"));
   MessageInterface::ShowMessage("--- is mSolarSystem NULL now?  %s\n",
         (mSolarSystem? "NO" : "YES, it's NULL"));
   MessageInterface::ShowMessage("OrbitData::GetCartState() for '%s'\n",
       mSpacePoint->GetName().c_str());
   MessageInterface::ShowMessage("Is it a Spacecraft now?  %s\n",
         (mSpacePoint->IsOfType("Spacecraft")? "YES!" : "no"));
   #endif
   
   mCartEpoch = mSpacePoint->GetEpoch();
   Rvector6 lastCartState = mSpacePoint->GetLastState();
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("----- epoch from spacepoint is %le\n", mCartEpoch);
   MessageInterface::ShowMessage
      ("----- state from spacepoint is %s\n", lastCartState.ToString().c_str());
   #endif
   
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetCartState() '%s' lastCartState=\n   %s\n",
       mSpacePoint->GetName().c_str(), lastCartState.ToString().c_str());
   MessageInterface::ShowMessage("   mInternalCS is %s, mParameterCS is %s\n",
         (mInternalCS? "NOT NULL" : "NULL"),  (mParameterCS? "NOT NULL" : "NULL"));
   MessageInterface::ShowMessage("   mInternalCS = <%p>, mOutCS = <%p>\n",
         mInternalCS, mParameterCS);
   #endif
   
   // if origin dependent parameter, the relative position/velocity is computed in
   // the parameter calculation, so just return prop state.
   if (mIsParamOriginDep)
      return lastCartState;
   
   if (mInternalCS == NULL || mParameterCS == NULL)
   {
      MessageInterface::ShowMessage
         ("OrbitData::GetCartState() Internal CoordSystem or Output CoordSystem is NULL.\n");
      
      throw ParameterException
         ("OrbitData::GetCartState() internal or output CoordinateSystem is NULL.\n");
   }
   
   //-----------------------------------------------------------------
   // convert to output CoordinateSystem
   //-----------------------------------------------------------------
   if (mInternalCS->GetName() != mParameterCS->GetName())
   {
      #ifdef DEBUG_ORBITDATA_CONVERT
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() mParameterCS:%s(%s), Axis addr=%d\n",
             mParameterCS->GetName().c_str(),
             mParameterCS->GetTypeName().c_str(),
             mParameterCS->GetRefObject(Gmat::AXIS_SYSTEM, ""));
         if (mParameterCS->AreAxesOfType("ObjectReferencedAxes"))
               MessageInterface::ShowMessage("OrbitData::GetCartState() <-- "
                     "mParameterCS IS of type ObjectReferencedAxes!!!\n");
         else
            MessageInterface::ShowMessage("OrbitData::GetCartState() <-- "
                  "mParameterCS IS NOT of type ObjectReferencedAxes!!!\n");
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() <-- Before convert: mCartEpoch=%f\n"
                  "state = %s\n", mCartEpoch, lastCartState.ToString().c_str());
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() <-- firstTimeEpochWarning = %s\n",
             (firstTimeEpochWarning? "true" : "false"));

      #endif

      if ((mParameterCS->AreAxesOfType("ObjectReferencedAxes")) && !firstTimeEpochWarning)
      {
         GmatBase *objRefOrigin = mParameterCS->GetOrigin();
         if (objRefOrigin->IsOfType("Spacecraft"))
         {
            std::string objRefScName = ((Spacecraft*) objRefOrigin)->GetName();
            if (objRefScName != mSpacePoint->GetName())
            {
               // Get the epochs of the spacecraft to see if they are different
               Real scEpoch   = mSpacePoint->GetRealParameter("A1Epoch");
               Real origEpoch = ((Spacecraft*) objRefOrigin)->GetRealParameter("A1Epoch");
               #ifdef DEBUG_ORBITDATA_OBJREF_EPOCH
                  MessageInterface::ShowMessage("obj ref cs sc epoch = %12.10f\n", origEpoch);
                  MessageInterface::ShowMessage("   and the sc epoch = %12.10f\n", scEpoch);
               #endif
               if (!GmatMathUtil::IsEqual(scEpoch, origEpoch, ORBIT_DATA_TOLERANCE))
               {
                  std::string errmsg = "Warning:  In Coordinate System \"";
                  errmsg += mParameterCS->GetName() + "\", \"";
                  errmsg += mSpacePoint->GetName() + "\" and \"";
                  errmsg += objRefScName + "\" have different epochs.\n";
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
                  mInternalCS->GetName().c_str(),
                  mParameterCS->GetName().c_str());
         #endif
         mCoordConverter.Convert(A1Mjd(mCartEpoch), lastCartState, mInternalCS,
                                 lastCartState, mParameterCS, true);
         #ifdef DEBUG_ORBITDATA_CONVERT
            MessageInterface::ShowMessage
               ("OrbitData::GetCartState() --> After  convert: mCartEpoch=%f\n"
                "state = %s\n", mCartEpoch, lastCartState.ToString().c_str());
         #endif
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() Failed to convert to %s coordinate system.\n   %s\n",
             mParameterCS->GetName().c_str(), e.GetFullMessage().c_str());
         std::string errmsg = "OrbitData::GetCartState() Failed to convert to " ;
         errmsg += mParameterCS->GetName() + " coordinate system.\n";
         errmsg += "Message: " + e.GetFullMessage() + "\n";
         throw ParameterException(errmsg);
      }
   }
   
   return lastCartState;
}


//------------------------------------------------------------------------------
// Rvector6 GetKepState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetKepState()
{
   //LOJ: NEW_PARAMETER: Using mSpacecraft here, no change
   
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Rvector6 kepState = StateConversionUtil::CartesianToKeplerian(mGravConst, state);
   
   #ifdef DEBUG_ORBITDATA_KEP_STATE
   MessageInterface::ShowMessage
      ("OrbitData::GetKepState() kepState=%s", kepState.ToString().c_str());
   #endif
   
   return kepState;
}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetModEquinState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetModEquinState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Rvector6 modEquinState =
      StateConversionUtil::Convert(state, "Cartesian", "ModifiedEquinoctial",
                                   mGravConst, mFlattening, mEqRadius);
   
   return modEquinState;
}

// AlternateEquinoctial by HYKim
//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetAltEquinState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetAltEquinState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
      
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Rvector6 altEquinState =
      StateConversionUtil::Convert(state, "Cartesian", "AlternateEquinoctial",
                                   mGravConst, mFlattening, mEqRadius);
   
   return altEquinState;
}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetDelaState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetDelaState()
{
   #ifdef DEBUG_DELAUNAY_STATE
   MessageInterface::ShowMessage
      ("OrbitData::GetDelaState() entered, mActualParamName='%s'\n", mActualParamName.c_str());
   #endif
   
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Rvector6 delaState =
      StateConversionUtil::Convert(state, "Cartesian", "Delaunay",
                                   mGravConst, mFlattening, mEqRadius);
   
   #ifdef DEBUG_DELAUNAY_STATE
   MessageInterface::ShowMessage
      ("OrbitData::GetDelaState() returning mDelaState='%s'", mDelaState.ToString().c_str());
   #endif
   
   return delaState;
}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetPlanetodeticState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetPlanetodeticState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetPlanetodeticState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Rvector6 planetodeticState =
      StateConversionUtil::Convert(state, "Cartesian", "Planetodetic",
                                   mGravConst, mFlattening, mEqRadius );
   
   return planetodeticState;
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
   Rvector6 kepState = StateConversionUtil::CartesianToKeplerian(mGravConst, state, "TA");
   Rvector6 modKepState = StateConversionUtil::KeplerianToModKeplerian(kepState);
   
   return modKepState;
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
   Rvector6 sphRaDecState = StateConversionUtil::Convert(state, "Cartesian", "SphericalRADEC",
                                                         mGravConst, mFlattening, mEqRadius);
   
   #ifdef DEBUG_ORBITDATA_STATE
   MessageInterface::ShowMessage
      ("OrbitData::GetSphRaDecState() sphRaDecState=\n   %s\n",
       sphRaDecState.ToString().c_str());
   #endif
   
   return sphRaDecState;
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
   Rvector6 sphAzFpaState = StateConversionUtil::Convert(state, "Cartesian", "SphericalAZFPA",
                                                         mGravConst, mFlattening, mEqRadius);
   
   return sphAzFpaState;
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
   Rvector6 mEquinState = StateConversionUtil::Convert(state, "Cartesian", "Equinoctial",
                          mGravConst, mFlattening, mEqRadius);
   
   return mEquinState;
}

// Modified by YK
//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetIncAsymState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetIncAsymState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Rvector6 incAsymState = StateConversionUtil::Convert(state, "Cartesian", "IncomingAsymptote",
                                                        mGravConst, mFlattening, mEqRadius);
   
   return incAsymState;
}

// Modified by YK
//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetOutAsymState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetOutAsymState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Rvector6 outAsymState = StateConversionUtil::Convert(state, "Cartesian", "OutgoingAsymptote",
                                                        mGravConst, mFlattening, mEqRadius);
   
   return outAsymState;
}

// Modified by YK
//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetBLshortState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetBLshortState()
{
   #ifdef DEBUG_BROUWER_SHORT
   MessageInterface::ShowMessage("OrbitData::GetBLshortState() entered\n");
   #endif
   
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();

   #ifdef DEBUG_BROUWER_SHORT
   MessageInterface::ShowMessage("   CartSate = %s", state.ToString().c_str());
   MessageInterface::ShowMessage
      ("   Calling StateConversionUtil::Convert() with mGravConst=%f, "
       "mFlattening=%f, mEqRadius=%f\n", mGravConst, mFlattening, mEqRadius);
   #endif
   
   Rvector6 blShortState = StateConversionUtil::Convert(state, "Cartesian", "BrouwerMeanShort",
                                                        mGravConst, mFlattening, mEqRadius);
   
   #ifdef DEBUG_BROUWER_SHORT
   MessageInterface::ShowMessage
      ("OrbitData::GetBLshortState() returning %s", blSshortState.ToString().c_str());
   #endif
   
   return blShortState;
}

// Modified by YK
//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetBLlongState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetBLlongState()
{
   #ifdef DEBUG_BROUWER_LONG
   MessageInterface::ShowMessage("OrbitData::GetBLlongState() entered\n");
   #endif
   
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Rvector6 blLongState = StateConversionUtil::Convert(state, "Cartesian", "BrouwerMeanLong",
                                                       mGravConst, mFlattening, mEqRadius);
   
   #ifdef DEBUG_BROUWER_LONG
   MessageInterface::ShowMessage
      ("OrbitData::GetBLlongState() returning %s", blLongState.ToString().c_str());
   #endif
   
   return blLongState;
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
   if (item >= CART_X && item <= CART_VZ)
      return state[item];
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

   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < GmatOrbitConstants::KEP_ZERO_TOL)
      throw ParameterException
         ("*** Error *** Cannot convert from Cartesian to Keplerian because position vector is a zero vector.");
   
   switch (item)
   {
   case KEP_SMA:
      return StateConversionUtil::CartesianToSMA(mGravConst, pos, vel);
   case KEP_ECC:
      return StateConversionUtil::CartesianToECC(mGravConst, pos, vel);
   case KEP_INC:
      return StateConversionUtil::CartesianToINC(mGravConst, pos, vel);
   case KEP_TA:
      return StateConversionUtil::CartesianToTA(mGravConst, pos, vel);
   case KEP_EA:
      return StateConversionUtil::CartesianToEA(mGravConst, pos, vel);
   case KEP_MA:
   {
      #ifdef DEBUG_MA
      MessageInterface::ShowMessage("In OrbitData, computing MA -------\n");
      #endif
      return StateConversionUtil::CartesianToMA(mGravConst, pos, vel);
   }
   case KEP_HA:
   {
      #ifdef DEBUG_HA
      MessageInterface::ShowMessage("In OrbitData, computing HA -------\n");
      #endif
      return StateConversionUtil::CartesianToHA(mGravConst, pos, vel);
   }
   case KEP_RAAN:
      return StateConversionUtil::CartesianToRAAN(mGravConst, pos, vel);
   case KEP_RADN:
      {
         Real raanDeg = StateConversionUtil::CartesianToRAAN(mGravConst, pos, vel);
         return AngleUtil::PutAngleInDegRange(raanDeg + 180, 0.0, 360.0);
      }
   case KEP_AOP:
      return StateConversionUtil::CartesianToAOP(mGravConst, pos, vel);
      
   default:
      throw ParameterException("OrbitData::GetKepReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetModKepReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Modified Keplerian element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetModKepReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetModKepReal() item=%d\n", item);
   #endif
      
   Rvector6 state = GetCartState();
   
   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   switch (item)
   {
   case MODKEP_RADAPO:
      return GmatCalcUtil::CalculateKeplerianData("RadApoapsis", state, mGravConst);
   case MODKEP_RADPER:
      return GmatCalcUtil::CalculateKeplerianData("RadPeriapsis", state, mGravConst);
      
   default:
      throw ParameterException
         ("OrbitData::GetModKepReal() Not readable or unknown item id: " +
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
   MessageInterface::ShowMessage
      ("OrbitData::GetOtherKepReal() item=%d, idToUse=%d\n", item, item-MM);
   #endif
   
   Rvector6 state = GetCartState();
   
   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   return GmatCalcUtil::CalculateKeplerianData(VALID_OTHER_ORBIT_PARAM_NAMES[item-MM], state, mGravConst);
   
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
   case RADEC_RMAG:
      {
         // if orgin is "Earth" just return default
         if (mOrigin->GetName() == "Earth")
            return state[RADEC_RMAG-Item2Count];
         else
            return GetPositionMagnitude(mOrigin);
      }
   case RADEC_RA:
   case RADEC_DEC:
   case RADEC_VMAG:
   case RADEC_RAV:
   case RADEC_DECV:
      return state[item-Item2Count];
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
   
   if (item >= AZIFPA_RMAG && item <= AZIFPA_FPA)
      //return mSphAzFpaState[item-Item3Count];
      return state[item-Item3Count];
   else
   {
      throw ParameterException("OrbitData::GetSphAzFpaReal() Unknown orbit item ID: " +
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
   MessageInterface::ShowMessage
      ("OrbitData::GetAngularReal() item=%d, idToUse=%d\n", item, item-SEMILATUS_RECTUM);
   #endif
   
   Rvector6 state = GetCartState();
   Rvector3 dummy;

   // provide relative state if necessary
   if ((item == SEMILATUS_RECTUM || (item == HMAG)) && mOrigin->GetName() != "Earth")
   {
      state = GetRelativeCartState(mOrigin);
   }

   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetAngularReal() item=%d state=%s\n",
       item, state.ToString().c_str());
   #endif
   
   return GmatCalcUtil::CalculateAngularData(VALID_ANGLE_PARAM_NAMES[item-SEMILATUS_RECTUM], state, mGravConst, dummy);

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
   MessageInterface::ShowMessage
      ("OrbitData::GetOtherAngleReal() item=%d, idToUse=%d\n", item, item-SEMILATUS_RECTUM);
   #endif
   
   Rvector6 state;
   if (mOrigin->GetName() != "Earth")
      state = GetRelativeCartState(mOrigin);
   else
      state = GetCartState();
   // compute sun unit vector from the origin
   Rvector3 sunPos = (mSolarSystem->GetBody(SolarSystem::SUN_NAME))->
      GetMJ2000Position(mCartEpoch);
   Rvector3 originPos = mOrigin->GetMJ2000Position(mCartEpoch);
   Rvector3 originToSun = sunPos - originPos;
   originToSun.Normalize();

   return GmatCalcUtil::CalculateAngularData(VALID_ANGLE_PARAM_NAMES[item-SEMILATUS_RECTUM],
                                             state, mGravConst,originToSun);
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

   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < GmatOrbitConstants::KEP_ZERO_TOL)
      throw ParameterException
         ("*** Error *** Cannot convert from Cartesian to Equinoctial because position vector is a zero vector.");
   
   switch (item)
   {
   case EQ_SMA:
      return StateConversionUtil::CartesianToSMA(mGravConst, pos, vel);
   case EQ_H:
      {
         Rvector6 equiState = GetEquinState();
         return equiState[1];
      }
   case EQ_K:
      {
         Rvector6 equiState = GetEquinState();
         return equiState[2];
      }
   case EQ_P:
      {
         Rvector6 equiState = GetEquinState();
         return equiState[3];
      }
   case EQ_Q:
      {
         Rvector6 equiState = GetEquinState();
         return equiState[4];
      }
   case EQ_MLONG:
      {
         Rvector6 equiState = GetEquinState();
         return equiState[5];
      }
      
   default:
      throw ParameterException("OrbitData::GetEquinReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}


// Modified by M.H.
//------------------------------------------------------------------------------
// Real GetModEquinReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves ModifiedEquinoctial element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetModEquinReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetModEquinReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();

   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < GmatOrbitConstants::KEP_ZERO_TOL)
      throw ParameterException
         ("*** Error *** Cannot convert from Cartesian to ModifiedEquinoctial because position vector is a zero vector.");
   
   switch (item)
   {
   case MOD_EQ_P:
      {
         Rvector6 modequiState = GetModEquinState();
         return modequiState[0];
      }
   case MOD_EQ_F:
      {
         Rvector6 modequiState = GetModEquinState();
         return modequiState[1];
      }
   case MOD_EQ_G:
      {
         Rvector6 modequiState = GetModEquinState();
         return modequiState[2];
      }
   case MOD_EQ_H:
      {
         Rvector6 modequiState = GetModEquinState();
         return modequiState[3];
      }
   case MOD_EQ_K:
      {
         Rvector6 modequiState = GetModEquinState();
         return modequiState[4];
      }
   case MOD_EQ_TLONG:
      {
         Rvector6 modequiState = GetModEquinState();
         return modequiState[5];
      }
      
   default:
      throw ParameterException("OrbitData::GetModEquinReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}

// AlternateEquinoctial by HYKim
//------------------------------------------------------------------------------
// Real GetAltEquinReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves Alternate Equinoctial element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetAltEquinReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetAltEquinReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();
   
   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < GmatOrbitConstants::KEP_ZERO_TOL)
      throw ParameterException
         ("*** Error *** Cannot convert from Cartesian to Alternate Equinoctial "
          "because position vector is a zero vector.");
   
   switch (item)
   {
   case ALT_EQ_P:
   {
      Rvector6 altequiState = GetAltEquinState();
      // I think the index should be 3 (LOJ: 2014.01.29)
      //return altequiState[1];
      return altequiState[3];
   }
   case ALT_EQ_Q:
   {
      Rvector6 altequiState = GetAltEquinState();
      // I think the index should be 4 (LOJ: 2014.01.29)
      //return altequiState[2];
      return altequiState[4];
   }
   
   default:
      throw ParameterException("OrbitData::GetAltEquinReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}


// Modified by M.H.
//------------------------------------------------------------------------------
// Real GetDelaReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves Delaunay element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetDelaReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetDelaReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();

   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < GmatOrbitConstants::KEP_ZERO_TOL)
      throw ParameterException
         ("*** Error *** Cannot convert from Cartesian to Delaunay because position vector is a zero vector.");
   
   switch (item)
   {
   case DEL_DELA_SL:
      {
         Rvector6 delaState = GetDelaState();
         return delaState[0];
      }
   case DEL_DELA_SG:
      {
         Rvector6 delaState = GetDelaState();
         return delaState[1];
      }
   case DEL_DELA_SH:
      {
         Rvector6 delaState = GetDelaState();
         return delaState[2];
      }
   case DEL_DELA_L:
      {
         Rvector6 delaState = GetDelaState();
         return delaState[3];
      }
   case DEL_DELA_G:
      {
         Rvector6 delaState = GetDelaState();
         return delaState[4];
      }
   case DEL_DELA_H:
      {
         Rvector6 delaState = GetDelaState();
         return delaState[5];
      }
      
   default:
      throw ParameterException("OrbitData::GetDelaReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}

// Modified by M.H. ??
//------------------------------------------------------------------------------
// Real GetPlanetodeticReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Planetodetic element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetPlanetodeticReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetPlanetodeticReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetPlanetodeticState();

   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetPlanetodeticReal() item=%s state=%s\n",
       item, state.ToString().c_str());
   #endif
   
   switch (item)
   {
   case PLD_RMAG:
      {
         Rvector6 PlanetodeticState = GetPlanetodeticState();
         return PlanetodeticState[0];
      }
   case PLD_LON:
      {
         Rvector6 PlanetodeticState = GetPlanetodeticState();
         return PlanetodeticState[1];
      }
   case PLD_LAT:
      {
         Rvector6 PlanetodeticState = GetPlanetodeticState();
         return PlanetodeticState[2];
      }
   case PLD_VMAG:
      {
         Rvector6 PlanetodeticState = GetPlanetodeticState();
         return PlanetodeticState[3];
      }
   case PLD_AZI:
      {
         Rvector6 PlanetodeticState = GetPlanetodeticState();
         return PlanetodeticState[4];
      }
   case PLD_HFPA:
      {
         Rvector6 PlanetodeticState = GetPlanetodeticState();
         return PlanetodeticState[5];
      }
      
   default:
      throw ParameterException("OrbitData::GetPlanetodeticReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}

// Modified by YK
//------------------------------------------------------------------------------
// Real GetIncAsymReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves IncomingAsymptote element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetIncAsymReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetIncAsymReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();

   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < GmatOrbitConstants::KEP_ZERO_TOL)
      throw ParameterException
         ("*** Error *** Cannot convert from Cartesian to IncomingAsymptote because position vector is a zero vector.");
   
   switch (item)
   {
   case INCASYM_RADPER:
   {
      Rvector6 IncAsymState = GetIncAsymState();
      return IncAsymState[0];
   }
   
   case INCASYM_C3:
   {
      Rvector6 IncAsymState = GetIncAsymState();
      return IncAsymState[1];
   }
   
   case INCASYM_RHA:
   {
      Rvector6 IncAsymState = GetIncAsymState();
      return IncAsymState[2];
   }
   
   case INCASYM_DHA:
   {
      Rvector6 IncAsymState = GetIncAsymState();
      return IncAsymState[3];
   }
   
   case INCASYM_BVAZI:
   {
      Rvector6 IncAsymState = GetIncAsymState();
      return IncAsymState[4];
   }
   
   case INCASYM_TA:
   {
      Rvector6 IncAsymState = GetIncAsymState();
      return IncAsymState[5];
   }
   
   default:
      throw ParameterException("OrbitData::GetIncAsymReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}

// Modified by YK
//------------------------------------------------------------------------------
// Real GetOutAsymReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves OutgoingAsymptote element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetOutAsymReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetOutAsymReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();

   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < GmatOrbitConstants::KEP_ZERO_TOL)
      throw ParameterException
         ("*** Error *** Cannot convert from Cartesian to OutgoingAsymptote because position vector is a zero vector.");
   
   switch (item)
   {
   case OUTASYM_RADPER:
   {
      Rvector6 OutAsymState = GetOutAsymState();
      return OutAsymState[0];
   }
   
   case OUTASYM_C3:
   {
      Rvector6 OutAsymState = GetOutAsymState();
      return OutAsymState[1];
   }
   
   case OUTASYM_RHA:
   {
      Rvector6 OutAsymState = GetOutAsymState();
      return OutAsymState[2];
   }
   
   case OUTASYM_DHA:
   {
      Rvector6 OutAsymState = GetOutAsymState();
      return OutAsymState[3];
   }
   
   case OUTASYM_BVAZI:
   {
      Rvector6 OutAsymState = GetOutAsymState();
      return OutAsymState[4];
   }
   
   case OUTASYM_TA:
   {
      Rvector6 OutAsymState = GetOutAsymState();
      return OutAsymState[5];
   }
   
   default:
      throw ParameterException("OrbitData::GetOutAsymReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}


// Modified by YK
//------------------------------------------------------------------------------
// Real GetBLshortReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves BrouwerMeanShort element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetBLshortReal(Integer item)
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("OrbitData::GetBLshortReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();

   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < GmatOrbitConstants::KEP_ZERO_TOL)
      throw ParameterException
         ("*** Error *** Cannot convert from Cartesian to BrouwerMeanShort because position vector is a zero vector.");
   
   switch (item)
   {
   case BLS_SMA:
   {
      Rvector6 BLshortState = GetBLshortState();
      return BLshortState[0];
   }
   
   case BLS_ECC:
   {
      Rvector6 BLshortState = GetBLshortState();
      return BLshortState[1];
   }
   
   case BLS_INC:
   {
      Rvector6 BLshortState = GetBLshortState();
      return BLshortState[2];
   }
   
   case BLS_RAAN:
   {
      Rvector6 BLshortState = GetBLshortState();
      return BLshortState[3];
   }
   
   case BLS_AOP:
   {
      Rvector6 BLshortState = GetBLshortState();
      return BLshortState[4];
   }
   
   case BLS_MA:
   {
      Rvector6 BLshortState = GetBLshortState();
      return BLshortState[5];
   }
   
   default:
      throw ParameterException("OrbitData::GetBLshortReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}

// Modified by YK
//------------------------------------------------------------------------------
// Real GetBLlongReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves BrouwerMeanShort element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetBLlongReal(Integer item)
{
   #ifdef DEBUG_BROUWER_LONG
   MessageInterface::ShowMessage("OrbitData::GetBLlongReal() item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();

   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < GmatOrbitConstants::KEP_ZERO_TOL)
      throw ParameterException
         ("*** Error *** Cannot convert from Cartesian to BrouwerMeanLong because position vector is a zero vector.");
   
   switch (item)
   {
   case BLL_SMA:
   {
      Rvector6 BLlongState = GetBLlongState();
      return BLlongState[0];
   }
   
   case BLL_ECC:
   {
      Rvector6 BLlongState = GetBLlongState();
      return BLlongState[1];
   }
   
   case BLL_INC:
   {
      Rvector6 BLlongState = GetBLlongState();
      return BLlongState[2];
   }
   
   case BLL_RAAN:
   {
      Rvector6 BLlongState = GetBLlongState();
      return BLlongState[3];
   }
   
   case BLL_AOP:
   {
      Rvector6 BLlongState = GetBLlongState();
      return BLlongState[4];
   }
   
   case BLL_MA:
   {
      Rvector6 BLlongState = GetBLlongState();
      return BLlongState[5];
   }
   
   default:
      throw ParameterException("OrbitData::GetBLlongReal() Unknown parameter id: " +
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

// The inherited methods from RefData
std::string OrbitData::GetRefObjectName(const Gmat::ObjectType type) const
{
   try
   {
      return RefData::GetRefObjectName(type);
   }
   catch (ParameterException &pe)
   {
      // if the type was SpacePoint, we may need to look in the Spacecraft
      // list (or vice versa?), since we are looking for a Spacecraft
      // and a Spacecraft is a SpacePoint
      Gmat::ObjectType altType = type;
      if (type == Gmat::SPACE_POINT) altType = Gmat::SPACECRAFT;
      #ifdef DEBUG_ORBITDATA_OBJNAME
         MessageInterface::ShowMessage(
               "OrbitData::GetRefObjectName -> couldn't find type %d, so look for type %d\n",
               type, altType);
      #endif
      for (int i=0; i<mNumRefObjects; i++)
      {
         if (mRefObjList[i].objType == altType)
         {
            //Notes: will return first object name.
            #ifdef DEBUG_ORBITDATA_OBJNAME
            MessageInterface::ShowMessage
               ("---> OrbitData::GetRefObjectName() altType=%d returning: %s\n", altType,
                mRefObjList[i].objName.c_str());
            #endif
            return mRefObjList[i].objName;
         }
      }

      throw;
   }
}

const StringArray& OrbitData::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   RefData::GetRefObjectNameArray(type);

   if (mAllRefObjectNames.empty() && type == Gmat::SPACE_POINT)
   {
      // if the type was SpacePoint, we may need to look in the Spacecraft
      // list (or vice versa?), since we are looking for a Spacecraft
      // and a Spacecraft is a SpacePoint
      return RefData::GetRefObjectNameArray(Gmat::SPACECRAFT);
   }
   else
      return mAllRefObjectNames;
}

//------------------------------------------------------------------------------
// bool SetRefObjectName(Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Adds type and name to reference object list.
 *
 * @param <type> reference object type
 * @param <name> reference object name
 *
 * @return true if type and name has successfully added to the list
 *
 */
//------------------------------------------------------------------------------
bool OrbitData::SetRefObjectName(Gmat::ObjectType type, const std::string &name)
{
   // We need to be able to handle SpacePoints, not just Spacecraft
   Gmat::ObjectType useType = type;
   if ((type == Gmat::GROUND_STATION)   || (type == Gmat::BODY_FIXED_POINT) ||
       (type == Gmat::CALCULATED_POINT) || (type == Gmat::LIBRATION_POINT)  ||
       (type == Gmat::BARYCENTER)       || (type == Gmat::CELESTIAL_BODY))
      useType = Gmat::SPACE_POINT;

   return RefData::SetRefObjectName(useType, name);

}

GmatBase* OrbitData::GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name)
{
   try
   {
      GmatBase* theObj = RefData::GetRefObject(type, name);
      return theObj;
   }
   catch (ParameterException &pe)
   {
      // if the type was SpacePoint, we may need to look in the Spacecraft
      // list (or vice versa?), since we are looking for a Spacecraft
      // and a Spacecraft is a SpacePoint
      Gmat::ObjectType altType = type;
      if (type == Gmat::SPACE_POINT) altType = Gmat::SPACECRAFT;
      for (int i=0; i<mNumRefObjects; i++)
      {
         if (mRefObjList[i].objType == altType)
         {
            if (name == "") //if name is "", return first object
               return mRefObjList[i].obj;

            if ((name == "" || mRefObjList[i].objName == name) &&
                (mRefObjList[i].obj)->IsOfType("Spacecraft"))
            {
               //Notes: will return first object name.
               #ifdef DEBUG_ORBITDATA_OBJNAME
               MessageInterface::ShowMessage
                  ("---> OrbitData::GetRefObject() altType=%d returning: %s\n", altType,
                   mRefObjList[i].objName.c_str());
               #endif
               return mRefObjList[i].obj;
            }
         }
      }

      throw;
   }
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Sets object which is used in evaluation.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool OrbitData::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name)
{
   // We need to be able to handle SpacePoints, not just Spacecraft
   Gmat::ObjectType useType = type;
   if ((type == Gmat::GROUND_STATION)   || (type == Gmat::BODY_FIXED_POINT) ||
       (type == Gmat::CALCULATED_POINT) || (type == Gmat::LIBRATION_POINT)  ||
       (type == Gmat::BARYCENTER)       || (type == Gmat::CELESTIAL_BODY))
      useType = Gmat::SPACE_POINT;

   return RefData::SetRefObject(obj, useType, name);
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool AddRefObject(const Gmat::ObjectType type, const std::string &name, GmatBase *obj,
//                   bool replaceName)
//------------------------------------------------------------------------------
bool OrbitData::AddRefObject(const Gmat::ObjectType type,
                            const std::string &name, GmatBase *obj,
                            bool replaceName)
{
   // We need to be able to handle SpacePoints, not just Spacecraft
   Gmat::ObjectType useType = type;
   if ((type == Gmat::GROUND_STATION)   || (type == Gmat::BODY_FIXED_POINT) ||
       (type == Gmat::CALCULATED_POINT) || (type == Gmat::LIBRATION_POINT)  ||
       (type == Gmat::BARYCENTER))
      useType = Gmat::SPACE_POINT;

   return RefData::AddRefObject(useType, name, obj, replaceName);
}


//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void OrbitData::InitializeRefObjects()
{
   #ifdef DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage
      ("OrbitData::InitializeRefObjects() entered, mActualParamName='%s', "
       "mParamOwnerType=%d, mParamOwnerName='%s', mParamDepName='%s', mParamTypeName='%s'\n",
       mActualParamName.c_str(), mParamOwnerType, mParamOwnerName.c_str(), mParamDepName.c_str(),
       mParamTypeName.c_str());
   #endif
   
   //LOJ: NEW_PARAMETER: Changed mSpacecraft to mSpacePoint and to use FindObject with mParamOwnerName
   // Changed to pass Gmat::SPACE_POINT since orbit Parameter's owner type is SpacePoint
   mSpacePoint = (SpacePoint*)FindObject(mParamOwnerType, mParamOwnerName);
   //mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   
   #ifdef DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage("   mSpacePoint=<%p>\n", mSpacePoint);
   #endif
   
   //if (mSpacecraft == NULL)
   if (mSpacePoint == NULL)
   {
      #ifdef DEBUG_ORBITDATA_INIT
      MessageInterface::ShowMessage
         //("OrbitData::InitializeRefObjects() Cannot find spacecraft: " +
         ("OrbitData::InitializeRefObjects() Cannot find SpacePoint: " +
          GetRefObjectName(Gmat::SPACE_POINT) + ".\n" +
          "Make sure Spacecraft is set to any internal parameters.\n");
      #endif
      
      throw ParameterException
         //("Cannot find spacecraft: " + GetRefObjectName(Gmat::SPACE_POINT));
         ("Cannot find Spacepoint: " + GetRefObjectName(Gmat::SPACE_POINT));
   }
   
   //LOJ: NEW_PARAMETER: Handle special to spacecraft
   if ((mParamOwnerType == Gmat::SPACECRAFT) || (mSpacePoint->IsOfType("Spacecraft")))
   {
      mSpacecraft = (Spacecraft*)mSpacePoint;
      if (stateTypeId == -1)
         stateTypeId = mSpacecraft->GetParameterID("DisplayStateType");
   }
   #ifdef DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage
      ("OrbitData::InitializeRefObjects()  ... 1\n");
   #endif
   
   mSolarSystem =
      (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   
   if (mSolarSystem == NULL)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() Cannot find SolarSystem object\n");
   
   if (mInternalCS == NULL)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() Cannot find internal "
          "CoordinateSystem object\n");
   
   #ifdef DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage
      ("OrbitData::InitializeRefObjects()  solar system and coord system are not NULL\n");
   #endif

   //-----------------------------------------------------------------
   // if dependent body name exist and it is a CelestialBody,
   // it is origin dependent parameter, set new gravity constant.
   //-----------------------------------------------------------------
   
   //LOJ: NEW_PARAMETER: Changed to use mParamDepName

   //std::string originName =
   //   FindFirstObjectName(GmatBase::GetObjectType(VALID_OBJECT_TYPE_LIST[SPACE_POINT]));
   
   std::string originName;
   GmatBase *depObj = NULL;
   mIsParamOriginDep = false;
   if (mParamDepName != "")
   {
      depObj = FindObject(Gmat::SPACE_POINT, mParamDepName);
      if (depObj)
         originName = depObj->GetName();
   }
   
   #ifdef DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage
      ("   originName='%s', depObj=<%p>\n", originName.c_str(), depObj);
   #endif
   
   if (originName != "")
   {
      #ifdef DEBUG_ORBITDATA_INIT
      MessageInterface::ShowMessage
         ("OrbitData::InitializeRefObjects() getting originName: %s pointer.\n",
          originName.c_str());
      #endif
      
      mOrigin = (SpacePoint*)depObj;
            
      if (!mOrigin)
         throw InvalidDependencyException
            (" is a central body dependent parameter.");
      
      // override gravity constant if origin is CelestialBody
      if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
      {
         mGravConst  = ((CelestialBody*)mOrigin)->GetGravitationalConstant();
         mFlattening = ((CelestialBody*)mOrigin)->GetFlattening();
         mEqRadius   = ((CelestialBody*)mOrigin)->GetEquatorialRadius();
         #ifdef DEBUG_ORBITDATA_INIT
         MessageInterface::ShowMessage
            ("OrbitData::InitializeRefObjects() Setting grav constant to %le (for origin %s)\n",
                  mGravConst, (mOrigin->GetName()).c_str());
         #endif
      }
      
      mIsParamOriginDep = true;
   }
   //-----------------------------------------------------------------
   // It is CoordinateSystem dependent parameter.
   // Set Origin to CoordinateSystem origin and mu to origin mu.
   //-----------------------------------------------------------------
   else
   {
      mParameterCS =
         (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
      
      if (mParameterCS == NULL)
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
      mOrigin = mParameterCS->GetOrigin();
      
      if (!mOrigin)
      {
         #ifdef DEBUG_ORBITDATA_INIT
         MessageInterface::ShowMessage
            ("OrbitData::InitializeRefObjects() origin not found: " +
             mParameterCS->GetOriginName() + "\n");
         #endif
      
         throw ParameterException
            ("OrbitData::InitializeRefObjects() The origin of CoordinateSystem \"" +
             mParameterCS->GetOriginName() + "\" is NULL");
      }
      
      // get gravity constant if out coord system origin is CelestialBody
      if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
      {
         mGravConst  = ((CelestialBody*)mOrigin)->GetGravitationalConstant();
         mFlattening = ((CelestialBody*)mOrigin)->GetFlattening();
         mEqRadius   = ((CelestialBody*)mOrigin)->GetEquatorialRadius();
      }
      #ifdef DEBUG_ORBITDATA_INIT
      MessageInterface::ShowMessage
         ("OrbitData::InitializeRefObjects() Now setting grav constant to %le (for origin %s)\n",
               mGravConst, (mOrigin->GetName()).c_str());
      #endif
   }
   
   #ifdef DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage
      ("OrbitData::InitializeRefObjects() exiting, mOrigin.Name=%s, mGravConst=%f, "
       "mIsParamOriginDep=%d\n",  mOrigin->GetName().c_str(), mGravConst, mIsParamOriginDep);
   MessageInterface::ShowMessage
      ("   mSpacePoint=<%p> '%s', mSolarSystem=<%p>, mParameterCS=<%p>, mOrigin=<%p>\n",
       mSpacecraft, mSpacePoint->GetName().c_str(),mSolarSystem, mParameterCS, mOrigin);
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
   return mInternalCS;
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
   mInternalCS = cs;
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
// Rvector6 GetCartStateInParameterCS(Integer item, Real rval)
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetCartStateInParameterCS(Integer item, Real rval)
{
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::GetCartStateInParameterCS() '%s' entered, item=%d, rval=%f\n"
       "   Now converting to Parameter CS before setting value\n", 
       mActualParamName.c_str(), item, rval);
   #endif
   Rvector6 cartState = GetCartState();
   Rvector6 cartStateInParamCS;
   Real currEpoch = mSpacePoint->GetEpoch();
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("   Setting new value %f to Parameter that has different CS than Parameter Owner CS\n"
       "   cartState = \n   %s", rval, cartState.ToString().c_str());
   #endif
   
   // Convert to parameterCS
   mCoordConverter.Convert(A1Mjd(currEpoch), cartState, mInternalCS,
                           cartStateInParamCS, mParameterCS, false);
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("   from internalCS to paramCS = \n   %s", cartStateInParamCS.ToString().c_str());
   #endif
   
   if (item >= CART_X && item <= CART_VZ)
   {
      // Now set new value to state
      cartStateInParamCS(item) = rval;
   }
   else if (item >= KEP_SMA && item <= KEP_TA)
   {
      Rvector6 keplStateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "Keplerian",
                                      mGravConst, mFlattening, mEqRadius);
      keplStateInParamCS[item-Item1Count] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(keplStateInParamCS, "Keplerian", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= RADEC_RMAG && item <= RADEC_DECV)
   {
      Rvector6 radecStateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "SphericalRADEC",
                                      mGravConst, mFlattening, mEqRadius);
      radecStateInParamCS[item-Item2Count] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(radecStateInParamCS, "SphericalRADEC", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= AZIFPA_RMAG && item <= AZIFPA_FPA)
   {
      Rvector6 azfpaStateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "SphericalAZFPA",
                                      mGravConst, mFlattening, mEqRadius);
      azfpaStateInParamCS[item-Item3Count] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(azfpaStateInParamCS, "SphericalAZFPA", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= EQ_SMA && item <= EQ_MLONG)
   {
      Rvector6 equinStateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "Equinoctial",
                                      mGravConst, mFlattening, mEqRadius);
      equinStateInParamCS[item-Item6Count] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(equinStateInParamCS, "Equinoctial", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= MOD_EQ_P && item <= MOD_EQ_TLONG)
   {
      Rvector6 modequinStateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "ModifiedEquinoctial",
                                      mGravConst, mFlattening, mEqRadius);
      modequinStateInParamCS[item-Item8Count] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(modequinStateInParamCS, "ModifiedEquinoctial", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= DEL_DELA_SL && item <= DEL_DELA_H)
   {
      Rvector6 delaunayStateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "Delaunay",
                                      mGravConst, mFlattening, mEqRadius);
      delaunayStateInParamCS[item-Item9Count] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(delaunayStateInParamCS, "Delaunay", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= PLD_RMAG && item <= PLD_HFPA)
   {
      Rvector6 planetoStateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "Planetodetic",
                                      mGravConst, mFlattening, mEqRadius);
      planetoStateInParamCS[item-Item10Count] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(planetoStateInParamCS, "Planetodetic", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else
   {
      // Unimplemented state types should get here, so throw a exception
      ParameterException pe;
      pe.SetDetails("Setting the Parameter %s has not been implemented)", mActualParamName.c_str());
      throw pe;
   }
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::GetCartStateInParameterCS() '%s' returning\n   %s",
       mActualParamName.c_str(), cartStateInParamCS.ToString().c_str());
   #endif
   return cartStateInParamCS;
}


//------------------------------------------------------------------------------
// Rvector6 GetCartStateInParameterOrigin(Integer item, Real rval)
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetCartStateInParameterOrigin(Integer item, Real rval)
{
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::GetCartStateInParameterOrigin() '%s' entered, item=%d, rval=%f\n"
       "   Now converting to Parameter CS before setting value\n", 
       mActualParamName.c_str(), item, rval);
   #endif
   
   Rvector6 cartStateInParamOrigin = GetRelativeCartState(mOrigin);
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("   Setting new value %f to Parameter that has different Origin than "
       "Parameter Owner CS Origin\n   cartStateInParamOrigin = \n   %s", rval,
       cartStateInParamOrigin.ToString().c_str());
   #endif
   
   switch (item)
   {
   case KEP_SMA:
   case KEP_ECC:
   case KEP_TA:
   {
      Rvector6 keplStateInParamOrigin =
         StateConversionUtil::Convert(cartStateInParamOrigin, "Cartesian", "Keplerian",
                                      mGravConst, mFlattening, mEqRadius);
      keplStateInParamOrigin[item-Item1Count] = rval;
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   keplStateInParamOrigin = \n   %s", keplStateInParamOrigin.ToString().c_str());
      #endif
      cartStateInParamOrigin =
         StateConversionUtil::Convert(keplStateInParamOrigin, "Keplerian", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);      
      break;
   }
   case MODKEP_RADPER:
   case MODKEP_RADAPO:
   {
      Rvector6 keplStateInParamOrigin =
         StateConversionUtil::Convert(cartStateInParamOrigin, "Cartesian", "ModifiedKeplerian",
                                      mGravConst, mFlattening, mEqRadius);
      // ModKeplerian elements: RadPer, RadApo, INC, RAAN, AOP, TA
      if (item == MODKEP_RADPER)
         keplStateInParamOrigin[0] = rval;
      else
         keplStateInParamOrigin[1] = rval;
      cartStateInParamOrigin =
         StateConversionUtil::Convert(keplStateInParamOrigin, "ModifiedKeplerian", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
      break;
   }
   case RADEC_RMAG:
   {
      Rvector6 radecStateInParamOrigin =
         StateConversionUtil::Convert(cartStateInParamOrigin, "Cartesian", "SphericalRADEC",
                                      mGravConst, mFlattening, mEqRadius);
      // SphericalRADEC elements: RMAG, RA, DEC, VMAG, RAV, DECV
      radecStateInParamOrigin[0] = rval;
      cartStateInParamOrigin =
         StateConversionUtil::Convert(radecStateInParamOrigin, "SphericalRADEC", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
      break;
   }
   default:
   {
      // Unimplemented state types should get here, so throw a exception
      ParameterException pe;
      pe.SetDetails("Setting the Parameter %s has not been implemented)", mActualParamName.c_str());
      throw pe;
   }
   }
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::GetCartStateInParameterOrigin() '%s' returning\n   %s",
       mActualParamName.c_str(), cartStateInParamOrigin.ToString().c_str());
   #endif
   return cartStateInParamOrigin;
}


//------------------------------------------------------------------------------
// void SetRealParameters(Integer item, Real rval)
//------------------------------------------------------------------------------
void OrbitData::SetRealParameters(Integer item, Real rval)
{
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::SetRealParameters() entered, item=%d, rval=%f\n   "
       "Now calling Spacecraft::SetRealParameter()\n", item, rval);
   #endif
   
   // Do the direct setting
   switch (item)
   {
   // Cartesian
   case CART_X:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("X"), rval);
      break;
   case CART_Y:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("Y"), rval);
      break;
   case CART_Z:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("Z"), rval);
      break;
   case CART_VX:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("VX"), rval);
      break;
   case CART_VY:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("VY"), rval);
      break;
   case CART_VZ:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("VZ"), rval);
      break;
      
   // Keplerian
   case KEP_SMA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("SMA"), rval);
      break;
   case KEP_ECC:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("ECC"), rval);
      break;
   case KEP_INC:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("INC"), rval);
      break;
   case KEP_RAAN:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("RAAN"), rval);
      break;
   case KEP_AOP:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("AOP"), rval);
      break;
   case KEP_TA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("TA"), rval);
      break;
   case KEP_MA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("MA"), rval);
      break;
   case KEP_EA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("EA"), rval);
      break;
   case KEP_HA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("HA"), rval);
      break;

   // ModifiedKeplerian
   case MODKEP_RADAPO:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("RadApo"), rval);
      break;
   case MODKEP_RADPER:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("RadPer"), rval);
      break;

   // Spherical RADEC
   case RADEC_RMAG:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("RMAG"), rval);
      break;
   case RADEC_RA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("RA"), rval);
      break;
   case RADEC_DEC:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("DEC"), rval);
      break;
   case RADEC_VMAG:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("VMAG"), rval);
      break;
   case RADEC_RAV:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("RAV"), rval);
      break;
   case RADEC_DECV:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("DECV"), rval);
      break;
   
   // Spherical AZFPA
   case AZIFPA_RMAG:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("RMAG"), rval);
      break;
   case AZIFPA_RA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("RA"), rval);
      break;
   case AZIFPA_DEC:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("DEC"), rval);
      break;
   case AZIFPA_VMAG:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("VMAG"), rval);
      break;
   case AZIFPA_AZI:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("AZI"), rval);
      break;
   case AZIFPA_FPA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("FPA"), rval);
      break;

   // Equinoctial
   case EQ_SMA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("SMA"), rval);
      break;
   case EQ_H:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("EquinoctialH"), rval);
      break;
   case EQ_K:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("EquinoctialK"), rval);
      break;
   case EQ_P:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("EquinoctialP"), rval);
      break;
   case EQ_Q:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("EquinoctialQ"), rval);
      break;
   case EQ_MLONG:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("MLONG"), rval);
      break;

   // ModifiedEquinoctial;Modified by M.H.
   case MOD_EQ_P:
      //mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("SemiLatusRectum"), rval);
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("SemilatusRectum"), rval);
      break;
   case MOD_EQ_F:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("ModEquinoctialF"), rval);
      break;
   case MOD_EQ_G:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("ModEquinoctialG"), rval);
      break;
   case MOD_EQ_H:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("ModEquinoctialH"), rval);
      break;
   case MOD_EQ_K:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("ModEquinoctialK"), rval);
      break;
   case MOD_EQ_TLONG:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("TLONG"), rval);
      break;
      
   // Alternate Equinoctial by HYKim
   case ALT_EQ_P:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("AltEquinoctialP"), rval);
      break;
   case ALT_EQ_Q:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("AltEquinoctialQ"), rval);
      break;
      
   // Delaunay;Modified by M.H.
   case DEL_DELA_SL:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("Delaunayl"), rval);
      break;
   case DEL_DELA_SG:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("Delaunayg"), rval);
      break;
   case DEL_DELA_SH:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("Delaunayh"), rval);
      break;
   case DEL_DELA_L:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("DelaunayL"), rval);
      break;
   case DEL_DELA_G:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("DelaunayG"), rval);
      break;
   case DEL_DELA_H:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("DelaunayH"), rval);
      break;

   // Planetodetic;Modified by M.H.
   case PLD_RMAG:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("PlanetodeticRMAG"), rval);
      break;
   case PLD_LON:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("PlanetodeticLON"), rval);
      break;
   case PLD_LAT:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("PlanetodeticLAT"), rval);
      break;
   case PLD_VMAG:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("PlanetodeticVMAG"), rval);
      break;
   case PLD_AZI:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("PlanetodeticAZI"), rval);
      break;
   case PLD_HFPA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("PlanetodeticHFPA"), rval);
      break;
      
   // Incoming Asymptote State
   case INCASYM_RADPER:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("RadPer"), rval);
      break;
   case INCASYM_C3:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("C3Energy"), rval);
      break;
   case INCASYM_RHA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("IncomingRHA"), rval);
      break;
   case INCASYM_DHA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("IncomingDHA"), rval);
      break;
   case INCASYM_BVAZI:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("IncomingBVAZI"), rval);
      break;
   
   // Outgoing Asymptote State
   case OUTASYM_RADPER:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("RadPer"), rval);
      break;
    case OUTASYM_C3:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("C3Energy"), rval);
      break;
  case OUTASYM_RHA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("OutgoingRHA"), rval);
      break;
   case OUTASYM_DHA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("OutgoingDHA"), rval);
      break;
   case OUTASYM_BVAZI:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("OutgoingBVAZI"), rval);
      break;
   case OUTASYM_TA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("TA"), rval);
      break;
      
   // Brouwer-Lyddane Mean-short
   case BLS_SMA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("SMAP"), rval);
      break;
   case BLS_ECC:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("ECCP"), rval);
      break;
   case BLS_INC:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("INCP"), rval);
      break;
   case BLS_RAAN:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("RAANP"), rval);
      break;
   case BLS_AOP:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("AOPP"), rval);
      break;
   case BLS_MA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("MAP"), rval);
      break;

   // Brouwer-Lyddane Mean-long
   case BLL_SMA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("SMADP"), rval);
      break;
   case BLL_ECC:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("ECCDP"), rval);
      break;
   case BLL_INC:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("INCDP"), rval);
      break;
   case BLL_RAAN:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("RAANDP"), rval);
      break;
   case BLL_AOP:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("AOPDP"), rval);
      break;
   case BLL_MA:
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("MADP"), rval);
      break;

   default:
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("**** ERROR **** OrbitData::SetReal() Unknown item id: %d\n", item);
      #endif
      throw ParameterException("OrbitData::SetReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage("OrbitData::SetRealParameters() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void DebugOutputData(CoordinateSystem *paramOwnerCS)
//------------------------------------------------------------------------------
void OrbitData::DebugOutputData(CoordinateSystem *paramOwnerCS)
{
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("   Spacecraft                 = <%p>'%s'\n", mSpacecraft,
       mSpacecraft ? mSpacecraft->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   Parameter CS               = <%p>'%s'\n", mParameterCS,
       mParameterCS ? mParameterCS->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   ParamOwner CS              = <%p>'%s'\n", paramOwnerCS,
       paramOwnerCS ? paramOwnerCS->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   Parameter CS Origin        = <%p>'%s'\n", mParameterCS,
       mParameterCS ? mParameterCS->GetOriginName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   ParamOwner CS Origin       = <%p>'%s'\n", paramOwnerCS,
       paramOwnerCS ? paramOwnerCS->GetOriginName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   Parameter CS Dependent     = %s\n", (mParamDepObj == GmatParam::COORD_SYS) ? "true" : "false");
   MessageInterface::ShowMessage
      ("   Parameter Origin Dependent = %s\n", (mParamDepObj == GmatParam::ORIGIN) ? "true" : "false");
   MessageInterface::ShowMessage
      ("   Parameter Settable         = %s\n", mIsParamSettable ? "true" : "false");
   MessageInterface::ShowMessage
      ("   current cart state         = \n      %s", GetCartState().ToString().c_str());
   MessageInterface::ShowMessage
      ("   mOrigin = <%p>'%s', mGravConst = %f, mFlattening = %f, mEqRadius = %f\n",
       mOrigin, mOrigin ? mOrigin->GetName().c_str() : "NULL", mGravConst, mFlattening, mEqRadius);
   #endif
}
