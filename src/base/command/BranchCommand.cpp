#include "BranchCommand.hpp"

BranchCommand::BranchCommand(const std::string &typeStr) :
    Command         (typeStr),
    branch          (NULL)
{
}


BranchCommand::~BranchCommand()
{
}


BranchCommand::BranchCommand(const BranchCommand& bc) :
    Command         (bc),
    branch          (NULL)
{
}


BranchCommand& BranchCommand::operator=(const BranchCommand& bc)
{
    if (this == &bc)
        return *this;
        
    return *this;
}


void BranchCommand::AddBranch(Command *cmd)
{
    if (branch == NULL)
        branch = cmd;
    else
        branch->Append(cmd);
}
