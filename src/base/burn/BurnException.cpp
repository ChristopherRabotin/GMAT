//$Header$
//------------------------------------------------------------------------------
//                               BurnException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2004/1/13
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
/**
 * Exception class used by the Burn hierarchy.
 */
//------------------------------------------------------------------------------


#include "BurnException.hpp"


//------------------------------------------------------------------------------
//  BurnException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs BurnException instance (default constructor).
 * 
 * @param details A message providing the details of the exception. 
 */
//------------------------------------------------------------------------------
BurnException::BurnException(std::string details) :
    BaseException           ("Burn Exception Thrown: ", details)
{
}


//------------------------------------------------------------------------------
//  ~BurnException()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
BurnException::~BurnException()
{
}


//------------------------------------------------------------------------------
//  BurnException(const BurnException &ce)
//------------------------------------------------------------------------------
/**
 * Constructs BurnException instance (copy constructor). 
 */
//------------------------------------------------------------------------------
BurnException::BurnException(const BurnException &be) :
    BaseException       (be)
{}

