//$Header$
//------------------------------------------------------------------------------
//                                IfCommand 
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurgnaus
// Created: 2004/01/30
//
/**
 * Definition for the IF command class
 */
//------------------------------------------------------------------------------


#ifndef IfCommand_hpp
#define IfCommand_hpp
 
#include "BranchCommand.hpp"

/**
 * Command that manages processing for entry to the IF statement 
 *
 * The IfCommand command manages the for loop.  All targeters implement a 
 * state machine that evaluates the current state of the targeting process, and 
 * provides data to the command sequence about the next step to be taken in the 
 * targeting process.  As far as the Target command is concerned, there are 3 
 * possible steps to take: 
 *
 * 1.  Fire the IF Statement to perform a calculation.
 *
 * 2.  Run through the Commands in the for IF statement.
 *
 * 3.  On convergence, continue with the command sequence following the 
 *     IF statement. 
 *
 */
class IfCommand : public BranchCommand
{
public:
    IfCommand(void);
    virtual ~IfCommand(void);
    
    IfCommand(const IfCommand& ic);
    IfCommand&             operator=(const IfCommand& ic);
    
    // Inherited methods that need some enhancement from the base class
    virtual bool        Append(Command *cmd);
    
    // Methods used to run the command
    virtual bool        Initialize(void);
    virtual bool        Execute(void);
    
protected:
    /// The name of the spacecraft that gets maneuvered
    std::string         ifName;
    Integer             numberOfConditions; 
    
};
#endif  // IfCommand_hpp
