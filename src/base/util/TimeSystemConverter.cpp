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
using namespace std;

//---------------------------------
// static data
//---------------------------------
const std::string
TimeSystemConverter::TIME_SYSTEM_TEXT[6] =
{
   "A1Mjd",
   "UtcMjd",
   "Ut1Mjd",
   "TdbMjd",
   "TcbMjd",
   "TtMjd",
};

// Specified in Math Spec section 2.3
const Real TimeSystemConverter::TDB_COEFF1               = 0.001658;
const Real TimeSystemConverter::TDB_COEFF2               = 0.00001385;
const Real TimeSystemConverter::M_E_OFFSET               = 357.5277233;
const Real TimeSystemConverter::M_E_COEFF1               = 35999.05034;
const Real TimeSystemConverter::T_TT_OFFSET              = 2451545.0;
const Real TimeSystemConverter::T_TT_COEFF1              = 36525;
// from email from Wendy 1/26/05
const Real TimeSystemConverter::L_B                      = 1.550505e-8;
const Real TimeSystemConverter::TCB_JD_MJD_OFFSET        = 2443144.5;
const Real TimeSystemConverter::NUM_SECS                 = 86400;

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  TimeSystemConverter()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 *
 */
TimeSystemConverter::TimeSystemConverter()
{
   jdOffset = 2400000.5;
}

//---------------------------------------------------------------------------
//  TimeSystemConverter(const std::string &type)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <typeStr> GMAT script string associated with this type of object.
 *
 */
TimeSystemConverter::TimeSystemConverter(const std::string &type)
{
}

//---------------------------------------------------------------------------
//  TimeSystemConverter(const TimeSystemConverter &TimeSystemConverter)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base TimeSystemConverter structures.
 *
 * @param <stateConverter> The original that is being copied.
 */
TimeSystemConverter::TimeSystemConverter(const TimeSystemConverter &TimeSystemConverter)
{
}

//---------------------------------------------------------------------------
//  ~TimeSystemConverter()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
TimeSystemConverter::~TimeSystemConverter()
{
}

//---------------------------------------------------------------------------
//  TimeSystemConverter& operator=(const TimeSystemConverter &TimeSystemConverter)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TimeSystemConverter structures.
 *
 * @param <converter> The original that is being copied.
 *
 * @return Reference to this object
 */
TimeSystemConverter& TimeSystemConverter::operator=(const TimeSystemConverter &TimeSystemConverter)
{
    // Don't do anything if copying self
    if (&TimeSystemConverter == this)
        return *this;

    // Will fix it later
    return *this;
}

//---------------------------------------------------------------------------
//  Real TimeSystemConverter::Convert(const Real time,
//                              const std::string &fromDateFormat,
//                              const std::string &toDateFormat)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TimeSystemConverter structures.
 *
 * @param <time>            Given Time 
 * @param <fromDateFormat>  Time which is converted from date format
 * @param <toDateFormat>    Tiem which is converted to date format 
 *
 * @return Converted time from the specific data format 
 */
Real TimeSystemConverter::Convert(const Real time,
                        const std::string &fromTimeFormat,
                        const std::string &toTimeFormat)
{
    return 0;
}

Real TimeSystemConverter::ConvertToTaiMjd(std::string fromTimeFormat, Real time)
{
   // a1mjd
   if (fromTimeFormat == TIME_SYSTEM_TEXT[0])
   {
      return (time - GmatTimeUtil::A1_TAI_OFFSET);
   }
   // utc
   else if (fromTimeFormat == TIME_SYSTEM_TEXT[1])
   {
      // look up leap secs from file
      Real numLeapSecs =
            theLeapSecsFileReader->NumberOfLeapSecondsFrom(time);
      return (time + numLeapSecs);
   }
   // ut1
   else if (fromTimeFormat == TIME_SYSTEM_TEXT[2])
   {
      Real ut1Offset = theEopFile->GetUt1UtcOffset(time);
      Real utcOffset = theEopFile->GetUt1UtcOffset(time - ut1Offset);

      return (ConvertToTaiMjd("UtcMjd", (time - utcOffset)));

   }
   // tdb
   else if (fromTimeFormat == TIME_SYSTEM_TEXT[3])
   {
      throw TimeSystemConverter::ImplementationException("Not Implement - TDB to TAI");
//      Real tdbJd = time + jdOffset;
//      Real taiJd = ((tdbJd - ((TDB_COEFF1 *sin(m_E)) +
//                    (TDB_COEFF2 * sin(2 * m_E)))) * T_TT_COEFF1) - T_TT_OFFSET;
//      return (taiJd - jdOffset);
   }
   // tcb
   else if (fromTimeFormat == TIME_SYSTEM_TEXT[4])
   {
      throw TimeSystemConverter::ImplementationException("Not Implement - TCB to TAI");
//      Real tdbMjd;
//      return ConvertToTaiMjd("TdbMjd", tdbMjd);
   }
   // tt
   else if (fromTimeFormat == TIME_SYSTEM_TEXT[5])
   {
      return (time - GmatTimeUtil::TT_TAI_OFFSET);
   }

   return 0;
}


Real TimeSystemConverter::ConvertFromTaiMjd(std::string toTimeFormat, Real time)
{
   if (toTimeFormat == TIME_SYSTEM_TEXT[0])
   {
      return (time + GmatTimeUtil::A1_TAI_OFFSET);
   }
   // utc
   else if (toTimeFormat == TIME_SYSTEM_TEXT[1])
   {
       Real taiLeapSecs =
            theLeapSecsFileReader->NumberOfLeapSecondsFrom(time);

      // no leap second
      if (taiLeapSecs == 0)
         return time;
      // leap second
      else
      {
         // leap second falls in the middle
         Real utcLeapSecs =
               theLeapSecsFileReader->NumberOfLeapSecondsFrom(time - taiLeapSecs);

         if (utcLeapSecs == taiLeapSecs)
            return (time - taiLeapSecs);
         else
            return (time - utcLeapSecs);
      }
   }
   // ut1
   else if (toTimeFormat == TIME_SYSTEM_TEXT[2])
   {
      // convert time to utc
      Real utcMjd = ConvertFromTaiMjd("UtcMjd", time);
      Real numOffset = 0;

      if (theEopFile != NULL)
         numOffset = theEopFile->GetUt1UtcOffset(utcMjd);

      // add delta ut1 read from eop file
      return (utcMjd + numOffset);
   }
   // tdb
   else if (toTimeFormat == TIME_SYSTEM_TEXT[3])
   {
      // convert time to tt
      Real ttJd = ConvertFromTaiMjd("TtMjd", time);
      // convert to ttJD
      ttJd += jdOffset;

      // compute T_TT
      Real t_TT = (ttJd - T_TT_OFFSET) / T_TT_COEFF1;
      // compute M_E
      Real m_E = M_E_OFFSET + (M_E_COEFF1 * t_TT);

      Real tdbJd = ttJd + (TDB_COEFF1 *Sin(m_E)) + (TDB_COEFF2 * Sin(2 * m_E));
      return tdbJd - jdOffset;
   }
   // tcb
   else if (toTimeFormat == TIME_SYSTEM_TEXT[4])
   {
      // convert time to tdb
      Real tdbMjd = ConvertFromTaiMjd("TdbMjd", time);
      return ((L_B * (time - TCB_JD_MJD_OFFSET) * NUM_SECS) + tdbMjd);
   }
   // tt
   else if (toTimeFormat == TIME_SYSTEM_TEXT[5])
   {
      return (time + GmatTimeUtil::TT_TAI_OFFSET);
   }

   return 0;
}

bool TimeSystemConverter::SetEopFile(EopFile *eopFile)
{
   theEopFile = eopFile;
   return true;
}

bool TimeSystemConverter::SetLeapSecsFileReader(LeapSecsFileReader *leapSecsFileReader)
{
   theLeapSecsFileReader = leapSecsFileReader;
   return true;
}

