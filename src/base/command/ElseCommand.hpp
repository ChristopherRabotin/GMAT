//$Header$
//------------------------------------------------------------------------------
//                              ElseCommand 
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
 * Definition for the ELSE command class
 */
//------------------------------------------------------------------------------


#ifndef ElseCommand_hpp
#define ElseCommand_hpp
 
#include "BranchCommand.hpp"

/**
 * Command that manages processing for entry to the ELSE statement 
 *
 * The IfCommand command manages the ELSE statement.  All targeters implement a 
 * state machine that evaluates the current state of the targeting process, and 
 * provides data to the command sequence about the next step to be taken in the 
 * targeting process.  As far as the Target command is concerned, there are 3 
 * possible steps to take: 
 *
 * 1.  Fire the ELSE Statement to perform a calculation.
 *
 * 2.  Run through the Commands in the for ELSE statement.
 *
 * 3.  On convergence, continue with the command sequence following the 
 *     ELSE statement. 
 *
 */
class ElseCommand : public BranchCommand
{
public:
    ElseCommand(void);
    virtual ~ElseCommand(void);
    
    ElseCommand(const ElseCommand& ec);
    ElseCommand&             operator=(const ElseCommand& ec);
    
    // Inherited methods that need some enhancement from the base class
    virtual bool        Append(GmatCommand *cmd);
    
    // Methods used to run the command
    virtual bool        Initialize(void);
    virtual bool        Execute(void);
    
protected:
    Integer             numberOfConditions; 
    
};
#endif  // ElseCommand_hpp
