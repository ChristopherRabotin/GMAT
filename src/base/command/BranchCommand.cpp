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

// #define DEBUG_BRANCHCOMMAND_DEALLOCATION
// #define DEBUG_BRANCHCOMMAND_DEALLOCATION


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
   branchToFill         (0),
   nestLevel            (0)
{
   depthChange = 1;
   parameterCount = BranchCommandParamCount;
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
      MessageInterface::ShowMessage("In BranchCommand::~BranchCommand()\n");
   #endif
   std::vector<GmatCommand*>::iterator node;
   GmatCommand* current;
      
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      // Find the end for each branch and disconnect it fron the start
      current = *node;
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
            MessageInterface::ShowMessage("Removing %s\n", 
                                       current->GetTypeName().c_str());
         #endif
         current->Remove(current);  
      }
      
      if (*node)
         delete *node;
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
   branchToFill      (0),
   nestLevel        (bc.nestLevel)
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
   // Increase the size of the vector if it's not big enough
   if (which >= (Integer)branch.capacity())
   {
      branch.reserve(which+1);
//      current.reserve(which+1);
   }
   
   if (which == (Integer)(branch.size()))
   {
      branch.push_back(cmd);
//      current.push_back(NULL);
   }   
   else if (branch[which] == NULL)
   {
      branch.at(which) = cmd;
//      current.at(which) = NULL;
   }
   else
      (branch.at(which))->Append(cmd);
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
   // Increase the size of the vector if it's not big enough
   if (which >= (Integer)branch.capacity())
      branch.reserve(which+1);
   
   if (which == (Integer)(branch.size()))
      branch.push_back(cmd);   
   else if (branch.at(which) == NULL)
      branch.at(which) = cmd;
   else
   {
      GmatCommand* tmp = branch.at(which);
      branch.at(which) = cmd;
      cmd->Append(tmp);
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
   // If we are still filling in a branch, append on that branch
   if (branchToFill >= 0)
   {
      AddBranch(cmd, branchToFill);
      return true;
   }
   
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
   GmatCommand *current = NULL;
   
   // See if we're supposed to put it at the top of the first branch
   if (prev == this)
   {
      
      current = branch[0];
      branch[0] = cmd;
      cmd->Append(current);
      return true;
   }
   // see if we're supposed to add it to the front of a branch
   // i.e. the prev = the last command in the previous branch
   // (e.g. an Else command)
   // check all but the last branch - End*** should take care of it
   // at that point
   for (Integer br = 0; br < (Integer) (branch.size() - 1); ++br)
   {
      current = branch.at(br);
      if (current != NULL)
      {
         while (current->GetNext() != this)
            current = current->GetNext();
         if (current == prev) 
         {
            AddToFrontOfBranch(cmd,br+1);
            return true;
         }
      }
   }
   // If we have branches, try to insert there first
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      if (current != NULL)
         if (current->Insert(cmd, prev))
               return true;
   }
   
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
   if (cmd == this)
      return GmatCommand::Remove(cmd);    // Use base method to remove cmd
   
   GmatCommand *fromBranch = NULL;
   GmatCommand *current = NULL;
   
   // If we have branches, try to remove there first
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      if (current != NULL)
      {
         fromBranch = current->Remove(cmd);
         if (fromBranch != NULL)
               return fromBranch;
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
   GmatCommand *current;

   std::string newPrefix = "   " + prefix;

   // Loop through the branches, appending the strings from commands in each
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      while ((current != NULL) && (current != this))
      {
         fullString += "\n";
         if (current->GetNext() != this)
            fullString += current->GetGeneratingString(mode, newPrefix, useName);
         else // current is the End command for this branch command
            fullString += current->GetGeneratingString(mode, prefix, useName);
         current = current->GetNext();
      }
   }
   
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
   bool retval = true;
   
   if (current == NULL)
      current = branch[which];
      
   if (current == this)
   {
      branchExecuting = false;
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
 * Tells the sewuence that the run was ended, possibly before reaching the end.
 *
 * BranchCommands clear the "current" pointer and call RunComplete on their
 * branches, ensuring that the command sequence has reset the branches to an
 * idle state.
 */
//------------------------------------------------------------------------------
void BranchCommand::RunComplete()
{
   current = NULL;
   for (std::vector <GmatCommand *>::iterator i = branch.begin(); i != branch.end(); ++i)
      if (*i != NULL)
         if (!(*i)->IsOfType("BranchEnd"))
            (*i)->RunComplete();

   if (next)
      if (!next->IsOfType("BranchEnd"))
         next->RunComplete();

   commandComplete = false;
   commandExecuting = false;
   branchExecuting = false;

   GmatCommand::RunComplete();
}
