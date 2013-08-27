//$Id: EstimationStateManager.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         EstimationStateManager
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
// Created: 2009/08/05
//
/**
 * Implementation code for the estimation state manager.
 */
//------------------------------------------------------------------------------


#include "EstimationStateManager.hpp"
#include "MessageInterface.hpp"
#include "EstimatorException.hpp"
#include "Spacecraft.hpp"
#include <sstream>                  // for stringstream


//#define DEBUG_ESM_LOADING
//#define DEBUG_STATE_CONSTRUCTION
//#define DEBUG_OBJECT_UPDATES
//#define DUMP_STATE
//#define DEBUG_OBJECT_MAPPING
//#define DEBUG_CLONING
//#define DEBUG_INITIALIZATION
//#define DEBUG_ESM_LOADING

//------------------------------------------------------------------------------
// EstimationStateManager(Integer size)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param size The state manager size
 */
//------------------------------------------------------------------------------
EstimationStateManager::EstimationStateManager(Integer size) :
   StateManager         (size),
   psm                  (NULL),
   propagationState     (NULL),
   measMan              (NULL)
{
}


//------------------------------------------------------------------------------
// ~EstimationStateManager()
//------------------------------------------------------------------------------
/**
 * Class destructor
 */
//------------------------------------------------------------------------------
EstimationStateManager::~EstimationStateManager()
{
   for (UnsignedInt i = 0; i < estimationObjectClones.size(); ++i)
      delete estimationObjectClones[i];
}


//------------------------------------------------------------------------------
// EstimationStateManager(const EstimationStateManager & esm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param esm The state manager that is being copied
 */
//------------------------------------------------------------------------------
EstimationStateManager::EstimationStateManager(
      const EstimationStateManager & esm) :
   StateManager            (esm),
   solveForNames           (esm.solveForNames),
   solveForObjectNames     (esm.solveForObjectNames),
   solveForIDNames         (esm.solveForIDNames),
   considerNames           (esm.considerNames),
   considerObjectNames     (esm.considerObjectNames),
   considerIDNames         (esm.considerIDNames),
   participantNames        (esm.participantNames),
   stmMap                  (esm.stmMap),
   stmColCount             (esm.stmColCount),
   covMap                  (esm.covMap),
   covColCount             (esm.covColCount)
{
   for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
   {
      solveForObjects.push_back(NULL);
      solveForIDs.push_back(-1);
   }
   for (UnsignedInt i = 0; i < considerNames.size(); ++i)
   {
      considerObjects.push_back(NULL);
      considerIDs.push_back(-1);
   }
}


//------------------------------------------------------------------------------
// EstimationStateManager& operator=(const EstimationStateManager & esm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param esm The state manager assigning data to this one
 *
 * @return This state manager, configured to match esm
 */
//------------------------------------------------------------------------------
EstimationStateManager& EstimationStateManager::operator=(
      const EstimationStateManager & esm)
{
   if (&esm != this)
   {
      StateManager::operator=(esm);
      solveForNames       = esm.solveForNames;
      solveForObjectNames = esm.solveForObjectNames;
      solveForIDNames     = esm.solveForIDNames;
      considerNames       = esm.considerNames;
      considerObjectNames = esm.considerObjectNames;
      considerIDNames     = esm.considerIDNames;
      participantNames    = esm.participantNames;
      stmMap              = esm.stmMap;
      stmColCount         = esm.stmColCount;
      covMap              = esm.covMap;
      covColCount         = esm.covColCount;
      for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
      {
         solveForObjects.push_back(NULL);
         solveForIDs.push_back(-1);
      }
      for (UnsignedInt i = 0; i < considerNames.size(); ++i)
      {
         considerObjects.push_back(NULL);
         considerIDs.push_back(-1);
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// const StringArray& GetObjectList(std::string ofType)
//------------------------------------------------------------------------------
/**
 * Retrieves the names of all of the objects used by this estimation state
 * manager.
 *
 * @param ofType The type of object names requested.  Pass in the empty string
 *               for all objects.
 *
 * @return The list of objects
 */
//------------------------------------------------------------------------------
const StringArray& EstimationStateManager::GetObjectList(std::string ofType)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("EstimationStateManager::GetObjectList(%s) "
            "called\n", ofType.c_str());
   #endif
   chunks.clear();

   if (ofType == "")
      chunks = StateManager::GetObjectList(ofType);

   if ((ofType == "") || (ofType == "SolveFor"))
   {
      for (UnsignedInt i = 0; i < solveForObjectNames.size(); ++i)
      {
  	 		#ifdef DEBUG_INITIALIZATION
         	MessageInterface::ShowMessage("ESM processing %s\n",
            	   solveForNames[i].c_str());
			#endif

         if (find(chunks.begin(), chunks.end(), solveForNames[i]) ==
               chunks.end())
            chunks.push_back(solveForObjectNames[i]);
      }
   }

   if ((ofType == "") || (ofType == "Consider"))
   {
      for (UnsignedInt i = 0; i < considerObjectNames.size(); ++i)
      {
         if (find(chunks.begin(), chunks.end(), considerObjectNames[i]) ==
               chunks.end())
            chunks.push_back(considerObjectNames[i]);
      }
   }

   if ((ofType == "") || (ofType == "Participants"))
   {
      for (UnsignedInt i = 0; i < participantNames.size(); ++i)
      {
         if (find(chunks.begin(), chunks.end(), participantNames[i]) ==
               chunks.end())
            chunks.push_back(participantNames[i]);
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Current Object List:\n");
      for (UnsignedInt i = 0; i < chunks.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", chunks[i].c_str());
   #endif

   return chunks;
}


//------------------------------------------------------------------------------
// bool SetObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Sets the object pointers for objects used in this estimation state manager.
 *
 * @param obj The object pointer that is getting set
 *
 * @return true if the object was set, false if not
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetObject(GmatBase *obj)
{
   #ifdef DEBUG_ESM_LOADING
      MessageInterface::ShowMessage("+++Adding %s to the ESM\n",
            obj->GetName().c_str());
   #endif

   bool retval = false;
   std::string objName = obj->GetName();

   // If measurement manager has one of these objects (by name), use that one
   GmatBase *clone = measMan->GetClone(obj);
   if (clone != NULL)
      obj = clone;

   // Be sure object is not already in the list
   if (find(objects.begin(), objects.end(), obj) == objects.end())
   {
      objects.push_back(obj);
      current = obj;
      StringArray *objectProps = new StringArray;
      elements[current] = objectProps;

      if (obj->IsOfType(Gmat::FORMATION))
      {
         Integer id = obj->GetParameterID("A1Epoch");
         epochIDs.push_back(id);
      }
      else
      {
         #ifdef DEBUG_PARTICIPANTS
            MessageInterface::ShowMessage("Getting Epoch parameter for a %s "
                  "<%p> named %s\n", obj->GetTypeName().c_str(), obj,
                  obj->GetName().c_str());
         #endif
         Integer id;
         try
         {
            id = obj->GetParameterID("Epoch");
         }
         catch (...)
         {
            id = -1;
         }
         if (id != -1)
         {
            if (obj->GetParameterType(id) != Gmat::REAL_TYPE)
               id = obj->GetParameterID("A1Epoch");
         }

         epochIDs.push_back(id);
      }
   }

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Object set; current points to %s\n",
         current->GetName().c_str());
   #endif

   for (UnsignedInt i = 0; i < solveForObjectNames.size(); ++i)
   {
      // Set the object property
      if (solveForObjectNames[i] == objName)
      {
         solveForObjects[i] = obj;

         // Set the property ID
         Integer id = obj->GetEstimationParameterID(solveForIDNames[i]);
         solveForIDs[i] = id;
         elements[obj]->push_back(solveForIDNames[i]);
         retval = true;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// void EstimationStateManager::BufferObjects()
//------------------------------------------------------------------------------
/**
 * Clones the objects used in the estimation so that they can be restored later
 */
//------------------------------------------------------------------------------
void EstimationStateManager::BufferObjects(ObjectArray *buffer)
{
#ifdef DEBUG_CLONING
   MessageInterface::ShowMessage("EstimationStateManager::BufferObjects(%p) "
         "called\n", buffer);
#endif

   GmatBase *clone;
   for (UnsignedInt i = 0; i < estimationObjectClones.size(); ++i)
      delete estimationObjectClones[i];
   estimationObjectClones.clear();

   if (buffer != NULL)
   {
      for (UnsignedInt i = 0; i < buffer->size(); ++i)
         delete (*buffer)[i];
      buffer->clear();
   }

   for (UnsignedInt i = 0; i < objects.size(); ++i)
   {
      clone = objects[i]->Clone();
      estimationObjectClones.push_back(clone);
      if (buffer != NULL)
         buffer->push_back(clone->Clone());
   }
}


//------------------------------------------------------------------------------
// void EstimationStateManager::RestoreObjects()
//------------------------------------------------------------------------------
/**
 * Restores the objects from clones created in the BufferObjects() method
 */
//------------------------------------------------------------------------------
void EstimationStateManager::RestoreObjects(ObjectArray *fromBuffer)
{
   #ifdef DEBUG_CLONING
      MessageInterface::ShowMessage("EstimationStateManager::RestoreObjects(%p)"
            " called\n", fromBuffer);
      MessageInterface::ShowMessage("   %d objects and %d clones\n",
            objects.size(), estimationObjectClones.size());
   #endif

   ObjectArray *restoreBuffer = fromBuffer;
   if (fromBuffer == NULL)
   {
      restoreBuffer = &estimationObjectClones;
      MessageInterface::ShowMessage("Using internal buffer\n");
   }

   if (restoreBuffer->size() != objects.size())
   {
      std::stringstream errmsg;
      errmsg << "EstimationStateManager::RestoreObjects(): "
                "Clone size mismatch; there are " << objects.size()
             << " objects and " << restoreBuffer->size() << " clones.";
      throw EstimatorException(errmsg.str());
   }

   for (UnsignedInt i = 0; i < restoreBuffer->size(); ++i)
   {
      #ifdef DEBUG_CLONING
         MessageInterface::ShowMessage("Setting object %s from clone named "
               "%s\n", objects[i]->GetName().c_str(),
               (*restoreBuffer)[i]->GetName().c_str());
      #endif
      if (objects[i]->IsOfType(Gmat::SPACECRAFT))
         ((Spacecraft*)(objects[i]))->operator=(*((Spacecraft*)((*restoreBuffer)[i])));
      else
         *(objects[i]) = *((*restoreBuffer)[i]);
      #ifdef DEBUG_CLONING
         MessageInterface::ShowMessage("Object data:\n%s",
               objects[i]->GetGeneratingString(Gmat::SHOW_SCRIPT, "   ").c_str());
      #endif
   }
   #ifdef DEBUG_CLONING
      MessageInterface::ShowMessage("EstimationStateManager::RestoreObjects() "
            "finished\n");
   #endif
}


//------------------------------------------------------------------------------
// bool SetProperty(std::string prop)
//------------------------------------------------------------------------------
/**
 * Method used to identify SolveFor parameters to the estimation state manager
 *
 * @param prop String identifying the SolveFor parameter
 *
 * @return true if the parameter is accepted, false if not
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetProperty(std::string prop)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting SolveFor parameter \"%s\"\n",
            prop.c_str());
   #endif
   
   bool retval = false;

   if (find(solveForNames.begin(), solveForNames.end(), prop) ==
         solveForNames.end())
   {
      // Decompose the property name into object name and property ID
      DecomposeParameter(prop, 2);
      if (chunks.size() < 2)
         throw EstimatorException("SolveFor parameter is not properly "
               "formatted; the format entered, \"" + prop +
               "\", should be in the form object.parameter.");

      // Save the property elements and fill out the vectors
      solveForNames.push_back(prop);
      solveForObjectNames.push_back(chunks[0]);
      solveForObjects.push_back(NULL);
      solveForIDNames.push_back(chunks[1]);
      solveForIDs.push_back(-1);

      retval = true;
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Current SolveFor parameters:\n");
   
	   for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
   		MessageInterface::ShowMessage("   %s\n", solveForNames[i].c_str());
   #endif 

   return retval;
}


//------------------------------------------------------------------------------
// bool SetProperty(std::string prop, Integer loc)
//------------------------------------------------------------------------------
/**
 * Method used to identify SolveFor parameters to the estimation state manager,
 * potentially replacing a parameter that is already in place.
 *
 * @param prop String identifying the SolveFor parameter
 * @param loc The location in the SolveFor array desired for this property
 *
 * @return true if the parameter is accepted, false if not
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetProperty(std::string prop, Integer loc)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting SolveFor parameter %d to \"%s\"\n",
            loc, prop.c_str());
   #endif 
   bool retval = false;

   if (find(solveForNames.begin(), solveForNames.end(), prop) ==
         solveForNames.end())
   {
      // Decompose the property name into object name and property ID
      DecomposeParameter(prop, 2);
      if (chunks.size() < 2)
         throw EstimatorException("SolveFor parameter is not properly "
               "formatted; the format entered, \"" + prop +
               "\", should be in the form object.parameter.");

      // Save the property elements
      if (loc == (Integer)solveForNames.size())
      {
         // Save the property elements and fill out the vectors
         solveForNames.push_back(prop);
         solveForObjectNames.push_back(chunks[0]);
         solveForObjects.push_back(NULL);
         solveForIDNames.push_back(chunks[1]);
         solveForIDs.push_back(-1);
         retval = true;
      }
      else if ((loc < (Integer)solveForNames.size()) && (loc >= 0))
      {
         solveForNames[loc]       = prop;
         solveForObjectNames[loc] = chunks[0];
         solveForObjects[loc]     = NULL;
         solveForIDNames[loc]     = chunks[1];
         solveForIDs[loc]         = -1;
         retval = true;
      }
      else
         throw EstimatorException("Cannot set the SolveFor parameter " + prop);
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Current SolveFor parameters:\n");
      for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", solveForNames[i].c_str());
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool SetProperty(std::string sf, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Sets a SolveFor parameter associated with a specific object.
 *
 * This version of the call is not currently used.
 *
 * @param sf The solveFor parameter
 * @param obj The object that supplies the parameter data
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetProperty(std::string sf, GmatBase *obj)
{
   bool retval = false;
   return retval;
}


//------------------------------------------------------------------------------
// bool SetConsider(std::string prop)
//------------------------------------------------------------------------------
/**
 * Method used to identify Consider parameters to the estimation state manager
 *
 * @param prop String identifying the Consider parameter
 *
 * @return true if the parameter is accepted, false if not
 *
 * @note Consider parameters are not yet implemented
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetConsider(std::string prop)
{
   bool retval = false;

   MessageInterface::ShowMessage("Consider parameters are not yet "
         "implemented.\n");

   return retval;
}


//------------------------------------------------------------------------------
// bool SetConsider(std::string prop, Integer loc)
//------------------------------------------------------------------------------
/**
 * Method used to identify Consider parameters to the estimation state manager,
 * potentially replacing a parameter that is already in place.
 *
 * @param prop String identifying the Consider parameter
 * @param loc The location in the Consider array desired for this property
 *
 * @return true if the parameter is accepted, false if not
 *
 * @note Consider parameters are not yet implemented
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetConsider(std::string prop, Integer loc)
{
   bool retval = false;

   MessageInterface::ShowMessage("Consider parameters are not yet "
         "implemented.\n");

   return retval;
}

//------------------------------------------------------------------------------
// bool SetConsider(std::string con, GmatBase* obj)
//------------------------------------------------------------------------------
/**
 * Sets a Consider parameter associated with a specific object.
 *
 * This version of the call is not currently used.
 *
 * @param con The Consider parameter
 * @param obj The object that supplies the parameter data
 *
 * @return true on success, false on failure
 *
 * @note Consider parameters are not yet implemented
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetConsider(std::string con, GmatBase *obj)
{
   bool retval = false;

   MessageInterface::ShowMessage("Consider parameters are not yet "
         "implemented.\n");

   return retval;
}


//------------------------------------------------------------------------------
// void SetParticipantList(StringArray &p)
//------------------------------------------------------------------------------
/**
 * Passes the names of the participants into the ESM
 *
 * @param p The list of participants
 */
//------------------------------------------------------------------------------
void EstimationStateManager::SetParticipantList(StringArray &p)
{
   participantNames = p;
}


//------------------------------------------------------------------------------
// bool BuildState()
//------------------------------------------------------------------------------
/**
 * Assembles the estimation state vector
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::BuildState()
{
   bool retval = false;

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Entered BuildState()\n");
   #endif

   // Determine the size of the propagation state vector
   stateSize = SortVector();

   if (stateSize <= 0)
   {
      std::stringstream sizeVal;
      sizeVal << stateSize;
      throw EstimatorException("Estimation state vector size is " +
            sizeVal.str() + "; estimation is not possible.");
   }

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

   // Build the data structures for the STM and covariance matrix
   stm.SetSize(stateSize, stateSize);
   covariance.SetDimension(stateSize);
   stmColCount = stateSize;
   covColCount = stateSize;

   for (Integer index = 0; index < stateSize; ++index)
   {
      name = stateMap[index]->objectName;
      std::stringstream sel("");
      sel << stateMap[index]->subelement;
      state.SetElementProperties(index, stateMap[index]->elementID,
            name + "." + stateMap[index]->elementName + "." + sel.str(),
            associateMap[name]);
   }

   // Initialize the STM matrix
   for (Integer i = 0; i < stateSize; ++i)
      for (Integer j = 0; j < stateSize; ++j)
         if (i == j)
            stm(i,j) = 1.0;
         else
            stm(i,j) = 0.0;

   // Now build the covariance, using the elements the user has set and defaults
   // for the rest
   for (Integer i = 0; i < stateSize;)
   {
      Integer size    = stateMap[i]->length;
      Integer id      = stateMap[i]->parameterID;
      Covariance *cov = stateMap[i]->object->GetCovariance();

      #ifdef DEBUG_COVARIANCE
         MessageInterface::ShowMessage("Found length = %d for id = %d\n", size, id);
      #endif

      Rmatrix *subcov = cov->GetCovariance(id);
      if (subcov)
      {
         #ifdef DEBUG_COVARIANCE
            MessageInterface::ShowMessage("Found %d by %d subcovariance\n",
                  subcov->GetNumRows(), subcov->GetNumColumns());
         #endif
         for (Integer j = 0; j < size; ++j)
            for (Integer k = 0; k < size; ++k)
               covariance(i+j,i+k) = (*subcov)(j,k);
      }
      else
      {
         #ifdef DEBUG_COVARIANCE
            MessageInterface::ShowMessage("Found no subcovariance\n");
         #endif

         if (stateMap[i]->elementName == "CartesianState")
         {
            for (Integer j = 0; j < size; ++j)
            {
               for (Integer k = 0; k < size; ++k)
               {
                  if (j < 3)
                     covariance(i+j,i+k) = (j == k ? 1.0e12 : 0.0);
                  else
                     covariance(i+j,i+k) = (j == k ? 1.0e6 : 0.0);
               }
            }
         }
         else // Other defaults are set to the identity
         {
            for (Integer j = 0; j < size; ++j)
            {
               for (Integer k = 0; k < size; ++k)
               {
                  covariance(i+j,i+k) = (j == k ? 1.0e0 : 0.0);
               }
            }
         }
      }
      i += size;
   }

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage(
            "Estimation state vector has %d elements:\n", stateSize);
      StringArray props = state.GetElementDescriptions();
      for (Integer index = 0; index < stateSize; ++index)
         MessageInterface::ShowMessage("   %d:  %s\n", index,
               props[index].c_str());
   #endif

   #ifdef DUMP_STATE
      MapObjectsToVector();
      for (Integer i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage("State[%02d] = %.12lf, %s %d\n", i, state[i],
               "RefState start =", state.GetAssociateIndex(i));

      MessageInterface::ShowMessage("State Transition Matrix = %s\n",
            stm.ToString().c_str());
      MessageInterface::ShowMessage("Covariance Matrix = %s\n",
            covariance.ToString().c_str());
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool MapVectorToObjects()
//------------------------------------------------------------------------------
/**
 * Passes estimation state vector data to the associated objects
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::MapVectorToObjects()
{
   #ifdef DEBUG_OBJECT_UPDATES
      MessageInterface::ShowMessage("Mapping vector to objects\n"
            "   Epoch = %.12lf\n", state.GetEpoch());
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
      if (epochIDs[i] >= 0)
         objects[i]->SetRealParameter(epochIDs[i], theEpoch);

   return true;
}



//------------------------------------------------------------------------------
// bool EstimationStateManager::MapObjectsToVector()
//------------------------------------------------------------------------------
/**
 * Fills object data into the estimation state vector
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::MapObjectsToVector()
{
   #ifdef DEBUG_OBJECT_MAPPING
      MessageInterface::ShowMessage(
            "EstimationStateManager::MapObjectsToVector() called with state "
            "size %d\n", stateSize);
   #endif
   for (Integer index = 0; index < stateSize; ++index)
   {
      // Check object pointer
      if (stateMap[index]->object == NULL)
      {
         MessageInterface::ShowMessage("Object pointer for %s is NULL; "
               "skipping it for now.\n", stateMap[index]->objectName.c_str());
         continue;
      }
      #ifdef DEBUG_OBJECT_MAPPING
         else
            MessageInterface::ShowMessage("Object pointer for %s is %p\n",
                  stateMap[index]->objectName.c_str(), stateMap[index]->object);
      #endif

      switch (stateMap[index]->parameterType)
      {
         case Gmat::REAL_TYPE:
            state[index] = stateMap[index]->object->GetRealParameter(
                     stateMap[index]->parameterID);
            break;

         case Gmat::RVECTOR_TYPE:
            state[index] =
               stateMap[index]->object->GetRealParameter(
                     stateMap[index]->parameterID, stateMap[index]->rowIndex);
            break;

         case Gmat::RMATRIX_TYPE:
            state[index] =
               stateMap[index]->object->GetRealParameter(
                     stateMap[index]->parameterID, stateMap[index]->rowIndex,
                     stateMap[index]->colIndex);
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

   #ifdef DEBUG_OBJECT_MAPPING
      MessageInterface::ShowMessage("   State mapping complete; moving to epochs\n");
	#endif
	
   // Manage epoch
   GmatEpoch theEpoch = 0.0;
   for (UnsignedInt i = 0; i < objects.size(); ++i)
   {
      // Objects without epoch have a -1 set as their epochID
      if (epochIDs[i] >= 0)
      {
         if (theEpoch == 0.0)
            theEpoch = objects[i]->GetRealParameter(epochIDs[i]);
         else
         {
            if (theEpoch != objects[i]->GetRealParameter(epochIDs[i]))
               // should throw here
               MessageInterface::ShowMessage("Epoch mismatch\n");
         }
      }
   }
   state.SetEpoch(theEpoch);

   #ifdef DEBUG_OBJECT_UPDATES
      MessageInterface::ShowMessage(
            "After mapping objects to vector, contents are\n"
            "   Epoch = %.12lf\n", state.GetEpoch());
      for (Integer index = 0; index < stateSize; ++index)
      {
         std::stringstream msg("");
         msg << stateMap[index]->subelement;
         std::string lbl = stateMap[index]->objectName + "." +
            stateMap[index]->elementName + "." + msg.str() + " = ";
         MessageInterface::ShowMessage("   %d: %s%.12lf\n", index, lbl.c_str(),
               state[index]);
      }
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool MapSTMToObjects()
//------------------------------------------------------------------------------
/**
 * Passes state transition matrix data to the objects
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::MapSTMToObjects()
{
   bool retval = true;

   #ifdef DEBUG_STM_MAPPING
      MessageInterface::ShowMessage("Setting object STM's to\n");
      for (Integer i = 0; i < stateSize; ++i)
      {
         for (Integer j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", stm(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   // Fill in the STM based on the objects that comprise the state vector
   GmatBase* obj;
   Integer elementId; //, elementLength;
   for (UnsignedInt h = 0; h < stateMap.size(); ++h)
   {
      obj = stateMap[h]->object;
      if (stateMap[h]->subelement == 1)
      {
         elementId = stateMap[h]->parameterID;
//         elementLength = stateMap[h]->length;

         bool hasDstm = obj->HasDynamicParameterSTM(elementId);

         if (hasDstm)
         {
            Rmatrix* dstm = obj->GetParameterSTM(elementId);
            Integer stmSize = dstm->GetNumRows();
            // Fill in the object stm's from the master stm
            for (Integer i = 0; i < stmSize; ++i)
               for (Integer j = 0; j < stmSize; ++j)
                  (*dstm)(i,j) = stm(h+i, h+j);
         }

      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool MapObjectsToSTM()
//------------------------------------------------------------------------------
/**
 * Fills the estimation state transition matrix data from the objects
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::MapObjectsToSTM()
{
   bool retval = true;

   // Fill in the STM based on the objects that comprise the state vector
   GmatBase* obj;
   Integer elementId; //, elementLength;
   for (UnsignedInt h = 0; h < stateMap.size(); ++h)
   {
      obj = stateMap[h]->object;
      if (stateMap[h]->subelement == 1)
      {
         elementId = stateMap[h]->parameterID;
//         elementLength = stateMap[h]->length;

         bool hasDstm = obj->HasDynamicParameterSTM(elementId);

         #ifdef DEBUG_STM_MAPPING
            MessageInterface::ShowMessage("Prepping for STM; element %s for "
                  "object %s has ID %d and length %d, and %s a dynamic STM "
                  "contribution\n", stateMap[h]->elementName.c_str(),
                  obj->GetName().c_str(), elementId, elementLength,
                  (hasDstm ? "has" : "does not have"));
         #endif

         if (hasDstm)
         {
            const Rmatrix* dstm = obj->GetParameterSTM(elementId);
            Integer stmSize = dstm->GetNumRows();
            // Fill in the master stm with the current data
            for (Integer i = 0; i < stmSize; ++i)
               for (Integer j = 0; j < stmSize; ++j)
                  stm(h+i, h+j) = (*dstm)(i,j);
         }

      }
   }

   #ifdef DEBUG_STM_MAPPING
      MessageInterface::ShowMessage("Loaded object STM's; esm STM now contains\n");
      for (Integer i = 0; i < stateSize; ++i)
      {
         for (Integer j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", stm(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool MapCovariancesToObjects()
//------------------------------------------------------------------------------
/**
 * Passes covariance matrix data to the objects
 *
 * @return true on success
 *
 * @note Covariance mapping is not yet implemented
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::MapCovariancesToObjects()
{
   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// bool MapObjectsToCovariances()
//------------------------------------------------------------------------------
/**
 * Fills the covariance matrix data with data from the objects
 *
 * @return true on success
 *
 * @note Covariance mapping is not yet implemented
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::MapObjectsToCovariances()
{
   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// Rmatrix* GetSTM()
//------------------------------------------------------------------------------
/**
 * Retrieves the estimation state transition matrix
 *
 * @return A pointer to the matrix
 */
//------------------------------------------------------------------------------
Rmatrix* EstimationStateManager::GetSTM()
{
   return &stm;
}

//------------------------------------------------------------------------------
// Rmatrix* GetCovariance()
//------------------------------------------------------------------------------
/**
 * Retrieves the estimation covariance matrix
 *
 * @return A pointer to the matrix
 */
//------------------------------------------------------------------------------
Covariance* EstimationStateManager::GetCovariance()
{
   return &covariance;
}


//------------------------------------------------------------------------------
// void SetMeasurementManager(MeasurementManager *mm)
//------------------------------------------------------------------------------
/**
 * Passes a measurement manager pointer to the estimation state manager
 *
 * @param mm The MeasurementManager pointer
 */
//------------------------------------------------------------------------------
void EstimationStateManager::SetMeasurementManager(MeasurementManager *mm)
{
   measMan = mm;
}


//------------------------------------------------------------------------------
// void DecomposeParameter(std::string &param, Integer howMany)
//------------------------------------------------------------------------------
/**
 * This method breaks a string into several pieces, separating the string at
 * period characters
 *
 * @param param The string that is getting decomposed
 * @param howMany The maximum number of components allowed
 *
 * @return A StringArray containing the substrings
 */
//------------------------------------------------------------------------------
void EstimationStateManager::DecomposeParameter(std::string &param,
      Integer howMany)
{
   chunks.clear();
   UnsignedInt loc;

   loc = param.find('.');
   chunks.push_back(param.substr(0,loc));

   // todo: handle the howMany parameter.  For now, always split into 2 chunks
   // at 1st period
   if (loc != std::string::npos)
      chunks.push_back(param.substr(loc+1));

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("Chunks:\n");
      for (UnsignedInt i = 0; i < chunks.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", chunks[i].c_str());
   #endif
}


//------------------------------------------------------------------------------
// Integer SortVector()
//------------------------------------------------------------------------------
/**
 * This method performs preprocessing and state measurement tasks required by
 * the BuildState() method
 *
 * @return The size of the estimation state vector
 */
//------------------------------------------------------------------------------
Integer EstimationStateManager::SortVector()
{
   StringArray *elementList;
   std::vector<Integer> order;
   std::vector<Integer> idList;
   ObjectArray owners;
   StringArray property;
   std::vector<Integer>::iterator oLoc;

   Integer id = -1;
   Integer size, loc = 0, val;
   stateSize = 0;

   // First build a list of the property IDs and objects, measuring state size
   // at the same time
   for (std::map<GmatBase*, StringArray*>::iterator i = elements.begin();
         i != elements.end(); ++i)
   {
      current  = i->first;
      elementList = i->second;

      for (StringArray::iterator j = elementList->begin();
            j != elementList->end(); ++j)
      {
         id = current->GetEstimationParameterID(*j);
//         if (id == Gmat::UNKNOWN_STATE)
//            throw EstimatorException("Unknown state element: " + (*j));

         #ifdef DEBUG_OBJECT_MAPPING
            MessageInterface::ShowMessage("Element ID for %s is %d;", j->c_str(),
                  id);
         #endif

         size = current->GetEstimationParameterSize(id);
         #ifdef DEBUG_OBJECT_MAPPING
            MessageInterface::ShowMessage("  size is %d\n", size);
         #endif

         stateSize += size;
         for (Integer k = 0; k < size; ++k)
         {
            idList.push_back(id);
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
      newItem->object      = owners[order[i]];
      newItem->elementID   = idList[order[i]];
      newItem->subelement  = ++val;
      newItem->parameterID =
            owners[order[i]]->GetParameterID(property[order[i]]);
      newItem->parameterType =
            owners[order[i]]->GetParameterType(newItem->parameterID);

      if (newItem->parameterType == Gmat::REAL_TYPE)
         newItem->parameterID += val - 1;

      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("[%s, %s, %d, %d, %d, %d]\n",
               newItem->objectName.c_str(),
               newItem->elementName.c_str(),
               newItem->elementID,
               newItem->subelement,
               newItem->parameterID,
               newItem->parameterType);
      #endif

      if (newItem->parameterType == Gmat::RVECTOR_TYPE)
      {
         const Rvector mat =
            owners[order[i]]->GetRvectorParameter(property[order[i]]);
         newItem->rowLength = mat.GetSize();
         newItem->rowIndex = val-1;
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

      newItem->length =
            owners[order[i]]->GetEstimationParameterSize(idList[order[i]]);

      if (val == newItem->length)
         val = 0;

      stateMap.push_back(newItem);
   }

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("State size = %d; map contents:\n",
            stateSize);
      for (std::vector<ListItem*>::iterator i = stateMap.begin();
            i != stateMap.end(); ++i)
         MessageInterface::ShowMessage("   %s %s %d %d of %d, id = %d\n",
               (*i)->objectName.c_str(), (*i)->elementName.c_str(),
               (*i)->elementID, (*i)->subelement, (*i)->length,
               (*i)->parameterID);
   #endif

   return stateSize;
}
