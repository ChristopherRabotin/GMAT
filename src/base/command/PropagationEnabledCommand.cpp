//$Id$
//------------------------------------------------------------------------------
//                       PropagationEnabledCommand
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
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
// Created: 2009/07/20
//
/**
 * Implementation of the PropagationEnabledCommand base class
 */
//------------------------------------------------------------------------------


#include "PropagationEnabledCommand.hpp"
#include "MessageInterface.hpp"

#include "ODEModel.hpp"
#include "PropagationStateManager.hpp"

//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION
//#define DEBUG_STATE_BUFFERING
//#define DEBUG_PUBLISH_DATA
//#define DEBUG_EVENTLOCATORS
//#define DEBUG_EVENT_MODEL_FORCE

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//------------------------------------------------------------------------------
// PropagationEnabledCommand(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param typeStr The type string describing the subclass
 *
 * PropagationEnabledCommand is the abstract base class used for commands that
 * can propagate.  The class is made abstract through the destructor; all class
 * methods are implemented.
 */
//------------------------------------------------------------------------------
PropagationEnabledCommand::PropagationEnabledCommand(const std::string &typeStr)
:
   GmatCommand          (typeStr),
   direction            (1.0),
   overridePropInit     (false),
   hasFired             (false),
   inProgress           (false),
   dim                  (0),
   epochID              (-1),
   transientForces      (NULL),
   j2kState             (NULL),
   pubdata              (NULL),
   publishOnStep        (true)
{
   objectTypeNames.push_back("PropagationEnabledCommand");
   physicsBasedCommand = true;
}


//------------------------------------------------------------------------------
// ~PropagationEnabledCommand()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PropagationEnabledCommand::~PropagationEnabledCommand()
{
   if (!overridePropInit)
   {
      for (std::vector<PropSetup*>::iterator i = propagators.begin();
            i != propagators.end(); ++i)
      {
         if (*i)
            delete (*i);
      }
      propagators.clear();
   }

   for (std::vector<PropObjectArray*>::iterator i = propObjects.begin();
         i != propObjects.end(); ++i)
      delete (*i);
   propObjects.clear();

   if (pubdata)
      delete [] pubdata;
}


//------------------------------------------------------------------------------
// PropagationEnabledCommand(const PropagationEnabledCommand& pec)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param pec The PropagationEnabledCommand that is copied into a new instance
 */
//------------------------------------------------------------------------------
PropagationEnabledCommand::PropagationEnabledCommand(
      const PropagationEnabledCommand& pec) :
   GmatCommand          (pec),
   direction            (pec.direction),
   overridePropInit     (pec.overridePropInit),
   hasFired             (false),
   inProgress           (false),
   dim                  (pec.dim),
   epochID              (pec.epochID),
   transientForces      (NULL),
   j2kState             (NULL),
   pubdata              (NULL),
   publishOnStep        (true)
{
   isInitialized = false;
   propagatorNames = pec.propagatorNames;
   for (UnsignedInt i = 0; i < pec.propObjectNames.size(); ++i)
      propObjectNames.push_back(pec.propObjectNames[i]);

}


//------------------------------------------------------------------------------
// PropagationEnabledCommand& operator=(const PropagationEnabledCommand& pec)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param pec The PropagationEnabledCommand that is copied into this instance
 *
 * @return A reference to this instance, configured to match pec but not yet
 *         initialized.
 */
//------------------------------------------------------------------------------
PropagationEnabledCommand& PropagationEnabledCommand::operator=(
      const PropagationEnabledCommand& pec)
{
   if (this != &pec)
   {
      overridePropInit    = pec.overridePropInit;
      direction           = pec.direction;
      hasFired            = false;
      inProgress          = false;
      dim                 = pec.dim;
      epochID             = pec.epochID;
      isInitialized         = false;

      j2kState            = NULL;
      if (pubdata)
         delete [] pubdata;
      pubdata             = NULL;

      for (std::vector<PropSetup*>::const_iterator i = propagators.begin();
            i != propagators.end(); ++i)
         delete (*i);
      propagators.clear();
      propagatorNames = pec.propagatorNames;
      for (UnsignedInt i = 0; i < pec.propObjectNames.size(); ++i)
         propObjectNames.push_back(pec.propObjectNames[i]);

      transientForces = NULL;
      publishOnStep   = true;
   }

   return *this;
}


//------------------------------------------------------------------------------
// void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
 * Sets the array of transient forces, so it can be passed to the PropSetups.
 *
 * @param <tf> The array of transient forces.
 */
//------------------------------------------------------------------------------
void PropagationEnabledCommand::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   transientForces = tf;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for the command
 *
 * This method clones all of the objects that need cloned for a propagation
 * enabled command and sets up the corresponding data structures.  The data
 * is not filled in this method; PrepareToPropagate() performs that final piece
 * of preparation.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool PropagationEnabledCommand::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage
      ("PropagationEnabledCommand::Initialize() '%s' entered\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   bool retval = false;

   if (GmatCommand::Initialize())
   {
      inProgress = false;
      hasFired = false;
//      UnsignedInt index = 0;

      for (std::vector<PropObjectArray*>::iterator o = propObjects.begin();
            o != propObjects.end(); ++o)
      {
         delete (*o);
      }
      propObjects.clear();

//      SpaceObject *so;
      std::string pName;
//      GmatBase *mapObj = NULL;

      //// Ensure that we are using fresh objects when buffering stops
      EmptyBuffer();

      // Remove old PropSetups
      if (!overridePropInit)
      {
         if (propagators.size() > 0)
         {
            for (std::vector<PropSetup*>::iterator ps = propagators.begin();
                  ps != propagators.end(); ++ps)
            {
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Remove
                  ((*ps), (*ps)->GetName(), "PropagationEnabledCommand::"
                        "Initialize()", "deleting oldPs");
               #endif
               delete (*ps);
            }

            propagators.clear();
            p.clear();
            fm.clear();
         }

         // Todo Build the prop clones and set the related pointers
      }

      // Set the participant pointers
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Found %d lists of prop object names\n",
               propObjectNames.size());
      #endif

      // Now set the pointers for the objects that get propagated
      for (UnsignedInt i = 0; i < propObjectNames.size(); ++i)
      {
         PropObjectArray *objects;
         ObjectArray els;

         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("List %d contains %d prop objects\n",
                  i+1, propObjectNames[i].size());
         #endif

         if (propObjects.size() > i)
         {
            objects = propObjects[i];
            objects->clear();
         }
         else
         {
            // todo Memory debug and deallocation for this object
            objects = new PropObjectArray;
            propObjects.push_back(objects);
         }

         StringArray names = propObjectNames[i];
         PropSetup *currentPS = propagators[i];
         Propagator *currentP = currentPS->GetPropagator();
         ODEModel *currentODE = currentPS->GetODEModel();
         PropagationStateManager *currentPSM = currentPS->GetPropStateManager();

         StringArray owners, elements;
         /// @todo Check to see if All and All.Epoch belong for all modes.
         owners.push_back("All");
         elements.push_back("All.epoch");

         for (StringArray::iterator j = names.begin(); j != names.end(); ++j)
         {
            GmatBase *obj = FindObject(*j);
            if (obj == NULL)
               throw CommandException("Cannot find the object named " + (*j) +
                     " needed for propagation in the command\n" +
                     GetGeneratingString());
            if (obj->IsOfType(Gmat::SPACEOBJECT))
            {
               objects->push_back((SpaceObject*)obj);
               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("   Added the space object "
                        "named %s\n", obj->GetName().c_str());
               #endif

               // Now load up the PSM
               currentPSM->SetObject(obj);

               SpaceObject *so = (SpaceObject*)obj;
               if (epochID == -1)
                  epochID = so->GetParameterID("A1Epoch");
//               if (so->IsManeuvering())
//                  finiteBurnActive = true;

               AddToBuffer(so);

               // Uncommented out SetNames() for GMT-5101 fix (LOJ: 2015.05.14)
               if (so->GetType() == Gmat::FORMATION)
               {
                  // FillFormation(so, owners, elements);
               }
               else
               {
                  #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage
                     ("   Setting data labels for '%s'\n", so->GetName().c_str());
                  #endif
                  SetNames(so->GetName(), owners, elements);
               }
            }
            #ifdef DEBUG_INITIALIZATION
               else
                  MessageInterface::ShowMessage("   Found %s, not a space "
                        "object\n", obj->GetName().c_str());
            #endif
         }

         // Provide opportunity for derived cmds to set propagation properties
         SetPropagationProperties(currentPSM);

         if (currentPSM->BuildState() == false)
            throw CommandException("Could not build the state for the "
                  "command \n" + generatingString);
         if (currentPSM->MapObjectsToVector() == false)
            throw CommandException("Could not map state objects for the "
                  "command\n" + generatingString);

         currentODE->SetState(currentPSM->GetState());

         // Set solar system to ForceModel for Propagate inside a GmatFunction(loj: 2008.06.06)
         currentODE->SetSolarSystem(solarSys);

//         // Check for finite thrusts and update the force model if there are any
//         if (finiteBurnActive == true)
//            AddTransientForce(satName[index], currentODE);

         streamID = publisher->RegisterPublishedData(this, streamID, owners, elements);

         currentP->SetPhysicalModel(currentODE);
//         currentP->SetRealParameter("InitialStepSize",
//               fabs(currentP->GetRealParameter("InitialStepSize")) * direction);
         currentP->Initialize();

         // Set spacecraft parameters for forces that need them
         if (currentODE->SetupSpacecraftData((ObjectArray*)objects, 0) <= 0)
            throw PropagatorException("Propagate::Initialize -- "
                  "ODE model cannot set spacecraft parameters");

      }

      // Now we have everything we need to init the prop subsystem
      retval = true;
      isInitialized = true;
      // retval = AssemblePropagators();

      #ifdef DEBUG_INITIALIZATION
         if (retval == true)
            MessageInterface::ShowMessage("PEC Initialize() succeeded\n");
         else
            MessageInterface::ShowMessage(
                  "PEC Initialize() failed to initialize the PropSetups\n");
      #endif
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool HasLocalClones()
//------------------------------------------------------------------------------
/**
 * Method to check for the presence of local clones
 *
 * @return true if there are local clones, false if not
 */
//------------------------------------------------------------------------------
bool PropagationEnabledCommand::HasLocalClones()
{
   return true;
}


//------------------------------------------------------------------------------
// void UpdateClonedObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Updates cloned objects that are copies of the object passed in
 *
 * @param obj The object with setting updates
 */
//------------------------------------------------------------------------------
void PropagationEnabledCommand::UpdateClonedObject(GmatBase *obj)
{
   if (obj->IsOfType(Gmat::PROP_SETUP))
   {
      for (UnsignedInt i = 0; i < propagators.size(); ++i)
         if (obj->GetName() == propagators[i]->GetName())
         {
            propagators[i]->operator=(*((PropSetup*)obj));
            // Reset the fired flag so everything gets reconstructed
            hasFired = false;
         }
   }

   if (obj->IsOfType(Gmat::ODE_MODEL))
   {
      for (UnsignedInt j = 0; j < fm.size(); ++j)
         if (obj->GetName() == fm[j]->GetName())
         {
            fm[j]->operator=(*((ODEModel*)obj));
            #ifdef DEBUG_CLONES
               MessageInterface::ShowMessage("--------%p\n%s\nshould match %p"
                     "\n%s\n--------\n", fm[j],
                     fm[j]->GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
                     obj, obj->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
            #endif
         }

      for (UnsignedInt i = 0; i < propagators.size(); ++i)
      {
         ODEModel *odem = propagators[i]->GetODEModel();
         if (obj->GetName() == odem->GetName())
         {
            odem->operator=(*((ODEModel*)obj));

            #ifdef DEBUG_CLONES
               MessageInterface::ShowMessage("--------[%d] %p\n%s\nshould "
                     "match %p\n%s\n--------\n", i, odem,
                     odem->GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
                     obj, obj->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
            #endif
         }
      }
   }
}


//------------------------------------------------------------------------------
// void UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
//------------------------------------------------------------------------------
/**
 * This method changes a single parameter on an owned clone
 *
 * @param obj The master object holding the new parameter value
 * @param updatedParameterId The ID of the updated parameter
 */
//------------------------------------------------------------------------------
void PropagationEnabledCommand::UpdateClonedObjectParameter(GmatBase *obj,
      Integer updatedParameterId)
{
   GmatBase *theClone = NULL;
   if (obj->IsOfType(Gmat::PROP_SETUP))
      for (UnsignedInt i = 0; i < propagators.size(); ++i)
         if (obj->GetName() == propagators[i]->GetName())
            theClone = propagators[i];

   if (obj->IsOfType(Gmat::ODE_MODEL))
   {
      for (UnsignedInt j = 0; j < fm.size(); ++j)
         if (obj->GetName() == fm[j]->GetName())
            theClone = fm[j];

      for (UnsignedInt i = 0; i < propagators.size(); ++i)
         if (obj->GetName() == propagators[i]->GetName())
            theClone = propagators[i];
   }

   if (theClone != NULL)
      theClone->CopyParameter(*obj, updatedParameterId);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void SetPropagationProperties(PropagationStateManager *psm)
//------------------------------------------------------------------------------
/**
 * This method provides an interface derived commands can use to set specific
 * propagation properties where needed.  As an example the orbit STM can be set
 * using in an overridden implementation of this method for estimators that need
 * to propagate the orbit STM.
 *
 * @param psm A PropagationStateManager that controls the propagation state
 *            vector
 */
//------------------------------------------------------------------------------
void PropagationEnabledCommand::SetPropagationProperties(
      PropagationStateManager *psm)
{
}

//------------------------------------------------------------------------------
// bool PrepareToPropagate()
//------------------------------------------------------------------------------
/**
 * Fills in the data structures needed to start propagation
 *
 * This method completes the setup of the ODEModel, prepares the publisher data
 * array, fills in the data needed to propagate, and completes the steps needed
 * to propagate the objects selected for propagation by the user.
 *
 * @return true on success, false on failure
 *
 * todo The current implementation does not yet incorporate transient forces
 */
//------------------------------------------------------------------------------
bool PropagationEnabledCommand::PrepareToPropagate()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
            "PropagationEnabledCommand::PrepareToPropagate() entered\n");
   #endif
   bool retval = false;

   dim = 0;

   if (hasFired == true)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
               "   PrepareToPropagate() in hasFired state\n");
      #endif

//      // Handle the transient forces
//      for (std::vector<PropObjectArray*>::iterator poa = propObjects.begin();
//           poa != propObjects.end(); ++poa)
//      {
//         for (PropObjectArray::iterator sc = (*poa)->begin();
//               sc != (*poa)->end(); ++sc)
//         {
//            if (((SpaceObject*)(*sc))->IsManeuvering())
//            {
//               #ifdef DEBUG_FINITE_MANEUVER
//                  MessageInterface::ShowMessage(
//                     "SpaceObject %s is maneuvering\n", (*sc)->GetName().c_str());
//               #endif
//
// todo: Transient forces here
//               // Add the force
//               for (UnsignedInt index = 0; index < propagators.size(); ++index)
//               {
//                  for (std::vector<PhysicalModel*>::iterator i = transientForces->begin();
//                       i != transientForces->end(); ++i)
//                  {
//                     #ifdef DEBUG_TRANSIENT_FORCES
//                     MessageInterface::ShowMessage
//                        ("Propagate::PrepareToPropagate() Adding transientForce<%p>'%s'\n",
//                         *i, (*i)->GetName().c_str());
//                     #endif
//                     prop[index]->GetODEModel()->AddForce(*i);
//
//                     // todo: Rebuild ODEModel by calling BuildModelFromMap()
//                  }
//               }
//            }
//         }
//      }

      for (Integer n = 0; n < (Integer)propagators.size(); ++n)
      {
         elapsedTime[n] = 0.0;
         currEpoch[n]   = 0.0;
         fm[n]->SetTime(0.0);
         fm[n]->SetPropStateManager(propagators[n]->GetPropStateManager());
         fm[n]->UpdateInitialData();
         dim += fm[n]->GetDimension();

         p[n]->Initialize();
         p[n]->Update(true /*direction > 0.0*/);
//         state = fm[n]->GetState();
         j2kState = fm[n]->GetJ2KState();
      }

      baseEpoch.clear();

      for (Integer n = 0; n < (Integer)propagators.size(); ++n)
      {
         if (propObjectNames[n].empty())
            throw CommandException(
               "Propagator has no associated space objects.");

         GmatBase* sat1 = FindObject(*(propObjectNames[n].begin()));
         baseEpoch.push_back(sat1->GetRealParameter(epochID));
         elapsedTime[n] = fm[n]->GetTime();
         currEpoch[n] = baseEpoch[n] + elapsedTime[n] /
            GmatTimeConstants::SECS_PER_DAY;
      }

      inProgress = true;
   }
   else
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
               "PropagationEnabledCommand::PrepareToPropagate() first entry\n");
      #endif

//      // Set the prop state managers for the PropSetup ODEModels
//      for (std::vector<PropSetup*>::iterator i=propagators.begin();
//            i != propagators.end(); ++i)
//      {
//         #ifdef DEBUG_INITIALIZATION
//            MessageInterface::ShowMessage(
//                  "   Setting PSM on ODEModel for propagator %s\n",
//                  (*i)->GetName().c_str());
//         #endif
//
//         ODEModel *ode = (*i)->GetODEModel();
//         if (ode != NULL)    // Only do this for the PropSetups that integrate
//            ode->SetPropStateManager((*i)->GetPropStateManager());
//      }
//
//      // Initialize the subsystem
//      Initialize();

      // Loop through the PropSetups and build the models
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("   Looping through %d propagators\n",
               propagators.size());
      #endif
      for (std::vector<PropSetup*>::iterator i=propagators.begin();
            i != propagators.end(); ++i)
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage(
                  "   Setting PSM on ODEModel for propagator %s\n",
                  (*i)->GetName().c_str());
         #endif
         ODEModel *ode = (*i)->GetODEModel();
         if (ode != NULL)    // Only do this for the PropSetups that integrate
         {
            // Build the ODE model
            ode->SetPropStateManager((*i)->GetPropStateManager());
            if (ode->BuildModelFromMap() == false)
               throw CommandException("Unable to assemble the ODE model for " +
                     (*i)->GetName());
         }
      }

      p.clear();
      fm.clear();
      psm.clear();
      baseEpoch.clear();

      for (UnsignedInt n = 0; n < propagators.size(); ++n)
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage(
                  "   Setting pointers for propagator %s\n",
                  propagators[n]->GetName().c_str());
         #endif
         elapsedTime.push_back(0.0);

         p.push_back(propagators[n]->GetPropagator());
         fm.push_back(propagators[n]->GetODEModel());
         psm.push_back(propagators[n]->GetPropStateManager());
         currEpoch.push_back(psm[n]->GetState()->GetEpoch());

         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage(
                  "   Initializing propagator %s\n",
                  propagators[n]->GetName().c_str());
         #endif
         p[n]->Initialize();
         psm[n]->MapObjectsToVector();

         p[n]->Update(true/*direction > 0.0*/);
//         state = fm[n]->GetState();
         j2kState = fm[n]->GetJ2KState();
         baseEpoch.push_back(psm[n]->GetState()->GetEpoch());

         dim += fm[n]->GetDimension();

         hasFired = true;
         inProgress = true;
      }
   }

   if (pubdata)
   {
      #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (pubdata, "pubdata", "Propagate::PrepareToPropagate()",
             "deleting pub data");
      #endif
      delete [] pubdata;
   }
   pubdata = new Real[dim+1];
   #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (pubdata, "pubdata", "Propagate::PrepareToPropagate()",
          "pubdata = new Real[dim+1]");
   #endif

   // Publish the data
   pubdata[0] = currEpoch[0];
   memcpy(&pubdata[1], j2kState, dim*sizeof(Real));

   #ifdef DEBUG_PUBLISH_DATA
      MessageInterface::ShowMessage
         ("PropagationEnabledCommand::PrepareToPropagate() '%s' publishing initial %d data to "
          "stream %d, 1st data = %f\n", GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
          dim+1, streamID, pubdata[0]);
   #endif

   publisher->Publish(this, streamID, pubdata, dim+1, direction);

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage(
         "PropagationEnabledCommand::PrepareToPropagate() finished\n");
#endif

   return retval;
}


//------------------------------------------------------------------------------
// bool PropagationEnabledCommand::AssemblePropagators()
//------------------------------------------------------------------------------
/**
 * Prepares all of the propagators for propagation by setting the object
 * pointers to the objects that need to be propagated and then initializing the
 * PropSetups
 *
 * @return true on success, false if a PropSetup failed to initialize
 */
//------------------------------------------------------------------------------
bool PropagationEnabledCommand::AssemblePropagators()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
            "PropagationEnabledCommand::AssemblePropagators() entered\n");
   #endif
   bool retval = true;

   PropObjectArray *currentObjects;
   PropagationStateManager *psm;
   for (UnsignedInt i = 0; i < propagators.size(); ++i)
   {
      // For each PropSetup, set the SpaceObjects
      currentObjects = propObjects[i];
      psm = propagators[i]->GetPropStateManager();
      for (UnsignedInt j = 0; j < currentObjects->size(); ++j)
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage(
                  "Adding SpaceObject %s to PropSetup %s\n",
                  (*currentObjects)[j]->GetName().c_str(),
                  propagators[i]->GetName().c_str());
         #endif

         psm->SetObject((SpaceObject*)((*currentObjects)[j]));
      }

      // Now initialize the current PropSetup
      if (propagators[i]->Initialize() == false)
         return false;
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool Step(Real dt)
//------------------------------------------------------------------------------
/**
 * Steps each PropSetup by a specified amount
 *
 * todo: This is the method that needs to be adapted to handle large input time
 * steps
 *
 * @param dt The desired time step
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool PropagationEnabledCommand::Step(Real dt)
{
   bool retval = true;

   for (UnsignedInt i = 0; i < fm.size(); ++i)
   {
      fm[i]->UpdateInitialData();
      fm[i]->BufferState();
   }

   std::vector<Propagator*>::iterator current = p.begin();
   // Step all of the propagators by the input amount
   while (current != p.end())
   {
      if (!(*current)->Step(dt))
      {
         char size[32];
         std::sprintf(size, "%.12lf", dt);
         throw CommandException("In PropagationEnabledCommand::Step(), "
            "Propagator " + (*current)->GetName() +
            " failed to take a good final step (size = " + size + ")\n");
      }

      ++current;
   }

   for (UnsignedInt i = 0; i < fm.size(); ++i)
   {
      // orbit related parameters use spacecraft for data
      elapsedTime[i] = fm[i]->GetTime();
      currEpoch[i] = baseEpoch[i] + elapsedTime[i] /
         GmatTimeConstants::SECS_PER_DAY;

      // Update spacecraft epoch, without argument the spacecraft epoch
      // won't get updated for consecutive Propagate command
      fm[i]->UpdateSpaceObject(currEpoch[i]);
   }

   if (publishOnStep)
   {
      // Publish the data here
      pubdata[0] = currEpoch[0];
      memcpy(&pubdata[1], j2kState, dim*sizeof(Real));

      #ifdef DEBUG_PUBLISH_DATA
         MessageInterface::ShowMessage
            ("PropagationEnabledCommand::Step() '%s' publishing %d data to stream %d, 1st data = "
             "%f\n", GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
             dim+1, streamID, pubdata[0]);
      #endif

      publisher->Publish(this, streamID, pubdata, dim+1, direction);
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// bool TakeAStep(Real propStep)
//------------------------------------------------------------------------------
/**
 * Advances state but the input interval
 *
 * @param propStep Interval to step, in seconds.
 *
 * @return true on success, false on failure
 *
 * @note This default version always throws.  The method must be overridden in
 *       a derived class before using it.
 */
//------------------------------------------------------------------------------
bool PropagationEnabledCommand::TakeAStep(Real propStep)
{
   throw CommandException("TakeAStep must be overridden to use it; no override "
         "exists for " + typeName + " commands.");
}

//------------------------------------------------------------------------------
// void AddToBuffer(SpaceObject *so)
//------------------------------------------------------------------------------
/**
 * Adds satellites and formations to the state buffer.
 *
 * @param <so> The SpaceObject that is added.
 */
//------------------------------------------------------------------------------
void PropagationEnabledCommand::AddToBuffer(SpaceObject *so)
{
   #ifdef DEBUG_STOPPING_CONDITIONS
      MessageInterface::ShowMessage("Buffering states for '%s'\n",
         so->GetName().c_str());
   #endif

   if (so->IsOfType(Gmat::SPACECRAFT))
   {
      Spacecraft *clonedSat = (Spacecraft *)(so->Clone());
      satBuffer.push_back(clonedSat);
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (clonedSat, clonedSat->GetName(), "Propagate::AddToBuffer()",
          "(Spacecraft *)(so->Clone())");
      #endif
      //satBuffer.push_back((Spacecraft *)(so->Clone()));
   }
   else if (so->IsOfType(Gmat::FORMATION))
   {
      FormationInterface *form = (FormationInterface*)so;
      FormationInterface *clonedForm = (FormationInterface *)(so->Clone());
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (clonedForm, clonedForm->GetName(), "Propagate::AddToBuffer()",
          "(Formation *)(so->Clone())");
      #endif
      //formBuffer.push_back((Formation *)(so->Clone()));
      formBuffer.push_back(clonedForm);
      StringArray formSats = form->GetStringArrayParameter("Add");

      for (StringArray::iterator i = formSats.begin(); i != formSats.end(); ++i)
         AddToBuffer((SpaceObject *)(FindObject(*i)));
   }
   else
      throw CommandException("Object " + so->GetName() + " is not either a "
         "Spacecraft or a Formation; cannot buffer the object for propagator "
         "stopping conditions.");
}


//------------------------------------------------------------------------------
// void EmptyBuffer()
//------------------------------------------------------------------------------
/**
 * Cleans up the satellite state buffer.
 */
//------------------------------------------------------------------------------
void PropagationEnabledCommand::EmptyBuffer()
{
   for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
        i != satBuffer.end(); ++i)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*i), (*i)->GetName(), "Propagate::EmptyBuffer()", "deleting from satBuffer");
      #endif
      delete (*i);
   }
   satBuffer.clear();

   for (std::vector<FormationInterface *>::iterator i = formBuffer.begin();
        i != formBuffer.end(); ++i)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*i), (*i)->GetName(), "Propagate::EmptyBuffer()", "deleting from fromBuffer");
      #endif
      delete (*i);
   }
   formBuffer.clear();
}

//------------------------------------------------------------------------------
// void BufferSatelliteStates(bool fillingBuffer)
//------------------------------------------------------------------------------
/**
 * Preserves satellite state data so it can be restored after interpolating the
 * stopping condition propagation time.
 *
 * @param <fillingBuffer> Flag used to indicate the fill direction.
 */
//------------------------------------------------------------------------------
void PropagationEnabledCommand::BufferSatelliteStates(bool fillingBuffer)
{
   Spacecraft *fromSat, *toSat;
   FormationInterface *fromForm, *toForm;
   std::string soName;

   for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
        i != satBuffer.end(); ++i)
   {
      soName = (*i)->GetName();
      if (fillingBuffer)
      {
         fromSat = (Spacecraft *)FindObject(soName);
         toSat = *i;
      }
      else
      {
         fromSat = *i;
         toSat = (Spacecraft *)FindObject(soName);
      }

      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage(
            "   Sat is %s, fill direction is %s; fromSat epoch = %.12lf   "
            "toSat epoch = %.12lf\n",
            fromSat->GetName().c_str(),
            (fillingBuffer ? "from propagator" : "from buffer"),
            fromSat->GetRealParameter("A1Epoch"),
            toSat->GetRealParameter("A1Epoch"));

         MessageInterface::ShowMessage(
            "   '%s' Satellite state:\n", fromSat->GetName().c_str());
         Real *satrv = fromSat->GetState().GetState();
         MessageInterface::ShowMessage(
            "      %.12lf  %.12lf  %.12lf\n      %.12lf  %.12lf  %.12lf\n",
            satrv[0], satrv[1], satrv[2], satrv[3], satrv[4], satrv[5]);
      #endif

      (*toSat) = (*fromSat);

      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage(
            "After copy, From epoch %.12lf to epoch %.12lf\n",
            fromSat->GetRealParameter("A1Epoch"),
            toSat->GetRealParameter("A1Epoch"));
      #endif
   }

   for (std::vector<FormationInterface *>::iterator i = formBuffer.begin();
        i != formBuffer.end(); ++i)
   {
      soName = (*i)->GetName();
      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("Buffering formation %s, filling = %s\n",
            soName.c_str(), (fillingBuffer?"true":"false"));
      #endif
      if (fillingBuffer)
      {
         fromForm = (FormationInterface *)FindObject(soName);
         toForm = *i;
      }
      else
      {
         fromForm = *i;
         toForm = (FormationInterface *)FindObject(soName);
      }

      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage(
            "   Formation is %s, fill direction is %s; fromForm epoch = %.12lf"
            "   toForm epoch = %.12lf\n",
            fromForm->GetName().c_str(),
            (fillingBuffer ? "from propagator" : "from buffer"),
            fromForm->GetRealParameter("A1Epoch"),
            toForm->GetRealParameter("A1Epoch"));
      #endif

      (*toForm) = (*fromForm);

      toForm->UpdateState();

      #ifdef DEBUG_STATE_BUFFERING
         Integer count = fromForm->GetStringArrayParameter("Add").size();

         MessageInterface::ShowMessage(
            "After copy, From epoch %.12lf to epoch %.12lf\n",
            fromForm->GetRealParameter("A1Epoch"),
            toForm->GetRealParameter("A1Epoch"));

         MessageInterface::ShowMessage(
            "   %s for '%s' Formation state:\n",
            (fillingBuffer ? "Filling buffer" : "Restoring states"),
            fromForm->GetName().c_str());

         Real *satrv = fromForm->GetState().GetState();

         for (Integer i = 0; i < count; ++i)
            MessageInterface::ShowMessage(
               "      %d:  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf\n",
               i, satrv[i*6], satrv[i*6+1], satrv[i*6+2], satrv[i*6+3],
               satrv[i*6+4], satrv[i*6+5]);
      #endif
   }

   #ifdef DEBUG_STATE_BUFFERING
      for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
           i != satBuffer.end(); ++i)
         MessageInterface::ShowMessage(
            "   Epoch of '%s' is %.12lf\n", (*i)->GetName().c_str(),
            (*i)->GetRealParameter("A1Epoch"));
   #endif
}

//Moved from PropagateCommand for GMT-5101 fix (LOJ: 2015.05.14)
//------------------------------------------------------------------------------
// void SetNames(const std::string& name, StringArray& owners,
//               StringArray& elements)
//------------------------------------------------------------------------------
/**
 * Sets the parameter names used when publishing Spacecraft data.
 *
 * @param <name>     Name of the Spacecraft that is referenced.
 * @param <owners>   Array of published data identifiers.
 * @param <elements> Individual elements of the published data.
 */
//------------------------------------------------------------------------------
void PropagationEnabledCommand::SetNames(const std::string& name, StringArray& owners,
                         StringArray& elements)
{
   // Add satellite labels
   for (Integer i = 0; i < 6; ++i)
      owners.push_back(name);       // X, Y, Z, Vx, Vy, Vz

   elements.push_back(name+".X");
   elements.push_back(name+".Y");
   elements.push_back(name+".Z");
   elements.push_back(name+".Vx");
   elements.push_back(name+".Vy");
   elements.push_back(name+".Vz");
   
   #ifdef DEBUG_PUBLISH_DATA
   MessageInterface::ShowMessage
      ("PropagationEnabledCommand::SetNames() Setting data labels:\n");
   for (unsigned int i = 0; i < elements.size(); i++)
      MessageInterface::ShowMessage("%s ", elements[i].c_str());
   MessageInterface::ShowMessage("\n");
   #endif
}

