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
//#include "FileManager.hpp"
#include <sstream>
#include "SpaceObject.hpp"    // To access epoch data

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
   // todo Add useApriori here
};

const Gmat::ParameterType
BatchEstimator::PARAMETER_TYPE[] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
//   Gmat::ON_OFF_TYPE,												// made changes by TUAN NGUYEN
   Gmat::STRING_TYPE,
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
   newResidualRMS             (0.0),
   useApriori                 (false),						// second term of Equation Eq8-184 in GTDS MathSpec is not used	
   advanceToEstimationEpoch   (false),
   converged                  (false),
   chooseRMSP                 (true),
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
   newResidualRMS             (0.0),
   useApriori                 (est.useApriori),
   advanceToEstimationEpoch   (false),
   converged                  (false),
   chooseRMSP                 (est.chooseRMSP),
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
      converged                = false;
	  chooseRMSP               = est.chooseRMSP;

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
         if (value == "Cholesky")
            throw SolverException("The Cholesky inversion routine is not yet "
                  "integrated into the batch estimator code");
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
      converged   = false;

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
      converged = false;
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
      bool measOK = measManager.Initialize();
      if (!measOK)
         throw SolverException(
               "BatchEstimator::CompleteInitialization - error initializing "
               "MeasurementManager.\n");

      // Now load up the observations
      measManager.PrepareForProcessing();
      UnsignedInt numRec = measManager.LoadObservations();					// made changes by TUAN NGUYEN
	  if (numRec == 0)														// made changes by TUAN NGUYEN
         throw EstimatorException("No observation data is used for estimation\n");	// made changes by TUAN NGUYEN

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

   converged   = false;
   isInitialized = true;

   // Open report file											// made changes by TUAN NGUYEN
   //if (reportFile.is_open() == false)							// made changes by TUAN NGUYEN
   //{

   //   if (reportFilename != "")
   //   {
   //      std::string fullPath = "";

   //      // If no path designation slash character is found, add the default path
   //      if ((reportFilename.find('/') == std::string::npos) &&
   //          (reportFilename.find('\\') == std::string::npos))
   //      {
   //         FileManager *fm = FileManager::Instance();
   //         fullPath = fm->GetPathname(FileManager::MEASUREMENT_PATH);
   //      }
   //      fullPath += reportFilename;
   //      reportFile.open(fullPath, std::ios_base::out);		// made changes by TUAN NGUYEN
	  //}
   //}


   std::stringstream sLine;
   // Write out maximum residual for each measuement model
   std::vector<MeasurementModel*> modelsList = measManager.GetAllMeasurementModels();  
   sLine << "\nData Editing criteria:\n";
   for (int i = 0; i < modelsList.size(); ++i)
      sLine << "   " << modelsList[i]->GetName() <<".ResidualMax = " << modelsList[i]->GetRealParameter("ResidualMax") << "\n";

   // Write out OLSE Initial RMS: 
   sLine << "   " << GetName() <<".OLSEInitialRMSSigma        = " << maxResidualMult << "\n";
   sLine << "   " << GetName() <<".OLSEMultiplicativeConstant = " << constMult << "\n";
   sLine << "   " << GetName() <<".OLSEAdditiveConstant       = " << additiveConst << "\n\n";

   // Write out all notations used in table:
   sLine << "Notaions used in report file: \n";
   sLine << "  N: Normal\n";
   sLine << "  U: Unmatched to any measurement model\n";
   sLine << "  R: Out of ramped table\n";
   sLine << "  ET: measurement unfeasible due to the block of transmitted signal\n";
   sLine << "  ER: measurement unfeasible due to the block of received signal\n";
   sLine << "  EST: measurement unfeasible due to the block of Start path's transmitted signal\n";
   sLine << "  ESR: measurement unfeasible due to the block of Start path's received signal\n";
   sLine << "  EET: measurement unfeasible due to the block of Etart path's transmitted signal\n";
   sLine << "  EER: measurement unfeasible due to the block of Etart path's received signal\n";
   sLine << "  M: Edited by measurement maximum residual\n";
   sLine << "  I: Edited by Initial RMS Sigma filter\n";
   sLine << "  S: Edited by Outer-Loop Sigma Editing\n\n";

   // Write out the header
   sLine << "Iter   RecNum   Epoch                Meas-Type      Part.s     Edit-Criteria Band   Uplink-Frequency         RangeModule              Doppler-Interval         Obs Measuement (O)   Cal Measurement (C)      Residual (O-C)   Weight (W)            W*(O-C)^2             sqrt(W)*|O-C|            Elevation-Angle   Partial-Derivative Vector\n";
   
   linesBuff = sLine.str();

   //if (reportFile.is_open())
   //{
	  // reportFile << sLine.str();
	  // reportFile.flush();
   //}

   WriteToTextFile();
   ReportProgress();

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

   if (measManager.CalculateMeasurements() == false)
   {
      // No measurements were possible
      bool endOfDataSet = measManager.AdvanceObservation();
      if (endOfDataSet)										// made changes by TUAN NGUYEN
         currentState = ESTIMATING;							// made changes by TUAN NGUYEN
	  else													// made changes by TUAN NGUYEN
	  {														// made changes by TUAN NGUYEN
         nextMeasurementEpoch = measManager.GetEpoch();
         FindTimeStep();
	     if (currentEpoch <= nextMeasurementEpoch)
            currentState = PROPAGATING;
         else
            currentState = ESTIMATING;
	  }														// made changes by TUAN NGUYEN
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
   converged = TestForConvergence(convergenceReason);

   #ifdef RUN_SINGLE_PASS
      converged = true;
   #endif

   ++iterationsTaken;
   if ((converged) || (iterationsTaken >= maxIterations))
   {
#ifdef DEBUG_VERBOSE
      if (converged)
         MessageInterface::ShowMessage("Estimation has converged\n%s\n\n",
                  convergenceReason.c_str());
      else
         MessageInterface::ShowMessage("Estimation has reached the maximum "
                  "iteration count, but has not converged\n\n");
#endif
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
	  if (measManager.GetObsDataObject()->inUsed == false)			// if the first observation data is not in used, then go to the next in-used data record	// made changes by TUAN NGUYEN
	  {
		 measManager.AdvanceObservation();							// made changes by TUAN NGUYEN
	  }
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

   //// Close report file
   //reportFile.close();										// made changes by TUAN NGUYEN
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
                           << "): " << estEpoch << "\n\n";
               else
                  progress << "   Estimation Epoch (A.1 modified Julian): "
                           << estimationEpoch << "\n\n";

			   GetEstimationState(outputEstimationState);						// made changes by TUAN NGUYEN
               
			   for (UnsignedInt i = 0; i < map->size(); ++i)
               {
                  progress << "   "
                           << (*map)[i]->objectName << "."
                           << (*map)[i]->elementName << "."
                           << (*map)[i]->subelement << " = "
//                           << (*estimationState)[i] << "\n";					// made changes by TUAN NGUYEN
                           << outputEstimationState[i] << "\n";					// made changes by TUAN NGUYEN
               };

               progress << "\n a priori covariance:\n\n";
               Rmatrix aPrioriCovariance = *(stateCovariance->GetCovariance());
               for (Integer i = 0; i < aPrioriCovariance.GetNumRows(); ++i)
               {
                  progress << "----- Row " << (i+1) << "\n";
                  for (Integer j = 0; j < aPrioriCovariance.GetNumColumns(); ++j)
                     progress << "   " << aPrioriCovariance(i, j);
                  progress << "\n";
               }
            }
            break;

         case CHECKINGRUN:
            progress << "\n   WeightedRMS residuals for this iteration: "
                     << newResidualRMS;
            progress << "\n   PredictedRMS residuals for next iteration: "
                     << predictedRMS << "\n\n";

            progress << "------------------------------"
                     << "------------------------\n"
                     << "Iteration " << iterationsTaken
                     << "\n\nCurrent estimated state:\n";
            progress << "   Estimation Epoch: "
                     << estimationEpoch << "\n";

			GetEstimationState(outputEstimationState);							// made changes by TUAN NGUYEN

            for (UnsignedInt i = 0; i < map->size(); ++i)
            {
               progress << "   "
//                        << (*estimationState)[i];								// made changes by TUAN NGUYEN
			              << outputEstimationState[i];							// made changes by TUAN NGUYEN
            }

            break;

         case FINISHED:
            progress << "\n   WeightedRMS residuals for this iteration: "
                     << newResidualRMS;
            progress << "\n   PredictedRMS residuals for next iteration: "
                     << predictedRMS << "\n\n";

            progress << "\n****************************"
                     << "****************************\n"
                     << "*** Estimating Completed in " << iterationsTaken
                     << " iterations"
                     << "\n****************************"
                     << "****************************\n\n"
                     << "Estimation "
                     << (converged ? "converged!" : "did not converge") << "\n"
					 << convergenceReason << "\n"
                     << "\nFinal Estimated State:\n\n";

            if (estEpochFormat != "FromParticipants")
               progress << "   Estimation Epoch (" << estEpochFormat
                        << "): " << estEpoch << "\n\n";
            else
               progress << "   Estimation Epoch (A.1 modified Julian): "
                        << estimationEpoch << "\n\n";

			GetEstimationState(outputEstimationState);							// made changes by TUAN NGUYEN

            for (UnsignedInt i = 0; i < map->size(); ++i)
            {
               progress << "   "
                        << (*map)[i]->objectName << "."
                        << (*map)[i]->elementName << "."
                        << (*map)[i]->subelement << " = "
//                        << (*estimationState)[i] << "\n";						// made changes by TUAN NGUYEN
			            << outputEstimationState[i] << "\n";					// made changes by TUAN NGUYEN
            }

            { // Switch statement scoping
               Rmatrix finalCovariance = information.Inverse();
               progress << "\nFinal Covariance Matrix:\n\n";
               for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
               {
                  progress << "----- Row " << (i+1) << "\n";
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
                        << newResidualRMS << "\n\n";
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


//------------------------------------------------------------------------------
// bool TestForConvergence(std::string &reason)
//------------------------------------------------------------------------------
/**
 * Provides the default convergence test for the BatchEstimators.
 *
 * GMAT's batch estimator report a converged solution when any of the following
 * criteria are met:
 *
 * 1.  The change in the root mean square of the sum of the measurement
 * residuals is less than a user specified relative change tolerance.
 *
 * 2.  The L2 norm of the change in the estimation state vector is less than a
 * specified absolute tolerance.
 *
 * This method indicates that at least one of these criteria is met by returning
 * true; if no convergence criteria are met, the return value is false.
 *
 * Derived classes can override this method to specify other criteria as needed.
 *
 * @param reason A string that is set to indicate the criterion that was met
 *               when convergence is detected.  The string contains text for all
 *               criteria that are satisfied.
 *
 * @return true if the estimator has converged, false if not
 */
//------------------------------------------------------------------------------
bool BatchEstimator::TestForConvergence(std::string &reason)
{
   bool retval = false;
   std::stringstream why;

   if (GmatMathUtil::Abs((newResidualRMS - oldResidualRMS)/oldResidualRMS) <= relativeTolerance)
   {
      why << "   (WeightedRMS - oldWeightedRMS)/oldWeightedRMS = (" << newResidualRMS << " - " 
		  << oldResidualRMS << ")/ " << oldResidualRMS << " = " 
          << GmatMathUtil::Abs((newResidualRMS - oldResidualRMS)/oldResidualRMS)
          << " is less than RelativeTol, "
          << relativeTolerance << "\n";

	  reason = why.str();
      retval = true;
   }

   if (newResidualRMS <= absoluteTolerance)
   {
      why << "   WeightedRMS residual, " << newResidualRMS
          << " is within the AbsoluteTol, " << absoluteTolerance
          << "\n";

	  reason = why.str();
      retval = true;
   }

   if ((iterationsTaken + 1) >= maxIterations)
   {
	   reason =  "Estimation has reached the maximum iteration count\n";
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
   GmatState outputEstimationState;											// made changes by TUAN NGUYEN

   if (!showProgress)
      return;

   if (!textFile.is_open())
      OpenSolverTextFile();

   Solver::SolverState theState = sState;
   if (sState == Solver::UNDEFINED_STATE)
      theState = currentState;

   const std::vector<ListItem*> *map = esm.GetStateMap();

   switch (theState)
   {
      case INITIALIZING:
         textFile << "\n****************************"
                  << "****************************\n"
                  << "*** Initializing Estimation \n"
                  << "****************************"
                  << "****************************\n"
                  << "\n\na priori state:\n";
         if (estEpochFormat != "FromParticipants")
            textFile << "   Estimation Epoch (" << estEpochFormat
                     << "): " << estEpoch << "\n\n";
         else
            textFile << "   Estimation Epoch (A.1 modified Julian): "
                     << estimationEpoch << "\n\n";


         GetEstimationState(outputEstimationState);							// made changes by TUAN NGUYEN

         for (UnsignedInt i = 0; i < map->size(); ++i)
         {
            textFile << "   "
                     << (*map)[i]->objectName << "."
                     << (*map)[i]->elementName << "."
                     << (*map)[i]->subelement << " = "
//                     << (*estimationState)[i] << "\n";					// made changes by TUAN NGUYEN
                     << outputEstimationState[i] << "\n";					// made changes by TUAN NGUYEN
         }

		 if (textFileMode == "Debug")
		 {
			 //Write out all information for Debug mode here
			 textFile << linesBuff;
			 textFile.flush();
		 }

         break;
	  case ACCUMULATING:
		 if (textFileMode == "Debug")
		 {
			// Write detail about observation data and estimation calculation for that record here
			textFile << linesBuff;
			textFile.flush();
		 }
		 break;
	  case ESTIMATING:
		 if (textFileMode == "Debug")
		 {
			 // Write detail summary about estimation of current iteration here
			 textFile << linesBuff;
			 textFile.flush();
		 }
		 break;
      case CHECKINGRUN:
         if ((textFileMode == "Verbose")||(textFileMode == "Debug"))
         {
            textFile << "\n   WeightedRMS residuals for this iteration: "
                     << newResidualRMS;
            textFile << "\n   PredictedRMS residuals for next iteration: "
                     << predictedRMS << "\n\n";

			textFile << "------------------------------"
                      << "------------------------\n"
                      << "Iteration " << iterationsTaken
                      << "\n\nCurrent estimated state:\n";
            if (estEpochFormat != "FromParticipants")
               textFile << "   Estimation Epoch (" << estEpochFormat
                        << "): " << estEpoch << "\n\n";
            else
               textFile << "   Estimation Epoch (A.1 modified Julian): "
                        << estimationEpoch << "\n\n";


            GetEstimationState(outputEstimationState);							// made changes by TUAN NGUYEN

            for (UnsignedInt i = 0; i < map->size(); ++i)
            {
               textFile << "   "
                        << (*map)[i]->objectName << "."
                        << (*map)[i]->elementName << "."
                        << (*map)[i]->subelement << " = "
//                        << (*estimationState)[i] << "\n";
                        << outputEstimationState[i] << "\n";					// made changes by TUAN NGUYEN
            }

         }
         break;

      case FINISHED:
         if ((textFileMode == "Verbose")||(textFileMode == "Debug"))
         {
            textFile << "\n   WeightedRMS residuals for this iteration: "
                     << newResidualRMS;
            textFile << "\n   PredictedRMS residuals for next iteration: "
                     << predictedRMS << "\n\n";
		 }

         textFile << "\n****************************"
                  << "****************************\n"
                  << "*** Estimating Completed in " << iterationsTaken
                  << " iterations"
                  << "\n****************************"
                  << "****************************\n\n"
                  << "Estimation "
                  << (converged ? "converged!" : "did not converge") << "\n"
				  << convergenceReason << "\n"
                  << "\n\nFinal Estimated State:\n\n";

         if (estEpochFormat != "FromParticipants")
            textFile << "   Estimation Epoch (" << estEpochFormat
                     << "): " << estEpoch << "\n\n";
         else
            textFile << "   Estimation Epoch (A.1 modified Julian): "
                     << estimationEpoch << "\n\n";

		 
		 GetEstimationState(outputEstimationState);							// made changes by TUAN NGUYEN

         for (UnsignedInt i = 0; i < map->size(); ++i)
         {
            textFile << "   "
                     << (*map)[i]->objectName << "."
                     << (*map)[i]->elementName << "."
                     << (*map)[i]->subelement << " = "
//                     << (*estimationState)[i] << "\n";					// made changes by TUAN NGUYEN
                     << outputEstimationState[i] << "\n";					// made changes by TUAN NGUYEN
         }

         { // Switch statement scoping
            Rmatrix finalCovariance = information.Inverse();
            textFile << "\nFinal Covariance Matrix:\n\n";
            for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
            {
               for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
                  textFile << "   " << finalCovariance(i, j);
               textFile << "\n";
            }
         }

         if ((textFileMode == "Verbose")||(textFileMode == "Debug"))
         {
            textFile << "\n   WeightedRMS residuals for previous iteration: "
                     << oldResidualRMS;
            textFile << "\n   WeightedRMS residuals for this iteration:     "
                     << newResidualRMS << "\n\n";
         }

         textFile << "\n****************************"
                  << "****************************\n\n"
                  << std::endl;

         break;

      default:
         break;
   }
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

   MeasurementModel* measModel = measManager.GetMeasurementObject(modelsToAccess[0]);			// Get measurement model
   Real maxLimit = measModel->GetRealParameter("ResidualMax");									// Get value of MeasurementModel.ResidualMax parameter
   const ObservationData *currentObs =  measManager.GetObsData();								// Get observation measurement data O
   const MeasurementData *calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);		// Get calculated measurement data C

   bool retVal = false;
   if (iterationsTaken == 0)
   {
      for (Integer i=0; i < currentObs->value.size(); ++i)
	  {
         // 1.Data filtered based on measurement model's residual limits:
		 if (abs(currentObs->value[i] - calculatedMeas->value[i]) > maxLimit)	// if (abs(O-C) > max limit) then throw away this data record
		 {
//			MessageInterface::ShowMessage("Throw away this observation data due to measurement model's residual limit %lf:  %.12lf   %s   %d", maxLimit, currentObs->epoch, currentObs->typeName.c_str(), currentObs->type);
//			for(int k = 0; k < currentObs->participantIDs.size(); ++k)
//			   MessageInterface::ShowMessage("   %s", currentObs->participantIDs[k].c_str());
//			for(int k = 0; k < currentObs->value.size(); ++k)
//			   MessageInterface::ShowMessage("   %.8lf", currentObs->value[k]);
//			MessageInterface::ShowMessage("\n");

		    measManager.GetObsDataObject()->inUsed = false;
			measManager.GetObsDataObject()->removedReason = "M";	// "M": represent for maximum residual limit
			std::string filterName = measModel->GetName() + " maximum residual";				// made changes by TUAN NGUYEN
		    if (numRemovedRecords.find(filterName) == numRemovedRecords.end())					// made changes by TUAN NGUYEN
               numRemovedRecords[filterName] = 1;												// made changes by TUAN NGUYEN
			else																				// made changes by TUAN NGUYEN
			   numRemovedRecords[filterName]++;													// made changes by TUAN NGUYEN

			retVal = IsReuseableType("ResidualMax");
            break;
		 }

         // 2.Data filtered based on timespan:
         Real epoch1 = TimeConverterUtil::Convert(estimationStart, TimeConverterUtil::A1MJD, currentObs->epochSystem);
         Real epoch2 = TimeConverterUtil::Convert(estimationEnd, TimeConverterUtil::A1MJD, currentObs->epochSystem);
         if ((currentObs->epoch < epoch1)||(currentObs->epoch > epoch2))
         {
//			MessageInterface::ShowMessage("Throw away this observation data due to time span [%.12lf, %.12lf]:  %.12lf   %s   %d", epoch1, epoch2, currentObs->epoch, currentObs->typeName.c_str(), currentObs->type);
//			for(int k = 0; k < currentObs->participantIDs.size(); ++k)
//			   MessageInterface::ShowMessage("   %s", currentObs->participantIDs[k].c_str());
//			for(int k = 0; k < currentObs->value.size(); ++k)
//			   MessageInterface::ShowMessage("   %.8lf", currentObs->value[k]);
//			MessageInterface::ShowMessage("\n");

		    measManager.GetObsDataObject()->inUsed = false;
			measManager.GetObsDataObject()->removedReason = "T";	// "T": represent for time span
			std::string filterName = "Timespan";
		    if (numRemovedRecords.find(filterName) == numRemovedRecords.end())					// made changes by TUAN NGUYEN
               numRemovedRecords[filterName] = 1;												// made changes by TUAN NGUYEN
			else																				// made changes by TUAN NGUYEN
			   numRemovedRecords[filterName]++;													// made changes by TUAN NGUYEN

			retVal = IsReuseableType("Timespan");
			break;
         }

         // 3.Data filtered based on sigma editting
         // 3.1. Specify Weight
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
         // 3.2. Filter based on maximum residual multiplier
		 if (sqrt(weight)*abs(currentObs->value[i] - calculatedMeas->value[i]) > maxResidualMult)	// if (Wii*abs(O-C) > maximum residual multiplier) then throw away this data record
		 {
//			MessageInterface::ShowMessage("Throw away this observation data due to: sqrt(Wjj)*|O-C| > MaximumConstantMultiplier :  %.12lf   %s   %d", currentObs->epoch, currentObs->typeName.c_str(), currentObs->type);
//			for(int k = 0; k < currentObs->participantIDs.size(); ++k)
//			   MessageInterface::ShowMessage("   %s", currentObs->participantIDs[k].c_str());
//			for(int k = 0; k < currentObs->value.size(); ++k)
//			   MessageInterface::ShowMessage("   %.8lf", currentObs->value[k]);
//			MessageInterface::ShowMessage("\n");

		    measManager.GetObsDataObject()->inUsed = false;
			measManager.GetObsDataObject()->removedReason = "I";				// "I": represent for OLSEInitialRMSSigma
			std::string filterName = "OLSEInitialRMSSigma";
		    if (numRemovedRecords.find(filterName) == numRemovedRecords.end())					// made changes by TUAN NGUYEN
               numRemovedRecords[filterName] = 1;												// made changes by TUAN NGUYEN
			else																				// made changes by TUAN NGUYEN
			   numRemovedRecords[filterName]++;													// made changes by TUAN NGUYEN

			retVal = IsReuseableType("OLSEInitialRMSSigma");
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
			measManager.GetObsDataObject()->removedReason = "S";							// "S": represent for outer-loop sigma filter
            numRemovedRecords["OLSE"]++;													// made changes by TUAN NGUYEN

			std::stringstream ss;
			ss << "OLSE for Iteration " << iterationsTaken;									// made changes by TUAN NGUYEN

			if (numRemovedRecords.find(ss.str()) == numRemovedRecords.end())				// made changes by TUAN NGUYEN
               numRemovedRecords[ss.str()] = 1;												// made changes by TUAN NGUYEN
			else																			// made changes by TUAN NGUYEN
			   numRemovedRecords[ss.str()]++;												// made changes by TUAN NGUYEN

			retVal = IsReuseableType("OLSE");
			break;
		 }

//       MessageInterface::ShowMessage("    At epoch = %.12lf:   O-C = %.12lf  -  %.12lf = %.12lf\n", currentObs->epoch, currentObs->value[i], calculatedMeas->value[i], currentObs->value[i] - calculatedMeas->value[i]);
	  }

   }

   return retVal;
}


bool BatchEstimator::IsReuseableType(const std::string& value)
{
   bool isReuseable = false;
   for (int i = 0; i < reuseableTypes.size(); ++i)
   {
      if (reuseableTypes[i] == value)
      {
         isReuseable = true;
		 break;
	  }
   }

   return isReuseable;
}



//------------------------------------------------------------------------------
// Integer SchurInvert(Real *sum1, Integer array_size)
//------------------------------------------------------------------------------
/**
 * Matrix inversion routine using the Schur identity
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
      Integer rowCount = (Integer)((std::sqrt(1 + array_size * 8) - 1) / 2);

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
