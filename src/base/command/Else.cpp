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
 * Definition for the Else command class
 */
//------------------------------------------------------------------------------

#include "Else.hpp"


//------------------------------------------------------------------------------
//  Else(void)
//------------------------------------------------------------------------------
/**
 * Creates a Else command.  (default constructor)
 */
//------------------------------------------------------------------------------
Else::Else(void) :
    GmatCommand      ("Else")
{
}


//------------------------------------------------------------------------------
//  ~Else(void)
//------------------------------------------------------------------------------
/**
 * Destroys the for command.  (destructor)
 */
//------------------------------------------------------------------------------
Else::~Else(void)
{}

    
//------------------------------------------------------------------------------
//  Else(const Else& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a for command.  (Copy constructor)
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Else::Else(const Else& ic) :
    GmatCommand   (ic)
{
}


//------------------------------------------------------------------------------
//  Else& operator=(const Else& ic)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the for command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Else& Else::operator=(const Else& ic)
{
    if (this == &ic)
        return *this;

    GmatCommand::operator=(ic);
    return *this;
}


//------------------------------------------------------------------------------
//  bool Append(Command *cmd)
//------------------------------------------------------------------------------
/**
 * Adds a command to the ELSE statement.
 *
 * This method calls the BranchCommand base class method that adds a command
 * to the command sequence that branches off of the main mission sequence.  This
 * extension was needed so that the EndIf command can be set to point back 
 * to the head of the ELSE statement.
 *
 * @return true if the Command is appended, false if an error occurs.
 */
//------------------------------------------------------------------------------
//bool Else::Append(GmatCommand *cmd)
//{
    //if (!BranchCommand::Append(cmd))
    //    return false;

    // Check for the end of "Else" branch, point that end back to this command
    //if (cmd->GetTypeName() == "EndIf")
    //{
    //   cmd->Append(this);
       // Targeter loop is complete; -1 points us back to the main sequence. 
    //   branchToFill = -1;
    //}

    //return true;
//}

bool Else::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if we've gotten to this point, we should have inserted it into the front
   // of the next branch

   return false;
}
    
//------------------------------------------------------------------------------
//  bool Initialize(void)
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the ELSE statement.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Else::Initialize(void)
{
    bool retval = GmatCommand::Initialize();

    // if specific initialization goes here:
    
    return retval;
}


//------------------------------------------------------------------------------
//  bool Execute(void)
//------------------------------------------------------------------------------
/**
 * Target the variables defined for this for ELSE statement.
 *
 * This method (will eventually) runs the state machine in order to
 * determine the variable values needed to achieve the user specified 
 * goals.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Else::Execute(void)
{
    bool retval = true;

    //commandComplete = true;
    //GmatCommand::Execute();
   
    return retval;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Else.
 *
 * @return clone of the Else.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Else::Clone(void) const
{
   return (new Else(*this));
}
