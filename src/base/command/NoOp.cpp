//$Header$
//------------------------------------------------------------------------------
//                                  NoOp
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


#include "NoOp.hpp" // class's header file


//------------------------------------------------------------------------------
//  NoOp()
//------------------------------------------------------------------------------
/**
 * Constructs the NoOp command (default constructor).
 */
//------------------------------------------------------------------------------
NoOp::NoOp() :
    GmatCommand("NoOp")
{
}


//------------------------------------------------------------------------------
//  ~NoOp()
//------------------------------------------------------------------------------
/**
 * Destroys the NoOp command (default constructor).
 */
//------------------------------------------------------------------------------
NoOp::~NoOp()
{
}


//------------------------------------------------------------------------------
//  NoOp(const NoOp& noop)
//------------------------------------------------------------------------------
/**
 * Makes a copy of the NoOp command (copy constructor).
 */
//------------------------------------------------------------------------------
NoOp::NoOp(const NoOp& noop) :
    GmatCommand (noop)
{}


//------------------------------------------------------------------------------
//  NoOp& operator=(const NoOp&)
//------------------------------------------------------------------------------
/**
 * Sets this NoOp to match another one (assignment operator).
 * 
 * @param noop The original used to set parameters for this one.
 */
//------------------------------------------------------------------------------
NoOp& NoOp::operator=(const NoOp& noop)
{
    return *this;
}


//------------------------------------------------------------------------------
//  bool Execute(void)
//------------------------------------------------------------------------------
/**
 * Executes the NoOp command (copy constructor).
 * 
 * Like the name implies, NoOp is a null operation -- nothing is done in this 
 * command.  It functions as a place holder, and as the starting command in the 
 * command sequence managed by the Moderator.
 */
//------------------------------------------------------------------------------
bool NoOp::Execute(void)
{
    return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the NoOp.
 *
 * @return clone of the NoOp.
 *
 */
//------------------------------------------------------------------------------
GmatBase* NoOp::Clone(void) const
{
   return (new NoOp(*this));
}

