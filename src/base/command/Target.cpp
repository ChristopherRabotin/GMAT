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
#include "Spacecraft.hpp"


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
        throw CommandException("Target string does not identify the targeter");
    SetStringParameter(targeterNameID, component);
}


// Parameter access methods

//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Read accessor for parameter names.
 *
 * @param <id> the ID of the parameter.
 *
 * @return the text string for the parameter.
 */
//------------------------------------------------------------------------------
std::string Target::GetParameterText(const Integer id) const
{
    if (id == targeterNameID)
        return "Targeter";
    
    return BranchCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Read accessor for parameter IDs.
 *
 * @param <str> the text description of the parameter.
 *
 * @return the integer ID for the parameter.
 */
//------------------------------------------------------------------------------
Integer Target::GetParameterID(const std::string &str) const
{
    if (str == "Targeter")
        return targeterNameID;
    
    return BranchCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Read accessor for parameter types.
 *
 * @param <id> the integer ID of the parameter.
 *
 * @return the type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Target::GetParameterType(const Integer id) const
{
    if (id == targeterNameID)
        return Gmat::STRING_TYPE;
    
    return BranchCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Read accessor for parameter type data description.
 *
 * @param <id> the integer ID of the parameter.
 *
 * @return a string describing the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string Target::GetParameterTypeString(const Integer id) const
{
    if (id == targeterNameID)
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
    
    return BranchCommand::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Read accessor for string parameters.
 *
 * @param <id> the integer ID of the parameter.
 *
 * @return the string contained in the parameter.
 */
//------------------------------------------------------------------------------
std::string Target::GetStringParameter(const Integer id) const
{
    if (id == targeterNameID)
        return targeterName;
    
    return BranchCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Write accessor for string parameters.
 *
 * @param <id> the integer ID of the parameter.
 * @param <value> the new string stored in the parameter.
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Target::SetStringParameter(const Integer id, const std::string &value)
{
    if (id == targeterNameID) {
        targeterName = value;
        return true;
    }    
    
    return BranchCommand::SetStringParameter(id, value);
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
    if (objectMap->find(targeterName) == objectMap->end()) {
        std::string errorString = "Target command cannot find targeter \"";
        errorString += targeterName;
        errorString += "\"";
        throw CommandException(errorString);
    }

    targeter = (Solver *)((*objectMap)[targeterName]);
    targeter->Initialize();
        
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
#include <iostream>
bool Target::Execute(void)
{
    bool retval = BranchCommand::Execute();
    
    // Drive through the state machine.
    Solver::SolverState state = targeter->GetState();
    Command *current;
    
    switch (state) {
        case Solver::INITIALIZING:
std::cout << "Target Command State:  Solver::INITIALIZING\n";
            // Finalize initialization of the targeter data
            current = branch[0];
            while (current != this)  {
                std::string type = current->GetTypeName();
                if ((type == "Target") || (type == "Vary") || (type == "Achieve"))
                    current->Execute();
                current = current->GetNext();
            }
            StoreLoopData();
            break;
            
        case Solver::NOMINAL:
std::cout << "Target Command State:  Solver::NOMINAL\n";
            // Execute the nominal sequence
            if (!commandComplete) {
                ResetLoopData();
                retval = ExecuteBranch();
            }
            break;
            
        case Solver::CHECKINGRUN:
std::cout << "Target Command State:  Solver::CHECKINGRUN\n";
            // Check for convergence; this is done in the targeter state 
            // machine, so this case is a NoOp for the Target command
            break;
            
        case Solver::PERTURBING:
std::cout << "Target Command State:  Solver::PERTURBING\n";
            ResetLoopData();
            retval = ExecuteBranch();
            break;
            
        case Solver::CALCULATING:
std::cout << "Target Command State:  Solver::CALCULATING\n";
            // Calculate the next set of variables to use; this is performed in
            // the targeter -- nothing to be done here
            break;
            
        case Solver::FINISHED:
std::cout << "Target Command State:  Solver::FINISHED\n";
            // Final clean-up
            commandComplete = true;
            break;
            
        case Solver::ITERATING:     // Intentional fall-through
        default:
            throw CommandException("Invalid state in the Targeter state machine");
    }

    targeter->AdvanceState();
    return retval;
}


#include <iostream>
void Target::StoreLoopData(void)
{
std::cout << "Storing loop data\n";   

    // Make local copies of all of the objects that may be affected by targeter
    // loop iterations
    std::map<std::string, GmatBase *>::iterator pair = objectMap->begin();
    GmatBase *obj;
    
    // Loop through the object map, looking for objects we'll need to restore. 
    while (pair != objectMap->end()) {
        obj = (*pair).second;
        // Save copies of all of the spacecraft
        if (obj->GetType() == Gmat::SPACECRAFT)
            localStore.push_back(new Spacecraft((Spacecraft&)(*obj)));
        ++pair;
    }
}


void Target::ResetLoopData(void)
{
std::cout << "Restoring loop data\n";   
    Spacecraft *sc;
    std::string name;
    
    for (std::vector<GmatBase *>::iterator i = localStore.begin(); 
            i != localStore.end(); ++i) {
        name = (*i)->GetName();
std::cout << name << std::endl;
        GmatBase *gb = (*objectMap)[name];
        if (gb != NULL) {
            sc = (Spacecraft*)gb;
            *sc = *((Spacecraft*)(*i));  // Assignment operator better be right!
Real * state = sc->GetState();
std::cout << "State: " << state[0] << " " << state[1] << " " << state[2] << " " << state[3] << " " << state[4] << " " << state[5] << "\n";   
        }
    }
}


void Target::FreeLoopData(void)
{
    GmatBase *obj;
    while (!localStore.empty()) {
       obj = *(--localStore.end());
       localStore.pop_back();
       delete obj;
    }
}
