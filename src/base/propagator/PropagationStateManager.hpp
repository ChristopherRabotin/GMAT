//$Id$
//------------------------------------------------------------------------------
//                           PropagationStateManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2008/12/15
//
/**
 * Definition of the PropagationStateManager base class.  This is the class for
 * state managers used in GMAT's propagators and solvers.
 */
//------------------------------------------------------------------------------

#ifndef PropagationStateManager_hpp
#define PropagationStateManager_hpp

#include "StateManager.hpp"

/**
 * The state manager used in the propagation subsystem.
 */
class GMAT_API PropagationStateManager : public StateManager
{
public:
	PropagationStateManager(Integer size = 0);
	virtual ~PropagationStateManager();
	PropagationStateManager(const PropagationStateManager& psm);
	PropagationStateManager& operator=(const PropagationStateManager& psm);

   virtual Integer GetCount(Gmat::StateElementId elementType =
                               Gmat::UNKNOWN_STATE);

   virtual bool SetObject(GmatBase* theObject);
   virtual bool SetProperty(std::string propName);
   virtual bool SetProperty(std::string propName, Integer index);
   virtual bool SetProperty(std::string propName, GmatBase *forObject);
   virtual bool BuildState();
   virtual bool MapObjectsToVector();
   virtual bool MapVectorToObjects();
   virtual bool RequiresCompletion();
   virtual bool ObjectEpochsMatch();
   virtual Integer GetCompletionCount();
   virtual Integer GetCompletionIndex(const Integer which);
   virtual Integer GetCompletionSize(const Integer which);
   virtual Integer GetSTMIndex(Integer forParameterID);

protected:
   /// Flag indicating if a member needs updating after superposition occurs
   bool           hasPostSuperpositionMember;
   /// IDs of elements that need post superposition updates
   IntegerArray   completionIndexList;
   /// Sizes of the elements that need post superposition updates
   IntegerArray   completionSizeList;
   /// Mapping of the STM rows: entries are IDs of each row/column of the STM
   IntegerArray   stmRowMap;

   Integer        SortVector();
};

#endif /*PropagationStateManager_hpp*/
