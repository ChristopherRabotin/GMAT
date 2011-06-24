//$Id: GmatOdtbxFunctions.hpp 9540 2011-05-18 00:02:57Z djcinsb $
//------------------------------------------------------------------------------
//                            GmatOdtbxFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
// ODTBX: Orbit Determination Toolbox
//
// **Legal**
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

#ifndef GmatPluginFunctions_hpp
#define GmatPluginFunctions_hpp

#include "gmatdefs.hpp"
#include "GmatCFunc_defs.hpp"

class Factory;
class ODEModel;
class PropSetup;
class MessageReceiver;

extern "C"
{
   // If this library grows to include add-on code for GMAT, the GMAT plugin
   // interface needs these functions:
   Integer CINTERFACE_API GetFactoryCount();
   Factory CINTERFACE_API *GetFactoryPointer(Integer index);
   void CINTERFACE_API SetMessageReceiver(MessageReceiver* mr);

   // Load the function defs that are visible to ODTBX.  These are split into a 
   // separate file to simplify function include statements and the generation 
   // of a MATLAB m-file defining the interface.  All of the interface 
   // functions providing external callers access to GMAT are defined in this 
   // header:
   #include "CInterfaceFunctions.hpp"

   // Internal helper functions
   void GetODEModel(GmatCommand *cmd, const char *modelName = "");
   PropSetup *GetFirstPropagator(GmatCommand *cmd);
};

#endif /*GmatPluginFunctions_hpp*/
