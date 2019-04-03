//$Id$
//------------------------------------------------------------------------------
//                          CommandEchoFactory
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
//
/**
* Definition of the factory used to create message writing commands in
* CommandEcho plugin
*/
//------------------------------------------------------------------------------


#ifndef CommandEchoFactory_hpp
#define CommandEchoFactory_hpp

#include "ScriptToolsDefs.hpp"
#include "Factory.hpp"


/**
* Factory class that creates one class of your plugin's objects
*/
class ScriptTools_API CommandEchoFactory : public Factory
{
public:
   CommandEchoFactory();
   virtual ~CommandEchoFactory();
   CommandEchoFactory(const CommandEchoFactory& elf);
   CommandEchoFactory& operator=(const CommandEchoFactory& elf);

   virtual GmatCommand*     CreateCommand(const std::string &ofType,
      const std::string &withName = "");
};

#endif /* CommandEchoFactory_hpp */