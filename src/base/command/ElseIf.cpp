//$Header$
//------------------------------------------------------------------------------
//                                ElseIf
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Wendy Shoan
// Created: 2004/10/21
//
/**
 * Definition for the ElseIf command class
 */
//------------------------------------------------------------------------------

#include "ElseIf.hpp"

//------------------------------------------------------------------------------
//  ElseIf(void)
//------------------------------------------------------------------------------
/**
 * Creates a ElseIf command.  (default constructor)
 */
//------------------------------------------------------------------------------
ElseIf::ElseIf(void) :
    GmatCommand      ("ElseIf")
{
}


//------------------------------------------------------------------------------
//  ~ElseIf(void)
//------------------------------------------------------------------------------
/**
 * Destroys the for command.  (destructor)
 */
//------------------------------------------------------------------------------
ElseIf::~ElseIf(void)
{}

    
//------------------------------------------------------------------------------
//  ElseIf(const ElseIf& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a for command.  (Copy constructor)
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ElseIf::ElseIf(const ElseIf& ic) :
    GmatCommand   (ic)
{
}


//------------------------------------------------------------------------------
//  ElseIf& operator=(const ElseIf& ic)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the for command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ElseIf& ElseIf::operator=(const ElseIf& ic)
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
 * Adds a command to the ElseIf statement.
 *
 * This method calls the BranchCommand base class method that adds a command
 * to the command sequence that branches off of the main mission sequence.  This
 * extension was needed so that the EndIf command can be set to point back 
 * to the head of the ElseIf statement.
 *
 * @return true if the Command is appended, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool ElseIf::Append(GmatCommand *cmd)
{
    //if (!BranchCommand::Append(cmd))
    //    return false;

    // Check for the end of "ElseIf" branch, point that end back to this command
    if (cmd->GetTypeName() == "EndIf")
    {
       cmd->Append(this);
       // Targeter loop is complete; -1 points us back to the main sequence. 
    //   branchToFill = -1;
    }

    return true;
}

//------------------------------------------------------------------------------
//  bool Initialize(void)
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the ElseIf statement.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool ElseIf::Initialize(void)
{
    bool retval = GmatCommand::Initialize();

    // if specific initialization goes here:
    
    return retval;
}


//------------------------------------------------------------------------------
//  bool Execute(void)
//------------------------------------------------------------------------------
/**
 * Target the variables defined for this for ElseIf statement.
 *
 * This method (will eventually) runs the state machine in order to
 * determine the variable values needed to achieve the user specified 
 * goals.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool ElseIf::Execute(void)
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
 * This method returns a clone of the ElseIf.
 *
 * @return clone of the ElseIf.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ElseIf::Clone(void) const
{
   return (new ElseIf(*this));
}
