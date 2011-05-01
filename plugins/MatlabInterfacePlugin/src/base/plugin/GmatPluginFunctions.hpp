//$Id: GmatPluginFunctions.hpp 9460 2011-04-21 22:03:28Z lindajun $
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
// Author: Linda Jun (NASA/GSFC)
// Created: 2010/03/30
//
/**
 * Definition for library code interfaces.
 * 
 * This is prototype code.
 */
//------------------------------------------------------------------------------
#ifndef GmatPluginFunctions_hpp
#define GmatPluginFunctions_hpp

#include "matlabinterface_defs.hpp"
#include "Factory.hpp"

class MessageReceiver;

extern "C"
{
   Integer    MATLAB_API GetFactoryCount();
   Factory    MATLAB_API *GetFactoryPointer(Integer index);
   void       MATLAB_API SetMessageReceiver(MessageReceiver* mr);
};


#endif /*GmatPluginFunctions_hpp*/
