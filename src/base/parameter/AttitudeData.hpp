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
 * Declares Attitude related data class.
 */
//------------------------------------------------------------------------------
#ifndef AttitudeData_hpp
#define AttitudeData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"

class GMAT_API AttitudeData : public RefData
{
public:

   AttitudeData(const std::string name = "");
   AttitudeData(const AttitudeData &data);
   AttitudeData& operator= (const AttitudeData& right);
   virtual ~AttitudeData();
   
   Real GetReal(Integer item);
   void SetReal(Integer item, Real value);
   std::string GetString(Integer item);
   void        SetString(Integer item, const std::string &value);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
   const static Real ATTITUDE_REAL_UNDEFINED;
   
protected:

   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(Gmat::ObjectType type);
   
   Spacecraft *mSpacecraft;
   Integer mEpochId;
   
   enum 
   {
      DCM_11,
      DCM_12,
      DCM_13,
      DCM_21,
      DCM_22,
      DCM_23,
      DCM_31,
      DCM_32,
      DCM_33,
      QUAT_1, 
      QUAT_2, 
      QUAT_3, 
      QUAT_4,
      QUATERNION,
      EULER_ANGLE_1,
      EULER_ANGLE_2,
      EULER_ANGLE_3,
      MRP_1,  // Dunn Added
      MRP_2,  // Dunn Added
      MRP_3,  // Dunn Added
      ANGULAR_VELOCITY_X, 
      ANGULAR_VELOCITY_Y, 
      ANGULAR_VELOCITY_Z,
      EULER_ANGLE_RATE_1,
      EULER_ANGLE_RATE_2,
      EULER_ANGLE_RATE_3,
   };
   
   enum
   {
      SPACECRAFT = 0,
      AttitudeDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[AttitudeDataObjectCount];
};

#endif /*AttitudeData_hpp*/
