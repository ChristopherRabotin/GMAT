//$Id$
//------------------------------------------------------------------------------
//                              CommandException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2003/10/27
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Exception class used by the Command hierarchy.
 */
//------------------------------------------------------------------------------


#include "CommandException.hpp" // class's header file

//------------------------------------------------------------------------------
//  CommandException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs CommandException instance (default constructor).
 * 
 * @param details A message providing the details of the exception. 
 */
//------------------------------------------------------------------------------
CommandException::CommandException(const std::string &details,
      Gmat::MessageType mt) :
    BaseException           ("Command Exception: ", details, mt)
{
}


//------------------------------------------------------------------------------
//  ~CommandException()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
CommandException::~CommandException()
{
}


//------------------------------------------------------------------------------
//  CommandException(const CommandException &ce)
//------------------------------------------------------------------------------
/**
 * Constructs CommandException instance (copy constructor). 
 *
 * @param ce Instance that gets copied.
 */
//------------------------------------------------------------------------------
CommandException::CommandException(const CommandException &ce) :
    BaseException       (ce)
{}

