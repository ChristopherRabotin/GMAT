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


#include "EndForCommand.hpp"


EndForCommand::EndForCommand(void) :
    GmatCommand         ("EndFor")
{}


EndForCommand::~EndForCommand(void)
{}
    

EndForCommand::EndForCommand(const EndForCommand& ef) :
    GmatCommand         (ef)
{}


EndForCommand& EndForCommand::operator=(const EndForCommand& ef)
{
    if (this == &ef)
        return *this;
    
    return *this;
}
    

bool EndForCommand::Initialize(void)
{
    // Validate that next points to the owning for command
    if (!next)
        throw CommandException("EndFor Command not properly reconnected");
    
    if (next->GetTypeName() != "For")
        throw CommandException("EndFor Command not connected to For Command");
                             
    
    return true;    
}


bool EndForCommand::Execute(void)
{
    return true;
}
