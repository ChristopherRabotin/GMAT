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
    
};
#endif  // Else_hpp
