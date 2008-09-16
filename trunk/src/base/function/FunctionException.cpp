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


#include "FunctionException.hpp"


//------------------------------------------------------------------------------
//  FunctionException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs FunctionException instance (default constructor).
 * 
 * @param details A message providing the details of the exception. 
 */
//------------------------------------------------------------------------------
FunctionException::FunctionException(std::string details) :
    BaseException           ("Function Exception Thrown: ", details)
{
}


//------------------------------------------------------------------------------
//  ~FunctionException()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
FunctionException::~FunctionException()
{
}


//------------------------------------------------------------------------------
//  FunctionException(const FunctionException &ce)
//------------------------------------------------------------------------------
/**
 * Constructs FunctionException instance (copy constructor).
 */
//------------------------------------------------------------------------------
FunctionException::FunctionException(const FunctionException &be) :
    BaseException       (be)
{
}
