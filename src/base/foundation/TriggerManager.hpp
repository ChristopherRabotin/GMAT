//$Id$
//------------------------------------------------------------------------------
//                         TriggerManager
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/10
//
/**
 * Interface class defining GMAT's TriggerManager interfaces.
 */
//------------------------------------------------------------------------------


#ifndef TriggerManager_hpp
#define TriggerManager_hpp

#include "gmatdefs.hpp"


/**
 * Interface used to plug-in manager code for the Sandbox
 *
 * This class provides a set of methods that can be used by derived classes to
 * add management functionality to GMAT through plug-in libraries.
 *
 * An example of a TriggerManager is GMAT's event management subsystem, used to
 * find the epochs for events like station rise and set times and shadow entry
 * and exit times.
 */
class GMAT_API TriggerManager
{
public:
   TriggerManager();
   virtual ~TriggerManager();
   TriggerManager(const TriggerManager& tm);
   TriggerManager& operator=(const TriggerManager& tm);

   const Integer GetTriggerType();
   const std::string GetTriggerTypeString();

   virtual TriggerManager* Clone() = 0;
   virtual bool CheckForTrigger() = 0;
   virtual Real LocateTrigger() = 0;
   virtual void SetObject(GmatBase *obj);
   virtual void ClearObject(GmatBase* obj);


protected:
   Integer triggerType;
   std::string triggerTypeString;
};

#endif /* TriggerManager_hpp */
