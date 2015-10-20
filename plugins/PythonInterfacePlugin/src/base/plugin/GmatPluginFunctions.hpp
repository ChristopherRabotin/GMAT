//$Id$
//------------------------------------------------------------------------------
//                           GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS II .
//
// Author: Farideh Farahnak
// Created: 2015/02/23
//
//------------------------------------------------------------------------------
/**
 * The C functions used to load the PythonInterface plugin
 */
//------------------------------------------------------------------------------

#ifndef GmatPluginFunctions_hpp
#define GmatPluginFunctions_hpp

#include "pythoninterface_defs.hpp"
#include "Factory.hpp"

class MessageReceiver;

extern "C"
{
   Integer    PYTHON_API GetFactoryCount();
   Factory    PYTHON_API *GetFactoryPointer(Integer index);
   void       PYTHON_API SetMessageReceiver(MessageReceiver* mr);
};


#endif /*GmatPluginFunctions_hpp*/
