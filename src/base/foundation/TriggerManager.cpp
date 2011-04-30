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


#include "TriggerManager.hpp"

TriggerManager::TriggerManager() :
   triggerType       (Gmat::UNKNOWN_OBJECT),
   triggerTypeString ("Unknown")
{
}

TriggerManager::~TriggerManager()
{
}


TriggerManager::TriggerManager(const TriggerManager& tm) :
   triggerType       (tm.triggerType),
   triggerTypeString (tm.triggerTypeString)
{
}


TriggerManager& TriggerManager::operator=(const TriggerManager& tm)
{
   if (this != &tm)
   {
      triggerType       = tm.triggerType;
      triggerTypeString = tm.triggerTypeString;
   }

   return *this;
}


const Integer TriggerManager::GetTriggerType()
{
   return triggerType;
}


const std::string TriggerManager::GetTriggerTypeString()
{
   return triggerTypeString;
}


void TriggerManager::SetObject(GmatBase *obj)
{
}


void TriggerManager::ClearObject(GmatBase* obj)
{
}
