//$Id: GmatPluginFunctions.hpp 9461 2012-09-28 22:10:15Z TuanNguyen $
//------------------------------------------------------------------------------
//                            GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Nguyen (NASA/GSFC)
// Created: 2012/09/28
//
/**
 * Definition for library code interfaces.
 * 
 * This is prototype code.
 */
//------------------------------------------------------------------------------

#ifndef GmatPluginFunctions_hpp
#define GmatPluginFunctions_hpp

#include "polyhedrongravitymodel_defs.hpp"
#include "Factory.hpp"

class MessageReceiver;

extern "C"
{
   Integer    POLYHEDRONGRAVITYMODEL_API GetFactoryCount();
   Factory    POLYHEDRONGRAVITYMODEL_API *GetFactoryPointer(Integer index);
   void       POLYHEDRONGRAVITYMODEL_API SetMessageReceiver(MessageReceiver* mr);
};


#endif /*GmatPluginFunctions_hpp*/
