//$Header$
//------------------------------------------------------------------------------
//                            ParameterException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/18
//
/**
 * Defines parameter exception.
 */
//------------------------------------------------------------------------------
#ifndef ParameterException_hpp
#define ParameterException_hpp

#include "BaseException.hpp"

class ParameterException : public BaseException
{
   public:
      ParameterException(const std::string& message = 
                         "Error occurred in Parameter class") 
         : BaseException(message) {};
};
#endif
