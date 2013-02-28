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
 * Implementation of the PropagationStateManager base class.  This is the class
 * for state managers used in GMAT's propagation subsystem.
 */
//------------------------------------------------------------------------------

#include "PropagationStateManager.hpp"
#include "GmatBase.hpp"

#include "MessageInterface.hpp"
#include "PropagatorException.hpp"

#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "RealUtilities.hpp"       // for IsNaN() and IsInf()
#include <sstream>                 // for <<

//#define DEBUG_STATE_CONSTRUCTION
//#define DUMP_STATE
//#define DEBUG_OBJECT_UPDATES

// When Spacecraft epochs are this close, the PSM will call them identical:
#define IDENTICAL_TIME_TOLERANCE  5.0e-11


//------------------------------------------------------------------------------
// PropagationStateManager(Integer size)
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
PropagationStateManager::PropagationStateManager(Integer size) :
   StateManager                  (size),
   hasPostSuperpositionMember    (false)
{
}


//------------------------------------------------------------------------------
// ~PropagationStateManager()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PropagationStateManager::~PropagationStateManager()
{
//   for (StringArray::iterator i = elements.begin(); i != elements.end(); ++i)
//      delete (i.second());
}


//------------------------------------------------------------------------------
// PropagationStateManager(const PropagationStateManager& psm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param psm The state manager that is copied to the new one
 */
//------------------------------------------------------------------------------
PropagationStateManager::
         PropagationStateManager(const PropagationStateManager& psm) :
   StateManager                  (psm),
   hasPostSuperpositionMember    (psm.hasPostSuperpositionMember)
{
}


//------------------------------------------------------------------------------
// PropagationStateManager& operator=(const PropagationStateManager& psm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param psm The state manager that is copied to this one
 *
 * @return This PSM configured to match psm
 */
//------------------------------------------------------------------------------
PropagationStateManager& 
         PropagationStateManager::operator=(const PropagationStateManager& psm)
{
   if (this != &psm)
   {
      StateManager::operator=(psm);

      hasPostSuperpositionMember = psm.hasPostSuperpositionMember;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// Integer GetCount(Integer elementType)
//------------------------------------------------------------------------------
/**
 * Returns the number of objects that support the specified type
 * 
 * This default version just returns the total number of unique objects managed
 * by the StateManager
 * 
 * @param elementType ID for the type of state element that is being queried.  
 * 
 * @return The count of the number of objects supporting the type specified
 */
//------------------------------------------------------------------------------
Integer PropagationStateManager::GetCount(Gmat::StateElementId elementType)
{
   if (elementType == Gmat::UNKNOWN_STATE)
      return StateManager::GetCount(elementType);
   
   Integer count = 0;
   GmatBase *obj = NULL;
   
   for (Integer index = 0; index < stateSize; ++index)
   {
      if (stateMap[index]->elementID == elementType)
      {
         if (stateMap[index]->object != obj)
         {
            obj = stateMap[index]->object;
            ++count;
         }
      }
   }
   
   #ifdef DEBUG_STATE_ACCESS
      MessageInterface::ShowMessage("PropagationStateManager::GetCount found "
            "%d objects supporting type %d\n", count, elementType);
   #endif
      
   return count;
}


//------------------------------------------------------------------------------
// bool SetObject(GmatBase* theObject)
//------------------------------------------------------------------------------
/**
 * Adds an object to the prop state manager
 *
 * @param theObject The reference to the object
 *
 * @return true on success, false on failure or if the object is already in the
 *         list
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::SetObject(GmatBase* theObject)
{
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Setting object %s\n", 
            theObject->GetName().c_str());
   #endif
      
   // Be sure object is not already in the list 
   if (find(objects.begin(), objects.end(), theObject) != objects.end())
      return false;     // Could throw here, but that would stop everything

   // todo: validate that theObject can be propagated 

   objects.push_back(theObject);
   if (theObject->IsOfType(Gmat::FORMATION))
   {
      Integer id = theObject->GetParameterID("A1Epoch");
      epochIDs.push_back(id);
   }
   else
   {
      Integer id = theObject->GetParameterID("Epoch");
      if (theObject->GetParameterType(id) != Gmat::REAL_TYPE)
         id = theObject->GetParameterID("A1Epoch");
      epochIDs.push_back(id);
   }
   
   current = theObject;
   StringArray *objectProps = new StringArray;
   elements[current] = objectProps;
   *objectProps = current->GetDefaultPropItems();
   
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Object set; current points to %s\n", 
         current->GetName().c_str());
      MessageInterface::ShowMessage("Managing %d objects:\n", objects.size());
      for (UnsignedInt i = 0; i < objects.size(); ++i)
         MessageInterface::ShowMessage("   %2d: %s with %d prop items\n", i,
               objects[i]->GetName().c_str(),
               objects[i]->GetDefaultPropItems().size());
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool SetProperty(std::string propName)
//------------------------------------------------------------------------------
/**
 * Identifies a propagation property for the current object
 *
 * @param propName The name of the property
 *
 * @return true if the property was saved for the current object; false if not
 *         (or if there is no current object)
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::SetProperty(std::string propName)
{
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Entered SetProperty(%s); current = %ld\n",
            propName.c_str(), current);
   #endif
   if (current) 
   {
      // Validate that the property can be propagated
      if (current->SetPropItem(propName) == Gmat::UNKNOWN_STATE)
         throw PropagatorException(propName 
               + " is not a known propagation parameter on " 
               + current->GetName());
      
      // Only add it if it is not yet there
      if (find(elements[current]->begin(), elements[current]->end(),
            propName) == elements[current]->end())
         elements[current]->push_back(propName);

      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("Current property List:\n");
            for (StringArray::iterator i = elements[current]->begin(); 
                  i != elements[current]->end(); ++i)
               MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif
            
      return true;
   }
   
   return false;   
}


//------------------------------------------------------------------------------
// bool SetProperty(std::string propName, Integer index)
//------------------------------------------------------------------------------
/**
 * Identifies a propagation property for an object referenced by index
 *
 * @param propName The name of the property
 * @param index The index of the object that has the property
 *
 * @return true if the property was saved for the current object; false if not
 *         (or if there is no current object)
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::SetProperty(std::string propName, Integer index)
{
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Entered SetProperty(%s, %d)\n",
            propName.c_str(), index);
   #endif

   if ((index < 0) || (index >= (Integer)objects.size()))
      throw PropagatorException("Index out of bounds specifying a prop object "
            "in a propagation state manager\n");

   GmatBase *obj = objects[index];

   if (obj)
   {
      // Validate that the property can be propagated
      if (obj->SetPropItem(propName) == Gmat::UNKNOWN_STATE)
         throw PropagatorException(propName
               + " is not a known propagation parameter on "
               + obj->GetName());
      if (find(elements[obj]->begin(), elements[obj]->end(), propName) ==
            elements[obj]->end())
         elements[obj]->push_back(propName);

      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("Current property List:\n");
            for (StringArray::iterator i = elements[obj]->begin();
                  i != elements[obj]->end(); ++i)
               MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif

      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
// bool SetProperty(std::string propName, GmatBase *forObject)
//------------------------------------------------------------------------------
/**
 * Adds a propagation parameter associated with an object to the state
 * definition
 *
 * @param propName The name of the parameter
 * @param forObject The associated object
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::SetProperty(std::string propName,
      GmatBase *forObject)
{
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Entered SetProperty(%s, %s)\n",
            propName.c_str(), forObject->GetName().c_str());
   #endif

   if (find(objects.begin(), objects.end(), forObject) == objects.end())
      throw PropagatorException("Prop object " + forObject->GetName() +
            " not found in a propagation state manager\n");

   if (forObject)
   {
      // Validate that the property can be propagated
      if (forObject->SetPropItem(propName) == Gmat::UNKNOWN_STATE)
         throw PropagatorException(propName
               + " is not a known propagation parameter on "
               + forObject->GetName());
      if (find(elements[forObject]->begin(), elements[forObject]->end(),
            propName) == elements[forObject]->end())
         elements[forObject]->push_back(propName);

      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("Current property List:\n");
            for (StringArray::iterator i = elements[forObject]->begin();
                  i != elements[forObject]->end(); ++i)
               MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif

      return true;
   }

   return false;
}



//------------------------------------------------------------------------------
// bool BuildState()
//------------------------------------------------------------------------------
/**
 * Collects the data needed and fills in state data
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::BuildState()
{
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Entered BuildState()\n");

      MessageInterface::ShowMessage("StateMap:\n");
      for (Integer index = 0; index < stateSize; ++index)
      {
         MessageInterface::ShowMessage("   %s.%s",
               stateMap[index]->objectName.c_str(),
               stateMap[index]->elementName.c_str());
      }
   #endif
   
   // Determine the size of the propagation state vector
   stateSize = SortVector();
   
   std::map<std::string,Integer> associateMap;
   // Build the associate map
   std::string name;
   for (Integer index = 0; index < stateSize; ++index)
   {
      name = stateMap[index]->objectName;
      if (associateMap.find(name) == associateMap.end())
         associateMap[name] = index;
   }   
   
   state.SetSize(stateSize);
   for (Integer index = 0; index < stateSize; ++index)
   {
      name = stateMap[index]->objectName;
      std::stringstream sel("");
      sel << stateMap[index]->subelement;
      state.SetElementProperties(index, stateMap[index]->elementID, 
            name + "." + stateMap[index]->elementName + "." + sel.str(), 
            associateMap[stateMap[index]->associateName]);
   }
   
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage(
            "Propagation state vector has %d elements:\n", stateSize);
      StringArray props = state.GetElementDescriptions();
      for (Integer index = 0; index < stateSize; ++index)
         MessageInterface::ShowMessage("   %d:  %s  --> associate: %d\n", index,
               props[index].c_str(), state.GetAssociateIndex(index));
   #endif
   
   #ifdef DUMP_STATE
      MapObjectsToVector();
      for (Integer i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage("State[%02d] = %.12lf, %s %d\n", i, state[i], 
               "RefState start =", state.GetAssociateIndex(i));
   #endif   
      
   return true;
}


//------------------------------------------------------------------------------
// bool MapObjectsToVector()
//------------------------------------------------------------------------------
/**
 * Retrieves data from the objects that are to be propagated, and sets those
 * data in the propagation state vector
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::MapObjectsToVector()
{
   #ifdef DEBUG_OBJECT_UPDATES
      MessageInterface::ShowMessage("Mapping objects to vector\n");

      // Look at state data before
      MessageInterface::ShowMessage("O -> V: Real object data before: [");
      for (Integer index = 0; index < stateSize; ++index)
         if (stateMap[index]->parameterType ==  Gmat::REAL_TYPE)
            MessageInterface::ShowMessage(" %lf ",
                  (stateMap[index]->object)->GetRealParameter(
                        stateMap[index]->parameterID));
      MessageInterface::ShowMessage("]\n");
   #endif

   Real value;

   for (Integer index = 0; index < stateSize; ++index)
   {
      switch (stateMap[index]->parameterType)
      {
         case Gmat::REAL_TYPE:
            value = stateMap[index]->object->GetRealParameter(
                          stateMap[index]->parameterID);

            if (GmatMathUtil::IsNaN(value))
               throw PropagatorException("Value for parameter " +
                     stateMap[index]->object->GetParameterText(
                     stateMap[index]->parameterID) + " on object " +
                     stateMap[index]->object->GetName() +
                     " is not a number");

            if (GmatMathUtil::IsInf(value))
               throw PropagatorException("Value for parameter " +
                     stateMap[index]->object->GetParameterText(
                     stateMap[index]->parameterID) + " on object " +
                     stateMap[index]->object->GetName() +
                     " is infinite");

            state[index] = value;
            break;
            
         case Gmat::RVECTOR_TYPE:
            value = stateMap[index]->object->GetRealParameter(
                  stateMap[index]->parameterID, stateMap[index]->rowIndex);

            if (GmatMathUtil::IsNaN(value))
               throw PropagatorException("Value for array parameter " +
                     stateMap[index]->object->GetParameterText(
                     stateMap[index]->parameterID) + " on object " +
                     stateMap[index]->object->GetName() +
                     " is not a number");

            if (GmatMathUtil::IsInf(value))
               throw PropagatorException("Value for array parameter " +
                     stateMap[index]->object->GetParameterText(
                     stateMap[index]->parameterID) + " on object " +
                     stateMap[index]->object->GetName() +
                     " is infinite");

            state[index] = value;
            break;

         case Gmat::RMATRIX_TYPE:
            value = stateMap[index]->object->GetRealParameter(
                  stateMap[index]->parameterID, stateMap[index]->rowIndex,
                  stateMap[index]->colIndex);

            if (GmatMathUtil::IsNaN(value))
               throw PropagatorException("Value for array parameter " +
                     stateMap[index]->object->GetParameterText(
                     stateMap[index]->parameterID) + " on object " +
                     stateMap[index]->object->GetName() +
                     " is not a number");

            if (GmatMathUtil::IsInf(value))
               throw PropagatorException("Value for array parameter " +
                     stateMap[index]->object->GetParameterText(
                     stateMap[index]->parameterID) + " on object " +
                     stateMap[index]->object->GetName() +
                     " is infinite");

            state[index] = value;
            break;

         default:
            std::stringstream sel("");
            sel << stateMap[index]->subelement;
            std::string label = stateMap[index]->objectName + "." + 
                  stateMap[index]->elementName + "." + sel.str();
            MessageInterface::ShowMessage(
                  "%s not set; Element type not handled\n",label.c_str());
      }
   }
   
   // Manage epoch
   if (ObjectEpochsMatch() == false)
      MessageInterface::ShowMessage("Epochs do not match\n");

   if (objects.size() > 0)
      state.SetEpoch(objects[0]->GetRealParameter(epochIDs[0]));
   
   #ifdef DEBUG_OBJECT_UPDATES
      MessageInterface::ShowMessage(
            "After mapping %d objects to vector, contents are\n"
            "   Epoch = %.12lf\n", objects.size(), state.GetEpoch());
      for (Integer index = 0; index < stateSize; ++index)
      {
         std::stringstream msg("");
         msg << stateMap[index]->subelement;
         std::string lbl = stateMap[index]->objectName + "." + 
            stateMap[index]->elementName + "." + msg.str() + " = ";
         MessageInterface::ShowMessage("   %d: %s%.12lf\n", index, lbl.c_str(), 
               state[index]);
      }

      // Look at state data after
      MessageInterface::ShowMessage("Real object data after: [");
      for (Integer index = 0; index < stateSize; ++index)
         if (stateMap[index]->parameterType ==  Gmat::REAL_TYPE)
            MessageInterface::ShowMessage(" %lf ",
                  (stateMap[index]->object)->GetRealParameter(
                        stateMap[index]->parameterID));
      MessageInterface::ShowMessage("]\n");
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool PropagationStateManager::MapVectorToObjects()
//------------------------------------------------------------------------------
/**
 * Sets data from the propagation state vector into the objects that manage
 * those data
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::MapVectorToObjects()
{
   #ifdef DEBUG_OBJECT_UPDATES
      MessageInterface::ShowMessage("Mapping vector to objects\n"
            "   Epoch = %.12lf\n", state.GetEpoch());

      // Look at state data before
      MessageInterface::ShowMessage("V -> O: Real object data before: [");
      for (Integer index = 0; index < stateSize; ++index)
         if (stateMap[index]->parameterType ==  Gmat::REAL_TYPE)
            MessageInterface::ShowMessage(" %lf ",
                  (stateMap[index]->object)->GetRealParameter(
                        stateMap[index]->parameterID));
      MessageInterface::ShowMessage("]\n");
   #endif

   for (Integer index = 0; index < stateSize; ++index)
   {
      #ifdef DEBUG_OBJECT_UPDATES
         std::stringstream msg("");
         msg << stateMap[index]->subelement;
         std::string lbl = stateMap[index]->objectName + "." + 
            stateMap[index]->elementName + "." + msg.str() + " = ";
         MessageInterface::ShowMessage("   %d: %s%.12lf\n", index, lbl.c_str(), 
               state[index]);
      #endif

      switch (stateMap[index]->parameterType)
      {
         case Gmat::REAL_TYPE:
            (stateMap[index]->object)->SetRealParameter(
                     stateMap[index]->parameterID, state[index]);
            break;
            
         case Gmat::RVECTOR_TYPE:
            stateMap[index]->object->SetRealParameter(
                     stateMap[index]->parameterID, state[index],
                     stateMap[index]->rowIndex);
            break;

         case Gmat::RMATRIX_TYPE:
            stateMap[index]->object->SetRealParameter(
                     stateMap[index]->parameterID, state[index], 
                     stateMap[index]->rowIndex, stateMap[index]->colIndex);
            break;

         default:
            std::stringstream sel("");
            sel << stateMap[index]->subelement;
            std::string label = stateMap[index]->objectName + "." + 
                  stateMap[index]->elementName + "." + sel.str();
            MessageInterface::ShowMessage(
                  "%s not set; Element type not handled\n",label.c_str());
      }
   }
   

   GmatEpoch theEpoch = state.GetEpoch();
   for (UnsignedInt i = 0; i < objects.size(); ++i)
      objects[i]->SetRealParameter(epochIDs[i], theEpoch);

   #ifdef DEBUG_OBJECT_UPDATES
      // Look at object data after
      MessageInterface::ShowMessage("Real object data after: [");
      for (Integer index = 0; index < stateSize; ++index)
         if (stateMap[index]->parameterType ==  Gmat::REAL_TYPE)
            MessageInterface::ShowMessage(" %lf ",
                  (stateMap[index]->object)->GetRealParameter(
                        stateMap[index]->parameterID));
      MessageInterface::ShowMessage("]\n");
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool RequiresCompletion()
//------------------------------------------------------------------------------
/**
 * Flags if additional steps are needed for derivatives after superposition
 *
 * Returns true if there is a post-superposition step required in the
 * propagation state vector.  This case occurs, for example, for the orbit STM
 * and A-Matrix in order to fill in the upper half of the matrices, and (for the
 * STM) to apply \Phi\dot = A \Phi.
 *
 * @return true if a final "completion" step is needed, false if not.
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::RequiresCompletion()
{
   return hasPostSuperpositionMember;
}

//------------------------------------------------------------------------------
// bool ObjectEpochsMatch()
//------------------------------------------------------------------------------
/**
 * Tests to see if the object epochs match
 *
 * @return true is the eopchs match, false if not
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::ObjectEpochsMatch()
{
   bool retval = true;

   if (objects.size() > 0)
   {
      GmatEpoch theEpoch = objects[0]->GetRealParameter(epochIDs[0]);
      Real diff = 0.0, dt;
      for (UnsignedInt i = 1; i < objects.size(); ++i)
      {
         #ifdef DEBUG_OBJECT_UPDATES
            MessageInterface::ShowMessage("   Epochs are %.12lf for %s and "
               "%.12lf for %s\n", theEpoch, objects[0]->GetName().c_str(),
               objects[i]->GetRealParameter(epochIDs[i]), 
               objects[i]->GetName().c_str());
         #endif

         dt = fabs(theEpoch - objects[i]->GetRealParameter(epochIDs[i]));
         if (dt > IDENTICAL_TIME_TOLERANCE)
         {
            retval = false;
         }
         diff = (diff > dt ? diff : dt);
      }
      // Here's how we'll warn if needed:
//      if (retval && (diff != 0.0))
//         MessageInterface::ShowMessage("Spacecraft epochs do not match in a "
//               "Propagator used in the Propagate command, but are within "
//               "tolerance (%le sec) acceptable for propagation\n",
//               IDENTICAL_TIME_TOLERANCE * 86400.0);
   }
   return retval;
}


//------------------------------------------------------------------------------
// Integer GetCompletionCount()
//------------------------------------------------------------------------------
/**
 * Obtains the number of objects that need to complete updates
 *
 * @return The number of completion indices registered
 */
//------------------------------------------------------------------------------
Integer PropagationStateManager::GetCompletionCount()
{
   return completionIndexList.size();
}


//------------------------------------------------------------------------------
// Integer GetCompletionIndex(Integer start)
//------------------------------------------------------------------------------
/**
 * Describe the method here
 *
 * @param which Index of the beginning of the element that needs completion
 *
 * @return
 */
//------------------------------------------------------------------------------
Integer PropagationStateManager::GetCompletionIndex(const Integer which)
{
   return completionIndexList[which];
}


//------------------------------------------------------------------------------
// Integer GetCompletionSize(Integer start)
//------------------------------------------------------------------------------
/**
 * Describe the method here
 *
 * @param which Index of the beginning of the element that needs completion
 *
 * @return
 */
//------------------------------------------------------------------------------
Integer PropagationStateManager::GetCompletionSize(const Integer which)
{
   return completionSizeList[which];
}


//------------------------------------------------------------------------------
// Integer PropagationStateManager::SortVector()
//------------------------------------------------------------------------------
/**
 * Arranges the propagation state vector for use, and determines the size of the
 * vector
 *
 * @return The size of the state vector
 */
//------------------------------------------------------------------------------
Integer PropagationStateManager::SortVector()
{
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage(
            "Entered PropagationStateManager::SortVector()\n");
   #endif

   StringArray *propList;
   std::vector<Integer> order;
   std::vector<Gmat::StateElementId> idList;
   ObjectArray owners;
   StringArray property;
   std::vector<Integer>::iterator oLoc;

   Gmat::StateElementId id;
   Integer size, loc = 0, val;
   stateSize = 0;
   // Initially assume there is no post superposition member
   hasPostSuperpositionMember = false;


   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Element list:\n");
      Integer k = 0;
      for (std::map<GmatBase*, StringArray*>::iterator i = elements.begin();
            i != elements.end(); ++i)
      {
         current  = i->first;
         propList = i->second;
         MessageInterface::ShowMessage("   %d: %s ->\n", ++k,
               current->GetName().c_str());
         for (UnsignedInt j = 0; j < propList->size(); ++j)
         {
            MessageInterface::ShowMessage("      %s\n", (*propList)[j].c_str());
         }
      }
   #endif

   // First build a list of the property IDs and objects, measuring state size 
   // at the same time
   for (UnsignedInt q = 0; q < objects.size(); ++q)
   {
      current  = objects[q];
      propList = elements[current];
      
      for (StringArray::iterator j = propList->begin(); 
            j != propList->end(); ++j)
      {
         id = (Gmat::StateElementId)current->SetPropItem(*j);
         if (id == Gmat::UNKNOWN_STATE)
            throw PropagatorException("Unknown state element: " + (*j) +
                  " on object " + current->GetName() + ", a " +
                  current->GetTypeName());
         size = current->GetPropItemSize(id);
         if (size <= 0)
            throw PropagatorException("State element " + (*j) +
                  " has size set less than or equal to 0; unable to continue.");
         stateSize += size;
         for (Integer k = 0; k < size; ++k)
         {
            idList.push_back(id);
            if (current->PropItemNeedsFinalUpdate(id))
               hasPostSuperpositionMember = true;
            owners.push_back(current);
            property.push_back(*j);

            // Put this item in the ordering list
            oLoc = order.begin();
            while (oLoc != order.end())
            {
               val = idList[*oLoc];
               if (id < val)
               {
                  #ifdef DEBUG_STATE_CONSTRUCTION
                     MessageInterface::ShowMessage("Inserting; id = %d, z = %d,"
                           "  loc = %d\n", id, (*oLoc), loc);
                  #endif
                     
                  order.insert(oLoc, loc);
                  break;
               }
               ++oLoc;
            }
            if (oLoc == order.end())
               order.push_back(loc);
            
            ++loc;
         }
      }
   }
   
   ListItem *newItem;
   val = 0;
   completionIndexList.clear();
   completionSizeList.clear();

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage(
            "State size is %d()\n", stateSize);
   #endif
   
   for (Integer i = 0; i < stateSize; ++i)
   {
      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("%d <- %d: %d %s.%s gives ", i, order[i], 
               idList[order[i]], owners[order[i]]->GetName().c_str(), 
               property[order[i]].c_str());
      #endif

      newItem = new ListItem;
      newItem->objectName  = owners[order[i]]->GetName();
      newItem->elementName = property[order[i]];
      if (owners[order[i]]->HasAssociatedStateObjects())
         newItem->associateName = owners[order[i]]->GetAssociateName(val);
      else
         newItem->associateName = owners[order[i]]->GetName();
      newItem->object      = owners[order[i]];
      newItem->elementID   = idList[order[i]];
      newItem->subelement  = ++val;
      newItem->parameterID = 
            owners[order[i]]->GetParameterID(property[order[i]]);
      newItem->parameterType = 
            owners[order[i]]->GetParameterType(newItem->parameterID);
      
      newItem->dynamicObjectProperty =
            newItem->object->ParameterAffectsDynamics(newItem->parameterID);

      if (newItem->parameterType == Gmat::REAL_TYPE)
         newItem->parameterID += val - 1;

      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("[%s, %s, %s, %d, %d, %d, %d, %s]\n",
               newItem->objectName.c_str(),
               newItem->elementName.c_str(),
               newItem->associateName.c_str(),
               newItem->elementID,
               newItem->subelement,
               newItem->parameterID,
               newItem->parameterType,
               (newItem->dynamicObjectProperty ? "dynamic" : "static"));
      #endif

      if (newItem->parameterType == Gmat::RVECTOR_TYPE)
      {
         const Rvector vec =
            owners[order[i]]->GetRvectorParameter(property[order[i]]);
         newItem->rowLength = vec.GetSize();
         newItem->rowIndex = val - 1;
      }

      if (newItem->parameterType == Gmat::RMATRIX_TYPE)
      {
         const Rmatrix mat = 
            owners[order[i]]->GetRmatrixParameter(property[order[i]]);
         newItem->rowLength = mat.GetNumColumns();
         newItem->colIndex = (val-1) % newItem->rowLength;
         newItem->rowIndex = (Integer)((val - 1) / newItem->rowLength);
         
         #ifdef DEBUG_STATE_CONSTRUCTION
            MessageInterface::ShowMessage(
                  "RowLen = %d, %d -> row %2d  col %2d\n", newItem->rowLength, 
                  val, newItem->rowIndex, newItem->colIndex); 
         #endif
      }
      
      newItem->nonzeroInit = owners[order[i]]->
            ParameterDvInitializesNonzero(newItem->parameterID,
                  newItem->rowIndex, newItem->colIndex);
      if (newItem->nonzeroInit)
      {
         newItem->initialValue = owners[order[i]]->
                ParameterDvInitialValue(newItem->parameterID,
                      newItem->rowIndex, newItem->colIndex);
      }
      if (newItem->object->PropItemNeedsFinalUpdate(newItem->elementID))
      {
         completionIndexList.push_back(newItem->elementID);
         completionSizeList.push_back(1);       // Or count sizes?
//         newItem->nonzeroInit = true;
//         newItem->initialValue = 1.0;
      }

      newItem->postDerivativeUpdate = owners[order[i]]->
            ParameterUpdatesAfterSuperposition(newItem->parameterID);


      newItem->length      = owners[order[i]]->GetPropItemSize(idList[order[i]]);
      
      if (val == newItem->length)
         val = 0;
      
      stateMap.push_back(newItem);
   }

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("State map contents:\n");
      for (std::vector<ListItem*>::iterator i = stateMap.begin(); 
            i != stateMap.end(); ++i)
         MessageInterface::ShowMessage("   %s %s %d %d of %d, id = %d\n", 
               (*i)->objectName.c_str(), (*i)->elementName.c_str(),
               (*i)->elementID, (*i)->subelement, (*i)->length, 
               (*i)->parameterID); 
      MessageInterface::ShowMessage(
            "Finished PropagationStateManager::SortVector()\n");
   #endif
   
   return stateSize;
}
