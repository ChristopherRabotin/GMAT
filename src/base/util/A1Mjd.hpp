//$Id$
//------------------------------------------------------------------------------
//                                    A1Mjd
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

class GMAT_API A1Mjd
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
   A1Mjd* Clone() const;
   ~A1Mjd();
   
   A1Mjd operator+ (const A1Mjd &right) const;
   A1Mjd operator- (const A1Mjd &right) const;
   const A1Mjd& operator+= (const A1Mjd &right);
   const A1Mjd& operator-= (const A1Mjd &right);

   A1Mjd operator+ (const Real &right) const;
   A1Mjd operator- (const Real &right) const;
   const A1Mjd& operator+= (const Real &right);
   const A1Mjd& operator-= (const Real &right);

   bool operator< (const A1Mjd &right) const { return mMjd < right.mMjd; }
   bool operator> (const A1Mjd &right) const { return mMjd > right.mMjd; }
   bool operator== (const A1Mjd &right) const;
   bool operator!= (const A1Mjd &right) const;
   bool operator>= (const A1Mjd &right) const { return mMjd >= right.mMjd; }
   bool operator<= (const A1Mjd &right) const { return mMjd <= right.mMjd; }
   
   Real Subtract(const A1Mjd &right) const { return (mMjd - right.mMjd); }
   Real Subtract(const Real &right) const { return (mMjd - right); }
   
   Real Get() const { return mMjd; }
   void Set(Real val) { mMjd = val; }
   Real GetReal() const { return mMjd; }
   void SetReal(Real val) { mMjd = val; }

   UtcDate ToUtcDate();
   A1Date ToA1Date();

   Real   UtcMjdToA1Mjd(const Real utcMjd);
   //Real   Ut1MjdToA1Mjd(const Real ut1Mjd); //wcs: add later
   UtcMjd ToUtcMjd();

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
   static const Real mTolerance;

   static const Integer NUM_DATA = 1;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
   std::string stringValues[NUM_DATA];
};
#endif // A1Mjd_hpp
