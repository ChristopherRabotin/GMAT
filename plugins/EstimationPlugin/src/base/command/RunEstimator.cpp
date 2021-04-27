//$Id: RunEstimator.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         RunEstimator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
// Created: 2009/08/03
//
/**
 * Implementation of the Mission Control Sequence command that drives estimation
 */
//------------------------------------------------------------------------------


#include "RunEstimator.hpp"
#include "EstimatorException.hpp"
#include "MessageInterface.hpp"
#include "ODEModel.hpp"
#include "Plate.hpp"                          // made changes by TUAN NGUYEN

//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION
//#define DEBUG_STATE
//#define DEBUG_STATE_RESETS
//#define DEBUG_EVENT_STATE
//#define DEBUG_LOAD_SOLVEFORS
//#define DEBUG_INITIAL_STATE


//------------------------------------------------------------------------------
// RunEstimator()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
RunEstimator::RunEstimator(const std::string &typeStr) :
   RunSolver               (typeStr),
   theEstimator            (NULL),
   commandRunning          (false),
   commandComplete         (false),
   propPrepared            (false),
   estimationOffset        (0.0),
   bufferFilled            (false),
   currentEvent            (NULL),
   eventProcessComplete    (false),
   eventMan                (NULL)
{
   needToResetSTM = true;                           // made changes by TUAN NGUYEN

   overridePropInit = true;
   publishOnStep = false;
   delayInitialization = true;

   hasPrecisionTime = true;
}


//------------------------------------------------------------------------------
// ~RunEstimator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RunEstimator::~RunEstimator()
{
   if (theEstimator)
      delete theEstimator;

   // clean up ObjectArray eventList;
   // eventList contains a list of pointers which point to objects in MeasurementManager. The task to delete those objects have to be handled by MeasurementManager
   eventList.clear();

   // clean up Event *currentEvent;
   currentEvent = NULL;                  // This class does not create Event object, so it does not to delete it.
   // clean up EventManager *eventMan;
   eventMan = NULL;                      // This class does not create EventManager object, so it does not to delete it.

}


//------------------------------------------------------------------------------
// RunEstimator(const RunEstimator & rs)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rs The command that is copied into the new one
 */
//------------------------------------------------------------------------------
RunEstimator::RunEstimator(const RunEstimator & rs) :
   RunSolver               (rs),
   theEstimator            (NULL),
   commandRunning          (false),
   commandComplete         (false),
   propPrepared            (false),
   estimationOffset        (rs.estimationOffset),
   bufferFilled            (false),
   currentEvent            (NULL),
   eventProcessComplete    (false),
   eventMan                (NULL)
{
   overridePropInit = true;
   publishOnStep = false;
   delayInitialization = true;
}

//------------------------------------------------------------------------------
// RunEstimator& operator=(const RunEstimator & rs)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rs The RunEstimator object that supplies properties this one needs
 *
 * @return A reference to this instance
 */
//------------------------------------------------------------------------------
RunEstimator& RunEstimator::operator=(const RunEstimator & rs)
{
   if (&rs != this)
   {
      RunSolver::operator=(rs);

      if (theEstimator)
      {
         delete theEstimator;
         theEstimator = NULL;                 // made changes by TUAN NGUYEN
      }

      commandRunning   = false;
      commandComplete  = false;
      overridePropInit = true;
      publishOnStep    = false;
      propPrepared     = false;
      estimationOffset = rs.estimationOffset;
      bufferFilled     = false;
      currentEvent     = NULL;
      eventProcessComplete = false;

      if (eventMan)
      {
         eventMan->CleanUp();
         // use delete eventMan causes a crash
      }
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase *Clone() const
//------------------------------------------------------------------------------
/**
 * Cleates a duplicate of a RunEstimator object
 *
 * @return a clone of the object
 */
//------------------------------------------------------------------------------
GmatBase *RunEstimator::Clone() const
{
   return new RunEstimator(*this);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Accesses names for referenced objects.
 *
 * @param type Type of object requested.
 *
 * @return the referenced object's name.
 */
//------------------------------------------------------------------------------
std::string RunEstimator::GetRefObjectName(const UnsignedInt type) const
{
   switch (type)
   {
      case Gmat::SOLVER:
         #ifdef DEBUG_RUN_Estimator
            MessageInterface::ShowMessage
               ("Getting RunEstimator reference solver name\n");
         #endif
         return solverName;

      default:
         ;
   }

   return RunSolver::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a referenced object of a given type and name
 *
 * @param type The object's type
 * @param name The object's name
 *
 * @return The pointer to the associated object
 */
 //------------------------------------------------------------------------------
GmatBase* RunEstimator::GetRefObject(const UnsignedInt type,
   const std::string & name)
{
   switch (type)
   {
      case Gmat::SOLVER:
         #ifdef DEBUG_RUN_Estimator
            MessageInterface::ShowMessage
               ("Getting RunEstimator reference solver\n");
         #endif
         return theEstimator;

      default:
         ;
   }

   return RunSolver::GetRefObject(type, name);
}



//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets names for referenced objects.
 *
 * @param type Type of the object.
 * @param name Name of the object.
 *
 * @return true if the name was set, false if not.
 */
//------------------------------------------------------------------------------
bool RunEstimator::SetRefObjectName(const UnsignedInt type,
                                     const std::string &name)
{
   if (type == Gmat::SOLVER)
   {
      solverName = name;
      return true;
   }

   return RunSolver::SetRefObjectName(type, name);
}



//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool RunEstimator::RenameRefObject(const UnsignedInt type,
                                    const std::string &oldName,
                                    const std::string &newName)
{
   // RunEstimator needs to know about Solver name only
   if (type != Gmat::SOLVER)
      return RunSolver::RenameRefObject(type, oldName, newName);

   if (solverName == oldName)
   {
      solverName = newName;
      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString(Gmat::WriteMode mode,
//        const std::string &prefix, const std::string &useName)
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param mode    Specifies the type of serialization requested.
 * @param prefix  Optional prefix appended to the object's name. (Used for
 *                indentation)
 * @param useName Name that replaces the object's name (Not yet used
 *                in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& RunEstimator::GetGeneratingString(Gmat::WriteMode mode,
      const std::string &prefix, const std::string &useName)
{
   generatingString = prefix + "RunEstimator " + solverName + ";";

   return RunSolver::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the command for execution
 *
 * This method prepares the Estimator and associated measurement manager and
 * measurements for the simulation process.  Referenced objects are cloned or
 * set as needed in this method.
 *
 * The propagation subsystem is prepared in the base class components of the
 * command.  RunEstimator generaqtes teh PropSetup clones at this level, but
 * leaves the rest of the initialization process for the PropSetups in the base
 * class method, which is called from this method.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RunEstimator::Initialize()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Start RunEstimator::Initialize()\n");
#endif

   // This step is used to delay to initialize until it runs Execute() function.
   // It is needed due to no observation data available before simulation. As a result,
   // no tracking configurations are auto generated for estimation. After simulation 
   // step is completed, based on simulation data, GMAT generates tracking 
   // configuration automatically for estimation step. 
   if (delayInitialization)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Exit RunEstimator::Initialize():   delay initialization to the next time\n"); 
      #endif
      return true;
   }

   // if it is initialized, does not need to do it again
   if (isInitialized)
      return true;

   bool retval = false;

   // First set the Estimator object
   if (solverName == "")
      throw CommandException("Cannot initialize RunEstimator command -- the "
            "Estimator name is not specified.");

   // Clear the old clone if it was set
   if (theEstimator != NULL)
   {
      delete theEstimator;
      theEstimator = NULL;            // made changes by TUAN NGUYEN
   }

   GmatBase *estObj = FindObject(solverName);
   if (estObj == NULL)
      throw CommandException("Cannot initialize RunEstimator command -- the "
            "Estimator named " + solverName + " cannot be found.");

   if (!estObj->IsOfType("Estimator"))
      throw CommandException("Cannot initialize RunEstimator command -- the "
            "object named " + solverName + " is not a Estimator.");

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("RunEstimator::Initialize():   step 1: create an Estimator and set value to the Estimator\n"); 
   #endif
   theEstimator = (Estimator*)(estObj->Clone());
   
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("RunEstimator::Initialize():   step 1.1: reset delay flag\n"); 
   #endif
   theEstimator->SetDelayInitialization(false);
   
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("RunEstimator::Initialize():   step 1.2: initialize the Estimator\n"); 
   #endif
   theEstimator->Initialize();

   theEstimator->TakeAction("ResetInstanceCount");
   theEstimator->TakeAction("IncrementInstanceCount"); 
   estObj->TakeAction("ResetInstanceCount");                // does it need to do it???
   estObj->TakeAction("IncrementInstanceCount");            // does it need to do it???

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("RunEstimator::Initialize():   step 2: set DATASTREAM objects to measurement manager\n");
#endif
   // Set the observation data streams for the measurement manager
   MeasurementManager *measman = theEstimator->GetMeasurementManager();
   StringArray streamList = measman->GetStreamList();
   for (UnsignedInt ms = 0; ms < streamList.size(); ++ms)
   {
      GmatBase *obj = FindObject(streamList[ms]);
      if (obj != NULL)
      {
         if (obj->IsOfType(Gmat::DATASTREAM))
         {
            DataFile *df = (DataFile*)obj;
            measman->SetStreamObject(df);
         }
      }
      else
         throw CommandException("Did not find the object named " +
               streamList[ms]);
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("RunEstimator::Initialize():   step 3: set ramped table to measurement manager\n");
   #endif
///// Check for generic approach here
   // Set the ramp table data streams for the measurement manager
   streamList = measman->GetRampTableDataStreamList();
   for (UnsignedInt ms = 0; ms < streamList.size(); ++ms)
   {
      GmatBase *obj = FindObject(streamList[ms]);
      if (obj != NULL)
      {
         if (obj->IsOfType(Gmat::DATASTREAM))
         {
            DataFile *df = (DataFile*)obj;
            measman->SetRampTableDataStreamObject(df);
         }
         else
            MessageInterface::ShowMessage("Object '%s' is not Gmat::DATASTREAM\n", obj->GetName().c_str());
      }
      else
      {
         throw CommandException("Error: Did not find the object named " + 
               streamList[ms]);
      }
   }

   // All the code of initialize was moved to PreExcecution() function

   retval = true;
   isInitialized = retval;

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Exit RunEstimator::Initialize()\n");
#endif
   return retval;
}


//--------------------------------------------------------------------------------
// void LoadSolveForsToESM()
//--------------------------------------------------------------------------------
/**
* This function is used to load all solve-for variables and store them into 
* EstimationStateManager object.
*/
//--------------------------------------------------------------------------------
void RunEstimator::LoadSolveForsToESM()
{
   #ifdef DEBUG_LOAD_SOLVEFORS
      MessageInterface::ShowMessage("RunEstimator::LoadSolveForsToEMS()  enter\n");
   #endif

   EstimationStateManager *esm = theEstimator->GetEstimationStateManager();

   // Set solve-for for all participants used in this estimator only. Solve-fors for participants in other estimator and similator are not set to ESM 
   StringArray names = theEstimator->GetMeasurementManager()->GetParticipantList();

   #ifdef DEBUG_LOAD_SOLVEFORS
      for (UnsignedInt j = 0; j < names.size(); ++j)
         MessageInterface::ShowMessage("RunEstimator::LoadSolveForsToEMS() "
               "Participants   %s\n", names[j].c_str());
   #endif

   ObjectMap objectmap = GetConfiguredObjectMap();

   for (UnsignedInt i = 0; i < names.size(); ++i)
   {
      GmatBase *obj = FindObject(names[i]);
      if (obj != NULL)
         esm->SetProperty(obj);
   }

   // Need to test extensively with this change
//   for (ObjectMap::iterator i = objectmap.begin(); i != objectmap.end(); ++i)
//   {
//      if (find(names.begin(), names.end(), (*i).first) != names.end())
//      {
//         #ifdef DEBUG_LOAD_SOLVEFORS
//            MessageInterface::ShowMessage("   Loading %s solve-fors\n",
//                  i->first.c_str());
//         #endif
//         // Set solve-for for all participants. If participant does not have solve-for defined in it,
//         // esm->SetProperty rejects the set request and return false otherwise return true
//         esm->SetProperty(i->second);
//      }
//   }

   // Scan the force model for solve-for parameters
   if (fm.size() > 0)
   {
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         #ifdef DEBUG_LOAD_SOLVEFORS
            MessageInterface::ShowMessage("   Scanning the force model %s <%p> for "
                  "solve-fors\n", fm[i]->GetName().c_str(), fm[i]);
         #endif

         if (fm[i])
         {
            StringArray solforNames = fm[i]->GetSolveForList();
            if (solforNames.size() > 0)
            {
               #ifdef DEBUG_LOAD_SOLVEFORS
                  MessageInterface::ShowMessage("   %s has %d solve-fors:\n",
                         fm[i]->GetName().c_str(), solforNames.size());
               #endif

               for (UnsignedInt j = 0; j < solforNames.size(); ++j)
               {
                  #ifdef DEBUG_LOAD_SOLVEFORS
                     MessageInterface::ShowMessage("      %s\n",
                           solforNames[j].c_str());
                  #endif
                  esm->SetProperty(solforNames[j], fm[i]);
               }
            }
         }
      }
   }

   #ifdef DEBUG_LOAD_SOLVEFORS
      MessageInterface::ShowMessage("RunEstimator::LoadSolveForsToEMS()  exit\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetPropagationProperties(PropagationStateManager *psm)
//------------------------------------------------------------------------------
/**
 * An interface used to set specific additional propagation properties as
 * needed.
 *
 * @param psm A PropagationStateManager that controls the propagation state
 *            vector
 */
//------------------------------------------------------------------------------
void RunEstimator::SetPropagationProperties(PropagationStateManager *psm)
{
   ObjectArray pObjects;
   psm->GetStateObjects(pObjects, Gmat::UNKNOWN_OBJECT);

   for (ObjectArray::iterator p = pObjects.begin(); p != pObjects.end(); ++p)
   {
      if ((*p)->IsOfType(Gmat::SPACEOBJECT))
      {
         psm->SetProperty("STM", *p);
      }
   }
}


//------------------------------------------------------------------------------
// bool PreExecution()
//------------------------------------------------------------------------------
/**
* This function plays the role as Initialize() function due to RunEstimator 
* delays initialization til before execution step.
*
* Note that: codes in the old Initialize function were moved here
*
*/
//------------------------------------------------------------------------------
bool RunEstimator::PreExecution()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Start RunEstimator::PreExecution()\n");
#endif

   bool retval = false;

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("PreExecution(): Step 1: initialize RunEstimator\n");
#endif
   if (Initialize())
   {
#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 2: re-initialize the Estimator\n");
#endif
      retval = theEstimator->Reinitialize();

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 3: set up event manager\n");
#endif
      // Find the event manager and store its pointer
      if (triggerManagers == NULL)
          throw CommandException("The Event Manager pointer was not set on the "
            "RunEstimator command");

      for (UnsignedInt i = 0; i < triggerManagers->size(); ++i)
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("RunEstimator has an TriggerManager of "
               "type %s, id %d\n",
               (*triggerManagers)[i]->GetTriggerTypeString().c_str(),
               (*triggerManagers)[i]->GetTriggerType());
         #endif
         if ((*triggerManagers)[i]->GetTriggerType() == Gmat::EVENT)
         {
            eventMan = (EventManager*)(*triggerManagers)[i];
            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("RunEstimator has an EventManager of "
                  "type %s\n", eventMan->GetTriggerTypeString().c_str());
            #endif
         }
      }
      if (eventMan == NULL)
         throw CommandException("The EventManager pointer was not set on the "
            "RunEstimator command");


#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 4: set up Propagator\n");
#endif
      // Load participant names to estimation state manager
      MeasurementManager *measman = theEstimator->GetMeasurementManager();
      EstimationStateManager *esm = theEstimator->GetEstimationStateManager();

      // Next comes the propagators
      // Clear old ones
      if (propagators.size() > 0)
      {
         for (std::vector<PropSetup*>::iterator pp = propagators.begin();
            pp != propagators.end(); ++pp)
         {
            delete (*pp);
         }
         propagators.clear();
         p.clear();
         fm.clear();
      }

      propObjectNames.clear();

      std::map<PropSetup*, StringArray> satList;
      std::map<std::string, PropSetup*> knownProps;
      
      // Setup new ones
      StringArray participants = measman->GetParticipantList();
      for (UnsignedInt i = 0; i < participants.size(); ++i)
      {
         GmatBase *sc = FindObject(participants[i]);
         if ((sc) && (sc->IsOfType("Spacecraft")))
         {
            
            ((Spacecraft*)(sc))->SetRunningCommandFlag(3);    // input value 3 for running estimation command        // made changes by TUAN NGUYEN

            PropSetup *prop = theEstimator->GetPropagator(participants[i]);
            if (prop == nullptr)
               throw CommandException("Cannot initialize RunEstimator command; the "
                     "propagator pointer requested from the Estimator " +
                     theEstimator->GetName() + " for the spacecraft " + participants[i] +
                     " is NULL.");

            std::string propName = prop->GetName();
            PropSetup *ps = nullptr;

            if (knownProps.find(propName) == knownProps.end())
            {
               ps = (PropSetup*)prop->Clone();
               ps->SetPrecisionTimeFlag(true);
               propagators.push_back(ps);

               StringArray satNames;
               satNames.push_back(participants[i]);
               satList[ps] = satNames;
               knownProps[propName] = ps;

               p.push_back(ps->GetPropagator());
               fm.push_back(ps->GetODEModel());
               eventMan->SetObject(ps);

               retval = true;
            }
            else
            {
               // One Ephem Prop per spacecraft
               if (prop->GetPropagator()->IsOfType("EphemerisPropagator"))
               {
                  ps = (PropSetup*)prop->Clone();
                  ps->SetPrecisionTimeFlag(true);
                  propagators.push_back(ps);

                  StringArray satNames;
                  satNames.push_back(participants[i]);
                  satList[ps] = satNames;
                  knownProps[propName] = ps;

                  p.push_back(ps->GetPropagator());
                  fm.push_back(ps->GetODEModel());
                  eventMan->SetObject(ps);

                  retval = true;
               }
               else
               {
                  ps = knownProps[propName];
                  StringArray satNames = satList[ps];
                  if (find(satNames.begin(), satNames.end(), participants[i]) == satNames.end())
                     (satList[ps]).push_back(participants[i]);
               }
            }

            if (ps->GetPropagator()->UsesODEModel())
               sc->TakeAction("UseSTM");
            else
               sc->TakeAction("SkipSTM");

         }
      }

      for (UnsignedInt i = 0; i < propagators.size(); ++i)
         propObjectNames.push_back(satList[propagators[i]]);

      propPrepared = false;

      measman->SetTransientForces(transientForces);
      theEstimator->SetTransientForces(transientForces);

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 5: call AddExternalStmSetting("", NULL, -1) on participant spacecrafts\n");
#endif
      #ifdef DEBUG_LOAD_SOLVEFORS
         MessageInterface::ShowMessage("Marching through the %d entry "
               "transient list\n", transientForces->size());
      #endif
      // Build spacecraft list and clear external STM settings
      std::vector<Spacecraft*> scs;
      for (UnsignedInt k = 0; k < participants.size(); ++k)
      {
         GmatBase *party = FindObject(participants[k]);
         if (party != NULL)
         {
            if (party->IsOfType(Gmat::SPACECRAFT))
            {
               scs.push_back((Spacecraft*)party);

               // made changes by TUAN NGUYEN
               //scs[scs.size()-1]->AddExternalStmSetting("", NULL, -1);                     // It causes error due to erase all prevous setting for external STM
            }
         }
      }

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 6: call AddExternalStmSetting(<solveForName>, <transientForce>, <parmID>) on participant spacecrafts\n");
#endif
      // March through transient forces and load up SC STM with their entries
      for (UnsignedInt i = 0; i < transientForces->size(); ++i)
      {
         StringArray sfl = transientForces->at(i)->GetSolveForList();
         #ifdef DEBUG_LOAD_SOLVEFORS
            MessageInterface::ShowMessage("   transientForces[%d] with name <%s> has %d solve-fors\n", i, transientForces->at(i)->GetName().c_str(),
                  sfl.size());
         #endif
         for (UnsignedInt j = 0; j < sfl.size(); ++j)
         {
            // Tell the spacecraft about STM entries needed for the STM
            #ifdef DEBUG_LOAD_SOLVEFORS
               MessageInterface::ShowMessage("      Adding %s to %d "
                     "paricipants\n", sfl[j].c_str(), participants.size());
            #endif
            for (UnsignedInt k = 0; k < scs.size(); ++k)
            {
               #ifdef DEBUG_LOAD_SOLVEFORS
                  MessageInterface::ShowMessage("         Adding to %s\n",
                        scs[k]->GetName().c_str());
               #endif
               Integer paramID = transientForces->at(i)->GetParameterID(sfl[j]);
               Integer id = scs[k]->AddExternalStmSetting(
                               sfl[j], transientForces->at(i),
                               paramID);

               // Add size of external entries to STM                                     // made changes by TUAN NGUYEN
               Integer size = transientForces->at(i)->GetEstimationParameterSize(id);     // made changes by TUAN NGUYEN
               scs[k]->SetIntegerParameter("FullSTMRowCount",                             // made changes by TUAN NGUYEN
                  scs[k]->GetIntegerParameter("FullSTMRowCount") + size);                 // made changes by TUAN NGUYEN

               transientForces->at(i)->SetStmIndex(id, paramID);
            }
         }
      }

      // Tell the spacecraft about STM entries needed for the STM
      for (UnsignedInt i = 0; i < scs.size(); ++i)
      {
         #ifdef DEBUG_LOAD_SOLVEFORS
            MessageInterface::ShowMessage("  Set STM external entries for Spacecraft %s\n",
               scs[i]->GetName().c_str());
         #endif
         // processing all plates on the spacecraft
         Spacecraft* sc = scs[i];
         std::vector<StringArray> constraints = sc->GetEqualConstrains();
         ObjectArray plates = sc->GetRefObjectArray("Plate");
         for (Integer j = 0; j < plates.size(); ++j)
         {
            // Resize STM for adding plate's solve-for variables
            Plate* pl = (Plate*)plates[j];                   // fix this bug. Index j is used for plates
            StringArray sfList = pl->GetStringArrayParameter("SolveFors");
            for (Integer k = 0; k < sfList.size(); ++k)
            {
               // Only add external entries and size to STM when solve-for is on the first entry in the constraint list
               std::string sfFullName = pl->GetName() + "." + sfList[k];
               bool addToList = true;
               for (Integer k1 = 0; k1 < constraints.size(); ++k1)
               {
                  for (Integer k2 = 1; k2 < constraints[k1].size(); ++k2)
                  {
                     if (sfFullName == constraints[k1][k2])
                     {
                        addToList = false;
                        break;
                     }
                  }

                  if (addToList == false)
                     break;
               }

               if (addToList)
               {
                  // Add external STM setting
                  Integer id = pl->GetParameterID(sfList[k]);
                  sc->AddExternalStmSetting(
                     sfFullName,                // Full name: ex: set solve-for "Plate1.DiffuseFraction" to external STM list in the spacecraft
                     pl, id);

                  // Add size of external entries to STM                            // made changes by TUAN NGUYEN
                  Integer size = pl->GetEstimationParameterSize(id);                // made changes by TUAN NGUYEN
                  sc->SetIntegerParameter("FullSTMRowCount",                        // made changes by TUAN NGUYEN
                     sc->GetIntegerParameter("FullSTMRowCount") + size);            // made changes by TUAN NGUYEN
               }
            }
         }
      }


#ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("RunEstimator command found %d "
            "participants\n", participants.size());
      #endif

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 7: load participant names to EstimationStateManager\n");
#endif
#ifdef DEBUG_LOAD_SOLVEFORS
      for (UnsignedInt j = 0; j < participants.size(); ++j)
        MessageInterface::ShowMessage("RunEstimator::PreExecution() Participants   %s\n", participants[j].c_str());
#endif
      // Load participant names to estimation state manager
      esm->SetParticipantList(participants);

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 8: initialize RunSolver\n");
#endif
      // Now we can initialize the propagation subsystem by calling up the
      // inheritance tree.
      try
      {
#ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("RunSolver::Initialize()\n");
#endif
         isInitialized = RunSolver::Initialize();
      } catch(GmatBaseException e)
      {
         MessageInterface::ShowMessage(" *** message: %s\n", e.GetDetails().c_str());
      }
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("End RunEstimator::PreExecution()\n");
#endif
   return retval;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Performs the command side processing for the Simulation
 *
 * This method calls the Estimator to determine the state of the Simulation
 * state machine and responds to that state as needed.  Typical command side
 * responses are to propagate as needed, to clean up memory, or to reset flags
 * based on the state machine.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RunEstimator::Execute()
{
   #ifdef DEBUG_STATE
     MessageInterface::ShowMessage("*** Enter RunEstimator:Execute()\n");
   #endif

   //--------------------------------------------------------------------
   // Steps to run before running Execute()
   //--------------------------------------------------------------------
   // Initialization step is moved here:
   if (delayInitialization)
   {
      // It needs to run initialization now
      delayInitialization = false;
      PreExecution();
   }


   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("\n\nThe \"%s\" command is running...\n",
            GetTypeName().c_str());
   #endif

   // Reset the command if called after it has completed execution
   // todo: Debug this piece; reentrance in a For loop doesn't work yet
//   if (commandComplete)
//      TakeAction("Reset");


   // Here we should check to see if the command is currently propagating and
   // finish that first...

   // Respond to the state in the state machine
   Solver::SolverState state = theEstimator->GetState();
   
   // Set run state here (for fixing GMT-5101 LOJ: 2015.06.16)
   if(theEstimator->IsFinalPass())
      publisher->SetRunState(Gmat::SOLVEDPASS);
   else
      publisher->SetRunState(Gmat::SOLVING);
   
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("\nEstimator state is %d\n", state);
   #endif
   
   try
   {
      switch (state)
      {
      case Solver::INITIALIZING:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): INITIALIZING state\n");
         #endif
			
         PrepareToEstimate();
         {
            EstimationStateManager *esm = theEstimator->GetEstimationStateManager();
#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Execute(): Solver::INITIALIZING: load solve-fors to EstimationStateManager\n");
#endif
            // Load solve for objects to esm
            LoadSolveForsToESM();

            // Reset the propagation state vector because the STM may have resized
            // TBD: Do we need to do this?
            // From HERE --------------------------------------
            hasFired = false;
            for (UnsignedInt i = 0; i < p.size(); ++i)
            {
               p[i]->Initialize();
               psm[i]->MapObjectsToVector();
               p[i]->Update(true);
            }
            // To HERE --------------------------------------

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Execute(): Solver::INITIALIZING: set solver-for objects to EstimationStateManager\n");
#endif
            // Pass in the objects
            StringArray objList = esm->GetObjectList("");
            for (UnsignedInt i = 0; i < objList.size(); ++i)
            {
               std::string propName = objList[i];
               std::string objName = propName;
               std::string refObjectName = "";
               size_t loc = propName.find('.');              // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
               if (loc != propName.npos)
               {
                  objName = propName.substr(0, loc);
                  refObjectName = propName.substr(loc+1);
               }

               GmatBase* obj = FindObject(objName);
               // if referent object is used, set referent object to be solve-for object
               // ex: propName = "CAN.ErrorModel1". Referent object is "ErrorModel1". It needs to set object ErrorModel1 to estimation state mananger
               if (refObjectName != "")
               {
                  GmatBase* refObj = obj->GetRefObject(Gmat::UNKNOWN_OBJECT, propName);
                  obj = refObj;
               }

               if (obj != NULL)
               {
                  if (obj->IsOfType(Gmat::ODE_MODEL))
                  {
                     // Use the internal ODEModel
                     obj = fm[0];
                     // Refresh its solvefor buffer
                     fm[0]->GetSolveForList();
                  }
#ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Execute(): Solver::INITIALIZING: object <%s,%p> set to EstimationStateManager\n", obj->GetName().c_str(), obj);
#endif
                  esm->SetObject(obj);
               }
            }

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Execute(): Solver::INITIALIZING: build state in EstimationStateManager\n");
#endif
            esm->BuildState();
            esm->MapObjectsToVector();

            UpdateInitialConditions();

            PublishState();
         }
			
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): INITIALIZING state\n");
         #endif
         break;

      case Solver::PROPAGATING:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): PROPAGATING state\n");
         #endif
         Propagate();

         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): PROPAGATING state\n");
         #endif

         break;

      case Solver::CALCULATING:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): CALCULATING state\n");
         #endif
         Calculate();
			
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): CALCULATING state\n");
         #endif

         break;

      case Solver::LOCATING:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): LOCATING state\n");
         #endif
         LocateEvent();
			
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): LOCATING state\n");
         #endif
         break;

      case Solver::ACCUMULATING:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): ACCUMULATING state\n");
         #endif
         Accumulate();
			
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): ACCUMULATING state\n");
         #endif
         break;

      case Solver::ESTIMATING:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): ESTIMATING state\n");
         #endif
         Estimate();
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): ESTIMATING state\n");
         #endif
         break;

      case Solver::CHECKINGRUN:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): CHECKINGRUN state\n");
         #endif
         CheckConvergence();
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): CHECKINGRUN state\n");
         #endif
         break;

      case Solver::FINISHED:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): FINISHED state\n");
         #endif

         // Set run state to SOLVEDPASS here (for fixing GMT-5101 LOJ: 2015.06.16)
         publisher->SetRunState(Gmat::SOLVEDPASS);

         for (UnsignedInt i = 0; i < fm.size(); ++i)
         {
            if (fm[i])
            {
               fm[i]->UpdateFromSpaceObject();
               fm[i]->TakeAction("UpdateSpacecraftParameters");
            }
            else
            {
               p[i]->UpdateFromSpaceObject();
               p[i]->TakeAction("UpdateSpacecraftParameters");
            }
         }

         // Publish the final state
         PublishState();

         // Why is Finalize commented out???  There is no command summary because of this change.
         // Finalize();
         // Adding in for now.
         BuildCommandSummary(true);
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): FINISHED state\n");
         #endif
         break;

      default:
         throw CommandException("Unknown state "
               " encountered in the RunEstimator command");
      }

      #ifdef DEBUG_STATE
         MessageInterface::ShowMessage("*** Start AdvanceState ... RunEstimator:Execute()\n");
      #endif

      if (state != Solver::FINISHED)
         state = theEstimator->AdvanceState(); // ekf mod 12/16
      else
      {
         // It has to run all work in AdvanceState() before Finalize()
         state = theEstimator->AdvanceState();  // ekf mod 12/16
         Finalize();
      }
   } catch (...)//(EstimatorException ex1)
   {
      Finalize();
      throw; // ex1;
   }

   #ifdef DEBUG_STATE
      MessageInterface::ShowMessage("*** Exit RunEstimator:Execute()\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
/**
 * Completes processing so that subsequent mission runs can be executed.
 */
//------------------------------------------------------------------------------
void RunEstimator::RunComplete()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::RunComplete()\n");
   #endif
   //commandRunning = false;                        // Fix bug GMT-5818 Batch estimation stop and start error

   RunSolver::RunComplete();
   commandComplete = true;                          // Fix bug GMT-5818 Batch estimation stop and start error
   commandRunning = false;                          // Fix bug GMT-5818 Batch estimation stop and start error
   propPrepared = false;                            // Fix bug GMT-5818 Batch estimation stop and start error

   overridePropInit = true;                         // Fix bug GMT-5818 Batch estimation stop and start error
   delayInitialization = true;                      // Fix bug GMT-5818 Batch estimation stop and start error

   try
   {
      if (eventMan)                                  // made changes by TUAN NGUYEN
      {                                              // made changes by TUAN NGUYEN
         eventMan->CleanUp();                        // made changes by TUAN NGUYEN

         // This causes crash. Use CleanUp() instead // made changes by TUAN NGUYEN
         //delete eventMan;                          // made changes by TUAN NGUYEN
         //eventMan = NULL;                          // made changes by TUAN NGUYEN
      }                                              // made changes by TUAN NGUYEN
   }
   catch (...)
   {
      MessageInterface::PopupMessage(Gmat::WARNING_, "Error: EventManager::CleanUp() has error.\n");
   }

   
   try
   {
      if (theEstimator)                              // made changes by TUAN NGUYEN
      {                                              // made changes by TUAN NGUYEN
         delete theEstimator;                        // made changes by TUAN NGUYEN
         theEstimator = NULL;                        // made changes by TUAN NGUYEN
      }                                              // made changes by TUAN NGUYEN
   }
   catch (...)
   {
      MessageInterface::PopupMessage(Gmat::WARNING_, "Error: ~Estimator() has error.\n");
   }

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::RunComplete()\n");
   #endif

}


//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * Performs actions at prompting from higher level structures
 *
 * @param action The action that needs to be taken
 * @param actionData Optional additional data the action needs
 *
 * @return true if an action was taken, false if not
 */
//------------------------------------------------------------------------------
bool RunEstimator::TakeAction(const std::string &action,
                              const std::string &actionData)
{
   if (action == "Reset")
   {
      theEstimator->TakeAction("Reset");
      commandRunning = false;
      commandComplete = false;
      return true;
   }

   return RunSolver::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// GmatCommand* GetNext()
//------------------------------------------------------------------------------
/**
 * Retrieves the pointer to the next command that the Sandbox needs to run
 *
 * This method returns a pointer to the current RunEstimator command while the
 * simulation state machine is running.  It returns the next pointer after the
 * simulation has finished execution.
 *
 * @return The next command that should Execute()
 */
//------------------------------------------------------------------------------
GmatCommand* RunEstimator::GetNext()
{
   if (commandRunning)
      return this;
   return next;
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
bool RunEstimator::HasLocalClones()
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
void RunEstimator::UpdateClonedObject(GmatBase *obj)
{
   if (obj->IsOfType("Spacecraft"))
      return;
   throw CommandException("To do: implement RunEstimator::UpdateClonedObject "
         "for " + obj->GetTypeName() + " objects");
}

//------------------------------------------------------------------------------
// Methods triggered by the finite state machine
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void PrepareToEstimate()
//------------------------------------------------------------------------------
/**
 * Responds to the INITIALIZING state of the finite state machine
 *
 * Performs the final stages of initialization that need to be performed prior
 * to running the estimation.  This includes the final ODEModel preparation and
 * the setting for the flags that indicate that an estimation is in process.
 */
//------------------------------------------------------------------------------
void RunEstimator::PrepareToEstimate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage(
            "Entered RunEstimator::PrepareToEstimate()\n");
   #endif

   if (!propPrepared)
   {
      // Re-register publisher now that STM is added to ForceModel
      publisher->UnregisterPublishedData(this);
      streamID = -1;

      StringArray owners, elements;
      /// @todo Check to see if All and All.Epoch belong for all modes.
      owners.push_back("All");
      elements.push_back("All.epoch");

      for (UnsignedInt ii = 0U; ii < sats.size(); ii++)
         if (sats[ii]->GetType() != Gmat::FORMATION)
            SetNames(sats[ii]->GetName(), owners, elements);

      streamID = publisher->RegisterPublishedData(this, streamID, owners, elements);

      PrepareToPropagate(false);  // ?? Test return value here?

      // Prepare to publish state
      if (pubdata)
         delete[] pubdata;
      pubdata = new Real[dim + 21 + 1];

      commandRunning  = true;
      commandComplete = false;
      propPrepared    = true;

      // Warn that the attitude is not updated at each propagation intermediate step [GMT-4398]
      // FIXME: Fix this when PropagationEnabledCommand is refactored
      // Check if the attitude affects the dynamics of any force model
      bool attAffectDyn = false;
      for (UnsignedInt n = 0U; n < propagators.size(); n++)
      {
         ODEModel *odem = propagators[n]->GetODEModel();
         if (odem)
         {
            for (UnsignedInt ii = 0U; ii < odem->GetNumForces(); ii++)
            {
               PhysicalModel* f = odem->GetForce(ii);

               if (f->AttitudeAffectsDynamics())
               {
                  attAffectDyn = true;
                  break;
               }
            }
         }

         if (attAffectDyn)
            break;
      }

      // If the attitude of the Spacecraft affects the force model,
      // check if the Spacecraft uses an ObjectReferencedAxes
      bool objRefAxes = false;
      if (attAffectDyn)
      {
         for (std::vector<GmatBase *>::const_iterator obj = sats.begin();
            obj != sats.end(); ++obj)
         {
            if ((*obj)->IsOfType("Spacecraft"))
            {
               GmatBase* att = (*obj)->GetRefObject(Gmat::ATTITUDE, "");

               if (att->IsOfType("NadirPointing"))
               {
                  objRefAxes = true;
               }
               else
               {
                  std::string refCSName = att->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
                  CoordinateSystem* cs = (CoordinateSystem*) att->GetRefObject(Gmat::COORDINATE_SYSTEM, refCSName);
                  AxisSystem* ax = cs->GetAxisSystem();

                  if (ax->IsOfType("ObjectReferencedAxes"))
                     objRefAxes = true;
               }
            }
         }
      }

      if (objRefAxes)
         MessageInterface::ShowMessage("Warning: A Spacecraft in the Estimator is using NadirPointing or "
            "an ObjectReferenced axes for its attitude with a force model that has dynamics affected by the attitude. "
            "The attitude is not updated at intermediate steps in the integrator, so the result may be inaccurate.\n");
      // End PropagationEnabledCommand FIXME
   }

   for (UnsignedInt i = 0; i < fm.size(); ++i)
   {
      if (fm[i])
      {
         estimationOffset = fm[i]->GetTime();
         // @todo Temporary -- Turn off range check for Cr.  This needs to be made conditional,
         // and only active if Cr is a solve-for
         fm[i]->TakeAction("SolveForCr");
      }
   }

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage(
            "Exit RunEstimator::PrepareToEstimate()\n");
   #endif

   #ifdef DEBUG_INITIAL_STATE
      MessageInterface::ShowMessage("Object states at close of PrepareToEstimate:\n");
      for (UnsignedInt i = 0; i < propObjects.size(); ++i)
      {
         PropObjectArray *poa = propObjects[i];
         for (UnsignedInt j = 0; j < poa->size(); ++j)
         {
            MessageInterface::ShowMessage("   %s:\n", poa->at(j)->GetName().c_str());
            if (poa->at(j)->IsOfType(Gmat::SPACEOBJECT))
            {
               MessageInterface::ShowMessage("      Epoch: [%s]\n",
                     poa->at(j)->GetStringParameter("Epoch").c_str());
               MessageInterface::ShowMessage("      EpochGT: [%s]\n",
                  ((SpaceObject*)poa->at(j))->GetEpochGT().ToString().c_str());
               MessageInterface::ShowMessage("      [%16.14lf, %16.14lf, %16.14lf]:\n",
                     poa->at(j)->GetRealParameter("X"),
                     poa->at(j)->GetRealParameter("Y"),
                     poa->at(j)->GetRealParameter("Z"));
               MessageInterface::ShowMessage("      [%16.14lf, %16.14lf, %16.14lf]:\n",
                     poa->at(j)->GetRealParameter("VX"),
                     poa->at(j)->GetRealParameter("VY"),
                     poa->at(j)->GetRealParameter("VZ"));
               
               
               Rmatrix stm = poa->at(j)->GetRmatrixParameter("STM");
               MessageInterface::ShowMessage("STM = [\n");
               for (Integer row = 0; row < stm.GetNumRows(); ++row)
               {
                  for (Integer col = 0; col < stm.GetNumColumns(); ++col)
                     MessageInterface::ShowMessage("%.15le   ", stm(row,col));
                  MessageInterface::ShowMessage("]\n");
               }
               MessageInterface::ShowMessage("]\n");
            }
            else
               MessageInterface::ShowMessage("      Not a SpaceObject\n");
         }
      }
   #endif
}


//------------------------------------------------------------------------------
// void Propagate()
//------------------------------------------------------------------------------
/**
 * Responds to the PROPAGATING state of the finite state machine.
 *
 * Propagation from the current epoch to the next estimation epoch is performed
 * in this method.
 */
//------------------------------------------------------------------------------
void RunEstimator::Propagate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Propagate()\n");
   #endif

   // If state reset at current epoch - e.g. during sequential estimation -
   // reload the prop vector
   if (theEstimator->ResetState())
   {
      #ifdef DEBUG_STATE_RESETS
         MessageInterface::ShowMessage("Calling UpdateFromSpaceObject()\n");
         Real* oldState = fm[0]->GetState();
         MessageInterface::ShowMessage("   Old state:  (%.12lf   %.12lf   %.12lf)\n", oldState[0], oldState[1], oldState[2]);
      #endif
      
      fm[0]->UpdateFromSpaceObject();
      fm[0]->TakeAction("UpdateSpacecraftParameters");

      // Publish the new state
      PublishState();
      
      #ifdef DEBUG_STATE_RESETS
         Real* newState = fm[0]->GetState();
         MessageInterface::ShowMessage("   New state:  (%.12lf   %.12lf   %.12lf\n", newState[0], newState[1], newState[2]);
      #endif
   }

   // If state reset to initial epoch - e.g. during batch estimation -
   // reload prop vector and reset the epoch information
   if (startNewPass == true)
   {
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         if (fm[i])
         {
            fm[i]->UpdateFromSpaceObject();
            fm[i]->SetTime(estimationOffset);
            fm[i]->TakeAction("UpdateSpacecraftParameters");
         }
         else
         {
            p[i]->UpdateFromSpaceObject();
            p[i]->SetTime(estimationOffset);
            p[i]->TakeAction("UpdateSpacecraftParameters");
         }
      }
      startNewPass = false;
   }
   
   Real dt = theEstimator->GetTimeStep();
   
   /// @note Wnen estimators allow multiple prop settings, adapt this code
   Real maxStep = fabs(maxSteps[0]);

   #ifdef DEBUG_STEP_CONTROL
      MessageInterface::ShowMessage("Estimator step = %.12lf; \n", dt);
   #endif

   if (fabs(dt) > maxStep)
      dt = (dt > 0.0 ? maxStep : -maxStep);

   #ifdef DEBUG_STEP_CONTROL
      MessageInterface::ShowMessage("Stepping by %.12lf; could be %lf\n", dt, p[0]->GetStepSize());
   #endif

   #ifdef DEBUG_INITIAL_STATE
      MessageInterface::ShowMessage("Stepping by %.12le seconds\n", dt);
   #endif

   #ifdef DEBUG_EVENT_STATE
      dim = fm[0]->GetDimension();
      Real *b4State = fm[0]->GetState();

		MessageInterface::ShowMessage("State before prop: dim = %d   fm[0]->GetState() = <%p>\n  ", dim, b4State);
      fm[0]->ReportEpochData();
      MessageInterface::ShowMessage("  ");
      for (Integer i = 0; i < dim; ++i)
         MessageInterface::ShowMessage("   %.12le", b4State[i]);
      MessageInterface::ShowMessage("\n");
   #endif

   Step(dt);
   bufferFilled = false;
   
   theEstimator->UpdateCurrentEpoch(currEpochGT[0]);
   
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::Propagate()\n");
   #endif

   #ifdef DEBUG_EVENT_STATE
      dim = fm[0]->GetDimension();
      Real *odeState = fm[0]->GetState();

		MessageInterface::ShowMessage("State after prop: epoch = %s   fm[0]->GetState() = <%p>\n", currEpochGT[0].ToString().c_str(), fm[0]->GetState());
      fm[0]->ReportEpochData();
      MessageInterface::ShowMessage("  ");
      for (Integer i = 0; i < dim; ++i)
         MessageInterface::ShowMessage("   %.12le", odeState[i]);
      MessageInterface::ShowMessage("\n");
   #endif

}


//------------------------------------------------------------------------------
// void Calculate()
//------------------------------------------------------------------------------
/**
 * Responds to the CALCULATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunEstimator::Calculate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Calculate() "
            "epoch = %.12lf\n", currEpoch[0]);
   #endif

   bufferFilled = false;

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::Calculate()\n");
   #endif
}


//------------------------------------------------------------------------------
// void LocateEvent()
//------------------------------------------------------------------------------
/**
 * Responds to the LOCATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunEstimator::LocateEvent()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::LocateEvent()\n");
   #endif

//   bool newEvent = false;
   EventStatus status;

   // First time through, buffer the objects that get propagated
   if (!bufferFilled)
   {
      if (fm[0])
         dt = fm[0]->GetTime();
      else
         dt = p[0]->GetTime();

      #ifdef DEBUG_EVENT_STATE
         Integer dim = fm[0]->GetDimension();
         Real *odeState = fm[0]->GetState();

         MessageInterface::ShowMessage("State at event location start:\n  ");
         fm[0]->ReportEpochData();
         MessageInterface::ShowMessage("  dt = %.12lf\n  ", dt);
         for (Integer i = 0; i < dim; ++i)
            MessageInterface::ShowMessage("   %.12lf", odeState[i]);
         MessageInterface::ShowMessage("\n");
      #endif
      BufferSatelliteStates(true);
      eventMan->ClearObject(NULL);

      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("   Buffer has %d objects; contents:\n",
               satBuffer.size());

         for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
              i != satBuffer.end(); ++i)
         {
            MessageInterface::ShowMessage(
                  "   Epoch of '%s' is %.12lf\n", (*i)->GetName().c_str(),
                  (*i)->GetRealParameter("A1Epoch"));
            MessageInterface::ShowMessage(
                  "   Position is [%.12lf  %.12lf  %.12lf]\n",
                  (*i)->GetRealParameter("X"), (*i)->GetRealParameter("Y"),
                  (*i)->GetRealParameter("Z"));
         }
      #endif

      bufferFilled = true;
      eventList.clear();
      eventList = theEstimator->GetRefObjectArray(Gmat::EVENT);

      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("Event list has %d events\n",
               eventList.size());
      #endif
      // Set status of the events to ITERATING
      for (UnsignedInt i = 0; i < eventList.size(); ++i)
      {
         ((Event*)eventList[i])->CheckStatus(ITERATING);
         ((Event*)eventList[i])->Initialize();
      }

      if (eventList.size() == 0)
      {
         currentEvent = NULL;
         #ifdef DEBUG_EXECUTION
            MessageInterface::ShowMessage("   No events were found\n");
         #endif
         return;
      }
      else
      {
         currentEvent = ((Event*)eventList[0]);
//         newEvent = true;
         eventIndex = 0;

       // Will need to be updated when multiple propagators are enabled:
         eventMan->SetObject(propagators[0]);
         eventMan->SetObject(currentEvent);
         eventMan->SetFixedState(currentEvent);
         
       // Reset the state data to the starting states
         BufferSatelliteStates(false);
         propagators[0]->GetODEModel()->UpdateFromSpaceObject();
         fm[0]->SetTime(dt);

         #ifdef DEBUG_EVENT_LOCATION
            MessageInterface::ShowMessage("   Locating Event %d, of type %s\n",
                  eventIndex, currentEvent->GetTypeName().c_str());
         #endif
      }

      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("Setting flag so state data will "
               "reset\n");
      #endif

      eventProcessComplete = true;
   }

   if (currentEvent == NULL)
      return;

   status = currentEvent->CheckStatus();

   if (status == LOCATED)
   {
      #ifdef DEBUG_EVENT_LOCATION
         MessageInterface::ShowMessage("Located the event %s (index %d)\n",
               currentEvent->GetTypeName().c_str(), eventIndex);
      #endif

      ++eventIndex;
      if (eventIndex < eventList.size())
      {
         #ifdef DEBUG_EVENT_LOCATION
            MessageInterface::ShowMessage("   Locating Event %d, of type %s\n",
                  eventIndex, currentEvent->GetTypeName().c_str());
         #endif

         // Prepare to work with the next active event
         eventMan->ClearObject(currentEvent);
         currentEvent = (Event*)eventList[eventIndex];
//         newEvent = true;

         // Eventually, set propagators associated with specific events here
         eventMan->SetObject(currentEvent);
         eventMan->SetFixedState(currentEvent);
         // Reset the state data to the starting states
         BufferSatelliteStates(false);
         propagators[0]->GetODEModel()->UpdateFromSpaceObject();
         fm[0]->SetTime(dt);

         #ifdef DEBUG_EXECUTION
            MessageInterface::ShowMessage("   Event %d, of type %s\n", eventIndex,
                  currentEvent->GetTypeName().c_str());
         #endif
      }
      else
      {
         eventMan->ClearObject(currentEvent);
         currentEvent = NULL;
         #ifdef DEBUG_EVENT_STATE
            Integer dim = fm[0]->GetDimension();
            Real *odeState = fm[0]->GetState();

            MessageInterface::ShowMessage("State at event location end:\n  ");
            fm[0]->ReportEpochData();
            MessageInterface::ShowMessage("  ");
            for (Integer i = 0; i < dim; ++i)
               MessageInterface::ShowMessage("   %.12lf", odeState[i]);
            MessageInterface::ShowMessage("\n");
         #endif

      }
   }

   if (currentEvent != NULL)
   {
      #ifdef DEBUG_EVENT_LOCATION
         MessageInterface::ShowMessage("   Finding root for the event\n");
      #endif
      eventMan->FindRoot(0);
   }

   // Reset the state data to the starting states
   BufferSatelliteStates(false);
   propagators[0]->GetODEModel()->UpdateFromSpaceObject();
   fm[0]->SetTime(dt);

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::LocateEvent()\n");
   #endif

}



//------------------------------------------------------------------------------
// void Accumulate()
//------------------------------------------------------------------------------
/**
 * Performs command side actions taken during accumulation
 */
//------------------------------------------------------------------------------
void RunEstimator::Accumulate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Accumulate()\n");
   #endif

   CleanUpEvents();

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::Accumulate()\n");
   #endif
}


//------------------------------------------------------------------------------
// void Estimate()
//------------------------------------------------------------------------------
/**
 * Responds to the ESTIMATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunEstimator::Estimate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Estimate()\n");
   #endif

   CleanUpEvents();

   PublishState();
   publisher->FlushBuffers(true);
   publisher->SetSpacecraftPropertyChanged(this, currEpochGT[0].GetMjd(), sats[0]->GetName(), "RunEstimator");

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::Estimate()\n");
   #endif
}

//------------------------------------------------------------------------------
// void CheckConvergence()
//------------------------------------------------------------------------------
/**
 * Performs command actions needed when testing for convergence
 */
//------------------------------------------------------------------------------
void RunEstimator::CheckConvergence()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::CheckConvergence() "
            "epoch = %.12lf\n", currEpoch[0]);
   #endif

   startNewPass = true;

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::CheckConvergence()\n");
   #endif
}

//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
/**
 * Responds to the FINALIZING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunEstimator::Finalize()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Finalize() "
            "epoch = %.12lf\n", currEpoch[0]);
   #endif

   // Finalize the Estimator
   if (theEstimator->Finalize() == false)
      MessageInterface::ShowMessage(
            "The Estimator %s reported a problem completing processing\n",
            theEstimator->GetName().c_str());
   
   BuildCommandSummary(true);
   
   commandComplete = true;
   commandRunning  = false;
   propPrepared    = false;

   overridePropInit = true;
   delayInitialization = true;

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::Finalize()\n");
   #endif
}


//------------------------------------------------------------------------------
// void CleanUpEvents()
//------------------------------------------------------------------------------
/**
 * Restores the buffered satellite states after event processing
 */
//------------------------------------------------------------------------------
void RunEstimator::CleanUpEvents()
{
   // If events were processed, we need to reset the state data
   if (eventProcessComplete == true)
   {
      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("Resetting state data after event "
               "location\n");
      #endif

      // Reset the state data to the starting states
      BufferSatelliteStates(false);
      propagators[0]->GetODEModel()->UpdateFromSpaceObject();
      eventProcessComplete = false;

      #ifdef DEBUG_EVENT_STATE
         Integer dim = fm[0]->GetDimension();
         Real *odeState = fm[0]->GetState();

         MessageInterface::ShowMessage("State in the Accumulate method:\n  ");
         fm[0]->ReportEpochData();
         MessageInterface::ShowMessage("  ");
         for (Integer i = 0; i < dim; ++i)
            MessageInterface::ShowMessage("   %.12lf", odeState[i]);
         MessageInterface::ShowMessage("\n");
      #endif
   }
}


//------------------------------------------------------------------------------
// void PublishState()
//------------------------------------------------------------------------------
/**
 * Publishes the state to the publisher if ready.
 */
 //------------------------------------------------------------------------------
void RunEstimator::PublishState()
{
   EstimationStateManager *esm = theEstimator->GetEstimationStateManager();

   if (theEstimator->IsOfType("SeqEstimator") && esm->HasStateOffset())
   {
      // Include the state offset in the subscribers
      esm->MapObjectsToVector();
      esm->MapFullVectorToObjects();
      fm[0]->UpdateFromSpaceObject();
   }

   pubdata[0] = currEpochGT[0].GetMjd();
   Integer index = 1, theDim;
   for (UnsignedInt i = 0; i < fm.size(); ++i)
   {
      if (fm[i])
      {
         j2kState = fm[i]->GetJ2KState();
         theDim = fm[i]->GetDimension();
      }
      else
      {
         j2kState = p[i]->GetJ2KState();
         theDim = p[i]->GetDimension();
      }
      memcpy(&pubdata[index], j2kState, theDim * sizeof(Real));
      index += theDim;
   }

   Covariance *stateCovariance = esm->GetCovariance();
   Rmatrix cov = *(stateCovariance->GetCovariance());
   Rmatrix dX_dS = esm->CartToSolveForStateConversionDerivativeMatrix();
   Rmatrix covCart = dX_dS * cov * dX_dS.Transpose();

   UnsignedInt idx = 0;
   for (UnsignedInt ii = 0; ii < 6; ii++)
   {
      for (UnsignedInt jj = 0; jj <= ii; jj++)
      {
         pubdata[dim + 1 + idx] = covCart(ii, jj);
         idx++;
      }
   }

   publisher->Publish(this, streamID, pubdata, dim + 21 + 1, direction);

   if (theEstimator->IsOfType("SeqEstimator") && esm->HasStateOffset())
   {
      // Remove the state offset from the reference trajectory
      esm->MapVectorToObjects();
      fm[0]->UpdateFromSpaceObject();
   }
}


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
void RunEstimator::SetNames(const std::string& name, StringArray& owners,
   StringArray& elements)
{
   // Need the size of elements StringArray before the states are added
   Integer initElementSize = elements.size();

   // Populate X, Y, Z, Vx, Vy, Vz
   RunSolver::SetNames(name, owners, elements);

   // Buffer for additional states (e.g. STM) in the forcemodel
   Integer fdim;
   if (fm[0])
      fdim = fm[0]->GetDimension();
   else
      fdim = p[0]->GetDimension();

   Integer startdim = elements.size();
   startdim -= initElementSize;

   for (Integer i = startdim; i < fdim; ++i)
   {
      owners.push_back(name);
      elements.push_back(name + ".");
   }

   std::string names[6] = { "X", "Y", "Z", "Vx", "Vy", "Vz" };

   for (UnsignedInt ii = 0; ii < 6; ii++)
   {
      for (UnsignedInt jj = 0; jj <= ii; jj++)
      {
         owners.push_back(name);
         std::string elementName = name + ".C" + names[ii] + names[jj];
         elements.push_back(elementName);
      }
   }

#ifdef DEBUG_PUBLISH_DATA
   MessageInterface::ShowMessage
   ("RunEstimator::SetNames() Setting data labels:\n");
   for (unsigned int i = 0; i < elements.size(); i++)
      MessageInterface::ShowMessage("%s ", elements[i].c_str());
   MessageInterface::ShowMessage("\n");
#endif
}


//------------------------------------------------------------------------------
// void UpdateInitialConditions()
//------------------------------------------------------------------------------
/**
 * Publishes the state to the publisher if ready.
 */
 //------------------------------------------------------------------------------
void RunEstimator::UpdateInitialConditions()
{
   if (theEstimator->UpdateInitialConditions())
   {
      EstimationStateManager *esm = theEstimator->GetEstimationStateManager();
      GmatTime epochGT = esm->GetEstimationEpochGT();

      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         baseEpochGT[i] = epochGT;
         elapsedTime[i] = 0.0;
         currEpochGT[i] = epochGT;

         if (fm[i])
         {
            fm[i]->UpdateFromSpaceObject();
            fm[i]->TakeAction("UpdateSpacecraftParameters");
         }
         else
         {
            p[i]->UpdateFromSpaceObject();
            p[i]->TakeAction("UpdateSpacecraftParameters");
         }
      }
   }
}
