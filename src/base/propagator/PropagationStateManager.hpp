//$Id$
//------------------------------------------------------------------------------
//                           PropagationStateManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

protected:

   bool           hasPostSuperpositionMember;
   IntegerArray   completionIndexList;
   IntegerArray   completionSizeList;

   Integer        SortVector();
};

#endif /*PropagationStateManager_hpp*/
