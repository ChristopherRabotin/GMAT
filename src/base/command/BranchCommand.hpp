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
 * Definition for the Command classes that branch (Target, If, While, etc).
 */
//------------------------------------------------------------------------------


#ifndef BRANCHCOMMAND_H
#define BRANCHCOMMAND_H


#include "Command.hpp"
#include <vector>


class BranchCommand : public Command
{
public:
    BranchCommand(const std::string &typeStr);
    virtual ~BranchCommand();
    BranchCommand(const BranchCommand& bc);
    BranchCommand&          operator=(const BranchCommand& bc);
    
    void                    AddBranch(Command *cmd, Integer which = 0);
    bool                    ExecuteBranch(Integer which = 0);
    
    // Inherited methods that need refinements to handle the branching
    virtual bool            Append(Command *cmd);
    virtual bool            Insert(Command *cmd, Command *prev);
    virtual void            SetSolarSystem(SolarSystem *ss);
    virtual void            SetObjectMap(std::map<std::string, GmatBase*> *map);
    
    virtual Command*        GetNext(void);
    virtual bool            Initialize(void);
    virtual bool            Execute(void);
    
protected:
    /// The managed branch(es).  A vector so Case can use it unchanged in build 3.
    std::vector <Command *> branch;
    /// Flag used to indicate if the command is finished executing
    bool                    commandComplete;
    /// Flag used to indicate a run is being executed
    bool                    commandExecuting;
    /// The branch that is being filled while the command sequence is being built
    Integer                 branchToFill;
};

#endif // BRANCHCOMMAND_H
