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

#include <sstream>
#include "Converter.hpp"
#include "A1Date.hpp"
#include "A1Mjd.hpp"
#include "UtcDate.hpp"
#include "DateUtil.hpp"

class GMAT_API TimeConverter : public Converter
{
public:
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
    Real Convert(const Real time, const std::string &fromDateFormat,
                 const std::string &toDateFormat);

protected:
    // Declare protetced method data 

private:

};

#endif // TimeConverter_hpp
