//$Header$
//------------------------------------------------------------------------------
//                            UtilityException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/04/19
//
/**
 * Declares utility exception.
 */
//------------------------------------------------------------------------------
#ifndef UtilityException_hpp
#define UtilityException_hpp

#include "BaseException.hpp"

class UtilityException : public BaseException
{
public:
   UtilityException(const std::string& message = 
                    "Error occurred in utility class") 
      : BaseException(message) {};
};
#endif
