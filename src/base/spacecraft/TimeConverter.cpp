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
std::string TimeConverter::Convert(const std::string &time,   
                                   const std::string &fromDateFormat,
                                   const std::string &toDateFormat)
{
    Real realTime = atof(time.c_str());
    std::string newTime = time;

    std::ostringstream timeBuffer;
    timeBuffer.precision(9);
    timeBuffer.setf(std::ios::fixed);
    timeBuffer.str("");

    // Determine the input of date format 
    if (fromDateFormat == "TAIModJulian" && toDateFormat != "TAIModJulian") 
    {
       A1Mjd jd(realTime);

       if (toDateFormat == "UTCModJulian")
       {
          realTime = jd.ToUtcMjd();
          timeBuffer << realTime;
          newTime = timeBuffer.str();
       }
    
       else if (toDateFormat == "UTCGregorian")
       {
          UtcDate utcDate = jd.ToUtcDate();

          GregorianDate gregorianDate(&utcDate);
          newTime = gregorianDate.GetDate();   
       }

       else if (toDateFormat == "TAIGregorian")
       {
          A1Date a1Date = jd.ToA1Date();

          GregorianDate gregorianDate(&a1Date);
          newTime = gregorianDate.GetDate();   
       }

    }

    else if (fromDateFormat == "TAIGregorian" && toDateFormat != "TAIGregorian")
    {
       Real mjd;  
       GregorianDate gregorianDate(time);

       if (!gregorianDate.IsValid())
          throw TimeConverterException();

       try
       {
          A1Date a1Date(gregorianDate.GetYMDHMS());

          mjd = ModifiedJulianDate(a1Date.GetYear(),a1Date.GetMonth(),
                                   a1Date.GetDay(),a1Date.GetHour(),
                                   a1Date.GetMinute(),a1Date.GetSecond());
                                   
       }
       catch (Date::TimeRangeError& e)
       {
          throw TimeConverterException();
       }
    
       if (toDateFormat == "TAIModJulian")
       {
          timeBuffer << mjd;
          return(timeBuffer.str());
       }
       // Convert to A1 MJD and then UTC MJD 
       A1Mjd a1Mjd(mjd);

       if (toDateFormat == "UTCGregorian")
       {
          UtcDate utcDate = a1Mjd.ToUtcDate();
          gregorianDate.SetDate(&utcDate);
  
          if (!gregorianDate.IsValid())
             throw TimeConverterException();
  
          newTime = gregorianDate.GetDate();

          // newTime = utcDate.ToPackedCalendarString();
       }
       else if (toDateFormat == "UTCModJulian")  
       {

          realTime = a1Mjd.ToUtcMjd();

          timeBuffer << realTime;
          return(timeBuffer.str());
       }

    }

    else if (fromDateFormat == "UTCModJulian" && toDateFormat != "UTCModJulian")
    {
       GregorianDate gregorianDate;

       if (toDateFormat == "TAIGregorian")
       {
          // Convert to A1 MJD and then A1 Date 
          A1Mjd a1Mjd;
          Real mjd =  a1Mjd.UtcMjdToA1Mjd(realTime); 
          a1Mjd.Set(mjd);
          A1Date a1Date = a1Mjd.ToA1Date();

          gregorianDate.SetDate(&a1Date);
  
          if (!gregorianDate.IsValid())
             throw TimeConverterException();
  
          newTime = gregorianDate.GetDate();
       }

       else if (toDateFormat == "TAIModJulian")
       {
          A1Mjd a1Mjd;
          realTime = a1Mjd.UtcMjdToA1Mjd(realTime);
 
          timeBuffer << realTime;
          return(timeBuffer.str());
       }

       else if (toDateFormat == "UTCGregorian")
       {
          A1Mjd a1Mjd;
          Real a1mjd = a1Mjd.UtcMjdToA1Mjd(realTime);
          a1Mjd.Set(a1mjd);  
          UtcDate utcDate = a1Mjd.ToUtcDate();

          gregorianDate.SetDate(&utcDate);
  
          if (!gregorianDate.IsValid())
             throw TimeConverterException();
  
          newTime = gregorianDate.GetDate();
       }
    }

    else if (fromDateFormat == "UTCGregorian" && toDateFormat != "UTCGregorian")
    {
       GregorianDate gregorianDate(time);

       if (!gregorianDate.IsValid())
          throw TimeConverterException();
 
       UtcDate utcDate(gregorianDate.GetYMDHMS());

       // Get UTC MJD 
       Real mjd = ModifiedJulianDate(utcDate.GetYear(),
                                     utcDate.GetMonth(),
                                     utcDate.GetDay(),
                                     utcDate.GetHour(),
                                     utcDate.GetMinute(),
                                     utcDate.GetSecond());

       if (toDateFormat == "TAIModJulian" || toDateFormat == "UTCModJulian")
       {
          realTime = mjd;
          if (toDateFormat == "TAIModJulian") 
              realTime = utcDate.ToA1Mjd();
  
          timeBuffer << realTime;
          newTime = timeBuffer.str();
       }
       else if (toDateFormat == "TAIGregorian")
       {
          // Convert to A1 MJD and then A1 Date 
          A1Mjd a1Mjd;
          mjd =  a1Mjd.UtcMjdToA1Mjd(mjd); 
          a1Mjd.Set(mjd);
          A1Date a1Date = a1Mjd.ToA1Date();
         
          gregorianDate.SetDate(&a1Date);
          newTime = gregorianDate.GetDate();
       }
       
    }

    return newTime;
}
