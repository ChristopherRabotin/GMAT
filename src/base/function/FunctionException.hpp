//$Id$
//------------------------------------------------------------------------------
//                               FunctionException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

class GMAT_API FunctionException : public BaseException
{
public:
   FunctionException(std::string details = "");
   virtual ~FunctionException();
   FunctionException(const FunctionException &be);
};

#endif // FUNCTIONEXCEPTION_H
