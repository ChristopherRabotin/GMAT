//$Header$
//------------------------------------------------------------------------------
//                              Else 
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


#ifndef Else_hpp
#define Else_hpp
 
#include "Command.hpp"

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
class Else : public GmatCommand
{
public:
    Else(void);
    virtual ~Else(void);
    
    Else(const Else& ec);
    Else&             operator=(const Else& ec);
    
    // Inherited methods that need some enhancement from the base class
    virtual bool        Append(GmatCommand *cmd);
    
    // Methods used to run the command
    virtual bool        Initialize(void);
    virtual bool        Execute(void);  // do I even need this?

    // inherited from GmatBase
    virtual GmatBase* Clone(void) const;

    
protected:
    //Integer             numberOfConditions; add later for ElseIf
    
};
#endif  // Else_hpp
