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


#ifndef BranchCommand_hpp
#define BranchCommand_hpp


#include "Command.hpp"
#include <vector>


class GMAT_API BranchCommand : public GmatCommand
{
public:
   BranchCommand(const std::string &typeStr);
   virtual ~BranchCommand();
   BranchCommand(const BranchCommand& bc);
   BranchCommand&          operator=(const BranchCommand& bc);
   
   void                    AddBranch(GmatCommand *cmd, Integer which = 0);
   void                    AddToFrontOfBranch(GmatCommand *cmd,
                                              Integer which = 0);
   bool                    ExecuteBranch(Integer which = 0);
   
   // Inherited methods that need refinements to handle the branching
   virtual bool            Append(GmatCommand *cmd);
   virtual bool            Insert(GmatCommand *cmd, GmatCommand *prev);
   virtual GmatCommand*    Remove(GmatCommand *cmd);
   // Insert into the main sequence, not into a branch
   virtual bool            InsertRightAfter(GmatCommand *cmd);
   
   virtual void            SetSolarSystem(SolarSystem *ss);
   virtual void            SetObjectMap(std::map<std::string, GmatBase*> *map);
   
   virtual const std::string&
                           GetGeneratingString(Gmat::WriteMode mode =
                                               Gmat::SCRIPTING,
                                               const std::string &prefix = "",
                                               const std::string &useName = "");

   virtual GmatCommand*    GetNext();
   virtual GmatCommand*    GetChildCommand(Integer whichOne = 0);
   virtual bool            Initialize();
   virtual bool            Execute();
    
protected:
      
   // no additional parameters to add at this time
   enum
   {
      BranchCommandParamCount = GmatCommandParamCount,
   };
   
   /// The managed branch(es).
   std::vector <GmatCommand *> branch;
   /// Flag used to indicate if the command is finished executing
   bool                    commandComplete;
   /// Flag used to indicate a run is being executed
   bool                    commandExecuting;
   /// The branch that is being filled while the command sequence is being built
   Integer                 branchToFill;
   /// Local container used to return the full sequence from the branches
   std::string             fullString;
   /// Counter to track how deep the nesting is
   Integer                 nestLevel;
};

#endif // BranchCommand_hpp
