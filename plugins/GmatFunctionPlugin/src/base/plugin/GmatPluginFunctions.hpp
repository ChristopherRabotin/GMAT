//$Id$
//------------------------------------------------------------------------------
//                            GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, task order 28.
//
// Author: Darrel Conway (Thinking Systems)
// Created: 2012/09/18
//
/**
 * Definition for library code interfaces.
 */
//------------------------------------------------------------------------------

#ifndef GmatPluginFunctions_hpp
#define GmatPluginFunctions_hpp

#include "GmatFunction_defs.hpp"
#include "Factory.hpp"

class MessageReceiver;


/**
 * This code defines the C interfaces GMAT uses to load a plug-in library.
 */
extern "C"
{
   Integer GMATFUNCTION_API GetFactoryCount();
   Factory GMATFUNCTION_API *GetFactoryPointer(Integer index);
   void    GMATFUNCTION_API SetMessageReceiver(MessageReceiver* mr);
};


#endif /*GmatPluginFunctions_hpp*/
