//$Header$
//------------------------------------------------------------------------------
//                                ElseCommand
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
//
/**
 * Definition for the Else command class
 */
//------------------------------------------------------------------------------

#include "ElseCommand.hpp"

//------------------------------------------------------------------------------
//  ElseCommand(void)
//------------------------------------------------------------------------------
/**
 * Creates a ElseCommand command.  (default constructor)
 */
//------------------------------------------------------------------------------
ElseCommand::ElseCommand(void) :
    BranchCommand      ("Else")
{
}


//------------------------------------------------------------------------------
//  ~ElseCommand(void)
//------------------------------------------------------------------------------
/**
 * Destroys the for command.  (destructor)
 */
//------------------------------------------------------------------------------
ElseCommand::~ElseCommand(void)
{}

    
//------------------------------------------------------------------------------
//  ElseCommand(const ElseCommand& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a for command.  (Copy constructor)
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ElseCommand::ElseCommand(const ElseCommand& ic) :
    BranchCommand   (ic)
{
}


//------------------------------------------------------------------------------
//  ElseCommand& operator=(const ElseCommand& ic)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the for command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ElseCommand& ElseCommand::operator=(const ElseCommand& ic)
{
    if (this == &ic)
        return *this;
    
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
bool ElseCommand::Append(Command *cmd)
{
    if (!BranchCommand::Append(cmd))
        return false;

    // Check for the end of "Else" branch, point that end back to this command
    if (cmd->GetTypeName() == "EndIf")
    {
       cmd->Append(this);
       // Targeter loop is complete; -1 points us back to the main sequence. 
       branchToFill = -1;
    }

    return true;
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
bool ElseCommand::Initialize(void)
{
    bool retval = BranchCommand::Initialize();

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
bool ElseCommand::Execute(void)
{
    bool retval = true;

    commandComplete = true;
    BranchCommand::Execute();
   
    return retval;
}
