//$Header$
//------------------------------------------------------------------------------
//                             EndForCommand
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus 
// Created: 2004/01/29
//
/**
 * Definition for the closing line of a for loop
 */
//------------------------------------------------------------------------------


#ifndef EndForCommand_hpp
#define EndForCommand_hpp


#include "Command.hpp"


class EndForCommand : public GmatCommand
{
public:
    EndForCommand(void);
    virtual ~EndForCommand(void);
    
    EndForCommand(const EndForCommand& ef);
    EndForCommand&          operator=(const EndForCommand& ef);
    
    virtual bool            Initialize(void);
    virtual bool            Execute(void);
};


#endif // EndForCommand_hpp
