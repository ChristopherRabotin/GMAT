
//$Header$
//------------------------------------------------------------------------------
//                                  ForFor
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


#ifndef For_hpp
#define For_hpp
 

#include "BranchCommand.hpp"


/**
 * Command that manages processing for entry to the targeter loop
 *
 * The For command manages the for loop.  All targeters implement a 
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
class For : public BranchCommand
{
public:
    For(void);
    virtual ~For(void);
    
    For(const For& f);
    For&             operator=(const For& f);
    
    // Inherited methods that need some enhancement from the base class
    virtual bool        Append(GmatCommand *cmd);
    
    // Methods used to run the command
    virtual bool        Initialize(void);
    virtual bool        Execute(void);

    // inherited from GmatBase
    virtual GmatBase* Clone(void) const;

    
protected:
    std::string         forName;
    
};
#endif  // For_hpp
