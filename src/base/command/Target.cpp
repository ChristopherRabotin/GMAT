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
    BranchCommand      ("Target"),
    targeterName       (""),
    targeter           (NULL),
    nestLevel          (0),
    targeterConverged  (false),
    targeterNameID     (parameterCount),
    TargeterConvergedID(parameterCount+1)
{
    parameterCount += 2;
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
    BranchCommand       (t),
    targeterName        (t.targeterName),
    targeter            (NULL),
    nestLevel           (0),
    targeterNameID      (t.targeterNameID),
    TargeterConvergedID (t.TargeterConvergedID)
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
//  bool Append(GmatCommand *cmd)
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
bool Target::Append(GmatCommand *cmd)
{
// Need to fix the nested branching...
//    if (nestLevel > 0) {
//        Command *cmd = branch[0];
//    }
    
    if (!BranchCommand::Append(cmd))
        return false;
    
    // If at the end of a targeter branch, point that end back to this comand.
    if (cmd->GetTypeName() == "EndTarget") {
        if (nestLevel == 0) {
            cmd->Append(this);
            // Targeter loop is complete; -1 pops to the next higher sequence.
            branchToFill = -1;
        }
        else
            --nestLevel;
    }

    // If it's a nested targeter branch, add to the nest level.
    if (cmd->GetTypeName() == "Target")
        ++nestLevel;

    return true;
}


////------------------------------------------------------------------------------
////  void Target::InterpretAction(void)
////------------------------------------------------------------------------------
///**
// * Parses the command string and builds the corresponding command structures.
// *
// * The Target command has the following syntax:
// *
// *     Target myDC;
// *
// * where myDC is a Solver used to target a set of variables to achieve the
// * corresponding goals.  This method breaks the script line into the 
// * corresponding pieces, and stores the name of the Solver so it can be set to
// * point to the correct object during initialization.
// */
////------------------------------------------------------------------------------
//bool Target::InterpretAction(void)
//{
//    /// @todo: Clean up this hack for the Target::InterpretAction method
//    // Sample string:  "Target myDC;"
//    
//    // Set starting location to the space following the command string
//    Integer loc = generatingString.find("Target", 0) + 6, end;
//    const char *str = generatingString.c_str();
//    
//    // Skip white space
//    while (str[loc] == ' ')
//        ++loc;
//    
//    // Stop at the semicolon if it is there
//    end = generatingString.find(";", loc);
//    
//    
//    std::string component = generatingString.substr(loc, end-loc);
//    if (component == "")
//        throw CommandException("Target string does not identify the targeter");
//    SetStringParameter(targeterNameID, component);
//    
//    return true;
//}



//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Target.
 *
 * @return clone of the Target.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Target::Clone(void) const
{
   return (new Target(*this));
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
    if (str == "TargeterConverged")
        return TargeterConvergedID;
    
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
    if (id == TargeterConvergedID)
        return Gmat::BOOLEAN_TYPE;
    
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
    if (id == TargeterConvergedID)
        return PARAM_TYPE_STRING[Gmat::BOOLEAN_TYPE];
    
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


//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 *
 * @todo Setup the GmatBase Get/Set methods to throw exceptions for invalid
 *       parameter accesses.
 */
bool Target::GetBooleanParameter(const Integer id) const
{
   if (id == TargeterConvergedID)
      return targeterConverged;
      
   return BranchCommand::GetBooleanParameter(id);
}


std::string Target::GetRefObjectName(const Gmat::ObjectType type) const
{
   if (type == Gmat::SOLVER)
      return targeterName;
   return BranchCommand::GetRefObjectName(type);
}


bool Target::SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
{
   if (type == Gmat::SOLVER) {
      targeterName = name;
      return true;
   }
   return BranchCommand::SetRefObjectName(type, name);
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
bool Target::Execute(void)
{
    bool retval = BranchCommand::Execute();
    
    // Drive through the state machine.
    Solver::SolverState state = targeter->GetState();
    GmatCommand *current;
    
    switch (state) {
        case Solver::INITIALIZING:
            // Finalize initialization of the targeter data
            current = branch[0];
            targeterConverged = false;
            while (current != this)  {
                std::string type = current->GetTypeName();
                if ((type == "Target") || (type == "Vary") || (type == "Achieve"))
                    current->Execute();
                current = current->GetNext();
            }
            StoreLoopData();
            break;
            
        case Solver::NOMINAL:
            // Execute the nominal sequence
            if (!commandComplete) {
                ResetLoopData();
                retval = ExecuteBranch();
            }
            break;
            
        case Solver::CHECKINGRUN:
            // Check for convergence; this is done in the targeter state 
            // machine, so this case is a NoOp for the Target command
            break;
            
        case Solver::PERTURBING:
            ResetLoopData();
            retval = ExecuteBranch();
            break;
            
        case Solver::CALCULATING:
            // Calculate the next set of variables to use; this is performed in
            // the targeter -- nothing to be done here
            break;
            
        case Solver::FINISHED:
            // Final clean-up
            commandComplete = true;
            targeterConverged = true;
            
            // Run once more to publish the data from the converged state
            retval = ExecuteBranch();
            break;
            
        case Solver::ITERATING:     // Intentional fall-through
        default:
            throw CommandException("Invalid state in the Targeter state machine");
    }

    targeter->AdvanceState();
    return retval;
}


void Target::StoreLoopData(void)
{
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
    Spacecraft *sc;
    std::string name;
    
    for (std::vector<GmatBase *>::iterator i = localStore.begin(); 
            i != localStore.end(); ++i) {
        name = (*i)->GetName();
        GmatBase *gb = (*objectMap)[name];
        if (gb != NULL) {
            sc = (Spacecraft*)gb;
            *sc = *((Spacecraft*)(*i));  // Assignment operator better be right!
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

