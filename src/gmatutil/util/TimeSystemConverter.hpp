//$Id$
//------------------------------------------------------------------------------
//                                 TimeSystemConverter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Allison Greene
// Created: 2005/02/03
//
/**
 * Definition of the TimeSystemConverter class
 */
//------------------------------------------------------------------------------

#ifndef TimeSystemConverter_hpp
#define TimeSystemConverter_hpp

//#include <iostream>
//#include <iomanip>
//#include <sstream>
//#include "BaseException.hpp"
#include "EopFile.hpp"
#include "LeapSecsFileReader.hpp"
//#include "GmatConstants.hpp"

#include "GmatTime.hpp"

// struct TimeSystemConverterExceptions
// {
   class GMATUTIL_API UnimplementedException : public BaseException
   {
      public:
         UnimplementedException(const std::string &message =
         "TimeSystemConverter: Conversion not implemented: ")
         : BaseException(message) {};
   };
   class GMATUTIL_API TimeFileException : public BaseException
   {
      public:
         TimeFileException(const std::string &message =
         "TimeSystemConverter: File is unknown: ")
         : BaseException(message) {};
   };
   class GMATUTIL_API TimeFormatException : public BaseException
   {
      public:
         TimeFormatException(const std::string &message =
         "TimeSystemConverter: Requested format not implemented: ")
         : BaseException(message) {};
   };
   class GMATUTIL_API InvalidTimeException : public BaseException
   {
      public:
         InvalidTimeException(const std::string &message =
         "TimeSystemConverter: Requested time is invalid: ")
         : BaseException(message) {};
   };
// };

namespace TimeConverterUtil
{
   // Specified in Math Spec section 2.3
   static const Real TDB_COEFF1                    = 0.001658;
   static const Real TDB_COEFF2                    = 0.00001385;
   static const Real M_E_OFFSET                    = 357.5277233;
   static const Real M_E_COEFF1                    = 35999.05034;
   static const Real T_TT_OFFSET                   = GmatTimeConstants::JD_OF_J2000;
   static const Real T_TT_COEFF1                   = GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;
   static const Real L_B                           = 1.550505e-8;
   static const Real NUM_SECS                      = GmatTimeConstants::SECS_PER_DAY;

   enum TimeSystemTypes
   {
      A1MJD = 0,
      TAIMJD,
      UTCMJD,
      UT1MJD,
      TDBMJD,
      TTMJD,
      A1,
      TAI,
      UTC,
      UT1,
      TDB,
      TT,
      TimeSystemCount
   };

   static const std::string TIME_SYSTEM_TEXT[TimeSystemCount] =
   {
      "A1Mjd",
      "TaiMjd",
      "UtcMjd",
      "Ut1Mjd",
      "TdbMjd",
      "TtMjd",
      // New entries added by DJC
      "A1",
      "TAI",
      "UTC",
      "UT1",
      "TDB",
      "TT",
   };

/*   Real Convert(const Real origValue,
                      const std::string &fromType,
                      const std::string &toType,
                      Real refJd = GmatTimeConstants::JD_NOV_17_1858);

   Real ConvertToTaiMjd(std::string fromType, Real origValue,
      Real refJd= GmatTimeConstants::JD_NOV_17_1858);
   Real ConvertFromTaiMjd(std::string toType, Real origValue,
      Real refJd= GmatTimeConstants::JD_NOV_17_1858);
 */
   
   Integer GMATUTIL_API GetTimeTypeID(std::string &str);
   
   Real GMATUTIL_API Convert(const Real origValue,
                         const Integer fromType,
                         const Integer toType,
                         Real refJd = GmatTimeConstants::JD_JAN_5_1941);
   Real GMATUTIL_API ConvertToTaiMjd(Integer fromType, Real origValue,
                         Real refJd = GmatTimeConstants::JD_NOV_17_1858);
   Real GMATUTIL_API ConvertFromTaiMjd(Integer toType, Real origValue,
                         Real refJd = GmatTimeConstants::JD_NOV_17_1858);


   GmatTime GMATUTIL_API Convert(const GmatTime origValue, 
                         const Integer fromType, 
                         const Integer toType, 
                         Real refJd = GmatTimeConstants::JD_JAN_5_1941);
   GmatTime GMATUTIL_API ConvertToTaiMjd(Integer fromType, GmatTime origValue, 
                         Real refJd = GmatTimeConstants::JD_NOV_17_1858);
   GmatTime GMATUTIL_API ConvertFromTaiMjd(Integer toType, GmatTime origValue, 
                         Real refJd = GmatTimeConstants::JD_NOV_17_1858);

   Real GMATUTIL_API NumberOfLeapSecondsFrom(Real utcMjd,
                         Real jdOfMjdRef = GmatTimeConstants::JD_JAN_5_1941);
   Real GMATUTIL_API GetFirstLeapSecondMJD(Real fromUtcMjd, Real toUtcMjd,
                         Real jdOfMjdRef = GmatTimeConstants::JD_JAN_5_1941);
   
   void GMATUTIL_API SetEopFile(EopFile *eopFile);
   void GMATUTIL_API SetLeapSecsFileReader(LeapSecsFileReader *leapSecsFileReader);
   
   void GMATUTIL_API GetTimeSystemAndFormat(const std::string &type, std::string &system,
                         std::string &format);
   
   std::string GMATUTIL_API ConvertMjdToGregorian(const Real mjd, bool handleLeapSecond = false,
                                              Integer format = 1);
   Real GMATUTIL_API ConvertGregorianToMjd(const std::string &greg);
   GmatTime GMATUTIL_API ConvertGregorianToMjdGT(const std::string &greg);
   
   void GMATUTIL_API Convert(const char *fromType, Real fromMjd,
                         const char *fromStr, const char *toType,
                         Real &toMjd, std::string &toStr, Integer format = 1);
   void GMATUTIL_API Convert(const char *fromType, Real fromMjd,
                         const std::string &fromStr, const std::string &toType,
                         Real &toMjd, std::string &toStr, Integer format = 1);
   void GMATUTIL_API Convert(const std::string &fromType, Real fromMjd,
                         const std::string &fromStr, const std::string &toType,
                         Real &toMjd, std::string &toStr, Integer format = 1);
   
   void GMATUTIL_API Convert(const char *fromType, GmatTime fromMjd, 
                         const char *fromStr, const char *toType, 
                         GmatTime &toMjd, std::string &toStr, Integer format = 1);
   void GMATUTIL_API Convert(const char *fromType, GmatTime fromMjd, 
                         const std::string &fromStr, const std::string &toType, 
                         GmatTime &toMjd, std::string &toStr, Integer format = 1);
   void GMATUTIL_API Convert(const std::string &fromType, GmatTime fromMjd, 
                         const std::string &fromStr, const std::string &toType, 
                         GmatTime &toMjd, std::string &toStr, Integer format = 1);

   bool GMATUTIL_API ValidateTimeSystem(std::string sys);   
   bool GMATUTIL_API ValidateTimeFormat(const std::string &format, const std::string &value,
                                    bool checkValue = true);
   StringArray GMATUTIL_API GetValidTimeRepresentations();
   
   bool GMATUTIL_API IsValidTimeSystem(const std::string& system);
   
   bool GMATUTIL_API IsInLeapSecond(Real theTaiMjd);
   bool GMATUTIL_API IsInLeapSecond(GmatTime theTaiMjd);
   
   bool GMATUTIL_API HandleLeapSecond();
   
   static bool isInLeapSecond;
   
}

#endif // TimeSystemConverter_hpp
