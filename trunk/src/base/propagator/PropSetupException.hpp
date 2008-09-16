//$Header$
//------------------------------------------------------------------------------
//                            PropSetupException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/10/10
//
/**
 * Defines stopping condition exception.
 */
//------------------------------------------------------------------------------
#ifndef PropSetupException_hpp
#define PropSetupException_hpp

#include "BaseException.hpp"

class PropSetupException : public BaseException
{
   public:
      PropSetupException(const std::string& message = 
                         "Error occurred in PropSetup class") 
         : BaseException(message) {};
};
#endif
