//$Header$
//------------------------------------------------------------------------------
//                                  Target
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
 * Definition for the Target command class
 */
//------------------------------------------------------------------------------


#ifndef Target_hpp
#define Target_hpp
 

#include "BranchCommand.hpp"
#include "Solver.hpp"


/**
 * Command that manages processing for entry to the targeter loop
 *
 * The Target command manages the targeter loop.  All targeters implement a state
 * machine that evaluates the current state of the targeting process, and provides 
 * data to the command sequence about the next step to be taken in the targeting
 * process.  As far as the Target command is concerned, there are 3 possible steps 
 * to take: 
 *
 * 1.  Fire the Targeter to perform a calculation.
 *
 * 2.  Run through the Commands in the targeter loop.
 *
 * 3.  On convergence, continue with the command sequence following the targeter loop. 
 *
 * 
 *
 */
class Target : public BranchCommand
{
public:
    Target(void);
    virtual ~Target(void);
    
    Target(const Target& t);
    Target&             operator=(const Target& t);
    
    // Inherited methods that need some enhancement from the base class
    virtual bool        Append(Command *cmd);
    
    // Methods used to run the command
    virtual void        InterpretAction(void);
    virtual bool        Initialize(void);
    virtual bool        Execute(void);
    
protected:
    /// The name of the spacecraft that gets maneuvered
    std::string         targeterName;
    /// The targeter instance used to manage the targeter state machine
    Solver              *targeter;
    
    // Parameter IDs 
    /// ID for the burn object
    Integer             targeterNameID;
    
};


#endif  // Target_hpp
