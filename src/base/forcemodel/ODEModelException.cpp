//$Id$
//------------------------------------------------------------------------------
//                             ODEModelException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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


#include "ODEModelException.hpp"


//------------------------------------------------------------------------------
//  ODEModelException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs CommandException instance (default constructor).
 * 
 * @param details A message providing the details of the exception. 
 */
//------------------------------------------------------------------------------
ODEModelException::ODEModelException(const std::string &details) :
    BaseException           ("ODEModel Exception Thrown: ", details)
{
}


//------------------------------------------------------------------------------
//  ~CommandException()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
ODEModelException::~ODEModelException()
{
}


//------------------------------------------------------------------------------
//  CommandException(const CommandException &ce)
//------------------------------------------------------------------------------
/**
 * Constructs CommandException instance (copy constructor). 
 */
//------------------------------------------------------------------------------
ODEModelException::ODEModelException(const ODEModelException &fme) :
    BaseException       (fme)
{
}
