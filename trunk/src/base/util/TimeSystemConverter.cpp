//$Id$ 
//------------------------------------------------------------------------------
//                              TimeSystemConverter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Allison Greene
// Created: 2005/02/03
//
/**
 * Implements the TimeSystemConverter class
 */
//------------------------------------------------------------------------------

#include "TimeSystemConverter.hpp"
#include "A1Mjd.hpp"
#include "GregorianDate.hpp"
#include "DateUtil.hpp"            // for ModifiedJulianDate()
#include "StringUtil.hpp"          // for ToReal()
#include "GmatGlobal.hpp"          // for TIME_PRECISION
#include "MessageInterface.hpp"

//#define DEBUG_FIRST_CALL
//#define DEBUG_TIMECONVERTER_DETAILS
//#define DEBUG_GREGORIAN
//#define DEBUG_TIME_CONVERT

#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
   static Real lastValue = -9999999.999999e99;
#endif


using namespace GmatMathUtil;

static EopFile *theEopFile;
static LeapSecsFileReader *theLeapSecsFileReader;

//------------------------------------------------------------------------------
// Integer GetTimeTypeID(std::string &str) 
//------------------------------------------------------------------------------
Integer TimeConverterUtil::GetTimeTypeID(std::string &str) 
{
    for (Integer i = 0; i < TimeSystemCount; i++)
    {
        if (str == TIME_SYSTEM_TEXT[i])
            return i;
    }

    return -1;
}

//---------------------------------------------------------------------------
//  Real TimeConverterUtil::Convert(const Real origValue,
//                                  const std::string &fromType,
//                                  const std::string &toType)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TimeConverter structures.
 *
 * @param <origValue>            Given Time
 * @param <fromType>  Time which is converted from date format
 * @param <toType>    Tiem which is converted to date format
 *
 * @return Converted time from the specific data format 
 */
Real TimeConverterUtil::Convert(const Real origValue,
                                const Integer fromType,
                                const Integer toType,
                                Real refJd)
{   
   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == true) && (origValue < (lastValue-0.25)))
      {
         firstCallFired = false;
         MessageInterface::ShowMessage(
            "Reset firstCallFired, orig == '%.12lf', last = '%.12lf'\n",
            origValue, lastValue);
      }
      lastValue = origValue;
   #endif
   
   #ifdef DEBUG_TIMECONVERTER_DETAILS
      MessageInterface::ShowMessage(
         "      TimeConverterUtil::Converting %.18lf in %s to %s; refJD = %.18lf\n", origValue, 
         TIME_SYSTEM_TEXT[fromType].c_str(), TIME_SYSTEM_TEXT[toType].c_str(), refJd);
   #endif
      
   Real newTime =
      TimeConverterUtil::ConvertToTaiMjd(fromType, origValue, refJd);

   #ifdef DEBUG_TIMECONVERTER_DETAILS
      MessageInterface::ShowMessage("      TAI time =  %.18lf\n", newTime);
   #endif
   
   Real returnTime =
      TimeConverterUtil::ConvertFromTaiMjd(toType, newTime, refJd);

   #ifdef DEBUG_FIRST_CALL
//       if (toType == "TtMjd")
      if (toType == TTMJD)
         firstCallFired = true;
   #endif

   #ifdef DEBUG_TIMECONVERTER_DETAILS
      MessageInterface::ShowMessage("      %s time =  %.18lf\n", TIME_SYSTEM_TEXT[toType].c_str(), 
         returnTime);
   #endif

   return returnTime;
}

//---------------------------------------------------------------------------
// Real ConvertToTaiMjd(std::string fromType, Real origValue, Real refJd)
//---------------------------------------------------------------------------
Real TimeConverterUtil::ConvertToTaiMjd(Integer fromType, Real origValue,
                                        Real refJd)
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   Converting %.11lf to TAI from %s\n", origValue, TIME_SYSTEM_TEXT[fromType].c_str());
   #endif

   #ifdef DEBUG_TIMECONVERTER_DETAILS
      MessageInterface::ShowMessage( 
         "      ***Converting %.18lf to TAI from %s\n", origValue, 
         TIME_SYSTEM_TEXT[fromType].c_str());
   #endif
   
   switch(fromType)
   {
    case TimeConverterUtil::A1MJD:
    case TimeConverterUtil::A1:
        return (origValue -
             (GmatTimeUtil::A1_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));
    case TimeConverterUtil::TAIMJD:
    case TimeConverterUtil::TAI:
        return origValue;
    case TimeConverterUtil::UTCMJD:
    case TimeConverterUtil::UTC:
    {
        Real offsetValue = 0;
    
        if (refJd != GmatTimeUtil::JD_NOV_17_1858)
        {
           // DJC: 6/16/05 Reversed order of difference so future times are positive
           // offsetValue = GmatTimeUtil::JD_NOV_17_1858 - refJd;
           offsetValue = refJd - GmatTimeUtil::JD_NOV_17_1858;
        }
    
        //loj: 4/12/05 Added
        if (theLeapSecsFileReader == NULL)
           throw TimeFileException
              ("theLeapSecsFileReader is unknown\n");
          
        // look up leap secs from file
        Real numLeapSecs =
           theLeapSecsFileReader->NumberOfLeapSecondsFrom(origValue + offsetValue);
  
        #ifdef DEBUG_TIMECONVERTER_DETAILS
           MessageInterface::ShowMessage(
              "      CVT to TAI, Leap secs count = %.14lf\n", numLeapSecs);
        #endif
    
        return (origValue + (numLeapSecs/GmatTimeUtil::SECS_PER_DAY));
    }
    case TimeConverterUtil::UT1MJD:
    case TimeConverterUtil::UT1:
    {
        if (theEopFile == NULL)
           throw TimeFileException(
                 "EopFile is unknown\n");
   
        Real offsetValue = 0;
  
        if (refJd != GmatTimeUtil::JD_NOV_17_1858)
        {
           offsetValue = GmatTimeUtil::JD_NOV_17_1858 - refJd;
        }
    
        Real ut1Offset = theEopFile->GetUt1UtcOffset(origValue + offsetValue);
        Real utcOffset = theEopFile->GetUt1UtcOffset((origValue + offsetValue)
            - (ut1Offset/GmatTimeUtil::SECS_PER_DAY));
    
        return (TimeConverterUtil::ConvertToTaiMjd(TimeConverterUtil::UTCMJD,
                 (origValue - (utcOffset/GmatTimeUtil::SECS_PER_DAY)), refJd));
    }
    case TimeConverterUtil::TDBMJD:
    case TimeConverterUtil::TDB:
          throw UnimplementedException(
               "Not Implement - TDB to TAI");
//      Real tdbJd = origValue + jdOffset;
//      Real taiJd = ((tdbJd - ((TDB_COEFF1 *sin(m_E)) +
//                    (TDB_COEFF2 * sin(2 * m_E)))) * T_TT_COEFF1) - T_TT_OFFSET;
//      return (taiJd - jdOffset);
    case TimeConverterUtil::TCBMJD:
    case TimeConverterUtil::TCB:
          throw UnimplementedException(
               "Not Implement - TCB to TAI");
//      Real tdbMjd;
//      return ConvertToTaiMjd("TdbMjd", tdbMjd);    
    case TimeConverterUtil::TTMJD:
    case TimeConverterUtil::TT:
          return (origValue -
             (GmatTimeUtil::TT_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));
    default:
         ;
   }
   return 0.0;

}


//---------------------------------------------------------------------------
// Real ConvertFromTaiMjd(std::string toType, Real origValue, Real refJd)
//---------------------------------------------------------------------------
Real TimeConverterUtil::ConvertFromTaiMjd(Integer toType, Real origValue,
                                          Real refJd)
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   Converting %.11lf from TAI to %s\n", origValue,
            TIME_SYSTEM_TEXT[toType].c_str());
   #endif
   #ifdef DEBUG_TIMECONVERTER_DETAILS
      MessageInterface::ShowMessage(
         "      ** Converting %.18lf from TAI to %s\n", origValue, 
         TIME_SYSTEM_TEXT[toType].c_str());
   #endif
   
   switch (toType)
   {
      case TimeConverterUtil::A1MJD:
      case TimeConverterUtil::A1:
      {
          #ifdef DEBUG_TIMECONVERTER_DETAILS
             MessageInterface::ShowMessage("      In the 'a1' block\n");
          #endif
          return (origValue +
                 (GmatTimeUtil::A1_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));      
      }
      case TimeConverterUtil::TAIMJD:
      case TimeConverterUtil::TAI:
      {
          #ifdef DEBUG_TIMECONVERTER_DETAILS
             MessageInterface::ShowMessage("      In the 'tai' block\n");
          #endif
          // already in tai
          return origValue;
      }
      case TimeConverterUtil::UTCMJD:
      case TimeConverterUtil::UTC:
       {
          #ifdef DEBUG_TIMECONVERTER_DETAILS
             MessageInterface::ShowMessage("      In the 'utc' block\n");
          #endif
          Real offsetValue = 0;
    
          if (refJd != GmatTimeUtil::JD_NOV_17_1858)
          {
             //offsetValue = GmatTimeUtil::JD_NOV_17_1858 - refJd;
             offsetValue = refJd - GmatTimeUtil::JD_NOV_17_1858;
          }
    
          //loj: 4/12/05 Added
          if (theLeapSecsFileReader == NULL)
             throw TimeFileException
                ("theLeapSecsFileReader is unknown\n");
          
          Real taiLeapSecs =
             theLeapSecsFileReader->NumberOfLeapSecondsFrom(origValue + offsetValue);
          
          Real utcLeapSecs =
             theLeapSecsFileReader->
             NumberOfLeapSecondsFrom((origValue + offsetValue)
                                     - (taiLeapSecs/GmatTimeUtil::SECS_PER_DAY));
    
          #ifdef DEBUG_TIMECONVERTER_DETAILS
             MessageInterface::ShowMessage("      offsetValue = %.17lf\n",
                offsetValue);
             MessageInterface::ShowMessage("      Leap secs: tai = %.17lf, utc = %.17lf\n",
                taiLeapSecs, utcLeapSecs);
          #endif
    
          if (utcLeapSecs == taiLeapSecs)
             return (origValue - (taiLeapSecs/GmatTimeUtil::SECS_PER_DAY));
          else
             return (origValue - (utcLeapSecs/GmatTimeUtil::SECS_PER_DAY));
       }
      case TimeConverterUtil::UT1MJD:
      case TimeConverterUtil::UT1:
       {
          #ifdef DEBUG_TIMECONVERTER_DETAILS
             MessageInterface::ShowMessage("      In the 'ut1' block\n");
          #endif
          if (theEopFile == NULL)
             throw TimeFileException(
                   "EopFile is unknown");
    
          Real offsetValue = 0;
    
          if (refJd != GmatTimeUtil::JD_NOV_17_1858)
          {
             //offsetValue = GmatTimeUtil::JD_NOV_17_1858 - refJd;
             offsetValue = refJd - GmatTimeUtil::JD_NOV_17_1858;
          }
          // convert origValue to utc
          Real utcMjd = TimeConverterUtil::ConvertFromTaiMjd(TimeConverterUtil::UTCMJD, 
                    origValue, refJd);
          Real numOffset = 0;
    
          numOffset = theEopFile->GetUt1UtcOffset(utcMjd + offsetValue);
    
          #ifdef DEBUG_TIMECONVERTER_DETAILS
             MessageInterface::ShowMessage("   offsetValue = %.17lf\n",
                offsetValue);
             MessageInterface::ShowMessage(
                "   utcMjd = %.20lf, numOffset = %.20lf\n", utcMjd, numOffset);
          #endif
          #ifdef DEBUG_FIRST_CALL
             if (!firstCallFired || (numOffset == 0.0))
                MessageInterface::ShowMessage(
                   "   utcMjd = %.20lf, numOffset = %.20lf\n", utcMjd, numOffset);
          #endif
    
          // add delta ut1 read from eop file
          //return (utcMjd + numOffset);
          return (utcMjd + (numOffset/GmatTimeUtil::SECS_PER_DAY));
       }
      case TimeConverterUtil::TDBMJD:
      case TimeConverterUtil::TDB:      
       {
          #ifdef DEBUG_TIMECONVERTER_DETAILS
             MessageInterface::ShowMessage("      In the 'tdb' block\n");
          #endif
          // convert time to tt
          Real ttJd = TimeConverterUtil::ConvertFromTaiMjd(TimeConverterUtil::TTMJD, 
                origValue, refJd);
          // convert to ttJD
          ttJd += refJd;
    
          // compute T_TT
          Real t_TT = (ttJd - T_TT_OFFSET) / T_TT_COEFF1;
          // compute M_E
          Real m_E = (M_E_OFFSET + (M_E_COEFF1 * t_TT)) * GmatMathUtil::RAD_PER_DEG;
          Real offset = ((TDB_COEFF1 *Sin(m_E)) + (TDB_COEFF2 * Sin(2 * m_E))) / 
                        GmatTimeUtil::SECS_PER_DAY ;
          Real tdbJd = ttJd + offset;
          return tdbJd - refJd;
       }      
       case TimeConverterUtil::TCBMJD:
       case TimeConverterUtil::TCB:
       {
          #ifdef DEBUG_TIMECONVERTER_DETAILS
             MessageInterface::ShowMessage("      In the 'tcb' block\n");
          #endif
          // convert time to tdb
          Real tdbMjd = TimeConverterUtil::ConvertFromTaiMjd(TimeConverterUtil::TDBMJD, 
                origValue, refJd);
          //Real jdValue = origValue;  // but this is TAI
          Real jdValue = tdbMjd;
          //Real offset = L_B * ((jdValue + refJd) - TCB_JD_MJD_OFFSET) * GmatTimeUtil::SECS_PER_DAY;
          Real offset = L_B * ((jdValue + refJd) - TCB_JD_MJD_OFFSET);
          // units of offset are in seconds, so convert to fraction of days
          //return ((offset / GmatTimeUtil::SECS_PER_DAY) + tdbMjd);
          return (offset + tdbMjd);
       }
       case TimeConverterUtil::TTMJD:
       case TimeConverterUtil::TT:
       {
          #ifdef DEBUG_TIMECONVERTER_DETAILS
             MessageInterface::ShowMessage("      In the 'tt' block\n");
          #endif
          return (origValue +
                 (GmatTimeUtil::TT_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));
       }       
       default:
        ;
   }

   return 0;
}


//---------------------------------------------------------------------------
// void SetEopFile(EopFile *eopFile)
//---------------------------------------------------------------------------
void TimeConverterUtil::SetEopFile(EopFile *eopFile)
{
   theEopFile = eopFile;
}


//---------------------------------------------------------------------------
// void SetLeapSecsFileReader(LeapSecsFileReader *leapSecsFileReader)
//---------------------------------------------------------------------------
void TimeConverterUtil::SetLeapSecsFileReader(LeapSecsFileReader *leapSecsFileReader)
{
   theLeapSecsFileReader = leapSecsFileReader;
}


//---------------------------------------------------------------------------
// void GetTimeSystemAndFormat(const std::string &type, std::string &system,
//                             std::string &format)
//---------------------------------------------------------------------------
/*
 * Returns time system and format from the input format.
 * For example, TAIModJulian gives TAI and ModJulian.
 *
 * @param  type    input time type
 * @param  system  output time type
 * @param  format  output time format
 *
 * @exception <TimeFormatException> thrown if input type is invalid
 */
//---------------------------------------------------------------------------
void TimeConverterUtil::GetTimeSystemAndFormat(const std::string &type,
                                               std::string &system,
                                               std::string &format)
{
   //-------------------------------------------------------
   // Get from time system and format
   //-------------------------------------------------------
   std::string::size_type loc = type.find("ModJulian", 0);
   if (loc == type.npos)
      loc = type.find("Gregorian", 0);
   
   if (loc == type.npos)
      throw TimeFormatException
         ("\"" + type + "\" is not a valid time format.\n"
          "The allowed values are: [A1ModJulian, TAIModJulian, UTCModJulian, "
          "TTModJulian, A1Gregorian, TAIGregorian, UTCGregorian, TTGregorian]");
   
   system = type.substr(0, loc);
   format = type.substr(loc);
}


//---------------------------------------------------------------------------
// std::string TimeConverterUtil::ConvertMjdToGregorian(const Real mjd)
//---------------------------------------------------------------------------
std::string TimeConverterUtil::ConvertMjdToGregorian(const Real mjd)
{
   A1Mjd a1Mjd(mjd);
   A1Date a1Date = a1Mjd.ToA1Date();
   GregorianDate gregorianDate(&a1Date);
    #ifdef DEBUG_GREGORIAN
       MessageInterface::ShowMessage("------ In ConvertMjdToGregorian\n");
       MessageInterface::ShowMessage("------ input mjd     = %.18lf\n", mjd);
       MessageInterface::ShowMessage("------ A1Date        = %s\n", 
          (a1Date.ToPackedCalendarString()).c_str());
       MessageInterface::ShowMessage("------ GregorianDate = %s\n", 
          (gregorianDate.GetDate()).c_str());
    #endif
   return gregorianDate.GetDate();
}


//---------------------------------------------------------------------------
// Real TimeConverterUtil::ConvertGregorianToMjd(const std::string &greg)
//---------------------------------------------------------------------------
Real TimeConverterUtil::ConvertGregorianToMjd(const std::string &greg)
{
   GregorianDate gregorianDate(greg);
   Real jules;

   if (!gregorianDate.IsValid())
      throw TimeFormatException(
         "Gregorian date '" + greg + "' is not valid.");

   //MessageInterface::ShowMessage
   //   ("==> TimeConverterUtil::ConvertGregorianToMjd() greg=%s\n", greg.c_str());
   
   try
   {
      A1Date a1Date(gregorianDate.GetYMDHMS());
      
      #ifdef DEBUG_TIMECONVERTER_DETAILS
         MessageInterface::ShowMessage("Gregorian: %s\n", 
            gregorianDate.GetYMDHMS().c_str());
         MessageInterface::ShowMessage("YMDHMS:    %d  %d  %d  %d  %d  %.11lf\n", 
            a1Date.GetYear(),a1Date.GetMonth(), a1Date.GetDay(),
            a1Date.GetHour(), a1Date.GetMinute(),a1Date.GetSecond());
      #endif
      
      jules = ModifiedJulianDate(a1Date.GetYear(),a1Date.GetMonth(),
                                 a1Date.GetDay(),a1Date.GetHour(),
                                 a1Date.GetMinute(),a1Date.GetSecond());
    #ifdef DEBUG_GREGORIAN
       MessageInterface::ShowMessage("------ In ConvertGregorianToMjd\n");
       MessageInterface::ShowMessage("------ input greg = %s\n", greg.c_str());
       MessageInterface::ShowMessage("------ Gregorian  = %s\n", 
            gregorianDate.GetYMDHMS().c_str());
       MessageInterface::ShowMessage("------ A1Date     = %s\n", 
          (a1Date.ToPackedCalendarString()).c_str());
       MessageInterface::ShowMessage("------ YMDHMS:    =     %d  %d  %d  %d  %d  %.17lf\n", 
            a1Date.GetYear(),a1Date.GetMonth(), a1Date.GetDay(),
            a1Date.GetHour(), a1Date.GetMinute(),a1Date.GetSecond());
       MessageInterface::ShowMessage("------ jules      = %s\n", 
          (gregorianDate.GetDate()).c_str());
       MessageInterface::ShowMessage("------ jules (as Real)      = %.12f\n", 
          jules);
    #endif
   }
   catch (Date::TimeRangeError& e)
   {
      throw TimeFormatException(
         "Gregorian date '" + greg +"' appears to be out of range.");
   }

   //MessageInterface::ShowMessage
   //   ("==> TimeConverterUtil::ConvertGregorianToMjd() jules=%.11f\n", jules);
   
   return jules;
}


//---------------------------------------------------------------------------
// void Convert(const std::string &fromType, Real fromMjd,
//              const std::string &fromStr, const std::string &toType,
//              Real &toMjd, std::string &toStr)
//---------------------------------------------------------------------------
/*
 * Converts input time and time format to output format. If input fromMjd
 * is -999.999 it uses fromStr to get a input value.
 *
 * @param  fromType  input time system and format (A1ModJulian, etc)
 * @param  fromMjd   input time in mjd Real if fromType is ModJulian
 * @param  fromStr   input time in string to be used if fromMjd is -999.999
 * @param  toType    output time system and format (A1ModJulian, etc)
 * @param  toMjd     output time in mjd Real if toType is ModJulian, -999.999 otherwise
 * @param  toStr     output time string in toType format
 */
//---------------------------------------------------------------------------
void TimeConverterUtil::Convert(const std::string &fromType, Real fromMjd, 
                                const std::string &fromStr,
                                const std::string &toType, Real &toMjd,
                                std::string &toStr)
{
   #ifdef DEBUG_TIME_CONVERT
   MessageInterface::ShowMessage
      ("TimeConverterUtil::Convert() entered fromType=%s, fromMjd=%f, fromStr=%s\n"
       "   toType=%s\n", fromType.c_str(), fromMjd, fromStr.c_str(), toType.c_str());
   #endif
   
   Real fromMjdVal = fromMjd;
   bool convertToModJulian = false;
   Integer timePrecision = GmatGlobal::Instance()->GetTimePrecision();
   
   if (fromMjd == -999.999)
      convertToModJulian = true;
   
   //-------------------------------------------------------
   // Get from time system and format
   //-------------------------------------------------------
   std::string fromSystem ;
   std::string fromFormat;
   GetTimeSystemAndFormat(fromType, fromSystem, fromFormat);
   
   // Validate from time system
   if (!TimeConverterUtil::ValidateTimeSystem(fromSystem))
      throw TimeFormatException
         ("\"" + fromSystem + "\" is not a valid time system");
   
   // Validate time format and value
   if (convertToModJulian)
      ValidateTimeFormat(fromFormat, fromStr);
   
   //-------------------------------------------------------
   // Get to time system and format
   //-------------------------------------------------------   
   std::string toSystem;
   std::string toFormat;
   GetTimeSystemAndFormat(toType, toSystem, toFormat);
   
   // Validate to time system
   if (!TimeConverterUtil::ValidateTimeSystem(toSystem))
      throw TimeFormatException
         ("\"" + toSystem + "\" is not a valid time system");
   
   //-------------------------------------------------------
   // Compute from time in mjd
   //-------------------------------------------------------
   toMjd = -999.999;
   
   if (fromFormat == "ModJulian")
   {
      if (convertToModJulian)
      {
         std::stringstream str;
         str.precision(timePrecision);
         str.str(fromStr);
         str >> fromMjdVal;
      }
   }
   else
   {
      #ifdef DEBUG_TIME_CONVERT
      MessageInterface::ShowMessage("===> Converting %s from Gregorian to MJD\n",
      fromStr.c_str());
      #endif
      fromMjdVal = TimeConverterUtil::ConvertGregorianToMjd(fromStr);
   }
   
   #ifdef DEBUG_TIME_CONVERT
   MessageInterface::ShowMessage("===> fromMjdVal=%.12f\n", fromMjdVal);
   MessageInterface::ShowMessage("===> fromType=%s\n", fromType.c_str());
   MessageInterface::ShowMessage("===> toType=%s\n", toType.c_str());
   #endif
   
   //-------------------------------------------------------
   // Compute to time in mjd
   //-------------------------------------------------------
   if (fromType != toType)
   {
      Integer fromId = TimeConverterUtil::GetTimeTypeID(fromSystem);
      Integer toId = TimeConverterUtil::GetTimeTypeID(toSystem);
      toMjd = TimeConverterUtil::Convert(fromMjdVal, fromId, 
                                         toId, GmatTimeUtil::JD_JAN_5_1941);
   }
   else
   {
      //toMjd = fromMjd;  // wcs 2007.05.01 A1ModJulian fix for Bug 807
      toMjd = fromMjdVal;
   }
   
   //-------------------------------------------------------
   // Convert to output format
   //-------------------------------------------------------
   if (toFormat == "ModJulian")
      toStr = GmatStringUtil::ToString(toMjd, timePrecision);
   else
      toStr = TimeConverterUtil::ConvertMjdToGregorian(toMjd);
   
   #ifdef DEBUG_TIME_CONVERT
   MessageInterface::ShowMessage
      ("TimeConverterUtil::Convert() leaving toMjd=%f, toStr=%s\n", toMjd,
       toStr.c_str());
   #endif
}


//---------------------------------------------------------------------------
// bool TimeConverterUtil::ValidateTimeSystem(std::string sys)
//---------------------------------------------------------------------------
bool TimeConverterUtil::ValidateTimeSystem(std::string sys)
{
   //for (Integer i = 0; i < 13; ++i)
   for (Integer i = 0; i < TimeSystemCount; ++i)
      if (TIME_SYSTEM_TEXT[i] == sys)
         return true;
         
   return false;
}


//---------------------------------------------------------------------------
// bool TimeConverterUtil::ValidateTimeFormat(const std::string &format, 
//                                            const std::string &value)
//---------------------------------------------------------------------------
bool TimeConverterUtil::ValidateTimeFormat(const std::string &format, 
                                           const std::string &value)
{
   bool retval = false;
   
   std::string timeFormat = "ModJulian";
   if (format.find("Gregorian") != format.npos)
      timeFormat = "Gregorian";

   #if DEBUG_VALIDATE_TIME
   MessageInterface::ShowMessage
      ("TimeConverterUtil::ValidateTimeFormat() format=%s, timeFormat=%s, "
       "value=%s\n", format.c_str(), timeFormat.c_str(), value.c_str());
   #endif
   
   if (timeFormat == "Gregorian")
   {
      retval = DateUtil::IsValidGregorian(value);
      
      if (!retval)
          throw TimeFormatException
             ("Gregorian date \"" + value + "\" is not valid.");
      
//       GregorianDate gregorianDate(value);
      
//       if (!gregorianDate.IsValid())
//          throw TimeFormatException
//             ("Gregorian date \"" + value + "\" is not valid.");
      
//       retval = true;
   }
   else if (timeFormat == "ModJulian")
   {
      Real rval;
      if (GmatStringUtil::ToReal(value, rval))
      {
         // Sputnik launched Oct 4, 1957 = 6116 MJ; don't accept earlier epochs.
         if (rval >= 6116)
            retval = true;
      }
      
      if (!retval)
         throw InvalidTimeException
            ("ModJulian Time \"" + value + "\" is not valid.");
      
   }
   else
   {
      throw TimeFormatException
         ("Invalid Time Format \"" + format + "\" found.");
   }
   
   return true;
}


//---------------------------------------------------------------------------
// StringArray TimeConverterUtil::GetValidTimeRepresentations()
//---------------------------------------------------------------------------
StringArray TimeConverterUtil::GetValidTimeRepresentations()
{
   StringArray systems;
   for (Integer i = A1; i < TimeSystemCount; ++i)
   {
      if ((i != UT1) && (i != TDB) && (i != TCB))
         systems.push_back(TIME_SYSTEM_TEXT[i] + "ModJulian");
   }
   for (Integer i = A1; i < TimeSystemCount; ++i)
   {
      if ((i != UT1) && (i != TDB) && (i != TCB))
         systems.push_back(TIME_SYSTEM_TEXT[i] + "Gregorian");
   }
   return systems;
}
