//$Header$
//------------------------------------------------------------------------------
//                                 TimeSystemConverter
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
 * Definition of the TimeSystemConverter class
 */
//------------------------------------------------------------------------------

#ifndef TimeSystemConverter_hpp
#define TimeSystemConverter_hpp

#include <iostream>
#include <iomanip>
#include <sstream>
#include "BaseException.hpp"
#include "EopFile.hpp"
#include "LeapSecsFileReader.hpp"

struct TimeSystemConverterExceptions
{
   class ImplementationException : public BaseException
   {
      public:
         ImplementationException(const std::string &message =
         "TimeSystemConverter: Converstion not implemented: ")
         : BaseException(message) {};
   };
   class FileException : public BaseException
   {
      public:
         FileException(const std::string &message =
         "TimeSystemConverter: File is unknown: ")
         : BaseException(message) {};
   };

};

namespace TimeConverterUtil
{
   // Specified in Math Spec section 2.3
   static const Real TDB_COEFF1                    = 0.001658;
   static const Real TDB_COEFF2                    = 0.00001385;
   static const Real M_E_OFFSET                    = 357.5277233;
   static const Real M_E_COEFF1                    = 35999.05034;
   static const Real T_TT_OFFSET                   = 2451545.0;
   static const Real T_TT_COEFF1                   = 36525;
   static const Real L_B                           = 1.550505e-8;
   static const Real TCB_JD_MJD_OFFSET             = 2443144.5;
   static const Real NUM_SECS                      = 86400;

   static const std::string TIME_SYSTEM_TEXT[6] =
         {
            "A1Mjd",
            "UtcMjd",
            "Ut1Mjd",
            "TdbMjd",
            "TcbMjd",
            "TtMjd",
         };

   Real Convert(const Real origValue,
                      const std::string &fromType,
                      const std::string &toType,
                      Real refJd = GmatTimeUtil::JD_NOV_17_1858);

   Real ConvertToTaiMjd(std::string fromType, Real origValue,
      Real refJd= GmatTimeUtil::JD_NOV_17_1858);
   Real ConvertFromTaiMjd(std::string toType, Real origValue,
      Real refJd= GmatTimeUtil::JD_NOV_17_1858);

   void SetEopFile(EopFile *eopFile);
   void SetLeapSecsFileReader(LeapSecsFileReader *leapSecsFileReader);
}

#endif // TimeSystemConverter_hpp
