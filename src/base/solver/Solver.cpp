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
    currentState    (NOMINAL)
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
 * @retval Returns true if the process is complete, false if not.
 */
bool Solver::AdvanceState(void)
{
    bool retval = false;
    
    switch (currentState) {
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
            retval = true;
            break;
        
        default:
            throw SolverException("Undefined Solver state");
    };
    
    return retval; 
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

