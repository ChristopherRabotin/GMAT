//$Header$
//------------------------------------------------------------------------------
//                               FunctionException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Allison Greene
// Created: 2004/9/22
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
/**
 * Exception class used by the Function hierarchy.
 */
//------------------------------------------------------------------------------
#ifndef FUNCTIONEXCEPTION_H
#define FUNCTIONEXCEPTION_H

#include "BaseException.hpp"

class FunctionException : public BaseException
{
public:
   FunctionException(std::string details = "");
   virtual ~FunctionException();
   FunctionException(const FunctionException &be);
};

#endif // FUNCTIONEXCEPTION_H
