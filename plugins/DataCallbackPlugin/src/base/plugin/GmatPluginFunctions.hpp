//$Id: GmatPluginFunctions.hpp 9460 2011-04-21 22:03:28Z ravimathur $
//------------------------------------------------------------------------------
//                            GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Ravi Mathur, Emergent Space Technologies, Inc.
// Created: December 16, 2014
//
/**
 * Definition for library code interfaces.
 * 
 * This is prototype code.
 */
//------------------------------------------------------------------------------
#ifndef GmatPluginFunctions_hpp
#define GmatPluginFunctions_hpp

#include "datacallback_defs.hpp"

class Factory;
class MessageReceiver;

extern "C"
{
   Integer    DATACALLBACK_API GetFactoryCount();
   Factory    DATACALLBACK_API *GetFactoryPointer(Integer index);
   void       DATACALLBACK_API SetMessageReceiver(MessageReceiver* mr);

   // Set the user-specified callback function
   int        DATACALLBACK_API SetCallback(const char* subscriberName, void (*CBFcn)(const double*, int, void*), void* userData);

   // Get last message from a CInterface function
   const char DATACALLBACK_API *getLastMessage();
};


#endif /*GmatPluginFunctions_hpp*/
