//$Id$
//------------------------------------------------------------------------------
//                              TimeConverter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

#include "TimeConverter.hpp"

#include <cstdlib>			// Required for GCC 4.3

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
std::string TimeConverter::Convert(const std::string &time,
                                   const std::string &fromDateFormat,
                                   const std::string &toDateFormat)
{
   Real inTime = atof(time.c_str()), outTime;
   std::string newTime = time;

   std::ostringstream timeBuffer;
   timeBuffer.precision(11);
   timeBuffer.setf(std::ios::fixed);
   timeBuffer.str("");

   // Determine the input of date format
   if (fromDateFormat == "TAIModJulian" && toDateFormat != "TAIModJulian")
   {
      if (toDateFormat == "TAIGregorian")
         return ModJulianToGregorian(inTime);

    // 20.02.06 - arg: changed to use enum types instead of strings
//      outTime = TimeConverterUtil::Convert(inTime, "A1Mjd", "UtcMjd",
//         GmatTimeConstants::JD_JAN_5_1941);
      outTime = TimeConverterUtil::Convert(inTime, TimeConverterUtil::A1MJD,
         TimeConverterUtil::UTCMJD, GmatTimeConstants::JD_JAN_5_1941);

      if (toDateFormat == "UTCModJulian")
      {
         timeBuffer << outTime;
         newTime = timeBuffer.str();
      }
      else if (toDateFormat == "UTCGregorian")
         newTime = ModJulianToGregorian(outTime);
   }
   else if (fromDateFormat == "TAIGregorian" && toDateFormat != "TAIGregorian")
   {
      inTime = GregorianToModJulian(time);
      if (toDateFormat == "TAIModJulian")
      {
         timeBuffer << inTime;
         return(timeBuffer.str());
      }

    // 20.02.06 - arg: changed to use enum types instead of strings
//      outTime = TimeConverterUtil::Convert(inTime, "A1Mjd", "UtcMjd",
//         GmatTimeConstants::JD_JAN_5_1941);
      outTime = TimeConverterUtil::Convert(inTime, TimeConverterUtil::A1MJD,
         TimeConverterUtil::UTCMJD, GmatTimeConstants::JD_JAN_5_1941);

      if (toDateFormat == "UTCGregorian")
         newTime = ModJulianToGregorian(outTime);
      else if (toDateFormat == "UTCModJulian")
      {
         timeBuffer << outTime;
         return(timeBuffer.str());
      }
   }
   else if (fromDateFormat == "UTCModJulian" && toDateFormat != "UTCModJulian")
   {
      if (toDateFormat == "UTCGregorian")
         return ModJulianToGregorian(inTime);

    // 20.02.06 - arg: changed to use enum types instead of strings
//      outTime = TimeConverterUtil::Convert(inTime, "UtcMjd", "A1Mjd",
//         GmatTimeConstants::JD_JAN_5_1941);
      outTime = TimeConverterUtil::Convert(inTime, TimeConverterUtil::UTCMJD,
         TimeConverterUtil::A1MJD, GmatTimeConstants::JD_JAN_5_1941);

      if (toDateFormat == "TAIGregorian")
         newTime = ModJulianToGregorian(outTime);
      else if (toDateFormat == "TAIModJulian")
      {
         timeBuffer << outTime;
         return timeBuffer.str();
      }
   }
   else if (fromDateFormat == "UTCGregorian" && toDateFormat != "UTCGregorian")
   {
      inTime = GregorianToModJulian(time);
      if (toDateFormat == "UTCModJulian")
      {
         timeBuffer << inTime;
         return timeBuffer.str();
      }

    // 20.02.06 - arg: changed to use enum types instead of strings
//      outTime = TimeConverterUtil::Convert(inTime, "UtcMjd", "A1Mjd",
//         GmatTimeConstants::JD_JAN_5_1941);
      outTime = TimeConverterUtil::Convert(inTime, TimeConverterUtil::UTCMJD,
         TimeConverterUtil::A1MJD, GmatTimeConstants::JD_JAN_5_1941);

      if (toDateFormat == "TAIModJulian")
      {
         timeBuffer << outTime;
         return timeBuffer.str();
      }
      else if (toDateFormat == "TAIGregorian")
         newTime = ModJulianToGregorian(outTime);
   }

   return newTime;
}


std::string TimeConverter::ModJulianToGregorian(const Real mjTime)
{
   A1Mjd a1Mjd(mjTime);
   A1Date a1Date = a1Mjd.ToA1Date();
   GregorianDate gregorianDate(&a1Date);
   return gregorianDate.GetDate();
}


Real TimeConverter::GregorianToModJulian(const std::string greg)
{
   GregorianDate gregorianDate(greg);
   Real jules;

   if (!gregorianDate.IsValid())
      throw TimeConverterException();

   try
   {
      A1Date a1Date(gregorianDate.GetYMDHMS());
      jules = ModifiedJulianDate(a1Date.GetYear(),a1Date.GetMonth(),
                                 a1Date.GetDay(),a1Date.GetHour(),
                                 a1Date.GetMinute(),a1Date.GetSecond());
   }
   catch (Date::TimeRangeError&)
   {
      throw TimeConverterException();
   }

   return jules;
}
