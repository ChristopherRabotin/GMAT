//$Id: BatchEstimator.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         BatchEstimator
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
// Created: 2009/08/04
//
/**
 * Implementation of the BatchEstimator class
 */
//------------------------------------------------------------------------------


#include "BatchEstimator.hpp"
#include "GmatState.hpp"
#include "PropagationStateManager.hpp"
#include "EstimatorException.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"
#include "MessageInterface.hpp"
#include <sstream>
#include "SpaceObject.hpp"    // To access epoch data
#include "StringUtil.hpp"


//#define DEBUG_STATE_MACHINE
//#define DEBUG_SIMULATOR_WRITE
//#define DEBUG_SIMULATOR_INITIALIZATION
//#define DEBUG_EXECUTION
//#define DEBUG_EVENT
//#define DEBUG_ACCUMULATION_RESULTS
//#define DEBUG_PROPAGATION

// Macros for debugging of the state machine
//#define WALK_STATE_MACHINE
//#define DEBUG_VERBOSE
//#define RUN_SINGLE_PASS
//#define DUMP_FINAL_RESIDUALS


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
BatchEstimator::PARAMETER_TEXT[] =
{
   "EstimationEpochFormat",         // The epoch of the solution
   "EstimationEpoch",				// The epoch of the solution
//   "UsePrioriEstimate",											// made changes by TUAN NGUYEN
   "InversionAlgorithm",
   "MaxConsecutiveDivergences",										// made changes by TUAN NGUYEN
   // todo Add useApriori here
};

const Gmat::ParameterType
BatchEstimator::PARAMETER_TYPE[] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
//   Gmat::ON_OFF_TYPE,												// made changes by TUAN NGUYEN
   Gmat::STRING_TYPE,
   Gmat::INTEGER_TYPE,
};


//------------------------------------------------------------------------------
// BatchEstimator(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The subclass type name
 * @param name The name of the instance that is being created
 */
//------------------------------------------------------------------------------
BatchEstimator::BatchEstimator(const std::string &type,
      const std::string &name) :
   Estimator                  (type, name),
   estEpochFormat             ("FromParticipants"),
   estEpoch                   (""),
   oldResidualRMS             (0.0),
   newResidualRMS             (1.0e12),
   useApriori                 (false),						// second term of Equation Eq8-184 in GTDS MathSpec is not used	
   advanceToEstimationEpoch   (false),
//   converged                  (false),
   estimationStatus           (UNKNOWN),
   chooseRMSP                 (true),
   maxConsDivergences		  (3),							// made changes by TUAN NGUYEN
   inversionType              ("")
{
   objectTypeNames.push_back("BatchEstimator");
   parameterCount = BatchEstimatorParamCount;
}


//------------------------------------------------------------------------------
// ~BatchEstimator()
//------------------------------------------------------------------------------
/**
 * BatchEstimator destructor
 */
//------------------------------------------------------------------------------
BatchEstimator::~BatchEstimator()
{
   for (UnsignedInt i = 0; i < outerLoopBuffer.size(); ++i)
      delete outerLoopBuffer[i];
   outerLoopBuffer.clear();
}


//------------------------------------------------------------------------------
// BatchEstimator(const BatchEstimator& est)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param est The BatchEstimator that is being copied
 */
//------------------------------------------------------------------------------
BatchEstimator::BatchEstimator(const BatchEstimator& est) :
   Estimator                  (est),
   estEpochFormat             (est.estEpochFormat),
   estEpoch                   (est.estEpoch),
   oldResidualRMS             (0.0),
   newResidualRMS             (1.0e12),
   useApriori                 (est.useApriori),
   advanceToEstimationEpoch   (false),
//   converged                  (false),
   estimationStatus           (UNKNOWN),
   chooseRMSP                 (est.chooseRMSP),
   maxConsDivergences		  (est.maxConsDivergences),			// made changes by TUAN NGUYEN
   inversionType              (est.inversionType)
{
   // Clear the loop buffer
   for (UnsignedInt i = 0; i < outerLoopBuffer.size(); ++i)
      delete outerLoopBuffer[i];
   outerLoopBuffer.clear();
}


//------------------------------------------------------------------------------
// BatchEstimator& operator=(const BatchEstimator& est)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param est The BatchEstimator that is being copied
 *
 * @return This BatchEstimator, configured to match est
 */
//------------------------------------------------------------------------------
BatchEstimator& BatchEstimator::operator=(const BatchEstimator& est)
{
   if (this != &est)
   {
      Estimator::operator=(est);

      maxIterations  = est.maxIterations;
      estEpochFormat = est.estEpochFormat;
      estEpoch       = est.estEpoch;
      oldResidualRMS = 0.0;
      newResidualRMS = 0.0;
      useApriori     = est.useApriori;

      advanceToEstimationEpoch = false;
//      converged                = false;
	  estimationStatus         = UNKNOWN;

	  chooseRMSP               = est.chooseRMSP;
	  maxConsDivergences	   = est.maxConsDivergences;		// made changes by TUAN NGUYEN

      // Clear the loop buffer
      for (UnsignedInt i = 0; i < outerLoopBuffer.size(); ++i)
         delete outerLoopBuffer[i];
      outerLoopBuffer.clear();

      inversionType = est.inversionType;
   }

   return *this;
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string BatchEstimator::GetParameterText(const Integer id) const
{
   if (id >= EstimatorParamCount && id < BatchEstimatorParamCount)
      return PARAMETER_TEXT[id - EstimatorParamCount];
   return Estimator::GetParameterText(id);
}


//---------------------------------------------------------------------------
//  std::string GetParameterUnit(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the unit for the parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return unit for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string BatchEstimator::GetParameterUnit(const Integer id) const
{
   return Estimator::GetParameterUnit(id); // TBD
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str String for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer BatchEstimator::GetParameterID(const std::string &str) const
{
   for (Integer i = EstimatorParamCount; i < BatchEstimatorParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - EstimatorParamCount])
         return i;
   }

   return Estimator::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType BatchEstimator::GetParameterType(const Integer id) const
{
   if (id >= EstimatorParamCount && id < BatchEstimatorParamCount)
      return PARAMETER_TYPE[id - EstimatorParamCount];

   return Estimator::GetParameterType(id);
}



//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string BatchEstimator::GetParameterTypeString(const Integer id) const
{
   return Estimator::PARAM_TYPE_STRING[GetParameterType(id)];
}


Integer BatchEstimator::GetIntegerParameter(const Integer id) const
{
   if (id == MAX_CONSECUTIVE_DIVERGENCES)
      return maxConsDivergences;

   return Estimator::GetIntegerParameter(id);
}

Integer BatchEstimator::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == MAX_CONSECUTIVE_DIVERGENCES)
   {
      if (value < 1)
	  {
		 std::stringstream ss;
		 ss << "Error: " << GetName() << ".MaxConsecutiveDivergences has invalid value (" << value << "). It has to be a positive integer greater than 0.\n";
         throw EstimatorException(ss.str());
		 return value;
	  }

	  maxConsDivergences = value;
	  return value;
   }

   return Estimator::SetIntegerParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param id The integer ID for the parameter
 *
 * @return The string assigned to the parameter
 */
//------------------------------------------------------------------------------
std::string BatchEstimator::GetStringParameter(const Integer id) const
{
   if (id == ESTIMATION_EPOCH_FORMAT)
   {
      return estEpochFormat;
   }

   if (id == ESTIMATION_EPOCH)
   {
      return estEpoch;
   }

   if (id == INVERSION_ALGORITHM)
   {
      return inversionType;
   }

   return Estimator::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a parameter
 *
 * @param id The integer ID for the parameter
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool BatchEstimator::SetStringParameter(const Integer id,
         const std::string &value)
{
   if (id == ESTIMATION_EPOCH_FORMAT)
   {
      bool retVal = false;
	  StringArray sa = GetPropertyEnumStrings(id);
	  for (UnsignedInt i=0; i < sa.size(); ++i)
	  {
		 if (value == sa[i])
		 {
			estEpochFormat = value;
			retVal = true;
			break;
		 }
	  }

      if (value == "FromParticipants")
      {
         estimationEpoch = 0.0;
         estEpoch = "";
      }

      return retVal;
   }

   if (id == INVERSION_ALGORITHM)
   {
      if ((value == "Internal") || (value == "Schur") || (value == "Cholesky"))
      {
         inversionType = value;
         return true;
      }
      else
         throw SolverException("The requested inversion routine is not an "
               "allowed value for the field \"InversionAlgorithm\"; allowed "
               "values are \"Internal\", \"Schur\" and \"Cholesky\"");
   }

   if (id == ESTIMATION_EPOCH)
   {
      if ((estEpochFormat == "FromParticipants") && (value != ""))
      {
         MessageInterface::ShowMessage("Setting estimation epoch has no "
               "effect; EstimationEpochFormat is \"%s\"\n",
               estEpochFormat.c_str());
      }
      if (estEpochFormat != "FromParticipants")
      {
         estEpoch = value;
         // Convert to a.1 time for internal processing
         estimationEpoch = ConvertToRealEpoch(estEpoch, estEpochFormat);
      }
      return true;
   }

   return Estimator::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter from a StringArray
 *
 * @param id The integer ID for the parameter
 * @param index The index into the StringArray
 *
 * @return The value
 */
//------------------------------------------------------------------------------
std::string BatchEstimator::GetStringParameter(const Integer id,
      const Integer index) const
{
   return Estimator::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a string parameter in a StringArray
 *
 * @param id The integer ID for the parameter
 * @param value The new value
 * @param index The index into the StringArray
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool BatchEstimator::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   return Estimator::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param label The text label for the parameter
 *
 * @return The string assigned to the parameter
 */
//------------------------------------------------------------------------------
std::string BatchEstimator::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a parameter
 *
 * @param label The text label for the parameter
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool BatchEstimator::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter from a StringArray
 *
 * @param label The text label for the parameter
 * @param index The index into the StringArray
 *
 * @return The value
 */
//------------------------------------------------------------------------------
std::string BatchEstimator::GetStringParameter(const std::string &label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a string parameter in a StringArray
 *
 * @param label The text label for the parameter
 * @param value The new value
 * @param index The index into the StringArray
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool BatchEstimator::SetStringParameter(const std::string &label,
                                        const std::string &value,
                                        const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// std::string BatchEstimator::GetOnOffParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method gets "On" or "Off" value
 *
 * @param id	The id number of a parameter
 *
 * @return "On" or "Off" value
 */
//------------------------------------------------------------------------------
// made changes by TUAN NGUYEN
std::string BatchEstimator::GetOnOffParameter(const Integer id) const
{
//   if (id == USE_PRIORI_ESTIMATE)
//      return (useApriori ? "On" : "Off");

   return Estimator::GetOnOffParameter(id);
}


//------------------------------------------------------------------------------
// bool BatchEstimator::SetOnOffParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method gets "On" or "Off" value
 *
 * @param id		The id number of a parameter
 * @param value		value "On" or "Off"
 *
 * @return true value when it successfully sets the value, false otherwise. 
 */
//------------------------------------------------------------------------------
// made changes by TUAN NGUYEN
bool BatchEstimator::SetOnOffParameter(const Integer id, const std::string &value)
{
//   if (id == USE_PRIORI_ESTIMATE)
//  {
//      if (value == "On")
//	  {
//        useApriori = true;
//		 return true;
//	  }
//      if (value == "Off")
//	  {
//        useApriori = false;
//		 return true;
//	  }
//
//	  return false;
//   }

   return Estimator::SetOnOffParameter(id, value);
}


//------------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the list of allowable settings for the enumerated parameters
 *
 * @param id The ID of the parameter
 *
 * @return A const string array with the allowed settings.
 */
//------------------------------------------------------------------------------
const StringArray& BatchEstimator::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   enumStrings.clear();

   if (id == ESTIMATION_EPOCH_FORMAT)
   {
      enumStrings.push_back("FromParticipants");
      // todo This list should come from TimeSystemConverter in the util folder
      enumStrings.push_back("A1ModJulian");
      enumStrings.push_back("TAIModJulian");
      enumStrings.push_back("UTCModJulian");
      enumStrings.push_back("TTModJulian");
      enumStrings.push_back("A1Gregorian");
      enumStrings.push_back("TAIGregorian");
      enumStrings.push_back("UTCGregorian");
      enumStrings.push_back("TTGregorian");
      return enumStrings;
   }
   return Estimator::GetPropertyEnumStrings(id);

}





//------------------------------------------------------------------------------
//  bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * This method performs an action on the instance.
 *
 * TakeAction is a method overridden from GmatBase.  The only action defined for
 * a Simulator is "Reset" which resets the state to INITIALIZING
 *
 * @param <action>      Text label for the action.
 * @param <actionData>  Related action data, if needed.
 *
 * @return  flag indicating successful completion or not.
 */
//------------------------------------------------------------------------------
bool BatchEstimator::TakeAction(const std::string &action,
                           const std::string &actionData)
{
   // @todo  Complete Reset action (?) and add others if needed
   if (action == "Reset")
   {
      currentState = INITIALIZING;
      isInitialized = false;
//      converged   = false;
	  estimationStatus = UNKNOWN;

      return true;
   }

   return Estimator::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the estimator - checks for unset references and does some
 * validation checking.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool BatchEstimator::Initialize()
{
   bool retval = false;

   plotCount = 1;

   if (Estimator::Initialize())
   {
//    converged = false;
      estimationStatus = UNKNOWN;
      retval    = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
//  Solver::SolverState AdvanceState()
//------------------------------------------------------------------------------
/**
 * Advances the simulator to the next state.
 *
 * @return The state machine's finite state at the end of the method.
 */
//------------------------------------------------------------------------------
Solver::SolverState BatchEstimator::AdvanceState()
{
#ifdef DEBUG_STATE_MACHINE
   MessageInterface::ShowMessage("BatchEstimator::AdvanceState():  entered: currentState = %d\n", currentState);
#endif

   switch (currentState)
   {
      case INITIALIZING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Estimator state machine: "
                  "INITIALIZING\n");
         #endif
         // ReportProgress();
         CompleteInitialization();
         break;

      case PROPAGATING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Estimator state machine: "
                  "PROPAGATING\n");
         #endif
         // ReportProgress();
         FindTimeStep();
         break;

      case CALCULATING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Estimator state machine: "
                  "CALCULATING\n");
         #endif
         // ReportProgress();
         CalculateData();
         break;

      case LOCATING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Estimator state machine: "
                  "LOCATING\n");
         #endif
         // ReportProgress();
         ProcessEvent();
         break;

      case ACCUMULATING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Estimator state machine: "
                  "ACCUMULATING\n");
         #endif
         // ReportProgress();
         Accumulate();
         break;

      case ESTIMATING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Estimator state machine: "
                  "ESTIMATING\n");
         #endif
         // ReportProgress();
         Estimate();
         break;

      case CHECKINGRUN:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Estimator state machine: "
                  "CHECKINGRUN\n");
         #endif
         // ReportProgress();
         CheckCompletion();
         break;

      case FINISHED:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Estimator state machine: "
                  "FINISHED\n");
         #endif
         RunComplete();
         // ReportProgress();
         break;

      default:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Estimator state machine: "
               "Bad state for an estimator.\n");
         #endif
         /* throw EstimatorException("Solver state not supported for the simulator")*/;
   }

#ifdef DEBUG_STATE_MACHINE
   MessageInterface::ShowMessage("BatchEstimator::AdvanceState():  exit\n");
#endif
   return currentState;
}

//------------------------------------------------------------------------------
//  bool Finalize()
//------------------------------------------------------------------------------
/**
 * Finalizes the simulator.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool BatchEstimator::Finalize()
{
   bool retval = false;

   if (Estimator::Finalize())
   {
      retval = true;
      measManager.Finalize();
      esm.MapVectorToObjects();
   }

   return retval;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Methods used in the finite state machine
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void CompleteInitialization()
//------------------------------------------------------------------------------
/**
 * This method completes initialization for the Simulator object, initializing
 * its MeasurementManager, retrieving the epoch and setting the state.
 */
//------------------------------------------------------------------------------
void BatchEstimator::CompleteInitialization()
{
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("BatchEstimator state is INITIALIZING\n");
   #endif

   // Show all residuals plots
   if (showAllResiduals)
   {
      StringArray plotMeasurements;
      for (UnsignedInt i = 0; i < modelNames.size(); ++i)
      {
         plotMeasurements.clear();
         plotMeasurements.push_back(modelNames[i]);
         std::string plotName = instanceName + "_" + modelNames[i] +
               "_Residuals";
         BuildResidualPlot(plotName, plotMeasurements);
      }
   }

   if (advanceToEstimationEpoch == false)
   {
      PropagationStateManager *psm = propagator->GetPropStateManager();
      GmatState               *gs  = psm->GetState();
      estimationState              = esm.GetState();
      stateSize = estimationState->GetSize();

      Estimator::CompleteInitialization();

      // If estimation epoch not set, use the epoch from the prop state
      if ((estEpochFormat == "FromParticipants") || (estimationEpoch <= 0.0))
      {
         ObjectArray participants;
         esm.GetStateObjects(participants, Gmat::SPACEOBJECT);
         for (UnsignedInt i = 0; i < participants.size(); ++i)
            estimationEpoch   = ((SpaceObject *)(participants[i]))->GetEpoch();
      }
      currentEpoch         = gs->GetEpoch();

      // Tell the measManager to complete its initialization
      bool measOK = measManager.SetPropagator(propagator);
      measOK = measOK && measManager.Initialize();
      if (!measOK)
         throw SolverException(
               "BatchEstimator::CompleteInitialization - error initializing "
               "MeasurementManager.\n");

      // Now load up the observations
      measManager.PrepareForProcessing(false);
      UnsignedInt numRec = measManager.LoadObservations();					// made changes by TUAN NGUYEN
	  if (numRec == 0)														// made changes by TUAN NGUYEN
	  {
         throw EstimatorException("No observation data is used for estimation\n");	// made changes by TUAN NGUYEN
	  }
	  
///// Check for more generic approach
	  measManager.LoadRampTables();											// made changes by TUAN NGUYEN

      if (!GmatMathUtil::IsEqual(currentEpoch, estimationEpoch))
      {
         advanceToEstimationEpoch = true;
         nextMeasurementEpoch = estimationEpoch;
         currentState = PROPAGATING;
         return;
      }
   }

   advanceToEstimationEpoch = false;

   // First measurement epoch is the epoch of the first measurement.  Duh.
   nextMeasurementEpoch = measManager.GetEpoch();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
            "Init complete!\n   STM = %s\n   Covariance = %s\n",
            stm->ToString().c_str(), covariance->ToString().c_str());
   #endif

   hAccum.clear();
   if (useApriori)
   {
      information = stateCovariance->GetCovariance()->Inverse();
   }
   else
   {
      information.SetSize(stateSize, stateSize);
      for (UnsignedInt i = 0; i <  stateSize; ++i)
         for (UnsignedInt j = 0; j <  stateSize; ++j)
            information(i,j) = 0.0;
   }

   residuals.SetSize(stateSize);
   x0bar.SetSize(stateSize);

   measurementResiduals.clear();
   measurementEpochs.clear();

   for (Integer i = 0; i < information.GetNumRows(); ++i)
   {
      residuals[i] = 0.0;
//      if (useApriori)											// made changes by TUAN NGUYEN
//         x0bar[i] = (*estimationState)[i];					// made changes by TUAN NGUYEN
//      else													// made changes by TUAN NGUYEN			Note that: x0bar is set to zero-vector as shown in  page 195 Statistical Orbit Determination
         x0bar[i] = 0.0;
   }

   if (useApriori)
      for (Integer i = 0; i < information.GetNumRows(); ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            residuals[i] += information(i,j) * x0bar[j];
      }

   esm.BufferObjects(&outerLoopBuffer);
   esm.MapObjectsToVector();

//   converged   = false;
   estimationStatus = UNKNOWN;					// made changes by TUAN NGUYEN
   // Convert estimation state from GMAT internal coordinate system to participants' coordinate system
   GetEstimationState(aprioriSolveForState);	// made changes by TUAN NGUYEN

   isInitialized = true;
   numDivIterations = 0;						// It need to reset it's value when starting estimatimation calculation

   WriteToTextFile();
   ReportProgress();

   numRemovedRecords["U"] = 0;
   numRemovedRecords["R"] = 0;
   numRemovedRecords["B"] = 0;
   numRemovedRecords["OLSE"] = 0;
   numRemovedRecords["IRMS"] = 0;

   if (GmatMathUtil::IsEqual(currentEpoch, nextMeasurementEpoch))
      currentState = CALCULATING;
   else
   {
      timeStep = (nextMeasurementEpoch - currentEpoch) *
            GmatTimeConstants::SECS_PER_DAY;
      currentState = PROPAGATING;
   }



   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("BatchEstimator::CompleteInitialization "
            "process complete\n");
      MessageInterface::ShowMessage("   Estimation state = [");
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage(" %.12lf ", (*estimationState)[i]);
      MessageInterface::ShowMessage("]\n");
      MessageInterface::ShowMessage("   Information Matrix = \n");
      for (Integer i = 0; i < information.GetNumRows(); ++i)
      {
         MessageInterface::ShowMessage("      [");
         for (Integer j = 0; j < information.GetNumColumns(); ++j)
         {
            MessageInterface::ShowMessage(" %.12lf ", information(i, j));
         }
         MessageInterface::ShowMessage("]\n");
      }
      MessageInterface::ShowMessage("   Residuals = [");
      for (Integer i = 0; i < residuals.GetSize(); ++i)
         MessageInterface::ShowMessage(" %.12lf ", residuals[i]);
      MessageInterface::ShowMessage("]\n");
   #endif
}


//------------------------------------------------------------------------------
//  void FindTimeStep()
//------------------------------------------------------------------------------
/**
 * This method determines whether the simulation is finished or still
 * calculating, and if neither, computes the timeStep.
 */
//------------------------------------------------------------------------------
void BatchEstimator::FindTimeStep()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("BatchEstimator::FindTimeStep() "
            "current epoch = %.12lf, next epoch = %.12lf\n",
            currentEpoch, nextMeasurementEpoch);
   #endif

   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("BatchEstimator state is PROPAGATING\n");
   #endif

   if (advanceToEstimationEpoch == true)
   {
      if (GmatMathUtil::IsEqual(currentEpoch, estimationEpoch))
      {
         timeStep = 0.0;
         currentState = INITIALIZING;
         return;
      }
      timeStep = (estimationEpoch - currentEpoch) *
            GmatTimeConstants::SECS_PER_DAY;
      return;
   }

   if (nextMeasurementEpoch == 0.0)
   {
      // Estimate and check for convergence after processing measurements
      currentState = ESTIMATING;
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("Next state will be ESTIMATING\n");
   #endif

   }
   else if (GmatMathUtil::IsEqual(currentEpoch, nextMeasurementEpoch))
   {
      // We're at the next measurement, so process it
      currentState = CALCULATING;
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("Next state will be CALCULATING\n");
   #endif
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
//  void CalculateData()
//------------------------------------------------------------------------------
/**
 * This method checks for valid measurements and changes state based on the
 * results.
 */
//------------------------------------------------------------------------------
void BatchEstimator::CalculateData()
{
   #ifdef WALK_STATE_MACHINE
	  MessageInterface::ShowMessage("Entered BatchEstimator::CalculateData()\n");
   #endif

   // Update the STM
   esm.MapObjectsToSTM();

   // Tell the measurement manager to calculate the simulation data
//   measManager.CalculateMeasurements();

   //if (measManager.CalculateMeasurements() == false)
   //{
   //   // No measurements were possible
   //   bool endOfDataSet = measManager.AdvanceObservation();
   //   if (endOfDataSet)										// made changes by TUAN NGUYEN
   //      currentState = ESTIMATING;							// made changes by TUAN NGUYEN
	  //else													// made changes by TUAN NGUYEN
	  //{														// made changes by TUAN NGUYEN
   //      nextMeasurementEpoch = measManager.GetEpoch();
   //      FindTimeStep();
	  //   if (currentEpoch <= nextMeasurementEpoch)
   //         currentState = PROPAGATING;
   //      else
   //         currentState = ESTIMATING;
	  //}														// made changes by TUAN NGUYEN
   //}
   if (measManager.CalculateMeasurements() == false)
   {
	   currentState = ACCUMULATING;
   }
   else if (measManager.GetEventCount() > 0)
   {
      currentState = LOCATING;
      locatingEvent = true;
   }
   else
      currentState = ACCUMULATING;
   
   #ifdef WALK_STATE_MACHINE
	  MessageInterface::ShowMessage("Exit BatchEstimator::CalculateData()\n");
   #endif

}


//------------------------------------------------------------------------------
//  void ProcessEvent()
//------------------------------------------------------------------------------
/**
 * This method manages the state machine operations while processing events.
 */
//------------------------------------------------------------------------------
void BatchEstimator::ProcessEvent()
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

   // Check each active event.  If all located, move into the ACCUMULATING state
   if (!locatingEvent)
   {
      currentState = ACCUMULATING;
   }
}


//------------------------------------------------------------------------------
//  void CheckCompletion()
//------------------------------------------------------------------------------
/**
 * This method tests to see if the estimation algorithm has converged.
 */
//------------------------------------------------------------------------------
void BatchEstimator::CheckCompletion()
{
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("BatchEstimator state is CHECKINGRUN\n");
   #endif

   #ifdef DEBUG_VERBOSE
      MessageInterface::ShowMessage("\nCompleted iteration %d\n\n",
            iterationsTaken+1);
   #endif

   convergenceReason = "";
//   converged = TestForConvergence(convergenceReason);
   estimationStatus = TestForConvergence(convergenceReason);
   
   #ifdef RUN_SINGLE_PASS
      converged = true;
   #endif

   ++iterationsTaken;
//   if ((converged) || (iterationsTaken >= maxIterations))
   if ((estimationStatus == ABSOLUTETOL_CONVERGED) ||
	   (estimationStatus == RELATIVETOL_CONVERGED) ||
	   (estimationStatus == ABS_AND_REL_TOL_CONVERGED) ||
	   (estimationStatus == MAX_CONSECUTIVE_DIVERGED) ||
	   (estimationStatus == MAX_ITERATIONS_DIVERGED))
   {
      currentState = FINISHED;
   }
   else
   {
      if (showAllResiduals)
         PlotResiduals();

      // Reset to the new initial state, clear the processed data, etc
      esm.RestoreObjects(&outerLoopBuffer);
      esm.MapVectorToObjects();
      esm.MapObjectsToSTM();
      currentEpoch = estimationEpoch;
      measManager.Reset();											// set current observation data to be the first one in observation data table				// made changes by TUAN NGUYEN
	  // Note that: all unused data records will be handled in BatchEstimatorInv::Accumulate() function
	  // Therefore, there is no prblem when passing those records to Accumulate() 
	  //if (measManager.GetObsDataObject()->inUsed == false)			// if the first observation data is not in used, then go to the next in-used data record	// made changes by TUAN NGUYEN
	  //{
	  //   measManager.AdvanceObservation();							// made changes by TUAN NGUYEN
	  //}
      nextMeasurementEpoch = measManager.GetEpoch();

      // Need to reset STM and covariances
      hAccum.clear();
      if (useApriori)
         information = stateCovariance->GetCovariance()->Inverse();
      else
      {
         information.SetSize(stateSize, stateSize);
         for (UnsignedInt i = 0; i <  stateSize; ++i)
            for (UnsignedInt j = 0; j <  stateSize; ++j)
               information(i,j) = 0.0;
      }
	  measurementResiduals.clear();
      measurementEpochs.clear();
      measurementResidualID.clear();

      for (UnsignedInt i = 0; i <  stateSize; ++i)
         for (UnsignedInt j = 0; j <  stateSize; ++j)
            if (i == j)
               (*stm)(i,j) = 1.0;
            else
               (*stm)(i,j) = 0.0;

      esm.MapSTMToObjects();

      for (UnsignedInt i = 0; i < information.GetNumRows(); ++i)
         residuals[i] = 0.0;
      
      for (UnsignedInt j = 0; j < stateSize; ++j)
         x0bar[j] -= dx[j];

      if (useApriori)
	  {
         for (Integer i = 0; i < information.GetNumRows(); ++i)
         {
            for (UnsignedInt j = 0; j < stateSize; ++j)
            {
               residuals[i] += information(i,j) * x0bar[j];
            }
         }
      }
      #ifdef DEBUG_VERBOSE
         MessageInterface::ShowMessage("Starting iteration %d\n\n",
               iterationsTaken+1);
      #endif

      #ifdef DEBUG_ITERATIONS
         MessageInterface::ShowMessage(
               "Init complete!\n   STM = %s\n   Covariance = %s\n",
               stm->ToString().c_str(), covariance->ToString().c_str());
      #endif

      WriteToTextFile();
      ReportProgress();

      numRemovedRecords["U"] = 0;
      numRemovedRecords["R"] = 0;
      numRemovedRecords["B"] = 0;
      numRemovedRecords["OLSE"] = 0;
      numRemovedRecords["IRMS"] = 0;

      if (GmatMathUtil::IsEqual(currentEpoch, nextMeasurementEpoch))
         currentState = CALCULATING;
      else
      {
         timeStep = (nextMeasurementEpoch - currentEpoch) *
               GmatTimeConstants::SECS_PER_DAY;
         currentState = PROPAGATING;
      }
   }
}


//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
/**
 * This method updates the simulator text file at the end of a simulator run.
 */
//------------------------------------------------------------------------------
void BatchEstimator::RunComplete()
{
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("BatchEstimator state is FINALIZING\n");
   #endif

   measManager.ProcessingComplete();

   // Report the results
   WriteToTextFile();
   ReportProgress();

   if (showAllResiduals)
      PlotResiduals();
}


//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
 * Generates a string for reporting the current simulator state.
 */
//------------------------------------------------------------------------------
std::string BatchEstimator::GetProgressString()
{
   StringArray::iterator current;

   std::stringstream progress;
   progress.str("");
   progress.precision(12);
   const std::vector<ListItem*> *map = esm.GetStateMap();

   GmatState outputEstimationState;											// made changes by TUAN NGUYEN

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

               if (estEpochFormat != "FromParticipants")
                  progress << "   Estimation Epoch (" << estEpochFormat
                           << "): " << estEpoch << "\n";
               else
			   {
                  char s[100];
				  sprintf(&s[0], "%22.12lf", estimationEpoch);
                  progress << "   Estimation Epoch (A.1 modified Julian): " << s << "\n";
			   }

			   GetEstimationState(outputEstimationState);						// made changes by TUAN NGUYEN
               
			   for (UnsignedInt i = 0; i < map->size(); ++i)
               {
                  progress << "   " << GetElementFullName((*map)[i], false) << " = "
                           //<< (*map)[i]->objectName << "."
                           //<< (*map)[i]->elementName << "."
                           //<< (*map)[i]->subelement << " = "
                           << outputEstimationState[i] << "\n";					// made changes by TUAN NGUYEN
               };

               //progress << "\n a priori covariance:\n\n";
               //Rmatrix aPrioriCovariance = *(stateCovariance->GetCovariance());
               //for (Integer i = 0; i < aPrioriCovariance.GetNumRows(); ++i)
               //{
               //   progress << "----- Row " << (i+1) << "\n";
               //   for (Integer j = 0; j < aPrioriCovariance.GetNumColumns(); ++j)
               //      progress << "   " << aPrioriCovariance(i, j);
               //   progress << "\n";
               //}
            }
            break;

         case CHECKINGRUN:
            progress << "\n   WeightedRMS residuals for this iteration: "
                     << newResidualRMS;
            progress << "\n   BestRMS residuals for this iteration: "
                     << bestResidualRMS;
            progress << "\n   PredictedRMS residuals for next iteration: "
                     << predictedRMS << "\n";
			
			switch(estimationStatus)
			{
			case ABSOLUTETOL_CONVERGED:
			   progress << "This iteration is converged due to absolute tolerance convergence criteria\n";
			   break;
			case RELATIVETOL_CONVERGED:
			   progress << "This iteration is converged due to relative convergence criteria \n";
			   break;
			case ABS_AND_REL_TOL_CONVERGED:
			   progress << "This iteration is converged due to boths: absolute and relative convergence criteria\n";
			   break;
			case MAX_CONSECUTIVE_DIVERGED:
			   progress << "This iteration is diverged due to maximum consecutive diverged criteria\n";
			   break;
			case CONVERGING:
			   progress << "This iteration is converging\n";
			   break;
			case DIVERGING:
			   progress << "This iteration is diverging\n";
			   break;
			}
			progress << "\n";

			progress << "------------------------------"
                     << "------------------------\n"
                     << "Iteration " << iterationsTaken
                     << "\n\nCurrent estimated state:\n";
			char s[100];
			sprintf(&s[0], "%22.12lf", estimationEpoch);
            progress << "   Estimation Epoch (A.1 modified Julian): " << s << "\n";

			GetEstimationState(outputEstimationState);							// made changes by TUAN NGUYEN

            for (UnsignedInt i = 0; i < map->size(); ++i)
            {
               progress << "   " << GetElementFullName((*map)[i], false) << " = "
                        //<< (*map)[i]->objectName << "."
                        //<< (*map)[i]->elementName << "."
                        //<< (*map)[i]->subelement << " = "
                        << outputEstimationState[i] << "\n";					// made changes by TUAN NGUYEN
            }

            break;

         case FINISHED:
            progress << "\n   WeightedRMS residuals for this iteration: "
                     << newResidualRMS;
            progress << "\n   BestRMS residuals for this iteration: "
                     << bestResidualRMS;
            progress << "\n   PredictedRMS residuals for next iteration: "
                     << predictedRMS << "\n";

			switch(estimationStatus)
			{
			case ABSOLUTETOL_CONVERGED:
			   progress << "This iteration is converged due to absolute tolerance convergence criteria\n";
			   break;
			case RELATIVETOL_CONVERGED:
			   progress << "This iteration is converged due to relative convergence criteria \n";
			   break;
			case ABS_AND_REL_TOL_CONVERGED:
			   progress << "This iteration is converged due to boths: absolute and relative convergence criteria\n";
			   break;
			case MAX_CONSECUTIVE_DIVERGED:
			   progress << "This iteration is diverged due to maximum consecutive diverged criteria\n";
			   break;
			case MAX_ITERATIONS_DIVERGED:
			   progress << "This iteration is diverged due to maximum iterations\n";
			   break;
			case CONVERGING:
			   progress << "This iteration is converging\n";
			   break;
			case DIVERGING:
			   progress << "This iteration is diverging\n";
			   break;
			}
			progress << "\n";

            progress << "\n****************************"
                     << "****************************\n"
                     << "*** Estimating Completed in " << iterationsTaken
                     << " iterations"
                     << "\n****************************"
                     << "****************************\n\n"
                     << "Estimation ";
			switch(estimationStatus)
			{
			case ABSOLUTETOL_CONVERGED:
			case RELATIVETOL_CONVERGED:
			case ABS_AND_REL_TOL_CONVERGED:
				progress << "converged!\n";
				break;
			case MAX_CONSECUTIVE_DIVERGED:
			case MAX_ITERATIONS_DIVERGED:
			case CONVERGING:
			case DIVERGING:
				progress << "did not converge!\n";
				break;
			case UNKNOWN:
				break;
			};
            //         << (converged ? "converged!" : "did not converge") << "\n"
			progress   << "   " << convergenceReason << "\n"
                     << "Final Estimated State:\n\n";

            if (estEpochFormat != "FromParticipants")
               progress << "   Estimation Epoch (" << estEpochFormat
                        << "): " << estEpoch << "\n";
            else
			{
			   char s[100];
			   sprintf(&s[0],"%22.12lf", estimationEpoch);
               progress << "   Estimation Epoch (A.1 modified Julian): " << s << "\n";
			}

			GetEstimationState(outputEstimationState);							// made changes by TUAN NGUYEN

            for (UnsignedInt i = 0; i < map->size(); ++i)
            {
               progress << "   " << GetElementFullName((*map)[i], false) << " = "
                        //<< (*map)[i]->objectName << "."
                        //<< (*map)[i]->elementName << "."
                        //<< (*map)[i]->subelement << " = "
			            << outputEstimationState[i] << "\n";					// made changes by TUAN NGUYEN
            }

            { // Switch statement scoping
               Rmatrix finalCovariance = information.Inverse();
               progress << "\nFinal Covariance Matrix:\n\n";
               for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
               {
                  for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
                     progress << "   " << finalCovariance(i, j);
                  progress << "\n";
               }
            }

            if (textFileMode == "Verbose")
            {
               progress << "\n   WeightedRMS residuals for previous iteration: "
                        << oldResidualRMS;
               progress << "\n   WeightedRMS residuals for this iteration:     "
                        << newResidualRMS;
               progress << "\n   BestRMS residuals for this iteration: "
                     << bestResidualRMS << "\n\n";
            }

            progress << "\n****************************"
                     << "****************************\n\n"
                     << std::endl;

            #ifdef DUMP_FINAL_RESIDUALS
            {
               std::fstream residFile;
               residFile.open("FinalResiduals.csv", std::ios::out);

               for (UnsignedInt i = 0; i < measurementResiduals.size(); ++i)
               {
                  residFile << i << "   " << measurementEpochs[i]
                            << "   " << measurementResiduals[i] << "\n";
               }

               residFile.close();
            }
            #endif

            break;

         default:
            throw SolverException(
               "Solver state not supported for the simulator");
      }
   }
   else
      return Estimator::GetProgressString();

   return progress.str();
}


#include "Spacecraft.hpp"
std::string BatchEstimator::GetElementFullName(ListItem* infor, bool isInternalCS) const
{
   std::stringstream ss;
   
   ss << infor->objectName << ".";
   if (infor->elementName == "CartesianState")
   {
	   //MessageInterface::ShowMessage("<<<<  Object name <%s>  coordinate system <%s>\n", infor->object->GetName().c_str(), ((Spacecraft*)(infor->object))->GetRefObject(Gmat::COORDINATE_SYSTEM, "")->GetName().c_str());
      if (isInternalCS)
		 ss << "EarthMJ2000Eq" << ".";
	  else
	     ss << ((Spacecraft*)(infor->object))->GetRefObject(Gmat::COORDINATE_SYSTEM, "")->GetName() << ".";

	  switch(infor->subelement)
	  {
	  case 1:
		 ss << "X";
		 break;
	  case 2:
		 ss << "Y";
		 break;
	  case 3:
		 ss << "Z";
		 break;
	  case 4:
		 ss << "VX";
		 break;
	  case 5:
		 ss << "VY";
		 break;
	  case 6:
		 ss << "VZ";
		 break;
	  }
   }
   else if (infor->elementName == "Position")
   {
	  switch(infor->subelement)
	  {
	  case 1:
		 ss << "X";
		 break;
	  case 2:
		 ss << "Y";
		 break;
	  case 3:
		 ss << "Z";
		 break;
	  }
   }
   else if (infor->elementName == "Velocity")
   {
	  switch(infor->subelement)
	  {
	  case 1:
		 ss << "VX";
		 break;
	  case 2:
		 ss << "VY";
		 break;
	  case 3:
		 ss << "VZ";
		 break;
	  }
   }
   else
      ss << infor->elementName << "." << infor->subelement;

   return ss.str();
}


//------------------------------------------------------------------------------
// Integer TestForConvergence(std::string &reason)
//------------------------------------------------------------------------------
/**
 * Provides the default convergence test for the BatchEstimators.
 *
 * GMAT's batch estimator report a converged solution when any of the following
 * criteria are met:
 *
 * 1. Aboslute Tolerance test: |RMS| <= Absolute Tol
 *
 * 2. Relative Tolerance test: |1 - RMSP/RMSB| <= Relative Tol
 *
 * 3. Maximum consecutive divergence test
 * 
 * 4. Converging and diverging tests
 *
 * Note that: tests 2, 3, and 4 only perform after iteration 0
 *
 * @param reason A string that is set to indicate the criterion that was met
 *               when convergence is detected.  The string contains text for all
 *               criteria that are satisfied.
 *
 * @return true if the estimator has converged, false if not
 */
//------------------------------------------------------------------------------
Integer BatchEstimator::TestForConvergence(std::string &reason)
{
   Integer retval = UNKNOWN;
   std::stringstream why;
   
   // AbsoluteTol test
   if (newResidualRMS <= absoluteTolerance)
   {
      why << "   WeightedRMS residual, " << newResidualRMS
          << " is within the AbsoluteTol, " << absoluteTolerance
          << "\n";

	  reason = why.str();
      retval = ABSOLUTETOL_CONVERGED;
   }

   // RelativeTol test
   if (GmatMathUtil::Abs((predictedRMS - bestResidualRMS)/bestResidualRMS) <= relativeTolerance)
   {
      why << "   |1 - RMSP/RMSB| = | 1- " << predictedRMS << " / " << bestResidualRMS << "| = " 
          << GmatMathUtil::Abs(1 - predictedRMS/bestResidualRMS)
          << " is less than RelativeTol, "
          << relativeTolerance << "\n";

	  reason = why.str();
	  if (retval == ABSOLUTETOL_CONVERGED)
         retval = ABS_AND_REL_TOL_CONVERGED;
	  else
		 retval = RELATIVETOL_CONVERGED;
   }
   if (retval != UNKNOWN)
      return retval;

   if (iterationsTaken == (maxIterations-1))
   {
      retval = MAX_ITERATIONS_DIVERGED;
	  why << "Number of iterations reaches it's maximum setting value (" << maxIterations << ")\n";
	  reason = why.str();
      return retval;
   }

   if (iterationsTaken >= 1)
   {
	  // Maximmum consecutive divergence test
	  if (newResidualRMS > oldResidualRMS)
	  {
		 numDivIterations++;
		 if (numDivIterations >= maxConsDivergences)
		 {
			why << "Number of consecutive divergences reaches it's maximum setting value (" << maxConsDivergences << ")\n";
			reason = why.str();
		    retval = MAX_CONSECUTIVE_DIVERGED;
		 }
		 else
		    retval = DIVERGING;
	  }
	  else
	  {
		 numDivIterations = 0;
		 retval = CONVERGING;
	  }
   }

   return retval;
}


//------------------------------------------------------------------------------
// void WriteToTextFile(Solver::SolverState)
//------------------------------------------------------------------------------
/**
 * This method adds text to the estimator text file.
 *
 * @note The contents of the text file are TBD
 *
 * @param solverState The current state from the finite state machine
 */
//------------------------------------------------------------------------------
void BatchEstimator::WriteToTextFile(Solver::SolverState sState)
{
   // Only write to report file when ReportStyle is Normal or Verbose
//   if ((textFileMode != "Normal")&&(textFileMode != "Verbose"))
//      return;

   GmatState outputEstimationState;											// made changes by TUAN NGUYEN

   if (!showProgress)
      return;

   if (!textFile.is_open())
      OpenSolverTextFile();

   Solver::SolverState theState = sState;
   if (sState == Solver::UNDEFINED_STATE)
      theState = currentState;

   const std::vector<ListItem*> *map = esm.GetStateMap();

   textFile.setf(std::ios::fixed, std::ios::floatfield);

   switch (theState)
   {
      case INITIALIZING:
		 WriteHeader();
         break;
	  case ACCUMULATING:
		textFile << linesBuff;
		textFile.flush();
		 break;
	  case ESTIMATING:
		 WriteSummary(theState);
		 break;
      case CHECKINGRUN:
		 WriteSummary(theState);
		 WriteHeader();
         break;

      case FINISHED:
		 WriteSummary(theState);
		 WriteConclusion();
         break;

      default:
         break;
   }
}


void BatchEstimator::WriteConclusion()
{
   GmatState outputEstimationState;
   const std::vector<ListItem*> *map = esm.GetStateMap();

   /// 1. Write estimation status
   textFile << "\n"
            << "********************************************************\n"
            << "*** Estimating Completed in " << iterationsTaken << " iterations\n"
            << "********************************************************\n\n"
            << "Estimation ";
   switch(estimationStatus)
   {
      case ABSOLUTETOL_CONVERGED:
      case RELATIVETOL_CONVERGED:
      case ABS_AND_REL_TOL_CONVERGED:
         textFile << "converged!\n";
         break;
      case MAX_CONSECUTIVE_DIVERGED:
	  case MAX_ITERATIONS_DIVERGED:
      case CONVERGING:
      case DIVERGING:
         textFile << "did not converge!\n";
         break;
      case UNKNOWN:
         break;
   };

   /// 2. Write convergence reason
   textFile.precision(15);
   textFile << "   " << convergenceReason << "\n"
            << "Final Estimated State:\n";

   if (estEpochFormat != "FromParticipants")
      textFile << "   Estimation Epoch (" << estEpochFormat
               << "): " << estEpoch << "\n";
   else
      textFile << "   Estimation Epoch (A.1 Mod. Julian): "
               << estimationEpoch << "\n";

	
   /// 3. Write final state
   GetEstimationState(outputEstimationState);
   textFile.precision(8);
   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
	  textFile << "   ";
	  if (((*map)[i]->object->IsOfType(Gmat::MEASUREMENT_MODEL))&&
          ((*map)[i]->elementName == "Bias"))
	  {
		 MeasurementModel* mm = (MeasurementModel*)((*map)[i]->object);
		 StringArray sa = mm->GetStringArrayParameter("Participants");
		 textFile << mm->GetStringParameter("Type") << " ";
		 for( UnsignedInt j=0; j < sa.size(); ++j)
			textFile << sa[j] << (((j+1) != sa.size())?",":" Bias.");
		 textFile << (*map)[i]->subelement;
	  }
	  else
	  {
         //textFile << (*map)[i]->objectName << "."
         //         << (*map)[i]->elementName << "."
         //         << (*map)[i]->subelement;
		 textFile << GetElementFullName((*map)[i], false);
	  }
	  textFile << " = " << outputEstimationState[i] << "\n";
   }
   textFile << "\n";


   /// 4. Write final coveriance matrix 
   Rmatrix finalCovariance = information.Inverse();
   textFile << "\nFinal Covariance Matrix:\n";
   textFile.precision(12);
   for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
   {
      for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
         textFile << "   " << finalCovariance(i, j);
      textFile << "\n";
   }


   /// 5. Write previous RMS, current RMS, and the best RMS
   textFile << "\n   WeightedRMS residuals for previous iteration: "
            << oldResidualRMS;
   textFile << "\n   WeightedRMS residuals for this iteration:     "
            << newResidualRMS ;
   textFile << "\n   BestRMS residuals for this iteration: "
            << bestResidualRMS << "\n\n";

   textFile << "\n********************************************************\n\n";
   textFile.flush();

}


void BatchEstimator::WriteHeader()
{
   
   GmatState outputEstimationState;
   const std::vector<ListItem*> *map = esm.GetStateMap();

   /// 1. Write iteration number
   textFile << "\n"
	        << "********************************************************\n"
            << "*** Iteration " << iterationsTaken << "\n"
            << "********************************************************\n\n";


   /// 2. Write state at beginning iteration:
   textFile << "State at Beginning of Iteration:\n";
   textFile.precision(15);
   if (estEpochFormat != "FromParticipants")
      textFile << "   Estimation Epoch (" << estEpochFormat
               << "): " << estEpoch << "\n";
   else
      textFile << "   Estimation Epoch (A.1 Mod. Julian): "
               << estimationEpoch << "\n";

   // Convert state to participants' coordinate system:
   GetEstimationState(outputEstimationState);
   // write out state
   textFile.precision(8);
   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
	  textFile << "   ";
	  if (((*map)[i]->object->IsOfType(Gmat::MEASUREMENT_MODEL))&&
          ((*map)[i]->elementName == "Bias"))
	  {
		 MeasurementModel* mm = (MeasurementModel*)((*map)[i]->object);
		 StringArray sa = mm->GetStringArrayParameter("Participants");
		 textFile << mm->GetStringParameter("Type") << " ";
		 for( UnsignedInt j=0; j < sa.size(); ++j)
			textFile << sa[j] << (((j+1) != sa.size())?",":" Bias.");
		 textFile << (*map)[i]->subelement;
	  }
	  else
	  {
         //textFile << (*map)[i]->objectName << "."
         //         << (*map)[i]->elementName << "."
         //         << (*map)[i]->subelement;
		 textFile << GetElementFullName((*map)[i], false);
	  }
	  textFile << " = " << outputEstimationState[i] << "\n";
   }
   textFile << "\n";


   /// 3. Write data editing criteria:
   textFile.precision(2);
   textFile << "Data Editing Criteria:\n"
	        << "   " << GetName() << ".OLSEInitialRMSSigma        = " << maxResidualMult << "\n"
	        << "   " << GetName() << ".OLSEMultiplicativeConstant = " << constMult << "\n"
			<< "   " << GetName() << ".OLSEAdditiveConstant       = " << additiveConst << "\n\n";

   /// 4. Write notations used in report file:
   textFile << "Notations Used In Report File: \n" 
            << "   N    : Not Edited \n"                                
            << "   U    : Unused Because No Computed Value Configuration Available \n"
            << "   R    : Out of Ramped Table Range \n"
            << "   BXY  : Blocked.  X = Path Index.  Y = Count Index (Doppler) \n"
            << "   IRMS : Edited by Initial RMS Sigma Filter \n"
            << "   OLSE : Edited by Outer-Loop Sigma Editor \n\n";

   /// 5. Write report header
   if (textFileMode == "Normal")
      textFile << "Iter      RecNum   UTCGregorian-Epoch       Obs Type           Units  Participants        Edit                     Obs (o)        Obs-Correction(O)                  Cal (C)       Residual (O-C)            Weight (W)             W*(O-C)^2         sqrt(W)*|O-C|      Elevation-Angle   \n";
   else
   {
      textFile << "Iter      RecNum   UTCGregorian-Epoch      TAIModJulian-Epoch        Obs Type           Units  Participants        Edit                     Obs (O)        Obs-Correction(O)                  Cal (C)       Residual (O-C)            Weight (W)             W*(O-C)^2         sqrt(W)*|O-C|      Elevation-Angle     Partial-Derivatives";
      // fill out N/A for partial derivative
	  for (int i = 0; i < esm.GetStateMap()->size()-1; ++i)
		 textFile << "                         ";
	  textFile << "   Uplink-Band         Uplink-Frequency             Range-Modulo         Doppler-Interval\n";
   }
   textFile.flush();

}


void BatchEstimator::WriteSummary(Solver::SolverState sState)
{
   const std::vector<ListItem*> *map = esm.GetStateMap();
   GmatState outputEstimationState;

   if (sState == ESTIMATING)
   {
      /// 1. Write state summary
      // Convert state to participants' coordinate system:
      GetEstimationState(outputEstimationState);
      // Write state to report file
	  Integer max_len = 15;
      for (int i = 0; i < map->size(); ++i) 
      {
	     std::stringstream ss;
	     if (((*map)[i]->object->IsOfType(Gmat::MEASUREMENT_MODEL))&&
             ((*map)[i]->elementName == "Bias"))
	     {
		    MeasurementModel* mm = (MeasurementModel*)((*map)[i]->object);
		    StringArray sa = mm->GetStringArrayParameter("Participants");
		    ss << mm->GetStringParameter("Type") << " ";
		    for( UnsignedInt j=0; j < sa.size(); ++j)
			   ss << sa[j] << (((j+1) != sa.size())?",":" Bias.");
		    ss << (*map)[i]->subelement;
	     }
	     else
		    ss << GetElementFullName((*map)[i], false);
		 max_len = GmatMathUtil::Max(max_len, ss.str().length());
	  }

      textFile << "\n";
      textFile << "Iteration " << iterationsTaken << ": State Information \n"
		       << "   " << GmatStringUtil::GetAlignmentString("State Component", max_len, GmatStringUtil::LEFT)                                                
			   << "               Apriori State              Previous State               Current State             Current-Apriori            Current-Previous\n";

      textFile.precision(8);
      for (int i = 0; i < map->size(); ++i) 
      {
	     textFile << "   ";
	     std::stringstream ss;
	     if (((*map)[i]->object->IsOfType(Gmat::MEASUREMENT_MODEL))&&
             ((*map)[i]->elementName == "Bias"))
	     {
		    MeasurementModel* mm = (MeasurementModel*)((*map)[i]->object);
		    StringArray sa = mm->GetStringArrayParameter("Participants");
		    ss << mm->GetStringParameter("Type") << " ";
		    for( UnsignedInt j=0; j < sa.size(); ++j)
			   ss << sa[j] << (((j+1) != sa.size())?",":" Bias.");
		    ss << (*map)[i]->subelement;
	     }
	     else
	     {
            //textFile << (*map)[i]->objectName << "."
            //         << (*map)[i]->elementName << "."
            //         << (*map)[i]->subelement;
		    ss << GetElementFullName((*map)[i], false);
	     }
	  
	     textFile << GmatStringUtil::GetAlignmentString(ss.str(), max_len + 3, GmatStringUtil::LEFT);
         textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(aprioriSolveForState[i]), 25, GmatStringUtil::RIGHT) << "   "			    // Apriori state
		          << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(previousSolveForState[i]), 25, GmatStringUtil::RIGHT) << "   "				// initial state
			      << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(currentSolveForState[i]), 25, GmatStringUtil::RIGHT) << "   "				// updated state
			      << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(currentSolveForState[i] - aprioriSolveForState[i]), 25, GmatStringUtil::RIGHT)  << "   "	// Apriori - Current state
				  << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(currentSolveForState[i] - previousSolveForState[i]), 25, GmatStringUtil::RIGHT) << "\n";	// Previous - Current state
      }
      textFile << "\n";


      /// 2. Write statistics
      textFile << "Iteration " << iterationsTaken << ":  Statistics \n"
	           << "   Total Number Of Records : " << GetMeasurementManager()->GetObservationDataList()->size() << "\n"
			   << "   Records Removed Due To :\n"
			   << "      No Computed Value Configuration Available : " << numRemovedRecords["U"] << "\n"
			   << "      Out of Ramped Table Range : " << numRemovedRecords["R"] << "\n"
			   << "      Signal Blocked : " << numRemovedRecords["B"] << "\n"
			   << "      Sigma Editing : " << ((iterationsTaken == 0)?numRemovedRecords["IRMS"]:numRemovedRecords["OLSE"]) << "\n"
		   	   << "   Records Used For Estimation : " << measurementResiduals.size() << "\n";
      textFile.precision(12);
      textFile << "   WeightedRMS Residuals : " << newResidualRMS << "\n"
               << "   PredictedRMS Residuals : " << predictedRMS << "\n";
   }

   if ((sState == CHECKINGRUN)||(sState == FINISHED))
   {
      textFile << "   DC Status : ";
      switch(estimationStatus)
      {
      case ABSOLUTETOL_CONVERGED:
	     textFile << "Absolute Tolerance Converged\n";
	     break;
      case RELATIVETOL_CONVERGED:
	     textFile << "Relative Tolerance Converged\n";
	     break;
      case ABS_AND_REL_TOL_CONVERGED:
	     textFile << "Absolute and Relative Tolerance Converged\n";
	     break;
      case MAX_CONSECUTIVE_DIVERGED:
	     textFile << "Maximum Consecutive Diverged\n";
	     break;
      case MAX_ITERATIONS_DIVERGED:
	     textFile << "Maximum Iterations Diverged\n";
	     break;
      case CONVERGING:
         textFile << "Converging\n";
	     break;
      case DIVERGING:
         textFile << "Diverging\n";
	     break;
      case UNKNOWN:
         textFile << "Unknown\n";
	     break;
      }
   }

   textFile.flush();

}


// made changes by TUAN NGUYEN
//-------------------------------------------------------------------------
// void DataFilter()
//-------------------------------------------------------------------------
/**
* This function is used to filter bad observation data records. It has
*   1. Measurement model's maximum residual filter
*   2. Data filter based on time span
*   3. Sigma editting
*/
//-------------------------------------------------------------------------
bool BatchEstimator::DataFilter()
{

   //MeasurementModel* measModel = measManager.GetMeasurementObject(modelsToAccess[0]);			// Get measurement model
   //Real maxLimit = measModel->GetRealParameter("ResidualMax");								// Get value of MeasurementModel.ResidualMax parameter
   const ObservationData *currentObs =  measManager.GetObsData();								// Get observation measurement data O
   const MeasurementData *calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);		// Get calculated measurement data C

   bool retVal = false;
   if (iterationsTaken == 0)
   {
      for (Integer i=0; i < currentObs->value.size(); ++i)
	  {
         //1.Data filtered based on measurement model's residual limits:
		 //if (abs(currentObs->value[i] - calculatedMeas->value[i]) > maxLimit)	// if (abs(O-C) > max limit) then throw away this data record
		 //{
		 //   measManager.GetObsDataObject()->inUsed = false;
		 //   measManager.GetObsDataObject()->removedReason = "M";	// "M": represent for maximum residual limit
		 //   std::string filterName = measModel->GetName() + " maximum residual";				// made changes by TUAN NGUYEN
		 //   if (numRemovedRecords.find(filterName) == numRemovedRecords.end())				// made changes by TUAN NGUYEN
         //      numRemovedRecords[filterName] = 1;												// made changes by TUAN NGUYEN
		 //   else																				// made changes by TUAN NGUYEN
		 //      numRemovedRecords[filterName]++;												// made changes by TUAN NGUYEN
		 //   retVal = true;		// IsReuseableType("ResidualMax");
         //   break;
		 //}

         // 2.Data filtered based on sigma editting
         // 2.1. Specify Weight
         Real weight = 1.0;
         if (currentObs->noiseCovariance == NULL)
         {
            if ((*(calculatedMeas->covariance))(i,i) != 0.0)
               weight = 1.0 / (*(calculatedMeas->covariance))(i,i);
            else
               weight = 1.0;
         }
         else
               weight = 1.0 / (*(currentObs->noiseCovariance))(i,i);
         // 2.2. Filter based on maximum residual multiplier
		 if (sqrt(weight)*abs(currentObs->value[i] - calculatedMeas->value[i]) > maxResidualMult)	// if (Wii*abs(O-C) > maximum residual multiplier) then throw away this data record
		 {
		    measManager.GetObsDataObject()->inUsed = false;
			measManager.GetObsDataObject()->removedReason = "IRMS";				// "IRMS": represent for OLSEInitialRMSSigma
			std::string filterName = "IRMS";

			retVal = true;
			break;
		 }
	  }
   }
   else
   {
	  for (Integer i=0; i < currentObs->value.size(); ++i)
	  {
         // Data filtered based on sigma editting
         // 1. Specify Weight
         Real weight = 1.0;
         if (currentObs->noiseCovariance == NULL)
         {
            if ((*(calculatedMeas->covariance))(i,i) != 0.0)
               weight = 1.0 / (*(calculatedMeas->covariance))(i,i);
            else
               weight = 1.0;
         }
         else
            weight = 1.0 / (*(currentObs->noiseCovariance))(i,i);

         // 2. Filter based on n-sigma
		 Real sigmaVal = (chooseRMSP ? predictedRMS : newResidualRMS);
		 if (sqrt(weight)*abs(currentObs->value[i] - calculatedMeas->value[i]) > (constMult*sigmaVal + additiveConst))	// if (Wii*abs(O-C) > k*sigma+ K) then throw away this data record
		 {
		    measManager.GetObsDataObject()->inUsed = false;
			measManager.GetObsDataObject()->removedReason = "OLSE";							// "OLSE": represent for outer-loop sigma filter

			retVal = true;
			break;
		 }
	  }
   }

   return retVal;
}


//------------------------------------------------------------------------------
// Integer SchurInvert(Real *sum1, Integer array_size)
//------------------------------------------------------------------------------
/**
 * Matrix inversion routine using the Schur identity
 *
 * This method is a port of the inversion code from GTDS, as ported by Angel
 * Wang of Thinking Systems and then integrated into GMAT by D. Conway.
 *
 * @param sum1 The matrix to be inverted, packed in upper triangular form
 * @param array_size The size of the sum1 array
 *
 * @return 0 on success, anything else indicates a problem
 */
//------------------------------------------------------------------------------
Integer BatchEstimator::SchurInvert(Real *sum1, Integer array_size)
{
   #ifdef DEBUG_SCHUR
      MessageInterface::ShowMessage("Performing Schur inversion\n   ");
      MessageInterface::ShowMessage("array_size = %d\n", array_size);
      MessageInterface::ShowMessage("Packed array:   [");
      for (UnsignedInt i = 0; i < array_size; ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage(", ");
         MessageInterface::ShowMessage("%.15le", sum1[i]);
      }
      MessageInterface::ShowMessage("]\n");
   #endif

   Integer retval = -1;

   // Check to see if the upper left element is invertible
   if ((array_size > 0) && (sum1[0] != 0.0))
   {
      Real *delta = new Real[array_size];
      Integer ij = 0, now = ij + 1;
      Integer rowCount = (Integer)((GmatMathUtil::Sqrt(1 + array_size*8)-1)/2);

      sum1[0] = 1.0/ sum1[0];
      if (rowCount > 1)
      {
         Integer i, i1, j, j1, jl, jn, l, l1, lPlus1, n, nn, nMinus1;
         Integer rowCountMinus1 = rowCount - 1;

         // Recursively invert the n X n matrix knowing the inverse of the
         // (n-1) X (n-1) matrix until the inverted matrix is found
         for (n = 2; n <=rowCount; ++n)
         {
            nMinus1 = n-1;
            l1 = 0;

            // Compute delta working arrays
            for (l = 1; l <= nMinus1; ++l)
            {
               j1 = 0;
               delta[l-1] = 0.0;

               for (j = 1; j <= l; ++j)
               {
                  jl = j1 + l-1;
                  jn = j1 + n-1;
                  delta[l-1] = delta[l-1] + (sum1[jl] * sum1[jn]);
                  j1 +=  rowCount - j;
               }

               if (l != nMinus1)
               {
                  lPlus1 = l + 1;

                  for (j = lPlus1; j <= nMinus1; ++j)
                  {
                     jn = j1 + n-1;
                     jl = l1 + j-1;
                     delta[l-1] += (sum1[jl] * sum1[jn]);
                     j1 += rowCount - j;
                  }
                  l1 += rowCount - l;
               }
            }
            j1 = n;
            nn = rowCountMinus1 + n;

            // Compute W
            for (j = 1; j <= nMinus1; ++j)
            {
               sum1[nn-1] -= (delta[j-1] * sum1[j1-1]);
               j1 += rowCount - j;
            }

            // Check if observation is '0'; if so, throw an exception
            now = n + ij;
            if (now > rowCount)
               if (ij != 0)
                  break;

            if (sum1[nn-1] == 0.0)
               continue;

            sum1[nn-1] = 1.0 / sum1[nn-1];
            j1 = n;

            // Compute Y
            for (j=1; j <= nMinus1; ++j)
            {
               sum1[j1-1] = -delta[j-1] * sum1[nn-1];
               j1 += rowCount - j;
            }

            // Compute X
            i1 = n;
            for (i=1; i <= nMinus1; ++i)
            {
               j1 = i;
               for (j=1; j <= i; ++j)
               {
                  sum1[j1-1] -= (sum1[i1-1] * delta[j-1]);
                  j1 += rowCount - j;
               }
               i1 += rowCount - i;
            }
            rowCountMinus1 += rowCount - n;
         }
      }
      delete [] delta;
      retval = 0;
   }
   else
   {
      if (array_size == 0)
         throw SolverException("Schur inversion cannot proceed; the size of "
               "the array being inverted is zero");

      if (sum1[0] == 0.0)
         throw SolverException("Schur inversion cannot proceed; the upper "
               "left element of the array being inverted is zero");
   }

   return retval;
}

//------------------------------------------------------------------------------
// Integer CholeskyInvert(Real* SUM1, Integer array_size)
//------------------------------------------------------------------------------
/**
 * Matrix inversion routine using Cholesky decomposition
 *
 * This method is a port of the inversion code from GEODYN, as ported by Angel
 * Wang of Thinking Systems and then integrated into GMAT by D. Conway.
 *
 * @param sum1 The matrix to be inverted, packed in upper triangular form
 * @param array_size The size of the sum1 array
 *
 * @return 0 on success, anything else indicates a problem
 */
//------------------------------------------------------------------------------
Integer BatchEstimator::CholeskyInvert(Real* sum1, Integer array_size)
{
   Integer retval = -1;

   Integer rowCount = (Integer)((GmatMathUtil::Sqrt(1 + array_size*8) - 1) / 2);
   Integer i, i1, i2, i3, ist, iERowCount, iError = 0, il, il1, il2;
   Integer j, k, k1, kl, iLeRowCount, rowCountIf, iPivot;
   Real dPivot, din, dsum, tolerance;
   Real work;

   const Real epsilon = 1.0e-8;

   rowCountIf = 0;
   j = 1;

   for (k = 1; k <= rowCount; ++k)
   {
      iLeRowCount = k - 1;
      tolerance = abs(epsilon * sum1[j-1]);
      for (i = k; i <= rowCount; ++i)
      {
         dsum = 0.0;
         if(k != 1)
         {
            for (il = 1; il <= iLeRowCount; ++il)
            {
               kl = k-il;
               il1 = (kl-1) * rowCount - (kl-1) * kl / 2;
               dsum = dsum + sum1[il1 + k - 1] * sum1[il1 + i - 1];
            }
         }
         dsum = sum1[j-1] - dsum;
         if (i > k)
            sum1[j -1] = dsum * dPivot;
         else if (dsum > tolerance)
         {
            dPivot = sqrt(dsum);
            sum1[j-1] = dPivot;
            dPivot = 1.0/dPivot;
         }
         else if (iError < 0)
         {
            iError = k-1;
            dPivot = GmatMathUtil::Sqrt(dsum);
            sum1[j-1] = dPivot;
            dPivot = 1.0 / dPivot;
         }
         else if (dsum < 0.0)
         {
            retval = 1;
            break;            // Throw here?
         }

         j = j + 1;
      }
      j = j + rowCountIf;
   }

   if (retval == -1)
   {
      // Invert R
      j = (rowCount - 1) * rowCount + (3 - rowCount) * rowCount/2;

      sum1[j-1] = 1.0 / sum1[j-1];
      iPivot = j;

      for (i = 2; i <= rowCount; ++i)
      {
         j = iPivot - rowCountIf;
         iPivot = j - i;
         din = 1.0 / sum1[iPivot-1];
         sum1[iPivot-1] = din;

         i1 = rowCount + 2 - i;
         i2 = i - 1;
         i3 = i1 - 1;
         il1 = (i3 - 1) * rowCount - (i3 - 1) * i3/2;
         for (k1 = 1; k1 <= i2; ++k1)
         {
            k = rowCount + 1 - k1;
            j = j - 1;
            work = 0.0;
            for (il = i1; il <= k; ++il)
            {
               il2 = (il - 1) * rowCount - (il - 1) * il/2 + k;
               work = work + sum1[il1+il-1] * sum1[il2-1];
            }
            sum1[j-1] = -din * work;
         }
      }

      // Inverse(A) = INV(R) * TRN(INV(R));
      il = 1;
      for (i = 1; i <= rowCount; ++i)
      {
         il1 = (i - 1) * rowCount - (i - 1) * i/2;
         for (j = i; j <= rowCount; ++j)
         {
            il2 = (j - 1) * rowCount - (j - 1) * j/2;
            work = 0.0;
            for (k = j; k <= rowCount; ++k)
               work = work + sum1[il1+k-1] * sum1[il2+k-1];

            sum1[il-1] = work;
            il = il + 1;
         }
         il = il + rowCountIf;
      }
      retval = 0;
   }

   return retval;
}
