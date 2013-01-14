//$Id$
//------------------------------------------------------------------------------
//                          AvroInterfaceFactory
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
//
/**
 * Definition of the factory used to create message writing commands in this
 * plugin
 */
//------------------------------------------------------------------------------


#ifndef SaveCommandFactory_hpp
#define SaveCommandFactory_hpp

#include "SaveCommandDefs.hpp"
#include "Factory.hpp"


/**
 * Factory class that creates one class of your plugin's objects
 *
 * Rename this class from SaveCommandFactory( to something more descriptive of your 
 * plugin object(s).
 *
 * @note: The "SAMPLE_API" descriptor needs to match the definition in your 
 * ...Defs.hpp file
 */
class SAVECOMMAND_API SaveCommandFactory : public Factory
{
public:
   SaveCommandFactory();
   virtual ~SaveCommandFactory();
   SaveCommandFactory(const SaveCommandFactory& elf);
   SaveCommandFactory& operator=(const SaveCommandFactory& elf);

   virtual GmatCommand*     CreateCommand(const std::string &ofType,
                                          const std::string &withName = "");
};

#endif /* SaveCommandFactory_hpp */
