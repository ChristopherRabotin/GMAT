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
#include <iomanip>
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
    if (fromDateFormat == "TAIModJulian" && toDateFormat != "TAIModJulian") 
    {
       A1Mjd *jd = new A1Mjd(time);

       if (toDateFormat == "UTCModJulian")
       {
          newTime = jd->ToUtcMjd();
       }
    
       else if (toDateFormat == "UTCGregorian")
       {
          UtcDate utcDate = jd->ToUtcDate();
          newTime = utcDate.ToPackedCalendarReal();
       }

       else if (toDateFormat == "TAIGregorian")
       {
          A1Date a1Date = jd->ToA1Date();
          newTime = a1Date.ToPackedCalendarReal();
       }

       delete jd;
    }

    else if (fromDateFormat == "TAIGregorian" && toDateFormat != "TAIGregorian")
    {
       std::ostringstream timeBuffer;
       timeBuffer.precision(9);
       timeBuffer.setf(std::ios::fixed);
       timeBuffer << time; 
      
       A1Date *a1Date = new A1Date(timeBuffer.str());

       Real mjd = ModifiedJulianDate(a1Date->GetYear(),
                                     a1Date->GetMonth(),
                                     a1Date->GetDay(),
                                     a1Date->GetHour(),
                                     a1Date->GetMinute(),
                                     a1Date->GetSecond());
       delete a1Date;

       if (toDateFormat == "TAIModJulian")
          return(mjd);

       // Convert to A1 MJD and then UTC MJD 
       A1Mjd a1Mjd(mjd);

       if (toDateFormat == "UTCGregorian")
       {
          UtcDate utcDate = a1Mjd.ToUtcDate();
          newTime = utcDate.ToPackedCalendarReal();
       }
       else if (toDateFormat == "UTCModJulian")  
          newTime = a1Mjd.ToUtcMjd();

    }

    else if (fromDateFormat == "UTCModJulian" && toDateFormat != "UTCModJulian")
    {
       if (toDateFormat == "TAIGregorian")
       {
          // Convert to A1 MJD and then A1 Date 
          A1Mjd a1Mjd;
          Real mjd =  a1Mjd.UtcMjdToA1Mjd(time); 
          a1Mjd.Set(mjd);
          A1Date a1Date = a1Mjd.ToA1Date();
          newTime = a1Date.ToPackedCalendarReal();
       }

       else if (toDateFormat == "TAIModJulian")
       {
          A1Mjd a1Mjd;
          newTime = a1Mjd.UtcMjdToA1Mjd(time);
       }

       else if (toDateFormat == "UTCGregorian")
       {
          A1Mjd a1Mjd;
          Real a1mjd = a1Mjd.UtcMjdToA1Mjd(time);
          a1Mjd.Set(a1mjd);  
          UtcDate utcDate = a1Mjd.ToUtcDate();
          newTime = utcDate.ToPackedCalendarReal();
       }
    }

    else if (fromDateFormat == "UTCGregorian" && toDateFormat != "UTCGregorian")
    {
       std::ostringstream timeBuffer;
       timeBuffer.precision(9);
       timeBuffer.setf(std::ios::fixed);
       timeBuffer << time; 
      
       UtcDate *utcDate = new UtcDate(timeBuffer.str());

       // Get UTC MJD 
       Real mjd = ModifiedJulianDate(utcDate->GetYear(),
                                     utcDate->GetMonth(),
                                     utcDate->GetDay(),
                                     utcDate->GetHour(),
                                     utcDate->GetMinute(),
                                     utcDate->GetSecond());

       if (toDateFormat == "TAIModJulian")
          newTime = utcDate->ToA1Mjd();

       else if (toDateFormat == "UTCModJulian")
          newTime = mjd;

       else if (toDateFormat == "TAIGregorian")
       {
          // Convert to A1 MJD and then A1 Date 
          A1Mjd a1Mjd;
          mjd =  a1Mjd.UtcMjdToA1Mjd(mjd); 
          a1Mjd.Set(mjd);
          A1Date a1Date = a1Mjd.ToA1Date();
          newTime = a1Date.ToPackedCalendarReal();
       }

       delete utcDate;
    }

    return newTime;
}
