//$Id$
//------------------------------------------------------------------------------
//                            SolverBranchCommand
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway
// Created: 2006/10/20
//
/**
 * Definition for the Solver loop command base class (Target, Optimize and 
 * Iterate).
 */
//------------------------------------------------------------------------------


#include "SolverBranchCommand.hpp"
#include "Spacecraft.hpp"
#include "Formation.hpp"

#include "Vary.hpp"           // For SetInitialValue() method

//------------------------------------------------------------------------------
//  SolverBranchCommand(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Creates a SolverBranchCommand command.  (default constructor)
 * 
 * @param <typeStr> The type name of the SolverBranchCommand.
 */
//------------------------------------------------------------------------------
SolverBranchCommand::SolverBranchCommand(const std::string &typeStr) :
   BranchCommand  (typeStr),
   solverName     (""),
   theSolver      (NULL),
   startMode      (RUN_AND_SOLVE),
   exitMode       (DISCARD_AND_CONTINUE),
   specialState   (Solver::INITIALIZING)
{
   parameterCount = SolverBranchCommandParamCount;
   objectTypeNames.push_back("SolverBranchCommand");
   
   solverModes.push_back("RunInitialGuess");
   solverModes.push_back("Solve");
//   solverModes.push_back("RunCorrected");
   
   exitModes.push_back("DiscardAndContinue");
   exitModes.push_back("SaveAndContinue");
   exitModes.push_back("Stop");

}

//------------------------------------------------------------------------------
//  SolverBranchCommand(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Destroys a SolverBranchCommand command.  (destructor)
 */
//------------------------------------------------------------------------------
SolverBranchCommand::~SolverBranchCommand()
{
   if (theSolver)
      delete theSolver;
}


//------------------------------------------------------------------------------
//  SolverBranchCommand(const SolverBranchCommand &sbc)
//------------------------------------------------------------------------------
/**
 * Creates a SolverBranchCommand command based on another.  (Copy constructor)
 * 
 * @param <sbc> The SolverBranchCommand that is copied into this instance.
 */
//------------------------------------------------------------------------------
SolverBranchCommand::SolverBranchCommand(const SolverBranchCommand& sbc) :
   BranchCommand  (sbc),
   solverName     (sbc.solverName),
   theSolver      (NULL),
   startMode      (sbc.startMode),
   exitMode       (sbc.exitMode),
   specialState   (Solver::INITIALIZING)
{
}

//------------------------------------------------------------------------------
//  SolverBranchCommand& operator=(const SolverBranchCommand& sbc)
//------------------------------------------------------------------------------
/**
 * Copies a SolverBranchCommand command.  (Assignment operator)
 * 
 * @param <sbc> The SolverBranchCommand that is copied into this instance.
 * 
 * @return This instance, configured to match sbc.
 */
//------------------------------------------------------------------------------
SolverBranchCommand& SolverBranchCommand::operator=(
   const SolverBranchCommand& sbc)
{
   if (&sbc != this)
   {
      BranchCommand::operator=(sbc);
      solverName   = sbc.solverName;
      theSolver    = NULL;
      startMode    = sbc.startMode;
      exitMode     = sbc.exitMode;
      specialState = Solver::INITIALIZING;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
//  GmatCommand* GetNext()
//------------------------------------------------------------------------------
/**
 * Access the next command in the mission sequence.
 *
 * For SolverBranchCommands, this method returns its own pointer while the child
 * commands are executingm, and it tells the Publisher about a state change after
 * the Solver has finished its work.
 *
 * @return The next command, or NULL if the sequence has finished executing.
 */
//------------------------------------------------------------------------------
GmatCommand* SolverBranchCommand::GetNext()
{
   // Return the next pointer in the command sequence if this command -- 
   // includng its branches -- has finished executing.
   if ((commandExecuting) && (!commandComplete))
      return this;
   
   // Set state back to RUNNING
   if (publisher)
      publisher->SetRunState(Gmat::RUNNING);
   
   if (((commandExecuting) && (commandComplete)) && (exitMode == STOP))
   {
      std::string msg = 
         "Mission interrupted -- Solver is running with ExitMode = \"Stop\"\n";
      throw CommandException(msg);
   }
   
   return next;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void StoreLoopData()
//------------------------------------------------------------------------------
/**
 * Makes local copies of the data so that a solver loop can recover initial
 * data while iterating.
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::StoreLoopData()
{
   // Make local copies of all of the objects that may be affected by optimize
   // loop iterations
   // Check the Local Object Store first
   std::map<std::string, GmatBase *>::iterator pair = objectMap->begin();
   GmatBase *obj;
    
   // Loop through the object map, looking for objects we'll need to restore.
   while (pair != objectMap->end()) 
   {
      obj = (*pair).second;
      // Save copies of all of the spacecraft
      if (obj->GetType() == Gmat::SPACECRAFT)
      {
         Spacecraft *orig = (Spacecraft*)(obj);
         Spacecraft *sc = new Spacecraft(*orig);
         // Handle CoordinateSystems
         if (orig->GetInternalCoordSystem() == NULL)
            MessageInterface::ShowMessage(
               "Internal CS is NULL on spacecraft %s prior to optimizer cloning\n",
               orig->GetName().c_str());
         if (orig->GetRefObject(Gmat::COORDINATE_SYSTEM, "") == NULL)
            MessageInterface::ShowMessage(
               "Coordinate system is NULL on spacecraft %s prior to optimizer cloning\n",
               orig->GetName().c_str());
         sc->SetInternalCoordSystem(orig->GetInternalCoordSystem());
         sc->SetRefObject(orig->GetRefObject(Gmat::COORDINATE_SYSTEM, ""),
            Gmat::COORDINATE_SYSTEM, "");
         
         localStore.push_back(sc);
      }
      if (obj->GetType() == Gmat::FORMATION)
      {
         Formation *orig = (Formation*)(obj);
         Formation *form  = new Formation(*orig);
         localStore.push_back(form);
      }
      ++pair;
   }
   // Check the Global Object Store next
   std::map<std::string, GmatBase *>::iterator globalPair = globalObjectMap->begin();
    
   // Loop through the object map, looking for objects we'll need to restore.
   while (globalPair != globalObjectMap->end()) 
   {
      obj = (*globalPair).second;
      // Save copies of all of the spacecraft
      if (obj->GetType() == Gmat::SPACECRAFT)
      {
         Spacecraft *orig = (Spacecraft*)(obj);
         Spacecraft *sc = new Spacecraft(*orig);
         // Handle CoordinateSystems
         if (orig->GetInternalCoordSystem() == NULL)
            MessageInterface::ShowMessage(
               "Internal CS is NULL on spacecraft %s prior to optimizer cloning\n",
               orig->GetName().c_str());
         if (orig->GetRefObject(Gmat::COORDINATE_SYSTEM, "") == NULL)
            MessageInterface::ShowMessage(
               "Coordinate system is NULL on spacecraft %s prior to optimizer cloning\n",
               orig->GetName().c_str());
         sc->SetInternalCoordSystem(orig->GetInternalCoordSystem());
         sc->SetRefObject(orig->GetRefObject(Gmat::COORDINATE_SYSTEM, ""),
            Gmat::COORDINATE_SYSTEM, "");
         
         localStore.push_back(sc);
      }
      if (obj->GetType() == Gmat::FORMATION)
      {
         Formation *orig = (Formation*)(obj);
         Formation *form  = new Formation(*orig);
         localStore.push_back(form);
      }
      ++globalPair;
   }
}


//------------------------------------------------------------------------------
// void ResetLoopData()
//------------------------------------------------------------------------------
/**
 * Resets starting data from local copies so that a solver loop can iterate.
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::ResetLoopData()
{
   Spacecraft *sc;
   Formation  *fm;
   std::string name;
    
   for (std::vector<GmatBase *>::iterator i = localStore.begin();
        i != localStore.end(); ++i) {
      name = (*i)->GetName();
      //GmatBase *gb = (*objectMap)[name];
      GmatBase *gb = FindObject(name);
      if (gb != NULL) {
         if (gb->GetType() == Gmat::SPACECRAFT)
         {
            sc = (Spacecraft*)gb;
            *sc = *((Spacecraft*)(*i));
         }
         else if (gb->GetType() == Gmat::FORMATION)
         {
            fm = (Formation*)gb;
            *fm = *((Formation*)(*i));
         }
      }
   }
   // Reset the propagators so that propagations run identically loop to loop
   BranchCommand::TakeAction("ResetLoopData");
//   GmatCommand *cmd = branch[0];
//   while (cmd != this)
//   {
//      if (cmd->GetTypeName() == "Propagate")
//         cmd->TakeAction("ResetLoopData");
//      cmd = cmd->GetNext();
//   }
}


//------------------------------------------------------------------------------
// void FreeLoopData()
//------------------------------------------------------------------------------
/**
 * Cleans up starting data store after solver has completed.
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::FreeLoopData()
{
   GmatBase *obj;
   while (!localStore.empty()) {
      obj = *(--localStore.end());
      localStore.pop_back();
      delete obj;
   }
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Solver commands have one of the following syntaxes:
 * 
 *    Target DC
 *    Target DC {SolveMode = Solve}
 *    Target DC {ExitMode = DiscardAndContinue}
 *    Target DC {SolveMode = RunInitialGuess, ExitMode = SaveAndContinue}
 * 
 *    Optimize VF13
 *    Optimize VF13 {SolveMode = Solve}
 *    Optimize VF13 {ExitMode = SaveAndContinue}
 *    Optimize VF13 {SolveMode = RunInitialGuess, ExitMode = Stop}
 * 
 * If the undecorated command is used, the default values (SolveMode = Solve, 
 * ExitMode = DiscardAndContinue) are used.
 *
 * This method breaks the script line into the corresponding pieces, and stores
 * the name of the Solver and all specified solver options.
 *
 * @return true on successful parsing of the command.
 */
//------------------------------------------------------------------------------
bool SolverBranchCommand::InterpretAction()
{
   #ifdef DEBUG_SOLVERBC_ASSEMBLE
      MessageInterface::ShowMessage
         ("%s::InterpretAction() genString = \"%s\"\n", typeName.c_str(),
          generatingString.c_str());
   #endif
   
   StringArray blocks = parser.DecomposeBlock(generatingString);

   StringArray chunks = parser.SeparateBrackets(blocks[0], "{}", " ", false);

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("Chunks from \"%s\":\n", 
            blocks[0].c_str());
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
   #endif
   
   if (chunks.size() < 2)
      throw CommandException(typeName + "::InterpretAction() cannot identify "
            "the Solver -- is it missing? -- in line\n" + generatingString);

   if (chunks.size() > 3)
      throw CommandException(typeName + 
            "::InterpretAction() found too many components to parse in the "
            "line\n" + generatingString);

   if (chunks[0] != typeName)
      throw CommandException(typeName + "::InterpretAction() does not identify "
            "the correct Solver type in line\n" + generatingString);
   
   solverName = chunks[1];

   if (chunks.size() == 3)
      CheckForOptions(chunks[2]);

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("%s::InterpretAction for \"%s\", type = %s\n", 
            typeName.c_str(), generatingString.c_str(), typeName.c_str());
      MessageInterface::ShowMessage("   Solver name:     \"%s\"\n", 
            solverName.c_str());
      MessageInterface::ShowMessage("   SolveMode:       %d\n", startMode);
      MessageInterface::ShowMessage("   ExitMode:        %d\n", exitMode);
   #endif
   
   return true;
}


void SolverBranchCommand::CheckForOptions(std::string &opts)
{
   StringArray chunks = parser.SeparateBrackets(opts, "{}", ", ", true);

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("Chunks from \"%s\":\n", opts.c_str());
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
   #endif
   
   for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
   {
      StringArray option = parser.SeparateBy(*i, "= ");

      #ifdef DEBUG_PARSING
         MessageInterface::ShowMessage("Options from \"%s\":\n", i->c_str());
         for (StringArray::iterator i = option.begin(); i != option.end(); ++i)
            MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
      #endif
         
      if (option.size() != 2)
         throw CommandException(typeName + "::InterpretAction() Solver option "
               "is not in the form option = value in line\n" + 
               generatingString);
         
      if (option[0] == "SolveMode")
      {
         if (option[1] == "Solve")
            startMode = RUN_AND_SOLVE;
         else if (option[1] == "RunInitialGuess")
            startMode = RUN_INITIAL_GUESS;
         else
            throw CommandException(typeName + "::InterpretAction() Solver "
                  "SolveMode option " + option[1] + 
                  " is not a recognized value on line\n" + generatingString +
                  "\nAllowed values are \"Solve\" and \"RunInitialGuess\"\n");
      }
      else if (option[0] == "ExitMode")
      {
         if (option[1] == "DiscardAndContinue")
            exitMode = DISCARD_AND_CONTINUE;
         else if (option[1] == "SaveAndContinue")
            exitMode = SAVE_AND_CONTINUE;
         else if (option[1] == "Stop")
            exitMode = STOP;
         else
            throw CommandException(typeName + "::InterpretAction() Solver "
                  "ExitMode option " + option[1] + 
                  " is not a recognized value on line\n" + generatingString +
                  "\nAllowed values are \"DiscardAndContinue\", "
                  "\"SaveAndContinue\", and \"Stop\"\n");
      }
      else
      {
         throw CommandException(typeName + 
               "::InterpretAction() Solver option " + option[0] + 
               " is not a recognized option on line\n" + generatingString +
                                 "\nAllowed options are \"SolveMode\" and "
               "\"ExitMode\"\n");
      }         
   }
}


std::string SolverBranchCommand::GetSolverOptionText()
{
   std::string optionString = "";
   
   if (!((startMode == RUN_AND_SOLVE) && (exitMode == DISCARD_AND_CONTINUE)))
   {
      bool startModeSet = false;
      optionString += " {";
      
      // Handle the SolveMode options
      if (startMode == RUN_INITIAL_GUESS)
      {
         optionString += "SolveMode = RunInitialGuess";
         startModeSet = true;
      }
      
      // Next handle the ExitMode options
      if (exitMode == SAVE_AND_CONTINUE)
      {
         if (startModeSet)
            optionString += ", ";
         optionString += "ExitMode = SaveAndContinue";
      }
      if (exitMode == STOP)
      {
         if (startModeSet)
            optionString += ", ";
         optionString += "ExitMode = Stop";
      }
      
      optionString += "}";
   }
   
   return optionString;
}


bool SolverBranchCommand::TakeAction(const std::string &action, 
      const std::string &actionData)
{
   
   MessageInterface::ShowMessage("Taking action %s\n", action.c_str());
   
   if (action == "ApplyCorrections")
   {
      if (theSolver == NULL)
      {
         MessageInterface::PopupMessage(Gmat::INFO_, 
               "Please run the mission first.  Corrections cannot be applied "
               "until the solver control sequence has been run.");
         return true;
      }
      // This action walks through the solver loop and passes the Solver's 
      // variable values to the Vary commands that use that solver, updating the
      // initial values for the variables.  The solver must have run once first,
      // though it need not have converged.
      Integer status = theSolver->GetIntegerParameter(
            theSolver->GetParameterID("SolverStatus"));
      
      if ((status == Solver::CREATED) ||
          (status == Solver::COPIED) ||
          (status == Solver::INITIALIZED))
      {
         MessageInterface::PopupMessage(Gmat::INFO_, 
               "Please run the mission first.  Corrections cannot be applied "
               "until the solver control sequence has been run.");
         return true;
      }
      
      ApplySolution();
   
      return true;
   }
   
   return BranchCommand::TakeAction(action, actionData);
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
std::string SolverBranchCommand::GetParameterText(const Integer id) const
{
   if (id == SOLVER_NAME_ID)
      return "SolverName";
   if (id == SOLVER_SOLVE_MODE)
      return "SolveMode";
    
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
Integer SolverBranchCommand::GetParameterID(const std::string &str) const
{
   if (str == "SolverName")
      return SOLVER_NAME_ID;
   if (str == "SolveMode")
      return SOLVER_SOLVE_MODE;
   if (str == "ExitMode")
      return SOLVER_EXIT_MODE;
   if (str == "SolveModeOptions")
      return SOLVER_SOLVE_MODE_OPTIONS;
   if (str == "ExitModeOptions")
      return SOLVER_EXIT_MODE_OPTIONS;
    
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
Gmat::ParameterType SolverBranchCommand::GetParameterType(const Integer id) const
{
   if (id == SOLVER_NAME_ID)
      return Gmat::STRING_TYPE;
   if (id == SOLVER_SOLVE_MODE)
      return Gmat::STRING_TYPE;
   if (id == SOLVER_SOLVE_MODE_OPTIONS)
      return Gmat::STRINGARRAY_TYPE;
    
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
std::string SolverBranchCommand::GetParameterTypeString(const Integer id) const
{
   if (id == SOLVER_NAME_ID)
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
   if (id == SOLVER_SOLVE_MODE)
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
   if (id == SOLVER_SOLVE_MODE_OPTIONS)
      return PARAM_TYPE_STRING[Gmat::STRINGARRAY_TYPE];
    
   return BranchCommand::GetParameterTypeString(id);
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
bool SolverBranchCommand::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == SOLVER_NAME_ID)
   {
      solverName = value;
      return true;
   }

   if (id == SOLVER_SOLVE_MODE) 
   {
      if (value == "RunInitialGuess")
         startMode = RUN_INITIAL_GUESS;
      else if (value == "Solve")
         startMode = RUN_AND_SOLVE;
      else if (value == "RunCorrected")
         startMode = RUN_SOLUTION;
      else
         throw CommandException("Unknown solver mode \"" + value + 
               "\"; known values are {\"RunInitialGuess\", \"Solve\" "
               "\"RunCorrected\"}");
      return true;
   }
    
   if (id == SOLVER_EXIT_MODE) 
   {
      if (value == "DiscardAndContinue")
         exitMode = DISCARD_AND_CONTINUE;
      else if (value == "SaveAndContinue")
         exitMode = SAVE_AND_CONTINUE;
      else if (value == "Stop")
         exitMode = STOP;
      else
         throw CommandException("Unknown solver exit mode \"" + value + 
               "\"; known values are {\"DiscardAndContinue\", "
               "\"SaveAndContinue\", \"Stop\"}");
      return true;
   }
   
   return BranchCommand::SetStringParameter(id, value);
}

std::string SolverBranchCommand::GetStringParameter(const Integer id) const
{
   if (id == SOLVER_NAME_ID)
      return solverName;

   if (id == SOLVER_SOLVE_MODE) 
   {
      if (startMode == RUN_INITIAL_GUESS)
         return "RunInitialGuess";
      if (startMode == RUN_AND_SOLVE)
         return "Solve";
      if (startMode == RUN_SOLUTION)
         return "RunCorrected";
   }
   
   if (id == SOLVER_EXIT_MODE) 
   {
      if (exitMode == DISCARD_AND_CONTINUE)
         return "DiscardAndContinue";
      if (exitMode == SAVE_AND_CONTINUE)
         return "SaveAndContinue";
      if (exitMode == STOP)
         return "Stop";
   }
   
   return BranchCommand::GetStringParameter(id);
}


std::string SolverBranchCommand::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


const StringArray& SolverBranchCommand::GetStringArrayParameter(const Integer id) const
{
   if (id == SOLVER_SOLVE_MODE_OPTIONS)
      return solverModes;
   
   if (id == SOLVER_EXIT_MODE_OPTIONS)
      return exitModes;
   
   return BranchCommand::GetStringArrayParameter(id);
}


const StringArray& SolverBranchCommand::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool NeedsServerStartup()
//------------------------------------------------------------------------------
/**
 * Indicates in the engine needs to start an external process to run the command
 * 
 * The Sandbox calls this method and tells the Moderator to start the MATLAB
 * engine if a true value is returned.  Note that the method name is not MATLAB 
 * specific; future builds may provide additional interfaces so that specific
 * servers can be started -- for example, Octave -- rather than just assuming 
 * that MATLAB is the engine to start.
 * 
 * @return true if a solver is an external solver, false otherwise.
 */
//------------------------------------------------------------------------------
bool SolverBranchCommand::NeedsServerStartup()
{
   if (theSolver == NULL)
      throw CommandException("The Solver pointer is not set in command\n" +
               GetGeneratingString());
   
   if (theSolver->IsSolverInternal())
      return false;
   
   return true;
}

//------------------------------------------------------------------------------
// void ApplySolution()
//------------------------------------------------------------------------------
/**
 * Tells the Solver to update the initial values of the variables with the 
 * most recent solved state. 
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::ApplySolution()
{
   // Walk through the solver loop, setting new variable values as needed
   GmatCommand *current;
   std::vector<GmatCommand*>::iterator node;
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      current = *node;

      while ((current != NULL) && (current != this))
      {
         if (current->GetTypeName() == "Vary")
            ((Vary*)current)->SetInitialValue(theSolver);
         current = current->GetNext();
      }
   }
}
