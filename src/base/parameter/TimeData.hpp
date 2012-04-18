//$Id$
//------------------------------------------------------------------------------
//                                  TimeData
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
// Created: 2004/01/12
//
/**
 * Declares Time related data class.
 */
//------------------------------------------------------------------------------
#ifndef TimeData_hpp
#define TimeData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"


class GMAT_API TimeData : public RefData
{
public:

   TimeData(const std::string &name = "");
   TimeData(const TimeData &td);
   TimeData& operator= (const TimeData& td);
   virtual ~TimeData();
   
   bool IsInitialEpochSet();
   Real GetInitialEpoch() const;
   void SetInitialEpoch(const Real &initialEpoch);
   
   Real GetTimeReal(Integer id);
   void SetTimeReal(Integer id, Real value);
   
   std::string GetTimeString(Integer id);
   void        SetTimeString(Integer id, const std::string &value);
   
   Real GetElapsedTimeReal(Integer id);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
protected:
    
   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(Gmat::ObjectType type);
   
   Real mInitialEpoch;
   bool mIsInitialEpochSet;
   Spacecraft *mSpacecraft;
   
   const static Real TIME_REAL_UNDEFINED;
   const static std::string TIME_STRING_UNDEFINED;
   
   enum TimeDataID
   {
      A1, TAI, TT, TDB, UTC,
      YEARS, MONTHS, DAYS, HOURS, MINS, SECS
   };
   
   enum
   {
      SPACECRAFT = 0,
      TimeDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[TimeDataObjectCount];
   
};
#endif // TimeData_hpp

