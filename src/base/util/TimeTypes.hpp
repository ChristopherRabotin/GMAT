//$Header$
//------------------------------------------------------------------------------
//                                TimeTypes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: K. Mueller, L. Jun
// Created: 1995/08/28 for GSS project
// Modified:
//   2003/09/12 Linda Jun - Defined namespace GmatTimeUtil. Added A1MJD_OF_J2000. 
//
/**
 * Provides declarations for date & time types.
 */
//------------------------------------------------------------------------------
#ifndef TimeTypes_hpp
#define TimeTypes_hpp

#include "gmatdefs.hpp"

typedef Real     UtcMjd;
//loj: implemented ElapsedTime as a class
//loj: typedef Real     ElapsedTime;
//loj: 3/12/04 conflicts with ElapsedDays parameter class, use Integer
//typedef Integer  ElapsedDays;
typedef Integer  YearNumber;

typedef Integer  DayOfYear;
typedef Integer  MonthOfYear;
typedef Integer  DayOfMonth;
typedef Integer  HourOfDay;
typedef Integer  MinuteOfHour;

namespace GmatTimeUtil
{
   class CalDate
   {
   public:
   CalDate(YearNumber y, MonthOfYear mo, DayOfMonth d,
           HourOfDay h, MinuteOfHour m, Real s)
      : year(y), month(mo), day(d), hour(h), minute(m), second(s)
   {};

   CalDate() : year(1941), month(1), day(5), hour(0), minute(0), second(0.0) {};
   
   YearNumber   year;
   MonthOfYear  month;
   DayOfMonth   day;
   HourOfDay    hour;
   MinuteOfHour minute;
   Real         second;
   };
   
   class ElapsedDate
   {
   public:
   ElapsedDate(Integer d, Integer h, Integer m, Real s)
      : days(d), hours(d), minutes(m), seconds(s)
   {};

   ElapsedDate() : days(0), hours(0), minutes(0), seconds(0.0) {};

   Integer days;
   Integer hours;
   Integer minutes;
   Real    seconds;
   };

   const Real SECS_PER_DAY = 86400.0;
   const Real SECS_PER_HOUR = 3600.0;
   const Real SECS_PER_MINUTE = 60.0;
   
   const Real TIME_OF_J2000 = 883655990.850000; // 2000/01/01 43167.85
   const Real A1MJD_OF_J2000 = 21545.0;         // 2000/01/01 11:59:27.965622
   const Real MJDSEC_TO_WHOLE_NUM = 1.0e6;      // Convert mjd seconds to whole number

   const Integer DAYS_BEFORE_MONTH[12] =
   {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
   const Integer LEAP_YEAR_DAYS_BEFORE_MONTH[12] =
   {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
   const Integer DAYS_IN_MONTH[12] =
   {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   const Integer LEAP_YEAR_DAYS_IN_MONTH[12] =
   {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   const Integer JULIAN_DATE_OF_010541 = 2430000;
   
   enum DayName {SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY};

   enum MonthName {JANUARY = 1, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST,  
                   SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER};
          
   enum SIGN {PLUS = 1, MINUS = -1 };
   
}
#endif // TimeTypes_hpp
