//$Id$
//------------------------------------------------------------------------------
//                             EndFor
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
// Author:  Joey Gurganus
// Created: 2004/01/29
//
/**
 * Definition for the closing line of a for loop
 */
//------------------------------------------------------------------------------
#include "EndFor.hpp"
#include "BranchCommand.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
//none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EndFor()
//------------------------------------------------------------------------------
/*
 * Creates an EndFor object (default constructor)
 */
//---------------------------------------------------------------------------
EndFor::EndFor() :
    GmatCommand         ("EndFor")
{
   objectTypeNames.push_back("BranchEnd");
   depthChange = -1;
}
    
//------------------------------------------------------------------------------
// EndFor(const EndFor& ef) :
//------------------------------------------------------------------------------
/*
 * Creates an EndFor object from the input object (copy constructor)
 *
 * @param ef  the EndFor to copy
 */
//---------------------------------------------------------------------------
EndFor::EndFor(const EndFor& ef) :
    GmatCommand         (ef)
{
}

//------------------------------------------------------------------------------
// EndFor& operator=(const EndFor& ef)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the EndFor command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
EndFor& EndFor::operator=(const EndFor& ef)
{
   if (this == &ef)
     return *this;

   GmatCommand::operator=(ef);
   return *this;
}

//------------------------------------------------------------------------------
// ~EndFor()
//------------------------------------------------------------------------------
/**
 * Destroys this EndFor instance (destructor)
 */
//------------------------------------------------------------------------------
EndFor::~EndFor()
{
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes this EndFor command.
 *
 * @return  success flag
 */
//------------------------------------------------------------------------------
bool EndFor::Initialize()
{
   GmatCommand::Initialize();
   
   // Validate that next points to the owning For command
   if (!next)
     throw CommandException("EndFor Command not properly reconnected");

   if (next->GetTypeName() != "For")
     throw CommandException("EndFor Command not connected to For Command");
                          

   return true;    
}

//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Executes this EndFor command.
 *
 * @return  success flag
 */
//------------------------------------------------------------------------------
bool EndFor::Execute()
{
   BuildCommandSummary(true);
   return true;
}

//------------------------------------------------------------------------------
// bool Insert(GmatCommand *cmd, GmatCommand *prev)
//------------------------------------------------------------------------------
/**
 * Inserts the input command into the command sequences after the specified
 * command.
 *
 * @param   cmd   command to insert
 * @param   prev  command after which to insert the command cmd
 *
 * @return  success flag
 */
//------------------------------------------------------------------------------
bool EndFor::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if inserting after End statement for branch command, we want to 
   // insert right after the entire For command
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
 * @param <type>    type of the reference object.
 * @param <oldName> old name of the reference object.
 * @param <newName> new name of the reference object.
 *
 * @return always true to indicate RenameRefObject() was implemented.
 */
//---------------------------------------------------------------------------
bool EndFor::RenameRefObject(const Gmat::ObjectType type,
                             const std::string &oldName,
                             const std::string &newName)
{
   // There are no renameable objects
   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EndFor.
 *
 * @return clone of the EndFor.
 */
//------------------------------------------------------------------------------
GmatBase* EndFor::Clone() const
{   
   return (new EndFor(*this));
}

//------------------------------------------------------------------------------
//  const std::string GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName)
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
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
