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

class GMAT_API TimeSystemConverter 
{
public:
    class TimeSystemConverterException : public BaseException
    {
       public: 
          TimeSystemConverterException(const std::string &message =
           "TimeSystemConverterException: Can't convert due to invalid date")
           : BaseException(message) {};
    };
    class ImplementationException : public BaseException
    {
       public:
          ImplementationException(const std::string &message =
           "TimeSystemConverter: Converstion not implemented: ")
           : BaseException(message) {};
    };

    // Default constructor
    TimeSystemConverter();
    TimeSystemConverter(const std::string &name);
    TimeSystemConverter(const std::string &typeStr, const std::string &name);
    // Copy constructor
    TimeSystemConverter(const TimeSystemConverter &TimeSystemConverter);
    // Assignment operator
    TimeSystemConverter& operator=(const TimeSystemConverter &TimeSystemConverter);

    // Destructor
    virtual ~TimeSystemConverter();

    // public method 
    Real Convert(const Real time,
                        const std::string &fromTimeFormat,
                        const std::string &toTimeFormat);

    Real ConvertToTaiMjd(std::string fromTimeFormat, Real time);
    Real ConvertFromTaiMjd(std::string toTimeFormat, Real time);
    bool SetEopFile(EopFile *eopFile);
    bool SetLeapSecsFileReader(LeapSecsFileReader *leapSecsFileReader);

protected:
   // Declare protetced method data

private:
   static const std::string TIME_SYSTEM_TEXT[];
   Real jdOffset;
   EopFile *theEopFile;
   LeapSecsFileReader *theLeapSecsFileReader;

   static const Real TDB_COEFF1;
   static const Real TDB_COEFF2;
   static const Real M_E_OFFSET;
   static const Real M_E_COEFF1;
   static const Real T_TT_OFFSET;
   static const Real T_TT_COEFF1;
   static const Real L_B; 
   static const Real TCB_JD_MJD_OFFSET;
   static const Real NUM_SECS;


};

#endif // TimeSystemConverter_hpp
