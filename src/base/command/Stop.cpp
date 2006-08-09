//$Header$
//------------------------------------------------------------------------------
//                                  Stop
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/10/24
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Null operator for the command sequence -- typically used as a place holder
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
//  Stop(const Stop& noop)
//------------------------------------------------------------------------------
/**
 * Makes a copy of the Stop command (copy constructor).
 *
 * @param noop The original used to set parameters for this one.
 */
//------------------------------------------------------------------------------
Stop::Stop(const Stop& noop) :
    GmatCommand (noop)
{
}


//------------------------------------------------------------------------------
//  Stop& operator=(const Stop&)
//------------------------------------------------------------------------------
/**
 * Sets this Stop to match another one (assignment operator).
 * 
 * @param noop The original used to set parameters for this one.
 *
 * @return this instance.
 */
//------------------------------------------------------------------------------
Stop& Stop::operator=(const Stop& noop)
{
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
      "Command Sequence intentionally stopped by Stop command.\n");
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
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
