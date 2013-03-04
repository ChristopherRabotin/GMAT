//$Id$
//------------------------------------------------------------------------------
//                                  AttitudeData
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
// Developed further jointly by NASA/GSFC, Thinking Systems, Inc., and 
// Schafer Corp., under AFRL NOVA Contract #FA945104D03990003
//
// Author: Daniel Hunter
// Created: 2006/6/26
// Modified:  Dunn Idle (added MRPs)
// Date:    2010/08/24
//
/**
 * Implements Attitude related data class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "AttitudeData.hpp"
#include "Attitude.hpp"
#include "Linear.hpp"              // for GmatRealUtil::ToString()
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"


const std::string
AttitudeData::VALID_OBJECT_TYPE_LIST[AttitudeDataObjectCount] =
{
   "Spacecraft"
}; 

const Real AttitudeData::ATTITUDE_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;

//------------------------------------------------------------------------------
// AttitudeData(const std::string name = "")
//------------------------------------------------------------------------------
AttitudeData::AttitudeData(const std::string name)
   : RefData(name)
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
// Real GetReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves Attitude element.
 */
//------------------------------------------------------------------------------
Real AttitudeData::GetReal(Integer item)
{
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   Real epoch = mSpacecraft->GetEpoch();
   
   // get the basics - cosine matrix, angular velocity, euler angle sequence
   Rmatrix33        cosMat        = mSpacecraft->GetAttitude(epoch);
   Rvector3         angVel        = mSpacecraft->GetAngularVelocity(epoch) 
                                    * GmatMathConstants::DEG_PER_RAD;
   UnsignedIntArray seq           = mSpacecraft->GetEulerAngleSequence();
   Rvector3         euler;
   
   if (item == DCM_11)   return cosMat(0,0);
   if (item == DCM_12)   return cosMat(0,1);
   if (item == DCM_13)   return cosMat(0,2);
   if (item == DCM_21)   return cosMat(1,0);
   if (item == DCM_22)   return cosMat(1,1);
   if (item == DCM_23)   return cosMat(1,2);
   if (item == DCM_31)   return cosMat(2,0);
   if (item == DCM_32)   return cosMat(2,1);
   if (item == DCM_33)   return cosMat(2,2);
   if (item == ANGULAR_VELOCITY_X) return angVel[0];
   if (item == ANGULAR_VELOCITY_Y) return angVel[1];
   if (item == ANGULAR_VELOCITY_Z) return angVel[2];
   
   // do conversions if necessary
   if ((item >= QUAT_1) && (item <= QUAT_4))
   {
      Rvector quat = Attitude::ToQuaternion(cosMat);
      return quat[item - QUAT_1];
   }    
   if ((item >= EULER_ANGLE_1) && (item <= EULER_ANGLE_3))
   {
      euler = Attitude::ToEulerAngles(cosMat, 
              (Integer) seq[0], 
              (Integer) seq[1], 
              (Integer) seq[2]) * GmatMathConstants::DEG_PER_RAD;
      return euler[item - EULER_ANGLE_1];
   }
   // Dunn added conversion below with the comment that this
   // is slightly hacked!  We might want to change to a more
   // obvious method of index control.
   if ((item >= MRP_1) && (item <= MRP_3))
   {
      Rvector  quat = Attitude::ToQuaternion(cosMat);
      Rvector3 mrp  = Attitude::ToMRPs(quat);
      return mrp[item - MRP_1];
   }           
   if ((item >= EULER_ANGLE_RATE_1) && (item <= EULER_ANGLE_RATE_3))
   {
      euler = Attitude::ToEulerAngles(cosMat, 
              (Integer) seq[0], 
              (Integer) seq[1], 
              (Integer) seq[2]) * GmatMathConstants::DEG_PER_RAD;
      Rvector3 eulerRates = Attitude::ToEulerAngleRates(
                            angVel * GmatMathConstants::RAD_PER_DEG,
                            euler  * GmatMathConstants::RAD_PER_DEG,
                            (Integer) seq[0], 
                            (Integer) seq[1], 
                            (Integer) seq[2]) * GmatMathConstants::DEG_PER_RAD;
      return eulerRates[item - EULER_ANGLE_RATE_1];
   }        
   
   // otherwise, there is an error   
   throw ParameterException
      ("AttitudeData::GetReal() Not readable or unknown item id: " +
       GmatRealUtil::ToString(item));   
}

//------------------------------------------------------------------------------
// void SetReal(Integer item, Real value)
//------------------------------------------------------------------------------
/**
 * Retrieves Attitude element.
 */
//------------------------------------------------------------------------------
void AttitudeData::SetReal(Integer item, Real value)
{
   #ifdef DEBUG_SET_REAL
   MessageInterface::ShowMessage
      ("AttitudeData::SetReal() entered, item = %d, value = %f\n", item, value);
   #endif
   
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   Attitude *attitude = (Attitude*)mSpacecraft->GetRefObject(Gmat::ATTITUDE, "");
   if (attitude == NULL)
      throw ParameterException
         ("AttitudeData::SetReal() Attitude of the Spacecraft \"" +
          mSpacecraft->GetName() + "\" is NULL\n");
   
   switch (item)
   {
   case DCM_11:
      attitude->SetRealParameter("DCM11", value);
      break;
   case DCM_12:
      attitude->SetRealParameter("DCM12", value);
      break;
   case DCM_13:
      attitude->SetRealParameter("DCM13", value);
      break;
   case DCM_21:
      attitude->SetRealParameter("DCM21", value);
      break;
   case DCM_22:
      attitude->SetRealParameter("DCM22", value);
      break;
   case DCM_23:
      attitude->SetRealParameter("DCM23", value);
      break;
   case DCM_31:
      attitude->SetRealParameter("DCM31", value);
      break;
   case DCM_32:
      attitude->SetRealParameter("DCM32", value);
      break;
   case DCM_33:
      attitude->SetRealParameter("DCM33", value);
      break;
   case EULER_ANGLE_1:
      attitude->SetRealParameter("EulerAngle1", value);
      break;
   case EULER_ANGLE_2:
      attitude->SetRealParameter("EulerAngle2", value);
      break;
   case EULER_ANGLE_3:
      attitude->SetRealParameter("EulerAngle3", value);
      break;
   case ANGULAR_VELOCITY_X:
      attitude->SetRealParameter("AngularVelocityX", value);
      break;
   case ANGULAR_VELOCITY_Y:
      attitude->SetRealParameter("AngularVelocityY", value);
      break;
   case ANGULAR_VELOCITY_Z:
      attitude->SetRealParameter("AngularVelocityZ", value);
      break;
   case EULER_ANGLE_RATE_1:
      attitude->SetRealParameter("EulerAngleRate1", value);
      break;
   case EULER_ANGLE_RATE_2:
      attitude->SetRealParameter("EulerAngleRate2", value);
      break;
   case EULER_ANGLE_RATE_3:
      attitude->SetRealParameter("EulerAngleRate3", value);
      break;
      
   default:
      // otherwise, there is an error   
      throw ParameterException
         ("AttitudeData::SetReal() Not settable or unknown item id: " + 
          GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// std::string GetString(Integer item)
//------------------------------------------------------------------------------
std::string AttitudeData::GetString(Integer item)
{
   #ifdef DEBUG_GET_STRING
   MessageInterface::ShowMessage
      ("AttitudeData::GetString() entered, item = %d\n", item);
   #endif
   
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   Real epoch = mSpacecraft->GetEpoch();
   
   // Get cosine matrix which is internal representation and always gets updated
   Rmatrix33 cosMat = mSpacecraft->GetAttitude(epoch);
   Rvector quat = Attitude::ToQuaternion(cosMat);
   
   switch (item)
   {
   case QUATERNION:
   {
      // We don't want square brackets around values (GMT-3663)
      //std::string str = "[ " + GmatStringUtil::ToString(quat[0], 15) + " " +
      //   GmatStringUtil::ToString(quat[1], 15) + " " + GmatStringUtil::ToString(quat[2], 15) + 
      //   " " + GmatStringUtil::ToString(quat[3], 15) + "]";
      std::string str = GmatStringUtil::ToString(quat[0], 15) + " " +
         GmatStringUtil::ToString(quat[1], 15) + " " + GmatStringUtil::ToString(quat[2], 15) + 
         " " + GmatStringUtil::ToString(quat[3], 15);
      return str;
   }
   default:
      // otherwise, there is an error   
      throw ParameterException
         ("AttitudeData::SetString() Not redable or unknown item id: " +
          GmatRealUtil::ToString(item));
   }
}

//------------------------------------------------------------------------------
// void SetString(Integer item, const std::string &value)
//------------------------------------------------------------------------------
void AttitudeData::SetString(Integer item, const std::string &value)
{
   #ifdef DEBUG_SET_STRING
   MessageInterface::ShowMessage
      ("AttitudeData::SetString() entered, item = %d, value = '%s'\n",
       item, value.c_str());
   #endif
   
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   Attitude *attitude = (Attitude*)mSpacecraft->GetRefObject(Gmat::ATTITUDE, "");
   if (attitude == NULL)
      throw ParameterException
         ("AttitudeData::SetString() Attitude of the Spacecraft \"" +
          mSpacecraft->GetName() + "\" is NULL\n");
   
   switch (item)
   {
   case QUATERNION:
      attitude->SetStringParameter("Quaternion", value);
      break;
   default:
      // otherwise, there is an error   
      throw ParameterException
         ("AttitudeData::SetString() Unknown item id: " +
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

