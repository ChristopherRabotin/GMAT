//$Header$
//------------------------------------------------------------------------------
//                                ForCommand
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus
// Created: 2004/01/29
//
/**
 * Definition for the ForCommand command class
 */
//------------------------------------------------------------------------------

#include "ForCommand.hpp"

//------------------------------------------------------------------------------
//  ForCommand(void)
//------------------------------------------------------------------------------
/**
 * Creates a ForCommand command.  (default constructor)
 */
//------------------------------------------------------------------------------
ForCommand::ForCommand(void) :
    BranchCommand   ("For"),
    forName         ("")
{
}


//------------------------------------------------------------------------------
//  ~ForCommand(void)
//------------------------------------------------------------------------------
/**
 * Destroys the for command.  (destructor)
 */
//------------------------------------------------------------------------------
ForCommand::~ForCommand(void)
{}

    
//------------------------------------------------------------------------------
//  ForCommand(const ForCommand& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a for command.  (Copy constructor)
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ForCommand::ForCommand(const ForCommand& f) :
    BranchCommand   (f),
    forName         (f.forName)
{
}


//------------------------------------------------------------------------------
//  ForCommand& operator=(const ForCommand& f)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the for command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ForCommand& ForCommand::operator=(const ForCommand& f)
{
    if (this == &f)
        return *this;
    
    return *this;
}


//------------------------------------------------------------------------------
//  bool Append(Command *cmd)
//------------------------------------------------------------------------------
/**
 * Adds a command to the for loop.
 *
 * This method calls the BranchCommand base class method that adds a command
 * to the command sequence that branches off of the main mission sequence.  This
 * extension was needed so that the EndFor command can be set to point back 
 * to the head of the targeter loop.
 *
 * @return true if the Command is appended, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool ForCommand::Append(Command *cmd)
{
    if (!BranchCommand::Append(cmd))
        return false;
    
    // If at the end of a for branch, point that end back to this comand.
    if (cmd->GetTypeName() == "EndFor") {
        cmd->Append(this);
        // for loop is complete; -1 points us back to the main sequence.
        branchToFill = -1;
    }
    return true;
}


//------------------------------------------------------------------------------
//  bool Initialize(void)
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the targeter.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool ForCommand::Initialize(void)
{
    bool retval = BranchCommand::Initialize();

    return retval;
}


//------------------------------------------------------------------------------
//  bool Execute(void)
//------------------------------------------------------------------------------
/**
 * Target the variables defined for this for loop.
 *
 * This method (will eventually) runs the targeter state machine in order to
 * determine the variable values needed to achieve the user specified 
 * goals.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool ForCommand::Execute(void)
{
    bool retval = true;
    BranchCommand::Execute();
    
    if (!commandComplete) {
        // Target logic goes here -- basically the solver state machine calls
        
        // If the state machine says to run through the sub-commands, do this: 
        retval = ExecuteBranch();
        
        // For now, just execute the branched command and then continue.  That gets 
        // enough of the structure in place to validate that the command flow is 
        // working correctly.
        commandComplete = true;
    }
    
    return retval;
}
