//$Header$
//------------------------------------------------------------------------------
//                             EndIfCommand
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


#ifndef EndIfCommand_hpp
#define EndIfCommand_hpp


#include "Command.hpp"


class EndIfCommand : public GmatCommand
{
public:
    EndIfCommand(void);
    virtual ~EndIfCommand(void);
    
    EndIfCommand(const EndIfCommand& ic);
    EndIfCommand&              operator=(const EndIfCommand& ic);
    
    virtual bool            Initialize(void);
    virtual bool            Execute(void);
};


#endif // EndIfCommand_hpp
