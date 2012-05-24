//$Id: GmatPluginFunctions.hpp 1914 2011-11-16 19:06:27Z djconway@NDC $
//------------------------------------------------------------------------------
//                            GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway (Thinking Systems, Inc.)
// Created: 2011/07/06
//
/**
 * Definition for library code interfaces.
 */
//------------------------------------------------------------------------------
#ifndef GmatPluginFunctions_hpp
#define GmatPluginFunctions_hpp

#include "EventLocatorDefs.hpp"
#include "Factory.hpp"

class MessageReceiver;

extern "C"
{
   Integer    LOCATOR_API GetFactoryCount();
   Factory    LOCATOR_API *GetFactoryPointer(Integer index);
   void       LOCATOR_API SetMessageReceiver(MessageReceiver* mr);
};


#endif /*GmatPluginFunctions_hpp*/
