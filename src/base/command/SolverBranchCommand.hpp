//$Id$
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
#include "Solver.hpp"

class SolverBranchCommand : public BranchCommand
{
public:
        SolverBranchCommand(const std::string &typeStr);
        virtual ~SolverBranchCommand();
   SolverBranchCommand(const SolverBranchCommand& sbc);

   SolverBranchCommand&    operator=(const SolverBranchCommand& sbc);
   
   virtual GmatCommand*    GetNext();
   virtual bool            TakeAction(const std::string &action, 
                                      const std::string &actionData = "");
   
   // Handle parsing internally
   virtual bool InterpretAction();
   
   // Parameter access methods
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
   virtual std::string GetStringParameter(const Integer id) const; 
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual const StringArray& 
                       GetStringArrayParameter(const Integer id) const; 
   virtual const StringArray& 
                       GetStringArrayParameter(const std::string &label) const;

   virtual bool        NeedsServerStartup();
   
protected:
   // Mode definitions for the state machine overrrides
   enum solverStartMode
   {
      RUN_INITIAL_GUESS,
      RUN_AND_SOLVE,
      RUN_SOLUTION
   };
   
   // Mode definitions used on convergence
   enum solverExitMode
   {
      DISCARD_AND_CONTINUE,
      SAVE_AND_CONTINUE,
      STOP
   };
   
   std::string         solverName;
   Solver              *theSolver; 
   solverStartMode     startMode;
   solverExitMode      exitMode;
   Solver::SolverState specialState;
   
   /// Modes used in the solver, filled in the derived classes
   StringArray         solverModes;    
   /// Modes used to leave the solver, filled in the derived classes
   StringArray         exitModes;    

   /// Local store of the objects that we'll need to reset
   ObjectArray         localStore;

   /// Parsing function for SolveMode and ExitMode
   void                CheckForOptions(std::string &opts);
   /// Returns the text for options that are not set to the default values
   std::string GetSolverOptionText();
   
   // Methods used to save the starting point for the loops
   virtual void        StoreLoopData();
   virtual void        ResetLoopData();
   virtual void        FreeLoopData();
   
   virtual void        ApplySolution();
   
   enum
   {
      SOLVER_NAME_ID  = BranchCommandParamCount,
      SOLVER_SOLVE_MODE,
      SOLVER_EXIT_MODE,
      SOLVER_SOLVE_MODE_OPTIONS,
      SOLVER_EXIT_MODE_OPTIONS,
      SolverBranchCommandParamCount
   };
   
};

#endif /*SOLVERBRANCHCOMMAND_HPP_*/
