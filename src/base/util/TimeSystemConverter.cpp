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
      // look up leap secs from file
      Real numLeapSecs =
            theLeapSecsFileReader->NumberOfLeapSecondsFrom(origValue);
      return (origValue + (numLeapSecs/GmatTimeUtil::SECS_PER_DAY));
   }
   // ut1
   else if (fromType == TIME_SYSTEM_TEXT[2])
   {
      if (theEopFile == NULL)
         throw TimeSystemConverterExceptions::FileException(
               "EopFile is unknown");

      Real ut1Offset = theEopFile->GetUt1UtcOffset(origValue);
      Real utcOffset = theEopFile->GetUt1UtcOffset(origValue - ut1Offset);

      return (TimeConverterUtil::ConvertToTaiMjd("UtcMjd", (origValue - utcOffset),
               refJd));

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


Real TimeConverterUtil::ConvertFromTaiMjd(std::string toType, Real origValue,
      Real refJd)
{
   if (toType == TIME_SYSTEM_TEXT[0])
   {
      return (origValue +
             (GmatTimeUtil::A1_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));
   }
   // utc
   else if (toType == TIME_SYSTEM_TEXT[1])
   {
      Real taiLeapSecs =
            theLeapSecsFileReader->NumberOfLeapSecondsFrom(origValue);

      Real utcLeapSecs =
            theLeapSecsFileReader->NumberOfLeapSecondsFrom(origValue -
            (taiLeapSecs/GmatTimeUtil::SECS_PER_DAY));

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

      // convert origValue to utc
      Real utcMjd = TimeConverterUtil::ConvertFromTaiMjd("UtcMjd", origValue,
            refJd);
      Real numOffset = 0;

      numOffset = theEopFile->GetUt1UtcOffset(utcMjd);

      // add delta ut1 read from eop file
      return (utcMjd + numOffset);
   }
   // tdb
   else if (toType == TIME_SYSTEM_TEXT[3])
   {
      // convert time to tt
      Real ttJd = TimeConverterUtil::ConvertFromTaiMjd("TtMjd", origValue);
      // convert to ttJD
      ttJd += refJd;

      // compute T_TT
      Real t_TT = (ttJd - T_TT_OFFSET) / T_TT_COEFF1;
      // compute M_E
      Real m_E = M_E_OFFSET + (M_E_COEFF1 * t_TT);

      Real tdbJd = ttJd + (TDB_COEFF1 *Sin(m_E)) + (TDB_COEFF2 * Sin(2 * m_E));
      return tdbJd - refJd;
   }
   // tcb
   else if (toType == TIME_SYSTEM_TEXT[4])
   {
      // convert time to tdb
      Real tdbMjd = TimeConverterUtil::ConvertFromTaiMjd("TdbMjd", origValue,
         refJd);
      return ((L_B * (origValue - TCB_JD_MJD_OFFSET) * NUM_SECS) + tdbMjd);
   }
   // tt
   else if (toType == TIME_SYSTEM_TEXT[5])
   {
      return (origValue +
             (GmatTimeUtil::TT_TAI_OFFSET/GmatTimeUtil::SECS_PER_DAY));
   }

   return 0;
}

bool TimeConverterUtil::SetEopFile(EopFile *eopFile)
{
   theEopFile = eopFile;
   return true;
}

bool TimeConverterUtil::SetLeapSecsFileReader(LeapSecsFileReader *leapSecsFileReader)
{
   theLeapSecsFileReader = leapSecsFileReader;
   return true;
}


