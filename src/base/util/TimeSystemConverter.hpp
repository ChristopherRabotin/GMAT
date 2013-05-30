//$Id$
//------------------------------------------------------------------------------
//                                 TimeSystemConverter
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

// struct TimeSystemConverterExceptions
// {
   class GMAT_API UnimplementedException : public BaseException
   {
      public:
         UnimplementedException(const std::string &message =
         "TimeSystemConverter: Conversion not implemented: ")
         : BaseException(message) {};
   };
   class GMAT_API TimeFileException : public BaseException
   {
      public:
         TimeFileException(const std::string &message =
         "TimeSystemConverter: File is unknown: ")
         : BaseException(message) {};
   };
   class GMAT_API TimeFormatException : public BaseException
   {
      public:
         TimeFormatException(const std::string &message =
         "TimeSystemConverter: Requested format not implemented: ")
         : BaseException(message) {};
   };
   class GMAT_API InvalidTimeException : public BaseException
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
   
   Integer GMAT_API GetTimeTypeID(std::string &str);
   
   Real GMAT_API Convert(const Real origValue,
                         const Integer fromType,
                         const Integer toType,
                         Real refJd = GmatTimeConstants::JD_JAN_5_1941);
   
   Real GMAT_API ConvertToTaiMjd(Integer fromType, Real origValue,
                         Real refJd = GmatTimeConstants::JD_NOV_17_1858);
   Real GMAT_API ConvertFromTaiMjd(Integer toType, Real origValue,
                         Real refJd = GmatTimeConstants::JD_NOV_17_1858);
   Real GMAT_API NumberOfLeapSecondsFrom(Real utcMjd,
                         Real jdOfMjdRef = GmatTimeConstants::JD_JAN_5_1941);
   
   void GMAT_API SetEopFile(EopFile *eopFile);
   void GMAT_API SetLeapSecsFileReader(LeapSecsFileReader *leapSecsFileReader);
   
   void GMAT_API GetTimeSystemAndFormat(const std::string &type, std::string &system,
                         std::string &format);
   
   std::string GMAT_API ConvertMjdToGregorian(const Real mjd, Integer format = 1);   
   Real GMAT_API ConvertGregorianToMjd(const std::string &greg);
   void GMAT_API Convert(const std::string &fromType, Real fromMjd,
                         const std::string &fromStr, const std::string &toType,
                         Real &toMjd, std::string &toStr, Integer format = 1);
   
   bool GMAT_API ValidateTimeSystem(std::string sys);   
   bool GMAT_API ValidateTimeFormat(const std::string &format, const std::string &value,
                                    bool checkValue = true);
   StringArray GMAT_API GetValidTimeRepresentations();
   
   bool GMAT_API IsValidTimeSystem(const std::string& system);
}

#endif // TimeSystemConverter_hpp
