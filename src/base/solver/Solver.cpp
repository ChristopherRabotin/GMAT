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


Solver::Solver(std::string type, std::string name) :
    GmatBase        (Gmat::SOLVER, type, name),
    currentState    (INITIALIZING)
{
}


Solver::~Solver()
{
}


Solver::Solver(const Solver& sol) :
    GmatBase        (sol),
    currentState    (sol.currentState)
{
}


Solver& Solver::operator=(const Solver& sol)
{
    if (&sol == this)
        return *this;

    return *this;
}


/**
 * The method used to iterate until a solution is found.  Derived classes 
 * use this method to implement their solution technique.
 * 
 * @return solver state at the end of the process.
 */
Solver::SolverState Solver::AdvanceState(void)
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
    

/**
 * State used to finalize initialization.
 */
void Solver::CompleteInitialization(void)
{
    currentState = NOMINAL;
}


void Solver::RunNominal(void)
{
    currentState = (SolverState)(currentState+1);
}


void Solver::RunPerturbation(void)
{
    currentState = (SolverState)(currentState+1);
}


void Solver::RunIteration(void)
{
    currentState = (SolverState)(currentState+1);
}


void Solver::CalculateParameters(void)
{
    currentState = (SolverState)(currentState+1);
}

void Solver::CheckCompletion(void)
{
    currentState = (SolverState)(currentState+1);
}


void Solver::RunComplete(void)
{
    currentState = FINISHED;
}
