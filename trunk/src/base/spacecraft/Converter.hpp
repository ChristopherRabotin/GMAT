//$Header$ 
//------------------------------------------------------------------------------
//                                 Converter
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
 * Definition of the Converter base class
 */
//------------------------------------------------------------------------------

#ifndef Converter_hpp
#define Converter_hpp

#include "GmatBase.hpp"

class GMAT_API Converter
{
public:
    // Default constructor
    Converter();
    Converter(const std::string &type);
    // Copy constructor
    Converter(const Converter &converter);
    // Assignment operator
    Converter& operator=(const Converter &converter);
    
    virtual std::string GetType();
    virtual void SetType(const std::string &type);

    // Destructor
    virtual ~Converter();

protected:
    // Declare protetced method data 
    std::string type;

private:

};

#endif // Converter_hpp
