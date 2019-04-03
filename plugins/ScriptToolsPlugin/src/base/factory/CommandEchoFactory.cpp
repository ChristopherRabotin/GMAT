//$Id$
//------------------------------------------------------------------------------
//                         CommandEchoFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2012 Thinking Systems, Inc.
// Free usage granted to all users; this is shell code.  Adapt and enjoy.  
// Attribution is appreciated.
//
// Author: Joshua J. Raymond, Thinking Systems, Inc.
// Created: June 23, 2017
//
/**
* Implementation of a factory used in the GMAT plugin CommandEcho
*/
//------------------------------------------------------------------------------


#include "CommandEchoFactory.hpp"
#include "CommandEcho.hpp"

//------------------------------------------------------------------------------
// CommandEchoFactory()
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
CommandEchoFactory::CommandEchoFactory() :
Factory(Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("CommandEcho");
   }
}


//------------------------------------------------------------------------------
// ~CommandEchoFactory()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
CommandEchoFactory::~CommandEchoFactory()
{
}


//------------------------------------------------------------------------------
// CommandEchoFactory(const CommandEchoFactory& elf)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param elf The factory copied here
*/
//------------------------------------------------------------------------------
CommandEchoFactory::CommandEchoFactory(const CommandEchoFactory& elf) :
Factory(elf)
{
   if (creatables.empty())
   {
      creatables.push_back("CommandEcho");
   }
}


//------------------------------------------------------------------------------
// CommandEchoFactory& operator=(const CommandEchoFactory& elf)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param elf The factory copied to this one
*
* @return this instance, set to match elf
*/
//------------------------------------------------------------------------------
CommandEchoFactory& CommandEchoFactory::operator=(
	const CommandEchoFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         // Replace the SampleClass string here with your class name.  For multiple 
         // classes of the same type, push back multiple names here
         creatables.push_back("CommandEcho");
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
GmatCommand* CommandEchoFactory::CreateCommand(const std::string &ofType,
   const std::string &withName)
{
   if (ofType == "CommandEcho")
      return new CommandEcho();

   return NULL;   // doesn't match any type of Command known by this factory
}
