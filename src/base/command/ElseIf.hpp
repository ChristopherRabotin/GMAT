//$Header$
//------------------------------------------------------------------------------
//                              ElseIf 
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Wendy Shoan
// Created: 2004/10/21
//
/**
 * Definition for the ElseIf command class
 */
//------------------------------------------------------------------------------


#ifndef ElseIf_hpp
#define ElseIf_hpp
 
#include "Command.hpp"

/**
 * Command that manages processing for entry to the ElseIf statement 
 *
 * The IfCommand command manages the ElseIf statement.  All targeters implement a 
 * state machine that evaluates the current state of the targeting process, and 
 * provides data to the command sequence about the next step to be taken in the 
 * targeting process.  As far as the Target command is concerned, there are 3 
 * possible steps to take: 
 *
 * 1.  Fire the ElseIf Statement to perform a calculation.
 *
 * 2.  Run through the Commands in the for ElseIf statement.
 *
 * 3.  On convergence, continue with the command sequence following the 
 *     ElseIf statement. 
 *
 */
class ElseIf : public GmatCommand
{
public:
    ElseIf(void);
    virtual ~ElseIf(void);
    
    ElseIf(const ElseIf& ec);
    ElseIf&             operator=(const ElseIf& ec);
    
    // Inherited methods that need some enhancement from the base class
    //virtual bool        Append(GmatCommand *cmd);
    virtual bool        Insert(GmatCommand *cmd, GmatCommand *prev);
    
    // Methods used to run the command
    virtual bool        Initialize(void);
    virtual bool        Execute(void);  // do I even need this?

    // inherited from GmatBase
    virtual GmatBase* Clone(void) const;

    
protected:
    
};
#endif  // ElseIf_hpp
