//$Id$
//------------------------------------------------------------------------------
//                         RunSmoother
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
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2019/07/25
//
/**
 * Implementation of the Mission Control Sequence command that drives smoothing
 */
//------------------------------------------------------------------------------


#include "RunSmoother.hpp"

//#define DEBUG_INITIALIZATION
//#define DEBUG_LOAD_SOLVEFORS


//------------------------------------------------------------------------------
// RunSmoother()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
RunSmoother::RunSmoother() :
   RunEstimator            ("RunSmoother")
{
}


//------------------------------------------------------------------------------
// ~RunSmoother()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RunSmoother::~RunSmoother()
{
}


//------------------------------------------------------------------------------
// RunSmoother(const RunSmoother & rs)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rs The command that is copied into the new one
 */
//------------------------------------------------------------------------------
RunSmoother::RunSmoother(const RunSmoother & rs) :
   RunEstimator            (rs)
{
}

//------------------------------------------------------------------------------
// RunSmoother& operator=(const RunSmoother & rs)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rs The RunSmoother object that supplies properties this one needs
 *
 * @return A reference to this instance
 */
//------------------------------------------------------------------------------
RunSmoother& RunSmoother::operator=(const RunSmoother & rs)
{
   if (&rs != this)
   {
      RunEstimator::operator=(rs);
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase *Clone() const
//------------------------------------------------------------------------------
/**
 * Cleates a duplicate of a RunSmoother object
 *
 * @return a clone of the object
 */
//------------------------------------------------------------------------------
GmatBase *RunSmoother::Clone() const
{
   return new RunSmoother(*this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the command for execution
 *
 * This method prepares the Smoother and associated measurement manager and
 * measurements for the simulation process. This method also finds the instance
 * of the forward filter used by the smoother in order to obtain the filter data
 * from it. Referenced objects are cloned or set as needed in this method.
 *
 * @return true on success, false on failure
 */
 //------------------------------------------------------------------------------
bool RunSmoother::Initialize()
{
   bool retval = RunEstimator::Initialize();

   if (theEstimator)
   {
      // Get the name of the SeqEstimator used in the Smoother
      std::string filterName = theEstimator->GetRefObjectName(GmatType::GetTypeId("SeqEstimator"));
      GmatCommand *prevCmd = GetPrevious();

      while (prevCmd)
      {
         // If this command is an estimator, check if it uses a SeqEstimator
         if (prevCmd->IsOfType("RunEstimator"))
         {
            GmatBase *obj = prevCmd->GetRefObject(Gmat::SOLVER, filterName);

            if (obj && obj->IsOfType("SeqEstimator"))
            {
               std::vector<SeqEstimator::UpdateInfoType> forwardFilterInfo;

               // Get the filter statistics from this instance.
               forwardFilterInfo = ((SeqEstimator*) obj)->GetUpdateStats();
               ((Smoother*) theEstimator)->SetForwardFilterInfo(forwardFilterInfo);
               break;
            }
         }

         prevCmd = prevCmd->GetPrevious();
      }
   }

   return retval;
}


//--------------------------------------------------------------------------------
// void LoadSolveForsToFilterESM()
//--------------------------------------------------------------------------------
/**
* This function is used to load all solve-for variables and store them into
* the smoother's filter's EstimationStateManager object.
*/
//--------------------------------------------------------------------------------
void RunSmoother::LoadSolveForsToFilterESM()
{
#ifdef DEBUG_LOAD_SOLVEFORS
   MessageInterface::ShowMessage("RunSmoother::LoadSolveForsToFilterESM()  enter\n");
#endif

   EstimationStateManager *esm = ((Smoother*)theEstimator)->GetFilter()->GetEstimationStateManager();

   // Set solve-for for all participants used in this estimator only. Solve-fors for participants in other estimator and similator are not set to ESM 
   StringArray names = ((Smoother*)theEstimator)->GetFilter()->GetMeasurementManager()->GetParticipantList();

#ifdef DEBUG_LOAD_SOLVEFORS
   for (UnsignedInt j = 0; j < names.size(); ++j)
      MessageInterface::ShowMessage("RunSmoother::LoadSolveForsToFilterESM() "
         "Participants   %s\n", names[j].c_str());
#endif

   ObjectMap objectmap = GetConfiguredObjectMap();

   for (UnsignedInt i = 0; i < names.size(); ++i)
   {
      GmatBase *obj = FindObject(names[i]);
      if (obj != NULL)
         esm->SetProperty(obj);
   }

   // Scan the force model for solve-for parameters
   if (fm.size() > 0)
   {
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
#ifdef DEBUG_LOAD_SOLVEFORS
         MessageInterface::ShowMessage("   Scanning the force model %s <%p> for "
            "solve-fors\n", fm[i]->GetName().c_str(), fm[i]);
#endif

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

#ifdef DEBUG_LOAD_SOLVEFORS
   MessageInterface::ShowMessage("RunSmoother::LoadSolveForsToFilterESM()  exit\n");
#endif
}


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
void RunSmoother::PrepareToEstimate()
{
   // Now prepare the smoother's SeqEstimator

   EstimationStateManager *esm = ((Smoother*) theEstimator)->GetFilter()->GetEstimationStateManager();
#ifdef DEBUG_INITIALIZATION
MessageInterface::ShowMessage("Execute(): Solver::INITIALIZING: load solve-fors to EstimationStateManager\n");
#endif
   // Load filter solve for objects to esm
   LoadSolveForsToFilterESM();

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
         else if (obj->IsOfType(Gmat::SPACECRAFT))                    // made changes by TUAN NGUYEN
         {                                                            // made changes by TUAN NGUYEN
            // Tell spacecraft that it runs in estimation command 
            // in order to disable to display error messages when Plate.AreaCoefficient is negative.
            ((Spacecraft*)obj)->SetRunningCommandFlag(3);   // input value 3 for running estimation command    // made changes by TUAN NGUYEN
         }                                                            // made changes by TUAN NGUYEN

         esm->SetObject(obj);
      }
   }

   esm->BuildState();

   ((Smoother*)theEstimator)->PrepareFilter();

   RunEstimator::PrepareToEstimate();
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
void RunSmoother::Propagate()
{
   Smoother* theSmoother = (Smoother*) theEstimator;
   Smoother::SmootherState SmootherState = theSmoother->GetSmootherState();

   if (SmootherState == Smoother::SmootherState::FILTERING || SmootherState == Smoother::SmootherState::PREDICTING)
      RunEstimator::Propagate();
   else
   {
      fm[0]->UpdateFromSpaceObject();
      fm[0]->TakeAction("UpdateSpacecraftParameters");
      currEpochGT[0] = theSmoother->GetCurrentEpoch();
      currEpoch[0] = currEpochGT[0].GetMjd();

      PublishState();

      theSmoother->MoveToNext(false);
      currEpochGT[0] = theSmoother->GetCurrentEpoch();
      currEpoch[0] = currEpochGT[0].GetMjd();
      fm[0]->UpdateFromSpaceObject();
   }
}
