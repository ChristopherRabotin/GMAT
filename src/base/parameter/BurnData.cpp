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
#include "MessageInterface.hpp"

//#define DEBUG_BURNDATA_INIT 1
//#define DEBUG_BURNDATA_CONVERT 1
//#define DEBUG_BURNDATA_RUN 1
//#define DEBUG_BURNDATA_SET 1

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

const Real BurnData::BURN_REAL_UNDEFINED = -9876543210.1234;
const Real BurnData::BURN_TOL            = 1.0e-10;


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BurnData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
BurnData::BurnData()
   : RefData()
{
   mImpBurn = NULL;
   mSpacecraft = NULL;
   mSolarSystem = NULL;
   mOrigin = NULL;
   mInternalCoordSystem = NULL;
   mOutCoordSystem = NULL;
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
   mImpBurn = copy.mImpBurn;
   mSpacecraft = copy.mSpacecraft;
   mSolarSystem = copy.mSolarSystem;
   mOrigin = copy.mOrigin;
   mInternalCoordSystem = copy.mInternalCoordSystem;
   mOutCoordSystem = copy.mOutCoordSystem;   
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
      
   mImpBurn = right.mImpBurn;
   mSpacecraft = right.mSpacecraft;
   mSolarSystem = right.mSolarSystem;
   mOrigin = right.mOrigin;
   mInternalCoordSystem = right.mInternalCoordSystem;
   mOutCoordSystem = right.mOutCoordSystem;   
   
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
 * Retrives Burn element.
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
   
//    if (mInternalCoordSystem == NULL)
//       throw ParameterException
//          ("BurnData::InitializeRefObjects() Cannot find internal "
//           "CoordinateSystem object\n");
   
//    mOutCoordSystem =
//       (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
   
//    if (mOutCoordSystem == NULL)
//       throw ParameterException
//          ("BurnData::InitializeRefObjects() Cannot find output "
//           "CoordinateSystem object\n");

//    // get Burn CoordinateSystem
//    std::string csName = mImpBurn->GetRefObjectName(Gmat::COORDINATE_SYSTEM);   
//    CoordinateSystem *cs = (CoordinateSystem*)mImpBurn->
//       GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
   
//    if (!cs)
//       throw ParameterException
//          ("BurnData::InitializeRefObjects() spacecraft CoordinateSystem not "
//           "found: " + csName + "\n");

//    // get origin
//    std::string originName =
//       FindFirstObjectName(GmatBase::GetObjectType(VALID_OBJECT_TYPE_LIST[SPACE_POINT]));
   
//    if (originName != "")
//    {
//       #if DEBUG_BURNDATA_INIT
//       MessageInterface::ShowMessage
//          ("BurnData::InitializeRefObjects() getting originName:%s pointer.\n",
//           originName.c_str());
//       #endif
      
//       mOrigin =
//          (SpacePoint*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACE_POINT]);

//       if (!mOrigin)
//          throw ParameterException
//             ("BurnData::InitializeRefObjects() Cannot find Origin object: " +
//              originName + "\n");
      
//    }
//    else
//    {
//       throw ParameterException
//          ("BurnData::InitializeRefObjects() Burn Origin not specified\n");
//    }
   
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


