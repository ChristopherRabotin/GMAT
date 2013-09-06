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
OrbitData::OrbitData(const std::string &name, Gmat::ObjectType paramOwnerType)
   : RefData(name, paramOwnerType),
   stateTypeId (-1)
{
   mCartState     = Rvector6::RVECTOR6_UNDEFINED;
   mKepState      = Rvector6::RVECTOR6_UNDEFINED;
   mModKepState   = Rvector6::RVECTOR6_UNDEFINED;
   mSphRaDecState = Rvector6::RVECTOR6_UNDEFINED;
   mSphAzFpaState = Rvector6::RVECTOR6_UNDEFINED;
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
   
   mCartState     = data.mCartState;
   mKepState      = data.mKepState;
   mModKepState   = data.mModKepState;
   mSphRaDecState = data.mSphRaDecState;
   mSphAzFpaState = data.mSphAzFpaState;
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
   
   mCartState     = right.mCartState;
   mKepState      = right.mKepState;
   mModKepState   = right.mModKepState;
   mSphRaDecState = right.mSphRaDecState;
   mSphAzFpaState = right.mSphAzFpaState;
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
      ("OrbitData::SetReal() entered, item=%d, rval=%f\n", item, rval);
   #endif
   
   //if (mSpacecraft == NULL)
   if (mSpacePoint == NULL)
      InitializeRefObjects();
   
   //if (mSpacecraft == NULL)
   if (mSpacePoint == NULL)
   {
      MessageInterface::ShowMessage
         //("*** INTERNAL ERROR *** Cannot find Spacecraft object so returning %f\n",
         ("*** INTERNAL ERROR *** Cannot find SpacePoint object so returning %f\n",
          GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
   }
   
   //LOJ: NEW_PARAMETER: added
   CoordinateSystem *paramOwnerCS = NULL;
   
   if ((mSpacePoint->IsOfType(Gmat::SPACECRAFT)) ||
       (mSpacePoint->IsOfType("Spacecraft")))
   {
      mSpacecraft = (Spacecraft*)mSpacePoint;
      
      //CoordinateSystem *satCS =
      paramOwnerCS = 
         (CoordinateSystem*)mSpacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, "");
   }
   else
   {
      paramOwnerCS = mInternalCS;
   }
   
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("   Parameter CS         = <%p>'%s'\n", mParameterCS,
       mParameterCS ? mParameterCS->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   ParamOwner CS        = <%p>'%s'\n", paramOwnerCS,
       paramOwnerCS ? paramOwnerCS->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   Parameter CS Origin  = <%p>'%s'\n", mParameterCS,
       mParameterCS ? mParameterCS->GetOriginName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   ParamOwner CS Origin = <%p>'%s'\n", paramOwnerCS,
       paramOwnerCS ? paramOwnerCS->GetOriginName().c_str() : "NULL");
   #endif
   
   // Check if origin is different from spacecraft CS origin
   if (mIsParamOriginDep && mOrigin)
   {
      if (paramOwnerCS->GetOriginName() != mOrigin->GetName())
      {
         ParameterException pe;
         pe.SetDetails("Currently GMAT cannot set %s; the spacecraft '%s' "
                       "requires values to be in the '%s' origin (setting "
                       "values in different origin will be implemented in "
                       "future builds)",  mActualParamName.c_str(),
                       mSpacecraft->GetName().c_str(), paramOwnerCS->GetOriginName().c_str());
         throw pe;
      }
   }

   //bool convertBeforeSet = false;
   
   // Check for different coordinate system (2013.03.28)
   //if (mParameterCS != NULL && mParameterCS != paramOwnerCS)
   if (!mIsParamOriginDep && mParameterCS != NULL && paramOwnerCS != NULL)
   {
      if (mParameterCS->GetName() != paramOwnerCS->GetName())
      {
         // Now allow setting value to Parameter that has different
         // coordinate system (GMT-4020 Fix)
         //#if 1
         ParameterException pe;
         pe.SetDetails("Currently GMAT cannot set %s; the spacecraft '%s' "
                       "requires values to be in the '%s' coordinate system (setting "
                       "values in different coordinate systems will be implemented in "
                       "future builds)",  mActualParamName.c_str(),
                       mSpacecraft->GetName().c_str(), paramOwnerCS->GetName().c_str());
         throw pe;
         //#else
         //convertBeforeSet = true;
         //#endif
      }
   }
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage("   Now calling Spacecraft::SetRealParameter()\n");
   #endif
   
   //LOJ: NEW_PARAMETER: Old code assuming parameter owner is always spacecraft
   if (mSpacePoint == NULL)
      throw ParameterException
         ("OrbitData::SetReal() Cannot set Parameter " + GmatRealUtil::ToString(item) +
          ". SpacePoint object is NULL\n");
   
   #if 0
   //=================================================================
   //@note This part of code will be continued later for GMT-4020 fix
   //=================================================================
   if (convertBeforeSet)
   {
      Real tempEpoch = mSpacePoint->GetEpoch();
      Rvector6 tempState1 = mSpacePoint->GetLastState();
      Rvector6 tempState2;
      
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("   Setting new value %f to Parameter that has different CS than Parameter Owner CS\n"
          "   currEpoch = %f\n   currState = %s", rval, tempEpoch, tempState1.ToString().c_str());
      #endif
      
      if (item >= CART_X && item <= CART_VZ)
      {
         // Convert to parameterCS
         mCoordConverter.Convert(A1Mjd(tempEpoch), tempState1, mInternalCS,
                                 tempState2, mParameterCS, false);
         
         #ifdef DEBUG_ORBITDATA_SET
         MessageInterface::ShowMessage
            ("   from internalCS to paramCS = %s", tempState2.ToString().c_str());
         #endif
         
         // Now set new value to state
         tempState2(item) = rval;
         
         // Convert to internal CS
         mCoordConverter.Convert(A1Mjd(tempEpoch), tempState2, mParameterCS,
                                 tempState1, mInternalCS, false);
         
         #ifdef DEBUG_ORBITDATA_SET
         MessageInterface::ShowMessage
            ("   from paramCS to internalCS = %s", tempState1.ToString().c_str());
         #endif
         
         // Need to set whole state in internal CS
         mSpacecraft->SetState(tempState1);
         
         #ifdef DEBUG_ORBITDATA_SET
         MessageInterface::ShowMessage
            ("OrbitData::SetReal() leaving, item set to non-ParameterOwner CS, item=%d, rval=%f\n", item, rval);
         #endif
         return;
      }
   }
   //=================================================================
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
      mSpacecraft->SetRealParameter(mSpacecraft->GetParameterID("SemiLatusRectum"), rval);
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


   default:
      #ifdef DEBUG_ORBITDATA_SET
      MessageInterface::ShowMessage
         ("**** ERROR **** OrbitData::SetReal() Unknown item id: %d\n", item);
      #endif
      throw ParameterException("OrbitData::SetReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
   
   #ifdef DEBUG_ORBITDATA_SET
   MessageInterface::ShowMessage
      ("OrbitData::SetReal() leaving, item=%d, rval=%f\n", item, rval);
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
   mCartState = mSpacePoint->GetLastState();
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("----- epoch from spacepoint is %le\n", mCartEpoch);
   MessageInterface::ShowMessage
      ("----- state from spacepoint is %s\n", mCartState.ToString().c_str());
   #endif
   
   #ifdef DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetCartState() '%s' mCartState=\n   %s\n",
       mSpacePoint->GetName().c_str(), mCartState.ToString().c_str());
   MessageInterface::ShowMessage("   mInternalCS is %s, mParameterCS is %s\n",
         (mInternalCS? "NOT NULL" : "NULL"),  (mParameterCS? "NOT NULL" : "NULL"));
   MessageInterface::ShowMessage("   mInternalCS = <%p>, mOutCS = <%p>\n",
         mInternalCS, mParameterCS);
   #endif
   
   // if origin dependent parameter, the relative position/velocity is computed in
   // the parameter calculation, so just return prop state.
   if (mIsParamOriginDep)
      return mCartState;
   
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
                  "state = %s\n", mCartEpoch, mCartState.ToString().c_str());
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
//            if (objRefScName != mSpacecraft->GetName())
            if (objRefScName != mSpacePoint->GetName())
            {
               // Get the epochs of the spacecraft to see if they are different
//               Real scEpoch   = mSpacecraft->GetRealParameter("A1Epoch");
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
//                  errmsg += mSpacecraft->GetName() + "\" and \"";
                  errmsg += mSpacePoint->GetName() + "\" and \"";
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
                  mInternalCS->GetName().c_str(),
                  mParameterCS->GetName().c_str());
         #endif
         mCoordConverter.Convert(A1Mjd(mCartEpoch), mCartState, mInternalCS,
                                 mCartState, mParameterCS, true);
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
             mParameterCS->GetName().c_str(), e.GetFullMessage().c_str());
         std::string errmsg = "OrbitData::GetCartState() Failed to convert to " ;
         errmsg += mParameterCS->GetName() + " coordinate system.\n";
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
   //LOJ: NEW_PARAMETER: Using mSpacecraft here, no change
   
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();

   #ifdef DEBUG_ORBITDATA_STATE
   MessageInterface::ShowMessage("rbitData::GetKepState() from SC mKepState=%s\n",
                                  mKepState.ToString().c_str());
   #endif
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   mKepState = StateConversionUtil::CartesianToKeplerian(mGravConst, state);

   #ifdef DEBUG_ORBITDATA_KEP_STATE
   MessageInterface::ShowMessage("OrbitData::GetKepState() mKepState=%s\n",
                                 mKepState.ToString().c_str());
   #endif
   
   return mKepState;
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
   Rvector6 mModEquinState =
      StateConversionUtil::Convert(state, "Cartesian", "ModifiedEquinoctial",
                                   mGravConst, mFlattening, mEqRadius);
   
   return mModEquinState;
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
   Rvector6 mDelaState =
      StateConversionUtil::Convert(state, "Cartesian", "Delaunay",
                                   mGravConst, mFlattening, mEqRadius);
   
   #ifdef DEBUG_DELAUNAY_STATE
   MessageInterface::ShowMessage
      ("OrbitData::GetDelaState() returning mDelaState='%s'", mDelaState.ToString().c_str());
   #endif
   
   return mDelaState;
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
   Rvector6 mPlanetodeticState =
      StateConversionUtil::Convert(state, "Cartesian", "Planetodetic",
                                   mGravConst, mFlattening, mEqRadius );
   
   return mPlanetodeticState;
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
   mModKepState = StateConversionUtil::KeplerianToModKeplerian(kepState);
   
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
   mSphRaDecState = StateConversionUtil::Convert(state, "Cartesian", "SphericalRADEC",
                    mGravConst, mFlattening, mEqRadius);

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
   mSphAzFpaState = StateConversionUtil::Convert(state, "Cartesian", "SphericalAZFPA",
                    mGravConst, mFlattening, mEqRadius);

   
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
   Rvector6 mEquinState = StateConversionUtil::Convert(state, "Cartesian", "Equinoctial",
                          mGravConst, mFlattening, mEqRadius);
   
   return mEquinState;
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
            //return mSphRaDecState[RADEC_RMAG];
            return mSphRaDecState[RADEC_RMAG-Item2Count];
         else
            return GetPositionMagnitude(mOrigin);
      }
   case RADEC_RA:
   case RADEC_DEC:
   case RADEC_VMAG:
   case RADEC_RAV:
   case RADEC_DECV:
      //return mSphRaDecState[item];
      return mSphRaDecState[item-Item2Count];
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
      //return mSphAzFpaState[item];
      return mSphAzFpaState[item-Item3Count];
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
      
      //mOrigin =
      //   (SpacePoint*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACE_POINT]);
      
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


