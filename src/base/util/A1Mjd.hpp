//$Header$
//------------------------------------------------------------------------------
//                                    A1Mjd
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created:  2003/09/15
// Modified: 2004/05/06  J. Gurganus - Added UtcMjdToA1Mjd and moved ToUtcMjd
//                                     from private to public method.
//
/**
 * This class provides A1 Modified Julian Date(MJD). The zero date of the MJD
 * 12 noon on January 5th, 1941.
 */
//------------------------------------------------------------------------------
#ifndef A1Mjd_hpp
#define A1Mjd_hpp

#include "gmatdefs.hpp"
#include "TimeTypes.hpp"
#include "UtcDate.hpp"
#include "A1Date.hpp"

class A1Mjd
{
public:

   A1Mjd();
   A1Mjd(const Real &val);
//loj: do the following later
//     A1Mjd(const Integer year, const Integer month, const Integer day,
//           const Real &secsOfDay);
//     A1Mjd(const UtcDate &utcDate);
//     A1Mjd(const A1Date &a1Date);
   A1Mjd(const A1Mjd &a1mjd);
   A1Mjd& operator= (const A1Mjd &right);
   ~A1Mjd();
   
   A1Mjd operator+ (const A1Mjd &right) const;
   A1Mjd operator- (const A1Mjd &right) const;
   const A1Mjd& operator+= (const A1Mjd &right);
   const A1Mjd& operator-= (const A1Mjd &right);

   A1Mjd operator+ (const Real &right) const;
   A1Mjd operator- (const Real &right) const;
   const A1Mjd& operator+= (const Real &right);
   const A1Mjd& operator-= (const Real &right);

   bool operator< (const A1Mjd &right) const;
   bool operator> (const A1Mjd &right) const;
   bool operator== (const A1Mjd &right) const;
   bool operator!= (const A1Mjd &right) const;
   bool operator>= (const A1Mjd &right) const;
   bool operator<= (const A1Mjd &right) const;

   Real Subtract(const A1Mjd &right) const;
   Real Subtract(const Real &right) const;

   Real Get() const;
   void Set(Real secs);

   UtcDate ToUtcDate();
   A1Date ToA1Date();

   Real   UtcMjdToA1Mjd(const Real utcMjd);
   //Real   Ut1MjdToA1Mjd(const Real ut1Mjd); //wcs: add later
   UtcMjd ToUtcMjd();
   Ut1Mjd ToUt1Mjd();

   // additions to compute offsets for planetary orientation data (see IAU doc)
   Real   JulianDaysFromTCBEpoch() const;

   Integer GetNumData() const;
   const std::string* GetDataDescriptions() const;
   std::string* ToValueStrings();

   static const A1Mjd J2000;

protected:
private:

   Real GetA1UtcDiff(const UtcMjd &utcmjd);
   //Real GetA1Ut1Diff(const UtcMjd &utcmjd); //wcs: add later
   Real GetA1UtcDiffAnalytic(const UtcMjd &utcmjd);
   GmatTimeUtil::CalDate A1MjdToCalDate(const A1Mjd &a1mjd);
   GmatTimeUtil::CalDate UtcMjdToCalDate(const UtcMjd &utcmjd);

//     static Real SecondsSinceReference(Integer year, MonthOfYear month,
//                                       DayOfMonth day, HourOfDay hour,
//                                       MinuteOfHour minute, Real second);

   Real mMjd;
   Real mTolerance;

   static const Integer NUM_DATA = 1;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
   std::string stringValues[NUM_DATA];
};
#endif // A1Mjd_hpp
