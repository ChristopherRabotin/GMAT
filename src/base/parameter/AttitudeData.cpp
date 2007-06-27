//$Header$
//------------------------------------------------------------------------------
//                                  AttitudeData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Daniel Hunter
// Created: 2006/6/26
//
/**
 * Implements Attitude related data class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "AttitudeData.hpp"
#include "Attitude.hpp"
#include "Linear.hpp"              // for GmatRealUtil::ToString()
#include "RealUtilities.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"


const std::string
AttitudeData::VALID_OBJECT_TYPE_LIST[AttitudeDataObjectCount] =
{
   "Spacecraft"
}; 

const Real AttitudeData::ATTITUDE_REAL_UNDEFINED = -9876543210.1234;

//------------------------------------------------------------------------------
// AttitudeData()
//------------------------------------------------------------------------------
AttitudeData::AttitudeData()
   : RefData()
{
   mSpacecraft = NULL;
}


//------------------------------------------------------------------------------
// AttitudeData(const AttitudeData &copy)
//------------------------------------------------------------------------------
AttitudeData::AttitudeData(const AttitudeData &copy)
   : RefData(copy)
{
   mSpacecraft = copy.mSpacecraft;
}


//------------------------------------------------------------------------------
// AttitudeData& operator= (const AttitudeData& right)
//------------------------------------------------------------------------------
AttitudeData& AttitudeData::operator= (const AttitudeData& right)
{
   if (this == &right)
      return *this;
   
   RefData::operator=(right);
   
   mSpacecraft = right.mSpacecraft;

   return *this;
}


//------------------------------------------------------------------------------
// ~AttitudeData()
//------------------------------------------------------------------------------
AttitudeData::~AttitudeData()
{
}


//------------------------------------------------------------------------------
// Real GetAttitudeReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Attitude element.
 */
//------------------------------------------------------------------------------
Real AttitudeData::GetAttitudeReal(Integer item)
{
   if (mSpacecraft == NULL)
      InitializeRefObjects();

   Real epoch = mSpacecraft->GetRealParameter(mEpochId);
   
   Rmatrix33 cosMat = mSpacecraft->GetAttitude(epoch);
   Rvector   quat   = Attitude::ToQuaternion(cosMat);
   Rvector3  angVel = mSpacecraft->GetAngularVelocity(epoch) 
                      * GmatMathUtil::DEG_PER_RAD;
   
   switch (item)
   {
   case QUAT1:
      return quat[0];
   case QUAT2:
      return quat[1];
   case QUAT3:
      return quat[2];
   case QUAT4:
      return quat[3];
   case ANGVELX:
      return angVel[0];
   case ANGVELY:
      return angVel[1];
   case ANGVELZ:
      return angVel[2];
   default:
      throw ParameterException
         ("AttitudeData::GetAttitudeReal() Unknown parameter id: " +
          GmatRealUtil::ToString(item));
   }
}

//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* AttitudeData::GetValidObjectList() const
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
bool AttitudeData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<AttitudeDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == AttitudeDataObjectCount)
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
void AttitudeData::InitializeRefObjects()
{
   #if DEBUG_ATTDATA_INIT
   MessageInterface::ShowMessage
      ("AttitudeData::InitializeRefObjects() entered.\n");
   #endif
   
   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   
   if (mSpacecraft == NULL)
   {
      // just write a message
      #if DEBUG_ATTDATA_INIT
      MessageInterface::ShowMessage
         ("AttitudeData::InitializeRefObjects() Cannot find Attitude object.\n");
      #endif
      
      //throw ParameterException
      //   ("AttitudeData::InitializeRefObjects() Cannot find Attitude object.\n");
   }
   else
   {
      mEpochId = mSpacecraft->GetParameterID("A1Epoch");
   }
   
   #if DEBUG_ATTDATA_INIT
   MessageInterface::ShowMessage
      ("AttitudeData::InitializeRefObjects() mSpacecraft=%s\n",
       mSpacecraft->GetName().c_str())
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
bool AttitudeData::IsValidObjectType(Gmat::ObjectType type)
{
   for (int i=0; i<AttitudeDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;

}

