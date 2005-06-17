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
   Real newTime =
      TimeConverterUtil::ConvertToTaiMjd(fromType, origValue, refJd);
   Real returnTime =
      TimeConverterUtil::ConvertFromTaiMjd(toType, newTime, refJd);
   return returnTime;
}

//---------------------------------------------------------------------------
// Real ConvertToTaiMjd(std::string fromType, Real origValue, Real refJd)
//---------------------------------------------------------------------------
Real TimeConverterUtil::ConvertToTaiMjd(std::string fromType, Real origValue,
                                        Real refJd)
{
   // a1mjd
   if (fromType == TIME_SYSTEM_TEXT[0])
   {
      return (origValue -
             (GmatTimeUtil::A1_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));
   }
   // utc
   else if (fromType == TIME_SYSTEM_TEXT[1])
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

      return (origValue + (numLeapSecs/GmatTimeUtil::SECS_PER_DAY));
   }
   // ut1
   else if (fromType == TIME_SYSTEM_TEXT[2])
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
   else if (fromType == TIME_SYSTEM_TEXT[3])
   {
      throw TimeSystemConverterExceptions::ImplementationException(
               "Not Implement - TDB to TAI");
//      Real tdbJd = origValue + jdOffset;
//      Real taiJd = ((tdbJd - ((TDB_COEFF1 *sin(m_E)) +
//                    (TDB_COEFF2 * sin(2 * m_E)))) * T_TT_COEFF1) - T_TT_OFFSET;
//      return (taiJd - jdOffset);
   }
   // tcb
   else if (fromType == TIME_SYSTEM_TEXT[4])
   {
      throw TimeSystemConverterExceptions::ImplementationException(
               "Not Implement - TCB to TAI");
//      Real tdbMjd;
//      return ConvertToTaiMjd("TdbMjd", tdbMjd);
   }
   // tt
   else if (fromType == TIME_SYSTEM_TEXT[5])
   {
      return (origValue -
             (GmatTimeUtil::TT_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));
   }

   return 0;
}


//---------------------------------------------------------------------------
// Real ConvertFromTaiMjd(std::string toType, Real origValue, Real refJd)
//---------------------------------------------------------------------------
Real TimeConverterUtil::ConvertFromTaiMjd(std::string toType, Real origValue,
                                          Real refJd)
{
   // a1
   if (toType == TIME_SYSTEM_TEXT[0])
   {
      return (origValue +
             (GmatTimeUtil::A1_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));
   }
   // utc
   else if (toType == TIME_SYSTEM_TEXT[1])
   {
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

      if (utcLeapSecs == taiLeapSecs)
         return (origValue - (taiLeapSecs/GmatTimeUtil::SECS_PER_DAY));
      else
         return (origValue - (utcLeapSecs/GmatTimeUtil::SECS_PER_DAY));
   }
   // ut1
   else if (toType == TIME_SYSTEM_TEXT[2])
   {
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

      // add delta ut1 read from eop file
      //return (utcMjd + numOffset);
      return (utcMjd + (numOffset/GmatTimeUtil::SECS_PER_DAY));
   }
   // tdb
   else if (toType == TIME_SYSTEM_TEXT[3])
   {
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
   else if (toType == TIME_SYSTEM_TEXT[4])
   {
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
   else if (toType == TIME_SYSTEM_TEXT[5])
   {
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



