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


#include "EndTarget.hpp"


EndTarget::EndTarget(void) :
    GmatCommand         ("EndTarget")
{}


EndTarget::~EndTarget(void)
{}
    

EndTarget::EndTarget(const EndTarget& et) :
    GmatCommand         (et)
{}


EndTarget& EndTarget::operator=(const EndTarget& et)
{
    if (this == &et)
        return *this;
    
    return *this;
}
    

bool EndTarget::Initialize(void)
{
    // Validate that next points to the owning Target command
    if (!next)
        throw CommandException("EndTarget Command not properly reconnected");
    
    if (next->GetTypeName() != "Target")
        throw CommandException("EndTarget Command not connected to Target Command");
    
    return true;    
}


bool EndTarget::Execute(void)
{
    return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EndTarget.
 *
 * @return clone of the EndTarget.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EndTarget::Clone(void) const
{
   return (new EndTarget(*this));
}

