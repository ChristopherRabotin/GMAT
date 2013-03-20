//$Id$
//------------------------------------------------------------------------------
//                            SolverBranchCommand
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "Vary.hpp"                // For SetInitialValue() method
#include "Subscriber.hpp"
#include "MessageInterface.hpp"
#include "EventLocator.hpp"
#include "FormationInterface.hpp"

#include <sstream>                 // for <<

//#define DEBUG_PARSING
//#define DEBUG_OPTIONS

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif


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
   objectTypeNames.push_back("SolverCommand");
   
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
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (theSolver, "local solver", "SolverBranchCommand::~SolverBranchCommand()",
          "deleting local solver");
      #endif
      delete theSolver;
   }
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
   ChangeRunState(Gmat::RUNNING);
   
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
   GmatBase *obj = NULL;
   
   // Loop through the object map, looking for objects we'll need to restore.
   while (pair != objectMap->end()) 
   {
      obj = (*pair).second;
      
      if (obj == NULL)
         throw CommandException
            (typeName + "::StoreLoopData() cannot continue "
             "due to NULL object pointer in " + generatingString);
      
      // Save copies of all of the spacecraft
      if (obj->GetType() == Gmat::SPACECRAFT)
      {
         Spacecraft *orig = (Spacecraft*)(obj);
         Spacecraft *sc = new Spacecraft(*orig);
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            ((GmatBase*)sc, "cloned local sc", "SolverBranchCommand::StoreLoopData()",
             "Spacecraft *sc = new Spacecraft(*orig)");
         #endif
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
         FormationInterface *orig = (FormationInterface*)(obj);
         FormationInterface *form = (FormationInterface*)orig->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            ((GmatBase*)form, "cloned local form", "SolverBranchCommand::StoreLoopData()",
             "Formation *form  = new Formation(*orig)");
         #endif
         
         localStore.push_back(form);
      }
      if (obj->GetType() == Gmat::EVENT_LOCATOR)
      {
         EventLocator *orig = (EventLocator*)(obj);
         EventLocator *el  = (EventLocator*)orig->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            ((GmatBase*)el, "cloned local eventLocator",
             "SolverBranchCommand::StoreLoopData()",
             "EventLocator *el  = new EventLocator(*orig)");
         #endif

         localStore.push_back(el);
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
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            ((GmatBase*)sc, "cloned local sc",
             "SolverBranchCommand::StoreLoopData()",
             "Spacecraft *sc = new Spacecraft(*orig)");
         #endif
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
         FormationInterface *orig = (FormationInterface*)(obj);
         FormationInterface *form  = (FormationInterface*)orig->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            ((GmatBase*)form, "cloned local form",
             "SolverBranchCommand::StoreLoopData()",
             "Formation *form  = new Formation(*orig)");
         #endif
         localStore.push_back(form);
      }
      if (obj->GetType() == Gmat::EVENT_LOCATOR)
      {
         EventLocator *orig = (EventLocator*)(obj);
         EventLocator *el  = (EventLocator*)orig->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            ((GmatBase*)el, "cloned local eventLocator",
             "SolverBranchCommand::StoreLoopData()",
             "EventLocator *el  = new EventLocator(*orig)");
         #endif

         localStore.push_back(el);
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
   FormationInterface *fm;
   EventLocator *el;
   std::string name;
    
   for (std::vector<GmatBase *>::iterator i = localStore.begin();
        i != localStore.end(); ++i)
   {
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
            fm = (FormationInterface*)gb;
            *fm = *((FormationInterface*)(*i));
         }
         else if (gb->GetType() == Gmat::EVENT_LOCATOR)
         {
            el = (EventLocator*)gb;
            *el = *((EventLocator*)(*i));
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

   // Now push the current data point to the subscribers to avoid a data gap

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
   while (!localStore.empty())
   {
      obj = *(--localStore.end());
      localStore.pop_back();
      #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (obj, obj->GetName(), "SolverBranchCommand::FreeLoopData()",
             "deleting local obj");
      #endif
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
bool SolverBranchCommand::RenameRefObject(const Gmat::ObjectType type,
                                          const std::string &oldName,
                                          const std::string &newName)
{
   if (type == Gmat::SOLVER)
   {
      if (solverName == oldName)
         solverName = newName;
   }
   
   BranchCommand::RenameRefObject(type, oldName, newName);
   
   return true;
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the Vary.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& SolverBranchCommand::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   ObjectTypeArray parentRefTypes = BranchCommand::GetRefObjectTypeArray();
   copy(parentRefTypes.begin(), parentRefTypes.end(), back_inserter(refObjectTypes));
   refObjectTypes.push_back(Gmat::SOLVER);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref objects used by the Vary.
 *
 * @param <type> The type of object desired, or Gmat::UNKNOWN_OBJECT for the
 *               full list.
 * 
 * @return the list of object names.
 * 
 */
//------------------------------------------------------------------------------
const StringArray& SolverBranchCommand::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   refObjectNames.clear();
   StringArray parentRefNames = BranchCommand::GetRefObjectNameArray(type);
   copy(parentRefNames.begin(), parentRefNames.end(), back_inserter(refObjectNames));
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SOLVER)
      refObjectNames.push_back(solverName);
   
   return refObjectNames;
}

//------------------------------------------------------------------------------
// void CheckForOptions(std::string &opts)
//------------------------------------------------------------------------------
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
         #ifdef DEBUG_EXITMODE
            MessageInterface::ShowMessage("Setting solvemode to %s\n",
                  option[1].c_str());
         #endif
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
   #ifdef DEBUG_OPTIONS
      MessageInterface::ShowMessage("Entering GetSolverOptionText with startMode = %d, and exitMode = %d\n",
            (Integer) startMode, (Integer) exitMode);
   #endif
   std::string optionString = "";
   optionString += " {SolveMode = ";
   optionString += GetStringParameter(SOLVER_SOLVE_MODE);
   optionString += ", ExitMode = ";
   optionString += GetStringParameter(SOLVER_EXIT_MODE);
   optionString += "}";

   #ifdef DEBUG_OPTIONS
      MessageInterface::ShowMessage("Exiting GetSolverOptionText and optionString = %s\n",
            optionString.c_str());
   #endif
   
   return optionString;
}


//-----------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//-----------------------------------------------------------------------------
/**
 * Applies a programmatic action to the comamnd
 *
 * @param action String describing the desired action
 * @param actionData Additional data used by teh action (when needed)
 * 
 * @return true is the action was applied, false if not.
 */
//-----------------------------------------------------------------------------
bool SolverBranchCommand::TakeAction(const std::string &action, 
      const std::string &actionData)
{
   #ifdef DEBUG_SOLVER_ACTIONS
      MessageInterface::ShowMessage("Taking action %s\n", action.c_str());
   #endif
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


//------------------------------------------------------------------------------
// void GetActiveSubscribers()
//------------------------------------------------------------------------------
/**
 * Builds a list of subscribers that are active for use in color changes and
 * pen up/down actions
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::GetActiveSubscribers()
{
   // Builds a table of Subscribers that are currently receiving data
   // Currently only set to work with XY Plots

   activeSubscribers.clear();

   for (ObjectMap::iterator i = objectMap->begin(); i != objectMap->end(); ++i)
   {
      GmatBase *obj = i->second;
      if (obj->IsOfType(Gmat::SUBSCRIBER))
      {
         // Here's where we go XY specific
         if (obj->IsOfType("XYPlot"))
         {
            if (obj->GetBooleanParameter("Drawing"))
               activeSubscribers.push_back((Subscriber*)(obj));
         }
      }
   }

   for (ObjectMap::iterator i = globalObjectMap->begin();
         i != globalObjectMap->end(); ++i)
   {
      GmatBase *obj = i->second;
      if (obj->IsOfType(Gmat::SUBSCRIBER))
      {
         // Here's where we go XY specific
         if (obj->IsOfType("XYPlot"))
         {
            if (obj->GetBooleanParameter("Drawing"))
               activeSubscribers.push_back((Subscriber*)(obj));
         }
      }
   }
}


//------------------------------------------------------------------------------
// void PenUpSubscribers()
//------------------------------------------------------------------------------
/**
 * Sends a PenUp command to all active subscribers
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::PenUpSubscribers()
{
   for (UnsignedInt i = 0; i < activeSubscribers.size(); ++i)
      activeSubscribers[i]->TakeAction("PenUp");
}


//------------------------------------------------------------------------------
// void PenDownSubscribers()
//------------------------------------------------------------------------------
/**
 * Sends a PenDown command to all active subscribers
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::PenDownSubscribers()
{
   for (UnsignedInt i = 0; i < activeSubscribers.size(); ++i)
   {
      activeSubscribers[i]->TakeAction("PenDown");
   }
}


//------------------------------------------------------------------------------
// void DarkenSubscribers(Integer denominator = 1)
//------------------------------------------------------------------------------
/**
 * Darkens subscribers by a specified amount
 *
 * This method darkens by 1 / denominator
 *
 * @param denominator The darkening factor
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::DarkenSubscribers(Integer denominator)
{
//   MessageInterface::ShowMessage("Darkening by %d\n", denominator);

   std::stringstream factor;
   factor << denominator;
   for (UnsignedInt i = 0; i < activeSubscribers.size(); ++i)
      activeSubscribers[i]->TakeAction("Darken", factor.str().c_str());
}


//------------------------------------------------------------------------------
// void LightenSubscribers(Integer denominator = 1)
//------------------------------------------------------------------------------
/**
 * Lightens subscribers by a specified amount
 *
 * This method lightens by 1 / denominator
 *
 * @param denominator The lightening factor
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::LightenSubscribers(Integer denominator)
{
   std::stringstream factor;
   factor << denominator;
   for (UnsignedInt i = 0; i < activeSubscribers.size(); ++i)
      activeSubscribers[i]->TakeAction("Lighten", factor.str().c_str());
}


//------------------------------------------------------------------------------
// void SetSubscriberBreakpoint()
//------------------------------------------------------------------------------
/**
 * Marks a break point on a plot
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::SetSubscriberBreakpoint()
{
   for (UnsignedInt i = 0; i < activeSubscribers.size(); ++i)
      activeSubscribers[i]->TakeAction("MarkBreak");
}

//------------------------------------------------------------------------------
// void ApplySubscriberBreakpoint(Integer bp = -1)
//------------------------------------------------------------------------------
/**
 * Breaks the curves on the subscribers, throwing away data beyond the break
 * point
 *
 * @param bp The index of the breakpoint
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::ApplySubscriberBreakpoint(Integer bp)
{
   std::stringstream breakpoint;
   breakpoint << bp;
   for (UnsignedInt i = 0; i < activeSubscribers.size(); ++i)
      activeSubscribers[i]->TakeAction("ClearFromBreak", breakpoint.str());
}


//------------------------------------------------------------------------------
// Integer GetCloneCount()
//------------------------------------------------------------------------------
/**
 * Retrieves the clone count for the members of the solver control sequence
 *
 * @return The count
 */
//------------------------------------------------------------------------------
Integer SolverBranchCommand::GetCloneCount()
{
   cloneCount = BranchCommand::GetCloneCount();
   if (theSolver != NULL)
      ++cloneCount;

   return cloneCount;
}


//------------------------------------------------------------------------------
// GmatBase* GetClone(Integer cloneIndex = 0)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a clone so its attributes can be accessed
 *
 * @param cloneIndex The index into the clone array
 *
 * @return The clone pointer, or NULL if no clone exists
 */
//------------------------------------------------------------------------------
GmatBase* SolverBranchCommand::GetClone(Integer cloneIndex)
{
   GmatBase *retPtr = NULL;

   if (cloneIndex == 0)
      retPtr = theSolver;
   else
      retPtr = BranchCommand::GetClone(cloneIndex - 1);

   return retPtr;
}


//------------------------------------------------------------------------------
// bool HasLocalClones()
//------------------------------------------------------------------------------
/**
 * Method used to check for the (possible) existence of local clones
 *
 * @return true if this object can have local clones of configured objects
 */
//------------------------------------------------------------------------------
bool SolverBranchCommand::HasLocalClones()
{
   return true;
}

//------------------------------------------------------------------------------
// void UpdateClonedObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Passes updated parameter data in to an owned clone object
 *
 * @param obj The configured object that was cloned
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::UpdateClonedObject(GmatBase *obj)
{
   if (obj->GetName() == solverName)
   {
      if (obj->GetTypeName() == theSolver->GetTypeName())
         theSolver = (Solver*)obj;
      else
         throw CommandException("Object type mismatch (clone is a " +
               theSolver->GetTypeName() + ", original is a " +
               obj->GetTypeName() + ") when updating cloned solvers");
   }
}


//------------------------------------------------------------------------------
// void UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
//------------------------------------------------------------------------------
/**
 * This method changes a single parameter on an owned clone
 *
 * @param obj The master object holding the new parameter value
 * @param updatedParameterId The ID of the updated parameter
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::UpdateClonedObjectParameter(GmatBase *obj,
      Integer updatedParameterId)
{
   if (obj->GetName() == solverName)
      if (obj->GetTypeName() == theSolver->GetTypeName())
         theSolver->CopyParameter(*obj, updatedParameterId);
}


//------------------------------------------------------------------------------
// void PrepareToPublish(bool publishAll)
//------------------------------------------------------------------------------
/**
 * Prepares the command for data publishing
 *
 * @param publishAll Flag used to indicate if everything should be published
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::PrepareToPublish(bool publishAll)
{
   StringArray owners, elements;

   if (publishAll)
   {
      owners.push_back("All");
      elements.push_back("All.epoch");
   }

   streamID = publisher->RegisterPublishedData(this, streamID, owners,
         elements);
}


//------------------------------------------------------------------------------
// void PublishData()
//------------------------------------------------------------------------------
/**
 * Pushes provider data to the publisher
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::PublishData()
{
   publisher->Publish(this, streamID, NULL, 0);
}


//------------------------------------------------------------------------------
// void ChangeRunState(Gmat::RunState newState)
//------------------------------------------------------------------------------
/**
 * Passes run state into all members of the Solver Control Sequence & Publisher
 *
 * @param newState The new run state
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::ChangeRunState(Gmat::RunState newState)
{
   currentRunState = newState;

   if (publisher != NULL)
      publisher->SetRunState(currentRunState);

   GmatCommand *current;
   std::vector<GmatCommand*>::iterator node;
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      current = *node;

      if ((current != NULL) && (current != this))
         current->SetRunState(currentRunState);
   }
}
