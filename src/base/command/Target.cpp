//$Header$
//------------------------------------------------------------------------------
//                                  Target
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/22
//
/**
 * Definition for the Target command class
 */
//------------------------------------------------------------------------------


#include "Target.hpp"


//------------------------------------------------------------------------------
//  Target(void)
//------------------------------------------------------------------------------
/**
 * Creates a Target command.  (default constructor)
 */
//------------------------------------------------------------------------------
Target::Target(void) :
    BranchCommand   ("Target"),
    targeterName    (""),
    targeter        (NULL),
    targeterNameID  (parameterCount)
{
    parameterCount += 1;
}


//------------------------------------------------------------------------------
//  ~Target(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Target command.  (destructor)
 */
//------------------------------------------------------------------------------
Target::~Target(void)
{}

    
//------------------------------------------------------------------------------
//  Target(const Target& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Target command.  (Copy constructor)
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Target::Target(const Target& t) :
    BranchCommand   (t),
    targeterName    (t.targeterName),
    targeter        (NULL),
    targeterNameID  (t.parameterCount)
{
    parameterCount = t.parameterCount;
}


//------------------------------------------------------------------------------
//  Target& operator=(const Target& t)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the target command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Target& Target::operator=(const Target& t)
{
    if (this == &t)
        return *this;
    
    return *this;
}


//------------------------------------------------------------------------------
//  bool Append(Command *cmd)
//------------------------------------------------------------------------------
/**
 * Adds a command to the targeter loop.
 *
 * This method calls the BranchCommand base class method that adds a command
 * to the command sequence that branches off of the main mission sequence.  This
 * extension was needed so that the EndTarget command can be set to point back 
 * to the head of the targeter loop.
 *
 * @return true if the Command is appended, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Target::Append(Command *cmd)
{
    if (!BranchCommand::Append(cmd))
        return false;
    
    // If at the end of a targeter branch, point that end back to this comand.
    if (cmd->GetTypeName() == "EndTarget") {
        cmd->Append(this);
        // Targeter loop is complete; -1 points us back to the main sequence.
        branchToFill = -1;
    }
    return true;
}


//------------------------------------------------------------------------------
//  void Target::InterpretAction(void)
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Target command has the following syntax:
 *
 *     Target myDC;
 *
 * where myDC is a Solver used to target a set of variables to achieve the
 * corresponding goals.  This method breaks the script line into the 
 * corresponding pieces, and stores the name of the Solver so it can be set to
 * point to the correct object during initialization.
 */
//------------------------------------------------------------------------------
void Target::InterpretAction(void)
{
    /// @todo: Clean up this hack for the Target::InterpretAction method
    // Sample string:  "Target myDC;"
    
    // Set starting location to the space following the command string
    Integer loc = generatingString.find("Target", 0) + 6, end;
    const char *str = generatingString.c_str();
    
    // Skip white space
    while (str[loc] == ' ')
        ++loc;
    
    // Stop at the semicolon if it is there
    end = generatingString.find(";", loc);
    
    
    std::string component = generatingString.substr(loc, end-loc);
    if (component == "")
        throw CommandException("Target string does not the targeter");
    SetStringParameter(targeterNameID, component);
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
bool Target::Initialize(void)
{
    bool retval = BranchCommand::Initialize();

    // Targeter specific initialization goes here:
    
    return retval;
}


//------------------------------------------------------------------------------
//  bool Execute(void)
//------------------------------------------------------------------------------
/**
 * Target the variables defined for this targeting loop.
 *
 * This method (will eventually) runs the targeter state machine in order to
 * determine the variable values needed to achieve the user specified 
 * goals.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Target::Execute(void)
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
