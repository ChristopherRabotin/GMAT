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
// Created: 2004/01/29
//
/**
 * Definition for the closing line of the ENDIF Statement
 */
//------------------------------------------------------------------------------


#include "EndIfCommand.hpp"


EndIfCommand::EndIfCommand(void) :
    GmatCommand         ("EndIf")
{}


EndIfCommand::~EndIfCommand(void)
{}
    

EndIfCommand::EndIfCommand(const EndIfCommand& ic) :
    GmatCommand         (ic)
{}


EndIfCommand& EndIfCommand::operator=(const EndIfCommand& ic)
{
    if (this == &ic)
        return *this;
    
    return *this;
}
    

bool EndIfCommand::Initialize(void)
{
    // Validate that next points to the owning for command
    if (!next)
        throw CommandException("EndIf Command not properly reconnected");
    
    if (next->GetTypeName() != "If" && next->GetTypeName() != "Else")
        throw CommandException("EndIf Command not connected to IF Command");
                             
    return true;    
}


bool EndIfCommand::Execute(void)
{
    return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EndIfCommand.
 *
 * @return clone of the EndIfCommand.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EndIfCommand::Clone(void) const
{
   return (new EndIfCommand(*this));
}

