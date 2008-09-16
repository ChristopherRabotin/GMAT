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

   AttitudeData();
   AttitudeData(const AttitudeData &data);
   AttitudeData& operator= (const AttitudeData& right);
   virtual ~AttitudeData();
   
   Real GetAttitudeReal(Integer item);
   
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
      DCM1_1,
      DCM1_2,
      DCM1_3,
      DCM2_1,
      DCM2_2,
      DCM2_3,
      DCM3_1,
      DCM3_2,
      DCM3_3,
      QUAT1, 
      QUAT2, 
      QUAT3, 
      QUAT4, 
      EULERANGLE1,
      EULERANGLE2,
      EULERANGLE3,
      ANGVELX, 
      ANGVELY, 
      ANGVELZ,
      EULERANGLERATE1,
      EULERANGLERATE2,
      EULERANGLERATE3,
   };
   
   enum
   {
      SPACECRAFT = 0,
      AttitudeDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[AttitudeDataObjectCount];
};

#endif /*AttitudeData_hpp*/
