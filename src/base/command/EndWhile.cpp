//$Header$
//------------------------------------------------------------------------------
//                             EndWhile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Wendy Shoan/GSFC/MAB
// Created: 2004/08/11
//
/**
 * Definition for the closing line of the While Statement
 */
//------------------------------------------------------------------------------


#include "EndWhile.hpp"
#include "BranchCommand.hpp"


EndWhile::EndWhile(void) :
GmatCommand         ("EndWhile")
{
}


EndWhile::~EndWhile(void)
{
}


EndWhile::EndWhile(const EndWhile& ew) :
GmatCommand         (ew)
{
}


EndWhile& EndWhile::operator=(const EndWhile& ew)
{
   if (this == &ew)
      return *this;
   GmatCommand::operator=(ew);

   return *this;
}


bool EndWhile::Initialize(void)
{
   // Validate that next points to the owning for command
   if (!next)
      throw CommandException("EndWhile Command not properly reconnected");

   if (next->GetTypeName() != "While")
      throw CommandException("EndWhile Command not connected to IF Command");

   return true;
}


bool EndWhile::Execute(void)
{
   return true;
}


bool EndWhile::Insert(GmatCommand *cmd, GmatCommand *prev)
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
 * This method returns a clone of the EndWhile.
 *
 * @return clone of the EndWhile.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EndWhile::Clone(void) const
{
   return (new EndWhile(*this));
}

