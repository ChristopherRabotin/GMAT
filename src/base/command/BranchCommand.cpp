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


BranchCommand::BranchCommand(const std::string &typeStr) :
    Command         (typeStr),
    branch          (1),
    commandComplete (false),
    commandExecuting(false),
    branchToFill    (0)
{
}


BranchCommand::~BranchCommand()
{
    std::vector<Command*>::iterator node;
    for (node = branch.begin(); node != branch.end(); ++node) {
        if (*node)
            delete *node;
    }
}


BranchCommand::BranchCommand(const BranchCommand& bc) :
    Command         (bc),
    branch          (1),
    commandComplete (false),
    commandExecuting(false),
    branchToFill    (0)
{
}


BranchCommand& BranchCommand::operator=(const BranchCommand& bc)
{
    if (this == &bc)
        return *this;
        
    return *this;
}


Command* BranchCommand::GetNext(void)
{
    // Return the next pointer in the command sequence if this command -- 
    // includng its branches -- has finished executing.
    if ((commandExecuting) && (!commandComplete))
        return this;
    
    return next;
}


bool BranchCommand::Initialize(void)
{
    std::vector<Command*>::iterator node;
    Command *current;
    bool retval = true;

    for (node = branch.begin(); node != branch.end(); ++node) {
        current = *node;
        while ((current != NULL) && (current != this)) {
            if (!current->Initialize())
                retval = false;
            current = current->GetNext();
        }
    }
    
    commandExecuting = false;
    return retval;
}


void BranchCommand::AddBranch(Command *cmd, Integer which)
{
    // Increase the size of the vector if it's not big enough
    if (which >= (Integer)branch.capacity())
        branch.reserve(which+1);
    
    if (branch[which] == NULL)
        branch[which] = cmd;
    else
        branch[which]->Append(cmd);
}


bool BranchCommand::Append(Command *cmd)
{
    // If we are still filling in a branch, append on that branch
    if (branchToFill >= 0) {
        AddBranch(cmd, branchToFill);
        return true;
    }
    
    // Otherwise, just call the base class method
    return Command::Append(cmd);
}


bool BranchCommand::Insert(Command *cmd, Command *prev)
{
    Command *current = NULL;
    
    // If we have branches, try to insert there first
    for (Integer which = 0; which < (Integer)branch.size(); ++which) {
        current = branch[which];
        if (current != NULL)
            if (current->Insert(cmd, prev))
                return true;
    }
    
    // Otherwise, just call the base class method
    return Command::Insert(cmd, prev);
}


void BranchCommand::SetSolarSystem(SolarSystem *ss)
{
    Command::SetSolarSystem(ss);
    Command *current = NULL;
    
    // Set it for all of the branch nodes
    // If we have branches, try to insert there first
    for (Integer which = 0; which < (Integer)branch.size(); ++which) {
        current = branch[which];
        while ((current != NULL) && (current != this)) {
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
    Command::SetObjectMap(map);
    Command *current = NULL;
    
    // Set it for all of the branch nodes
    // If we have branches, try to insert there first
    for (Integer which = 0; which < (Integer)branch.size(); ++which) {
        current = branch[which];
        while ((current != NULL) && (current != this)) {
            current->SetObjectMap(map);
            current = current->GetNext();
        }
    }
}


bool BranchCommand::Execute(void)
{
    commandExecuting = true;
    return true;
}


bool BranchCommand::ExecuteBranch(Integer which)
{
    bool retval = true;
    
    Command *current = branch[which];
    while ((current != NULL) && (current != this)) {
        if (current->Execute() == false) {
            retval = false;
            break;
        }
        // May need to add a test for user interruption here
        current = current->GetNext();
    }

    return retval;
}
