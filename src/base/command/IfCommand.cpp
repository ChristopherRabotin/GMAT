//$Header$
//------------------------------------------------------------------------------
//                                IfCommand
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
 * Definition for the IfCommand command class
 */
//------------------------------------------------------------------------------

#include "IfCommand.hpp"

//------------------------------------------------------------------------------
//  IfCommand(void)
//------------------------------------------------------------------------------
/**
 * Creates a IfCommand command.  (default constructor)
 */
//------------------------------------------------------------------------------
IfCommand::IfCommand(void) :
    BranchCommand      ("If")
{
}


//------------------------------------------------------------------------------
//  ~IfCommand(void)
//------------------------------------------------------------------------------
/**
 * Destroys the for command.  (destructor)
 */
//------------------------------------------------------------------------------
IfCommand::~IfCommand(void)
{}

    
//------------------------------------------------------------------------------
//  IfCommand(const IfCommand& ic)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a for command.  (Copy constructor)
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
IfCommand::IfCommand(const IfCommand& ic) :
    BranchCommand   (ic)
{
}


//------------------------------------------------------------------------------
//  IfCommand& operator=(const IfCommand& ic)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the for command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
IfCommand& IfCommand::operator=(const IfCommand& ic)
{
    if (this == &ic)
        return *this;
    
    return *this;
}


//------------------------------------------------------------------------------
//  bool Append(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Adds a command to the IF statement.
 *
 * This method calls the BranchCommand base class method that adds a command
 * to the command sequence that branches off of the main mission sequence.  This
 * extension was needed so that the EndIf command can be set to point back 
 * to the head of the IF statement.
 *
 * @return true if the Command is appended, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool IfCommand::Append(GmatCommand *cmd)
{
    if (!BranchCommand::Append(cmd))
        return false;

    // Check for the end of "If" branch, point that end back to this command
    if (cmd->GetTypeName() == "EndIf" || cmd->GetTypeName() == "Else")
    {
       cmd->Append(this);

       // IF statement is complete; -1 points us back to the main sequence. 
       if (cmd->GetTypeName() == "EndIf")
          branchToFill = -1;
       else
          ++branchToFill;
    }

    return true;
}

//------------------------------------------------------------------------------
//  bool Initialize(void)
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the IF statement.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool IfCommand::Initialize(void)
{
    bool retval = BranchCommand::Initialize();

    // if specific initialization goes here:
    
    return retval;
}


//------------------------------------------------------------------------------
//  bool Execute(void)
//------------------------------------------------------------------------------
/**
 * Target the variables defined for this for IF statement.
 *
 * This method (will eventually) runs the state machine in order to
 * determine the variable values needed to achieve the user specified 
 * goals.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool IfCommand::Execute(void)
{
    bool retval = true;

    commandComplete = true;
    BranchCommand::Execute();
   
#if 0
    if (!commandComplete) 
    {
        retval = ExecuteBranch();
        commandComplete = true;
    }
#endif
    
    return retval;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the IfCommand.
 *
 * @return clone of the IfCommand.
 *
 */
//------------------------------------------------------------------------------
GmatBase* IfCommand::Clone(void) const
{
   return (new IfCommand(*this));
}

