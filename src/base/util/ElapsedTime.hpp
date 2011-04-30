//$Id$
//------------------------------------------------------------------------------
//                              ElapsedTime
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
// Created: 2003/09/22
//
/**
 * Declares elapsed time in operations. Internal elapsed time is in seconds.
 */
//------------------------------------------------------------------------------
#ifndef ElapsedTime_hpp
#define ElapsedTime_hpp

#include "gmatdefs.hpp"
#include "TimeTypes.hpp"
#include "GmatConstants.hpp"

class GMAT_API ElapsedTime
{
public:

//ElapsedTime();
   ElapsedTime(const Real &secs = 0.0, const Real tol = GmatRealConstants::REAL_EPSILON);
   ElapsedTime(const ElapsedTime &elapsedTime, const Real tol = GmatRealConstants::REAL_EPSILON);
   ElapsedTime& operator=(const ElapsedTime &right); 
   virtual ~ElapsedTime();

   ElapsedTime operator+ (const Real &right) const;
   ElapsedTime operator- (const Real &right) const;
   const ElapsedTime& operator+= (const Real &right);
   const ElapsedTime& operator-= (const Real &right);

   bool operator<  (const ElapsedTime &right) const;
   bool operator>  (const ElapsedTime &right) const;
   bool operator== (const ElapsedTime &right) const;
   bool operator!= (const ElapsedTime &right) const;
   bool operator>= (const ElapsedTime &right) const;
   bool operator<= (const ElapsedTime &right) const;

   Real Get() const;
   void Set(Real secs);

   GmatTimeUtil::ElapsedDate ToElapsedDate() const;

   Integer GetNumData() const;
   const std::string* GetDataDescriptions() const;
   std::string* ToValueStrings();

protected:
private:

   Real seconds;
   Real tolerance; // Used for time comparison (==, !=)

   static const Integer NUM_DATA = 1;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
   std::string stringValues[NUM_DATA];

};

#endif // ElapsedTime_hpp
