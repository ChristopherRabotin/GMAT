//$Id$
//------------------------------------------------------------------------------
//                             EndWhile
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

//#define DEBUG_ENDWHILE_EXECUTE

#ifdef DEBUG_ENDWHILE_EXECUTE
   #include "MessageInterface.hpp"
#endif


//------------------------------------------------------------------------------
// EndWhile(void) :
//------------------------------------------------------------------------------
EndWhile::EndWhile(void) :
GmatCommand         ("EndWhile")
{
   objectTypeNames.push_back("BranchEnd");
   depthChange = -1;
}


//------------------------------------------------------------------------------
// ~EndWhile(void)
//------------------------------------------------------------------------------
EndWhile::~EndWhile(void)
{
}


//------------------------------------------------------------------------------
// EndWhile(const EndWhile& ew)
//------------------------------------------------------------------------------
EndWhile::EndWhile(const EndWhile& ew) :
GmatCommand         (ew)
{
}


//------------------------------------------------------------------------------
// EndWhile& operator=(const EndWhile& ew)
//------------------------------------------------------------------------------
EndWhile& EndWhile::operator=(const EndWhile& ew)
{
   if (this == &ew)
      return *this;
   GmatCommand::operator=(ew);

   return *this;
}


//------------------------------------------------------------------------------
// bool Initialize(void)
//------------------------------------------------------------------------------
bool EndWhile::Initialize(void)
{
   GmatCommand::Initialize();
   
   // Validate that next points to the owning for command
   if (!next)
      throw CommandException("EndWhile Command not properly reconnected");

   if (next->GetTypeName() != "While")
      throw CommandException("EndWhile Command not connected to While Command");

   return true;
}


//------------------------------------------------------------------------------
// bool Execute(void)
//------------------------------------------------------------------------------
bool EndWhile::Execute(void)
{
   BuildCommandSummary(true);
   #ifdef DEBUG_ENDWHILE_EXECUTE
      MessageInterface::ShowMessage("In EndWhile::Execute, next is a %s\n",
      (next->GetTypeName()).c_str());
      MessageInterface::ShowMessage("---- that is, in EndWhile %p, object While is %p\n",
      this, next);
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool Insert(GmatCommand *cmd, GmatCommand *prev)
//------------------------------------------------------------------------------
bool EndWhile::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if inserting after End statement for branch command, we want to 
   // insert right after the entire If command
   #ifdef DEBUG_ENDWHILE_EXECUTE
      MessageInterface::ShowMessage(
      "In EndWhile::Insert, calling InsertRightAfter(), next is a %s\n",
      (next->GetTypeName()).c_str());
   #endif
   if (this == prev) return ((BranchCommand*)next)->InsertRightAfter(cmd);
   return false;
}


//---------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type,
//                      const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/*
 * Renames referenced objects
 *
 * @param <type> type of the reference object.
 * @param <oldName> old name of the reference object.
 * @param <newName> new name of the reference object.
 *
 * @return always true to indicate RenameRefObject() was implemented.
 */
//---------------------------------------------------------------------------
bool EndWhile::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   // There are no renamealbe objects
   return true;
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
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}
