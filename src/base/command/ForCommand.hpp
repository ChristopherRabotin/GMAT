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
// Author:  Joey Gurgnaus
// Created: 2004/01/22
//
/**
 * Definition for the For command class
 */
//------------------------------------------------------------------------------


#ifndef ForCommand_hpp
#define ForCommand_hpp
 

#include "BranchCommand.hpp"


/**
 * Command that manages processing for entry to the targeter loop
 *
 * The ForCommand command manages the for loop.  All targeters implement a 
 * state machine that evaluates the current state of the targeting process, and 
 * provides data to the command sequence about the next step to be taken in the 
 * targeting process.  As far as the Target command is concerned, there are 3 
 * possible steps to take: 
 *
 * 1.  Fire the Targeter to perform a calculation.
 *
 * 2.  Run through the Commands in the for loop.
 *
 * 3.  On convergence, continue with the command sequence following the 
 *     targeter loop. 
 *
 */
class ForCommand : public BranchCommand
{
public:
    ForCommand(void);
    virtual ~ForCommand(void);
    
    ForCommand(const ForCommand& f);
    ForCommand&             operator=(const ForCommand& f);
    
    // Inherited methods that need some enhancement from the base class
    virtual bool        Append(GmatCommand *cmd);
    
    // Methods used to run the command
    virtual bool        Initialize(void);
    virtual bool        Execute(void);
    
protected:
    std::string         forName;
    
};
#endif  // ForCommand_hpp
