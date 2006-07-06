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
#include "ParameterException.hpp"
#include "RealUtilities.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"

using namespace GmatMathUtil;

const std::string
AttitudeData::VALID_OBJECT_TYPE_LIST[AttitudeDataObjectCount] =
{
   "Attitude",
   "Spacecraft"
}; 


AttitudeData::AttitudeData()
   : RefData()
{
	mSpacecraft = NULL;
}

AttitudeData::AttitudeData(const AttitudeData &copy)
   : RefData(copy)
{
	mSpacecraft = copy.mSpacecraft;
}

AttitudeData& AttitudeData::operator= (const AttitudeData& right)
{
   if (this == &right)
      return *this;
   
   RefData::operator=(right);
      
	mSpacecraft = right.mSpacecraft;
}

AttitudeData::~AttitudeData()
{
}

Real AttitudeData::GetAttitudeReal(const std::string &str)
{
   ///@todo convert internal DeltaV to Parameter CoordinateSystem.
   if (mImpAttitude == NULL)
      InitializeRefObjects();
   
   if (str == "Quat1")
   {
   	Rvector quat = mAttitude->GetRvectorParameter("quaternions");
      return quat[0];
   }
   else if (str == "Quat2")
   {
   	Rvector quat = mAttitude->GetRvectorParameter("quaternions");
      return quat[1];
   }
   else if (str == "Quat3")
   {
   	Rvector quat = mAttitude->GetRvectorParameter("quaternions");
      return quat[2];
   }
   else if (str == "Quat4")
   {
   	Rvector quat = mAttitude->GetRvectorParameter("quaternions");
      return quat[3];
   }
   else
      throw ParameterException
         ("AttitudeData::GetAttitudeReal() Unknown parameter name: \n" + str);
    
}
