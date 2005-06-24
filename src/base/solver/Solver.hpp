//$Header$
//------------------------------------------------------------------------------
//                                Solver
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/29
//
/**
 * Base class for Targeters, Optimizers, and other parametric scanning tools. 
 */
//------------------------------------------------------------------------------


#ifndef Solver_hpp
#define Solver_hpp


#include "GmatBase.hpp"
#include "SolverException.hpp"


/**
 * @brief Base class for targeters, optimizers, and parameter scanning tools.
 * 
 * The Solver subsystem provides the numerical engines that adjust input 
 * parameters (the "variables") and measure the results of these perturbations.  
 * The system works as a state machine.  The specific path through the state 
 * machine depends on the solver implementation.  This class defines the state 
 * values used, and the core methods that use these states and that report on 
 * the results of the states.
 */
class GMAT_API Solver : public GmatBase
{
public:
   /// Enumeration defining the states in the state machine
   enum SolverState
   {
      INITIALIZING = 10001,
      NOMINAL,
      PERTURBING,
      ITERATING,
      CALCULATING,
      CHECKINGRUN,
      FINISHED            // This one should stay at the end of the list.
   };
    
public:
   Solver(const std::string &type, const std::string &name);
   virtual ~Solver();
   Solver(const Solver& sol);
   Solver&             operator=(const Solver& sol);

   virtual SolverState GetState();
   virtual SolverState AdvanceState();
   virtual bool        UpdateSolverGoal(Integer id, Real newValue);
   
   // Access methods overriden from the base class
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual bool        GetBooleanParameter(const Integer id) const;
   virtual bool        SetBooleanParameter(const Integer id,
                                           const bool value);
                                           
   virtual void        ReportProgress();
    
   //---------------------------------------------------------------------------
   //  bool Initialize()
   //---------------------------------------------------------------------------
   /**
    * Derived classes implement this method to set object pointers and validate
    * internal data structures.
    *
    * @return true on success, false (or throws a SolverException) on failure
    */
   //---------------------------------------------------------------------------
   virtual bool        Initialize() = 0;
    
   //---------------------------------------------------------------------------
   //  Integer SetSolverVariables(Real *data, std::string name)
   //---------------------------------------------------------------------------
   /**
    * Derived classes use this method to pass in parameter data specific to
    * the algorithm implemented.
    *
    * @param <data> An array of data appropriate to the variables used in the
    *               algorithm.
    * @param <name> A label for the data parameter.  Defaults to the empty
    *               string.
    *
    * @return The ID used for the variable.
    */
   //---------------------------------------------------------------------------
   virtual Integer     SetSolverVariables(Real *data,
                                          const std::string &name) = 0;

   //---------------------------------------------------------------------------
   //  Real GetSolverVariable(Integer id)
   //---------------------------------------------------------------------------
   /**
    * Derived classes use this method to retrieve parameter data specific to
    * the algorithm implemented.
    *
    * @param <id> The ID used for the variable.
    *
    * @return The value used for this variable
    */
   //---------------------------------------------------------------------------
   virtual Real        GetSolverVariable(Integer id) = 0;
    
   //---------------------------------------------------------------------------
   //  Integer SetSolverResults(Real *data, std::string name)
   //---------------------------------------------------------------------------
   /**
    * Sets up the data fields used for the results of an iteration.
    *
    * @param <data> An array of data appropriate to the results used in the
    *               algorithm (for instance, tolerances for targeter goals).
    * @param <name> A label for the data parameter.  Defaults to the empty
    *               string.
    *
    * @return The ID used for this variable.
    */
   //---------------------------------------------------------------------------
   virtual Integer     SetSolverResults(Real *data,
                                        const std::string &name) = 0;
    
   //---------------------------------------------------------------------------
   //  void SetResultValue(Integer id, Real value)
   //---------------------------------------------------------------------------
   /**
    * Passes in the results obtained from a run in the solver loop.
    *
    * @param <id>    The ID used for this result.
    * @param <value> The corresponding result.
    */
   //---------------------------------------------------------------------------
   virtual void        SetResultValue(Integer id, Real value) = 0;

protected:
   /// Current state for the state machine
   SolverState         currentState;
   /// Output mode: Compact, Normal, and Verbose
   std::string         textFileMode;
   /// Toggle for showing solver status
   bool                showProgress;

   /// Generic solver parameters.
   enum
   {
      ShowProgressID   = GmatBaseParamCount,
      SolverParamCount
   };
   
   static const std::string    PARAMETER_TEXT[SolverParamCount -
                                              GmatBaseParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[SolverParamCount -
                                              GmatBaseParamCount];


   // Methods that correspond to the solver states.  Derived classes should
   // implement the methods that correspond to the Solver's state machine.  The
   // default implementation just advances the state to the "next" state in the
   // list.
   virtual void        CompleteInitialization();
   virtual void        RunNominal();
   virtual void        RunPerturbation();
   virtual void        RunIteration();
   virtual void        CalculateParameters();
   virtual void        CheckCompletion();
   virtual void        RunComplete();
   
   virtual std::string GetProgressString();
    
   //---------------------------------------------------------------------------
   //  void WriteToTextFile()
   //---------------------------------------------------------------------------
   /**
    * Utility function used by the solvers to generate a progress file.
    */
   //---------------------------------------------------------------------------
   virtual void        WriteToTextFile() = 0;
};


#endif // Solver_hpp
