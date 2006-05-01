//$Header$
//------------------------------------------------------------------------------
//                                  BplaneData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/06/16
//
/**
 * Implements B-Plane related data class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "BplaneData.hpp"
#include "ParameterException.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"   // for Sqrt()
#include "UtilityException.hpp"
#include "CelestialBody.hpp"
#include "Linear.hpp"           // for GmatRealUtil::ToString()
#include "MessageInterface.hpp"

//#define DEBUG_BPLANE_DATA_INIT 1
//#define DEBUG_BPLANE_DATA_CONVERT 1
//#define DEBUG_BPLANE_DATA_RUN 1

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const std::string
BplaneData::VALID_OBJECT_TYPE_LIST[BplaneDataObjectCount] =
{
   "Spacecraft",
   "SolarSystem",
   "CoordinateSystem",
   "SpacePoint"
}; 


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BplaneData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
BplaneData::BplaneData()
   : RefData()
{
   mCartState = Rvector6::RVECTOR6_UNDEFINED;
   mSpacecraft = NULL;
   mSolarSystem = NULL;
   mOrigin = NULL;
   mInternalCoordSystem = NULL;
   mOutCoordSystem = NULL;
   mCartEpoch = 0.0;
   mGravConst = 0.0;
}


//------------------------------------------------------------------------------
// BplaneData(const BplaneData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the BplaneData object being copied.
 */
//------------------------------------------------------------------------------
BplaneData::BplaneData(const BplaneData &copy)
   : RefData(copy)
{
   mCartState = copy.mCartState;
   
   mSpacecraft = copy.mSpacecraft;
   mSolarSystem = copy.mSolarSystem;
   mOrigin = copy.mOrigin;
   mInternalCoordSystem = copy.mInternalCoordSystem;
   mOutCoordSystem = copy.mOutCoordSystem;
   
//    mSpacecraft = NULL;
//    mSolarSystem = NULL;
//    mOrigin = NULL;
//    mInternalCoordSystem = NULL;
//    mOutCoordSystem = NULL;
   
   mCartEpoch = copy.mCartEpoch;
   mGravConst = copy.mGravConst;
}


//------------------------------------------------------------------------------
// BplaneData& operator= (const BplaneData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
BplaneData& BplaneData::operator= (const BplaneData &right)
{
   if (this == &right)
      return *this;
   
   RefData::operator=(right);
   
   mCartState = right.mCartState;
   
   mSpacecraft = right.mSpacecraft;
   mSolarSystem = right.mSolarSystem;
   mOrigin = right.mOrigin;
   mInternalCoordSystem = right.mInternalCoordSystem;
   mOutCoordSystem = right.mOutCoordSystem;
   
//    mSpacecraft = NULL;
//    mSolarSystem = NULL;
//    mOrigin = NULL;
//    mInternalCoordSystem = NULL;
//    mOutCoordSystem = NULL;
   
   mCartEpoch = right.mCartEpoch;
   mGravConst = right.mGravConst;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~BplaneData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
BplaneData::~BplaneData()
{
}


//------------------------------------------------------------------------------
// Real GetBplaneReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Computes B-Plane related paramters.
 *
 * @note - Implements GMAT math Spec 2.11 B-Plane Coordinates
 */
//------------------------------------------------------------------------------
Real BplaneData::GetBplaneReal(Integer item)
{
   // Check item range
   if (item <= BplaneParamBegin || item >= BplaneParamEnd)
      throw ParameterException
         ("BplaneData::GetBplaneReal() Unknown parameter ID: " +
          GmatRealUtil::ToString(item, 2));
   
   if (mSpacecraft == NULL || mSolarSystem == NULL || mOrigin == NULL ||
       mInternalCoordSystem == NULL || mOutCoordSystem == NULL)
      InitializeRefObjects();
   
   // If the selected coordinate system does not have a celestial body as
   // its origin, then throw an exception
   if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
      mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();
   else
      throw ParameterException
         ("BplaneData::GetBplaneReal() Cannot compute B-Plane "
          "parameter for CoordinateSystem that has non-celestial body as origin.  "
          "CoordinateSystem: " + mOutCoordSystem->GetName() + "  Origin: " +
          mOrigin->GetName());
   
   Rvector6 state = GetCartState();
   
   #if DEBUG_BPLANE_DATA_RUN > 1
   MessageInterface::ShowMessage
      ("BplaneData::GetBplaneReal() item = %d, mGravConst = %f\n     state = %s\n",
       item, mGravConst, state.ToString().c_str());
   #endif
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);
   
   Real rMag = pos.GetMagnitude();
   Real vMag = vel.GetMagnitude();
   
   // Compute eccentricity related information   
   Rvector3 eVec =
      ((vMag*vMag - mGravConst/rMag) * pos - (pos*vel)*vel) / mGravConst;
   
   Real eMag = eVec.GetMagnitude();
   
   // if eMag <= 1, then the method fails, orbit should be hyperbolic
   if (eMag <= 1.0)
      throw ParameterException
         ("BplaneData::GetBplaneReal() ParamID: " + GmatRealUtil::ToString(item, 2) +
          "\n     eccentricity magnitude is <= 1.0. eMag: " +
          GmatRealUtil::ToString(eMag));
   
   eVec.Normalize();
   
   // Compute the angular momentum and orbit normal vectors
   Rvector3 hVec = Cross(pos, vel);
   Real hMag = hVec.GetMagnitude();
   hVec.Normalize();
   Rvector3 nVec = Cross(hVec, eVec);
   
   #if DEBUG_BPLANE_DATA_RUN > 1
   MessageInterface::ShowMessage
      ("BplaneData::GetBplaneReal()\n     eVec = %s\n     hVec = %s\n     nVec = %s\n",
       eVec.ToString().c_str(), hVec.ToString().c_str(), nVec.ToString().c_str());
   #endif
      
   // Compute semiminor axis, b
   Real b = (hMag*hMag) / (mGravConst * Sqrt(eMag*eMag - 1.0));
   
   // Compute incoming asymptote
   Real oneOverEmag = 1.0/eMag;
   Real temp = Sqrt(1.0 - oneOverEmag*oneOverEmag);
   Rvector3 sVec = (eVec/eMag) + (temp*nVec);
   
   // Compute the B-vector
   Rvector3 bVec = b * (temp * eVec - oneOverEmag*nVec);
   
   #if DEBUG_BPLANE_DATA_RUN > 1
   MessageInterface::ShowMessage
      ("BplaneData::GetBplaneReal() b = %f\n     sVec = %s\n     bVec = %s\n",
       b, sVec.ToString().c_str(), bVec.ToString().c_str());
   #endif
   
   // Compute T and R vector
   Rvector3 sVec1(sVec[1], -sVec[0], 0.0);
   Rvector3 tVec = sVec1 / Sqrt(sVec[0]*sVec[0] + sVec[1]*sVec[1]);
   Rvector3 rVec = Cross(sVec, tVec);

   #if DEBUG_BPLANE_DATA_RUN > 1
   MessageInterface::ShowMessage
      ("BplaneData::GetBplaneReal()\n     sVec1 = %s\n     tVec = %s\n     rVec = %s\n",
       sVec1.ToString().c_str(), tVec.ToString().c_str(), rVec.ToString().c_str());
   #endif

   Real bDotT = bVec * tVec;
   Real bDotR = bVec * rVec;
   
   #if DEBUG_BPLANE_DATA_RUN
   MessageInterface::ShowMessage
      ("==>BplaneData::GetBplaneReal() B_DOT_T=%f, B_DOT_R=%f\n   B_VECTOR_MAG=%f, "
       "B_VECTOR_ANGLE=%f\n", bDotT, bDotR, Sqrt(bDotT*bDotT + bDotR*bDotR),
       ATan(bDotR, bDotT));
   #endif
   
   switch (item)
   {
   case B_DOT_T:
      return bDotT;
   case B_DOT_R:
      return bDotR;
   case B_VECTOR_MAG:
      return Sqrt(bDotT*bDotT + bDotR*bDotR);
   case B_VECTOR_ANGLE:
      return ATan(bDotR, bDotT);
   default:
      throw ParameterException
         ("BplaneData::GetBplaneReal() Unknown parameter ID: " +
          GmatRealUtil::ToString(item, 2));
   }
}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* BplaneData::GetValidObjectList() const
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
bool BplaneData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<BplaneDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == BplaneDataObjectCount)
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
void BplaneData::InitializeRefObjects()
{
   #if DEBUG_BPLANE_DATA_INIT
   MessageInterface::ShowMessage
      ("BplaneData::InitializeRefObjects() entered.\n");
   #endif
   
   mSpacecraft =
      (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);

   if (mSpacecraft == NULL)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() Cannot find Spacecraft object.\n"
          "Make sure Spacecraft is set to any unnamed parameters\n");
   
   mSolarSystem =
      (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   
   if (mSolarSystem == NULL)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() Cannot find SolarSystem object\n");
   
   if (mInternalCoordSystem == NULL)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() Cannot find internal "
          "CoordinateSystem object\n");
   
   mOutCoordSystem =
      (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
   
   if (mOutCoordSystem == NULL)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() Cannot find output "
          "CoordinateSystem object\n");
   
   // get spacecraft CoordinateSystem
   std::string csName = mSpacecraft->GetRefObjectName(Gmat::COORDINATE_SYSTEM);   
   CoordinateSystem *cs = (CoordinateSystem*)mSpacecraft->
      GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
   
   if (!cs)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() spacecraft CoordinateSystem not "
          "found: " + csName + "\n");
   
   mOrigin = mOutCoordSystem->GetOrigin();
   
   if (!mOrigin)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() origin not found: " +
          cs->GetOriginName() + "\n");
   
   // get gravity constant if out coord system origin is CelestialBody
   if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
      mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();
   
   #if DEBUG_BPLANE_DATA_INIT
   MessageInterface::ShowMessage
      ("BplaneData::InitializeRefObjects() mOriginName=%s\n",
       mOrigin->GetName().c_str());
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
bool BplaneData::IsValidObjectType(Gmat::ObjectType type)
{
   for (int i=0; i<BplaneDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   MessageInterface::ShowMessage
      ("==> BplaneData::IsValidObjectType() type=%d is not valid object type\n",
       type);
   
   return false;

}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystem()
//------------------------------------------------------------------------------
SolarSystem* BplaneData::GetSolarSystem()
{
   return mSolarSystem;
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordSys()
//------------------------------------------------------------------------------
CoordinateSystem* BplaneData::GetInternalCoordSys()
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
void BplaneData::SetInternalCoordSys(CoordinateSystem *cs)
{
   mInternalCoordSystem = cs;
}


//------------------------------------------------------------------------------
// Rvector6 GetCartState()
//------------------------------------------------------------------------------
Rvector6 BplaneData::GetCartState()
{
//    if (mSpacecraft == NULL || mSolarSystem == NULL)
//       InitializeRefObjects();
   
   mCartEpoch = mSpacecraft->GetRealParameter("A1Epoch");
   mCartState.Set(mSpacecraft->GetState().GetState());
   
   #if DEBUG_BPLANE_DATA_RUN
   MessageInterface::ShowMessage
      ("BplaneData::GetCartState() stateType=%s, internalCoordName=%s\n"
       "     outCoordName=%s\n", elemType.c_str(),
       mInternalCoordSystem->GetName().c_str(),
       mOutCoordSystem->GetName().c_str());
   #endif
   
   
   if (mInternalCoordSystem == NULL || mOutCoordSystem == NULL)
   {
      MessageInterface::ShowMessage
         ("BplaneData::GetCartState() Internal CoordSystem or Output "
          "CoordSystem is NULL.\n");
         
      throw ParameterException
         ("BplaneData::GetCartState() internal or output CoordinateSystem "
          "is NULL.\n");
   }
   
   // convert to output CoordinateSystem
   if (mInternalCoordSystem->GetName() != mOutCoordSystem->GetName())
   {
      #if DEBUG_BPLANE_DATA_CONVERT
      MessageInterface::ShowMessage
         ("BplaneData::GetCartState() ===> mOutCoordSystem:%s Axis=%s\n",
          mOutCoordSystem->GetName().c_str(),
          mOutCoordSystem->GetRefObject(Gmat::AXIS_SYSTEM, "")->GetName().c_str());
      MessageInterface::ShowMessage
         ("BplaneData::GetCartState() Before convert: mCartEpoch=%f\n"
          "state = %s\n", mCartEpoch, mCartState.ToString().c_str());
      #endif
      
      try
      {
         mCoordConverter.Convert(A1Mjd(mCartEpoch), mCartState,
                                 mInternalCoordSystem,
                                 mCartState, mOutCoordSystem);
         
         #if DEBUG_BPLANE_DATA_CONVERT
         MessageInterface::ShowMessage
            ("BplaneData::GetCartState() --After convert: mCartEpoch=%f\n"
             "     state = %s\n", mCartEpoch, mCartState.ToString().c_str());
         #endif
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage(e.GetMessage());
      }
   }
   
   return mCartState;
}


