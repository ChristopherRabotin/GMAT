//$Id: SequentialEstimator.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         SequentialEstimator
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
// Created: 2009/09/03
//
/**
 * Base class for sequential estimation
 */
//------------------------------------------------------------------------------


#include "SequentialEstimator.hpp"
#include "GmatConstants.hpp"
#include <sstream>
#include "MessageInterface.hpp"
#include "EstimatorException.hpp"


//#define DEBUG_INITIALIZATION

// Macros for debugging of the state machine
//#define WALK_STATE_MACHINE
#define DUMP_RESIDUALS

#define DEFAULT_POSITION_COVARIANCE 100.0
#define DEFAULT_VELOCITY_COVARIANCE 0.0001
#define DEFAULT_OTHER_COVARIANCE    1.0


//------------------------------------------------------------------------------
// SequentialEstimator(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The subtype of the derived class
 * @param name The name of the instance
 */
//------------------------------------------------------------------------------
SequentialEstimator::SequentialEstimator(const std::string &type,
      const std::string &name) :
   Estimator         (type, name),
   measCovariance    (NULL)
{
   hiLowData.push_back(&sigma);
   showErrorBars = true;
}


//------------------------------------------------------------------------------
// ~SequentialEstimator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SequentialEstimator::~SequentialEstimator()
{
}


//------------------------------------------------------------------------------
// SequentialEstimator::SequentialEstimator(const SequentialEstimator & se) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param se The estimator that provides configuration information for this one
 */
//------------------------------------------------------------------------------
SequentialEstimator::SequentialEstimator(const SequentialEstimator & se) :
   Estimator         (se),
   measCovariance    (NULL)
{
   hiLowData.push_back(&sigma);
}


//------------------------------------------------------------------------------
// SequentialEstimator& operator=(const SequentialEstimator &se)
//------------------------------------------------------------------------------
/**
 * Assignemtn operator
 *
 * @param se The estimator that provides configuration information for this one
 *
 * @return This estimator, configured to match se
 */
//------------------------------------------------------------------------------
SequentialEstimator& SequentialEstimator::operator=(
      const SequentialEstimator &se)
{
   if (this != &se)
   {
      Estimator::operator=(se);
      measCovariance = NULL;
   }
   return *this;
}


//------------------------------------------------------------------------------
// Solver::SolverState AdvanceState()
//------------------------------------------------------------------------------
/**
 * Implements the basic sequential estimation state machine
 *
 * This method changes the SequentialEstimator's finite state based on the
 * status of the elements involved in the estimation process by calling the
 * state machine to process one step in its implementation
 *
 * @return The state at the end of the state machine step.
 */
//------------------------------------------------------------------------------
Solver::SolverState SequentialEstimator::AdvanceState()
{
   switch (currentState)
   {
      case INITIALIZING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("Executing the INITIALIZING state\n");
         #endif
         CompleteInitialization();
         break;

      case PROPAGATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("Executing the PROPAGATING state\n");
         #endif
         FindTimeStep();
         break;

      case CALCULATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("Executing the CALCULATING state\n");
         #endif
         CalculateData();
         break;

      case LOCATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("Executing the LOCATING state\n");
         #endif
         ProcessEvent();
         break;

      case ESTIMATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("Executing the ESTIMATING state\n");
         #endif
         Estimate();
         break;

      case CHECKINGRUN:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("Executing the CHECKINGRUN state\n");
         #endif
         CheckCompletion();
         break;

      case FINISHED:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("Executing the FINISHED state\n");
         #endif
         RunComplete();
         break;

      default:
         throw EstimatorException("Unknown state encountered in the " +
               instanceName + " sequential estimator.");

   }

   return currentState;
}


//------------------------------------------------------------------------------
// bool TakeAction(const std::string & action, const std::string & actionData)
//------------------------------------------------------------------------------
/**
 * This method implements a custom action
 *
 * @param action The action requested
 * @param actionData Optional additional data required by the action
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SequentialEstimator::TakeAction(const std::string & action,
      const std::string & actionData)
{
   return Estimator::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the estimator for use by setting and checking reference object
 * pointers
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SequentialEstimator::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("SequentialEstimator::Initialize() "
            "entered\n");
   #endif
   bool retval = false;

   if (Estimator::Initialize())
   {
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool Finalize()
//------------------------------------------------------------------------------
/**
 * Cleans up the estimator after a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SequentialEstimator::Finalize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("SequentialEstimator::Finalize() "
            "entered\n");
   #endif
   bool retval = false;

   if (Estimator::Finalize())
      retval = true;

   return retval;
}


//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void CompleteInitialization()
//------------------------------------------------------------------------------
/**
 * Performs final setup prior to use of the estimator
 *
 * This method is called in the INITIALIZING state
 */
//------------------------------------------------------------------------------
void SequentialEstimator::CompleteInitialization()
{
   PropagationStateManager *psm = propagator->GetPropStateManager();
   GmatState               *gs  = psm->GetState();
   estimationState              = esm.GetState();
   stateSize = estimationState->GetSize();

   Estimator::CompleteInitialization();

//   // Reset the a priori covariance to something more reasonable
//   for (UnsignedInt i = 0; i < stateSize; ++i)
//      if (i < 3)
//         (*stateCovariance)(i,i) = DEFAULT_POSITION_COVARIANCE;
//      else if (i < 6)
//         (*stateCovariance)(i,i) = DEFAULT_VELOCITY_COVARIANCE;
//      else
//         (*stateCovariance)(i,i) = DEFAULT_OTHER_COVARIANCE;

   estimationEpoch   = gs->GetEpoch();
   currentEpoch      = gs->GetEpoch();

   // Tell the measManager to complete its initialization
   bool measOK = measManager.Initialize();
   if (!measOK)
      throw SolverException(
            "BatchEstimator::CompleteInitialization - error initializing "
            "MeasurementManager.\n");

   // Now load up the observations
   measManager.PrepareForProcessing();
   measManager.LoadObservations();

   // First measurement epoch is the epoch of the first measurement.  Duh.
   nextMeasurementEpoch = measManager.GetEpoch();

   hAccum.clear();
   residuals.SetSize(stateSize);
   x0bar.SetSize(stateSize);
   dx.SetSize(stateSize);

   esm.MapObjectsToVector();

   measurementResiduals.clear();
   isInitialized = true;
   ReportProgress();

   if (GmatMathUtil::IsEqual(currentEpoch, nextMeasurementEpoch))
      currentState = CALCULATING;
   else
   {
      timeStep = (nextMeasurementEpoch - currentEpoch) *
            GmatTimeConstants::SECS_PER_DAY;
      PrepareForStep();
      currentState = PROPAGATING;
   }

   if (showAllResiduals)
   {
      StringArray plotMeasurements;
      for (UnsignedInt i = 0; i < measurementNames.size(); ++i)
      {
         plotMeasurements.clear();
         plotMeasurements.push_back(measurementNames[i]);
         std::string plotName = instanceName + "_" + measurementNames[i] +
               "_Residuals";
         BuildResidualPlot(plotName, plotMeasurements);
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
            "Init complete!\n   STM = %s\n   Covariance = %s\n",
            stm->ToString().c_str(), covariance->ToString().c_str());
   #endif
}

//------------------------------------------------------------------------------
// void FindTimeStep()
//------------------------------------------------------------------------------
/**
 * Calculates the time step to the next measurement
 *
 * This method is called in the PROPAGATING state
 */
//------------------------------------------------------------------------------
void SequentialEstimator::FindTimeStep()
{

   if (GmatMathUtil::IsEqual(currentEpoch, nextMeasurementEpoch))
   {
      // We're at the next measurement, so process it
      currentState = CALCULATING;
   }
   else if (nextMeasurementEpoch == 0.0)
   {
      // Finished running through the data
      currentState = CHECKINGRUN;
   }
   else
   {
      // Calculate the time step in seconds and stay in the PROPAGATING state;
      // timeStep could be positive or negative
      timeStep = (nextMeasurementEpoch - currentEpoch) *
            GmatTimeConstants::SECS_PER_DAY;
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("   timestep = %.12lf; nextepoch = "
               "%.12lf; current = %.12lf\n", timeStep, nextMeasurementEpoch,
               currentEpoch);
      #endif
   }
}


//------------------------------------------------------------------------------
// void CalculateData()
//------------------------------------------------------------------------------
/**
 * Determines if a measurement is feasible, and if so, prepares the data for use
 * while estimating
 *
 * This method is called in the CALCULATING state
 */
//------------------------------------------------------------------------------
void SequentialEstimator::CalculateData()
{
   // Update the STM
   esm.MapObjectsToSTM();
   esm.MapObjectsToVector();

   // Tell the measurement manager to calculate the simulation data
   if (measManager.CalculateMeasurements() == false)
   {
      // No measurements were possible
      measManager.AdvanceObservation();
      nextMeasurementEpoch = measManager.GetEpoch();
      FindTimeStep();

      if (currentEpoch < nextMeasurementEpoch)
      {
         currentState = PROPAGATING;
         PrepareForStep();
      }
      else
         currentState = CHECKINGRUN;
   }
   else if (measManager.GetEventCount() > 0)
   {
      currentState = LOCATING;
      locatingEvent = true;
   }
   else
   {
      currentState = ESTIMATING;
   }
}


//------------------------------------------------------------------------------
// void ProcessEvent()
//------------------------------------------------------------------------------
/**
 * Performs the estimator side actions needed for event location
 *
 * This method is called in the LOCATING state
 */
//------------------------------------------------------------------------------
void SequentialEstimator::ProcessEvent()
{
   locatingEvent = false;

   for (UnsignedInt i = 0; i < activeEvents.size(); ++i)
   {
      #ifdef DEBUG_EVENT
            MessageInterface::ShowMessage("*** Checking event %d\n", i);
      #endif
      if (((Event*)activeEvents[i])->CheckStatus() != LOCATED)
      {
         locatingEvent = true;
         #ifdef DEBUG_EVENT
            MessageInterface::ShowMessage("   *** %d not yet located\n", i);
         #endif
      }
      else
      {
         if (measManager.ProcessEvent((Event*)activeEvents[i]) == false)
            MessageInterface::ShowMessage("Event %d located but not "
                  "processed!\n", i);
         #ifdef DEBUG_EVENT
            MessageInterface::ShowMessage("   *** %d located!\n", i);
         #endif
      }
   }

   // Check each active event.  If all located, move into the ESTIMATING state
   if (!locatingEvent)
   {
      currentState = ESTIMATING;
   }
}


//------------------------------------------------------------------------------
// void CheckCompletion()
//------------------------------------------------------------------------------
/**
 * Performs final checks after processing the observations
 *
 * This method is called in the CHECKINGRUN state
 *
 * @note Sequential estimators might not need this method or the CHECKINGRUN
 * state
 */
//------------------------------------------------------------------------------
void SequentialEstimator::CheckCompletion()
{
   currentState = FINISHED;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
/**
 * Finishes the estimation process and reports results
 *
 * This method is called in the FINISHED state
 */
//------------------------------------------------------------------------------
void SequentialEstimator::RunComplete()
{
   ReportProgress();
   measManager.ProcessingComplete();
   esm.MapVectorToObjects();

//   WriteToTextFile();
}


//------------------------------------------------------------------------------
// void PrepareForStep()
//------------------------------------------------------------------------------
/**
 * This method performs actions common to sequential estimators prior to a
 * propagation step.  That includes resetting the STM to the identity.
 */
//------------------------------------------------------------------------------
void SequentialEstimator::PrepareForStep()
{
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
         if (i == j)
            stm->operator()(i,j) = 1.0;
         else
            stm->operator()(i,j) = 0.0;
   }
   esm.MapSTMToObjects();
}


//------------------------------------------------------------------------------
//  void WriteToTextFile(Solver::SolverState state)
//------------------------------------------------------------------------------
/**
 * Utility function used by the solvers to generate a progress file.
 *
 * @param <state> SolverState used for the report; if this parameter is
 *                different from the default value (UNDEFINED_STATE),
 *                it is used.  If the value is UNDEFINED_STATE, then the
 *                value of currentState is used.
 */
//------------------------------------------------------------------------------
void SequentialEstimator::WriteToTextFile(Solver::SolverState state)
{

}


//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
 * Generates a string for reporting the current simulator state.
 */
//------------------------------------------------------------------------------
std::string SequentialEstimator::GetProgressString()
{
   StringArray::iterator current;

   std::stringstream progress;
   progress.str("");
   progress.precision(12);
   const std::vector<ListItem*> *map = esm.GetStateMap();

   if (isInitialized)
   {
      switch (currentState)
      {
         case INITIALIZING:
            // This state is basically a "paused state" used for the Target
            // command to finalize the initial data for the variables and
            // goals.  All that is written here is the header information.
            {
               progress << "************************************************"
                        << "********\n"
                        << "*** Performing Estimation "
                        << "(using \"" << instanceName << "\")\n";

               // Write out the setup data
               progress << "*** " ;
               progress << "\n****************************"
                        << "****************************\n\na priori state:\n";

               for (UnsignedInt i = 0; i < map->size(); ++i)
               {
                  progress << "   "
                           << (*map)[i]->objectName << "."
                           << (*map)[i]->elementName << "."
                           << (*map)[i]->subelement << " = "
                           << (*estimationState)[i] << "\n";
               };

               progress << "\n a priori covariance:\n\n";
               Rmatrix aPrioriCovariance = *(stateCovariance->GetCovariance());
               for (Integer i = 0; i < aPrioriCovariance.GetNumRows(); ++i)
               {
                  progress << "----- Row " << (i+1) << "\n";
                  for (Integer j=0; j < aPrioriCovariance.GetNumColumns(); ++j)
                     progress << "   " << aPrioriCovariance(i, j);
                  progress << "\n";
               }
            }
            break;

         case ESTIMATING:
            progress << "Current estimated state:\n";
            progress << "   Estimation Epoch: "
                     << currentEpoch << "\n";

            for (UnsignedInt i = 0; i < map->size(); ++i)
            {
               progress << "   "
                        << (*estimationState)[i];
            }

            progress << "\n   Current Residual Value: "
                     << *(--(measurementResiduals.end())) << "      "
                     << "   Trace of the State Covariance:  "
                     << stateCovariance->GetCovariance()->Trace() << "\n";
            break;

         case FINISHED:
            progress << "\n****************************"
                     << "****************************\n"
                     << "*** Estimating Completed"
                     << "\n****************************"
                     << "****************************\n\n"
                     << "\n\nFinal Estimated State:\n\n";

            progress << "   Estimation Epoch (A.1 modified Julian): "
                        << currentEpoch << "\n\n";

            for (UnsignedInt i = 0; i < map->size(); ++i)
            {
               progress << "   "
                        << (*map)[i]->objectName << "."
                        << (*map)[i]->elementName << "."
                        << (*map)[i]->subelement << " = "
                        << (*estimationState)[i] << "\n";
            }

            { // Switch statement scoping
               Rmatrix finalCovariance = *(stateCovariance->GetCovariance());
               progress << "\nFinal Covariance Matrix:\n\n";
               for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
               {
                  progress << "----- Row " << (i+1) << "\n";
                  for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
                     progress << "   " << finalCovariance(i, j);
                  progress << "\n";
               }
            }

            #ifdef DUMP_RESIDUALS
               MessageInterface::ShowMessage("\nMeasurement Residuals: \n");
               for (UnsignedInt i = 0; i < measurementEpochs.size(); ++i)
               {
                  MessageInterface::ShowMessage("   %.12lf   %.12lf\n",
                        measurementEpochs[i], measurementResiduals[i]);
               }
            #endif

            progress << "\n****************************"
                     << "****************************\n"
                     << std::endl;

            break;

         default:
            throw SolverException(
               "Solver state not supported for the sequential estimator");
      }
   }
   else
      return Estimator::GetProgressString();

   return progress.str();
}
