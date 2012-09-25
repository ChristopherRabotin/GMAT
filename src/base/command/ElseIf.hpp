//$Id$
//------------------------------------------------------------------------------
//                              ElseIf 
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

//******************************************************************************
//******************************************************************************
//@todo
// ElseIf does not work yet. (2008.08.29)
// The workaround is to use nested If-Else statements.
// The work that needs to be done concerns the conditions, IIRC - WCS
//******************************************************************************
//******************************************************************************

#ifndef ElseIf_hpp
#define ElseIf_hpp
 
#include "GmatCommand.hpp"

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
class GMAT_API ElseIf : public GmatCommand
{
public:
    ElseIf();
    ElseIf(const ElseIf& ec);
    ElseIf&             operator=(const ElseIf& ec);
    virtual ~ElseIf();
    
    
    // Inherited methods that need some enhancement from the base class
    virtual bool        Insert(GmatCommand *cmd, GmatCommand *prev);
    
    // Methods used to run the command
    virtual bool        Initialize();
    virtual bool        Execute();  

    // inherited from GmatBase
    virtual GmatBase* Clone() const;
    
    DEFAULT_TO_NO_CLONES
    DEFAULT_TO_NO_REFOBJECTS

protected:
    
};
#endif  // ElseIf_hpp
