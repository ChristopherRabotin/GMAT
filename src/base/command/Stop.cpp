//$Id$
//------------------------------------------------------------------------------
//                                  Stop
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2003/10/24
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Stop command used to halt execution.
 */
//------------------------------------------------------------------------------


#include "Stop.hpp" // class's header file


//------------------------------------------------------------------------------
//  Stop()
//------------------------------------------------------------------------------
/**
 * Constructs the Stop command (default constructor).
 */
//------------------------------------------------------------------------------
Stop::Stop() :
    GmatCommand("Stop")
{
   generatingString = "Stop";
}


//------------------------------------------------------------------------------
//  ~Stop()
//------------------------------------------------------------------------------
/**
 * Destroys the Stop command (default constructor).
 */
//------------------------------------------------------------------------------
Stop::~Stop()
{
}


//------------------------------------------------------------------------------
//  Stop(const Stop& stop)
//------------------------------------------------------------------------------
/**
 * Makes a copy of the Stop command (copy constructor).
 *
 * @param stop The original used to set parameters for this one.
 */
//------------------------------------------------------------------------------
Stop::Stop(const Stop& stop) :
    GmatCommand (stop)
{
   generatingString = stop.generatingString;
}


//------------------------------------------------------------------------------
//  Stop& operator=(const Stop &stop)
//------------------------------------------------------------------------------
/**
 * Sets this Stop to match another one (assignment operator).
 * 
 * @param stop The original used to set parameters for this one.
 *
 * @return this instance.
 */
//------------------------------------------------------------------------------
Stop& Stop::operator=(const Stop &stop)
{
   if (this != &stop)
   {
      GmatCommand::operator=(stop);
      generatingString = stop.generatingString;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Executes the Stop command (copy constructor).
 * 
 * Like the name implies, Stop is a null operation -- nothing is done in this 
 * command.  It functions as a place holder, and as the starting command in the 
 * command sequence managed by the Moderator.
 *
 * @return true always.
 */
//------------------------------------------------------------------------------
bool Stop::Execute()
{
   throw CommandException(
      "Command Sequence intentionally interrupted by Stop command.\n");
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Stop.
 *
 * @return clone of the Stop.
 */
//------------------------------------------------------------------------------
GmatBase* Stop::Clone() const
{
   return (new Stop(*this));
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName)
//------------------------------------------------------------------------------
const std::string& Stop::GetGeneratingString(Gmat::WriteMode mode,
                                             const std::string &prefix,
                                             const std::string &useName)
{
   generatingString = prefix + "Stop;";
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}
