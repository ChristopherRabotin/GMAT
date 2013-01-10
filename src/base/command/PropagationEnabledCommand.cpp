//$Id$
//------------------------------------------------------------------------------
//                       PropagationEnabledCommand
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
#include "EventLocator.hpp"
#include "Brent.hpp"
#include "EventModel.hpp"


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
   activeLocatorCount   (0),
   previousEventData    (NULL),
   currentEventData     (NULL),
   tempEventData        (NULL),
   eventBufferSize      (0),
   finder               (NULL),
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

   if (previousEventData != NULL)
      delete [] previousEventData;

   if (currentEventData != NULL)
      delete [] currentEventData;

   if (tempEventData != NULL)
      delete [] tempEventData;

   if (finder != NULL)
      delete finder;
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
   activeLocatorCount   (0),
   previousEventData    (NULL),
   currentEventData     (NULL),
   tempEventData        (NULL),
   eventBufferSize      (0),
   finder               (NULL),
   publishOnStep        (true)
{
   isInitialized = false;
   propagatorNames = pec.propagatorNames;
   for (UnsignedInt i = 0; i < pec.propObjectNames.size(); ++i)
      propObjectNames.push_back(pec.propObjectNames[i]);

   activeEventIndices.clear();
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

      activeLocatorCount  = 0;
      if (previousEventData != NULL)
         delete [] previousEventData;
      previousEventData = NULL;

      if (currentEventData != NULL)
         delete [] currentEventData;
      currentEventData = NULL;

      if (tempEventData != NULL)
         delete [] tempEventData;
      tempEventData = NULL;

      transientForces = NULL;
      eventBufferSize = 0;
      publishOnStep   = true;

      if (finder != NULL)
      {
         delete finder;
         finder = NULL;
      }
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

               // Add any locator that uses so to the PSM for step size control
               LocateObjectEvents(obj, els);

//               if (so->GetType() == Gmat::FORMATION)
//                  FillFormation(so, owners, elements);
//               else
//               {
//                  SetNames(so->GetName(), owners, elements);
//               }
            }
            #ifdef DEBUG_INITIALIZATION
               else
                  MessageInterface::ShowMessage("   Found %s, not a space "
                        "object\n", obj->GetName().c_str());
            #endif
         }

         if (els.size() != 0)
         {
            AddLocators(currentPSM, els);
            els.clear();
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

   publisher->Publish(this, streamID, pubdata, dim+1);

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

      publisher->Publish(this, streamID, pubdata, dim+1);
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


void PropagationEnabledCommand::LocateObjectEvents(const GmatBase *obj,
                           ObjectArray &els)
{
   #ifdef DEBUG_EVENTLOCATORS
      MessageInterface::ShowMessage("LocateObjectEvents called for %s\n",
            obj->GetName().c_str());
   #endif
   if (events == NULL)
      return;
   if (events->size() == 0)
      return;

   std::string objName = obj->GetName();

   // Walk through the events and see if any use this body as the target
   for (UnsignedInt i = 0; i < events->size(); ++i)
   {
      UnsignedInt fc = events->at(i)->GetFunctionCount();
      for (UnsignedInt j = 0; j < fc; ++j)
      {
         if (events->at(i)->GetTarget(j) == objName)
         {
            #ifdef DEBUG_EVENTLOCATORS
               MessageInterface::ShowMessage("      %s uses %s\n",
                     events->at(i)->GetName().c_str(), objName.c_str());
            #endif
            if (find(els.begin(), els.end(), events->at(i)) == els.end())
               els.push_back(events->at(i));
            break;
         }
      }
   }
}


void PropagationEnabledCommand::AddLocators(PropagationStateManager *currentPSM,
                           ObjectArray &els)
{
   #ifdef DEBUG_EVENTLOCATORS
      MessageInterface::ShowMessage("AddLocators called with %d locators\n",
            els.size());
   #endif
   for (UnsignedInt i = 0; i < els.size(); ++i)
      currentPSM->SetObject(els[i]);
}


//------------------------------------------------------------------------------
// void InitializeForEventLocation()
//------------------------------------------------------------------------------
/**
 * Prepares the event location functions to seek event boundaries
 */
//------------------------------------------------------------------------------
void PropagationEnabledCommand::InitializeForEventLocation()
{
   if (events == NULL)
   {
      #ifdef DEBUG_EVENTLOCATORS
         MessageInterface::ShowMessage("Initializing with no event locator "
               "pointer\n");
      #endif
      return;
   }

   #ifdef DEBUG_EVENTLOCATORS
      MessageInterface::ShowMessage("Initializing with %d event locators\n",
            events->size());
   #endif

   activeLocatorCount = 0;
   activeEventIndices.clear();
   eventStartIndices.clear();
   eventBufferSize = 0;

   for (UnsignedInt i = 0; i < events->size(); ++i)
   {
      if (events->at(i)->GetBooleanParameter("IsActive"))
      {
         ++activeLocatorCount;
         activeEventIndices.push_back(i);

         // Data for the Locator starts at the end of the current size
         eventStartIndices.push_back(eventBufferSize);
         // Each function returns 3 datum
         eventBufferSize += events->at(i)->GetFunctionCount() * 3;
      }
   }
   #ifdef DEBUG_EVENTLOCATORS
      MessageInterface::ShowMessage("Found %d active event locators\n",
            activeLocatorCount);
   #endif

   if (currentEventData != NULL)
   {
      delete [] currentEventData;
      currentEventData = NULL;
   }
   if (previousEventData != NULL)
   {
      delete [] previousEventData;
      previousEventData = NULL;
   }
   if (tempEventData != NULL)
   {
      delete [] tempEventData;
      tempEventData = NULL;
   }

   if ((eventBufferSize != 0) /*&& (propagators.size() > 0)*/)
   {
      currentEventData = new Real[eventBufferSize];
      previousEventData = new Real[eventBufferSize];
      tempEventData = new Real[eventBufferSize];

      // Load the initial data used in event location
      Integer dataIndex;
      for (Integer i = 0; i < activeLocatorCount; ++i)
      {
         Real *data = events->at(activeEventIndices[i])->Evaluate();
         dataIndex = eventStartIndices[i];
         UnsignedInt fc = events->at(activeEventIndices[i])->GetFunctionCount();
         for (UnsignedInt j = 0; j < fc*3; ++j)
            previousEventData[dataIndex + j] = data[j];
      }

      if (finder != NULL)
         delete finder;
      finder = new Brent;

      /// @todo: be sure the EM is added to force models 
      for (UnsignedInt index = 0; index < propagators.size(); ++index)
      {
         if (propagators[index]->GetPropagator()->UsesODEModel())
         {
            #ifdef DEBUG_EVENT_MODEL_FORCE
               MessageInterface::ShowMessage("   Adding event model to %s\n",
                     propagators[index]->GetName().c_str());
            #endif

            // Changed paradigm here -- event model belongs to ODEModel
            EventModel *em = new EventModel();
            em->SetEventLocators(events);
            propagators[index]->GetODEModel()->AddForce(em);

            // Refresh ODE model mapping, since a new force was added
            #ifdef DEBUG_EVENT_MODEL_FORCE
               MessageInterface::ShowMessage("   Building model from map\n");
            #endif
            //if (propagators[index]->GetODEModel()->BuildModelFromMap() == false)
            //   throw CommandException("Unable to assemble the ODE "
            //         "model  after adding an Event Model");
         }
      }
   }
   else
   {
      currentEventData = NULL;
      previousEventData = NULL;
      tempEventData = NULL;
   }

   #ifdef DEBUG_EVENT_MODEL_FORCE
      MessageInterface::ShowMessage("PropagationEnabledCommand::"
         "InitializeForEventLocation finished\n");
   #endif
}

//------------------------------------------------------------------------------
// void CheckForEvents()
//------------------------------------------------------------------------------
/**
 * Looks for a sign change in event function values and derivative values
 */
//------------------------------------------------------------------------------
void PropagationEnabledCommand::CheckForEvents()
{
   #ifdef DEBUG_EVENTLOCATORS
      MessageInterface::ShowMessage("In CheckForEvents: events = <%p> "
         "ActiveLocatorCount = %d Running = %d Solved = %d current = %d\n",
         events, activeLocatorCount, Gmat::RUNNING, Gmat::SOLVEDPASS, 
         currentRunState);
   #endif
   if (events == NULL)
   {
      #ifdef DEBUG_EVENTLOCATORS
         MessageInterface::ShowMessage("Checking for events with no event "
               "locator pointer\n");
      #endif
      return;
   }


   Integer dataIndex;
   // First evaluate the event functions
   for (Integer i = 0; i < activeLocatorCount; ++i)
   {
      Real *data = events->at(activeEventIndices[i])->Evaluate();
      dataIndex = eventStartIndices[i];
      UnsignedInt fc = events->at(activeEventIndices[i])->GetFunctionCount();
      for (UnsignedInt j = 0; j < fc*3; ++j)
         currentEventData[dataIndex + j] = data[j];
   }

   // Check function values
   for (UnsignedInt i = 1; i < eventBufferSize; i += 3)
   {
      if (currentEventData[i] * previousEventData[i] <= 0.0)
      {
         #ifdef DEBUG_EVENTLOCATORS
            MessageInterface::ShowMessage("Event function sign change "
                  "detected\n   Transition from %lf to %lf at index %d, epoch "
                  "[%12lf %12lf]\n", previousEventData[i], currentEventData[i],
                  i, previousEventData[i-1], currentEventData[i-1]);
            MessageInterface::ShowMessage("Relevant data:\n   Epochs:  "
               "%16.10lf %16.10lf\n   Values:  %16.12lf %16.12lf\n   Derivs:  "
               "%16.12lf, %16.12lf\n", currentEventData[i-1], 
               previousEventData[i-1], currentEventData[i], 
               previousEventData[i], currentEventData[i+1], 
               previousEventData[i+1]);
         #endif

         Integer index;
         for (index = 0; index < (Integer)(events->size()) - 1; ++index)
         {
            if (index + 1 < (Integer)events->size())
               if ((Integer)i < activeEventIndices[index+1])
                  break;
         }

         Integer functionIndex = (Integer)(i/3) - activeEventIndices[index];

         #ifdef DEBUG_EVENTLOCATORS
            MessageInterface::ShowMessage("i = %d gives function %d on "
                  "locator %d\n", i, functionIndex, index);
         #endif

         #ifdef DEBUG_EVENT_LOCATORS
            bool found = true; // for now; use to indicate refinements later
         #endif
         LocateEvent(events->at(index), functionIndex);

         #ifdef DEBUG_EVENT_LOCATORS
            if (found)
            {
               MessageInterface::ShowMessage("Found an event on locator %s "
                     "index %d\n",events->at(index)->GetTypeName().c_str(),
                     functionIndex);
            }
         #endif
      }
   }

   // Check derivative values
   for (UnsignedInt i = 2; i < eventBufferSize; i += 3)
   {
      // For now, skip derivative checks.  Remove this line when code is ready
      continue;
      
      // Sanity check: if fn value signs differ, the value locator should have 
      // caught the current case and the derivative check is irrelevant
      if (currentEventData[i-1] * previousEventData[i-1] <= 0.0)
         continue;

      // Proceed if derivative sign change was found
      if (currentEventData[i] * previousEventData[i] <= 0.0)
      {
         #ifdef DEBUG_EVENTLOCATORS
            MessageInterface::ShowMessage("Event function derivative sign "
                  "change detected\n   Transition from %lf to %lf at index %d, "
                  "epoch %12lf\n", currentEventData[i], previousEventData[i], i,
                  currentEventData[i-2]);
            MessageInterface::ShowMessage("Relevant data:\n   Epochs:  "
               "%16.10lf %16.10lf\n   Values:  %16.12lf %16.12lf\n   Derivs:  "
               "%16.12lf, %16.12lf\n", currentEventData[i-2], 
               previousEventData[i-2], currentEventData[i-1], 
               previousEventData[i-1], currentEventData[i], 
               previousEventData[i]);
         #endif

         // Apply the filtering logic
         bool findDerivativeZero = true;
         Real functionValue, earliestDeriv;

         // Set decision data based on time flow direction
         if (currentEventData[i-2] > previousEventData[i-2])
         {
            functionValue = previousEventData[i-1];
            earliestDeriv = previousEventData[i];
         }
         else
         {
            functionValue = currentEventData[i-1];
            earliestDeriv = currentEventData[i];
         }

         if ((earliestDeriv > 0.0) && (functionValue > 0.0))
            findDerivativeZero = false;
         if ((earliestDeriv < 0.0) && (functionValue < 0.0))
            findDerivativeZero = false;

         // Only proceed if a function zero might exist
         if (findDerivativeZero)
         {
            Integer index;
            for (index = 0; index < (Integer)(events->size()) - 1; ++index)
            {
               if (index + 1 < (Integer)events->size())
                  if ((Integer)i < activeEventIndices[index+1])
                     break;
            }

            Integer functionIndex = (Integer)(i/3) - activeEventIndices[index];

            #ifdef DEBUG_EVENTLOCATORS
               MessageInterface::ShowMessage("i = %d gives function %d on "
                     "locator %d\n", i, functionIndex, index);
            #endif

            #ifdef DEBUG_EVENT_LOCATORS
               bool found = true; // for now; use to indicate refinements later
            #endif

            // Find the derivative zero
            LocateEvent(events->at(index), functionIndex, true);

            #ifdef DEBUG_EVENT_LOCATORS
               if (found)
               {
                  MessageInterface::ShowMessage("Found an event on locator %s "
                        "index %d\n",events->at(index)->GetTypeName().c_str(),
                        functionIndex);
               }
            #endif
         }
      }
   }

   // Move current to previous
   memcpy(previousEventData, currentEventData, eventBufferSize*sizeof(Real));
}


//------------------------------------------------------------------------------
// bool LocateEvent(EventLocator* el, Integer index)
//------------------------------------------------------------------------------
/**
 * Seeks event boundaries given bracketing in the event function value
 *
 * @param el The EventLocator that holds the triggered event
 * @param index The index to the event in the locator that was triggered
 *
 * @return true if an event zero was found
 */
//------------------------------------------------------------------------------
bool PropagationEnabledCommand::LocateEvent(EventLocator* el, Integer index,
         bool forDerivative)
{
   bool eventFound = false;
   Integer valueOffset = (forDerivative ? 2 : 1);

   if (events == NULL)
   {
      #ifdef DEBUG_EVENTLOCATORS
         MessageInterface::ShowMessage("LocateEvent called; event locator "
               "pointer not set\n");
      #endif

      // TBD: Throw here?
      MessageInterface::ShowMessage("PropagationEnabledCommand::LocateEvent "
            "was called unexpectedly; no EventLocators have been set\n");

      return false;
   }

   if ((currentRunState == Gmat::RUNNING) ||
       (currentRunState == Gmat::SOLVEDPASS))
   {

      #ifdef DEBUG_EVENTLOCATORS
         // Linear interpolate to guess the epoch
         Real bounds[2], epochs[2];
         bounds[0] = previousEventData[index*3+1];
         bounds[1] = currentEventData[index*3+1];

         epochs[0] = previousEventData[index*3];
         epochs[1] = currentEventData[index*3];

         Real zero = epochs[0] - bounds[0] * (epochs[1] - epochs[0]) /
               (bounds[1] - bounds[0]);

         MessageInterface::ShowMessage("Zero ~ at %12lf for locator %s function "
            "index %d\n", zero, el->GetName().c_str(), index);
      #endif

      // Preserve the current state data
      Real propDir = direction;
      BufferSatelliteStates(true);
      bool wasPublishing = publishOnStep;
      publishOnStep = false;

      Integer stepsTaken = 0;
      Integer maxStepsAllowed = 31;  // 2^31 = 2.14e10

      // Build an estimate of machine precision for the independent variable
    //  Real machineEpochPrecision   = currentEventData[epochIndex];
      Real lastEpoch = currentEventData[index*3], currentStep, desiredEpoch;

      // Prepare the RootFinder
      Real brackets[2];
      brackets[0] = previousEventData[index*3];
      brackets[1] = currentEventData[index*3];
      finder->Initialize(previousEventData[index*3],
            previousEventData[index*3+valueOffset], currentEventData[index*3],
            currentEventData[index*3+valueOffset]);

      Real elapsedSeconds = 0.0;

      // Loop until (1) the maximum number of steps is taken, (2) the step
      // tolerance is at the numerical precision of the data, or (3) the step
      // achieves the step tolerance
   //   Real locateTolerance = el->GetTolerance() * GmatTimeConstants::DAYS_PER_SEC;
      Real locateTolerance = el->GetTolerance();
   //   // Variable used to get the time step measure from the search algorithm
   //   Real stepDifference;

      do
      {
         // Get the step desired
         desiredEpoch = finder->GetStep();
         currentStep = (desiredEpoch - lastEpoch) * GmatTimeConstants::SECS_PER_DAY;

         // Take the step
         std::vector<Propagator*>::iterator current = p.begin();
         // Step all of the propagators by the input amount
         while (current != p.end())
         {
            (*current)->SetAsFinalStep(true);
            bool isForward = (*current)->PropagatesForward();
            if (!(*current)->Step(currentStep))
            {
               char size[32], lbound[32], ubound[32];
               std::sprintf(size, "%.12lf", currentStep);
               std::sprintf(lbound, "%.12lf", (brackets[0] < brackets[1] ? 
                  brackets[0] : brackets[1]));
               std::sprintf(ubound, "%.12lf",  (brackets[0] > brackets[1] ? 
                  brackets[0] : brackets[1]));
               throw CommandException("In PropagationEnabledCommand::"
                  "LocateEvent, Propagator " + (*current)->GetName() +
                  " failed to take a good final step (size = " + size + 
                  "); Epoch bounds: [" + lbound + ", " + ubound + "]\n");

            }
            (*current)->SetAsFinalStep(false);
            (*current)->SetForwardPropagation(isForward);

            ++current;
         }

         // todo: This loop will miss propagators that do not have force models
         for (UnsignedInt i = 0; i < fm.size(); ++i)
         {
            // events and orbit related parameters use spacecraft for data
            Real elapsedTime;

            if (fm[i])
               elapsedTime = fm[i]->GetTime();
            else
               elapsedTime = p[i]->GetTime();

            GmatEpoch tempEpoch = baseEpoch[i] + elapsedTime /
               GmatTimeConstants::SECS_PER_DAY;

            // Update spacecraft epoch, without argument the spacecraft epoch
            // won't get updated for consecutive Propagate command
            if (fm[i])
               fm[i]->UpdateSpaceObject(tempEpoch);
            else
               p[i]->UpdateSpaceObject(tempEpoch);
         }

         elapsedSeconds += currentStep;

         // Evaluate the function and buffer it
         // First evaluate the event functions
         for (Integer i = 0; i < activeLocatorCount; ++i)
         {
            Real *data = events->at(activeEventIndices[i])->Evaluate();
            Integer dataIndex = eventStartIndices[i];
            UnsignedInt fc = events->at(activeEventIndices[i])->GetFunctionCount();
            for (UnsignedInt j = 0; j < fc*3; ++j)
               tempEventData[dataIndex + j] = data[j];
         }

         #ifdef DEBUG_EVENT_LOCATION
            MessageInterface::ShowMessage("ElapsedSecs = %.12lf; Passing in new "
                  "data: %12lf %.12lf\n", elapsedSeconds, tempEventData[index*3],
                  tempEventData[index*3+1]);
         #endif

         finder->SetValue(tempEventData[index*3], tempEventData[index*3+valueOffset]);
   //      stepDifference = finder->GetStepMeasure();

         lastEpoch = tempEventData[index*3];
         ++stepsTaken;
      }
      while ((stepsTaken < maxStepsAllowed) &&
             (GmatMathUtil::Abs(tempEventData[index*3+valueOffset]) > locateTolerance));
   //   while ((stepsTaken < maxStepsAllowed) && (stepDifference > locateTolerance));
   //
   //         &&
   //          (GmatMathUtil::Abs(currentStep) > GmatTimeConstants::MJD_EPOCH_PRECISION * 10.0));

      if ((GmatMathUtil::Abs(tempEventData[index*3+valueOffset]) < locateTolerance) &&
          (GmatMathUtil::Abs(tempEventData[index*3] - el->GetLastEpoch(index)) > 1.0 / 86400.0))
         eventFound = true;

   //   if ((stepDifference < locateTolerance) &&
   //       (GmatMathUtil::Abs(tempEventData[index*3] - el->GetLastEpoch(index)) > 1.0 / 86400.0))
   //      eventFound = true;

      // End of temporary section

      if (forDerivative && eventFound)
      {
         #ifdef DEBUG_EVENTLOCATORS
            MessageInterface::ShowMessage("Zero of derivative located at "
               "%.12lf\n", tempEventData[index*3]);
         #endif
         derivativeEpoch = tempEventData[index*3];
         return true;
      }

      if (eventFound)
      {
         #ifdef DEBUG_EVENTLOCATORS
            MessageInterface::ShowMessage("Found an event for function "
               "index %d\n", index);
         #endif
         UpdateEventTable(el, index);
         GmatEpoch start, end;
         finder->GetBrackets(start, end);
         el->SetFoundEventBrackets(index, start, end);
      }
      #ifdef DEBUG_EVENTLOCATORS
      else
      {
         // Linear interpolate to guess the epoch
         MessageInterface::ShowMessage("No zero found for locator %s function "
               "index %d; steps taken = %d\n", el->GetName().c_str(), index,
               stepsTaken);
      }
      #endif

      // Preserve the current state data
      BufferSatelliteStates(false);
      publishOnStep = wasPublishing;

      direction = propDir;
   }

   return eventFound;
}


void PropagationEnabledCommand::UpdateEventTable(EventLocator* el, Integer index)
{
   #ifdef DEBUG_EVENTLOCATORS
      MessageInterface::ShowMessage("Adding event %d on locator %s to the "
            "table\n", index, el->GetName().c_str());
   #endif
   el->BufferEvent(index);
}


//void PropagationEnabledCommand::UpdateEventTable(EventLocator* el, LocatedEvent *le)
//{
//}
