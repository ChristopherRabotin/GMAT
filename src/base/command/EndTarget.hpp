//$Header$
//------------------------------------------------------------------------------
//                                EndTarget
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/23
//
/**
 * Definition for the closing line of a Targeter loop
 */
//------------------------------------------------------------------------------


#ifndef EndTarget_hpp
#define EndTarget_hpp


#include "Command.hpp"


class EndTarget : public GmatCommand
{
public:
    EndTarget(void);
    virtual ~EndTarget(void);
    
    EndTarget(const EndTarget& et);
    EndTarget&              operator=(const EndTarget& et);
    
    virtual bool            Initialize(void);
    virtual bool            Execute(void);
    
    virtual bool            Insert(GmatCommand *cmd, GmatCommand *prev);

    // inherited from GmatBase
    virtual GmatBase* Clone(void) const;

};


#endif // EndTarget_hpp
