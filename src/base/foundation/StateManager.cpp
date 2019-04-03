//$Id$
//------------------------------------------------------------------------------
//                                  StateManager
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
 * Imnplementation of the StateManager base class.  This is the class for state 
 * managers used in GMAT's propagators and solvers.
 */
//------------------------------------------------------------------------------

#include "StateManager.hpp"
#include "MessageInterface.hpp"
#include "GmatBase.hpp"


//#define DEBUG_STATE_ACCESS


//------------------------------------------------------------------------------
// StateManager(Integer size)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param size The size of the state vector
 */
//------------------------------------------------------------------------------
StateManager::StateManager(Integer size) :
   stateSize      (size),
   state          (size),
   current        (NULL)
{
   objects.clear();
   epochIDs.clear();
   elements.clear();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
StateManager::~StateManager()
{
   // Delete the string arrays in elements
   for (std::map<GmatBase*, StringArray*>::iterator i = elements.begin();
         i != elements.end(); ++i)
      delete i->second;

   for (UnsignedInt i = 0; i < stateMap.size(); ++i)
      delete stateMap[i];
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
StateManager::StateManager(const StateManager& sm) :
   stateSize   (sm.stateSize),
   state       (sm.state),
   objectNames (sm.objectNames),
   current     (NULL),
   stateMap    (sm.stateMap)
{
   objects.clear();
   epochIDs.clear();
   elements.clear();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
StateManager& StateManager::operator=(const StateManager& sm)
{
   if (this != &sm)
   {
      stateSize = sm.stateSize;
      state = sm.state;
      
      // For now, copies start empty.  This may change later.
      objects.clear();
      epochIDs.clear();
      elements.clear();

      current     = NULL;
      objectNames = sm.objectNames;
      stateMap    = sm.stateMap;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// Integer GetCount(Gmat::StateElementId elementType)
//------------------------------------------------------------------------------
/**
 * Returns the number of objects that support the specified type
 * 
 * This default version just returns the total number of unique objects managed
 * by the StateManager
 * 
 * @param elementType ID for the type of state element that is being queried.  
 *                    Gmat::UNKNOWN_STATE, the default, counts the total number
 *                    of objects being managed.
 * 
 * @return The count of the number of objects supporting the type specified
 */
//------------------------------------------------------------------------------
Integer StateManager::GetCount(Gmat::StateElementId elementType)
{
   ObjectArray pObjects;
 
   GetStateObjects(pObjects, Gmat::UNKNOWN_OBJECT);
   Integer count = pObjects.size();

   #ifdef DEBUG_STATE_ACCESS
      MessageInterface::ShowMessage(
            "StateManager::GetCount found %d objects supporting type %d\n",
            count, elementType);
   #endif
      
   return count;
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
bool StateManager::UpdateState() 
{
   return true;
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
GmatState* StateManager::GetState()
{
   return &state;
}


Integer StateManager::GetStateSize()
{
   return state.GetSize();
}


//------------------------------------------------------------------------------
// bool GetStateObjects(ObjectArray& pObjects, UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Fills in the pointers to the objects
 *
 * @param pObjects Array that will receive the object pointers
 * @param type The type of objects requested
 *
 * @return true if at least one object pointer was set
 */
//------------------------------------------------------------------------------
bool StateManager::GetStateObjects(ObjectArray& pObjects, UnsignedInt type)
{
   bool retval = false;
   
   if (type == Gmat::UNKNOWN_OBJECT)
   {
      for (ObjectArray::iterator i = objects.begin(); i != objects.end(); ++i)
      {
         if (find(pObjects.begin(), pObjects.end(), (*i)) == pObjects.end())
         {
            pObjects.push_back(*i);
            retval = true;
         }
      }
   }
   else
   {
      for (ObjectArray::iterator i = objects.begin(); i != objects.end(); ++i)
      {
         if ((*i)->IsOfType(type))
         {
            if (find(pObjects.begin(), pObjects.end(), (*i)) == pObjects.end())
            {
               pObjects.push_back(*i);
               retval = true;
            }
         }
      }
   }
   
   return retval;
}


const StringArray& StateManager::GetObjectList(std::string ofType)
{
   return objectNames;
}


const std::vector<ListItem*>* StateManager::GetStateMap()
{
   return &stateMap;
}

