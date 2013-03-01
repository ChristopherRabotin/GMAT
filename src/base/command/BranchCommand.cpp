//$Id$
//------------------------------------------------------------------------------
//                               BranchCommand
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/22
//
/**
 * Base class implementation for the Command classes that branch (Target, If, 
 * While, etc).
 */
//------------------------------------------------------------------------------


#include "BranchCommand.hpp"
#include "MessageInterface.hpp"
#include "CallFunction.hpp"
#include "Assignment.hpp"
#include "CommandUtil.hpp"      // for GetCommandSeqString()
#include <sstream>              // for stringstream

//#define DEBUG_BRANCHCOMMAND_DEALLOCATION
//#define DEBUG_BRANCHCOMMAND_APPEND
//#define DEBUG_BRANCHCOMMAND_INSERT
//#define DEBUG_BRANCHCOMMAND_REMOVE
//#define DEBUG_BRANCHCOMMAND_ADD
//#define DEBUG_BRANCHCOMMAND_PREV_CMD
//#define DEBUG_BRANCHCOMMAND_EXECUTION
//#define DEBUG_BRANCHCOMMAND_GEN_STRING
//#define DEBUG_RUN_COMPLETE
//#define DEBUG_BRANCHCOMMAND_GMATFUNCTIONS
//#define DEBUG_BRANCHCOMMAND_SUMMARY

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  BranchCommand(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Constructs the BranchCommand command (default constructor).
 *
 * @param <typeStr> String setting the type name of the command.
 */
//------------------------------------------------------------------------------
BranchCommand::BranchCommand(const std::string &typeStr) :
   GmatCommand          (typeStr),
   branch               (1),
   commandComplete      (false),
   commandExecuting     (false),
   branchExecuting      (false),
   branchToExecute      (0),
   branchToFill         (0),
   nestLevel            (0),
   current              (NULL),
   lastFired            (NULL)
{
   depthChange = 1;
   parameterCount = BranchCommandParamCount;
   
   objectTypeNames.push_back("BranchCommand");
}

//------------------------------------------------------------------------------
//  BranchCommand(const BranchCommand& bc)
//------------------------------------------------------------------------------
/**
 * Constructs the BranchCommand command (copy constructor).
 *
 * @param <bc> The instance that is copied.
 */
//------------------------------------------------------------------------------
BranchCommand::BranchCommand(const BranchCommand& bc) :
   GmatCommand       (bc),
   branch            (1),
   commandComplete   (false),
   commandExecuting  (false),
   branchExecuting   (false),
   branchToExecute   (0),
   branchToFill      (0),
   nestLevel         (bc.nestLevel),
   current           (NULL),
   lastFired         (NULL)
{
   depthChange = 1;
   parameterCount = BranchCommandParamCount;
}

//------------------------------------------------------------------------------
//  BranchCommand& operator=(const BranchCommand& bc)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <bc> The instance that is copied.
 *
 * @return This instance, set to match the input instance.
 */
//------------------------------------------------------------------------------
BranchCommand& BranchCommand::operator=(const BranchCommand& bc)
{
   if (this != &bc)
   {
      GmatCommand::operator=(bc);
      lastFired = NULL;
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~BranchCommand(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Destroys the BranchCommand (destructor)
 */
//------------------------------------------------------------------------------
BranchCommand::~BranchCommand()
{
   #ifdef DEBUG_BRANCHCOMMAND_DEALLOCATION
      MessageInterface::ShowMessage
         ("In BranchCommand::~BranchCommand() this=<%p> '%s'\n", this,
          this->GetTypeName().c_str());
      MessageInterface::ShowMessage("branch.size()=%d\n", branch.size());
      std::string cmdstr = GmatCommandUtil::GetCommandSeqString(this);
      MessageInterface::ShowMessage("%s\n", cmdstr.c_str());
   #endif
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *current = NULL;
   
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      // Find the end for each branch and disconnect it from the start
      current = *node;
      if (current != NULL)
      {
         #ifdef DEBUG_BRANCHCOMMAND_DEALLOCATION
         ShowCommand("   ", "current=", current);
         #endif
         
         while (current->GetNext() != this && current != current->GetNext())
         {
            current = current->GetNext();
            if (current == NULL)
               break;
         }
         
         // Calling Remove this way just sets the next pointer to NULL
         if (current)
         {
            #ifdef DEBUG_BRANCHCOMMAND_DEALLOCATION
               MessageInterface::ShowMessage("   Removing ", current);
            #endif
               
            current->Remove(current);
         }
         
         if (*node)
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               ((*node), (*node)->GetTypeName(), (*node)->GetTypeName() +
                " deleting child command");
            #endif
            delete *node;
            *node = NULL;
         }
      }
      else
         break;
   }
   
   #ifdef DEBUG_BRANCHCOMMAND_DEALLOCATION
      MessageInterface::ShowMessage("Finished BranchCommand::~BranchCommand()\n");
   #endif
}

//------------------------------------------------------------------------------
//  GmatCommand* GetNext()
//------------------------------------------------------------------------------
/**
 * Access the next command in the mission sequence.
 *
 * For BranchCommands, this method returns its own pointer while the child
 * commands are executing.
 *
 * @return The next command, or NULL if the sequence has finished executing.
 */
//------------------------------------------------------------------------------
GmatCommand* BranchCommand::GetNext()
{
   // Return the next pointer in the command sequence if this command -- 
   // including its branches -- has finished executing.
   if ((commandExecuting) && (!commandComplete))
      return this;
   
   return next;
}

//------------------------------------------------------------------------------
// GmatCommand* BranchCommand::GetNextWhileExecuting()
//------------------------------------------------------------------------------
/**
 * Provides a mechanism to communicate with subsequent commands
 *
 * This method provides unconditional access to the next pointer in the branch
 * command, so that the Sandbox can communicate critical information to the full
 * Mission Control Sequence, even when the branch command is still executing.
 *
 * This method is used to pass owned clone data to the entire MCS when it is
 * generated from a command in a Branch Control Sequence.
 *
 * @return The next command at the current control sequence nesting level,
 *         regardless of the execution state of the current command.
 */
//------------------------------------------------------------------------------
GmatCommand* BranchCommand::GetNextWhileExecuting()
{
   return next;
}

//------------------------------------------------------------------------------
//  GmatCommand* GetChildCommand(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Access the children of this command.
 *
 * @param <whichOne> Identifies which child branch is needed.
 *
 * @return The child command starting the indicated branch, or NULL if there is
 *         no such child.
 */
//------------------------------------------------------------------------------
GmatCommand* BranchCommand::GetChildCommand(Integer whichOne)
{
   if (whichOne > (Integer)(branch.size())-1)
      return NULL;
   return branch[whichOne];
}

//------------------------------------------------------------------------------
//  void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
 * Tells the children about the transient forces.
 *
 * @param <tf> The transient force vector.
 */
//------------------------------------------------------------------------------
void BranchCommand::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   GmatCommand *currentPtr;
   
   std::vector<GmatCommand*>::iterator node;
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      currentPtr = *node;
      while (currentPtr != this)
      {
         currentPtr->SetTransientForces(tf);
         currentPtr = currentPtr->GetNext();
         if (currentPtr == NULL)
            throw CommandException("Branch command \"" + generatingString +
                                   "\" was not terminated!");
      }
   }
}

//------------------------------------------------------------------------------
// void SetEventLocators(std::vector<EventLocator*> *els)
//------------------------------------------------------------------------------
/**
 * Sets up the event location vector for a run.
 *
 * @param els The vector of event locators in the current Sandbox
 */
//------------------------------------------------------------------------------
void BranchCommand::SetEventLocators(std::vector<EventLocator*> *els)
{
   #ifdef DEBUG_EVENTLOCATION
      MessageInterface::ShowMessage("Setting event locator pointer <%p> with "
            "%d members: ", els, (els != NULL ? els->size() : 0));
   #endif
   GmatCommand *currentPtr;

   std::vector<GmatCommand*>::iterator node;

   for (node = branch.begin(); node != branch.end(); ++node)
   {
      currentPtr = *node;
      while (currentPtr != this)
      {
         currentPtr->SetEventLocators(els);
         currentPtr = currentPtr->GetNext();
         if (currentPtr == NULL)
            throw CommandException("Branch command \"" + generatingString +
                                   "\" was not terminated!");
      }
   }
   
   #ifdef DEBUG_EVENTLOCATION
      MessageInterface::ShowMessage("\n");
   #endif
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the BranchCommand command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool BranchCommand::Initialize()
{
   #ifdef DEBUG_BRANCHCOMMAND_INIT
   ShowCommand("BranchCommand::Initialize() entered ", "this=", this);
   #endif
   
   GmatCommand::Initialize();
   
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *currentPtr;
   bool retval = true;
   
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      currentPtr = *node;

      while (currentPtr != this)
      {
         #ifdef DEBUG_BRANCHCOMMAND_INIT
         ShowCommand("About to initialize child in ", "child=", currentPtr);
         #endif
         if (events != NULL)
            currentPtr->SetEventLocators(events);
         if (!currentPtr->Initialize())
               retval = false;
         currentPtr = currentPtr->GetNext();
         if (currentPtr == NULL)
            throw CommandException("Branch command \"" + generatingString +
                                   "\" was not terminated!");
      }
   }
   
   commandComplete  = false;
   commandExecuting = false;
   branchExecuting = false;
   current = NULL;
   lastFired = NULL;
   
   return retval;
}

//------------------------------------------------------------------------------
// void AddBranch(GmatCommand *cmd, Integer which)
//------------------------------------------------------------------------------
/**
 * Adds commands to a branch, starting a new branch if needed.
 *
 * @param <cmd>   The command that is added.
 * @param <which> Identifies which child branch is used.
 */
//------------------------------------------------------------------------------
void BranchCommand::AddBranch(GmatCommand *cmd, Integer which)
{
   #ifdef DEBUG_BRANCHCOMMAND_ADD
   ShowCommand("BranchCommand::", "AddBranch() cmd = ", cmd);
   MessageInterface::ShowMessage
      ("   which=%d, branch.size=%d\n", which, branch.size());
   #endif
   
   // Increase the size of the vector if it's not big enough
   if (which >= (Integer)branch.capacity())
   {
      branch.reserve(which+1);
      //current.reserve(which+1);
   }
   
   if (which == (Integer)(branch.size()))
   {
      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", "::AddBranch() Adding to branch ", cmd);
      #endif
      
      branch.push_back(cmd);
      if (which - 1 >= 0)
         SetPreviousCommand(cmd, branch.at(which-1), true);
   }
   else if (branch[which] == NULL)
   {
      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", " Setting branch.at(which) to ", cmd);
      #endif
      
      branch.at(which) = cmd;
      
      // Usually this case is adding Branch command from the GUI
      SetPreviousCommand(cmd, this, false);
   }
   else
   {
      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", " Appending ", cmd, " to ", branch.at(which));
      #endif
      
      (branch.at(which))->Append(cmd);
      
   }
} // AddBranch()

//------------------------------------------------------------------------------
// void AddToFrontOfBranch(GmatCommand *cmd, Integer which)
//------------------------------------------------------------------------------
/**
 * Adds commands to the beginning of a branch, starting a new branch if needed.
 *
 * @param <cmd>   The command that is added.
 * @param <which> Identifies which child branch is used.
 */
//------------------------------------------------------------------------------
void BranchCommand::AddToFrontOfBranch(GmatCommand *cmd, Integer which)
{
   
   #ifdef DEBUG_BRANCHCOMMAND_ADD
   ShowCommand("BranchCommand::", "AddToFrontOfBranch() cmd = ", cmd);
   MessageInterface::ShowMessage
      ("   which=%d, branch.size=%d\n", which, branch.size());
   #endif
   
   // Increase the size of the vector if it's not big enough
   if (which >= (Integer)branch.capacity())
      branch.reserve(which+1);
   
   if (which == (Integer)(branch.size()))
   {
      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", "::AddToFrontOfBranch() Adding to branch ", cmd);
      #endif
      
      branch.push_back(cmd);
      SetPreviousCommand(cmd, this, true);
      
   }
   else if (branch.at(which) == NULL)
   {
      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", " Setting branch.at(which) to ", cmd);
      #endif
      
      branch.at(which) = cmd;
   }
   else
   {
      GmatCommand *tmp = branch.at(which);
      
      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", " Setting branch.at(which) to ", cmd);
      #endif
      
      branch.at(which) = cmd;
      
      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", " Appending ", tmp, " to ", cmd);
      #endif
      
      cmd->Append(tmp);
      SetPreviousCommand(tmp, cmd, true);
      
   }
} // AddToFrontOfBranch()

//------------------------------------------------------------------------------
// bool BranchCommand::Append(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Appends a command to the mission sequence.
 *
 * Appends commands to the end of the current command stream, either by adding
 * them to a branch, or by adding them after this command based on the internal
 * BranchCommand flags.
 *
 * @param <cmd>   The command that is added.
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool BranchCommand::Append(GmatCommand *cmd)
{
   #ifdef DEBUG_BRANCHCOMMAND_APPEND
   ShowCommand("BranchCommand::", "Append() this = ", this, " next = ", next);
   ShowCommand("BranchCommand::", "Append() cmd = ", cmd);
   #endif
   
   // If we are still filling in a branch, append on that branch
   if (branchToFill >= 0)
   {
      AddBranch(cmd, branchToFill);
      return true;
   }
   #ifdef DEBUG_BRANCHCOMMAND_APPEND
      MessageInterface::ShowMessage(
      "In BranchCommand::Append - not appended to this command.\n");
   #endif
      
   // Otherwise, just call the base class method
   return GmatCommand::Append(cmd);
}

//------------------------------------------------------------------------------
// bool Insert(GmatCommand *cmd, GmatCommand *prev)
//------------------------------------------------------------------------------
/**
 * Inserts a command into the mission sequence.
 *
 * Inserts commands into the command stream immediately after another command,
 * and updates the command list accordingly.
 *
 * @param <cmd>  The command that is added.
 * @param <prev> The command preceding the added one.
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool BranchCommand::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   #ifdef DEBUG_BRANCHCOMMAND_INSERT
   ShowCommand("BranchCommand::", "Insert() this = ", this, " next = ", next);
   ShowCommand("BranchCommand::", "Insert() cmd  = ", cmd, " prev = ", prev);
   #endif
   
   GmatCommand *currentOne   = NULL;
   GmatCommand *toShift      = NULL;
   bool        newBranch     = false;
   Integer     brNum         = -1;
   bool        foundHere     = false;
   bool        hereOrNested  = false;
   
   // if we're adding a new Else or ElseIf, we will need to add a branch 
   if ( (this->GetTypeName() == "If") && 
        ( (cmd->GetTypeName() == "Else") || 
          (cmd->GetTypeName() == "ElseIf") ) )  newBranch = true;
   
   
   // See if we're supposed to put it at the top of the first branch
   if (prev == this)
   {
      
      currentOne = branch[0];
      branch[0] = cmd;
      if (newBranch)
      {
         toShift = currentOne;
         brNum   = 0;
      }
      
      #ifdef DEBUG_BRANCHCOMMAND_INSERT
      ShowCommand("BranchCommand::", "currentOne = ", currentOne, " branch[0] = ", branch[0]);
      #endif
      
      cmd->Append(currentOne);
      foundHere = true;
      
      #ifdef DEBUG_BRANCHCOMMAND_INSERT
      ShowCommand("BranchCommand::", "currentOne->GetPrevious() = ", currentOne->GetPrevious());
      ShowCommand("BranchCommand::", "currentOne->GetNext() = ", currentOne->GetNext());
      #endif
      
      //return true;
   }
   // see if we're supposed to add it to the front of a branch
   // i.e. the prev = the last command in the previous branch
   // (e.g. an Else command)
   // check all but the last branch - End*** should take care of it
   // at that point
   if (!foundHere)
   {
      for (Integer br = 0; br < (Integer) (branch.size() - 1); ++br)
      {
         currentOne = branch.at(br);
         if (currentOne != NULL)
         {
            while (currentOne->GetNext() != this)
               currentOne = currentOne->GetNext();
            if (currentOne == prev) 
            {
               if (newBranch)
               {
                  toShift = currentOne;
                  brNum   = br;
               }
               AddToFrontOfBranch(cmd,br+1);
               foundHere = true;
               //return true;
            }
         }
      }
   }
   // If we have branches, try to insert there first
   if (!foundHere)
   {
      GmatCommand *nc = NULL;
      for (Integer which = 0; which < (Integer)branch.size(); ++which)
      {
         currentOne = branch[which];
         if (currentOne != NULL)
         {
            nc = currentOne;
            while((nc != this) && !foundHere)
            {
                // let a nested If handle it, if it is supposed to go in there
               if (newBranch && (nc == prev) && (nc->GetTypeName() != "If"))
               {
                  toShift   = nc->GetNext();
                  brNum     = which;
                  foundHere = true;
               }
               nc = nc->GetNext();
            }

            hereOrNested = currentOne->Insert(cmd, prev);
            // check to see if it got added after the nested command
            if (hereOrNested && (currentOne->GetNext() == cmd))
            {
               toShift   = cmd->GetNext();
               brNum     = which;
               foundHere = true;
            }
         }
      }
   }
   
   #ifdef DEBUG_BRANCHCOMMAND_INSERT
   MessageInterface::ShowMessage
      ("BranchCommand::Insert() newBranch=%d, foundHere=%d, toShift=%p\n",
       newBranch, foundHere, toShift);
   #endif
   
   if (newBranch && foundHere && (toShift != NULL))
   {
      // make sure the new Else or ElseIf points back to the If command
      
      #ifdef DEBUG_BRANCHCOMMAND_INSERT
      ShowCommand("BranchCommand::", " Setting next of ", cmd, " to ", this);
      #endif
      
      cmd->ForceSetNext(this);
      SetPreviousCommand(this, cmd, false);
      
      // shift all the later commands down one branch
      bool isOK = ShiftBranches(toShift, brNum);
      if (!isOK) 
         MessageInterface::ShowMessage
            ("In BranchCommand::Insert - error adding Else/ElseIf");
   }
   
   if (foundHere || hereOrNested) return true;

   // Otherwise, just call the base class method
   return GmatCommand::Insert(cmd, prev);
} // Insert()

//------------------------------------------------------------------------------
// GmatCommand* Remove(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Removes a command from the mission sequence.
 *
 * @param <cmd>  The command that is to be removed.
 *
 * @return the removed command.
 */
//------------------------------------------------------------------------------
GmatCommand* BranchCommand::Remove(GmatCommand *cmd)
{
   #ifdef DEBUG_BRANCHCOMMAND_REMOVE
   ShowCommand("BranchCommand::", "Remove() this = ", this, " cmd = ", cmd);
   ShowCommand("BranchCommand::", "Remove() next = ", next);
   #endif
   
   if (next == cmd)
      return GmatCommand::Remove(cmd);    // Use base method to remove cmd
   
   if (cmd == this)
      return GmatCommand::Remove(cmd);    // Use base method to remove cmd
   
   GmatCommand *fromBranch = NULL;
   GmatCommand *current = NULL;
   GmatCommand *tempNext = NULL;
   
   // If we have branches, try to remove there first
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      
      if (current != NULL)
      {
         tempNext = current->GetNext();
      
         #ifdef DEBUG_BRANCHCOMMAND_REMOVE
         ShowCommand("BranchCommand::", "Remove() current = ", current, ", tempNext = ", tempNext);
         #endif
         
         fromBranch = current->Remove(cmd);
         
         #ifdef DEBUG_BRANCHCOMMAND_REMOVE
         ShowCommand("BranchCommand::", "Remove() fromBranch = ", fromBranch);
         #endif
         
         if (fromBranch == current)
            branch[which] = tempNext;
         
         if (fromBranch != NULL)
         {
            // set previous command
            #ifdef DEBUG_BRANCHCOMMAND_REMOVE
            ShowCommand("BranchCommand::", " Setting previous of ", tempNext,
                        " to ", fromBranch->GetPrevious());
            #endif
            
            tempNext->ForceSetPrevious(fromBranch->GetPrevious());
            
            #ifdef DEBUG_BRANCHCOMMAND_REMOVE
            MessageInterface::ShowMessage("   Returning fromBranch\n");
            #endif
            
            return fromBranch;
         }
      }
   }
   
   // Not in the branches, so continue with the sequence
   #ifdef DEBUG_BRANCHCOMMAND_REMOVE
   MessageInterface::ShowMessage
      ("   Not in the branches, so continue with the sequence\n");
   #endif
   
   return GmatCommand::Remove(cmd);
} // Remove()

//------------------------------------------------------------------------------
//  void BuildCommandSummaryString(bool commandCompleted)
//------------------------------------------------------------------------------
/**
 * Generates the summary string for a command by building the summary string for
 * this command, and then tacking on the summary for the branch commands.
 *
 *
 * @param <commandCompleted> has the command completed execution
 */
//------------------------------------------------------------------------------
void BranchCommand::BuildCommandSummaryString(bool commandCompleted)
{
   #ifdef DEBUG_BRANCHCOMMAND_SUMMARY
      MessageInterface::ShowMessage("Entering BranchCommand::BuildMissionSummaryString for command %s of type %s\n",
            summaryName.c_str(), typeName.c_str());
   #endif

   GmatCommand::BuildCommandSummaryString(commandCompleted);
   if (summaryForEntireMission)
   {
      std::string branchSummary = commandSummary;
      GmatCommand *current = NULL;

      // Build Command Summary string for all of the branch nodes
      for (Integer which = 0; which < (Integer)branch.size(); ++which)
      {
         current = branch[which];
         while ((current != NULL) && (current != this))
         {
            current->SetupSummary(summaryCoordSysName, summaryForEntireMission, missionPhysicsBasedOnly);
            #ifdef DEBUG_BRANCHCOMMAND_SUMMARY
               MessageInterface::ShowMessage("About to call for MissionSummary for command %s of type %s\n",
                     (current->GetSummaryName()).c_str(), (current->GetTypeName()).c_str());
            #endif
            branchSummary += current->GetStringParameter("Summary");
            current = current->GetNext();
         }
      }

      commandSummary = branchSummary;
   }
}

//------------------------------------------------------------------------------
// bool InsertRightAfter(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Inserts a command into the mission sequence right after this one.
 *
 * @param <cmd>  The command that is inserted.
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool BranchCommand::InsertRightAfter(GmatCommand *cmd)
{
   if (!next) 
   {
      next = cmd;
      return true;
   }
   
   return GmatCommand::Insert(cmd, this);
}

//------------------------------------------------------------------------------
// void BranchCommand::SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer for the child commands.
 *
 * @param <ss>  The SolarSystem instance.
 */
//------------------------------------------------------------------------------
void BranchCommand::SetSolarSystem(SolarSystem *ss)
{
   GmatCommand::SetSolarSystem(ss);
   GmatCommand *current = NULL;
   
   // Set it for all of the branch nodes
   // If we have branches, try to insert there first
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      while ((current != NULL) && (current != this))
      {
         current->SetSolarSystem(ss);;
         current = current->GetNext();
      }
   }
}

//------------------------------------------------------------------------------
// void BranchCommand::SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets the internal coordinate system pointer for the child commands.
 *
 * @param <ss>  The CoordinateSystem instance.
 */
//------------------------------------------------------------------------------
void BranchCommand::SetInternalCoordSystem(CoordinateSystem *cs)
{
   GmatCommand::SetInternalCoordSystem(cs);
   GmatCommand *current = NULL;

   // Set it for all of the branch nodes
   // If we have branches, try to insert there first
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      while ((current != NULL) && (current != this))
      {
         current->SetInternalCoordSystem(cs);;
         current = current->GetNext();
      }
   }
}

//------------------------------------------------------------------------------
//  void SetObjectMap(std::map<std::string, Asset *> *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox or Function to set the local asset store used by the
 * Command.  This implementation chains through the branches and sets the asset
 * map for each of the branch nodes.
 * 
 * @param <map> Pointer to the local asset map
 */
//------------------------------------------------------------------------------
void BranchCommand::SetObjectMap(std::map<std::string, GmatBase *> *map)
{
   GmatCommand::SetObjectMap(map);
   GmatCommand *current = NULL;
   
   // Set it for all of the branch nodes
   // If we have branches, try to insert there first
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      while ((current != NULL) && (current != this))
      {
         current->SetObjectMap(map);
         current = current->GetNext();
      }
   }
}

//------------------------------------------------------------------------------
//  void SetGlobalObjectMap(std::map<std::string, Asset *> *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox or Function to set the global asset store used by the
 * Command.  This implementation chains through the branches and sets the asset
 * map for each of the branch nodes.
 * 
 * @param <map> Pointer to the local asset map
 */
//------------------------------------------------------------------------------
void BranchCommand::SetGlobalObjectMap(std::map<std::string, GmatBase *> *map)
{
   GmatCommand::SetGlobalObjectMap(map);
   GmatCommand *current = NULL;
   
   // Set it for all of the branch nodes
   // If we have branches, try to insert there first
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      while ((current != NULL) && (current != this))
      {
         current->SetGlobalObjectMap(map);
         current = current->GetNext();
      }
   }
}

//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/*
 * Renames referenced objects
 *
 * @param <type> type of the reference object.
 * @param <oldName> old name of the reference object.
 * @param <newName> new name of the reference object.
 *
 * @return always true to indicate RenameRefObject() was implemented.
 */
//---------------------------------------------------------------------------
bool BranchCommand::RenameRefObject(const Gmat::ObjectType type,
                                    const std::string &oldName,
                                    const std::string &newName)
{
   std::vector<GmatCommand*>::iterator node;
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      current = *node;
      if (current != NULL)
      {
         #ifdef DEBUG_RENAME
         ShowCommand("BranchCommand::", "RenameRefObject() current = ", current);
         #endif
         
         current->RenameRefObject(type, oldName, newName);
         
         while (current->GetNext() != this)
         {
            current = current->GetNext();
            if (current == NULL)
               break;
            
            #ifdef DEBUG_RENAME
            ShowCommand("BranchCommand::", "RenameRefObject() current = ", current);
            #endif
            
            current->RenameRefObject(type, oldName, newName);            
         }
      }
      else
         break;
   }
   
   return true;
}

//------------------------------------------------------------------------------
//  const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                         const std::string &prefix,
//                                         const std::string &useName)
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * @param <mode>    Specifies the type of serialization requested (Not yet used
 *                  in commands).
 * @param <prefix>  Optional prefix appended to the object's name (Not yet used
 *                  in commands).
 * @param <useName> Name that replaces the object's name (Not yet used
 *                  in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& BranchCommand::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   fullString = prefix + generatingString;
   InsertCommandName(fullString);
   
   // We don't want BranchCommand to indent
   UnsignedInt prefixSize = prefix.size();   
   if (this->IsOfType("BranchCommand") && prefix != "")
      fullString = fullString.substr(prefixSize);
   
   #ifdef DEBUG_BRANCHCOMMAND_GEN_STRING
   ShowCommand("BranchCommand::", "GetGeneratingString() this = ", this);
   MessageInterface::ShowMessage
      ("   mode='%d', prefix='%s', useName='%s'\n", mode, prefix.c_str(),
       useName.c_str());
   MessageInterface::ShowMessage("   fullString = '%s'\n", fullString.c_str());
   #endif
   
   std::string indent = "   ";
   
   std::string commentLine = GetCommentLine();
   std::string inlineComment = GetInlineComment();
   
   #ifdef DEBUG_BRANCHCOMMAND_GEN_STRING
   ShowCommand("BranchCommand::", "GmatCommand::GetGeneratingString() this = ", this);
   MessageInterface::ShowMessage
      ("===> commentLine=<%s>, inlineComment=<%s>\n",
       commentLine.c_str(), inlineComment.c_str());
   #endif
   
   // Handle multiple line comments, we want to indent all lines.
   if (commentLine != "")
   {
      std::stringstream gen;
      TextParser tp;
      StringArray textArray = tp.DecomposeBlock(commentLine);
      
      // handle multiple comment lines
      for (UnsignedInt i=0; i<textArray.size(); i++)
      {
         gen << prefix << textArray[i];
         if (textArray[i].find("\n") == commentLine.npos &&
             textArray[i].find("\r") == commentLine.npos)
            gen << "\n";
      }
      
      fullString = gen.str() + fullString;
   }
   
   // Handle inline comment
   if (inlineComment != "")
      fullString = fullString + inlineComment;
   
   GmatCommand *current;
   std::string newPrefix = indent + prefix;   
   bool inTextMode = false;
   Integer scriptEventCount = 0;
   
   // Loop through the branches, appending the strings from commands in each
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      
      while ((current != NULL) && (current != this))
      {
         #ifdef DEBUG_BRANCHCOMMAND_GEN_STRING
         ShowCommand("BranchCommand::", "GetGeneratingString() current = ", current);
         MessageInterface::ShowMessage("   inTextMode=%d\n", inTextMode);
         #endif
         
         // BeginScript writes its own children, so write if not in TextMode.
         // EndScript is written from BeginScript
         if (!inTextMode)
         {
            fullString += "\n";
            if (current->GetNext() != this)
               fullString += current->GetGeneratingString(mode, newPrefix, useName);
            else // current is the End command for this branch command
               fullString += current->GetGeneratingString(mode, prefix, useName);
         }
         
         if (current->GetTypeName() == "BeginScript")
            scriptEventCount++;
         
         if (current->GetTypeName() == "EndScript")
            scriptEventCount--;
         
         inTextMode = (scriptEventCount == 0) ? false : true;
         
         current = current->GetNext();
      }
   }
   
   #ifdef DEBUG_BRANCHCOMMAND_GEN_STRING
   MessageInterface::ShowMessage
      ("%s::GetGeneratingString() return fullString = \n<%s>\n",
       this->GetTypeName().c_str(), fullString.c_str());
   #endif
   
   return fullString;
} // GetGeneratingString()

//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <action> The string descriptor for the requested action.
 * @param <actionData> Optional data used for the action.
 *
 * @return true if the action was performed, false if not.
 */
//------------------------------------------------------------------------------
bool BranchCommand::TakeAction(const std::string &action,
                           const std::string &actionData)
{
   #ifdef DEBUG_BRANCHCOMMAND_ACTIONS
      MessageInterface::ShowMessage("%s: Executing the action \"%s\"\n", 
         GetGeneratingString().c_str(), action.c_str());
   #endif
      
   if (action == "ResetLoopData")
   {
      GmatCommand *cmd; 
      for (std::vector<GmatCommand *>::iterator br = branch.begin(); 
           br != branch.end(); ++br)
      {
         cmd = *br;
         while (cmd != this)
         {
            #ifdef DEBUG_BRANCHCOMMAND_ACTIONS
               MessageInterface::ShowMessage("   Applying action to \"%s\"\n", 
                  cmd->GetGeneratingString().c_str());
            #endif
               
            if ((cmd->GetTypeName() == "Propagate") || 
                (cmd->IsOfType("BranchCommand")))
               cmd->TakeAction("ResetLoopData");
            cmd = cmd->GetNext();
         }
      }      
      return true;
   }
   
   return GmatCommand::TakeAction(action, actionData);
}

//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Executes the command.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool BranchCommand::Execute()
{
   commandExecuting = true;
   return true;
}

//------------------------------------------------------------------------------
// bool ExecuteBranch(Integer which)
//------------------------------------------------------------------------------
/**
 * Executes a specific branch.
 *
 * @param <which>  Specifies which branch is executed.
 *
 * @return true if the branch runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool BranchCommand::ExecuteBranch(Integer which)
{
   #ifdef DEBUG_BRANCHCOMMAND_EXECUTION
   MessageInterface::ShowMessage
      ("In BranchCommand (%s) '%s', executing branch %d\n", typeName.c_str(),
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), which);
   #endif
   bool retval = true;
   
   if (current == NULL)
      current = branch[which];
   
   #ifdef DEBUG_BRANCHCOMMAND_EXECUTION
   ShowCommand("In ExecuteBranch:", "current = ", current, "this    = ", this);
   #endif
   
   if (current == this)
   {
      #ifdef DEBUG_BRANCHCOMMAND_EXECUTION
      MessageInterface::ShowMessage
         ("In ExecuteBranch (%s) - current = this -> resetting\n", 
          typeName.c_str());
      if (next)
         MessageInterface::ShowMessage
            ("...... and the next one is : %s\n", (next->GetTypeName()).c_str());
      else
         MessageInterface::ShowMessage("...... and the next one is NULL!!!\n");
      #endif
      
      branchExecuting = false;
      current = NULL;
      lastFired = NULL;
   }
   
   if (current != NULL)
   {
      #ifdef DEBUG_BRANCHCOMMAND_EXECUTION
      ShowCommand("   Calling in ", "current = ", current);
      #endif
      
      try
      {
         // Save current command and set it after current command finished executing
         // in case for calling GmatFunction.
         GmatCommand *curcmd = current;
         if (current->Execute() == false)
            retval = false;
         
         current = curcmd;
         // check for user interruption here
         if (GmatGlobal::Instance()->GetRunInterrupted())
            throw CommandException
               ("Branch command \"" + generatingString + "\" interrupted!");
         
         // Check for NULL pointer here
         if (current == NULL)
         {
            #ifdef __THROW_EXCEPTION__            
            throw CommandException
               ("Branch command \"" + generatingString + "\" has NULL current pointer!");
            #endif
         }
         
         if (current != NULL)
         {
            lastFired = current;
            current = current->GetNext();
         }
         
         branchExecuting = true;
         // Set commandExecuting to true if branch is executing
         commandExecuting = true;
      }
      catch (BaseException &e)
      {
         // Use exception to remove Visual C++ warning
         e.GetMessageType();
         #ifdef DEBUG_BRANCHCOMMAND_EXECUTION
         MessageInterface::ShowMessage
            ("   BranchCommand rethrowing %s\n", e.GetFullMessage().c_str());
         #endif
         
         throw;
      }
   }
   
   #ifdef DEBUG_BRANCHCOMMAND_EXECUTION
   ShowCommand("Exiting ExecuteBranch:", "current = ", current, "this    = ", this);
   MessageInterface::ShowMessage
      ("   branchExecuting=%d, commandExecuting=%d, commandComplete=%d\n",
       branchExecuting, commandExecuting, commandComplete);
   #endif
   
   return retval;
} // ExecuteBranch()

//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
/**
 * Tells the sequence that the run was ended, possibly before reaching the end.
 *
 * BranchCommands clear the "current" pointer and call RunComplete on their
 * branches, ensuring that the command sequence has reset the branches to an
 * idle state.
 */
//------------------------------------------------------------------------------
void BranchCommand::RunComplete()
{
   #ifdef DEBUG_RUN_COMPLETE
   ShowCommand("BranchCommand::", "BranchCommand::RunComplete() this = ", this);
   #endif
   
   current = NULL;
   lastFired = NULL;
   
   for (std::vector <GmatCommand *>::iterator i = branch.begin(); i != branch.end(); ++i)
      if (*i != NULL)
         if (!(*i)->IsOfType("BranchEnd"))
            (*i)->RunComplete();

   commandComplete = false;
   commandExecuting = false;
   branchExecuting = false;

   GmatCommand::RunComplete();
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  bool ShiftBranches(GmatCommand *startWith, Integer ofBranchNumber)
//------------------------------------------------------------------------------
/**
 * Shifts command in the branches down one branch, starting with command 
 * startWith in branch ofBranchNumber.
 *
 * @param startingWith   first command to shift down one branch
 * @param ofBranchNumber branch where the command is located
 *
 * @return success of the operation
 *
 */
//------------------------------------------------------------------------------
bool  
BranchCommand::ShiftBranches(GmatCommand *startWith, Integer ofBranchNumber)
{   
   Integer brSz = (Integer) branch.size();
   // add another branch at the end
   branch.push_back(NULL);
   for (Integer i = brSz-1; i > ofBranchNumber; i--)
   {
      branch.at(i+1) = branch.at(i);
   }
   branch.at(ofBranchNumber+1) = startWith;
   return true;
}

//------------------------------------------------------------------------------
// void SetPreviousCommand(GmatCommand *cmd, GmatCommand *prev,
//                         bool skipBranchEnd)
//------------------------------------------------------------------------------
/*
 * Sets previous command of the command.
 *
 * @param  cmd            The command of which previous command to be set
 * @param  prev           The previous command to set to command
 * @param  skipBranchEnd  If true, it sets without checking for BranchEnd
 *
 */
//------------------------------------------------------------------------------
void BranchCommand::SetPreviousCommand(GmatCommand *cmd, GmatCommand *prev,
                                       bool skipBranchEnd)
{
   #ifdef DEBUG_BRANCHCOMMAND_PREV_CMD
   ShowCommand
      ("BranchCommand::SetPreviousCommand()", " cmd = ", cmd, " prev = ", prev);
   #endif
   
   if (skipBranchEnd && cmd->IsOfType("BranchEnd"))
   {
      #ifdef DEBUG_BRANCHCOMMAND_PREV_CMD
      MessageInterface::ShowMessage
         ("   cmd is of type \"BranchEnd\", so previous is not set\n");
      ShowCommand("   ", " previous of ", cmd, " is ", cmd->GetPrevious());
      #endif
      
      if (cmd->GetPrevious() == NULL)
         cmd->ForceSetPrevious(prev);
   }
   else
   {
      #ifdef DEBUG_BRANCHCOMMAND_PREV_CMD
      ShowCommand("BranchCommand::", " Setting previous of ", cmd, " to ", prev);
      #endif
      
      cmd->ForceSetPrevious(prev);
   }
}

//------------------------------------------------------------------------------
// const std::vector<GmatCommand*> GetCommandsWithGmatFunctions()
//------------------------------------------------------------------------------
/*
 * Gets a list of commands that have Gmat Functions
 *
 * @return  list of Gmat commands that have Gmat Functions
 *
 */
//------------------------------------------------------------------------------
const std::vector<GmatCommand*> BranchCommand::GetCommandsWithGmatFunctions()
{
   #ifdef DEBUG_BRANCHCOMMAND_GMATFUNCTIONS
      MessageInterface::ShowMessage("Entering BranchCommand::GetCommandsWithGmatFunctions\n");
   #endif
   cmdsWithFunctions.clear();
   std::vector<GmatCommand*> tmpArray;
   
   GmatCommand *brCmd, *subCmd;;
   for (unsigned int ii = 0; ii < branch.size(); ii++)
   {
      brCmd = branch.at(ii);
      subCmd = brCmd;
      while ((subCmd != NULL) && (subCmd != this))
      {
         #ifdef DEBUG_BRANCHCOMMAND_GMATFUNCTIONS
            MessageInterface::ShowMessage(
                  "--- checking a Command of type %s with name %s\n",
                  (subCmd->GetTypeName()).c_str(), (subCmd->GetName()).c_str());
         #endif
         tmpArray.clear();
         if(subCmd->IsOfType("BranchCommand"))  
         {
            tmpArray = ((BranchCommand*)subCmd)->GetCommandsWithGmatFunctions();
         }
         else if ((subCmd->IsOfType("CallFunction")) ||
                  (subCmd->IsOfType("Assignment")))
         {
            #ifdef DEBUG_BRANCHCOMMAND_GMATFUNCTIONS
               MessageInterface::ShowMessage(
                     "--- ADDING a Command of type %s with name %s to the list\n",
                     (subCmd->GetTypeName()).c_str(), (subCmd->GetName()).c_str());
            #endif
            tmpArray.push_back(subCmd);
         }
         for (unsigned int jj= 0; jj < tmpArray.size(); jj++)
            cmdsWithFunctions.push_back(tmpArray.at(jj));
         subCmd = subCmd->GetNext();
      }
   }
   return cmdsWithFunctions;
}

//------------------------------------------------------------------------------
// bool HasAFunction()
//------------------------------------------------------------------------------
/*
 * Returns a flag indicating whether or not the command has a function.
 *
 * @return  flag indicating whether or not the command has a function
 *
 */
//------------------------------------------------------------------------------
bool BranchCommand::HasAFunction()
{
   #ifdef DEBUG_IS_FUNCTION
      if (!current)
         MessageInterface::ShowMessage("In HasAFunction and current is NULL\n");
      else
         MessageInterface::ShowMessage("In HasAFunction and current is of type %s\n",
               (current->GetTypeName()).c_str());
   #endif
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *currentPtr;
   
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      currentPtr = *node;
      while (currentPtr != this)
      {
         // if some command in the branch has a function, return true
         if (currentPtr->HasAFunction()) return true;
         currentPtr = currentPtr->GetNext();
         if (currentPtr == NULL)
            throw CommandException("Branch command \"" + generatingString +
                                   "\" was not terminated!");
      }
   }
   // otherwise, there are no GmatFunctions in this Branch Command
   return false;
}

//------------------------------------------------------------------------------
// void SetCallingFunction(FunctionManager *fm)
//------------------------------------------------------------------------------
/*
 * Sets the calling function to the input function.
 *
 * @param  calling function
 *
 */
//------------------------------------------------------------------------------
void BranchCommand::SetCallingFunction(FunctionManager *fm)
{
   
   GmatCommand::SetCallingFunction(fm);
   
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *currentPtr;
   
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      currentPtr = *node;
      while (currentPtr != this)
      {
         currentPtr->SetCallingFunction(fm);
         currentPtr = currentPtr->GetNext();
         if (currentPtr == NULL)
            throw CommandException("Branch command \"" + generatingString +
                                   "\" was not terminated!");
      }
   }
}

//------------------------------------------------------------------------------
// bool IsExecuting()
//------------------------------------------------------------------------------
/**
 * Indicates whether the command is executing or not.
 *
 * @return true if command is executing
 */
//------------------------------------------------------------------------------
bool BranchCommand::IsExecuting()
{
   return branchExecuting;
}

//------------------------------------------------------------------------------
// bool NeedsServerStartup()
//------------------------------------------------------------------------------
/**
 * @see GmatCommand
 * Indicates in the engine needs to start an external process to run the command
 * 
 * @return true if a server needs to start, false otherwise.
 */
//------------------------------------------------------------------------------
bool BranchCommand::NeedsServerStartup()
{
   #ifdef DEBUG_SERVER
   MessageInterface::ShowMessage
      ("BranchCommand::NeedsServerStartup() entered, this=<%p>'%s'\n", this,
       generatingString.c_str());
   #endif
   
   GmatCommand *currentPtr;
   bool needServer = false;
   
   std::vector<GmatCommand*>::iterator node;
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      currentPtr = *node;
      while (currentPtr != this)
      {
         if (currentPtr->NeedsServerStartup())
         {
            needServer = true;
            break;
         }
         
         currentPtr = currentPtr->GetNext();
         if (currentPtr == NULL)
            throw CommandException("Branch command \"" + generatingString +
                                   "\" was not terminated!");
      }
      
      if (needServer)
         break;
   }
   
   #ifdef DEBUG_SERVER
   MessageInterface::ShowMessage
      ("BranchCommand::NeedsServerStartup() returning %d\n", needServer);
   #endif
   
   return needServer;
}


//------------------------------------------------------------------------------
// Integer GetCloneCount()
//------------------------------------------------------------------------------
/**
 * Determines how many clones are available in the branch command
 *
 * @return The number of clones
 */
//------------------------------------------------------------------------------
Integer BranchCommand::GetCloneCount()
{
   cloneCount = 0;

   // Count 'em up from the branch control sequence(s)
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *currentPtr;

   for (node = branch.begin(); node != branch.end(); ++node)
   {
      currentPtr = *node;
      while (currentPtr != this)
      {
         cloneCount += currentPtr->GetCloneCount();
         currentPtr = currentPtr->GetNext();
         if (currentPtr == NULL)
            throw CommandException("Branch command \"" + generatingString +
                                   "\" was not terminated!");
      }
   }

   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("CloneCount for branch command %s = %d\n",
            typeName.c_str(), cloneCount);
   #endif

   return cloneCount;
}

//------------------------------------------------------------------------------
// GmatBase* GetClone(Integer cloneIndex)
//------------------------------------------------------------------------------
/**
 * Retrieves a clone pointer from the branch command
 *
 * @param cloneIndex Index to the desired clone
 *
 * @return The pointer to the clone
 */
//------------------------------------------------------------------------------
GmatBase* BranchCommand::GetClone(Integer cloneIndex)
{
   GmatBase *retptr = NULL;
   Integer currentCount = 0, nodeCount;
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *currentPtr;

   if ((cloneIndex < cloneCount) && (cloneIndex >= 0))
   {
      for (node = branch.begin(); node != branch.end(); ++node)
      {
         currentPtr = *node;
         while (currentPtr != this)
         {
            nodeCount = currentPtr->GetCloneCount();

            if (cloneIndex < currentCount + nodeCount)
            {
               retptr = currentPtr->GetClone(cloneIndex - currentCount);
               break;
            }

            currentCount += nodeCount;
            currentPtr = currentPtr->GetNext();
            if (currentPtr == NULL)
               throw CommandException("Branch command \"" + generatingString +
                                      "\" was not terminated!");
         }
      }
   }

   return retptr;
}

//------------------------------------------------------------------------------
// bool AffectsClones()
//------------------------------------------------------------------------------
/**
 * Identifies commands that update objects that could be cloned in other objects
 *
 * @return true if a potential clone update is possible, false (the default) if
 *         not
 */
//------------------------------------------------------------------------------
bool BranchCommand::AffectsClones()
{
   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("Checking for branch command clone "
            "updates: this = %p, lastFired = %p\n", this, lastFired);
      if ((lastFired != NULL) && (lastFired != this))
         MessageInterface::ShowMessage("Last fired command:\n%s\n",
               lastFired->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif

   if ((lastFired != NULL) && (lastFired != this))
      return lastFired->AffectsClones();
   return false;
}

//------------------------------------------------------------------------------
// GmatBase* GetUpdatedObject()
//------------------------------------------------------------------------------
/**
 * Retrieves object that has updates so it can be passed to owned clones
 *
 * @return The object pointer, or NULL (the default) if there is no such object
 */
//------------------------------------------------------------------------------
GmatBase* BranchCommand::GetUpdatedObject()
{
   if ((lastFired != NULL) && (lastFired != this))
      return lastFired->GetUpdatedObject();
   return NULL;
}


//------------------------------------------------------------------------------
// Integer GetUpdatedObjectParameterIndex()
//------------------------------------------------------------------------------
/**
 * Returns updated parameter index for an updated object that may have clones
 *
 * @return The parameter index, 0r -1 if no index specified
 */
//------------------------------------------------------------------------------
Integer BranchCommand::GetUpdatedObjectParameterIndex()
{
   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("BranchCommand::GetUpdatedObjectParameter"
            "Index(): Got index %d from command type %s\n",
            lastFired->GetUpdatedObjectParameterIndex(),
            lastFired->GetTypeName().c_str());
   #endif

   if ((lastFired != NULL) && (lastFired != this))
      return lastFired->GetUpdatedObjectParameterIndex();
   return -1;
}
