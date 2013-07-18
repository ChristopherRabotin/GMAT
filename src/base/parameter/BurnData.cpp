//$Id$
//------------------------------------------------------------------------------
//                                  BurnData
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
// Created: 2005/05/27
//
/**
 * Implements ImpulsiveBurn related data class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "BurnData.hpp"
#include "ParameterException.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"           // for GmatRealUtil::ToString()
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BURNDATA_INIT 1
//#define DEBUG_BURNDATA_CONVERT 1
//#define DEBUG_BURNDATA_RUN 1
//#define DEBUG_BURNDATA_SET 1
//#define DEBUG_BURNDATA_GET
//#define DEBUG_BURNDATA_COORD_CONVERT

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const std::string
BurnData::VALID_OBJECT_TYPE_LIST[BurnDataObjectCount] =
{
   "ImpulsiveBurn",
   "SolarSystem",
   "CoordinateSystem",
   "Spacecraft"
}; 

const Real BurnData::BURN_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;
const Real BurnData::BURN_TOL            = 1.0e-10;


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BurnData(const std::string &name = "",
//          const Gmat::ObjectType paramOwnerType = Gmat::IMPULSIVE_BURN)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
BurnData::BurnData(const std::string &name,
                   const Gmat::ObjectType paramOwnerType)
   : RefData(name, paramOwnerType)
{
   mImpBurn                   = NULL;
   mSpacecraft                = NULL;
   mSolarSystem               = NULL;
   mOrigin                    = NULL;
   mInternalCoordSystem       = NULL;
   mOutCoordSystem            = NULL;

   firstTimeEpochWarning      = false;
   firstTimeHasntFiredWarning = false;

   if (mParamDepName == "")
      mIsParamCSDep = false;
   else
      mIsParamCSDep = true;
}


//------------------------------------------------------------------------------
// BurnData(const BurnData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the BurnData object being copied.
 */
//------------------------------------------------------------------------------
BurnData::BurnData(const BurnData &copy)
   : RefData(copy)
{
   mImpBurn                   = copy.mImpBurn;
   mSpacecraft                = copy.mSpacecraft;
   mSolarSystem               = copy.mSolarSystem;
   mOrigin                    = copy.mOrigin;
   mInternalCoordSystem       = copy.mInternalCoordSystem;
   mOutCoordSystem            = copy.mOutCoordSystem;
   mIsParamCSDep              = copy.mIsParamCSDep;
   firstTimeEpochWarning      = copy.firstTimeEpochWarning;        // or false?
   firstTimeHasntFiredWarning = copy.firstTimeHasntFiredWarning;   // or false?
}


//------------------------------------------------------------------------------
// BurnData& operator= (const BurnData& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
BurnData& BurnData::operator= (const BurnData& right)
{
   if (this == &right)
      return *this;
   
   RefData::operator=(right);
      
   mImpBurn                   = right.mImpBurn;
   mSpacecraft                = right.mSpacecraft;
   mSolarSystem               = right.mSolarSystem;
   mOrigin                    = right.mOrigin;
   mInternalCoordSystem       = right.mInternalCoordSystem;
   mOutCoordSystem            = right.mOutCoordSystem;
   mIsParamCSDep              = right.mIsParamCSDep;
   firstTimeEpochWarning      = right.firstTimeEpochWarning;
   firstTimeHasntFiredWarning = right.firstTimeHasntFiredWarning;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~BurnData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
BurnData::~BurnData()
{
}


//------------------------------------------------------------------------------
// Real GetReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves Burn element.
 */
//------------------------------------------------------------------------------
Real BurnData::GetReal(Integer item)
{
   ///@todo convert internal DeltaV to Parameter CoordinateSystem.
   if (mImpBurn == NULL)
      InitializeRefObjects();
   
   if (mImpBurn == NULL)
   {
      MessageInterface::ShowMessage
         ("Cannot find Burn object so returning %f\n", BURN_REAL_UNDEFINED);
      return BURN_REAL_UNDEFINED;
   }
   
   if (mIsParamCSDep)
   {
      if ((mInternalCoordSystem == NULL) ||
          (mOutCoordSystem == NULL))
      {
         std::string errmsg = "**** ERROR **** Missing, invalid, or ";
         errmsg            += "nonexistent internal or output ";
         errmsg            += "CoordinateSystem object, for parameter \"";
         errmsg            += mActualParamName + "\"\n";
         throw ParameterException(errmsg);
      }

      if (!mImpBurn->HasFired())
      {
         if (!firstTimeHasntFiredWarning)
         {
            std::string errmsg = "Warning: Impulsive Burn ";
            errmsg += mImpBurn->GetName() + " has not fired, so returning \"0\" ";
            errmsg += "for maneuver parameter.\n";
            MessageInterface::ShowMessage(errmsg);
            firstTimeHasntFiredWarning = true;
         }
         return 0.0;
      }
      // we need to do conversions and check for epochs here
      // write a warning of the epoch of the maneuver does not match the
      // epoch of the participants in the coordinate system
      // this part has been removed/postponed  WCS 2013.07.01
//      Real burnEpoch = mImpBurn->GetEpochAtLastFire();
//      ObjectArray objs =  mOurCoordSystem->GetSpacePointParticipants();

      Real burnEpoch = mImpBurn->GetEpochAtLastFire();
      Real *burnDeltaV;
      burnDeltaV = mImpBurn->GetDeltaVInertial();
      Rvector6 burnState(0.0,0.0,0.0, burnDeltaV[0], burnDeltaV[1], burnDeltaV[2]);
      #ifdef DEBUG_BURNDATA_GET
      MessageInterface::ShowMessage(
            "In BurnData::GetReal, burnEpoch = %le, burnDeltaV = %le %le %le\n",
            burnEpoch, burnDeltaV[0],burnDeltaV[1],burnDeltaV[2]);
      MessageInterface::ShowMessage("      and burnState = %s\n",
            burnState.ToString().c_str());
      MessageInterface::ShowMessage(
            "      mInternalCoordSystem = %s, mOutCoordSystem = %s\n",
            mInternalCoordSystem->GetName().c_str(),
            mOutCoordSystem->GetName().c_str());
      #endif
      Rvector6 burnOut(0.0,0.0,0.0,0.0,0.0,0.0);
      if (mInternalCoordSystem->GetName() != mOutCoordSystem->GetName())
      {
         mCoordConverter.Convert(A1Mjd(burnEpoch), burnState, mInternalCoordSystem,
                                 burnOut, mOutCoordSystem, false, true);   // do I want true here?
         #ifdef DEBUG_BURNDATA_COORD_CONVERT
            MessageInterface::ShowMessage("before conversion, burnState = %s\n",
                  burnState.ToString().c_str());
            Rmatrix33 lastR = mCoordConverter.GetLastRotationMatrix();
               MessageInterface::ShowMessage("Last R is: %s\n", lastR.ToString().c_str());
            Rmatrix33 lastRDot = mCoordConverter.GetLastRotationDotMatrix();
               MessageInterface::ShowMessage("Last RDot is: %s\n", lastRDot.ToString().c_str());
            MessageInterface::ShowMessage("after conversion, burnOut = %s\n",
                  burnOut.ToString().c_str());
         #endif
      }
      switch (item)
      {
      case ELEMENT1:
         return burnOut[3];
      case ELEMENT2:
         return burnOut[4];
      case ELEMENT3:
         return burnOut[5];
      default:
         throw ParameterException("BurnData::GetReal() Unknown ELEMENT id: " +
                                  GmatRealUtil::ToString(item));
      }
   }
   else
   {
      // if there is no dependency object, then return the values from the
      // burn in the burn coordinate system
      switch (item)
      {
      case ELEMENT1:
         return mImpBurn->GetRealParameter(mImpBurn->GetParameterID("Element1"));
      case ELEMENT2:
         return mImpBurn->GetRealParameter(mImpBurn->GetParameterID("Element2"));
      case ELEMENT3:
         return mImpBurn->GetRealParameter(mImpBurn->GetParameterID("Element3"));
      default:
         throw ParameterException("BurnData::GetReal() Unknown ELEMENT id: " +
                                  GmatRealUtil::ToString(item));
      }
   }

}


//------------------------------------------------------------------------------
// void SetReal(Integer item, Real rval)
//------------------------------------------------------------------------------
void BurnData::SetReal(Integer item, Real rval)
{
   #if DEBUG_BURNDATA_SET
   MessageInterface::ShowMessage
      ("BurnData::SetReal() item=%d, rval=%f\n", item, rval);
   #endif
   
   ///@todo convert internal DeltaV to Parameter CoordinateSystem.
   if (mImpBurn == NULL)
      InitializeRefObjects();
   
   if (mImpBurn == NULL)
   {
      MessageInterface::ShowMessage
         ("Cannot find Burn object so returning %f\n", BURN_REAL_UNDEFINED);
   }
   
   std::string csName = mImpBurn->GetStringParameter(mImpBurn->GetParameterID("CoordinateSystem"));
//   if (mIsParamCSDep)
   if (mIsParamCSDep && (mOutCoordSystem->GetName() != csName))
   {
      ParameterException pe;
      pe.SetDetails("Currently GMAT cannot set %s; the impulsive burn '%s' "
                    "requires values to be in its own coordinate system (setting "
                    "values in different coordinate systems will be implemented in "
                    "future builds)",  mActualParamName.c_str(),
                    mImpBurn->GetName().c_str());
      throw pe;
   }


   switch (item)
   {
   case ELEMENT1:
      mImpBurn->SetRealParameter(mImpBurn->GetParameterID("Element1"), rval);
      break;
   case ELEMENT2:
      mImpBurn->SetRealParameter(mImpBurn->GetParameterID("Element2"), rval);
      break;
   case ELEMENT3:
      mImpBurn->SetRealParameter(mImpBurn->GetParameterID("Element3"), rval);
      break;
   default:
      throw ParameterException("BurnData::SetReal() Unknown ELEMENT id: " +
                               GmatRealUtil::ToString(item));
   }
}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* BurnData::GetValidObjectList() const
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
bool BurnData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<BurnDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == BurnDataObjectCount)
      return true;
   else
      return false;
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void BurnData::InitializeRefObjects()
{
   #if DEBUG_BURNDATA_INIT
   MessageInterface::ShowMessage
      ("BurnData::InitializeRefObjects() entered.\n");
   #endif
   
   mImpBurn = (ImpulsiveBurn*)FindFirstObject(VALID_OBJECT_TYPE_LIST[IMPULSIVE_BURN]);
   
   if (mImpBurn == NULL)
   {
      // just write a message
      #if DEBUG_BURNDATA_INIT
      MessageInterface::ShowMessage
         ("BurnData::InitializeRefObjects() Cannot find Burn object.\n");
      #endif
      
      //throw ParameterException
      //   ("BurnData::InitializeRefObjects() Cannot find Burn object.\n");
   }
   
   //-------------------------------------------------------
   // Need this for future?
   //-------------------------------------------------------
//    mSolarSystem =
//       (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   
//    if (mSolarSystem == NULL)
//       throw ParameterException
//          ("BurnData::InitializeRefObjects() Cannot find SolarSystem object\n");
   
    if (mInternalCoordSystem == NULL)
    {
       std::string errmsg = "**** ERROR **** Missing, invalid, or ";
       errmsg            += "nonexistent internal CoordinateSystem object, ";
       errmsg            += "for parameter \"" + mActualParamName + "\"\n";
       throw ParameterException(errmsg);
    }

   #if DEBUG_BURNDATA_INIT
   MessageInterface::ShowMessage
      ("BurnData::InitializeRefObjects() Found internal coordinate system\n");
   #endif
   
    mOutCoordSystem =
       (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
   
    if (mOutCoordSystem == NULL)
    {
       std::string errmsg = "**** ERROR **** Missing, invalid, or ";
       errmsg            += "nonexistent output CoordinateSystem object, ";
       errmsg            += "for parameter \"" + mActualParamName + "\"\n";
       throw ParameterException(errmsg);
    }
   #if DEBUG_BURNDATA_INIT
   MessageInterface::ShowMessage
      ("BurnData::InitializeRefObjects() Found OUT coordinate system\n");
   #endif

//    // get Burn CoordinateSystem
//    std::string csName = mImpBurn->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
//    CoordinateSystem *cs = (CoordinateSystem*)mImpBurn->
//       GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
//
//    if (!cs)
//       throw ParameterException
//          ("BurnData::InitializeRefObjects() spacecraft CoordinateSystem not "
//           "found: " + csName + "\n");
//   #if DEBUG_BURNDATA_INIT
//   MessageInterface::ShowMessage
//      ("BurnData::InitializeRefObjects() Found spacecraft coordinate system\n");
//   #endif

   
   #if DEBUG_BURNDATA_INIT
   MessageInterface::ShowMessage
      ("BurnData::InitializeRefObjects() mImpBurn=%s(%p)\n",
       mImpBurn->GetName().c_str(), mImpBurn);
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
bool BurnData::IsValidObjectType(Gmat::ObjectType type)
{
   for (int i=0; i<BurnDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;

}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystem()
//------------------------------------------------------------------------------
SolarSystem* BurnData::GetSolarSystem()
{
   return mSolarSystem;
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordSys()
//------------------------------------------------------------------------------
CoordinateSystem* BurnData::GetInternalCoordSys()
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
void BurnData::SetInternalCoordSys(CoordinateSystem *cs)
{
   mInternalCoordSystem = cs;
}


