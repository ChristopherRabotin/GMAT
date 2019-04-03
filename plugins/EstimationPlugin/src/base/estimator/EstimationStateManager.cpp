//$Id: EstimationStateManager.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         EstimationStateManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
#include "GroundstationInterface.hpp"
#include "StateConversionUtil.hpp"
#include "Planet.hpp"
#include <sstream>                  // for stringstream


//#define DEBUG_ESM_LOADING
//#define DEBUG_STATE_CONSTRUCTION
//#define DEBUG_OBJECT_UPDATES
//#define DUMP_STATE
//#define DEBUG_OBJECT_MAPPING
//#define DEBUG_CLONING
//#define DEBUG_COVARIANCE
//#define DEBUG_INITIALIZATION
//#define DEBUG_STM_MAPPING

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
   // Estimation always use precision time for calculation
   state.SetPrecisionTimeFlag(true);
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
            	   solveForObjectNames[i].c_str());
			#endif

         if (find(chunks.begin(), chunks.end(), solveForObjectNames[i]) ==
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
      MessageInterface::ShowMessage("+++Adding <%s,%p> to the ESM\n",
            obj->GetFullName().c_str(), obj);
   #endif

   bool retval = false;

   // If measurement manager has one of these objects (by name), use that one
   GmatBase *clone = measMan->GetClone(obj);
   if (clone != NULL)
      obj = clone;

   // Be sure object is not already in the list
   if (find(objects.begin(), objects.end(), obj) == objects.end())   // It accepts clone objects in objects list
   {
      // Set flag to tell participant using precision time
      obj->SetPrecisionTimeFlag(true);

      objects.push_back(obj);
      current = obj;
      StringArray *objectProps = new StringArray;
      elements[current] = objectProps;

      try
      {
         if (obj->IsOfType(Gmat::FORMATION))
         {
            Integer id = obj->GetParameterID("A1Epoch");
            epochIDs.push_back(id);
         }
         else
         {
            Integer id = obj->GetParameterID("Epoch");
            if (obj->GetParameterType(id) != Gmat::REAL_TYPE)
               id = obj->GetParameterID("A1Epoch");
            epochIDs.push_back(id);
         }
      }
      catch (BaseException&)
      {
         // Epoch is not a valid parameter
         epochIDs.push_back(-1);
      }
   

      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("Object set; current points to <%s,%p>\n",
            current->GetName().c_str(), current);
      #endif

      std::string objFullName = obj->GetFullName();      
      for (UnsignedInt i = 0; i < solveForObjectNames.size(); ++i)
      {
         // Set the object property
         if (solveForObjectNames[i] == objFullName)
         {
            solveForObjects[i] = obj;

            // Set the property ID
            Integer id = obj->GetEstimationParameterID(solveForIDNames[i]);
            
            // verify valid ID
            if (id == -1)
               throw EstimatorException("Error: Solve-for parameter " + obj->GetName() + "." + solveForIDNames[i] + " does not exist.\n");
            if (obj->IsEstimationParameterValid(id) == false)
               throw EstimatorException("Error: parameter " + obj->GetName() + "." + solveForIDNames[i] + " is not allowed to use as a solve-for variable in this GMAT version.\n");

            // set the object property
            solveForIDs[i] = id;
            elements[obj]->push_back(solveForIDNames[i]);
            retval = true;
         }
      }
   }

   #ifdef DEBUG_ESM_LOADING
      MessageInterface::ShowMessage("   ESM now has %d objects, %d "
            "solveForObjects, and %d solveFors\n", objects.size(), solveForObjects.size(),
            solveForIDNames.size());
   #endif

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
      #ifdef DEBUG_CLONING
         MessageInterface::ShowMessage("Using internal buffer\n");
      #endif
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
         MessageInterface::ShowMessage("Object <%p,%s> data:\n%s", objects[i], objects[i]->GetName().c_str(),
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
      MessageInterface::ShowMessage("EstmationStateManager::SetProperty():  Current SolveFor parameters:\n");
   
	   for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
   		  MessageInterface::ShowMessage("  solve for name = '%s';  object name = '%s';   parameter = '%s'\n", solveForNames[i].c_str(), solveForObjectNames[i].c_str(), solveForIDNames[i].c_str());
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
   else
   {
      // Display a message and do nothing
      MessageInterface::ShowMessage("Solve-for '%s' was set twice to Estimation State Manager. Skip setting...\n", prop.c_str());
      return true;
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("EstimationStateManager::SetProperty():    Current SolveFor parameters:\n");
      for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
	     MessageInterface::ShowMessage("  solve for name = '%s';  object name = '%s';   parameter = '%s'\n", solveForNames[i].c_str(), solveForObjectNames[i].c_str(), solveForIDNames[i].c_str());
   #endif
   
   return retval;
}


bool EstimationStateManager::IsPropertiesSetupCorrect()
{
   if (solveForNames.size() == 0)
      throw EstimatorException("Error: No solvefor parameters are set to estimation.\n");

   for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
   {
      if (solveForObjectNames[i] == "")
         throw EstimatorException("Error: '" + solveForNames[i] +"' has an empty object name.\n");

      if (i >= solveForObjects.size()) 
         throw EstimatorException("Error: '" + solveForNames[i] +"' object which is specified in AddSolverFor was not defined in your script.\n");
      else
      {
         if(solveForObjects[i] == NULL)
            throw EstimatorException("Error: '" + solveForNames[i] +"' object which is specified in AddSolverFor was not defined in your script.\n");
      }

      if (solveForIDNames[i] == "")
         throw EstimatorException("Error: '" + solveForNames[i] +"' has an empty parameter name.\n");

      if (i >= solveForIDs.size()) 
         throw EstimatorException("Error: '" + solveForNames[i] +"' parameter which is specified in AddSolverFor was not defined in your script.\n");
      else
      {
         if(solveForIDs[i] == -1)  // Changed from NULL to clear warning
            throw EstimatorException("Error: '" + solveForNames[i] +"' paramter which is specified in AddSolverFor was not defined in your script.\n");
      }
   }

   return true;
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
// bool SetProperty(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Sets a SolveFor parameter associated with a specific object. Assume that the 
 * object obj has SolveFors paramter containing object's solve-for names. Example: 
 * Sat.SolveFors = Cartesian. The solve-for parameter for this case is Sat.Cartisian
 *
 * @param obj The object that supplies solve-for parameter
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetProperty(GmatBase *obj)
{
   StringArray solforNames = GetSolveForList(obj);
   
   if (solforNames.empty())
      return false;

   for (UnsignedInt i = 0; i < solforNames.size(); ++i)
      SetProperty(solforNames[i]);

   return true;
}


//------------------------------------------------------------------------------
// StringArray GetSolveForList(GmatBase* obj)
//------------------------------------------------------------------------------
/**
* This function is used to create a list of solve-for properties for a given object
*
* @param obj        a pointer to the object 
* return            a list of solve-for properties associated with the input object 
*/
//------------------------------------------------------------------------------
StringArray EstimationStateManager::GetSolveForList(GmatBase* obj)
{
   StringArray participantNames = this->measMan->GetParticipantList();

   StringArray solveforList;
   
   if (obj->IsOfType(Gmat::SPACECRAFT))
   {
      // 1. Load solve-for list from spacecraft
      solveforList = obj->GetStringArrayParameter("SolveFors");
   }
   else if (obj->IsOfType(Gmat::GROUND_STATION))
   {
      // 2. Load solve-for list from ground station
      // 2.1. Get a list of error models
      std::map<std::string,ObjectArray> errorModelMap = ((GroundstationInterface*)obj)->GetErrorModelMap();
      for (std::map<std::string,ObjectArray>::iterator mapIndex = errorModelMap.begin(); mapIndex != errorModelMap.end(); ++mapIndex)
      {
         ObjectArray errorModels = mapIndex->second;
         for (UnsignedInt i = 0; i < errorModels.size(); ++i)
         {
            // Given fullName = 'CAN.SimSat_DSNRange_ErrorModel'. It needs to extract 'SimSat' from this string
            std::string fullName = errorModels[i]->GetFullName();
            std::string name = errorModels[i]->GetName();
            std::string s = fullName.substr(0, fullName.size() - name.size() - 1);
            std::string::size_type pos = s.find_first_of('.');
            s = s.substr(pos + 1);

            // If the name of spacecraft ('SimSat') is not in the participants list. It needs to skip setting solve-for
            bool found = false;
            for (Integer j = 0; j < participantNames.size(); ++j)
            {
               if (participantNames[j] == s)
               {
                  found = true;
                  break;
               }
            }
            if (!found)
               continue;

            // 2.2. Get solve-for list from error models
            StringArray sfList = errorModels[i]->GetStringArrayParameter("SolveFors");
            for(UnsignedInt j = 0; j < sfList.size(); ++j)
               solveforList.push_back(errorModels[i]->GetFullName() + "." + sfList[j]);
         }
      }
   }

   // 3. Add prefix
   for (UnsignedInt i = 0; i < solveforList.size(); ++i)
   {
      if (!obj->IsOfType(Gmat::GROUND_STATION))
         solveforList[i] = obj->GetName() + "." + solveforList[i]; 
   }

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Solve for parameters for <%s>:\n", obj->GetName().c_str());
      for (UnsignedInt i = 0; i < solveforList.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", solveforList[i].c_str());
   #endif

   return solveforList;
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
      MessageInterface::ShowMessage("   ESM now has %d objects, %d "
            "solveForObjects, and %d solveFors\n", objects.size(), solveForObjects.size(),
            solveForIDNames.size());

      MessageInterface::ShowMessage("   There are %d elements:\n", elements.size());
      for (std::map<GmatBase*, StringArray*>::iterator i = elements.begin();
            i != elements.end(); ++i)
      {
         MessageInterface::ShowMessage("      %s, with %d components\n", i->first->GetName().c_str(), i->second->size());
         for (UnsignedInt j = 0; j < i->second->size(); ++j)
            MessageInterface::ShowMessage("         %s\n", i->second->at(j).c_str());
      }
   #endif

   // Determine the size of the propagation state vector
   stateSize = SortVector();

   if (stateSize <= 0)
   {
      std::stringstream sizeVal;
      sizeVal << stateSize;
      throw EstimatorException("No solve-for parameter is defined for estimator;"
            " estimation is not possible.\n");
   }

   std::map<std::string,Integer> associateMap;
   // Build the associate map
   std::string name;
   for (Integer index = 0; index < stateSize; ++index)
   {
      name = stateMap[index]->objectFullName;
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
      name = stateMap[index]->objectFullName;
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
         MessageInterface::ShowMessage("Found length = %d for id = %d    object = <%p,%s>\n", size, id, stateMap[i]->object, stateMap[i]->object->GetName().c_str());
      #endif

      Rmatrix *subcov = cov->GetCovariance(id);
      if (subcov)
      {
         #ifdef DEBUG_COVARIANCE
            MessageInterface::ShowMessage("Found %d by %d subcovariance: [\n",
                  subcov->GetNumRows(), subcov->GetNumColumns());
         #endif
         for (Integer j = 0; j < size; ++j)
            for (Integer k = 0; k < size; ++k)
               covariance(i + j, i + k) = (*subcov)(j, k);
#ifdef DEBUG_COVARIANCE
         for (Integer j = 0; j < size; ++j)
         {
            for (Integer k = 0; k < size; ++k)
               MessageInterface::ShowMessage("%le   ", (*subcov)(j, k));
            MessageInterface::ShowMessage("\n");
         }
         MessageInterface::ShowMessage("]\n");
#endif
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
            covariance.GetCovariance()->ToString().c_str());
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
            "   Epoch = %s\n", state.GetEpochGT().ToString().c_str());
   #endif

   for (Integer index = 0; index < stateSize; ++index)
   {
      #ifdef DEBUG_OBJECT_UPDATES
         std::stringstream msg("");
         msg << stateMap[index]->subelement;
         std::string lbl = stateMap[index]->objectFullName + "." + 
            stateMap[index]->elementName + "." + msg.str() + " = ";
         MessageInterface::ShowMessage("   %d: %s%.12lf  for object <%s, %p>\n", index, lbl.c_str(),
            state[index], stateMap[index]->object->GetFullName().c_str(), stateMap[index]->object);
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

   #ifdef DEBUG_CLONING
   MessageInterface::ShowMessage("After map vector to object:\n");
   for (Integer index = 0; index < objects.size(); ++index)
   {
      MessageInterface::ShowMessage("Object <%p,%s> data:\n%s", objects[index], objects[index]->GetName().c_str(),
         objects[index]->GetGeneratingString(Gmat::SHOW_SCRIPT, "   ").c_str());
   }
   #endif

   GmatEpoch theEpoch = state.GetEpoch();
   GmatTime theEpochGT = state.GetEpochGT();
   for (UnsignedInt i = 0; i < objects.size(); ++i)
   {
      if (epochIDs[i] >= 0)
      {
         objects[i]->SetRealParameter(epochIDs[i], theEpoch);
         if (state.HasPrecisionTime())
            objects[i]->SetGmatTimeParameter(epochIDs[i], theEpochGT);
         else
            objects[i]->SetGmatTimeParameter(epochIDs[i], GmatTime(theEpoch));
      }
   }

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
         {
            MessageInterface::ShowMessage("Object pointer for %s is %p\n",
                  stateMap[index]->objectName.c_str(), stateMap[index]->object);
            MessageInterface::ShowMessage("  parameter type = %d   parameterID = %d\n",
               stateMap[index]->parameterType, stateMap[index]->parameterID);
         }
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
   
   if (state.HasPrecisionTime())
   {
      GmatTime theEpochGT = 0.0;
      for (UnsignedInt i = 0; i < objects.size(); ++i)
      {
         // Objects without epoch have a -1 set as their epochID
         if (epochIDs[i] >= 0)
         {
            if (theEpochGT == 0.0)
               theEpochGT = objects[i]->GetGmatTimeParameter(epochIDs[i]);
            else
            {
               if (theEpochGT != objects[i]->GetGmatTimeParameter(epochIDs[i]))
                  // should throw here
                  MessageInterface::ShowMessage("Epoch mismatch\n");
            }
         }
      } 
      state.SetEpochGT(theEpochGT);
   }

   #ifdef DEBUG_OBJECT_UPDATES
      MessageInterface::ShowMessage(
            "After mapping objects to vector, contents are\n"
            "   Epoch = %s\n", state.GetEpochGT().ToString().c_str());
      for (Integer index = 0; index < stateSize; ++index)
      {
         std::stringstream msg("");
         msg << stateMap[index]->subelement;
         std::string lbl = stateMap[index]->objectFullName + "." + 
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

         bool hasDstm = obj->HasDynamicParameterSTM(elementId);

         #ifdef DEBUG_STM_MAPPING
            Integer elementLength = stateMap[h]->length;
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
   size_t loc;             // change from std::string::size_type to size_t in order to compatible with C++98 and C++11

   loc = param.find_last_of('.');
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

      #ifdef DEBUG_OBJECT_MAPPING
         MessageInterface::ShowMessage("Working with %s, %d elements:\n",
               current->GetName().c_str(), elementList->size());
         for (UnsignedInt m = 0; m < elementList->size(); ++m)
            MessageInterface::ShowMessage("   %s\n", elementList->at(m).c_str());
      #endif

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
            property.push_back(current->GetParameterNameForEstimationParameter(*j));

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
               idList[order[i]], owners[order[i]]->GetFullName().c_str(), 
               property[order[i]].c_str());
      #endif

      newItem = new ListItem;
      newItem->objectName  = owners[order[i]]->GetName();
      newItem->objectFullName = owners[order[i]]->GetFullName();
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
         MessageInterface::ShowMessage("[%s, %s, %s, %s, %d, %d, %d, %d]\n",
               newItem->objectName.c_str(),
               newItem->objectFullName.c_str(),
               newItem->elementName.c_str(),
               newItem->associateName.c_str(),
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
         MessageInterface::ShowMessage("   objectName = %s   objectFullName = %s  elementName = %s   elementID = %d   subelement = %d of %d,   parameterID = %d\n",
               (*i)->objectName.c_str(), (*i)->objectFullName.c_str(), (*i)->elementName.c_str(),
               (*i)->elementID, (*i)->subelement, (*i)->length,
               (*i)->parameterID);
   #endif

   return stateSize;
}


GmatTime EstimationStateManager::GetEstimationEpochGT()
{
   GmatTime estEpoch;
   if (state.HasPrecisionTime())
      estEpoch = state.GetEpochGT();
   else
      estEpoch = GmatTime(state.GetEpoch());

   return estEpoch;
}


//------------------------------------------------------------------------------
// Rvector& GetParticipantState(GmatBase* spaceObj, std::string inStateType)
//------------------------------------------------------------------------------
/**
* This method used to get state of a spacecraft in its own coordinate system and in
* a given state type. For Keplerian state, anomaly is in form of MA (instead of TA)
*
* @param spaceObj      spacecraft from which we need to get state
* @param inStateType   state type used in the state. Its value can be
*                       "Cartesian", "Keplerian", or ""
* @param anomalyType   spacefify anomaly type to be "MA" or "TA" for Keplerian anomaly element.
*
* @return            spacecraft's state in its coordinate system and
*                    If inStateType is "Cartesian", state will be in Cartesian state
*                    If inStateType is "Keplerian", state will be in Keplerian state
*                    If inStateType is "", state will be in state type specified in 
*                       spacecraft's DisplayStateType parameter set in script
*
*/
//------------------------------------------------------------------------------
Rvector6 EstimationStateManager::GetParticipantState(GmatBase* spaceObj, std::string inStateType, std::string anomalyType)
{
   Rvector6 outState;

   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // 1.Get state of spacecraft in its internal coordinate system
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }
      
      Rvector6 inState(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);
      
      // Convert internal state to spacecraft's Cartesian coordinate system
      outState = inState;
      GmatTime epoch = GetEstimationEpochGT();

      Spacecraft* obj = (Spacecraft*)spaceObj;
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();

      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outStateCart;
      cv->Convert(epoch, inState, internalcs, outStateCart, cs);
      delete cv;
      
      // Convert Cartesian to Keplerian if it needs
      if (inStateType == "")
      {
         std::string stateType = obj->GetStringParameter("DisplayStateType");
         if (stateType == "Keplerian")
         {
            SolarSystem* ss = cs->GetSolarSystem();
            SpacePoint* origin = cs->GetOrigin();
            Planet* pn = (Planet*)origin;
            if (origin->IsOfType(Gmat::GROUND_STATION))
            {
               // for case CS is GDSTropocentric, the origin is ground station GDS
               std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
               pn = (Planet*)(ss->GetBody(cbName));
            }

            Real mu = pn->GetRealParameter("Mu");
            outState = StateConversionUtil::CartesianToKeplerian(mu, outStateCart, anomalyType);     // Keplerian anomaly element is in form of MA (instead of TA)
         }
         else
            outState = outStateCart;
      }
      else if (inStateType == "Keplerian")
      {
         SolarSystem* ss = cs->GetSolarSystem();
         SpacePoint* origin = cs->GetOrigin();
         Planet* pn = (Planet*)origin;
         if (origin->IsOfType(Gmat::GROUND_STATION))
         {
            // for case CS is GDSTropocentric, the origin is ground station GDS
            std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
            pn = (Planet*)(ss->GetBody(cbName));
         }

         Real mu = pn->GetRealParameter("Mu");
         outState = StateConversionUtil::CartesianToKeplerian(mu, outStateCart, anomalyType);        // Keplerian anomaly emement is in form of MA (instaed of TA)
      }
      else if (inStateType == "Cartesian")
         outState = outStateCart;
      else
         throw EstimatorException("Error: Input state type '" + inStateType + "' is invalid. It would be Cartesian, Keplerian, or an empty string.\n");
   }

   return outState;
}


//------------------------------------------------------------------------------
// Rvector& GetParticipantMJ2000EqState(GmatBase* spaceObj, std::string inStateType)
//------------------------------------------------------------------------------
/**
* This method used to get state of a spacecraft in MJ2000Eq axis and in
* a given state type. For Keplerian state, anomaly is in form of MA (instead of TA)
*
* @param spaceObj      spacecraft from which we need to get state
* @param inStateType   state type used in the state. Its value can be
*                       "Cartesian", "Keplerian", or ""
* @param anomalyType   spacefify anomaly type to be "MA" or "TA" for Keplerian anomaly element.
*
* @return            spacecraft's state in MJ2000Eq and
*                    If inStateType is "Cartesian", state will be in Cartesian state
*                    If inStateType is "Keplerian", state will be in Keplerian state
*                    If inStateType is "", state will be in state type specified in
*                       spacecraft's DisplayStateType parameter set in script
*
*/
//------------------------------------------------------------------------------
Rvector6 EstimationStateManager::GetParticipantMJ2000EqState(GmatBase* spaceObj, std::string inStateType, std::string anomalyType)
{
   Rvector6 outState;

   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // 1.Get state of spacecraft in its internal coordinate system
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }

      Rvector6 inState(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);

      // Convert internal state to spacecraft's Cartesian coordinate system
      outState = inState;
      GmatTime epoch = GetEstimationEpochGT();

      // Get spacecraft's internal coordinate system
      Spacecraft* obj = (Spacecraft*)spaceObj;
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      // Create spacecraft's MJ2000Eq Cartesian coordinate system
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");

      SolarSystem* ss = cs->GetSolarSystem();
      SpacePoint* origin = cs->GetOrigin();
      if (origin->IsOfType(Gmat::GROUND_STATION))
      {
         std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
         origin = ss->GetBody(cbName);
      }
      CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem("mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);
      
      // Get spacecraft's state in MJ2000Eq Cartesian coordinate system 
      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outStateCart;
      cv->Convert(epoch, inState, internalcs, outStateCart, mj2kCS);
      delete cv;
      delete mj2kCS;

      // Convert Cartesian to Keplerian if it needs
      if (inStateType == "")
      {
         std::string stateType = obj->GetStringParameter("DisplayStateType");
         if (stateType == "Keplerian")
         {
            Planet* pn = (Planet*)origin;
            Real mu = pn->GetRealParameter("Mu");
            outState = StateConversionUtil::CartesianToKeplerian(mu, outStateCart, anomalyType);     // Keplerian anomaly element is in form of MA (instead of TA)
         }
         else
            outState = outStateCart;
      }
      else if (inStateType == "Keplerian")
      {
         Planet* pn = (Planet*)origin;
         Real mu = pn->GetRealParameter("Mu");
         outState = StateConversionUtil::CartesianToKeplerian(mu, outStateCart, anomalyType);        // Keplerian anomaly emement is in form of MA (instaed of TA)
      }
      else if (inStateType == "Cartesian")
         outState = outStateCart;
      else
         throw EstimatorException("Error: Input state type '" + inStateType + "' is invalid. It would be Cartesian, Keplerian, or an empty string.\n");
   }
   
   return outState;
}


//------------------------------------------------------------------------------
// bool SetParticipantState(GmatBase* spaceObj, Rvector6& inputState,
//                          std::string inStateType)
//------------------------------------------------------------------------------
/**
* Method used to set state of a spacecraft. For Keplerian state, anomaly element 
* is in form of MA (instead of TA).
*
* @param spaceObj      spacecraft from which we need to get state
* @param inputState    state of spacecraft in its own coordinate system. For 
*                       Keplerian, anomaly element in the input state is in form of "MA"
* @param inStateType   state type used in the state. Its value can be 
*                       "Cartesian", "Keplerian", or ""
*
* @return            spacecraft's state in its coordinate system and
*                    If inStateType is "Cartesian", state will be in Cartesian state
*                    If inStateType is "Keplerian", state will be in Keplerian state
*                    If inStateType is "", state will be in state type specified in
*                       spacecraft's DisplayStateType parameter set in script
*
*/
//------------------------------------------------------------------------------
bool EstimationStateManager::SetParticipantState(GmatBase* spaceObj, 
    Rvector6& inputState, std::string inStateType)
{
   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // 1.Get state of spacecraft in its internal coordinate system
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }

      //Rvector6 inState(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);
      GmatTime epoch = GetEstimationEpochGT();

      Spacecraft* obj = (Spacecraft*)spaceObj;
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();

      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      // Convert Keplerian to Cartesian if it needs
      Rvector6 tempState;
      if (inStateType == "")
      {
         std::string stateType = obj->GetStringParameter("DisplayStateType");
         if (stateType == "Keplerian")
         {
            SolarSystem* ss = cs->GetSolarSystem();
            SpacePoint* origin = cs->GetOrigin();
            Planet* pn = (Planet*)origin;
            if (origin->IsOfType(Gmat::GROUND_STATION))
            {
               // for case CS is GDSTropocentric, the origin is ground station GDS
               std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
               pn = (Planet*)(ss->GetBody(cbName));
            }

            Real mu = pn->GetRealParameter("Mu");
            tempState = StateConversionUtil::KeplerianToCartesian(mu, inputState, "MA");
         }
         else
            tempState = inputState;
      }
      else if (inStateType == "Keplerian")
      {
         SolarSystem* ss = cs->GetSolarSystem();
         SpacePoint* origin = cs->GetOrigin();
         Planet* pn = (Planet*)origin;
         if (origin->IsOfType(Gmat::GROUND_STATION))
         {
            // for case CS is GDSTropocentric, the origin is ground station GDS
            std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
            pn = (Planet*)(ss->GetBody(cbName));
         }

         Real mu = pn->GetRealParameter("Mu");
         tempState = StateConversionUtil::KeplerianToCartesian(mu, inputState, "MA");
      }
      else if (inStateType == "Cartesian")
         tempState = inputState;
      else
         throw EstimatorException("Error: Input state type '" + inStateType + "' is invalid. It would be Cartesian, Keplerian, or an empty string.\n");
      
      // Convert from spacecraft coordiante system to internal coordinate system
      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outState;
      cv->Convert(epoch, tempState, cs, outState, internalcs);

      // Set state
      for (Integer j = 0; j < 6; ++j)
         state[i+j] = outState[j];

      delete cv;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool SetParticipantMJ2000EqState(GmatBase* spaceObj, Rvector6& inputState,
//                          std::string inStateType)
//------------------------------------------------------------------------------
/**
* Method used to set MJ2000Eq state of a spacecraft. For Keplerian state, anomaly element
* is in form of MA (instead of TA).
*
* @param spaceObj      spacecraft from which we need to get state
* @param inputState    state of spacecraft in MJ2000Eq coordinate system. For
*                       Keplerian, anomaly element in the input state is in form of "MA"
* @param inStateType   state type used in the state. Its value can be
*                       "Cartesian", "Keplerian", or ""
*
* @return            spacecraft's state in its coordinate system and
*                    If inStateType is "Cartesian", state will be in Cartesian state
*                    If inStateType is "Keplerian", state will be in Keplerian state
*                    If inStateType is "", state will be in state type specified in
*                       spacecraft's DisplayStateType parameter set in script
*
*/
//------------------------------------------------------------------------------
bool EstimationStateManager::SetParticipantMJ2000EqState(GmatBase* spaceObj,
   Rvector6& inputState, std::string inStateType)
{
   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // Get state of spacecraft object
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }
      Spacecraft* obj = (Spacecraft*)spaceObj;

      // Create spaceraft's coordinate system
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");

      SolarSystem* ss = cs->GetSolarSystem();
      SpacePoint* origin = cs->GetOrigin();
      if (origin->IsOfType(Gmat::GROUND_STATION))
      {
         std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
         origin = ss->GetBody(cbName);
      }

      // Convert Keplerian to Cartesian if it needs
      Rvector6 tempState;
      if (inStateType == "")
      {
         std::string stateType = obj->GetStringParameter("DisplayStateType");
         if (stateType == "Keplerian")
         {
            Planet* pn = (Planet*)origin;

            Real mu = pn->GetRealParameter("Mu");
            tempState = StateConversionUtil::KeplerianToCartesian(mu, inputState, "MA");
         }
         else
            tempState = inputState;
      }
      else if (inStateType == "Keplerian")
      {
         Planet* pn = (Planet*)origin;
         Real mu = pn->GetRealParameter("Mu");
         tempState = StateConversionUtil::KeplerianToCartesian(mu, inputState, "MA");
      }
      else if (inStateType == "Cartesian")
         tempState = inputState;
      else
         throw EstimatorException("Error: Input state type '" + inStateType + "' is invalid. It would be Cartesian, Keplerian, or an empty string.\n");


      // Get internal coordinate system
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      // Create spacecraft's MJ2000Eq Cartesian coordinate system
      CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem("mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);

      // Convert from spacecraft coordiante system to internal coordinate system
      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outState;
      GmatTime epoch = GetEstimationEpochGT();
      cv->Convert(epoch, tempState, mj2kCS, outState, internalcs);
      delete cv;
      delete mj2kCS;

      // Set state
      for (Integer j = 0; j < 6; ++j)
         state[i + j] = outState[j];
   }

   return true;
}


//------------------------------------------------------------------------------
// Rvector& GetParticipantCartesianState(GmatBase* spaceObj)
//------------------------------------------------------------------------------
/**
* Method used to convert state of a spacecraft from internal coordinate
* system to the coordinate system specified in script
*
* @param spaceObj    spacecraft from which we need to get state
*
* @return            spacecraft's state in its apparent coordinate system and 
*                    Cartesian state type
*
*/
//------------------------------------------------------------------------------
Rvector EstimationStateManager::GetParticipantCartesianState(GmatBase* spaceObj)
{
   Rvector outState;

   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // 1.Get state of spacecraft in its internal coordinate system
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }

      Rvector6 inState(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);
      outState = inState;
      GmatTime epoch = GetEstimationEpochGT();

      Spacecraft* obj = (Spacecraft*)spaceObj;
      //std::string stateType = obj->GetStringParameter("DisplayStateType");

      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();

      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outStateCart;
      cv->Convert(epoch, inState, internalcs, outStateCart, cs);
      delete cv;

      outState = outStateCart;
   }

   return outState;
}


//------------------------------------------------------------------------------
// Rvector& GetParticipantMJ2000EqCartesianState(GmatBase* spaceObj)
//------------------------------------------------------------------------------
/**
* Method used to convert state of a spacecraft from internal coordinate
* system to MJ2000Eq Cartesian coordinate system
*
* @param spaceObj    spacecraft from which we need to get state
*
* @return            spacecraft's state in MJ2000Eq Cartesian coordinate system
* example: spacecraft's coordinate system is EarthFixed, This function gets
*          spacecraft state in EarthMJ2000Eq coordinate system.
*
*/
//------------------------------------------------------------------------------
Rvector EstimationStateManager::GetParticipantMJ2000EqCartesianState(GmatBase* spaceObj)
{
   Rvector outState;

   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // Get spacecraft object
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }
      Spacecraft* obj = (Spacecraft*)spaceObj;

      // Get internal coordinate system
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      // Create MJ2000Eq Cartesian coordinate system
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");

      SolarSystem* ss = cs->GetSolarSystem();
      SpacePoint* origin = cs->GetOrigin();
      if (origin->IsOfType(Gmat::GROUND_STATION))
      {
         std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
         origin = ss->GetBody(cbName);
      }
      CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem(origin->GetName() + "mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);

      // Convert state from internal coordinate system to MJ2000Eq Cartesian coordinate system
      Rvector6 inState(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);
      outState = inState;
      GmatTime epoch = GetEstimationEpochGT();
      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outStateCart;
      cv->Convert(epoch, inState, internalcs, outStateCart, mj2kCS);

      delete cv;
      delete mj2kCS;

      outState = outStateCart;
   }

   return outState;
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationState()
//-------------------------------------------------------------------------
/**
* This Method used to get estimation state in Cartesian or Keplerian as
* specified by solve-for variable. Cr_Epsilon and Cd_Epsilon will be use
* instead of Cr and Cd.
*
* @return     estimation state in participants' coordinate systems and 
*             state type, Cr_Epsilon, Cd_Epsilon and bias
*
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationState()
{
   GmatState outputState;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateElement;
   outputState.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      // Get Cr and Cd instead of Cr_Epsilon and Cd_Epsilon
      if (((*map)[i]->elementName == "Cr_Epsilon") ||
         ((*map)[i]->elementName == "Cd_Epsilon") ||
         ((*map)[i]->elementName == "Bias"))
      {
         outputState[i] = state[i];
      }
      else if (((*map)[i]->elementName == "CartesianState") ||
         ((*map)[i]->elementName == "KeplerianState") ||
         ((*map)[i]->elementName == "Position"))
      {
         if ((*map)[i]->subelement == 1)
         {
            std::string stateType = "Cartesian";
            if ((*map)[i]->elementName == "KeplerianState")
               stateType = "Keplerian";

            // get a solve-for state. Note that: solve-for state in normal equation has Keplerian anomaly element in form of "MA"
            //Rvector outState = GetParticipantState((*map)[i]->object, stateType, "MA");
            Rvector outState = GetParticipantMJ2000EqState((*map)[i]->object, stateType, "MA");
            for (Integer j = 0; j < 6; ++j)
               outputState[i + j] = outState[j];

            i = i + 5;
         }
      }
   }

   return outputState;
}


GmatState* EstimationStateManager::SetEstimationState(GmatState& inputState)
{
   const std::vector<ListItem*> *map = GetStateMap();

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      // Get Cr and Cd instead of Cr_Epsilon and Cd_Epsilon
      if (((*map)[i]->elementName == "Cr_Epsilon") ||
         ((*map)[i]->elementName == "Cd_Epsilon") ||
         ((*map)[i]->elementName == "Bias"))
      {
         state[i] = inputState[i];
      }
      else if (((*map)[i]->elementName == "CartesianState") ||
         ((*map)[i]->elementName == "KeplerianState") ||
         ((*map)[i]->elementName == "Position"))
      {
         if ((*map)[i]->subelement == 1)
         {
            Rvector6 setVal;
            for (Integer j = 0; j < 6; ++j)
               setVal[j] = inputState[i+j];

            std::string stateType = "Cartesian";
            if ((*map)[i]->elementName == "KeplerianState")
               stateType = "Keplerian";
            //SetParticipantState((*map)[i]->object, setVal, stateType);
            SetParticipantMJ2000EqState((*map)[i]->object, setVal, stateType);

            i = i + 5;
         }
      }
   }

   return &state;
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationCartesianState()
//-------------------------------------------------------------------------
/**
* This Method used to convert result of estimation state to participants'
* coordinate system and state type. For report, it reports Cr_Epsilon and
* Cd_Epsilon.
*
* @return     estimation state in participants' coordinate systems and
*             state type, Cr_Epsilon, Cd_Epsilon and bias
*
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationCartesianState()
{
   GmatState outputState;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateElement;
   outputState.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      // Get Cr and Cd instead of Cr_Epsilon and Cd_Epsilon
      if (((*map)[i]->elementName == "Cr_Epsilon") ||
         ((*map)[i]->elementName == "Cd_Epsilon") ||
         ((*map)[i]->elementName == "Bias"))
      {
         outputState[i] = state[i];
      }
      else if (((*map)[i]->elementName == "CartesianState") ||
         ((*map)[i]->elementName == "KeplerianState") ||
         ((*map)[i]->elementName == "Position"))
      {
         if ((*map)[i]->subelement == 1)
         {
            Rvector outState = GetParticipantCartesianState((*map)[i]->object);
            for (Integer j = 0; j < 6; ++j)
               outputState[i + j] = outState[j];

            i = i + 5;
         }
      }
   }

   return outputState;
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationMJ2000EqCartesianState()
//-------------------------------------------------------------------------
/**
* This Method used to convert result of estimation state to MJ2000Eq
* Cartesian coordinate system. For report, it reports Cr_Epsilon and
* Cd_Epsilon.
*
* @return     estimation state in MJ2000Eq Cartesian coordinate systems,
*             Cr_Epsilon, Cd_Epsilon and bias
* example: spacecraft's coordinate system is EarthFixed, This function gets
*          spacecraft state in EarthMJ2000Eq coordinate system.
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationMJ2000EqCartesianState()
{
   GmatState outputState;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateElement;
   outputState.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      // Get Cr and Cd instead of Cr_Epsilon and Cd_Epsilon
      if (((*map)[i]->elementName == "Cr_Epsilon") ||
         ((*map)[i]->elementName == "Cd_Epsilon") ||
         ((*map)[i]->elementName == "Bias"))
      {
         outputState[i] = state[i];
      }
      else if (((*map)[i]->elementName == "CartesianState") ||
         ((*map)[i]->elementName == "KeplerianState") ||
         ((*map)[i]->elementName == "Position"))
      {
         if ((*map)[i]->subelement == 1)
         {
            Rvector outState = GetParticipantMJ2000EqCartesianState((*map)[i]->object);
            for (Integer j = 0; j < 6; ++j)
               outputState[i + j] = outState[j];

            i = i + 5;
         }
      }
   }

   return outputState;
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationCartesianStateForReport()
//-------------------------------------------------------------------------
/**
* This Method used to convert result of estimation state to participants'
* coordinate system and state type. For report, it reports Cr and Cd.
*
* @return     estimation state in participants' coordinate systems and
*             state type, Cr, Cd, and bias
*
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationCartesianStateForReport()
{
   GmatState outputState;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateElement;
   outputState.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      // Get Cr and Cd instead of Cr_Epsilon and Cd_Epsilon
      if ((*map)[i]->elementName == "Cr_Epsilon")
         outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("Cr");
      else if ((*map)[i]->elementName == "Cd_Epsilon")
         outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("Cd");
      else if((*map)[i]->elementName == "Bias")
         outputState[i] = state[i];
      else if (((*map)[i]->elementName == "CartesianState") ||
         ((*map)[i]->elementName == "KeplerianState") ||
         ((*map)[i]->elementName == "Position"))
      {
         if ((*map)[i]->subelement == 1)
         {
            Rvector outState = GetParticipantCartesianState((*map)[i]->object);
            for (Integer j = 0; j < 6; ++j)
               outputState[i + j] = outState[j];

            i = i + 5;
         }
      }
   }

   return outputState;
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationStateForReport()
//-------------------------------------------------------------------------
/**
* This Method used to convert result of estimation state to participants'
* coordinate system and state type. For report, it reports Cr and Cd 
* instead of Cr_Epsilon and Cd_Epsilon.
*
* @param anomalyType    form of anomaly is used. It would be "TA" or "MA". 
*                       Default value is "TA".
*
* @return     estimation state in participants' coordinate systems and
*             state type, Cr, Cd, and bias
*
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationStateForReport(std::string anomalyType)
{
   GmatState outputState;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateElement;
   outputState.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      // Get Cr and Cd instead of Cr_Epsilon and Cd_Epsilon
      if ((*map)[i]->elementName == "Cr_Epsilon")
         outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("Cr");
      else if ((*map)[i]->elementName == "Cd_Epsilon")
         outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("Cd");
      else if (((*map)[i]->elementName == "CartesianState") ||
         ((*map)[i]->elementName == "KeplerianState") ||
         ((*map)[i]->elementName == "Position"))
      {
         if ((*map)[i]->subelement == 1)
         {
            // In estimation report file or GmatLog.txt anomaly is always in "TA" form
            Rvector outState = GetParticipantState((*map)[i]->object, "", anomalyType);     // inStateType = "" that means get participant state in its own DisplayStateType
            for (Integer j = 0; j < 6; ++j)
               outputState[i + j] = outState[j];

            i = i + 5;
         }
      }
      else if ((*map)[i]->elementName == "Bias")
      {
         outputState[i] = state[i];
      }
   }

   return outputState;
}



//--------------------------------------------------------------------------------------------
// Rmatrix66 CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state)
//--------------------------------------------------------------------------------------------
/**
* This function is use to calculate derivative state conversion matrix for a spacecraft state.
* It converts from Cartesian to Keplerian (with mean anomaly) coordiate system.
*
* @param obj      it is a spacecraft object
* @param state    Cartesian state of the spacecraft
*
* return          6x6 derivative state conversion matrix [dX/dK]
*/
//--------------------------------------------------------------------------------------------
Rmatrix66 EstimationStateManager::CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state)
{
   // 1. Get mu value 
   Spacecraft* spacecraft = (Spacecraft*)obj;
   CoordinateSystem* cs = (CoordinateSystem*)(spacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
   SolarSystem* ss = cs->GetSolarSystem();
   SpacePoint* origin = cs->GetOrigin();
   Planet* pn = (Planet*)origin;
   if (origin->IsOfType(Gmat::GROUND_STATION))
   {
      // for case CS is GDSTropocentric, the origin is ground station GDS
      std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
      pn = (Planet*)(ss->GetBody(cbName));
   }

   Real mu = pn->GetRealParameter("Mu");

   // 2. Specify conversion matrix
   Rmatrix66 convMatrix = StateConversionUtil::CartesianToKeplerianDerivativeConversion(mu, state);

   return convMatrix;
}



//--------------------------------------------------------------------
//Rmatrix CartToSolveForStateConversionDerivativeMatrix()
//--------------------------------------------------------------------
/**
* This function is used to get matrix [dX/dS] which converts derivative
* in Cartesian state to Solve-for state. For Keplerian anomaly element,
* it is in form of MA.
*
* @return        matrix [dX/dS]
*/
//--------------------------------------------------------------------
Rmatrix EstimationStateManager::CartToSolveForStateConversionDerivativeMatrix()
{
   // 1.Get list of solve-for elements
   const std::vector<ListItem*>* items = GetStateMap();

   // 2. Get current estimation MJ2000Eq Cartesian state
   GmatState estCartState = GetEstimationMJ2000EqCartesianState();

   // 3.Calculate conversion matrix
   Integer size = items->size();
   Rmatrix conversion;
   conversion.SetSize(size, size);
   for (Integer i = 0; i < items->size();)
   {
      ListItem* item = items->at(i);
      if (item->elementName == "KeplerianState")
      {
         // 3.1.1. Get spacecraft object
         GmatBase* obj = item->object;

         // 3.1.2. Specify Cartesian state of the spacecraft
         Rvector6 inState(estCartState[i], estCartState[i + 1], estCartState[i + 2], estCartState[i + 3], estCartState[i + 4], estCartState[i + 5]);
         //MessageInterface::ShowMessage("   Use cartesian state of spacecraft <%s> to calculate [dX/dS]: [%.12lf   %.12lf   %.12lf   %.12lf   %.12lf   %.12lf\n", obj->GetName().c_str(), inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);

         // 3.1.3. Get conversion matrix [dX/dK] for the spacecraft
         Rmatrix66 coversion66 = CartesianToKeplerianCoverianceConvertionMatrix(obj, inState);

         // 3.1.4. Set value to the submatrix
         for (Integer row = 0; row < 6; ++row)
         {
            for (Integer col = 0; col < 6; ++col)
               conversion(i + row, i + col) = coversion66(row, col);
         }

         // 3.1.5. Move index to the next element
         i = i + 6;
      }
      else if (item->elementName == "CartesianState")
      {
         // 3.1.4. Set 6x6 identity matrix to the submatrix
         for (Integer row = 0; row < 6; ++row)
         {
            for (Integer col = 0; col < 6; ++col)
            {
               if (row == col)
                  conversion(i + row, i + col) = 1.0;
            }
         }

         // 3.1.5. Move index to the next element
         i = i + 6;
      }
      else
      {
         conversion(i, i) = 1.0;
         ++i;
      }
   }

   return conversion;
}


////--------------------------------------------------------------------
////Rmatrix CartToKeplConversionDerivativeMatrix()
////--------------------------------------------------------------------
///**
//* This function is used to get matrix [dX/dK] which converts derivative
//* in Cartesian state to Keplerian state. For other variable such as
//* bias, Cr_Epsilon, and Cd_Epsilon, their submatrix is identity matrix.
//* Keplerian anomaly element is MA.
//*
//* @return        matrix [dX/dK]
//*/
////--------------------------------------------------------------------
//Rmatrix EstimationStateManager::CartToKeplConversionDerivativeMatrix()
//{
//   // 1.Get list of solve-for elements
//   const std::vector<ListItem*>* items = GetStateMap();
//
//   // 2. Get current estimation MJ2000Eq Cartesian state
//   GmatState estCartState = GetEstimationMJ2000EqCartesianState();
//
//   // 3.Calculate conversion matrix
//   Integer size = items->size();
//   Rmatrix conversion;
//   conversion.SetSize(size, size);
//   for (Integer i = 0; i < items->size();)
//   {
//      ListItem* item = items->at(i);
//      if ((item->elementName == "KeplerianState") || (item->elementName == "CartesianState"))     // if solve-for is a spacecraft state
//      {
//         // 3.2.1. Get spacecraft object
//         GmatBase* obj = item->object;
//
//         // 3.1.2. Specify Cartesian state of the spacecraft
//         Rvector6 inState(estCartState[i], estCartState[i + 1], estCartState[i + 2], estCartState[i + 3], estCartState[i + 4], estCartState[i + 5]);
//
//         // 3.1.3. Get conversion matrix [dX/dK] for the spacecraft. Keplerian anomaly element is MA.
//         Rmatrix66 coversion66 = CartesianToKeplerianCoverianceConvertionMatrix(obj, inState);
//
//         // 3.1.4. Set value to the submatrix
//         for (Integer row = 0; row < 6; ++row)
//         {
//            for (Integer col = 0; col < 6; ++col)
//               conversion(i + row, i + col) = coversion66(row, col);
//         }
//
//         // 3.1.5. Move index to the next element
//         i = i + 6;
//      }
//      else
//      {
//         conversion(i, i) = 1.0;
//         ++i;
//      }
//   }
//
//   return conversion;
//}


//--------------------------------------------------------------------
//Rmatrix SolveForStateToKeplConversionDerivativeMatrix()
//--------------------------------------------------------------------
/**
* This function is used to get matrix [dS/dK] which converts derivative
* in solve-for state to Keplerian state. For other variable such as
* bias, Cr_Epsilon, and Cd_Epsilon, their submatrix is identity matrix
*
* @return        matrix [dS/dK]
*/
//--------------------------------------------------------------------
Rmatrix EstimationStateManager::SolveForStateToKeplConversionDerivativeMatrix()
{
   // 1.Get list of solve-for elements
   const std::vector<ListItem*>* items = GetStateMap();

   // 2. Get current estimation MJ2000Eq Cartesian state
   GmatState estCartState = GetEstimationMJ2000EqCartesianState();

   // 3.Calculate conversion matrix
   Integer size = items->size();
   Rmatrix conversion;
   conversion.SetSize(size, size);
   for (Integer i = 0; i < items->size();)
   {
      ListItem* item = items->at(i);
      if (item->elementName == "KeplerianState")
      {
         // 3.1.1. Set value to the submatrix
         for (Integer row = 0; row < 6; ++row)
         {
            for (Integer col = 0; col < 6; ++col)
            {
               if (row == col)
                  conversion(i + row, i + col) = 1.0;
            }
         }

         // 3.1.5. Move index to the next element
         i = i + 6;
      }
      else if (item->elementName == "CartesianState")
      {
         // 3.1.1. Get spacecraft object
         GmatBase* obj = item->object;

         // 3.1.2. Specify Cartesian state of the spacecraft
         Rvector6 inState(estCartState[i], estCartState[i + 1], estCartState[i + 2], estCartState[i + 3], estCartState[i + 4], estCartState[i + 5]);

         // 3.1.3. Get conversion matrix for the spacecraft
         Rmatrix66 coversion66 = CartesianToKeplerianCoverianceConvertionMatrix(obj, inState);

         // 3.1.4. Set value to the submatrix
         for (Integer row = 0; row < 6; ++row)
         {
            for (Integer col = 0; col < 6; ++col)
               conversion(i + row, i + col) = coversion66(row, col);
         }

         // 3.1.5. Move index to the next element
         i = i + 6;
      }
      else
      {
         conversion(i, i) = 1.0;
         ++i;
      }
   }

   return conversion;
}


////--------------------------------------------------------------------
////Rmatrix SolveForStateToDisplStateTypeConversionDerivativeMatrix()
////--------------------------------------------------------------------
///**
//* This function is used to get matrix [dS/dD] which converts derivative
//* in solve-for state to the state specified in spacecraft's
//* DisplayStateType. For other variable such as bias, Cr_Epsilon, and
//* Cd_Epsilon, their submatrix is identity matrix.
//*
//* @return        matrix [dS/dD]
//*/
////--------------------------------------------------------------------
//Rmatrix EstimationStateManager::SolveForStateToDisplStateTypeConversionDerivativeMatrix()
//{
//   // 1.Get list of solve-for elements
//   const std::vector<ListItem*>* items = GetStateMap();
//
//   // 2. Get current estimation state in MJ2000Eq Cartesian coordinate system
//   GmatState estCartState = GetEstimationMJ2000EqCartesianState();
//
//   // 3.Calculate conversion matrix
//   Integer size = items->size();
//   Rmatrix conversion;
//   conversion.SetSize(size, size);
//   for (Integer i = 0; i < items->size();)
//   {
//      ListItem* item = items->at(i);
//      if (item->elementName == "KeplerianState")
//      {
//         // Case 1: solve-for state is Keplerian
//         if (((Spacecraft*)(item->object))->GetStringParameter("DisplayStateType") == "Keplerian")
//         {
//            // if solve-for state and display state type are the same, coversion submatrix will be identity matrix
//            // 3.1.1.1 Set 6x6 identity matrix to the submatrix
//            for (Integer row = 0; row < 6; ++row)
//            {
//               for (Integer col = 0; col < 6; ++col)
//               {
//                  if (row == col)
//                     conversion(i + row, i + col) = 1.0;
//               }
//            }
//
//            // 3.1.1.2. Move index to the next element
//            i = i + 6;
//         }
//         else if (((Spacecraft*)(item->object))->GetStringParameter("DisplayStateType") == "Cartesian")
//         {
//            // if they are not the same, specify [dK/dX] matrix
//            // 3.1.2.1. Get spacecraft object
//            GmatBase* obj = item->object;
//
//            // 3.1.2.2. Specify Cartesian state of the spacecraft
//            Rvector6 inState(estCartState[i], estCartState[i + 1], estCartState[i + 2], estCartState[i + 3], estCartState[i + 4], estCartState[i + 5]);
//
//            // 3.1.2.3. Get conversion matrix [dK/dX] for the spacecraft
//            Rmatrix66 coversion66 = CartesianToKeplerianCoverianceConvertionMatrix(obj, inState).Inverse();
//
//            // 3.1.2.4. Set value to the submatrix
//            for (Integer row = 0; row < 6; ++row)
//            {
//               for (Integer col = 0; col < 6; ++col)
//                  conversion(i + row, i + col) = coversion66(row, col);
//            }
//
//            // 3.1.2.5. Move index to the next element
//            i = i + 6;
//         }
//      }
//      else if (item->elementName == "CartesianState")
//      {
//         // Case 2: solve-for state is Cartesian
//         if (((Spacecraft*)(item->object))->GetStringParameter("DisplayStateType") == "Cartesian")
//         {
//            // if solve-for state and display state type are the same, coversion submatrix will be identity matrix
//            // 3.2.1.1 Set 6x6 identity matrix to the submatrix
//            for (Integer row = 0; row < 6; ++row)
//            {
//               for (Integer col = 0; col < 6; ++col)
//               {
//                  if (row == col)
//                     conversion(i + row, i + col) = 1.0;
//               }
//            }
//
//            // 3.2.1.2. Move index to the next element
//            i = i + 6;
//         }
//         else if (((Spacecraft*)(item->object))->GetStringParameter("DisplayStateType") == "Keplerian")
//         {
//            // 3.2.2.1. Get spacecraft object
//            GmatBase* obj = item->object;
//
//            // 3.2.2.2. Specify Cartesian state of the spacecraft
//            Rvector6 inState(estCartState[i], estCartState[i + 1], estCartState[i + 2], estCartState[i + 3], estCartState[i + 4], estCartState[i + 5]);
//
//            // 3.2.2.3. Get conversion matrix [dX/dK] for the spacecraft
//            Rmatrix66 coversion66 = CartesianToKeplerianCoverianceConvertionMatrix(obj, inState);
//
//            // 3.2.2.4. Set value to the submatrix
//            for (Integer row = 0; row < 6; ++row)
//            {
//               for (Integer col = 0; col < 6; ++col)
//                  conversion(i + row, i + col) = coversion66(row, col);
//            }
//
//            // 3.2.2.5. Move index to the next element
//            i = i + 6;
//         }
//      }
//      else
//      {
//         conversion(i, i) = 1.0;
//         ++i;
//      }
//   }
//
//   return conversion;
//}


