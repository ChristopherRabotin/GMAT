//$Id: GmatOdtbxFunctions.hpp 9540 2011-05-18 00:02:57Z djcinsb $
//------------------------------------------------------------------------------
//                          CInterfacePluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
// ODTBX: Orbit Determination Toolbox
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
//
// Developed jointly by NASA/GSFC, Emergent Space Technologies, Inc.
// and Thinking Systems, Inc. under the FDSS contract, Task 28
//
// Author: Darrel J. Conway (Thinking Systems)
// Created: 2011/03/22
//
/**
 * Definition for library code interfaces needed by GMAT.
 */
//------------------------------------------------------------------------------

#ifndef CInterfacePluginFunctions_hpp
#define CInterfacePluginFunctions_hpp

#include "gmatdefs.hpp"
#include "GmatCFunc_defs.hpp"

class Factory;
class ODEModel;
class PropSetup;
class MessageReceiver;
class GmatCommand;

extern "C"
{
   // If this library grows to include add-on code for GMAT, the GMAT plugin
   // interface needs these functions:
   Integer CINTERFACE_API GetFactoryCount();
   Factory CINTERFACE_API *GetFactoryPointer(Integer index);
   void CINTERFACE_API SetMessageReceiver(MessageReceiver* mr);

   // Load the function defs that are visible to the client.  These are split
   // into a  separate file to simplify function include statements and the
   // generation of a MATLAB m-file defining the interface.  All of the
   // interface functions providing external callers access to GMAT are defined
   // in this header:
   #include "CInterfaceFunctions.h"

   // Internal helper functions
   int GetODEModel(GmatCommand **cmd, const char *modelName = "");
   PropSetup *GetFirstPropagator(GmatCommand *cmd);
   PropSetup *GetPropagator(GmatCommand **current);
};

#endif /*CInterfacePluginFunctions_hpp*/
