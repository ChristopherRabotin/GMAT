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
// Created: 2004/01/29
//
/**
 * Definition for the closing line of the ENDIF Statement
 */
//------------------------------------------------------------------------------


#include "EndIf.hpp"
#include "BranchCommand.hpp"


EndIf::EndIf(void) :
    GmatCommand         ("EndIf")
{
   depthChange = -1;
}


EndIf::~EndIf(void)
{}
    

EndIf::EndIf(const EndIf& ic) :
    GmatCommand         (ic)
{}


EndIf& EndIf::operator=(const EndIf& ic)
{
    if (this == &ic)
        return *this;
    GmatCommand::operator=(ic);
    
    return *this;
}
    

bool EndIf::Initialize(void)
{
    // Validate that next points to the owning for command
    if (!next)
        throw CommandException("EndIf Command not properly reconnected");
    
    if (next->GetTypeName() != "If" && next->GetTypeName() != "Else" &&
        next->GetTypeName() != "ElseIf")
        throw CommandException("EndIf Command not connected to IF Command");
                             
    return true;    
}


bool EndIf::Execute(void)
{
    return true;
}

bool EndIf::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if inserting after End statement for branch command, we want to 
   // insert right after the entire If command
   if (this == prev) return ((BranchCommand*)next)->InsertRightAfter(cmd);
   
   return false;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EndIf.
 *
 * @return clone of the EndIf.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EndIf::Clone(void) const
{
   return (new EndIf(*this));
}

