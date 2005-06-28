//$Header$
//------------------------------------------------------------------------------
//                           StateVectorException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2005/06/07
//
/**
 * Declares exception for the StateVector class.
 */
//------------------------------------------------------------------------------
#ifndef StateVectorException_hpp
#define StateVectorException_hpp

#include "BaseException.hpp"

class StateVectorException : public BaseException
{
public:
   StateVectorException(const std::string& message = 
                    "Error occurred in StateVector class") 
      : BaseException(message) {};
};
#endif
