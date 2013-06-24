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
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: July 15, 2012
//
/**
 * Definition for library code interfaces.
 */
//------------------------------------------------------------------------------
#ifndef GmatPluginFunctions_hpp
#define GmatPluginFunctions_hpp

#include "DataInterfaceDefs.hpp"
#include "Factory.hpp"

class MessageReceiver;

extern "C"
{
   Integer    DATAINTERFACE_API GetFactoryCount();
   Factory    DATAINTERFACE_API *GetFactoryPointer(Integer index);
   void       DATAINTERFACE_API SetMessageReceiver(MessageReceiver* mr);
};


#endif /*GmatPluginFunctions_hpp*/
