//$Header$
//------------------------------------------------------------------------------
//                                    DateUtil
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 1995/10/18
//
/**
 * This class provides conversions among various ways of representing calendar
 * dates and times.
 */
//------------------------------------------------------------------------------
#ifndef DateUtil_hpp
#define DateUtil_hpp

#include "TimeTypes.hpp" // for ElapsedDays, YearNumber, MonthOfYear, DayOfMonth

class DateUtil
{
public:
   static Integer JulianDay(YearNumber year, MonthOfYear month,
                            DayOfMonth day);
   
   static std::string FormatGregorian(YearNumber year, MonthOfYear month,
                                      DayOfMonth day, Integer hour, Integer minute,
                                      Real second);
   
   friend Real JulianDate(YearNumber year, MonthOfYear month,
                          DayOfMonth day, Integer hour, Integer minute,
                          Real second);
   
   friend Real ModifiedJulianDate(YearNumber year, MonthOfYear month,
                                  DayOfMonth day, Integer hour, Integer minute,
                                  Real second);
   
   friend void UnpackDate(Real packedDate, Integer& year, Integer& month, 
                          Integer& day);        // input in YYYYMMDD. 
   friend void UnpackDateWithDOY(Real packedDate, Integer& year, Integer& day);
   
   // input in YYYYDDD. 
   friend void UnpackTime(Real packedTime, Integer& hour, Integer& minute, 
                          Real& second);        // input in hhmmssnnn.
   friend void ToMonthDayFromYearDOY(Integer year, Integer dayOfYear,
                                     Integer& month, Integer& day);
   friend Integer ToDOYFromYearMonthDay(Integer year, Integer month,
                                        Integer day);
   friend Real ToSecondsOfDayFromHMS(Integer hour, Integer minute,
                                     Real second);
   friend void ToHMSFromSecondsOfDay(Real secsOfDay, Integer& hour, 
                                     Integer& minute, Real& second);
   friend bool IsValidTime(Integer year, Integer month, Integer day, 
                           Integer hour, Integer minute, Real second);
   static bool IsValidGregorian(const std::string &str);
   friend bool IsLeapYear(Integer year);
  
};
#endif
