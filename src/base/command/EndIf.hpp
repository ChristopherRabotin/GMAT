//$Header$
//------------------------------------------------------------------------------
//                             EndIf
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus 
// Created: 2004/01/30
//
/**
 * Definition for the closing line of the IF Statement
 */
//------------------------------------------------------------------------------


#ifndef EndIf_hpp
#define EndIf_hpp


#include "Command.hpp"


class EndIf : public GmatCommand
{
public:
    EndIf(void);
    virtual ~EndIf(void);
    
    EndIf(const EndIf& ic);
    EndIf&              operator=(const EndIf& ic);
    
    virtual bool            Initialize(void);
    virtual bool            Execute(void);

    virtual bool            Insert(GmatCommand *cmd, GmatCommand *prev);

       // inherited from GmatBase
    virtual GmatBase* Clone(void) const;

};


#endif // EndIf_hpp
