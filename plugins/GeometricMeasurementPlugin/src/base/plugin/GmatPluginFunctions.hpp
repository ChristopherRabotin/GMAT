//$Id: GmatPluginFunctions.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/08/03
//
/**
 * Definition for library code interfaces.
 */
//------------------------------------------------------------------------------

#ifndef GmatPluginFunctions_hpp
#define GmatPluginFunctions_hpp

#include "geometricmeasurement_defs.hpp"
#include "Factory.hpp"
#include "TriggerManager.hpp"

class MessageReceiver;

extern "C"
{
   Integer          GEOMETRICMEAS_API GetFactoryCount();
   Factory          GEOMETRICMEAS_API *GetFactoryPointer(Integer index);
   void             GEOMETRICMEAS_API SetMessageReceiver(MessageReceiver* mr);
};


#endif /* GmatPluginFunctions_hpp */


/**
 * \mainpage API for the GMAT Geometric Measurement Plugin
 *
 * \section intro_sec Introduction
 *
 * To be written
 *
 */
