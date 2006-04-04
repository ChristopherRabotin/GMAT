//$Header$
//------------------------------------------------------------------------------
//                               MathException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Author: Allison Greene
// Created: 2006/03/27
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
/**
 * Exception class used by the Math hierarchy.
 */
//------------------------------------------------------------------------------


#include "MathException.hpp"


//------------------------------------------------------------------------------
//  MathException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs MathException instance (default constructor).
 * 
 * @param details A message providing the details of the exception. 
 */
//------------------------------------------------------------------------------
MathException::MathException(std::string details) :
    BaseException           ("Math Exception Thrown: ", details)
{
}


//------------------------------------------------------------------------------
//  ~MathException()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
MathException::~MathException()
{
}


//------------------------------------------------------------------------------
//  MathException(const MathException &ce)
//------------------------------------------------------------------------------
/**
 * Constructs MathException instance (copy constructor).
 */
//------------------------------------------------------------------------------
MathException::MathException(const MathException &be) :
    BaseException       (be)
{
}
