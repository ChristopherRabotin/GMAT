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
* Implements a method to register GMAT built-in plugin factoris used in
* the static link build.
*/
//------------------------------------------------------------------------------

#ifdef __INCLUDE_BUILTIN_PLUGINS__
#include "BuiltinPluginManager.hpp"
#include "GmatFunctionFactory.hpp"
#include "GmatFunctionCommandFactory.hpp"
#include "FormationFactory.hpp"
#include "StationFactory.hpp"
#include "SaveCommandFactory.hpp"
#include "EphemPropFactory.hpp"
#include "EventLocatorFactory.hpp"

#ifdef __USE_MATLAB__
#include "MatlabInterfaceFactory.hpp"
#include "MatlabFunctionFactory.hpp"
#include "CallMatlabFunctionFactory.hpp"
#include "FminconOptimizerFactory.hpp"
#endif
#endif

//------------------------------------------------------------------------------
// void RegisterBuiltinPluginFactories(FactoryManager *factoryManager)
//------------------------------------------------------------------------------
void ForStaticLinkBuild::RegisterBuiltinPluginFactories(FactoryManager *factoryManager)
{
   #ifdef __INCLUDE_BUILTIN_PLUGINS__
   factoryManager->RegisterFactory(new GmatFunctionFactory());
   factoryManager->RegisterFactory(new GmatFunctionCommandFactory());
   factoryManager->RegisterFactory(new FormationFactory());
   factoryManager->RegisterFactory(new StationFactory());
   factoryManager->RegisterFactory(new SaveCommandFactory());
   factoryManager->RegisterFactory(new EphemPropFactory());
   factoryManager->RegisterFactory(new EventLocatorFactory());
   
   #ifdef __USE_MATLAB__
   factoryManager->RegisterFactory(new MatlabInterfaceFactory());
   factoryManager->RegisterFactory(new MatlabFunctionFactory());
   factoryManager->RegisterFactory(new CallMatlabFunctionFactory());
   factoryManager->RegisterFactory(new FminconOptimizerFactory());
   #endif
   #endif
}

