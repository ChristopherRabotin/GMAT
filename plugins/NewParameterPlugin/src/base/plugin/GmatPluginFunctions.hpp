//$Id: GmatPluginFunctions.hpp 1397 2011-04-21 19:04:45Z ljun@NDC $
//------------------------------------------------------------------------------
//                            GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Darrel Conway
// Created: 2013/05/24
//
/**
 * Definition for library code interfaces.
 * 
 * This is prototype code.
 */
//------------------------------------------------------------------------------

#ifndef GMATPLUGINFUNCTIONS_H_
#define GMATPLUGINFUNCTIONS_H_

#include "newparameter_defs.hpp"
#include "Factory.hpp"

class MessageReceiver;

extern "C"
{
   Integer NEW_PARAMETER_API GetFactoryCount();
   Factory NEW_PARAMETER_API *GetFactoryPointer(Integer index);
   void    NEW_PARAMETER_API SetMessageReceiver(MessageReceiver* mr);
};


#endif /*GMATPLUGINFUNCTIONS_H_*/
