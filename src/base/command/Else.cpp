//$Header$
//------------------------------------------------------------------------------
//                                Else
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus
// Created: 2004/01/30
// Modified: 2004/08/10 W. Shoan - made a subclass of GmatCommand; modified
//           code accordingly
//
/**
 * Implementation for the Else command class
 */
//------------------------------------------------------------------------------

#include "Else.hpp"


//------------------------------------------------------------------------------
//  Else()
//------------------------------------------------------------------------------
/**
 * Creates a Else command.  (default constructor)
 */
//------------------------------------------------------------------------------
Else::Else() :
    GmatCommand      ("Else")
{
}


//------------------------------------------------------------------------------
//  ~Else()
//------------------------------------------------------------------------------
/**
 * Destroys the for command.  (destructor)
 */
//------------------------------------------------------------------------------
Else::~Else()
{}

    
//------------------------------------------------------------------------------
//  Else(const Else& ec)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a for command.  (Copy constructor)
 *
 * @param ec Else command to copy to create 'this' one
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Else::Else(const Else& ec) :
    GmatCommand   (ec)
{
}


//------------------------------------------------------------------------------
//  Else& operator=(const Else& ec)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the for command.
 *
 * @param ec Else command whose values to use to assign those of 'this' Else
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Else& Else::operator=(const Else& ec)
{
    if (this == &ec)
        return *this;

    GmatCommand::operator=(ec);
    return *this;
}

//------------------------------------------------------------------------------
//  bool Insert(GmatCommand *cmd, GmatCommand *prev)
//------------------------------------------------------------------------------
/**
 * Inserts the command cmd after the command prev.
 *
 * @param cmd Command to insert.
 *
 * @param prev Command after which to insert the command cmd.
 *
 * @return true if the cmd is inserted, false otherwise.
 */
//------------------------------------------------------------------------------
bool Else::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if we've gotten to this point, we should have inserted it into the front
   // of the next branch

   return false;
}
    
//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the ELSE statement.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Else::Initialize()
{
    bool retval = GmatCommand::Initialize();

    // if specific initialization goes here:
    
    return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Execute the Else statement.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Else::Execute()
{
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Else.
 *
 * @return clone of the Else.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Else::Clone() const
{
   return (new Else(*this));
}
