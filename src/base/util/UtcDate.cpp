//$Header$
//------------------------------------------------------------------------------
//                                UtcDate
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 1995/10/18 for GSS project
// Modified:
//   2003/09/12 Linda Jun - Added member data: descs, stringValues.
//              Replaced GSSString with std::string.
//
/**
 * Provides conversions among various ways representing UTC calendar
 * dates and times.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "A1Mjd.hpp"     // for A1Mjd
#include "UtcDate.hpp"   // for UtcDate
//  #include "A1Date.hpp"    // for A1Date

using namespace GmatTimeUtil;

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  UtcDate()
//------------------------------------------------------------------------------
/**
 * @note Calls A1Mjd default constructor which creates an object with 0 
 *       second from reference.
 */
//------------------------------------------------------------------------------
UtcDate::UtcDate()
  : Date()
{
}

//------------------------------------------------------------------------------
//  UtcDate (Integer year, Integer month, Integer day, Integer hour, 
//           Integer minute, Real second)
//------------------------------------------------------------------------------
/**
 * @note Assumes input date is in UTC time system.
 */
//------------------------------------------------------------------------------
UtcDate::UtcDate (Integer year, Integer month, Integer day, Integer hour, 
                  Integer minute, Real second)
  : Date(year, month, day, hour, minute, second)
{
}

//------------------------------------------------------------------------------
//  UtcDate (Integer year, Integer dayOfYear, Integer hour, Integer minute,
//           Real second)
//------------------------------------------------------------------------------
/**
 * @note Assumes input date is in UTC time system.
 */
//------------------------------------------------------------------------------
UtcDate::UtcDate (Integer year, Integer dayOfYear, Integer hour, Integer minute,
                  Real second)
  : Date(year, dayOfYear, hour, minute, second)
{
}

//------------------------------------------------------------------------------
//  UtcDate (Integer year, Integer month, Integer day, Real secondsOfDay);
//------------------------------------------------------------------------------
/**
 * @note Assumes input date is in UTC time system.
 */
//------------------------------------------------------------------------------
UtcDate::UtcDate (Integer year, Integer month, Integer day, Real secondsOfDay)
  : Date(year, month, day, secondsOfDay)
{
}

//------------------------------------------------------------------------------
//  UtcDate (const GmatTimeUtil::CalDate &date)
//------------------------------------------------------------------------------
/**
 * @note Assumes input date is in UTC time system.
 */
//------------------------------------------------------------------------------
UtcDate::UtcDate (const GmatTimeUtil::CalDate &date)
  : Date(date)
{
}

//------------------------------------------------------------------------------
//  UtcDate (const std::string& time)
//------------------------------------------------------------------------------
/**
 * @param <time> Time in "YYMMDD.hhmmssnnn" format
 *
 * @note Assumes input date is in UTC time system.
 */
//------------------------------------------------------------------------------
UtcDate::UtcDate (const std::string& time)
  : Date(time)
{
}

//------------------------------------------------------------------------------
//  UtcDate (const UtcDate &date)
//------------------------------------------------------------------------------
UtcDate::UtcDate (const UtcDate &date)
  : Date(date)
{
}

//------------------------------------------------------------------------------
//  UtcDate operator= (const UtcDate &date)
//------------------------------------------------------------------------------
UtcDate UtcDate::operator= (const UtcDate &date)
{
    yearD = date.yearD;
    monthD = date.monthD;
    dayD = date.dayD;
    secondsOfDayD = date.secondsOfDayD;
    return *this;
}

//------------------------------------------------------------------------------
//  ~UtcDate ()
//------------------------------------------------------------------------------
UtcDate::~UtcDate ()
{
}

//  //------------------------------------------------------------------------------
//  //  Real operator- (const UtcDate &date) const
//  //------------------------------------------------------------------------------
//  Real UtcDate::operator- (const UtcDate &date) const
//  {
//      Real offset;
   
//      A1Mjd t1(yearD, monthD, dayD, secondsOfDayD);
//      A1Mjd t2(date.yearD, date.monthD, date.dayD, date.secondsOfDayD);
   
//      //loj:offset = t1 - t2;
//      offset = t1.Subtract(t2);
//      return offset;
//  }

//  //------------------------------------------------------------------------------
//  //  UtcDate operator+ (const Real seconds) const
//  //------------------------------------------------------------------------------
//  UtcDate UtcDate::operator+ (const Real seconds) const
//  {   
//      A1Mjd tempTime(*this);

//      tempTime = tempTime + seconds;
//      return UtcDate(tempTime);
//  }

//  //------------------------------------------------------------------------------
//  //  UtcDate& operator+= (const Real seconds)
//  //------------------------------------------------------------------------------
//  UtcDate& UtcDate::operator+= (const Real seconds)
//  {
//      UtcDate utcDate;
//      A1Mjd tempTime(*this);

//      tempTime = tempTime + seconds;
//      utcDate = UtcDate(tempTime);
//      *this = utcDate;
//      return *this;
//  }

//  //------------------------------------------------------------------------------
//  //  UtcDate operator- (const Real seconds) const
//  //------------------------------------------------------------------------------
//  UtcDate UtcDate::operator- (const Real seconds) const
//  {
//      A1Mjd tempTime(*this);

//      tempTime = tempTime - seconds;
//      return UtcDate(tempTime);
//  }

//  //------------------------------------------------------------------------------
//  //  UtcDate& operator-= (const Real seconds)
//  //------------------------------------------------------------------------------
//  UtcDate& UtcDate::operator-= (const Real seconds)
//  {
//      UtcDate utcDate;
//      A1Mjd tempTime(*this);

//      tempTime = tempTime - seconds;
//      utcDate = UtcDate(tempTime);
//      *this = utcDate;
//      return *this;
//  }

//------------------------------------------------------------------------------
//  A1Date ToA1Date()
//------------------------------------------------------------------------------
/**
 * @note The two time systems label time differently.  At any given moment,
 *       the A.1 system is several seconds ahead of the UTC system.  This offset
 *       is constant between leap insertions.  For example, the instant of time
 *       labeled July 1, 1992, 12:00:27.0343817 in the A.1 system will be labeled
 *       July 1, 1992, 12:00:00 (Noon) in the UTC system.
 */
//------------------------------------------------------------------------------
//  A1Date UtcDate::ToA1Date()
//  {
//      A1Date a1Date;
//      CalDate tempDate;
   
//      // convert UTC date to equivalent A1 date
//      A1Mjd utcMjd(*this); //loj: should subtract leap seconds when A1Mjd is constructed
//      //a1Date = A1Date(utcMjd); //loj: compile error

//      return utcMjd.ToA1Date();
    
//  }
