//$Header$ 
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
#include "MessageInterface.hpp"

//#define DEBUG_FIRST_CALL
//#define DEBUG_TIMECONVERTER_DETAILS

#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
   static Real lastValue = -9999999.999999e99;
#endif


using namespace GmatMathUtil;

static EopFile *theEopFile;
static LeapSecsFileReader *theLeapSecsFileReader;

//---------------------------------------------------------------------------
//  Real TimeConverterUtil::Convert(const Real origValue,
//                              const std::string &fromType,
//                              const std::string &toType)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TimeSystemConverter structures.
 *
 * @param <origValue>            Given Time
 * @param <fromType>  Time which is converted from date format
 * @param <toType>    Tiem which is converted to date format
 *
 * @return Converted time from the specific data format 
 */
Real TimeConverterUtil::Convert(const Real origValue,
                        const std::string &fromType,
                        const std::string &toType,
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
         "      Converting %.10lf in %s to %s; refJD = %.9lf\n", origValue, 
         fromType.c_str(), toType.c_str(), refJd);
   #endif
      
   Real newTime =
      TimeConverterUtil::ConvertToTaiMjd(fromType, origValue, refJd);

   #ifdef DEBUG_TIMECONVERTER_DETAILS
      MessageInterface::ShowMessage("      TAI time =  %.10lf\n", newTime);
   #endif
   
   Real returnTime =
      TimeConverterUtil::ConvertFromTaiMjd(toType, newTime, refJd);

   #ifdef DEBUG_FIRST_CALL
      if (toType == "TtMjd")
         firstCallFired = true;
   #endif

   #ifdef DEBUG_TIMECONVERTER_DETAILS
      MessageInterface::ShowMessage("      %s time =  %.10lf\n", toType.c_str(), 
         returnTime);
   #endif

   return returnTime;
}

//---------------------------------------------------------------------------
// Real ConvertToTaiMjd(std::string fromType, Real origValue, Real refJd)
//---------------------------------------------------------------------------
Real TimeConverterUtil::ConvertToTaiMjd(std::string fromType, Real origValue,
                                        Real refJd)
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   Converting %.10lf to TAI from %s\n", origValue, fromType.c_str());
   #endif

   #ifdef DEBUG_TIMECONVERTER_DETAILS
      MessageInterface::ShowMessage( 
         "      ***Converting %.10lf to TAI from %s\n", origValue, 
         fromType.c_str());
   #endif
   
   // a1mjd
   if ((fromType == TIME_SYSTEM_TEXT[0]) || (fromType == TIME_SYSTEM_TEXT[6]))
   {
      return (origValue -
             (GmatTimeUtil::A1_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));
   }
   // tai
   else if (fromType == TIME_SYSTEM_TEXT[7])
   {
      return origValue;
   }
   // utc
   else if ((fromType == TIME_SYSTEM_TEXT[1]) || (fromType == TIME_SYSTEM_TEXT[8]))
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
         throw TimeSystemConverterExceptions::FileException
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
   // ut1
   else if ((fromType == TIME_SYSTEM_TEXT[2]) || (fromType == TIME_SYSTEM_TEXT[9]))
   {
      if (theEopFile == NULL)
         throw TimeSystemConverterExceptions::FileException(
               "EopFile is unknown\n");

      Real offsetValue = 0;

      if (refJd != GmatTimeUtil::JD_NOV_17_1858)
      {
         offsetValue = GmatTimeUtil::JD_NOV_17_1858 - refJd;
      }

      Real ut1Offset = theEopFile->GetUt1UtcOffset(origValue + offsetValue);
      Real utcOffset = theEopFile->GetUt1UtcOffset((origValue + offsetValue)
          - (ut1Offset/GmatTimeUtil::SECS_PER_DAY));

      return (TimeConverterUtil::ConvertToTaiMjd("UtcMjd", (origValue -
               (utcOffset/GmatTimeUtil::SECS_PER_DAY)), refJd));
   }
   // tdb
   else if ((fromType == TIME_SYSTEM_TEXT[3]) || (fromType == TIME_SYSTEM_TEXT[10]))
   {
      throw TimeSystemConverterExceptions::ImplementationException(
               "Not Implement - TDB to TAI");
//      Real tdbJd = origValue + jdOffset;
//      Real taiJd = ((tdbJd - ((TDB_COEFF1 *sin(m_E)) +
//                    (TDB_COEFF2 * sin(2 * m_E)))) * T_TT_COEFF1) - T_TT_OFFSET;
//      return (taiJd - jdOffset);
   }
   // tcb
   else if ((fromType == TIME_SYSTEM_TEXT[4]) || (fromType == TIME_SYSTEM_TEXT[11]))
   {
      throw TimeSystemConverterExceptions::ImplementationException(
               "Not Implement - TCB to TAI");
//      Real tdbMjd;
//      return ConvertToTaiMjd("TdbMjd", tdbMjd);
   }
   // tt
   else if ((fromType == TIME_SYSTEM_TEXT[5]) || (fromType == TIME_SYSTEM_TEXT[12]))
   {
      return (origValue -
             (GmatTimeUtil::TT_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));
   }

   return 0.0;
}


//---------------------------------------------------------------------------
// Real ConvertFromTaiMjd(std::string toType, Real origValue, Real refJd)
//---------------------------------------------------------------------------
Real TimeConverterUtil::ConvertFromTaiMjd(std::string toType, Real origValue,
                                          Real refJd)
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   Converting %.10lf from TAI to %s\n", origValue, toType.c_str());
   #endif
   #ifdef DEBUG_TIMECONVERTER_DETAILS
      MessageInterface::ShowMessage(
         "      ** Converting %.10lf from TAI to %s\n", origValue, 
         toType.c_str());
   #endif
   // a1
   if ((toType == TIME_SYSTEM_TEXT[0]) || (toType == TIME_SYSTEM_TEXT[6])) 
   {
      #ifdef DEBUG_TIMECONVERTER_DETAILS
         MessageInterface::ShowMessage("      In the 'a1' block\n");
      #endif
      return (origValue +
             (GmatTimeUtil::A1_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));
   }
   // tai 
   else if (toType == TIME_SYSTEM_TEXT[7])
   {
      #ifdef DEBUG_TIMECONVERTER_DETAILS
         MessageInterface::ShowMessage("      In the 'tai' block\n");
      #endif
      // already in tai
      return origValue;
   }
   // utc
   else if ((toType == TIME_SYSTEM_TEXT[1]) || (toType == TIME_SYSTEM_TEXT[8]))
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
         throw TimeSystemConverterExceptions::FileException
            ("theLeapSecsFileReader is unknown\n");
      
      Real taiLeapSecs =
         theLeapSecsFileReader->NumberOfLeapSecondsFrom(origValue + offsetValue);
      
      Real utcLeapSecs =
         theLeapSecsFileReader->
         NumberOfLeapSecondsFrom((origValue + offsetValue)
                                 - (taiLeapSecs/GmatTimeUtil::SECS_PER_DAY));

      #ifdef DEBUG_TIMECONVERTER_DETAILS
         MessageInterface::ShowMessage("      Leap secs: tai = %.14lf, utc = %.14lf\n",
            taiLeapSecs, utcLeapSecs);
      #endif

      if (utcLeapSecs == taiLeapSecs)
         return (origValue - (taiLeapSecs/GmatTimeUtil::SECS_PER_DAY));
      else
         return (origValue - (utcLeapSecs/GmatTimeUtil::SECS_PER_DAY));
   }
   // ut1
   else if ((toType == TIME_SYSTEM_TEXT[2]) || (toType == TIME_SYSTEM_TEXT[9]))
   {
      #ifdef DEBUG_TIMECONVERTER_DETAILS
         MessageInterface::ShowMessage("      In the 'ut1' block\n");
      #endif
      if (theEopFile == NULL)
         throw TimeSystemConverterExceptions::FileException(
               "EopFile is unknown");

      Real offsetValue = 0;

      if (refJd != GmatTimeUtil::JD_NOV_17_1858)
      {
         //offsetValue = GmatTimeUtil::JD_NOV_17_1858 - refJd;
         offsetValue = refJd - GmatTimeUtil::JD_NOV_17_1858;
      }
      // convert origValue to utc
      Real utcMjd = TimeConverterUtil::ConvertFromTaiMjd("UtcMjd", origValue,
            refJd);
      Real numOffset = 0;

      numOffset = theEopFile->GetUt1UtcOffset(utcMjd + offsetValue);

      #ifdef DEBUG_FIRST_CALL
         if (!firstCallFired || (numOffset == 0.0))
            MessageInterface::ShowMessage(
               "   utcMjd = %lf, numOffset = %lf\n", utcMjd, numOffset);
      #endif

      // add delta ut1 read from eop file
      //return (utcMjd + numOffset);
      return (utcMjd + (numOffset/GmatTimeUtil::SECS_PER_DAY));
   }
   // tdb
   else if ((toType == TIME_SYSTEM_TEXT[3]) || (toType == TIME_SYSTEM_TEXT[10]))
   {
      #ifdef DEBUG_TIMECONVERTER_DETAILS
         MessageInterface::ShowMessage("      In the 'tdb' block\n");
      #endif
      // convert time to tt
      Real ttJd = TimeConverterUtil::ConvertFromTaiMjd("TtMjd", origValue, refJd);
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
   // tcb
   else if ((toType == TIME_SYSTEM_TEXT[4]) || (toType == TIME_SYSTEM_TEXT[11]))
   {
      #ifdef DEBUG_TIMECONVERTER_DETAILS
         MessageInterface::ShowMessage("      In the 'tcb' block\n");
      #endif
      // convert time to tdb
      Real tdbMjd = TimeConverterUtil::ConvertFromTaiMjd("TdbMjd", origValue,
         refJd);
      //Real jdValue = origValue;  // but this is TAI
      Real jdValue = tdbMjd;
      //Real offset = L_B * ((jdValue + refJd) - TCB_JD_MJD_OFFSET) * GmatTimeUtil::SECS_PER_DAY;
      Real offset = L_B * ((jdValue + refJd) - TCB_JD_MJD_OFFSET);
      // units of offset are in seconds, so convert to fraction of days
      //return ((offset / GmatTimeUtil::SECS_PER_DAY) + tdbMjd);
      return (offset + tdbMjd);
   }
   // tt
   else if ((toType == TIME_SYSTEM_TEXT[5]) || (toType == TIME_SYSTEM_TEXT[12]))
   {
      #ifdef DEBUG_TIMECONVERTER_DETAILS
         MessageInterface::ShowMessage("      In the 'tt' block\n");
      #endif
      return (origValue +
             (GmatTimeUtil::TT_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));
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


std::string TimeConverterUtil::ConvertMjdToGregorian(const Real mjd)
{
   A1Mjd a1Mjd(mjd);
   A1Date a1Date = a1Mjd.ToA1Date();
   GregorianDate gregorianDate(&a1Date);
   return gregorianDate.GetDate();
}


Real TimeConverterUtil::ConvertGregorianToMjd(const std::string &greg)
{
   GregorianDate gregorianDate(greg);
   Real jules;

   if (!gregorianDate.IsValid())
      throw TimeSystemConverterExceptions::TimeFormatException(
         "Gregorian date '" + greg + "' is not valid.");

   try
   {
      A1Date a1Date(gregorianDate.GetYMDHMS());
      jules = ModifiedJulianDate(a1Date.GetYear(),a1Date.GetMonth(),
                                 a1Date.GetDay(),a1Date.GetHour(),
                                 a1Date.GetMinute(),a1Date.GetSecond());
   }
   catch (Date::TimeRangeError& e)
   {
      throw TimeSystemConverterExceptions::TimeFormatException(
         "Gregorian date '" + greg +"' appears to be out of range.");
   }

   return jules;
}


bool TimeConverterUtil::ValidateTimeSystem(std::string sys)
{
   for (Integer i = 0; i < 13; ++i)
      if (TIME_SYSTEM_TEXT[i] == sys)
         return true;
         
   return false;
}


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
