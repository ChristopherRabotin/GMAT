#ifndef BRANCHCOMMAND_H
#define BRANCHCOMMAND_H

#include "Command.hpp"

class BranchCommand : public Command{
public:
    BranchCommand(const std::string &typeStr);
    virtual ~BranchCommand();
    BranchCommand(const BranchCommand& bc);
    BranchCommand&      operator=(const BranchCommand& bc);
    
    void                AddBranch(Command *cmd);

protected:
    /// The branch managed by this command.
    Command             *branch;
};

#endif // BRANCHCOMMAND_H
