//$Id: GmatPluginFunctions.hpp,v 1.1 2008/07/03 19:15:33 djc Exp $
//------------------------------------------------------------------------------
//                            GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2008/07/03
//
/**
 * Definition for library code interfaces.
 * 
 * This is sample code demonstrating GMAT's plug-in capabilities.
 */
//------------------------------------------------------------------------------

#ifndef GMATEPHEMPROPPLUGINFUNCTIONS_H_
#define GMATEPHEMPROPPLUGINFUNCTIONS_H_

#include "ephempropagator_defs.hpp"
#include "Factory.hpp"

class MessageReceiver;


/**
 * This code defines the C interfaces GMAT uses to load a plug-in library.
 */
extern "C"
{
   Integer    EPHEM_PROPAGATOR_API GetFactoryCount();
   Factory    EPHEM_PROPAGATOR_API *GetFactoryPointer(Integer index);
   void       EPHEM_PROPAGATOR_API SetMessageReceiver(MessageReceiver* mr);
};


#endif /*GMATEPHEMPROPPLUGINFUNCTIONS_H_*/
