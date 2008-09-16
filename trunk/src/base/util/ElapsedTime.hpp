//$Header$
//------------------------------------------------------------------------------
//                              ElapsedTime
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
#include "RealTypes.hpp"

class GMAT_API ElapsedTime
{
public:

//ElapsedTime();
   ElapsedTime(const Real &secs = 0.0, const Real tol = GmatRealConst::REAL_EPSILON);
   ElapsedTime(const ElapsedTime &elapsedTime, const Real tol = GmatRealConst::REAL_EPSILON);
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
