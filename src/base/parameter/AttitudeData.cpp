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
// Date:      2010/08/24
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
 * Retrieves Attitude element.
 */
//------------------------------------------------------------------------------
Real AttitudeData::GetAttitudeReal(Integer item)
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
   
   if (item == DCM1_1)   return cosMat(0,0);
   if (item == DCM1_2)   return cosMat(0,1);
   if (item == DCM1_3)   return cosMat(0,2);
   if (item == DCM2_1)   return cosMat(1,0);
   if (item == DCM2_2)   return cosMat(1,1);
   if (item == DCM2_3)   return cosMat(1,2);
   if (item == DCM3_1)   return cosMat(2,0);
   if (item == DCM3_2)   return cosMat(2,1);
   if (item == DCM3_3)   return cosMat(2,2);
   if (item == ANGVELX) return angVel[0];
   if (item == ANGVELY) return angVel[1];
   if (item == ANGVELZ) return angVel[2];
   
   // do conversions if necessary
   if ((item >= QUAT1) && (item <= QUAT4))
   {
      Rvector quat = Attitude::ToQuaternion(cosMat);
      return quat[item - QUAT1];
   }    
   if ((item >= EULERANGLE1) && (item <= EULERANGLE3))
   {
      euler = Attitude::ToEulerAngles(cosMat, 
              (Integer) seq[0], 
              (Integer) seq[1], 
              (Integer) seq[2]) * GmatMathConstants::DEG_PER_RAD;
      return euler[item - EULERANGLE1];
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
   if ((item >= EULERANGLERATE1) && (item <= EULERANGLERATE3))
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
      return eulerRates[item - EULERANGLERATE1];
   }        
   
   // otherwise, there is an error   
   throw ParameterException
      ("AttitudeData::GetAttitudeReal() Unknown parameter id: " +
       GmatRealUtil::ToString(item));

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

