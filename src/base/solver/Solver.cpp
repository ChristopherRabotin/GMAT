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


#include "Solver.hpp"


//------------------------------------------------------------------------------
//  Solver(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Core constructor for Solver objects.
 * 
 * @param type Text description of the type of solver constructed 
 *             (e.g. "DifferentialCorrector")
 * @param name The solver's name
 */
//------------------------------------------------------------------------------
Solver::Solver(const std::string &type, const std::string &name) :
   GmatBase        (Gmat::SOLVER, type, name),
   currentState    (INITIALIZING),
   textFileMode    ("Normal")
{
}


//------------------------------------------------------------------------------
//  Solver(std::string type, std::string name)
//------------------------------------------------------------------------------
/**
 *  Solver destructor.
 */
//------------------------------------------------------------------------------
Solver::~Solver()
{
}


//------------------------------------------------------------------------------
//  Solver(const Solver &sol)
//------------------------------------------------------------------------------
/**
 * Copy constructor for Solver objects.
 * 
 * @param sol The solver that is copied
 */
//------------------------------------------------------------------------------
Solver::Solver(const Solver &sol) :
    GmatBase        (sol),
    currentState    (sol.currentState),
    textFileMode    (sol.textFileMode)
{
}


//------------------------------------------------------------------------------
//  Solver& operator=(const Solver &sol)
//------------------------------------------------------------------------------
/**
 * Assignment operator for solvers
 * 
 * @return this Solver, set to the same parameters as the input solver.
 */
//------------------------------------------------------------------------------
Solver& Solver::operator=(const Solver &sol)
{
    if (&sol == this)
        return *this;
        
    currentState = sol.currentState;
    textFileMode = sol.textFileMode;

    return *this;
}


//------------------------------------------------------------------------------
//  SolverState GetState()
//------------------------------------------------------------------------------
/**
 * Determine the state-machine state of this instance of the Solver.
 *
 * @return this Solver, set to the same parameters as the input solver.
 */
//------------------------------------------------------------------------------
Solver::SolverState Solver::GetState()
{
   return currentState;
}


//------------------------------------------------------------------------------
//  bool UpdateSolverGoal(Integer id, Real newValue)
//------------------------------------------------------------------------------
/**
 * Updates the targeter goals, for floating end point problems.
 * 
 * This default method just returns false.
 * 
 * @param id Id for the goal that is being reset.
 * @param newValue The new goal value.
 * 
 * @return The ID used for this variable.
 */
//------------------------------------------------------------------------------
bool Solver::UpdateSolverGoal(Integer id, Real newValue)
{
   return false;
}


//------------------------------------------------------------------------------
//  Solver::SolverState AdvanceState()
//------------------------------------------------------------------------------
/**
 * The method used to iterate until a solution is found.  Derived classes 
 * use this method to implement their solution technique.
 * 
 * @return solver state at the end of the process.
 */
//------------------------------------------------------------------------------
Solver::SolverState Solver::AdvanceState()
{
    switch (currentState) {
        case INITIALIZING:
            CompleteInitialization();
            break;
        
        case NOMINAL:
            RunNominal();
            break;
        
        case PERTURBING:
            RunPerturbation();
            break;
        
        case ITERATING:
            RunIteration();
            break;
        
        case CALCULATING:
            CalculateParameters();
            break;
        
        case CHECKINGRUN:
            CheckCompletion();
            break;
        
        case FINISHED:
            RunComplete();
            break;
        
        default:
            throw SolverException("Undefined Solver state");
    };
    
    return currentState; 
}
    

//------------------------------------------------------------------------------
//  void CompleteInitialization()
//------------------------------------------------------------------------------
/**
 * Finalized the initialization process by setting the current state for the
 * state machine to the entry state for the solver.  The default method provided
 * here sets the state to the NOMINAL state.
 */
//------------------------------------------------------------------------------
void Solver::CompleteInitialization()
{
    currentState = NOMINAL;
}


//------------------------------------------------------------------------------
//  void RunNominal()
//------------------------------------------------------------------------------
/**
 * Executes a nominal run and then advances the state machine to the next state.
 * 
 * This default method just advances the state.
 */
//------------------------------------------------------------------------------
void Solver::RunNominal()
{
    currentState = (SolverState)(currentState+1);
}


//------------------------------------------------------------------------------
//  void RunPerturbation()
//------------------------------------------------------------------------------
/**
 * Executes a perturbation run and then advances the state machine to the next 
 * state.
 * 
 * This default method just advances the state.
 */
//------------------------------------------------------------------------------
void Solver::RunPerturbation()
{
    currentState = (SolverState)(currentState+1);
}


//------------------------------------------------------------------------------
//  void RunIteration()
//------------------------------------------------------------------------------
/**
 * Executes an iteration run and then advances the state machine to the next 
 * state.
 * 
 * This default method just advances the state.
 */
//------------------------------------------------------------------------------
void Solver::RunIteration()
{
    currentState = (SolverState)(currentState+1);
}


//------------------------------------------------------------------------------
//  void CalculateParameters()
//------------------------------------------------------------------------------
/**
 * Executes a Calculates parameters needed by the state machine for the next
 * nominal run, and then advances the state machine to the next state.
 * 
 * This default method just advances the state.
 */
//------------------------------------------------------------------------------
void Solver::CalculateParameters()
{
    currentState = (SolverState)(currentState+1);
}


//------------------------------------------------------------------------------
//  void CheckCompletion()
//------------------------------------------------------------------------------
/**
 * Checks to see if the Solver has converged.
 * 
 * This default method just advances the state.
 */
//------------------------------------------------------------------------------
void Solver::CheckCompletion()
{
    currentState = (SolverState)(currentState+1);
}


//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
/**
 * Finalized the data at the end of a run.
 * 
 * This default method just sets the state to FINISHED.
 */
//------------------------------------------------------------------------------
void Solver::RunComplete()
{
    currentState = FINISHED;
}
