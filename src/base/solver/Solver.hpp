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
 * 
 * 
 */
class Solver : public GmatBase
{
public:
    Solver(std::string type, std::string name);
    virtual ~Solver();
    Solver(const Solver& sol);
    Solver&             operator=(const Solver& sol);
    
    /**
     * The method used to iterate until a solution is found.  Derived classes 
     * use this method to implement their solution technique.
     * 
     * @retval Returns true if the process is complete, false if not.
     */
    virtual bool                AdvanceState(void);
    
protected:    
    /// Enumeration defining the states in the state machine
    enum SolverState {
        NOMINAL = 10001,
        PERTURBING,
        ITERATING,
        CALCULATING,
        CHECKINGRUN,
        FINISHED            // This one should stay at the end of the list.
    };
    
    /// Current state for the state machine
    SolverState                 currentState;
    
    // Methods that correspond to the solver states.  Implement the methods
    // that correspond to the Solver's state machine.  The default 
    // implementation just advances the state to the "next" state in the list. 
    virtual void                RunNominal(void);
    virtual void                RunPerturbation(void);
    virtual void                RunIteration(void);
    virtual void                CalculateParameters(void);
    virtual void                CheckCompletion(void);
    virtual void                RunComplete(void);
};


#endif // Solver_hpp
