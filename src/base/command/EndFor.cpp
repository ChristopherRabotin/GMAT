//$Header$
//------------------------------------------------------------------------------
//                             EndFor
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
// Modified: W. Shoan 2004.09.13 - Updated for use in Build 3
//
/**
 * Definition for the closing line of a for loop
 */
//------------------------------------------------------------------------------


#include "EndFor.hpp"
#include "BranchCommand.hpp"


EndFor::EndFor(void) :
    GmatCommand         ("EndFor")
{
   depthChange = -1;
}


EndFor::~EndFor(void)
{
}
    

EndFor::EndFor(const EndFor& ef) :
    GmatCommand         (ef)
{
}


EndFor& EndFor::operator=(const EndFor& ef)
{
   if (this == &ef)
     return *this;

   GmatCommand::operator=(ef);
   return *this;
}
    

bool EndFor::Initialize(void)
{
   // Validate that next points to the owning for command
   if (!next)
     throw CommandException("EndFor Command not properly reconnected");

   if (next->GetTypeName() != "For")
     throw CommandException("EndFor Command not connected to For Command");
                          

   return true;    
}


bool EndFor::Execute(void)
{
   return true;
}

bool EndFor::Insert(GmatCommand *cmd, GmatCommand *prev)
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
 * This method returns a clone of the EndFor.
 *
 * @return clone of the EndFor.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EndFor::Clone(void) const
{   
   return (new EndFor(*this));
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
const std::string& EndFor::GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName)
{
   generatingString = prefix + "EndFor;";
   return generatingString;
}


