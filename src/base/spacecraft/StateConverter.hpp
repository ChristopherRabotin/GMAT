//$Header$ 
//------------------------------------------------------------------------------
//                                 StateConverter
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
 * Definition of the StateConverter class
 */
//------------------------------------------------------------------------------

#ifndef StateConverter_hpp
#define StateConverter_hpp

#include "Converter.hpp"
#include "A1Date.hpp"
#include "A1Mjd.hpp"
#include "UtcDate.hpp"
#include "Cartesian.hpp"
#include "Keplerian.hpp"
#include "SphericalOne.hpp"
#include "SphericalTwo.hpp"
#include "Rvector6.hpp"
#include "Rvector3.hpp"

class GMAT_API StateConverter : public Converter
{
public:
    // Default constructor
    StateConverter();
    StateConverter(const std::string &name);
    StateConverter(const std::string &typeStr, const std::string &name);
    // Copy constructor
    StateConverter(const StateConverter &state);
    // Assignment operator
    StateConverter& operator=(const StateConverter &state);

    // Destructor
    virtual ~StateConverter();

    // public method 
    Rvector6 Convert(const Real *state, const std::string &fromElementType,
                     const std::string &toElementType);

protected:
    // Declare protetced method data 

private:

};

#endif // StateConverter_hpp
