//$Header$
//------------------------------------------------------------------------------
//                            ParameterDatabaseException
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
 * Defines parameter database exception.
 */
//------------------------------------------------------------------------------
#ifndef ParameterDatabaseException_hpp
#define ParameterDatabaseException_hpp

#include "BaseException.hpp"

class ParameterDatabaseException : public BaseException
{
   public:
      ParameterDatabaseException(const std::string& message = 
                                 "Error occurred in ParameterDabase class") 
         : BaseException(message) {};
};
#endif
