//$Id$
//------------------------------------------------------------------------------
//                                  OrbitData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
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
//#define DEBUG_ORBITDATA_GET
//#define DEBUG_REF_OBJECT
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

//------------------------------------------------------------------------------
// OrbitData(const std::string &name = "", const Gmat::ObjectType paramOwnerType = Gmat::SPACECRAFT)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
OrbitData::OrbitData(const std::string &name, const std::string &typeName,
                     Gmat::ObjectType paramOwnerType,
                     GmatParam::DepObject depObj, bool isSettable)
   : RefData(name, typeName, paramOwnerType, depObj, isSettable),
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
      ("\nOrbitData::SetReal() '%s' entered, item=%d, rval=%f\n",
       mActualParamName.c_str(), item, rval);
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
   DebugWriteData(paramOwnerCS);
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
      }
   }
   
   bool convertCSBeforeSet = false;
   
   // Check for different coordinate system for Spacecraft owend Parameter(2013.03.28)
   if (!mIsParamOriginDep && mParameterCS != NULL && paramOwnerCS != NULL)
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("mParameter = <%p>'%s'\n", mParameter, mParameter ? mParameter->GetName().c_str() : "NULL");
      #endif
      
      // Do validation on coordinate system (for GMT-4512 fix)
      if (mParameter)
      {
         bool reqBodyFixedCS = mParameter->RequiresBodyFixedCS();
         bool reqCelesBodyOrigin  = mParameter->RequiresCelestialBodyCSOrigin();
         #ifdef DEBUG_ORBITDATA_SET
         MessageInterface::ShowMessage("RequiresBodyFixedCS = %d\n", reqBodyFixedCS);
         MessageInterface::ShowMessage("RequiresCelestialBodyCSOrigin = %d\n", reqCelesBodyOrigin);
         #endif
         if (reqBodyFixedCS)
         {
            StringArray typeNames = mParameterCS->GetAxisSystem()->GetTypeNames();
            #ifdef DEBUG_ORBITDATA_SET
            MessageInterface::ShowMessage("parameterCSAxis type names are:\n");
            for (UnsignedInt i = 0; i < typeNames.size(); i++)
               MessageInterface::ShowMessage("   typeNames[%d] = %s\n", i, typeNames[i].c_str());
            #endif
            if ((mParameterCS->GetAxisSystem())->IsOfType("BodyFixedAxes"))
            {
               #ifdef DEBUG_ORBITDATA_SET
               MessageInterface::ShowMessage("Parameter is in BodyFixed CS\n");
               #endif
            }
            else
            {
               ParameterException pe;
               pe.SetDetails("The %s is only defined for coordinate systems with BodyFixed axes",
                             mParameter->GetParameterClassType().c_str());
               throw pe;
            }
         }
         if (reqCelesBodyOrigin)
         {
            StringArray typeNames = mParameterCS->GetOrigin()->GetTypeNames();
            #ifdef DEBUG_ORBITDATA_SET
            MessageInterface::ShowMessage("parameterCSOrigin type names are:\n");
            for (UnsignedInt i = 0; i < typeNames.size(); i++)
               MessageInterface::ShowMessage("   typeNames[%d] = %s\n", i, typeNames[i].c_str());
            MessageInterface::ShowMessage
               ("   CS origin = <%p><%s>'%s'\n", mParameterCS->GetOrigin(),
                (mParameterCS->GetOrigin())->GetTypeName().c_str(),
                (mParameterCS->GetOrigin())->GetName().c_str());
            #endif
            if ((mParameterCS->GetOrigin())->IsOfType("CelestialBody"))
            {
               #ifdef DEBUG_ORBITDATA_SET
               MessageInterface::ShowMessage("Parameter is in CS with CelestialBody origin\n");
               #endif
            }
            else
            {
               ParameterException pe;
               pe.SetDetails("The %s is only defined for coordinate systems with "
                             "BodyFixed axes with CelestialBody origin",
                             mParameter->GetParameterClassType().c_str());
               throw pe;
            }
         }
      }
      
      
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
            pe.SetDetails("The Parameter %s is not settable", mActualParamName.c_str());
            throw pe;
         }
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
      Rvector6 cartStateInternal = cartStateInParamOrigin + mOrigin->GetMJ2000State(mCartEpoch);
      
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   cartStateInParamOrigin = \n   %s\n", cartStateInParamOrigin.ToString().c_str());
      MessageInterface::ShowMessage
         ("   cartStateInternal = \n   %s\n", cartStateInternal.ToString().c_str());
      #endif
      
      // Need to set whole state in internal CS
      mSpacecraft->SetState(cartStateInternal);
      
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
      
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   cartStateInParamCS= \n   %s\n", cartStateInParamCS.ToString().c_str());
      #endif
      
      // Convert to internal CS
      mCoordConverter.Convert(A1Mjd(currEpoch), cartStateInParamCS, mParameterCS,
                              cartStateNew, mInternalCS, false);
      
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   from paramCS to internalCS = \n   %s\n", cartStateNew.ToString().c_str());
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
/**
 * If Parameter is coordinate system dependent, it returns the spacecraft's last
 * cartesian state after converting to Parameter coordinate system.  If Parameter
 * is origin dependent, it returns the last state of spacecraft without conversion.
 */
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetCartState()
{
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetCartState() '%s' entered\n", mActualParamName.c_str());
   DebugWriteRefObjInfo();
   MessageInterface::ShowMessage
      ("   mParamOwnerType = %d, mParamTypeName=%s, mParamOwnerName=%s, mParamDepName=%s\n",
       mParamOwnerType, mParamTypeName.c_str(), mParamOwnerName.c_str(), mParamDepName.c_str());
   #endif
   
   if (mSolarSystem == NULL || mSpacePoint == NULL ||
      (mSpacePoint->IsOfType("Spacecraft") && mSpacecraft == NULL))
      InitializeRefObjects();
   
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("   InitializeRefObjects done ...\n");
   DebugWriteRefObjInfo();
   #endif
   
   mCartEpoch = mSpacePoint->GetEpoch();
   Rvector6 lastCartState = mSpacePoint->GetLastState();
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("   epoch from spacepoint is %le\n", mCartEpoch);
   MessageInterface::ShowMessage
      ("   state from spacepoint is %s\n", lastCartState.ToString().c_str());
   #endif
   
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("   GetCartState() '%s' lastCartState=\n   %s\n",
       mSpacePoint->GetName().c_str(), lastCartState.ToString().c_str());
   MessageInterface::ShowMessage
      ("   mInternalCS = <%p><%s>'%s', mParameterCS = <%p><%s>'%s'\n", mInternalCS,
       mInternalCS ? mInternalCS->GetTypeName().c_str() : "NULL",
       mInternalCS ? mInternalCS->GetName().c_str() : "NULL", mParameterCS,
       mParameterCS ? mParameterCS->GetTypeName().c_str() : "NULL",
       mParameterCS ? mParameterCS->GetName().c_str() : "NULL");
   #endif
   
   // if origin dependent parameter, the relative position/velocity is computed in
   // the parameter calculation, so just return prop state.
   if (mIsParamOriginDep)
   {
      #ifdef DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage
         ("OrbitDate::GetCartState() '%s' returning lastCartState \n   %s\n   "
          "since it is origin dep Parameter\n", mActualParamName.c_str(),
          lastCartState.ToString().c_str());
      #endif
      return lastCartState;
   }
   
   if (mInternalCS == NULL || mParameterCS == NULL)
   {
      MessageInterface::ShowMessage
         ("OrbitData::GetCartState() Internal CoordSystem or Output CoordSystem is NULL.\n");
      
      throw ParameterException
         ("OrbitData::GetCartState() internal or output CoordinateSystem is NULL.\n");
   }
   
   //-----------------------------------------------------------------
   // Convert to Parameter CoordinateSystem
   //-----------------------------------------------------------------
   if (mInternalCS->GetName() != mParameterCS->GetName())
   {
      #ifdef DEBUG_ORBITDATA_CONVERT
      MessageInterface::ShowMessage
         ("   GetCartState() mParameterCS: %s(%s), Axis addr=<%p>\n",
          mParameterCS->GetName().c_str(),
          mParameterCS->GetTypeName().c_str(),
          mParameterCS->GetRefObject(Gmat::AXIS_SYSTEM, ""));
      if (mParameterCS->AreAxesOfType("ObjectReferencedAxes"))
         MessageInterface::ShowMessage
            ("   GetCartState() <-- mParameterCS IS of type ObjectReferencedAxes!!!\n");
      else
         MessageInterface::ShowMessage
            ("   GetCartState() <-- mParameterCS IS NOT of type ObjectReferencedAxes!!!\n");
      MessageInterface::ShowMessage
         ("   GetCartState() <-- Before convert: mCartEpoch=%f\n   "
          "state = %s\n", mCartEpoch, lastCartState.ToString().c_str());
      MessageInterface::ShowMessage
         ("   GetCartState() <-- firstTimeEpochWarning = %s\n",
          (firstTimeEpochWarning? "true" : "false"));
      #endif
      
      if ((mParameterCS->AreAxesOfType("ObjectReferencedAxes")) && !firstTimeEpochWarning)
      {
         GmatBase *objRefOrigin = mParameterCS->GetOrigin();
         if (objRefOrigin == NULL)
         {
            std::string errmsg = "OrbitData::GetCartState() Failed to convert to " ;
            errmsg += mParameterCS->GetName() + " coordinate system. \n";
            errmsg += "The origin of " + mParameterCS->GetName() + " is NULL\n";
            ParameterException pe(errmsg);
            pe.SetFatal(true);
            throw pe;
         }
         
         #ifdef DEBUG_ORBITDATA_CONVERT
         MessageInterface::ShowMessage
            ("   Origin of '%s' is '%s'\n", mParameterCS->GetName().c_str(),
             objRefOrigin->GetName().c_str());
         #endif
         
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
            MessageInterface::ShowMessage("   --> Converting from %s to %s\n",
                  mInternalCS->GetName().c_str(),
                  mParameterCS->GetName().c_str());
         #endif
         mCoordConverter.Convert(A1Mjd(mCartEpoch), lastCartState, mInternalCS,
                                 lastCartState, mParameterCS, true);
         #ifdef DEBUG_ORBITDATA_CONVERT
            MessageInterface::ShowMessage
               ("   GetCartState() --> After convert: mCartEpoch=%f\n"
                "   state = %s\n", mCartEpoch, lastCartState.ToString().c_str());
         #endif
      }
      catch (BaseException &e)
      {
         // MessageInterface::ShowMessage
         //    ("OrbitData::GetCartState() Failed to convert to %s coordinate system.\n   %s\n",
         //     mParameterCS->GetName().c_str(), e.GetFullMessage().c_str());
         std::string errmsg = "OrbitData::GetCartState() Failed to convert to " ;
         errmsg += mParameterCS->GetName() + " coordinate system.\n";
         errmsg += "Message: " + e.GetFullMessage() + "\n";
         // Set fatal to true so that caller can handle fatal exception (LOJ: 2015.01.06)
         //throw ParameterException(errmsg);
         ParameterException pe(errmsg);
         pe.SetFatal(true);
         throw pe;
      }
   }
   
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitDate::GetCartState() '%s' returning lastCartState\n   %s\n",
       mActualParamName.c_str(), lastCartState.ToString().c_str());
   #endif
   
   return lastCartState;
}


//------------------------------------------------------------------------------
// Rvector6 GetKepState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetKepState()
{
   #ifdef DEBUG_ORBITDATA_KEP_STATE
   MessageInterface::ShowMessage
      ("OrbitData::GetKepState() '%s' entered\n", mActualParamName.c_str());
   #endif
   
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Rvector6 kepState = StateConversionUtil::CartesianToKeplerian(mGravConst, state);
   
   #ifdef DEBUG_ORBITDATA_KEP_STATE
   MessageInterface::ShowMessage
      ("OrbitData::GetKepState() '%s' returning kepState=\n   %s\n",
       mActualParamName.c_str(), kepState.ToString().c_str());
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
      ("OrbitData::GetDelaState() '%s' entered\n", mActualParamName.c_str());
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
      ("OrbitData::GetDelaState() '%s' returning mDelaState=%s\n",
       mActualParamName.c_str(), mDelaState.ToString().c_str());
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
   #ifdef DEBUG_ORBITDATA_GET
   MessageInterface::ShowMessage
      ("OrbitData::GetSphRaDecState() '%s' entered\n", mActualParamName.c_str());
   #endif
   
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Rvector6 sphRaDecState = StateConversionUtil::Convert(state, "Cartesian", "SphericalRADEC",
                                                         mGravConst, mFlattening, mEqRadius);
   
   #ifdef DEBUG_ORBITDATA_STATE
   MessageInterface::ShowMessage
      ("OrbitData::GetSphRaDecState() '%s' returning, sphRaDecState=\n   %s\n",
       mActualParamName.c_str(), sphRaDecState.ToString().c_str());
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
   MessageInterface::ShowMessage("   CartSate = %s\n", state.ToString().c_str());
   MessageInterface::ShowMessage
      ("   Calling StateConversionUtil::Convert() with mGravConst=%f, "
       "mFlattening=%f, mEqRadius=%f\n", mGravConst, mFlattening, mEqRadius);
   #endif
   
   Rvector6 blShortState = StateConversionUtil::Convert(state, "Cartesian", "BrouwerMeanShort",
                                                        mGravConst, mFlattening, mEqRadius);
   
   #ifdef DEBUG_BROUWER_SHORT
   MessageInterface::ShowMessage
      ("OrbitData::GetBLshortState() returning %s\n", blSshortState.ToString().c_str());
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
      ("OrbitData::GetBLlongState() returning %s\n", blLongState.ToString().c_str());
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
      return state[item - OrbitStmCount];
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
   MessageInterface::ShowMessage("OrbitData::GetModKepReal() entered, item=%d\n", item);
   #endif
   
   Rvector6 state = GetCartState();
   
   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("   state to pass to CalculateKeplerianData() is\n   %s\n", state.ToString().c_str());
   #endif
   
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
   #ifdef DEBUG_ORBITDATA_GET
   MessageInterface::ShowMessage
      ("OrbitData::GetSphRaDecReal() '%s' entered, item=%d\n",
       mActualParamName.c_str(), item);
   #endif
   
   Rvector6 sphRaDecState = GetSphRaDecState();
   
   #ifdef DEBUG_ORBITDATA_GET
   MessageInterface::ShowMessage
      ("   GetSphRaDecReal() sphRaDecState=\n   %s\n", sphRaDecState.ToString().c_str());
   #endif
   
   switch (item)
   {
   case RADEC_RMAG:
      {
         // if orgin is "Earth" just return default
         if (mOrigin->GetName() == "Earth")
            return sphRaDecState[RADEC_RMAG-KepCount];
         else
            return GetPositionMagnitude(mOrigin);
      }
   case RADEC_RA:
   case RADEC_DEC:
   case RADEC_VMAG:
   case RADEC_RAV:
   case RADEC_DECV:
   {
      #ifdef DEBUG_ORBITDATA_GET
      MessageInterface::ShowMessage
         ("OrbitData::GetSphRaDecReal() '%s' returning %f\n",
          mActualParamName.c_str(), sphRaDecState[item-KepCount]);
      #endif
      
      return sphRaDecState[item-KepCount];
   }
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
   MessageInterface::ShowMessage
      ("OrbitData::GetSphAzFpaReal() '%s' entered, item=%d\n", mActualParamName.c_str(), item);
   #endif
   
   Rvector6 sphAzFpaState = GetSphAzFpaState();
   
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetSphAzFpaReal() sphAzFpaState=%s\n",
       sphAzFpaState.ToString().c_str());
   #endif
   
   if (item >= AZIFPA_RMAG && item <= AZIFPA_FPA)
   {
      #ifdef DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage
         ("OrbitData::GetSphAzFpaReal() '%s' returning %f\n",
          mActualParamName.c_str(), sphAzFpaState[item-RaDecCount]);
      #endif
      return sphAzFpaState[item-RaDecCount];
   }
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
   case EQ_K:
   case EQ_P:
   case EQ_Q:
   case EQ_MLONG:
      {
         Rvector6 equiState = GetEquinState();
         return equiState[item - AziFpaCount];
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
         ("*** Error *** Cannot convert from Cartesian to ModifiedEquinoctial "
          "because position is a zero " + mActualParamName);
   
   switch (item)
   {
   case MOD_EQ_P:
   case MOD_EQ_F:
   case MOD_EQ_G:
   case MOD_EQ_H:
   case MOD_EQ_K:
   case MOD_EQ_TLONG:
      {
         Rvector6 modequiState = GetModEquinState();
         return modequiState[item - EquinCount];
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
   case ALT_EQ_SMA:
   case ALT_EQ_H:
   case ALT_EQ_K:
   case ALT_EQ_P:
   case ALT_EQ_Q:
   case ALT_EQ_MLONG:
   {
      Rvector6 altequiState = GetAltEquinState();
      #ifdef DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage("   altequiState = %s\n", altequiState.ToString().c_str());
      #endif
      return altequiState[item - ModEquinCount];
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
   case DEL_DELA_SG:
   case DEL_DELA_SH:
   case DEL_DELA_L:
   case DEL_DELA_G:
   case DEL_DELA_H:
      {
         Rvector6 delaState = GetDelaState();
         return delaState[item - AltEquinCount];
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
   case PLD_LON:
   case PLD_LAT:
   case PLD_VMAG:
   case PLD_AZI:
   case PLD_HFPA:
      {
         Rvector6 PlanetodeticState = GetPlanetodeticState();
         return PlanetodeticState[item - DelaCount];
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
   MessageInterface::ShowMessage("OrbitData::GetIncAsymReal() entered, item=%d\n", item);
   #endif
    
   Rvector6 state = GetCartState();
   
   if (mIsParamOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("   state = %s\n", state.ToString().c_str());
   #endif
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < GmatOrbitConstants::KEP_ZERO_TOL)
      throw ParameterException
         ("*** Error *** Cannot convert from Cartesian to IncomingAsymptote because position vector is a zero vector.");
   
   Rvector6 incAsymState =
      StateConversionUtil::Convert(state, "Cartesian", "IncomingAsymptote",
                                   mGravConst, mFlattening, mEqRadius);
   
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("   incAsymState = %s\n", incAsymState.ToString().c_str());
   #endif
   
   switch (item)
   {
   case INCASYM_RADPER:
   case INCASYM_C3_ENERGY:
   case INCASYM_RHA:
   case INCASYM_DHA:
   case INCASYM_BVAZI:   
   case INCASYM_TA:
   {
      return incAsymState[item - PlanetoCount];
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
   
   Rvector6 outAsymState =
      StateConversionUtil::Convert(state, "Cartesian", "OutgoingAsymptote",
                                   mGravConst, mFlattening, mEqRadius);
   
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage("   outAsymState = %s\n", outAsymState.ToString().c_str());
   #endif

   switch (item)
   {
   case OUTASYM_RADPER:
   case OUTASYM_C3_ENERGY:
   case OUTASYM_RHA:
   case OUTASYM_DHA:
   case OUTASYM_BVAZI:
   case OUTASYM_TA:
   {
      #ifdef DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage("   element index = %d\n", item-InAsymCount);
      #endif
      return outAsymState[item - InAsymCount];
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
   case BLS_ECC:
   case BLS_INC:
   case BLS_RAAN:
   case BLS_AOP:
   case BLS_MA:
   {
      Rvector6 BLshortState = GetBLshortState();
      return BLshortState[item - OutAsymCount];
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
   case BLL_ECC:
   case BLL_INC:
   case BLL_RAAN:
   case BLL_AOP:
   case BLL_MA:
   {
      Rvector6 BLlongState = GetBLlongState();
      return BLlongState[item - BrouwerShortCount];
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
//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* OrbitData::GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name)
{
   #ifdef DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("OrbitData::GetRefObject() <%p>'%s' entered, type=%d, name='%s'\n", this,
       mActualParamName.c_str(), type, name.c_str());
   #endif
   
   try
   {
      GmatBase* theObj = RefData::GetRefObject(type, name);
      #ifdef DEBUG_REF_OBJECT
      MessageInterface::ShowMessage
         ("OrbitData::GetRefObject() <%p>'%s' returning %s\n", this, mActualParamName.c_str(),
          GmatBase::WriteObjectInfo("", theObj).c_str());
      #endif
      return theObj;
   }
   catch (ParameterException &pe)
   {
      // if the type was SpacePoint, we may need to look in the Spacecraft
      // list (or vice versa?), since we are looking for a Spacecraft
      // and a Spacecraft is a SpacePoint
      Gmat::ObjectType altType = type;
      GmatBase *refObj = NULL;
      if (type == Gmat::SPACE_POINT) altType = Gmat::SPACECRAFT;
      for (int i=0; i<mNumRefObjects; i++)
      {
         if (mRefObjList[i].objType == altType)
         {
            if (name == "") //if name is "", return first object
            {
               refObj = mRefObjList[i].obj;
               #ifdef DEBUG_REF_OBJECT
               MessageInterface::ShowMessage
                  ("OrbitData::GetRefObject() <%p>'%s' returning %s\n", this,
                   mActualParamName.c_str(),
                   GmatBase::WriteObjectInfo("", refObj).c_str());
               #endif
               //return mRefObjList[i].obj;
               return refObj;
            }
            
            if ((name == "" || mRefObjList[i].objName == name) &&
                (mRefObjList[i].obj)->IsOfType("Spacecraft"))
            {
               //Notes: will return first object name.
               #ifdef DEBUG_REF_OBJECT
               MessageInterface::ShowMessage
                  ("---> OrbitData::GetRefObject() <%p>'%s' altType=%d returning: %s\n",
                   this, mActualParamName.c_str(), altType, mRefObjList[i].objName.c_str());
               #endif
               return mRefObjList[i].obj;
            }
         }
      }
      
      #ifdef DEBUG_REF_OBJECT
      MessageInterface::ShowMessage
         ("OrbitData::GetRefObject() <%p>'%s' rethrowing exception: %s\n",
          this, mActualParamName.c_str(), pe.GetFullMessage().c_str());
      #endif
      
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
   #ifdef DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("OrbitData::SetRefObject() <%p>'%s' entered, obj=<%p>[%s]'%s', type=%d, "
       "name='%s'\n", this, mActualParamName.c_str(), obj,
       obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", type, name.c_str());
   #endif
   
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
   
   mSpacePoint = (SpacePoint*)FindObject(mParamOwnerType, mParamOwnerName);
   
   #ifdef DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage("   mSpacePoint=<%p>\n", mSpacePoint);
   #endif
   
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
   // If dependent body name exist and it is a CelestialBody,
   // it is origin dependent parameter, set new gravity constant.
   //-----------------------------------------------------------------
      
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
            ("OrbitData::InitializeRefObjects() The origin '%s' of CS '%s' is NULL, "
             "so try to find it again...\n",
             mParameterCS->GetOriginName().c_str(), mParameterCS->GetName().c_str());
         #endif
         
         // Try to find origin again for GmatFunction (LOJ: 2014.01.12)
         mOrigin = (SpacePoint*)FindObject(Gmat::SPACE_POINT, mParameterCS->GetOriginName());
         
         if (!mOrigin)
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
         ("OrbitData::GetPositionMagnitude()\n    scPos=%s\n   originPos=%s\n   relPos=%s\n",
          scPos.ToString().c_str(), originPos.ToString().c_str(),
          relPos.ToString().c_str());
   #endif
   
   return relPos.GetMagnitude();
}


//------------------------------------------------------------------------------
// Rvector6 GetRelativeCartState(SpacePoint *origin)
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
   
   // Relative state
   Rvector6 relState = scState - originState;
   
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetRelativeCartState() origin = %s\n   satState = %s\n   orgState = %s\n"
       "   relState = %s\n", origin->GetName().c_str(), scState.ToString().c_str(),
       originState.ToString().c_str(), relState.ToString().c_str());
   #endif
   
   // return relative state
   return relState;
}


//------------------------------------------------------------------------------
// Rvector6 GetCartStateInParameterCS(Integer item, Real rval)
//------------------------------------------------------------------------------
/**
 * Returns cartesian state in Parameter coordinate system after input element
 * is set to state. If input element is not in cartesian state rep, it converts
 * to Parameter state rep and set the element value and converts it back to
 * cartesian state.
 */
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetCartStateInParameterCS(Integer item, Real rval)
{
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::GetCartStateInParameterCS() '%s' entered, item=%d, rval=%f\n"
       "   Now converting to Parameter CS before setting value\n", 
       mActualParamName.c_str(), item, rval);
   #endif
   
   // GetCartState() returns state in Parameter CS, so no conversion is needed
   Rvector6 cartStateInParamCS = GetCartState();
   Rvector6 stateInParamCS;
   Real currEpoch = mSpacePoint->GetEpoch();
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("   cartStateInParamCS = \n   %s\n", cartStateInParamCS.ToString().c_str());
   MessageInterface::ShowMessage
      ("   paramCsOriginState = \n   %s\n", mOrigin->GetMJ2000State(mCartEpoch).ToString().c_str());
   #endif
   
   if (item >= CART_X && item <= CART_VZ)
   {
      // Now set new value to state
      cartStateInParamCS(item - OrbitStmCount) = rval;
   }
   else if (item >= KEP_SMA && item <= KEP_TA)
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to Keplerian for items: KEP_SMA .. KEP_TA\n");
      #endif
      stateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "Keplerian",
                                      mGravConst, mFlattening, mEqRadius);
      stateInParamCS[item - CartCount] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(stateInParamCS, "Keplerian", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= RADEC_RMAG && item <= RADEC_DECV)
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to SphericalRADEC for items: RADEC_RMAG .. RADEC_DECV\n");
      #endif
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting Spherical RADEC to Cartesian, element index = %d\n", item-KepCount);
      #endif
      
      stateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "SphericalRADEC",
                                      mGravConst, mFlattening, mEqRadius);
      
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Before setting: stateInParamCS = \n   %s\n", stateInParamCS.ToString().c_str());
      #endif
      
      stateInParamCS[item - KepCount] = rval;
      
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   After  setting: stateInParamCS = \n   %s\n", stateInParamCS.ToString().c_str());
      #endif
      
      cartStateInParamCS =
         StateConversionUtil::Convert(stateInParamCS, "SphericalRADEC", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
      
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   cartStateInParamCS = %s\n", cartStateInParamCS.ToString().c_str());
      #endif
   }
   else if (item >= AZIFPA_RMAG && item <= AZIFPA_FPA)
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to SphericalAZFPA for items: AZIFPA_RMAG .. AZIFPA_FPA\n");
      #endif
      stateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "SphericalAZFPA",
                                      mGravConst, mFlattening, mEqRadius);
      stateInParamCS[item-RaDecCount] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(stateInParamCS, "SphericalAZFPA", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= EQ_SMA && item <= EQ_MLONG)
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to Equinoctial for items: EQ_SMA .. EQ_MLONG\n");
      #endif
      stateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "Equinoctial",
                                      mGravConst, mFlattening, mEqRadius);
      stateInParamCS[item - AziFpaCount] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(stateInParamCS, "Equinoctial", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= MOD_EQ_P && item <= MOD_EQ_TLONG)
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting ModifiedEquinoctial to for items: MOD_EQ_P .. MOD_EQ_TLONG\n");
      #endif
      stateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "ModifiedEquinoctial",
                                      mGravConst, mFlattening, mEqRadius);
      stateInParamCS[item - EquinCount] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(stateInParamCS, "ModifiedEquinoctial", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= ALT_EQ_SMA && item <= ALT_EQ_MLONG)
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to AlternateEquinoctial for items: ALT_EQ_SMA .. ALT_EQ_MLONG\n");
      #endif
      stateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "AlternateEquinoctial",
                                      mGravConst, mFlattening, mEqRadius);
      stateInParamCS[item - ModEquinCount] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(stateInParamCS, "AlternateEquinoctial", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= DEL_DELA_SL && item <= DEL_DELA_H)
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to Delaunay for items:  DEL_DELA_SL .. DEL_DELA_H\n");
      #endif
      stateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "Delaunay",
                                      mGravConst, mFlattening, mEqRadius);
      stateInParamCS[item - AltEquinCount] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(stateInParamCS, "Delaunay", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= PLD_RMAG && item <= PLD_HFPA)
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to Planetodetic for items: PLD_RMAG .. PLD_HFPA\n");
      #endif
      stateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "Planetodetic",
                                      mGravConst, mFlattening, mEqRadius);
      stateInParamCS[item - DelaCount] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(stateInParamCS, "Planetodetic", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= INCASYM_RADPER && item <= INCASYM_TA )
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to IncomingAsymptote for items: INCASYM_RADPER .. INCASYM_TA\n");
      #endif
      stateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "IncomingAsymptote",
                                      mGravConst, mFlattening, mEqRadius);
      stateInParamCS[item - PlanetoCount] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(stateInParamCS, "IncomingAsymptote", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= OUTASYM_RADPER && item <= OUTASYM_TA )
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to OutgoingAsymptote for items: OUTASYM_RADPER .. OUTASYM_TA\n");
      #endif
      stateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "OutgoingAsymptote",
                                      mGravConst, mFlattening, mEqRadius);
      stateInParamCS[item - InAsymCount] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(stateInParamCS, "OutgoingAsymptote", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= BLS_SMA && item <= BLS_MA )
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to BrouwerMeanShort for items: BLS_SMA .. BLS_MA\n");
      #endif
      stateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "BrouwerMeanShort",
                                      mGravConst, mFlattening, mEqRadius);
      stateInParamCS[item - OutAsymCount] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(stateInParamCS, "BrouwerMeanShort", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else if (item >= BLL_SMA && item <= BLL_MA )
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to BrouwerMeanLong for items: BLL_SMA .. BLL_MA\n");
      #endif
      stateInParamCS =
         StateConversionUtil::Convert(cartStateInParamCS, "Cartesian", "BrouwerMeanLong",
                                      mGravConst, mFlattening, mEqRadius);
      stateInParamCS[item - BrouwerShortCount] = rval;
      cartStateInParamCS =
         StateConversionUtil::Convert(stateInParamCS, "BrouwerMeanLong", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
   }
   else
   {
      // Unimplemented state types should get here, so throw a exception
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("OrbitData::GetCartStateInParameterCS() throwing exception\n"
          "Setting the Parameter %s has not been implemented\n", mActualParamName.c_str());
      #endif
      ParameterException pe;
      pe.SetDetails("Setting the Parameter %s has not been implemented)", mActualParamName.c_str());
      throw pe;
   }
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::GetCartStateInParameterCS() '%s' returning\n   %s\n",
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
       "   Now converting to Parameter Origin before setting value\n", 
       mActualParamName.c_str(), item, rval);
   #endif
   
   Rvector6 cartStateInParamOrigin = GetRelativeCartState(mOrigin);
   Rvector6 stateInParamOrigin;
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("   Setting new value %f to Parameter that has different Origin than "
       "Parameter Owner CS Origin\n   cartStateInParamOrigin = \n   %s\n", rval,
       cartStateInParamOrigin.ToString().c_str());
   #endif
   
   switch (item)
   {
   case KEP_SMA:
   case KEP_ECC:
   case KEP_TA:
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to Keplerian for items: KEP_SMA, KEP_ECC, KEP_TA\n");
      #endif
      stateInParamOrigin =
         StateConversionUtil::Convert(cartStateInParamOrigin, "Cartesian", "Keplerian",
                                      mGravConst, mFlattening, mEqRadius);
      stateInParamOrigin[item - CartCount] = rval;
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   keplStateInParamOrigin = \n   %s\n", stateInParamOrigin.ToString().c_str());
      #endif
      cartStateInParamOrigin =
         StateConversionUtil::Convert(stateInParamOrigin, "Keplerian", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);      
      break;
   }
   case MODKEP_RADPER:
   case MODKEP_RADAPO:
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to ModifiedKeplerian for items: MODKEP_RADPER, MODKEP_RADAPO\n");
      #endif
      stateInParamOrigin =
         StateConversionUtil::Convert(cartStateInParamOrigin, "Cartesian", "ModifiedKeplerian",
                                      mGravConst, mFlattening, mEqRadius);
      // ModKeplerian elements: RadPer, RadApo, INC, RAAN, AOP, TA
      if (item == MODKEP_RADPER)
         stateInParamOrigin[0] = rval;
      else
         stateInParamOrigin[1] = rval;
      cartStateInParamOrigin =
         StateConversionUtil::Convert(stateInParamOrigin, "ModifiedKeplerian", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
      break;
   }
   case INCASYM_RADPER:
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to IncomingAsymptote for items: INCASYM_RADPER\n");
      #endif
      stateInParamOrigin =
         StateConversionUtil::Convert(cartStateInParamOrigin, "Cartesian", "IncomingAsymptote",
                                      mGravConst, mFlattening, mEqRadius);
      // IncomingAsymptote elements:
      // IncomingRadPer, IncomingC3Energy, IncRHA, IncDHA, IncBVAZI, TA
      stateInParamOrigin[0] = rval;
      cartStateInParamOrigin =
         StateConversionUtil::Convert(stateInParamOrigin, "IncomingAsymptote", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
      break;
   }
   case OUTASYM_RADPER:
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to IncomingAsymptote for items: OUTASYM_RADPER\n");
      #endif
      stateInParamOrigin =
         StateConversionUtil::Convert(cartStateInParamOrigin, "Cartesian", "OutgoingAsymptote",
                                      mGravConst, mFlattening, mEqRadius);
      // OutgoingAsymptote elements:
      // OutgoingRadPer, OutgoingC3Energy, IncRHA, IncDHA, IncBVAZI, TA
      stateInParamOrigin[0] = rval;
      cartStateInParamOrigin =
         StateConversionUtil::Convert(stateInParamOrigin, "OutgoingAsymptote", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
      break;
   }
   case INCASYM_C3_ENERGY:
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to IncomingAsymptote for item: INCASYM_C3_ENERGY\n");
      #endif
      stateInParamOrigin =
         StateConversionUtil::Convert(cartStateInParamOrigin, "Cartesian", "IncomingAsymptote",
                                      mGravConst, mFlattening, mEqRadius);
      // IncomingAsymptote state elements:
      // IncomingRadPer, IncomingC3Energy, IncomingRHA, IncomingDHA, IncomingBVAZI, TA 
      stateInParamOrigin[1] = rval;
      cartStateInParamOrigin =
         StateConversionUtil::Convert(stateInParamOrigin, "IncomingAsymptote", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
      break;
   }
   case OUTASYM_C3_ENERGY:
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to OutgoingAsymptote for item: OUTASYM_C3_ENERGY\n");
      #endif
      stateInParamOrigin =
         StateConversionUtil::Convert(cartStateInParamOrigin, "Cartesian", "OutgoingAsymptote",
                                      mGravConst, mFlattening, mEqRadius);
      // OutgoingAsymptote state elements:
      // OutgoingRadPer, OutgoingC3Energy, OutgoingRHA, OutgoingDHA, OutgoingBVAZI, TA 
      stateInParamOrigin[1] = rval;
      cartStateInParamOrigin =
         StateConversionUtil::Convert(stateInParamOrigin, "OutgoingAsymptote", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
      break;
   }
   case RADEC_RMAG:
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to SphericalRADEC for item: RADEC_RMAG\n");
      #endif
      stateInParamOrigin =
         StateConversionUtil::Convert(cartStateInParamOrigin, "Cartesian", "SphericalRADEC",
                                      mGravConst, mFlattening, mEqRadius);
      // SphericalRADEC elements: RMAG, RA, DEC, VMAG, RAV, DECV
      stateInParamOrigin[0] = rval;
      cartStateInParamOrigin =
         StateConversionUtil::Convert(stateInParamOrigin, "SphericalRADEC", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);
      break;
   }
   case SEMILATUS_RECTUM:
   {
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Converting to ModifiedEquinoctial for item: SEMILATUS_RECTUM\n");
      #endif
      stateInParamOrigin =
         StateConversionUtil::Convert(cartStateInParamOrigin, "Cartesian", "ModifiedEquinoctial",
                                      mGravConst, mFlattening, mEqRadius);
      // ModifiedEquinoctial state elements:
      // SemilatusRectum, ModEquinoctialF, ModEquinoctialG, ModEquinoctialH, ModEquinoctialK, TLONG
      stateInParamOrigin[0] = rval;
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   modEqStateInParamOrigin = \n   %s\n", stateInParamOrigin.ToString().c_str());
      #endif
      cartStateInParamOrigin =
         StateConversionUtil::Convert(stateInParamOrigin, "ModifiedEquinoctial", "Cartesian",
                                      mGravConst, mFlattening, mEqRadius);

      // Try converting back to ModifiedEquinoctial
      Rvector modEquinState =
         StateConversionUtil::Convert(cartStateInParamOrigin, "Cartesian", "ModifiedEquinoctial",
                                                          mGravConst, mFlattening, mEqRadius);
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage("   modEquinState = \n   %s\n", modEquinState.ToString().c_str());
      #endif
      
      break;
   }
   default:
   {
      // Unimplemented state types should get here, so throw a exception
      
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("OrbitData::GetCartStateInParameterOrigin() throwing exception\n"
          "Setting the Parameter %s has not been implemented\n", mActualParamName.c_str());
      #endif
      ParameterException pe;
      pe.SetDetails("Setting the Parameter %s has not been implemented", mActualParamName.c_str());
      throw pe;
   }
   }
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::GetCartStateInParameterOrigin() '%s' returning\n   %s\n",
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
      mSpacecraft->SetRealParameter("X", rval);
      break;
   case CART_Y:
      mSpacecraft->SetRealParameter("Y", rval);
      break;
   case CART_Z:
      mSpacecraft->SetRealParameter("Z", rval);
      break;
   case CART_VX:
      mSpacecraft->SetRealParameter("VX", rval);
      break;
   case CART_VY:
      mSpacecraft->SetRealParameter("VY", rval);
      break;
   case CART_VZ:
      mSpacecraft->SetRealParameter("VZ", rval);
      break;
      
   // Keplerian
   case KEP_SMA:
      mSpacecraft->SetRealParameter("SMA", rval);
      break;
   case KEP_ECC:
      mSpacecraft->SetRealParameter("ECC", rval);
      break;
   case KEP_INC:
      mSpacecraft->SetRealParameter("INC", rval);
      break;
   case KEP_RAAN:
      mSpacecraft->SetRealParameter("RAAN", rval);
      break;
   case KEP_AOP:
      mSpacecraft->SetRealParameter("AOP", rval);
      break;
   case KEP_TA:
      mSpacecraft->SetRealParameter("TA", rval);
      break;
   case KEP_MA:
      mSpacecraft->SetRealParameter("MA", rval);
      break;
   case KEP_EA:
      mSpacecraft->SetRealParameter("EA", rval);
      break;
   case KEP_HA:
      mSpacecraft->SetRealParameter("HA", rval);
      break;

   // ModifiedKeplerian
   case MODKEP_RADAPO:
      mSpacecraft->SetRealParameter("RadApo", rval);
      break;
   case MODKEP_RADPER:
      mSpacecraft->SetRealParameter("RadPer", rval);
      break;

   // Spherical RADEC
   case RADEC_RMAG:
      mSpacecraft->SetRealParameter("RMAG", rval);
      break;
   case RADEC_RA:
      mSpacecraft->SetRealParameter("RA", rval);
      break;
   case RADEC_DEC:
      mSpacecraft->SetRealParameter("DEC", rval);
      break;
   case RADEC_VMAG:
      mSpacecraft->SetRealParameter("VMAG", rval);
      break;
   case RADEC_RAV:
      mSpacecraft->SetRealParameter("RAV", rval);
      break;
   case RADEC_DECV:
      mSpacecraft->SetRealParameter("DECV", rval);
      break;
   
   // Spherical AZFPA
   case AZIFPA_RMAG:
      mSpacecraft->SetRealParameter("RMAG", rval);
      break;
   case AZIFPA_RA:
      mSpacecraft->SetRealParameter("RA", rval);
      break;
   case AZIFPA_DEC:
      mSpacecraft->SetRealParameter("DEC", rval);
      break;
   case AZIFPA_VMAG:
      mSpacecraft->SetRealParameter("VMAG", rval);
      break;
   case AZIFPA_AZI:
      mSpacecraft->SetRealParameter("AZI", rval);
      break;
   case AZIFPA_FPA:
      mSpacecraft->SetRealParameter("FPA", rval);
      break;

   // Equinoctial
   case EQ_SMA:
      mSpacecraft->SetRealParameter("SMA", rval);
      break;
   case EQ_H:
      mSpacecraft->SetRealParameter("EquinoctialH", rval);
      break;
   case EQ_K:
      mSpacecraft->SetRealParameter("EquinoctialK", rval);
      break;
   case EQ_P:
      mSpacecraft->SetRealParameter("EquinoctialP", rval);
      break;
   case EQ_Q:
      mSpacecraft->SetRealParameter("EquinoctialQ", rval);
      break;
   case EQ_MLONG:
      mSpacecraft->SetRealParameter("MLONG", rval);
      break;
      
   // ModifiedEquinoctial;Modified by M.H.
   case MOD_EQ_P:
   case SEMILATUS_RECTUM:
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   mSpacecraft->GetParameterID(SemilatusRectum) = %d\n",
          mSpacecraft->GetParameterID("SemilatusRectum"));
      #endif
      mSpacecraft->SetRealParameter("SemilatusRectum", rval);
      //@note Change to following since Spacecraft code using parameter ID calls
      // SetRealParameter using label anyway
      //mSpacecraft->SetRealParameter("SemilatusRectum", rval);
      break;
   case MOD_EQ_F:
      mSpacecraft->SetRealParameter("ModEquinoctialF", rval);
      break;
   case MOD_EQ_G:
      mSpacecraft->SetRealParameter("ModEquinoctialG", rval);
      break;
   case MOD_EQ_H:
      mSpacecraft->SetRealParameter("ModEquinoctialH", rval);
      break;
   case MOD_EQ_K:
      mSpacecraft->SetRealParameter("ModEquinoctialK", rval);
      break;
   case MOD_EQ_TLONG:
      mSpacecraft->SetRealParameter("TLONG", rval);
      break;
      
   // Alternate Equinoctial by HYKim
   case ALT_EQ_P:
      mSpacecraft->SetRealParameter("AltEquinoctialP", rval);
      break;
   case ALT_EQ_Q:
      mSpacecraft->SetRealParameter("AltEquinoctialQ", rval);
      break;
      
   // Delaunay;Modified by M.H.
   case DEL_DELA_SL:
      mSpacecraft->SetRealParameter("Delaunayl", rval);
      break;
   case DEL_DELA_SG:
      mSpacecraft->SetRealParameter("Delaunayg", rval);
      break;
   case DEL_DELA_SH:
      mSpacecraft->SetRealParameter("Delaunayh", rval);
      break;
   case DEL_DELA_L:
      mSpacecraft->SetRealParameter("DelaunayL", rval);
      break;
   case DEL_DELA_G:
      mSpacecraft->SetRealParameter("DelaunayG", rval);
      break;
   case DEL_DELA_H:
      mSpacecraft->SetRealParameter("DelaunayH", rval);
      break;

   // Planetodetic;Modified by M.H.
   case PLD_RMAG:
      mSpacecraft->SetRealParameter("PlanetodeticRMAG", rval);
      break;
   case PLD_LON:
      mSpacecraft->SetRealParameter("PlanetodeticLON", rval);
      break;
   case PLD_LAT:
      mSpacecraft->SetRealParameter("PlanetodeticLAT", rval);
      break;
   case PLD_VMAG:
      mSpacecraft->SetRealParameter("PlanetodeticVMAG", rval);
      break;
   case PLD_AZI:
      mSpacecraft->SetRealParameter("PlanetodeticAZI", rval);
      break;
   case PLD_HFPA:
      mSpacecraft->SetRealParameter("PlanetodeticHFPA", rval);
      break;
      
   // Incoming Asymptote State
   case INCASYM_RADPER:
      mSpacecraft->SetRealParameter("IncomingRadPer", rval);
      break;
   case INCASYM_C3_ENERGY:
      mSpacecraft->SetRealParameter("IncomingC3Energy", rval);
      break;
   case INCASYM_RHA:
      mSpacecraft->SetRealParameter("IncomingRHA", rval);
      break;
   case INCASYM_DHA:
      mSpacecraft->SetRealParameter("IncomingDHA", rval);
      break;
   case INCASYM_BVAZI:
      mSpacecraft->SetRealParameter("IncomingBVAZI", rval);
      break;
   
   // Outgoing Asymptote State
   case OUTASYM_RADPER:
      mSpacecraft->SetRealParameter("OutgoingRadPer", rval);
      break;
   case OUTASYM_C3_ENERGY:
      mSpacecraft->SetRealParameter("OutgoingC3Energy", rval);
      break;
   case OUTASYM_RHA:
      mSpacecraft->SetRealParameter("OutgoingRHA", rval);
      break;
   case OUTASYM_DHA:
      mSpacecraft->SetRealParameter("OutgoingDHA", rval);
      break;
   case OUTASYM_BVAZI:
      mSpacecraft->SetRealParameter("OutgoingBVAZI", rval);
      break;
   case OUTASYM_TA:
      mSpacecraft->SetRealParameter("TA", rval);
      break;
      
   // Brouwer-Lyddane Mean-short
   case BLS_SMA:
      mSpacecraft->SetRealParameter("BrouwerShortSMA", rval);
      break;
   case BLS_ECC:
      mSpacecraft->SetRealParameter("BrouwerShortECC", rval);
      break;
   case BLS_INC:
      mSpacecraft->SetRealParameter("BrouwerShortINC", rval);
      break;
   case BLS_RAAN:
      mSpacecraft->SetRealParameter("BrouwerShortRAAN", rval);
      break;
   case BLS_AOP:
      mSpacecraft->SetRealParameter("BrouwerShortAOP", rval);
      break;
   case BLS_MA:
      mSpacecraft->SetRealParameter("BrouwerShortMA", rval);
      break;

   // Brouwer-Lyddane Mean-long
   case BLL_SMA:
      mSpacecraft->SetRealParameter("BrouwerLongSMA", rval);
      break;
   case BLL_ECC:
      mSpacecraft->SetRealParameter("BrouwerLongECC", rval);
      break;
   case BLL_INC:
      mSpacecraft->SetRealParameter("BrouwerLongINC", rval);
      break;
   case BLL_RAAN:
      mSpacecraft->SetRealParameter("BrouwerLongRAAN", rval);
      break;
   case BLL_AOP:
      mSpacecraft->SetRealParameter("BrouwerLongAOP", rval);
      break;
   case BLL_MA:
      mSpacecraft->SetRealParameter("BrouwerLongMA", rval);
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
// void DebugWriteData(CoordinateSystem *paramOwnerCS)
//------------------------------------------------------------------------------
void OrbitData::DebugWriteData(CoordinateSystem *paramOwnerCS)
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
      ("   Cart state in Parameter CS = \n   %s\n", GetCartState().ToString().c_str());
   MessageInterface::ShowMessage
      ("   mOrigin = <%p>'%s', mGravConst = %f, mFlattening = %f, mEqRadius = %f\n",
       mOrigin, mOrigin ? mOrigin->GetName().c_str() : "NULL", mGravConst, mFlattening, mEqRadius);
   #endif
}

//------------------------------------------------------------------------------
// void DebugWriteRefObjInfo()
//------------------------------------------------------------------------------
void OrbitData::DebugWriteRefObjInfo()
{
   #ifdef DEBUG_ORBITDATA_GET
   MessageInterface::ShowMessage
      ("   mSpacePoint=<%p><%s>'%s'\n", mSpacePoint,
       mSpacePoint ? mSpacePoint->GetTypeName().c_str() : "NULL",
       mSpacePoint ? mSpacePoint->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   mSpacecraft=<%p><%s>'%s'\n", mSpacecraft,
       mSpacecraft ? mSpacecraft->GetTypeName().c_str() : "NULL",
       mSpacecraft ? mSpacecraft->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   mSolarSystem=<%p><%s>'%s'\n", mSolarSystem,
       mSolarSystem ? mSolarSystem->GetTypeName().c_str() : "NULL",
       mSolarSystem ? mSolarSystem->GetName().c_str() : "NULL");
   #endif
}
