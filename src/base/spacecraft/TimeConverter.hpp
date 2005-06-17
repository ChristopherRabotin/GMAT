//$Header$ 
//------------------------------------------------------------------------------
//                                 TimeConverter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2004/03/22
//
/**
 * Definition of the TimeConverter class
 */
//------------------------------------------------------------------------------

#ifndef TimeConverter_hpp
#define TimeConverter_hpp

#include <iostream>
#include <iomanip>
#include <sstream>
#include "BaseException.hpp"
#include "Converter.hpp"
#include "A1Date.hpp"
#include "A1Mjd.hpp"
#include "UtcDate.hpp"
#include "DateUtil.hpp"
#include "GregorianDate.hpp"

#include "TimeSystemConverter.hpp"

class GMAT_API TimeConverter : public Converter
{
public:
    // Implements exception
    class TimeConverterException : public BaseException
    {
       public: 
          TimeConverterException(const std::string &message = 
           "TimeConverterException: Can't convert due to invalid date")
           : BaseException(message) {};
    };

    // Default constructor
    TimeConverter();
    TimeConverter(const std::string &name);
    TimeConverter(const std::string &typeStr, const std::string &name);
    // Copy constructor
    TimeConverter(const TimeConverter &timeConverter);
    // Assignment operator
    TimeConverter& operator=(const TimeConverter &timeConverter);

    // Destructor
    virtual ~TimeConverter();

    // public method 
    std::string Convert(const std::string &time, 
                        const std::string &fromDateFormat,
                        const std::string &toDateFormat);

protected:
   std::string          ModJulianToGregorian(const Real mjTime);
   Real                 GregorianToModJulian(const std::string greg);

private:

};

#endif // TimeConverter_hpp
