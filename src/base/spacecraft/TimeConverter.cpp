//$Header$ 
//------------------------------------------------------------------------------
//                              TimeConverter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2004/04/15
//
/**
 * Implements the TimeConverter class
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "TimeConverter.hpp"

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  TimeConverter()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 *
 */
TimeConverter::TimeConverter() :
   Converter("ModifiedJulian") 
{
}

//---------------------------------------------------------------------------
//  TimeConverter(const std::string &type)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <typeStr> GMAT script string associated with this type of object.
 *
 */
TimeConverter::TimeConverter(const std::string &type) :
   Converter(type) 
{
}

//---------------------------------------------------------------------------
//  TimeConverter(const TimeConverter &timeConverter)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base TimeConverter structures.
 *
 * @param <stateConverter> The original that is being copied.
 */
TimeConverter::TimeConverter(const TimeConverter &timeConverter) :
    Converter (timeConverter.type)
{
}

//---------------------------------------------------------------------------
//  ~TimeConverter()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
TimeConverter::~TimeConverter()
{
}

//---------------------------------------------------------------------------
//  TimeConverter& operator=(const TimeConverter &timeConverter)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TimeConverter structures.
 *
 * @param <converter> The original that is being copied.
 *
 * @return Reference to this object
 */
TimeConverter& TimeConverter::operator=(const TimeConverter &timeConverter)
{
    // Don't do anything if copying self
    if (&timeConverter == this)
        return *this;

    // Will fix it later
    return *this;
}

//---------------------------------------------------------------------------
//  Real TimeConverter::Convert(const Real time,   
//                              const std::string &fromDateFormat,
//                              const std::string &toDateFormat)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TimeConverter structures.
 *
 * @param <time>            Given Time 
 * @param <fromDateFormat>  Time which is converted from date format
 * @param <toDateFormat>    Tiem which is converted to date format 
 *
 * @return Converted time from the specific data format 
 */
Real TimeConverter::Convert(const Real time,   
                            const std::string &fromDateFormat,
                            const std::string &toDateFormat)
{
    Real newTime = time;

    // Determine the input of date format 
    if (fromDateFormat == "ModifiedJulian" && toDateFormat != "ModifiedJulian") 
    {
       A1Mjd *jd = new A1Mjd(time);

       if (toDateFormat == "UTC")
       {
          UtcDate utcDate = jd->ToUtcDate();
          newTime = utcDate.ToPackedCalendarReal();
       }

       if (toDateFormat == "Gregorian")
       {
          A1Date a1Date = jd->ToA1Date();
          newTime = a1Date.ToPackedCalendarReal();
       }

       delete jd;
    }
    else if (fromDateFormat == "UTC" && toDateFormat != "UTC") 
    {
       std::ostringstream timeBuffer;
       timeBuffer.precision(9);
       timeBuffer.setf(std::ios::fixed);
       timeBuffer << time; 
      
       UtcDate *utcDate = new UtcDate(timeBuffer.str());

       if (toDateFormat == "ModifiedJulian")
       {
          // Get the Julian day
          Integer julianDay = DateUtil::JulianDay(utcDate->GetYear(),
                                                  utcDate->GetMonth(),
                                                  utcDate->GetDay());

          Real seconds = utcDate->GetHour() * GmatTimeUtil::SECS_PER_HOUR +
                         utcDate->GetMinute() * GmatTimeUtil::SECS_PER_MINUTE +
                         utcDate->GetSecond();

          Real fractionalDay = (seconds/GmatTimeUtil::SECS_PER_HOUR)/24.0; 

          // Get the Modified Julian Date
          newTime = julianDay + fractionalDay - 
                    GmatTimeUtil::JULIAN_DATE_OF_010541;
       }

       if (toDateFormat == "Gregorian")
       {

       }

       delete utcDate;
    }
    else if (fromDateFormat == "Gregorian" && toDateFormat != "Gregorian")
    {
       std::ostringstream timeBuffer;
       timeBuffer.precision(9);
       timeBuffer.setf(std::ios::fixed);
       timeBuffer << time; 
      
       A1Date *a1Date = new A1Date(timeBuffer.str());

       if (toDateFormat == "ModifedJulian")
       {

       }

       if (toDateFormat == "UTC")
       {

       }

       delete a1Date;
    }

    return newTime;
}
