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
#include "BranchCommand.hpp"


//------------------------------------------------------------------------------
//  EndTarget()
//------------------------------------------------------------------------------
/**
 * Creates an EndTarget command.  (default constructor)
 */
//------------------------------------------------------------------------------
EndTarget::EndTarget() :
   GmatCommand         ("EndTarget")
{
   depthChange = -1;
}


//------------------------------------------------------------------------------
//  ~EndTarget()
//------------------------------------------------------------------------------
/**
 * Destroys an EndTarget command.  (destructor)
 */
//------------------------------------------------------------------------------
EndTarget::~EndTarget()
{
}
    

//------------------------------------------------------------------------------
//  EndTarget(const EndTarget& et)
//------------------------------------------------------------------------------
/**
 * Creates an EndTarget command.  (copy constructor)
 *
 * @param et The command that is copied.
 */
//------------------------------------------------------------------------------
EndTarget::EndTarget(const EndTarget& et) :
   GmatCommand         (et)
{
}


//------------------------------------------------------------------------------
//  EndTarget& operator=(const EndTarget& et)
//------------------------------------------------------------------------------
/**
 * Creates an EndTarget command.  (copy constructor)
 *
 * @param et The command that is copied.
 *
 * @return This instance, configured like the input instance.
 */
//------------------------------------------------------------------------------
EndTarget& EndTarget::operator=(const EndTarget& et)
{
   if (this == &et)
      return *this;
    
   return *this;
}
    

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Sets up the EndTarget command.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool EndTarget::Initialize()
{
   // Validate that next points to the owning Target command
   if (!next)
      throw CommandException("EndTarget Command not properly reconnected");
    
   if (next->GetTypeName() != "Target")
      throw CommandException("EndTarget Command not connected to Target Command");
    
   return true;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Runs the EndTarget command.
 *
 * The EndTarget command is basically a no-op command; it just marks the end of
 * the targeting loop.
 *
 * @return true always.
 */
//------------------------------------------------------------------------------
bool EndTarget::Execute()
{
   return true;
}


//------------------------------------------------------------------------------
// bool Insert(GmatCommand *cmd, GmatCommand *prev)
//------------------------------------------------------------------------------
/**
 * Inserts a command into the mission sequence.
 *
 * @param cmd  The command that gets inserted.
 * @param prev The command that will precede the inserted command.
 *
 * @return true.
 */
//------------------------------------------------------------------------------
bool EndTarget::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if inserting after End statement for branch command, we want to 
   // insert right after the entire If command
   if (this == prev)
      return ((BranchCommand*)next)->InsertRightAfter(cmd);
   return false;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EndTarget.
 *
 * @return clone of the EndTarget.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EndTarget::Clone() const
{
   return (new EndTarget(*this));
}

