//$Header$
//------------------------------------------------------------------------------
//                               BranchCommand
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
 * Base class implementation for the Command classes that branch (Target, If, 
 * While, etc).
 */
//------------------------------------------------------------------------------


#include "BranchCommand.hpp"
#include "MessageInterface.hpp"
#include <sstream>              // for stringstream

//#define DEBUG_BRANCHCOMMAND_DEALLOCATION
//#define DEBUG_BRANCHCOMMAND_APPEND
//#define DEBUG_BRANCHCOMMAND_INSERT
//#define DEBUG_BRANCHCOMMAND_REMOVE 1
//#define DEBUG_BRANCHCOMMAND_ADD
//#define DEBUG_BRANCHCOMMAND_EXECUTION
//#define DEBUG_BRANCHCOMMAND_GEN_STRING
//#define DEBUG_RUN_COMPLETE 1

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  BranchCommand(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Constructs the BranchCommand command (default constructor).
 *
 * @param <typeStr> Strinf setting the type name of the command.
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
   current              (NULL)
{
   depthChange = 1;
   parameterCount = BranchCommandParamCount;
   
   objectTypeNames.push_back("BranchCommand");
}


//------------------------------------------------------------------------------
// ~BranchCommand(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Destroys the BranchCommand.
 */
//------------------------------------------------------------------------------
BranchCommand::~BranchCommand()
{
   #ifdef DEBUG_BRANCHCOMMAND_DEALLOCATION
      MessageInterface::ShowMessage
         ("In BranchCommand::~BranchCommand() this=%s\n",
          this->GetTypeName().c_str());
   #endif
   std::vector<GmatCommand*>::iterator node;
   GmatCommand* current;

   for (node = branch.begin(); node != branch.end(); ++node)
   {
      // Find the end for each branch and disconnect it from the start
      current = *node;
      if (current != NULL)
      {
         #ifdef DEBUG_BRANCHCOMMAND_DEALLOCATION
            MessageInterface::ShowMessage
               ("   current=%s\n", current->GetTypeName().c_str());
         #endif
            
         while (current->GetNext() != this)
         {
            current = current->GetNext();
            if (current == NULL)
            {
               break;
            }
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
            delete *node;
      }
      else
         break;
   }
   
   #ifdef DEBUG_BRANCHCOMMAND_DEALLOCATION
      MessageInterface::ShowMessage("Finished BranchCommand::~BranchCommand()\n");
   #endif
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
   current           (NULL)
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
      GmatCommand::operator=(bc);

   return *this;
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
   // includng its branches -- has finished executing.
   if ((commandExecuting) && (!commandComplete))
      return this;
   
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
   GmatCommand::Initialize();
   
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *currentPtr;
   bool retval = true;
   
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      currentPtr = *node;
      while (currentPtr != this)
      {
         if (!currentPtr->Initialize())
               retval = false;
         currentPtr = currentPtr->GetNext();
         if (currentPtr == NULL)
            throw CommandException("Branch command \"" + generatingString +
                                   "\" was not terminated!");
      }
   }
//   for (UnsignedInt i = 0; i < current.size(); ++i)
//      current[i] = NULL;
   
   commandComplete  = false;
   commandExecuting = false;
   branchExecuting = false;
   current = NULL;
   
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
      ShowCommand("BranchCommand::", "addging to branch ", cmd);
      #endif
      
      branch.push_back(cmd);

      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", " Setting previous of ", cmd, " to ", this);
      #endif
      
      cmd->ForceSetPrevious(this);
      
      //current.push_back(NULL);
   }
   else if (branch[which] == NULL)
   {
      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", " Setting branch.at(which) to ", cmd);
      #endif
      
      branch.at(which) = cmd;

      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", " Setting previous of ", cmd, " to ", this);
      #endif
      
      cmd->ForceSetPrevious(this);
      
      //current.at(which) = NULL;
   }
   else
   {
      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", " Appending ", cmd, " to ", branch.at(which));
      #endif
      
      (branch.at(which))->Append(cmd);
      
      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", " Setting previous of ",cmd, " to ",
                  branch.at(which));
      #endif
      
      cmd->ForceSetPrevious(branch.at(which));
   }
}

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
      ShowCommand("BranchCommand::", " Adding to branch ", cmd);
      #endif
      
      branch.push_back(cmd);

      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", " Setting previous of ", cmd, " to ", this);
      #endif
      
      cmd->ForceSetPrevious(this);
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
      
      #ifdef DEBUG_BRANCHCOMMAND_ADD
      ShowCommand("BranchCommand::", " Setting previous of ", tmp, " to ", cmd);
      #endif
      
      tmp->ForceSetPrevious(cmd);
   }
}


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
            //if (inNested = currentOne->Insert(cmd, prev))
            //   return true;
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
      
      #ifdef DEBUG_BRANCHCOMMAND_INSERT
      ShowCommand("BranchCommand::", " Setting previous of ", this, " to ", cmd);
      #endif
      
      this->ForceSetPrevious(cmd);
      
      // shift all the later comamnds down one branch
      bool isOK = ShiftBranches(toShift, brNum);
      if (!isOK) 
         MessageInterface::ShowMessage
            ("In BranchCommand::Insert - error adding Else/ElseIf");
   }
   
   if (foundHere || hereOrNested) return true;

   // Otherwise, just call the base class method
   return GmatCommand::Insert(cmd, prev);
}


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
      tempNext = current->GetNext();
      
      #ifdef DEBUG_BRANCHCOMMAND_REMOVE
      ShowCommand("BranchCommand::", "Remove() current = ", current, ", tempNext = ", tempNext);
      #endif
      
      if (current != NULL)
      {
         fromBranch = current->Remove(cmd);
         
         if (fromBranch == current)
            branch[which] = tempNext;
         
         // set previous command
         #ifdef DEBUG_BRANCHCOMMAND_REMOVE
         ShowCommand("BranchCommand::", " Setting previous of ", tempNext,
                     " to ", fromBranch->GetPrevious());
         #endif
         
         if (fromBranch != NULL)
         {
            tempNext->ForceSetPrevious(fromBranch->GetPrevious());
            return fromBranch;
         }
      }
   }
   
   // Not in the branches, so continue with the sequence
   return GmatCommand::Remove(cmd);
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
 * Sets teh solar system pointer for the child commands.
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
//  void SetAssetMap(std::map<std::string, Asset *> *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the local asset store used by the Command.  This
 * implementation chains through the branches and sets the asset map for each of
 * the branch nodes.
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
//  const std::string GetGeneratingString()
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
   
   // We don't want BranchCommand to indent
   UnsignedInt prefixSize = prefix.size();   
   if (this->IsOfType("BranchCommand") && prefix != "")
      fullString = fullString.substr(prefixSize);
//    else if (this->IsOfType("BranchCommand") && prefix == "")
//       fullString = "\n" + fullString;
   
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
   
   // Add end-of-line before beginnig new outer branch command
   if (this->IsOfType("BranchCommand") && prefix == "")
      fullString = "\n" + fullString;
   
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
   
   // Add end-of-line after outer branch command
   if (prefix == "")
      fullString = fullString + "\n";
   
   #ifdef DEBUG_BRANCHCOMMAND_GEN_STRING
   MessageInterface::ShowMessage
      ("%s::GetGeneratingString() return fullString = \n%s\n",
       this->GetTypeName().c_str(), fullString.c_str());
   #endif
   
   return fullString;
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
MessageInterface::ShowMessage("In BranchCommand (%s), executing branch %d\n", 
typeName.c_str(), which);
#endif
   bool retval = true;
      
   if (current == NULL)
      current = branch[which];
      
#ifdef DEBUG_BRANCHCOMMAND_EXECUTION
   if (current != NULL)
   {
      std::string curName = current->GetTypeName();
      MessageInterface::ShowMessage
      ("In ExecuteBranch (%s) - current = %s\n", 
      typeName.c_str(), curName.c_str());
   }
   else
      MessageInterface::ShowMessage
         ("In ExecuteBranch (%s)  - current = NULL\n", typeName.c_str());
#endif
   if (current == this)
   {
#ifdef DEBUG_BRANCHCOMMAND_EXECUTION
      MessageInterface::ShowMessage
      ("In ExecuteBranch (%s) - current = this -> resetting\n", 
      typeName.c_str());
      if (next)
      MessageInterface::ShowMessage("...... and the next one is : %s\n",
      (next->GetTypeName()).c_str());
      else
       MessageInterface::ShowMessage("...... and the next one is NULL!!!\n");
#endif
      branchExecuting = false;
      //commandExecuting = false;  // ***********************
      //commandComplete  = true;   // ***********************
      current = NULL;
   }

   //while ((current != NULL) && (current != this))
   if (current != NULL)
   {
      if (current->Execute() == false)
      {
         retval = false;
//         break;
      }
      // May need to add a test for user interruption here
      current = current->GetNext();
   }
   
   return retval;
}


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
   #if DEBUG_RUN_COMPLETE
   ShowCommand("BranchCommand::", "BranchCommand::RunComplete() this = ", this);
   #endif
   
   current = NULL;
   
   for (std::vector <GmatCommand *>::iterator i = branch.begin(); i != branch.end(); ++i)
      if (*i != NULL)
         if (!(*i)->IsOfType("BranchEnd"))
            (*i)->RunComplete();
   
   
   // This block of code causes unnecesary looping since next is handled in
   // the GmatCommand::RunComplete() (loj: 2/16/07)
   //if (next)
   //   if (!next->IsOfType("BranchEnd"))
   //      next->RunComplete();
   
   
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
// protected methods
//------------------------------------------------------------------------------
// none at this time
