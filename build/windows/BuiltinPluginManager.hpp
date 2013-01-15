//$Id$
//------------------------------------------------------------------------------
//                           BuiltinPluginManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2013/01/11
//
/**
* Declares a method to register GMAT built-in plugin factoris used in
* the static link build.
*/
//------------------------------------------------------------------------------
#ifndef BuiltinPluginManager_hpp
#define BuiltinPluginManager_hpp

#include "FactoryManager.hpp"

namespace ForStaticLinkBuild
{
   void RegisterBuiltinPluginFactories(FactoryManager *factoryManager);
}

#endif

