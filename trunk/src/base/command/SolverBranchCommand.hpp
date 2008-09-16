//$Header$
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


#ifndef SolverBranchCommand_hpp
#define SolverBranchCommand_hpp

#include "BranchCommand.hpp"

class SolverBranchCommand : public BranchCommand
{
public:
        SolverBranchCommand(const std::string &typeStr);
        virtual ~SolverBranchCommand();
   SolverBranchCommand(const SolverBranchCommand& sbc);

   SolverBranchCommand&    operator=(const SolverBranchCommand& sbc);
   
   virtual GmatCommand*    GetNext();
   
protected:
   /// Local store of the objects that we'll need to reset
   ObjectArray         localStore;

   // Methods used to save the starting point for the loops
   virtual void        StoreLoopData();
   virtual void        ResetLoopData();
   virtual void        FreeLoopData();
   
   enum
   {
      SolverBranchCommandParamCount = BranchCommandParamCount,
   };
   
};

#endif /*SOLVERBRANCHCOMMAND_HPP_*/
