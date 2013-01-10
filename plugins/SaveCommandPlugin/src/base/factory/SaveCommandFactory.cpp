//$Id: SaveCommandFactory.cpp 36 2012-10-08 23:48:34Z djcinsb $
//------------------------------------------------------------------------------
//                         SaveCommandFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2012 Thinking Systems, Inc.
// Free usage granted to all users; this is shell code.  Adapt and enjoy.  
// Attribution is appreciated.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: September 20, 2012
//
/**
 * Implementation of a factory used in a GMAT plugin
 *
 * Replace code in the indicated places for your Factory
 */
//------------------------------------------------------------------------------


#include "SaveCommandFactory.hpp"
#include "Save.hpp"

//------------------------------------------------------------------------------
// SaveCommandFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * Replace the type Gmat::COMMAND in the base class call with the factory type 
 * you need.
 */
//------------------------------------------------------------------------------
SaveCommandFactory::SaveCommandFactory() :
   Factory           (Gmat::COMMAND)
{
   if (creatables.empty())
   {
      // Replace the SampleClass string here with your class name.  For multiple 
      // classes of the same core type, push back multiple names here
      creatables.push_back("Save");
   }
}


//------------------------------------------------------------------------------
// ~SaveCommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SaveCommandFactory::~SaveCommandFactory()
{
}


//------------------------------------------------------------------------------
// SaveCommandFactory(const SaveCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
SaveCommandFactory::SaveCommandFactory(const SaveCommandFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      // Replace the SampleClass string here with your class name.  For multiple 
      // classes of the same type, push back multiple names here
      creatables.push_back("Save");
   }
}


//------------------------------------------------------------------------------
// SaveCommandFactory& operator=(const SaveCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
SaveCommandFactory& SaveCommandFactory::operator=(
      const SaveCommandFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         // Replace the SampleClass string here with your class name.  For multiple 
         // classes of the same type, push back multiple names here
         creatables.push_back("Save");
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &ofType,
//       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for GMAT commands
 *
 * @param ofType The subtype of the command
 * @param withName The new command's name
 *
 * @return A newly created GmatCommand (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
GmatCommand* SaveCommandFactory::CreateCommand(const std::string &ofType,
                                     const std::string &withName)
{
   // This is how you'd implement creation of a ConsolePrint command 
   if (ofType == "Save")
      return new Save();
   // add more here .......

   return NULL;   // doesn't match any type of Command known by this factory
}
