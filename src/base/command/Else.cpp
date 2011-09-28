//$Id$
//------------------------------------------------------------------------------
//                                Else
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
// Created: 2004/01/30
// Modified: 2004/08/10 W. Shoan - made a subclass of GmatCommand; modified
//           code accordingly
//
/**
 * Implementation for the Else command class
 */
//------------------------------------------------------------------------------

#include "Else.hpp"


//------------------------------------------------------------------------------
//  Else()
//------------------------------------------------------------------------------
/**
 * Creates a Else command.  (default constructor)
 */
//------------------------------------------------------------------------------
Else::Else() :
    GmatCommand      ("Else")
{
   objectTypeNames.push_back("Else");
   objectTypeNames.push_back("BranchEnd");
   //depthChange = -1;  // is this correct here?
}


//------------------------------------------------------------------------------
//  ~Else()
//------------------------------------------------------------------------------
/**
 * Destroys the for command.  (destructor)
 */
//------------------------------------------------------------------------------
Else::~Else()
{}

    
//------------------------------------------------------------------------------
//  Else(const Else& ec)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a for command.  (Copy constructor)
 *
 * @param ec Else command to copy to create 'this' one
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Else::Else(const Else& ec) :
    GmatCommand   (ec)
{
}


//------------------------------------------------------------------------------
//  Else& operator=(const Else& ec)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the for command.
 *
 * @param ec Else command whose values to use to assign those of 'this' Else
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Else& Else::operator=(const Else& ec)
{
    if (this == &ec)
        return *this;

    GmatCommand::operator=(ec);
    return *this;
}

//------------------------------------------------------------------------------
//  bool Insert(GmatCommand *cmd, GmatCommand *prev)
//------------------------------------------------------------------------------
/**
 * Inserts the command cmd after the command prev.
 *
 * @param cmd Command to insert.
 *
 * @param prev Command after which to insert the command cmd.
 *
 * @return true if the cmd is inserted, false otherwise.
 */
//------------------------------------------------------------------------------
bool Else::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if we've gotten to this point, we should have inserted it into the front
   // of the next branch

   return false;
}
    
//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the ELSE statement.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Else::Initialize()
{
    bool retval = GmatCommand::Initialize();

    // if specific initialization goes here:
    
    return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Execute the Else statement.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Else::Execute()
{
   BuildCommandSummary(true);
   return true;
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
bool Else::RenameRefObject(const Gmat::ObjectType type,
                           const std::string &oldName,
                           const std::string &newName)
{
   // There are no renamealbe objects
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Else.
 *
 * @return clone of the Else.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Else::Clone() const
{
   return (new Else(*this));
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
const std::string& Else::GetGeneratingString(Gmat::WriteMode mode,
                                             const std::string &prefix,
                                             const std::string &useName)
{
   generatingString = prefix + "Else";
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}
