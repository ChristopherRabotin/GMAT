//$Id$
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
//  Target()
//------------------------------------------------------------------------------
/**
 * Creates a Target command.  (default constructor)
 */
//------------------------------------------------------------------------------
Target::Target() :
   SolverBranchCommand("Target"),
   targeterName       (""),
   targeter           (NULL),
   targeterConverged  (false),
   targeterNameID     (parameterCount),
   TargeterConvergedID(parameterCount+1),
   targeterInDebugMode(false)
{
   parameterCount += 2;
   objectTypeNames.push_back("Target");
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
   SolverBranchCommand (t),
   targeterName        (t.targeterName),
   targeter            (NULL),
   targeterConverged   (false),
   targeterNameID      (t.targeterNameID),
   TargeterConvergedID (t.TargeterConvergedID),
   targeterInDebugMode (t.targeterInDebugMode)
{
   parameterCount = t.parameterCount;
   localStore.clear();
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

   targeterName        = t.targeterName;
   targeter            = NULL;
   targeterConverged   = false;
   targeterNameID      = t.targeterNameID;
   TargeterConvergedID = t.TargeterConvergedID;
   targeterInDebugMode = t.targeterInDebugMode;
   localStore.clear();

   return *this;
}


//------------------------------------------------------------------------------
//  bool Append(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Adds a command to the targeter loop.
 *
 * This method calls the SolverBranchCommand base class method that adds a command
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
    
   if (!SolverBranchCommand::Append(cmd))
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
   if (mode == Gmat::NO_COMMENTS)
   {
      generatingString = "Target " + targeterName + ";";
      return generatingString;
   }
   
   generatingString = prefix + "Target " + targeterName + ";";
   return SolverBranchCommand::GetGeneratingString(mode, prefix, useName);
}


//---------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type,
//                      const std::string &oldName, const std::string &newName)
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

   SolverBranchCommand::RenameRefObject(type, oldName, newName);
   
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
    
   return SolverBranchCommand::GetParameterText(id);
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
    
   return SolverBranchCommand::GetParameterID(str);
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
    
   return SolverBranchCommand::GetParameterType(id);
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
    
   return SolverBranchCommand::GetParameterTypeString(id);
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
    
   return SolverBranchCommand::GetStringParameter(id);
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
    
   return SolverBranchCommand::SetStringParameter(id, value);
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
      
   return SolverBranchCommand::GetBooleanParameter(id);
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
   return SolverBranchCommand::GetRefObjectName(type);
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
   return SolverBranchCommand::SetRefObjectName(type, name);
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
   GmatBase *mapObj = NULL;
   if ((mapObj = FindObject(targeterName)) == NULL) 
   {
      std::string errorString = "Target command cannot find targeter \"";
      errorString += targeterName;
      errorString += "\"";
      throw CommandException(errorString);
   }

   // Clone the targeter for local use
   targeter = (Solver *)(mapObj->Clone());
   targeter->TakeAction("IncrementInstanceCount");
   mapObj->TakeAction("IncrementInstanceCount");
   
   if (targeter->GetStringParameter("ReportStyle") == "Debug")
      targeterInDebugMode = true;      
    
   // Set the local copy of the targeter on each node
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *current;

   for (node = branch.begin(); node != branch.end(); ++node)
   {
      current = *node;

      #ifdef DEBUG_TARGET_COMMANDS
         Integer nodeNum = 0;
      #endif
      while ((current != NULL) && (current != this))
      {
         #ifdef DEBUG_TARGET_COMMANDS
            MessageInterface::ShowMessage(
               "   Target Command %d:  %s\n", ++nodeNum, 
               current->GetTypeName().c_str());       
         #endif
         if ((current->GetTypeName() == "Vary") || 
             (current->GetTypeName() == "Achieve"))
            current->SetRefObject(targeter, Gmat::SOLVER, targeterName);
         current = current->GetNext();
      }
   }

   bool retval = SolverBranchCommand::Initialize();

   if (retval == true) {
      // Targeter specific initialization goes here:
      if (FindObject(targeterName) == NULL) 
      {
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
   bool retval = true;

   // Drive through the state machine.
   Solver::SolverState state = targeter->GetState();
   
   #ifdef DEBUG_TARGET_COMMANDS
      MessageInterface::ShowMessage("TargetExecute(%c%c%c%d)\n",
         (commandExecuting?'Y':'N'),
         (commandComplete?'Y':'N'),
         (branchExecuting?'Y':'N'),
         state);
      MessageInterface::ShowMessage("   targeterConverged=%d\n", targeterConverged);
   #endif
      
   // Attempt to reset if recalled   
   if (commandComplete)
   {
      commandComplete = false;
      commandExecuting = false;
   }  

   if (!commandExecuting) 
   {
      #ifdef DEBUG_TARGET_COMMANDS
          MessageInterface::ShowMessage(
          "Entered Targeter while command is not executing\n");
      #endif

      FreeLoopData();
      StoreLoopData();


      retval = SolverBranchCommand::Execute();

      #ifdef DEBUG_TARGETER
         MessageInterface::ShowMessage("Resetting the Differential Corrector\n");
      #endif

      targeter->TakeAction("Reset");
      state = targeter->GetState();
      
   }
   
   if (branchExecuting)
   {
      retval = ExecuteBranch();
      if (!branchExecuting && (state == Solver::FINISHED))
      {
         commandComplete = true;
      }  
   }
   else
   {
      GmatCommand *currentCmd;
   
      publisher->SetRunState(Gmat::SOLVING);
      
      switch (state) {
         case Solver::INITIALIZING:
            // Finalize initialization of the targeter data
            currentCmd = branch[0];
            targeterConverged = false;
            while (currentCmd != this)  {
               std::string type = currentCmd->GetTypeName();
               if ((type == "Target") || (type == "Vary") ||
                   (type == "Achieve"))
                  currentCmd->Execute();
               currentCmd = currentCmd->GetNext();
            }
            StoreLoopData();
            break;
               
         case Solver::NOMINAL:
            // Execute the nominal sequence
            if (!commandComplete) {
               branchExecuting = true;
               ResetLoopData();
            }
            break;
               
         case Solver::CHECKINGRUN:
            // Check for convergence; this is done in the targeter state
            // machine, so this case is a NoOp for the Target command
            break;
   
         case Solver::PERTURBING:
            branchExecuting = true;
            ResetLoopData();
            break;
               
         case Solver::CALCULATING:
            // Calculate the next set of variables to use; this is performed in
            // the targeter -- nothing to be done here
            break;
               
         case Solver::FINISHED:
            // Final clean-up
            targeterConverged = true;
            
            // Run once more to publish the data from the converged state
            if (!commandComplete)
            {
               ResetLoopData();
               branchExecuting = true;
               publisher->SetRunState(Gmat::SOLVEDPASS);
            }
            break;
               
         case Solver::ITERATING:     // Intentional fall-through
         default:
            throw CommandException(
               "Invalid state in the Targeter state machine");
      }
   }
   
   if (!branchExecuting)
   {
      targeter->AdvanceState();

      if (targeter->GetState() == Solver::FINISHED) {
         targeterConverged = true;
      }
   }

   // Pass spacecraft data to the targeter for reporting in debug mode
   if (targeterInDebugMode)
   {
      std::string dbgData = "";
      for (ObjectArray::iterator i = localStore.begin(); i < localStore.end(); ++i)
      {
         dbgData += (*i)->GetGeneratingString() + "\n---\n";
      }
      targeter->SetDebugString(dbgData);
   }
   BuildCommandSummary(true);
   return retval;
}


//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
/**
 * Tells the sequence that the run was ended, possibly before reaching the end.
 */
//------------------------------------------------------------------------------
void Target::RunComplete()
{
   if (targeter != NULL)
      targeter->Finalize();

   //Call RunComplete of parent (loj: 2/23/06)
   //GmatCommand::RunComplete();
   BranchCommand::RunComplete();
}
