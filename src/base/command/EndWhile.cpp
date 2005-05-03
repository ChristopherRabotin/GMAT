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
   depthChange = -1;
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


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param <mode>    Specifies the type of serialization requested.
 * @param <prefix>  Optional prefix appended to the object's name. (Used for
 *                  indentation)
 * @param <useName> Name that replaces the object's name (Not yet used
 *                  in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& EndWhile::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   generatingString = prefix + "EndWhile;";
   return generatingString;
}
