//$Header$
//------------------------------------------------------------------------------
//                             ForceModelException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/29
//
/**
 * Exceptions thrown in the force models
 */
//------------------------------------------------------------------------------


#include "ForceModelException.hpp"


//------------------------------------------------------------------------------
//  ForceModelException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs CommandException instance (default constructor).
 * 
 * @param details A message providing the details of the exception. 
 */
//------------------------------------------------------------------------------
ForceModelException::ForceModelException(const std::string &details) :
    BaseException           ("ForceModel Exception Thrown: ", details)
{
}


//------------------------------------------------------------------------------
//  ~CommandException()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
ForceModelException::~ForceModelException()
{
}


//------------------------------------------------------------------------------
//  CommandException(const CommandException &ce)
//------------------------------------------------------------------------------
/**
 * Constructs CommandException instance (copy constructor). 
 */
//------------------------------------------------------------------------------
ForceModelException::ForceModelException(const ForceModelException &fme) :
    BaseException       (fme)
{
}
