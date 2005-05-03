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


// #define DEBUG_TARGETER_PARSING
// #define DEBUG_TARGETER


//------------------------------------------------------------------------------
//  Target(void)
//------------------------------------------------------------------------------
/**
 * Creates a Target command.  (default constructor)
 */
//------------------------------------------------------------------------------
Target::Target() :
   BranchCommand      ("Target"),
   targeterName       (""),
   targeter           (NULL),
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
Target::~Target()
{
   if (targeter)
      delete targeter;
}

    
//------------------------------------------------------------------------------
//  Target(const Target& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Target command.  (Copy constructor)
 *
 * @param t The target command that is copied.
 */
//------------------------------------------------------------------------------
Target::Target(const Target& t) :
   BranchCommand       (t),
   targeterName        (t.targeterName),
   targeter            (NULL),
   targeterConverged   (false),
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
 * @param t The target command that is copied.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Target& Target::operator=(const Target& t)
{
   if (this == &t)
      return *this;
    
   GmatCommand::operator=(t);

   targeterName      = t.targeterName;
   targeter          = NULL;
   targeterConverged = false;

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
 * @param cmd The command that gets appended.
 *
 * @return true if the Command is appended, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Target::Append(GmatCommand *cmd)
{
   #ifdef DEBUG_TARGETER_PARSING
       MessageInterface::ShowMessage("\nTarget::Append received \"%s\" command",
                                     cmd->GetTypeName().c_str());
   #endif
    
   if (!BranchCommand::Append(cmd))
        return false;
    
   // If at the end of a targeter branch, point that end back to this comand.
   if (cmd->GetTypeName() == "EndTarget") {
      if ((nestLevel == 0) && (branchToFill != -1))  {
         cmd->Append(this);
         // Targeter loop is complete; -1 pops to the next higher sequence.
         branchToFill = -1;
         #ifdef DEBUG_TARGETER_PARSING
             MessageInterface::ShowMessage("\nTarget::Append closing \"%s\"",
                                           generatingString.c_str());
         #endif
      }
      else
         --nestLevel;
   }

   // If it's a nested targeter branch, add to the nest level.
   if (cmd->GetTypeName() == "Target")
      ++nestLevel;

   #ifdef DEBUG_TARGETER_PARSING
       MessageInterface::ShowMessage("\nTarget::Append for \"%s\" nest level = %d",
                                     generatingString.c_str(), nestLevel);
   #endif

   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Target.
 *
 * @return clone of the Target.
 */
//------------------------------------------------------------------------------
GmatBase* Target::Clone() const
{
   return (new Target(*this));
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param <mode>    Specifies the type of serialization requested.
 * @param <prefix>  Optional prefix appended to the object's name. (Used for
 *                  indentation)
 * @param <useName> Name that replaces the object's name (Not yet used
 *                  in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& Target::GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName)
{
   generatingString = "Target " + targeterName + ";";
   return BranchCommand::GetGeneratingString(mode, prefix, useName);
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Target::RenameRefObject(const Gmat::ObjectType type,
                             const std::string &oldName,
                             const std::string &newName)
{
   if (type == Gmat::SOLVER)
   {
      if (targeterName == oldName)
         targeterName = newName;
   }
   
   return true;
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
//---------------------------------------------------------------------------
bool Target::GetBooleanParameter(const Integer id) const
{
   if (id == TargeterConvergedID)
      return targeterConverged;
      
   return BranchCommand::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
//  std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Retrieve the name of a reference object.
 *
 * @param <type> The type of object that is being looked up.
 *
 * @return the object's name.
 */
//------------------------------------------------------------------------------
std::string Target::GetRefObjectName(const Gmat::ObjectType type) const
{
   if (type == Gmat::SOLVER)
      return targeterName;
   return BranchCommand::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Set the name of a reference object.
 *
 * @param <type> The type of object.
 * @param <name> The name of the object.
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Target::SetRefObjectName(const Gmat::ObjectType type,
                              const std::string &name)
{
   if (type == Gmat::SOLVER) {
      targeterName = name;
      return true;
   }
   return BranchCommand::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the targeter.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Target::Initialize()
{
   if (objectMap->find(targeterName) == objectMap->end()) {
      std::string errorString = "Target command cannot find targeter \"";
      errorString += targeterName;
      errorString += "\"";
      throw CommandException(errorString);
   }

   // Clone the targeter for local use
   targeter = (Solver *)((*objectMap)[targeterName])->Clone();
   targeter->TakeAction("IncrementInstanceCount");
   ((*objectMap)[targeterName])->TakeAction("IncrementInstanceCount");
    
   // Set the local copy of the targeter on each node
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *current;

   for (node = branch.begin(); node != branch.end(); ++node)
   {
      current = *node;
      while ((current != NULL) && (current != this))
      {
         if ((current->GetTypeName() == "Vary") || 
             (current->GetTypeName() == "Achieve"))
            current->SetRefObject(targeter, Gmat::SOLVER, targeterName);
         current = current->GetNext();
      }
   }

   bool retval = BranchCommand::Initialize();

   if (retval == true) {
      // Targeter specific initialization goes here:
      if (objectMap->find(targeterName) == objectMap->end()) {
         std::string errorString = "Target command cannot find targeter \"";
         errorString += targeterName;
         errorString += "\"";
         throw CommandException(errorString);
      }

      retval = targeter->Initialize();
   }
        
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
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
bool Target::Execute()
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
            if ((type == "Target") || (type == "Vary") ||
                (type == "Achieve"))
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
         ResetLoopData();
         retval = ExecuteBranch();
         break;
            
      case Solver::ITERATING:     // Intentional fall-through
      default:
         throw CommandException("Invalid state in the Targeter state machine");
   }

   targeter->AdvanceState();

   if (targeter->GetState() == Solver::FINISHED) {
      targeterConverged = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// void StoreLoopData()
//------------------------------------------------------------------------------
/**
 * Makes local copies of the data so that a targeting loop can recover initial
 * data while iterating.
 */
//------------------------------------------------------------------------------
void Target::StoreLoopData()
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


//------------------------------------------------------------------------------
// void ResetLoopData()
//------------------------------------------------------------------------------
/**
 * Resets starting data from local copiesso that a targeting loop can iterate.
 */
//------------------------------------------------------------------------------
void Target::ResetLoopData()
{
   Spacecraft *sc;
   std::string name;
    
   for (std::vector<GmatBase *>::iterator i = localStore.begin();
        i != localStore.end(); ++i) {
      name = (*i)->GetName();
      GmatBase *gb = (*objectMap)[name];
      if (gb != NULL) {
         sc = (Spacecraft*)gb;
          *sc = *((Spacecraft*)(*i));
      }
   }
}


//------------------------------------------------------------------------------
// void FreeLoopData()
//------------------------------------------------------------------------------
/**
 * Cleans up starting data store after targeting has completed.
 */
//------------------------------------------------------------------------------
void Target::FreeLoopData()
{
   GmatBase *obj;
   while (!localStore.empty()) {
      obj = *(--localStore.end());
      localStore.pop_back();
      delete obj;
   }
}
