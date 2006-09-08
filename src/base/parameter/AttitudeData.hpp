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
#ifndef ATTITUDEDATA_HPP_
#define ATTITUDEDATA_HPP_

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
   
   enum {QUAT1, QUAT2, QUAT3, QUAT4};
   
   enum
   {
      SPACECRAFT = 0,
      AttitudeDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[AttitudeDataObjectCount];
};

#endif /*ATTITUDEDATA_HPP_*/
