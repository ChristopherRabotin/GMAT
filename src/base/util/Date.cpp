//$Header$
//------------------------------------------------------------------------------
//                                  Date
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun, L. Cisney
// Created: 1995/10/18 for GSS project (originally GSSDate)
// Modified: 2003/09/12 Linda Jun - See Date.hpp
//
/**
 * This class is abstrace base class which provides conversions among various
 * ways of representing calendar dates and times.
 */
//------------------------------------------------------------------------------
#include <sstream>            // for stringstream
#include "gmatdefs.hpp"
#include "TimeTypes.hpp"      // for Time constants
#include "Date.hpp"
#include "A1Mjd.hpp"
#include "DateUtil.hpp"       // for ToHMSFromSecondsOfDay()

//---------------------------------
// static data
//---------------------------------
const std::string Date::DATA_DESCRIPTIONS[NUM_DATA] = 
{
   "Year", "Month", "Day", "Hour", "Minute", "Second"
};

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  Integer GetYear() const
//------------------------------------------------------------------------------
Integer Date::GetYear() const
{
   return yearD;
}

//------------------------------------------------------------------------------
//  Integer GetMonth() const
//------------------------------------------------------------------------------
Integer Date::GetMonth() const
{
   return monthD;
}

//------------------------------------------------------------------------------
//  Integer GetDay() const
//------------------------------------------------------------------------------
Integer Date::GetDay() const
{
   return dayD;
}

//------------------------------------------------------------------------------
//  Real GetSecondsOfDay() const
//------------------------------------------------------------------------------
Real Date::GetSecondsOfDay() const
{
   return secondsOfDayD;
}

//------------------------------------------------------------------------------
// Integer Date::GetHour() const
//------------------------------------------------------------------------------
Integer Date::GetHour() const
{
   Integer hour;
   Integer min;
   Real sec;
   
   ToHMSFromSecondsOfDay (secondsOfDayD, hour, min, sec);
   return hour;
}

//------------------------------------------------------------------------------
// Integer Date::GetMinute() const
//------------------------------------------------------------------------------
Integer Date::GetMinute() const
{
   Integer hour;
   Integer min;
   Real sec;
   
   ToHMSFromSecondsOfDay (secondsOfDayD, hour, min, sec);
   return min;
}

//------------------------------------------------------------------------------
// Real Date::GetSecond() const
//------------------------------------------------------------------------------
Real Date::GetSecond() const
{
   Integer hour;
   Integer min;
   Real sec;
   
   ToHMSFromSecondsOfDay (secondsOfDayD, hour, min, sec);
   return sec;
}

//------------------------------------------------------------------------------
//  GmatTimeUtil::DayName GetDayName() const
//------------------------------------------------------------------------------
GmatTimeUtil::DayName Date::GetDayName() const
{
   const Integer JD_OF_010172 = 2441318;
   const GmatTimeUtil::DayName DAY_NAME_OF_010172  = GmatTimeUtil::SATURDAY;
   Integer dayNumber;

   dayNumber = (Integer)(DateUtil::JulianDay(yearD, monthD, dayD))
      - JD_OF_010172;
   
   dayNumber = (dayNumber + DAY_NAME_OF_010172) % 7;

   return (GmatTimeUtil::DayName)dayNumber;
}

//------------------------------------------------------------------------------
//  Integer GetDaysPerMonth() const
//------------------------------------------------------------------------------
Integer Date::GetDaysPerMonth() const
{
   if (IsLeapYear(yearD))
      return GmatTimeUtil::LEAP_YEAR_DAYS_IN_MONTH[monthD - 1];
   else
      return GmatTimeUtil::DAYS_IN_MONTH[monthD - 1];
}

//------------------------------------------------------------------------------
//  GmatTimeUtil::MonthName GetMonthName() const
//------------------------------------------------------------------------------
GmatTimeUtil::MonthName Date::GetMonthName() const
{
   return (GmatTimeUtil::MonthName)monthD; 
}

//------------------------------------------------------------------------------
//  std::string ToPackedCalendarString() const
//------------------------------------------------------------------------------
//  std::string Date::ToPackedCalendarString() const
//  {
//     //loj: need to work on this
//     //loj: commented out: ostrstream not supported, so return "20030101.000000000"
//     //char timeString[18];
//      Real ymd;
//      Real hms;
   
//      YearMonDayHourMinSec (ymd, hms);
 
//      ostrstream outStream(timeString, 18);
//      outStream << (Integer)ymd << "." << (Integer)hms;

//     std::string packedTime(timeString);
//     return packedTime;
//  } 

//------------------------------------------------------------------------------
//  void ToYearDOYHourMinSec(Integer& year, Integer& dayOfYear,
//                           Integer& hour, Integer& minute, Real& second) const
//------------------------------------------------------------------------------
void Date::ToYearDOYHourMinSec(Integer& year, Integer& dayOfYear,
                                   Integer& hour, Integer& minute, Real& second) const
{
   year = yearD;
   dayOfYear = ToDOYFromYearMonthDay(yearD, monthD, dayD);
   ToHMSFromSecondsOfDay (secondsOfDayD, hour, minute, second);
}          

//------------------------------------------------------------------------------
//  void ToYearMonDayHourMinSec (Integer& year, Integer& month, Integer& day, 
//                               Integer& hour, Integer& minute, Real& second) const
//------------------------------------------------------------------------------
void Date::ToYearMonDayHourMinSec (Integer& year, Integer& month, Integer& day, 
                                       Integer& hour, Integer& minute, Real& second) const
{
   year = yearD;
   month = monthD;
   day = dayD;
   ToHMSFromSecondsOfDay (secondsOfDayD, hour, minute, second);
}

//------------------------------------------------------------------------------
//  void ToYearMonDayHourMinSec (Real& ymd, Real& hms) const
//------------------------------------------------------------------------------
void Date::ToYearMonDayHourMinSec (Real& ymd, Real& hms) const
{
   Integer h;
   Integer m;
   Real    s;
   
   ToHMSFromSecondsOfDay (secondsOfDayD, h, m, s);
   ymd = (Real) (yearD * 10000.0 + monthD * 100.0 + dayD);
   hms = (Real) (h * 1.0e+07 + m * 100000.0) +  s * 1000.0;
}

//------------------------------------------------------------------------------
// Integer GetNumData() const
//------------------------------------------------------------------------------
Integer Date::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* Date::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}

//------------------------------------------------------------------------------
// std::string* ToValueStrings()
//------------------------------------------------------------------------------
std::string* Date::ToValueStrings()
{
   Integer hour;
   Integer min;
   Real sec;
   
   ToHMSFromSecondsOfDay (secondsOfDayD, hour, min, sec);
   std::stringstream ss("");

   ss << yearD;
   stringValues[0] = ss.str();
   
   ss.str("");
   ss << monthD;
   stringValues[1] = ss.str();
   
   ss.str("");
   ss << dayD;
   stringValues[2] = ss.str();
   
   ss.str("");
   ss << hour;
   stringValues[3] = ss.str();
   
   ss.str("");
   ss << min;
   stringValues[4] = ss.str();
   
   ss.str("");
   ss << sec;
   stringValues[5] = ss.str();
   
   return stringValues;
}

//---------------------------------
//  protected
//---------------------------------

//------------------------------------------------------------------------------
//  Date()
//------------------------------------------------------------------------------
Date::Date()
   : yearD(1941), monthD(1), dayD(1), secondsOfDayD(43167.85)
{
}

//------------------------------------------------------------------------------
//  Date(Integer year, Integer month, Integer day, Integer hour, 
//           Integer minute, Real second)
//------------------------------------------------------------------------------
Date::Date(Integer year, Integer month, Integer day, Integer hour, 
                   Integer minute, Real second)
{
   // check time
   if (!IsValidTime(year, month, day, hour, minute, second))
   {
      throw TimeRangeError();
   }

   yearD = year;
   monthD = month;
   dayD = day;
   secondsOfDayD = ToSecondsOfDayFromHMS(hour, minute, second);
}

//------------------------------------------------------------------------------
//  Date(Integer year, Integer dayOfYear, Integer hour, Integer minute,
//           Real second)
//------------------------------------------------------------------------------
Date::Date(Integer year, Integer dayOfYear, Integer hour, Integer minute,
                   Real second)
{
   yearD = year;
   ToMonthDayFromYearDOY (year, dayOfYear, monthD, dayD);
   
   // check time
   if (!IsValidTime(yearD, monthD, dayD, hour, minute, second))
   {
      throw TimeRangeError();
   }
   secondsOfDayD = ToSecondsOfDayFromHMS(hour, minute, second);
}

//------------------------------------------------------------------------------
//  Date(Integer year, Integer month, Integer day, Real secondsOfDay)
//------------------------------------------------------------------------------
Date::Date(Integer year, Integer month, Integer day, Real secondsOfDay)
{
   Real    seconds;
   Integer hour, minute;
   
   yearD = year;
   monthD = month;
   dayD = day;
   secondsOfDayD = secondsOfDay;
   
   // check time
   ToHMSFromSecondsOfDay (secondsOfDay, hour, minute, seconds);
   if (!IsValidTime(yearD, monthD, dayD, hour, minute, seconds))
   {
      throw TimeRangeError();
   }
}

//------------------------------------------------------------------------------
// Date(const GmatTimeUtil::CalDate &date)
//------------------------------------------------------------------------------
Date::Date(const GmatTimeUtil::CalDate &date)
{
   yearD = date.year;
   monthD = date.month;
   dayD = date.day;
   secondsOfDayD = date.hour * GmatTimeUtil::SECS_PER_HOUR +
      date.minute * GmatTimeUtil::SECS_PER_MINUTE + date.second;
}

//------------------------------------------------------------------------------
//  Date(const std::string& time)
//------------------------------------------------------------------------------
/**
 * @param <time> time in string form of "YYYYMMDD.hhmmssnnn"
 */
//------------------------------------------------------------------------------
Date::Date(const std::string& time)
{
   const char *tempTime = time.c_str();
   Integer datePart;
   Integer timePart;
   Integer year, month, day, hour, minute;
   Real    second;
   
   datePart = atoi(tempTime);
   tempTime = strstr(tempTime, ".");
   timePart = atoi(tempTime + 1);
   
   try
   {
      UnpackDate(datePart, year, month, day);
      UnpackTime(timePart, hour, minute, second);
   }
   catch(TimeRangeError& tre)
   {
      throw;
   }
   yearD = year;
   monthD = month;
   dayD = day;
   secondsOfDayD = ToSecondsOfDayFromHMS(hour, minute, second);
}

//------------------------------------------------------------------------------
//  Date(const Date &date)
//------------------------------------------------------------------------------
Date::Date(const Date &date)
{
   yearD = date.yearD;
   monthD = date.monthD;
   dayD = date.dayD;
   secondsOfDayD = date.secondsOfDayD;
}
      
//------------------------------------------------------------------------------
//  ~Date()
//------------------------------------------------------------------------------
Date::~Date()
{
}

