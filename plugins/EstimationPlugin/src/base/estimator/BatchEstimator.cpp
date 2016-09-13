//$Id: BatchEstimator.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         BatchEstimator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
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
#include "Spacecraft.hpp"
#include "StringUtil.hpp"
#include "StateConversionUtil.hpp"
#include "GroundstationInterface.hpp"
#include "SolarRadiationPressure.hpp"
#include "ODEModel.hpp"
#include "Propagator.hpp"
#include "CalculationUtilities.hpp"
#include "GravityField.hpp"
#include "ErrorModel.hpp"
#include "EstimatorException.hpp"
#include "FileManager.hpp"

#include "DataWriterInterface.hpp"

#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>


// This is used for getting computer operating system name and version
#ifdef __linux__
#include <sys/utsname.h>
#else
#ifdef __APPLE__
#include <sys/utsname.h>
#endif
#endif

// This is used for getting hostname and user ID
#ifdef __linux__
#include <unistd.h>
#endif
#ifdef __APPLE__
#include <unistd.h>
#endif
#ifdef _MSC_VER
#include <WinSock2.h>
#endif


//#define DEBUG_STATE_MACHINE
//#define DEBUG_SIMULATOR_WRITE
//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION
//#define DEBUG_EVENT
//#define DEBUG_ACCUMULATION_RESULTS
//#define DEBUG_PROPAGATION
//#define DEBUG_DATA_FILTER

// Macros for debugging of the state machine
//#define WALK_STATE_MACHINE
//#define DEBUG_VERBOSE
//#define RUN_SINGLE_PASS
//#define DUMP_FINAL_RESIDUALS


//#define SPACECRAFT_TABLE_COLUMN_BREAK_UP             1
#define CELESTIAL_BODIES_TABLE_COLUMN_BREAK_UP       5
#define MAX_COLUMNS                                  7                 // covariance matrix column

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
BatchEstimator::PARAMETER_TEXT[] =
{
   "EstimationEpochFormat",         // The epoch of the solution
   "EstimationEpoch",               // The epoch of the solution
//   "UsePrioriEstimate",
   "UseInitialCovariance",
   "InversionAlgorithm",
   "MaxConsecutiveDivergences",
   "MatlabFile",
   // todo Add useApriori here
};

const Gmat::ParameterType
BatchEstimator::PARAMETER_TYPE[] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
//   Gmat::ON_OFF_TYPE,        // "UsePrioriEstimate"
   Gmat::BOOLEAN_TYPE,         // "UseInitialCovariance"
   Gmat::STRING_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::FILENAME_TYPE,        // MATLAB_OUTPUT_FILENAME
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
   estEpoch                   ("FromParticipants"),
   oldResidualRMS             (0.0),
   newResidualRMS             (1.0e12),
   useApriori                 (false),                  // second term of Equation Eq8-184 in GTDS MathSpec is not used   
   advanceToEstimationEpoch   (false),
//   converged                  (false),
//   estimationStatus           (UNKNOWN),
   chooseRMSP                 (true),
   maxConsDivergences         (3),
   inversionType              ("Internal"),
   matWriter                  (NULL),
   writeMatFile               (false),
   matFileName                (""),
   matIterationIndex          (-1),
   matPartIndex               (-1),
   matTypeIndex               (-1),
   matEpochIndex              (-1),
   matObsIndex                (-1),
   matCalcIndex               (-1),
   matOmcIndex                (-1),
   matElevationIndex          (-1),
   matGregorianIndex          (-1),
   matObsEditFlagIndex        (-1),
   matFrequencyIndex          (-1),
   matFreqBandIndex           (-1),
   matDoppCountIndex          (-1)
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
  if (matWriter != NULL)
     delete matWriter;
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
//   estimationStatus           (UNKNOWN),
   chooseRMSP                 (est.chooseRMSP),
   maxConsDivergences         (est.maxConsDivergences),
   inversionType              (est.inversionType),
   matWriter                  (NULL),
   writeMatFile               (est.writeMatFile),
   matFileName                (est.matFileName),
   matIterationIndex          (-1),
   matPartIndex               (-1),
   matTypeIndex               (-1),
   matEpochIndex              (-1),
   matObsIndex                (-1),
   matCalcIndex               (-1),
   matOmcIndex                (-1),
   matElevationIndex          (-1),
   matGregorianIndex          (-1),
   matObsEditFlagIndex        (-1),
   matFrequencyIndex          (-1),
   matFreqBandIndex           (-1),
   matDoppCountIndex          (-1)
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
//      estimationStatus         = UNKNOWN;

      chooseRMSP               = est.chooseRMSP;
      maxConsDivergences       = est.maxConsDivergences;

      // Clear the loop buffer
      for (UnsignedInt i = 0; i < outerLoopBuffer.size(); ++i)
         delete outerLoopBuffer[i];
      outerLoopBuffer.clear();

      inversionType = est.inversionType;

      if (matWriter != NULL)
         delete matWriter;
      matWriter           = NULL;
      matIterationIndex   = -1;
      matPartIndex        = -1;
      matTypeIndex        = -1;
      matEpochIndex       = -1;
      matObsIndex         = -1;
      matCalcIndex        = -1;
      matOmcIndex         = -1;
      matGregorianIndex   = -1;
      matObsEditFlagIndex = -1;
      matElevationIndex   = -1;
      matGregorianIndex   = -1;
      matObsEditFlagIndex = -1;
      matFrequencyIndex   = -1;
      matFreqBandIndex    = -1;
      matDoppCountIndex   = -1;

      writeMatFile = est.writeMatFile;
      matFileName  = est.matFileName;
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
//  Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns value of an integer parameter given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return value of the requested parameter.
 */
//------------------------------------------------------------------------------
Integer BatchEstimator::GetIntegerParameter(const Integer id) const
{
   if (id == MAX_CONSECUTIVE_DIVERGENCES)
      return maxConsDivergences;

   return Estimator::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
//  Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * This method sets value to an integer parameter specified by the input parameter ID.
 *
 * @param id       ID for the requested parameter.
 * @param value    integer value used to set to the request parameter. 
 *
 * @return value set to the requested parameter.
 */
//------------------------------------------------------------------------------
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
//  Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* This method gets value of an integer parameter specified by parameter name.
*
* @param label    name of parameter.
*
* @return         value of an integer parameter.
*/
//------------------------------------------------------------------------------
Integer BatchEstimator::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
* This method sets value to an integer parameter specified by the input parameter name.
*
* @param label    name for the requested parameter.
* @param value    integer value used to set to the request parameter.
*
* @return value set to the requested parameter.
*/
//------------------------------------------------------------------------------
Integer BatchEstimator::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
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

   if (id == MATLAB_OUTPUT_FILENAME)
      return matFileName;

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
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("BatchEstimator::SetStringParameter(%d, %s)\n",
            id, value.c_str());
   #endif

   if (id == ESTIMATION_EPOCH_FORMAT)
   {
      if (value != "FromParticipants")
         throw EstimatorException("Error: An invalid value (" + value + ") was set to " + GetName() + ".EstimationEpochFormat parameter. In current GMAT version, only 'FromParticipants' is a valid value.\n");

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
         throw EstimatorException("The requested inversion routine is not an "
               "allowed value for the field \"InversionAlgorithm\"; allowed "
               "values are \"Internal\", \"Schur\" and \"Cholesky\"");
   }

   if (id == ESTIMATION_EPOCH)
   {
      if (value != "FromParticipants")
         throw EstimatorException("Error: An invalid value (" + value + ") was set to " + GetName() + ".EstimationEpoch parameter. In current GMAT version, only 'FromParticipants' is a valid value.\n");

      if (value == "")
         throw EstimatorException("Error: No value was set to " + GetName() + ".EstimationEpoch parameter.\n");

      if (estEpochFormat == "FromParticipants")
      {
         MessageInterface::ShowMessage("Setting value for %s.EstimationEpoch has no "
               "effect due to %s.EstimationEpochFormat to be \"%s\"\n", 
               GetName().c_str(), GetName().c_str(), estEpochFormat.c_str());
      }
      else
      {
         estEpoch = value;
         if (TimeConverterUtil::IsValidTimeSystem(estEpochFormat))
         {
            // Convert to a.1 time for internal processing
            estimationEpoch = ConvertToRealEpoch(estEpoch, estEpochFormat);
         }
         else
            throw EstimatorException("Error: Cannot set value '" + value + " to " + GetName() + ".EstimationEpoch parameter due to its invalid time format.\n");
      }

      return true;
   }

   if (id == MATLAB_OUTPUT_FILENAME)
   {
      matFileName = value;
      if (matFileName.find(".mat") == std::string::npos)
         matFileName += ".mat";
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
// bool BatchEstimator::GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method gets value from a boolean parameter 
 *
 * @param id   The id number of a parameter
 *
 * @return     a boolean value
 */
//------------------------------------------------------------------------------
bool BatchEstimator::GetBooleanParameter(const Integer id) const
{
   if (id == USE_INITIAL_COVARIANCE)
      return useApriori;

   return Estimator::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool BatchEstimator::SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * This method sets value to a boolean parameter
 *
 * @param id         The id number of a parameter
 * @param value      value set to the parameter
 *
 * @return true value when it successfully sets the value, false otherwise. 
 */
//------------------------------------------------------------------------------
bool BatchEstimator::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == USE_INITIAL_COVARIANCE)
   {
      useApriori = value;
      return true;
   }

   return Estimator::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// bool BatchEstimator::GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* This method gets value from a boolean parameter
*
* @param label   The name of a parameter
*
* @return        value of the parameter
*/
//------------------------------------------------------------------------------
bool BatchEstimator::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool BatchEstimator::SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
* This method sets value to a boolean parameter
*
* @param label      The name of a parameter
* @param value      value used to set to parameter
*
* @return true value when it successfully sets the value, false otherwise.
*/
//------------------------------------------------------------------------------
bool BatchEstimator::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
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

      StringArray nameList = TimeConverterUtil::GetValidTimeRepresentations();
      for (UnsignedInt i = 0; i < nameList.size(); ++i)
         enumStrings.push_back(nameList[i]);

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
      retval    = true;

      if (matFileName != "")
      {
         // the mat writer
         matWriter = DataWriterInterface::Instance()->GetDataWriter("MatWriter");
         if (matWriter != NULL)
         {
            writeMatFile = true;
            // Add default path is there is no path data in matFileName
            if ((matFileName.find("/") == std::string::npos) &&
                  (matFileName.find("\\") == std::string::npos))
            {
               FileManager *fileman = FileManager::Instance();
               std::string path = fileman->GetPathname(FileManager::OUTPUT_PATH);
               matFileName = path + matFileName;
            }

            MessageInterface::ShowMessage("MATLAB file will be written to "
                  "%s\n", matFileName.c_str());

            matWriter->Initialize(matFileName, "w5");

            // Move later in the process
            matData.Clear();
         }
      }
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
   try
   {
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

         // Write .mat data
         if (matWriter != NULL)
            if (!WriteMatData())
               throw EstimatorException("Error writing .mat data file");
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
   }
   catch (...) //(EstimatorException ex)
   {
      currentState = FINISHED;
      throw; // ex;
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
      MessageInterface::ShowMessage("advanceToEstimationEpoch = %s\n", (advanceToEstimationEpoch?"true":"false"));
   #endif
   
   if (advanceToEstimationEpoch == false)
   {
      PropagationStateManager *psm = propagator->GetPropStateManager();

      ObjectArray satArray;
      esm.GetStateObjects(satArray, Gmat::SPACECRAFT);
      estimationState = esm.GetState();
      stateSize       = estimationState->GetSize();

      Estimator::CompleteInitialization();
      
      // If estimation epoch not set, use the epoch from the prop state
      if ((estEpochFormat == "FromParticipants") || (estimationEpoch <= 0.0))
      {
         ObjectArray participants;
         esm.GetStateObjects(participants, Gmat::SPACEOBJECT);
         for (UnsignedInt i = 0; i < participants.size(); ++i)
            estimationEpoch   = ((SpaceObject *)(participants[i]))->GetEpoch();
      }

      // Set the current epoch based on the first spacecraft in the ESM
      if(satArray.size() == 0)
         throw EstimatorException("Cannot initialized the estimator: there are "
               "no Spacecraft in the estimation state manager");
      currentEpoch         = ((Spacecraft*)satArray[0])->GetEpoch();
      
      
      // Set all solve-for and consider objects to tracking data adapters
      // Note that: it only sets for tracking data adapters. For measurement models, 
      // it does not has this option due to old GMAT Nav syntax will be removed, 
      // so we do not need to implement this option.
      ObjectArray objects;
      esm.GetStateObjects(objects);
      std::vector<TrackingDataAdapter*> adapters = measManager.GetAllTrackingDataAdapters();
      for (UnsignedInt i = 0; i < adapters.size(); ++i)
         adapters[i]->SetUsedForObjects(objects);

      
      // Now load up the observations
      measManager.PrepareForProcessing(false);
      
///// Check for more generic approach
      measManager.LoadRampTables();      

      if (!GmatMathUtil::IsEqual(currentEpoch, estimationEpoch))
      {
         advanceToEstimationEpoch = true;
         nextMeasurementEpoch = estimationEpoch;
         currentState = PROPAGATING;
         return;
      }
   }
   
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
   
   advanceToEstimationEpoch = false;

   // First measurement epoch is the epoch of the first measurement.  Duh.
   nextMeasurementEpoch = measManager.GetEpoch();
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
            "Init complete!\n   STM = %s\n   Covariance = %s\n",
            stm->ToString().c_str(), stateCovariance->GetCovariance()->ToString().c_str());
   #endif

   hAccum.clear();
   if (useApriori)
   {   // [Lambda] = [Px0]^-1
      try
      {
         information = stateCovariance->GetCovariance()->Inverse();         // stateCovariance is [Px0] matrix
      }
      catch (...)
      {
         MessageInterface::ShowMessage("Apriori covariance matrix:\n[");
         for (Integer row = 0; row < stateCovariance->GetDimension(); ++row)
         {
            for (Integer col = 0; col < stateCovariance->GetDimension(); ++col)
               MessageInterface::ShowMessage("%le   ", stateCovariance->GetCovariance()->GetElement(row, col));
            if (row < stateCovariance->GetDimension() - 1)
               MessageInterface::ShowMessage("\n");
         }
         MessageInterface::ShowMessage("]\n");

         throw EstimatorException("Error: Apriori covariance matrix is singular. GMAT cannot take inverse of that matrix.\n");
      }
   }
   else
   {  // [Lambda] = [0] 
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
         x0bar[i] = 0.0;                                              // it is delta_XTile(i) in equation 8-22 in GTDS MathSpec. Initialy its value is zero-vector 
   }

   if (useApriori)
   {
      for (Integer i = 0; i < information.GetNumRows(); ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            residuals[i] += information(i,j) * x0bar[j];             // It is the last term in open-close parenthesis in equation 8-57 in GTDS MathSpec
      }
   }

   esm.BufferObjects(&outerLoopBuffer);
   esm.MapObjectsToVector();
   
   estimationStatus = UNKNOWN;
   // Convert estimation state from GMAT internal coordinate system to participants' coordinate system
   GetEstimationStateForReport(aprioriSolveForState);

   isInitialized = true;
   numDivIterations = 0;                       // It need to reset its value when starting estimatimation calculation

   
   // Get list of signal paths and specify the length of participants' column
   pcolumnLen = 24;
   std::vector<StringArray> signalPaths = measManager.GetSignalPathList();
   for(UnsignedInt i = 0; i < signalPaths.size(); ++i)
   {
      Integer len = 0;
      for (UnsignedInt j = 0; j < signalPaths[i].size(); ++j)
      {
         GmatBase* obj = GetConfiguredObject(signalPaths[i].at(j));
         std::string id = "";
         if (obj->IsOfType(Gmat::SPACECRAFT))
            id = ((Spacecraft*)obj)->GetStringParameter("Id");
         else if (obj->IsOfType(Gmat::GROUND_STATION))
            id = ((GroundstationInterface*)obj)->GetStringParameter("Id");
         
         len = len + id.size() + 1;
      }
      if (pcolumnLen < len)
         pcolumnLen = len;
   }

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

   // Clear warning message's count
   warningCount = 0;


   // Verify no two different ground station having the same Id
   std::string errMsg = "";
   if (GetMeasurementManager()->ValidateDuplicationOfGroundStationID(errMsg) == false)
   {
      errMsg = "Error: " + errMsg + " in batch estimator '" + GetName() + "'.\n";
      throw EstimatorException(errMsg);
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
   //else if (GmatMathUtil::IsEqual(currentEpoch, nextMeasurementEpoch))       // value of accuray is set to 5.0e-12 due to the accuracy limit of double
   else if (fabs((currentEpoch - nextMeasurementEpoch)/currentEpoch) < GmatRealConstants::REAL_EPSILON)
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
   estimationStatus = TestForConvergence(convergenceReason);
   
   // Reset best RMS as needed                           // fix bug GMT-5711
   if (resetBestRMSFlag)                                 // fix bug GMT-5711
   {                                                     // fix bug GMT-5711
      if (estimationStatus == DIVERGING)                 // fix bug GMT-5711
         resetBestResidualRMS = newResidualRMS;          // fix bug GMT-5711
   }                                                     // fix bug GMT-5711

   #ifdef RUN_SINGLE_PASS
      converged = true;
   #endif

   ++iterationsTaken;
   if ((estimationStatus == ABSOLUTETOL_CONVERGED) ||
      (estimationStatus == RELATIVETOL_CONVERGED) ||
      (estimationStatus == ABS_AND_REL_TOL_CONVERGED) ||
      (estimationStatus == MAX_CONSECUTIVE_DIVERGED) ||
      (estimationStatus == MAX_ITERATIONS_DIVERGED))
   {
      if ((estimationStatus == ABSOLUTETOL_CONVERGED) ||
         (estimationStatus == RELATIVETOL_CONVERGED) ||
         (estimationStatus == ABS_AND_REL_TOL_CONVERGED))
         status = CONVERGED;
      else
         status = EXCEEDED_ITERATIONS;
      
      currentState = FINISHED;
   }
   else
   {
      if (showAllResiduals)
         PlotResiduals();

      // Reset to the new initial state, clear the processed data, etc
      esm.RestoreObjects(&outerLoopBuffer);                           // Restore solver-object initial state 
      esm.MapVectorToObjects();                                       // update objects state to current state
      esm.MapObjectsToSTM();                                          // update object STM to current STM
      currentEpoch = estimationEpoch;
      measManager.Reset();                                            // set current observation data to be the first one in observation data table
      nextMeasurementEpoch = measManager.GetEpoch();

      // Need to reset STM and covariances
      hAccum.clear();
      if (useApriori)
      {
         try
         {
            information = stateCovariance->GetCovariance()->Inverse();   // When starting an iteration, [Lambda] = [Px0]^-1
         }
         catch (...)
         {
            MessageInterface::ShowMessage("Apriori covariance matrix:\n[");
            for (Integer row = 0; row < stateCovariance->GetDimension(); ++row)
            {
               for (Integer col = 0; col < stateCovariance->GetDimension(); ++col)
                  MessageInterface::ShowMessage("%le   ", stateCovariance->GetCovariance()->GetElement(row, col));
               if (row < stateCovariance->GetDimension() - 1)
                  MessageInterface::ShowMessage("\n");
            }
            MessageInterface::ShowMessage("]\n");

            throw EstimatorException("Error: Apriori covariance matrix is singular. GMAT cannot take inverse of that matrix.\n");
         }
      }
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
         x0bar[j] -= dx[j];                                      // delta_XTile(i+1) = X[0] - X[i+1] = (X[0] - X[i]) - (X[i+1] - X[i]) = delta_X_Tile(i) - dx

      if (useApriori)
      {
         for (Integer i = 0; i < information.GetNumRows(); ++i)
         {
            for (UnsignedInt j = 0; j < stateSize; ++j)
            {
               residuals[i] += information(i,j) * x0bar[j];      // At the beginning of each iteration, [Lambda] = ([Px0]^-1).delta_XTile(i)  the last term in open-close square bracket in euqation 8-57 GTDS MathSpec
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
      // After writing to GmatLog.txt file, bestResidualRMS is set to resetBestResdualRMS    // fix bug GMT-5711
      if ((resetBestRMSFlag) && (estimationStatus == DIVERGING))                             // fix bug GMT-5711
         bestResidualRMS = resetBestResidualRMS;                                             // fix bug GMT-5711

      numRemovedRecords["U"] = 0;
      numRemovedRecords["R"] = 0;
      numRemovedRecords["B"] = 0;
      numRemovedRecords["OLSE"] = 0;
      numRemovedRecords["IRMS"] = 0;

      // reset value for statistics table
      statisticsTable.clear();
      statisticsTable1.clear();

      stationAndType.clear();
      stationsList.clear();
      measTypesList.clear();
      sumAllRecords.clear();
      sumAcceptRecords.clear();
      sumResidual.clear();
      sumResidualSquare.clear();
      sumWeightResidualSquare.clear();

      sumSERecords.clear();
      sumSEResidual.clear();
      sumSEResidualSquare.clear();
      sumSEWeightResidualSquare.clear();

      // Clear all media correct warning lists
      ionoWarningList.clear();
      tropoWarningList.clear();

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


   // Clean up memory
   for (UnsignedInt i = 0; i < hTilde.size(); ++i)
      hTilde[i].clear();
   hTilde.clear();
   
   for (UnsignedInt i = 0; i < hAccum.size(); ++i)
      hAccum[i].clear();
   hAccum.clear();

   Weight.clear();
   OData.clear();
   CData.clear();

   measurementResiduals.clear();
   measurementEpochs.clear();
   measurementResidualID.clear();

   statisticsTable.clear();
   statisticsTable1.clear();

   stationAndType.clear();
   stationsList.clear();
   measTypesList.clear();
   sumAllRecords.clear();
   sumAcceptRecords.clear();
   sumResidual.clear();
   sumResidualSquare.clear();
   sumWeightResidualSquare.clear();

   sumSERecords.clear();
   sumSEResidual.clear();
   sumSEResidualSquare.clear();
   sumSEWeightResidualSquare.clear();

   if (matWriter != NULL)
      matWriter->CloseFile();
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
   Real taiMjdEpoch, utcMjdEpoch;
   std::string utcEpoch;
   Rmatrix finalCovariance;

   StringArray::iterator current;

   std::stringstream progress;
   progress.str("");
   progress.precision(12);
   const std::vector<ListItem*> *map = esm.GetStateMap();

   bool handleLeapSecond;

   GmatState outputEstimationState;

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
                  progress << "   Estimation Epoch:\n";
                  progress << "   " << s << " A.1 modified Julian\n";
                  taiMjdEpoch = TimeConverterUtil::Convert(estimationEpoch, TimeConverterUtil::A1MJD, TimeConverterUtil::TAIMJD);
                  utcMjdEpoch = TimeConverterUtil::Convert(estimationEpoch, TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD);
                  handleLeapSecond = TimeConverterUtil::HandleLeapSecond();
                  sprintf(&s[0], "%22.12lf", taiMjdEpoch);
                  progress << "   " << s << " TAI modified Julian\n";
                  utcEpoch = TimeConverterUtil::ConvertMjdToGregorian(utcMjdEpoch, handleLeapSecond);
                  progress << "   " << utcEpoch << " UTCG\n";
               }

               GetEstimationStateForReport(outputEstimationState);
               
               for (UnsignedInt i = 0; i < map->size(); ++i)
               {
                  progress << "   " << GetElementFullName((*map)[i], false) << " = "
                           << outputEstimationState[i] << "\n";
               };

            }
            break;

         case CHECKINGRUN:
            progress << "\n   WeightedRMS residuals for this iteration : "
                     << newResidualRMS;
            progress << "\n   BestRMS residuals                        : "
                     << bestResidualRMS;
            if ((resetBestRMSFlag) && (estimationStatus == DIVERGING))                 // fix bug GMT-5711
            {                                                                          // fix bug GMT-5711
            progress << "\n   Reset value of BestRMS residuals         : "             // fix bug GMT-5711
                        << resetBestResidualRMS;                                       // fix bug GMT-5711
            }                                                                          // fix bug GMT-5711
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
            taiMjdEpoch = TimeConverterUtil::Convert(estimationEpoch, TimeConverterUtil::A1MJD, TimeConverterUtil::TAIMJD);
            utcMjdEpoch = TimeConverterUtil::Convert(estimationEpoch, TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD);
            handleLeapSecond = TimeConverterUtil::HandleLeapSecond();
            utcEpoch = TimeConverterUtil::ConvertMjdToGregorian(utcMjdEpoch, handleLeapSecond);
            progress << "   Estimation Epoch:\n";
            progress << "   " << s << " A.1 modified Julian\n";
            sprintf(&s[0], "%22.12lf", taiMjdEpoch);
            progress << "   " << s << " TAI modified Julian\n";
            progress << "   " << utcEpoch << " UTCG\n";


            GetEstimationStateForReport(outputEstimationState);

            for (UnsignedInt i = 0; i < map->size(); ++i)
            {
               progress << "   " << GetElementFullName((*map)[i], false) << " = "
                        << outputEstimationState[i] << "\n";
            }

            break;

         case FINISHED:
            progress << "\n   WeightedRMS residuals for this iteration : "
                     << newResidualRMS;
            progress << "\n   BestRMS residuals                        : "
                     << bestResidualRMS;
            progress << "\n   PredictedRMS residuals for next iteration: "
                     << predictedRMS << "\n";

            switch(estimationStatus)
            {
            case ABSOLUTETOL_CONVERGED:
               progress << "This iteration is converged due to absolute tolerance convergence criteria.\n";
               break;
            case RELATIVETOL_CONVERGED:
               progress << "This iteration is converged due to relative convergence criteria.\n";
               break;
            case ABS_AND_REL_TOL_CONVERGED:
               progress << "This iteration is converged due to boths: absolute and relative convergence criteria.\n";
               break;
            case MAX_CONSECUTIVE_DIVERGED:
               progress << "This iteration is diverged due to maximum consecutive diverged criteria.\n";
               break;
            case MAX_ITERATIONS_DIVERGED:
               progress << "This iteration is diverged due to exceeding the maximum iterations.\n";
               break;
            case CONVERGING:
               progress << "This iteration is converging.\n";
               break;
            case DIVERGING:
               progress << "This iteration is diverging.\n";
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
            
            progress   << "   " << convergenceReason << "\n"
                       << "Final Estimated State:\n\n";

            if (estEpochFormat != "FromParticipants")
               progress << "   Estimation Epoch (" << estEpochFormat
                        << "): " << estEpoch << "\n";
            else
            {
               char s[100];
               sprintf(&s[0],"%22.12lf", estimationEpoch);
               progress << "   Estimation Epoch:\n";
               progress << "   " << s << " A.1 modified Julian\n";
               taiMjdEpoch = TimeConverterUtil::Convert(estimationEpoch, TimeConverterUtil::A1MJD, TimeConverterUtil::TAIMJD);
               utcMjdEpoch = TimeConverterUtil::Convert(estimationEpoch, TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD);
               handleLeapSecond = TimeConverterUtil::HandleLeapSecond();
               sprintf(&s[0], "%22.12lf", taiMjdEpoch);
               progress << "   " << s << " TAI modified Julian\n";
               utcEpoch = TimeConverterUtil::ConvertMjdToGregorian(utcMjdEpoch, handleLeapSecond);
               progress << "   " << utcEpoch << " UTCG\n";
            }

            GetEstimationStateForReport(outputEstimationState);

            for (UnsignedInt i = 0; i < map->size(); ++i)
            {
               progress << "   " << GetElementFullName((*map)[i], false) << " = "
                     << outputEstimationState[i] << "\n";
            }

            if (textFileMode == "Verbose")
            {
               progress << "\n   WeightedRMS residuals for previous iteration: "
                        << oldResidualRMS;
               progress << "\n   WeightedRMS residuals                       : "
                        << newResidualRMS;
               progress << "\n   BestRMS residuals for this iteration        : "
                        << bestResidualRMS << "\n\n";
            }

            finalCovariance = information.Inverse();

            // Convert covariance matrix for Cr_Epsilon and Cd_Epsilon to covariance matrix for Cr and Cd
            for (UnsignedInt i = 0; i < map->size(); ++i)
            {
               if ((*map)[i]->elementName == "Cr_Epsilon")
               {
                  // Get Cr0
                  Real Cr0 = (*map)[i]->object->GetRealParameter("Cr") / (1 + (*map)[i]->object->GetRealParameter("Cr_Epsilon"));

                  // multiply row and column i with Cr0
                  for (UnsignedInt j = 0; j < finalCovariance.GetNumColumns(); ++j)
                     finalCovariance(i, j) *= Cr0;
                  for (UnsignedInt j = 0; j < finalCovariance.GetNumRows(); ++j)
                     finalCovariance(j, i) *= Cr0;
               }
               if ((*map)[i]->elementName == "Cd_Epsilon")
               {
                  // Get Cd0
                  Real Cd0 = (*map)[i]->object->GetRealParameter("Cd") / (1 + (*map)[i]->object->GetRealParameter("Cd_Epsilon"));

                  // multiply row and column i with Cd0
                  for (UnsignedInt j = 0; j < finalCovariance.GetNumColumns(); ++j)
                     finalCovariance(i, j) *= Cd0;
                  for (UnsignedInt j = 0; j < finalCovariance.GetNumRows(); ++j)
                     finalCovariance(j, i) *= Cd0;
               }
            }

            // Display final coveriance matrix
            progress << "\nFinal Covariance Matrix:\n\n";
            for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
            {
               for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
               {
                  char s[100];
                  sprintf(&s[0], "   %22.12le\0", finalCovariance(i, j));
                  std::string ss(s);
                  progress << "   " << ss.substr(ss.size()-24); //finalCovariance(i, j);
               }
               progress << "\n";
            }

            // Display final correlation matrix
            progress << "\nFinal Correlation Matrix:\n\n";
            for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
            {
               for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
               {
                  char s[100];
                  sprintf(&s[0], "   %22.12lf\0", finalCovariance(i, j)/ sqrt(finalCovariance(i, i)*finalCovariance(j, j)));
                  std::string ss(s);
                  progress << "   " << ss.substr(ss.size()-24);
               }
               progress << "\n";
            }
            //progress.fixed;

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
            throw EstimatorException(
               "Solver state not supported for the simulator");
      }
   }
   else
      return Estimator::GetProgressString();

   return progress.str();
}


//--------------------------------------------------------------------------------------
// std::string GetElementFullName(ListItem* infor, bool isInternalCS) const
//--------------------------------------------------------------------------------------
/**
* This function is used to get a full name of an element as specified by ListItem* infor
*
* @param infor              The ListItem element containing information about the elment
* @param isInternalCS       flag to indicate whether the element is presented in an 
*                           internal coordinate system or not. 
*
* return                    a string containing element full name
*/
//--------------------------------------------------------------------------------------
std::string BatchEstimator::GetElementFullName(ListItem* infor, bool isInternalCS) const
{
   std::stringstream ss;
   
   ss << infor->objectFullName << ".";
   if (infor->elementName == "CartesianState")
   {
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
   else if (infor->elementName == "Cr_Epsilon")
      ss << "Cr";
   else if (infor->elementName == "Cd_Epsilon")
      ss << "Cd";
   else if (infor->elementName == "Bias")
      ss << "Bias";
   else
      ss << infor->elementName << "." << infor->subelement;

   return ss.str();
}


//--------------------------------------------------------------------------------------
// std::string GetElementUnit(ListItem* infor) const
//--------------------------------------------------------------------------------------
/**
* Get element unit
*
* @param infor     The ListItem element containing information about the elment
*
* @return          unit of the element
*/
//--------------------------------------------------------------------------------------
std::string BatchEstimator::GetElementUnit(ListItem* infor) const
{
   std::string unit = "";

   if ((infor->elementName == "CartesianState") || (infor->elementName == "Position"))
   {
      switch (infor->subelement)
      {
      case 1:
      case 2:
      case 3:
         unit = "km";
         break;
      case 4:
      case 5:
      case 6:
         unit = "km/s";
         break;
      }
   }
   else if (infor->elementName == "Velocity")
   {
      switch (infor->subelement)
      {
      case 1:
      case 2:
      case 3:
         unit = "km/s";
         break;
      }
   }
   else if (infor->elementName == "Bias")
   {
      if (infor->object->IsOfType(Gmat::MEASUREMENT_MODEL))
      {
         // Get full name for Bias
         MeasurementModel* mm = (MeasurementModel*)(infor->object);

         // Get Bias unit. It is Km for Range_KM, RU for DSNRange, Km/s for Doppler_RangeRate, and for Doppler_HZ 
         if (mm->IsOfType("DSNTwoWayRange"))
            unit = "RU";
         else if (mm->IsOfType("DSNTwoWayDoppler"))
            unit = "Hz";
         else if(mm->IsOfType("USNTwoWayRange"))
            unit = "km";
         else if (mm->IsOfType("TDRSSTwoWayRange"))
            unit = "Hz";
         else
            unit = "****";
      }
      else
      {
         std::string measType = infor->object->GetStringParameter("Type");
         if (measType == "Range_RU")
            unit = "RU";
         else if (measType == "Range_KM")
            unit = "km";
         else if (measType == "Doppler_RangeRate")
            unit = "km/s";
         else if (measType == "Doppler_HZ")
            unit = "Hz";
         else if (measType == "TDRSDoppler_HZ")
            unit = "Hz";
      }
   }
   else if ((infor->elementName == "Cr_Epsilon") || (infor->elementName == "Cd_Epsilon"))
      unit = "";

   //@ todo: code to specify unit of other solve-for parameters is added here

   return unit;
}


//--------------------------------------------------------------------------------------
// Integer GetElementPrecision(std::string unit) const
//--------------------------------------------------------------------------------------
/**
* Get element precision associated with th given unit
*
* @param unit      a measurement unit
*
* @return          precision asscociated with the unit
*/
//--------------------------------------------------------------------------------------
Integer BatchEstimator::GetElementPrecision(std::string unit) const
{
   // parameter's precision is specified based on its unit 
   int precision = 10;

   if ((unit == "km") || (unit == "RU") || (unit == "Hz"))
      precision = 6;
   else if ((unit == "km/s") || (unit == "deg") || (unit == "km2/s2"))
      precision = 8;
   else if (unit == "min")
      precision = 6;
   else if (unit == "min/day")
      precision = 8;
   else if (unit == "")
      precision = 8;
   //@ todo: code to specify precision of other solve-for parameters is added here

   return precision;
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
      why << "Number of iterations reached its maximum value (" << maxIterations << ").\n";
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
            why << "Number of consecutive divergences reached its maximum value (" << maxConsDivergences << ").\n";
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
   
   GmatState outputEstimationState;

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
         WriteReportFileHeader();
         break;

      case ACCUMULATING:
         if ((measManager.GetCurrentRecordNumber() != 0) && (GmatMathUtil::Mod(measManager.GetCurrentRecordNumber(), 80) < 0.001))
            WritePageHeader();
         textFile << linesBuff;
         textFile.flush();
         break;

      case ESTIMATING:
         WriteReportFileSummary(theState);
         break;

      case CHECKINGRUN:
         WriteReportFileSummary(theState);
         textFile << textFile0.str() << textFile1.str() << textFile1_1.str() << textFile2.str() << textFile3.str() << textFile4.str();
         textFile0.str(""); textFile1.str(""); textFile1_1.str(""); textFile2.str(""); textFile3.str(""); textFile4.str("");
         WriteIterationHeader();
         break;

      case FINISHED:
         WriteReportFileSummary(theState);
         textFile << textFile0.str() << textFile1.str() << textFile1_1.str() << textFile2.str() << textFile3.str() << textFile4.str();
         textFile0.str(""); textFile1.str(""); textFile1_1.str(""); textFile2.str(""); textFile3.str(""); textFile4.str("");
         break;

      default:
         break;
   }
}



//----------------------------------------------------------------------------
// std::string GetFileCreateTime(std::string fileName)
//----------------------------------------------------------------------------
/**
* This function is used to get build date and time for a given file.
*
* @param  fileName       name of a file
*
* @return                a string containning build date and time
*/
//----------------------------------------------------------------------------
std::string BatchEstimator::GetFileCreateTime(std::string fileName)
{
   std::string time;
   struct stat sb;
   if (stat(fileName.c_str(), &sb) == -1)
   {
      MessageInterface::ShowMessage("Error:: Cannot get build date for file '%s'\n", fileName.c_str());
      return "";
   }

   return CTime(&sb.st_ctime);
}


//----------------------------------------------------------------------------
// std::string CTime(const time_t* time)
//----------------------------------------------------------------------------
/**
* This function is used to convert time from type time_t to string.
*
* @param time       time in time_t type
*
* @return           a string containning date and time
*/
//----------------------------------------------------------------------------
std::string BatchEstimator::CTime(const time_t* time)
{
   char* dt = ctime(time);
   std::string s;
   s.assign(dt);
   size_t pos = s.find_last_of(' ');
   std::string syear = s.substr(pos + 1, 4);
   s = s.substr(0, pos);

   pos = s.find_last_of(' ');
   std::string stime = s.substr(pos + 1);
   s = s.substr(0, pos);

   s = s + ", " + syear + " " + stime;

   return s;
}


//------------------------------------------------------------------------------
// std::string GetGMATBuildDate()
//------------------------------------------------------------------------------
/**
* Get GMAT build date
*/
//------------------------------------------------------------------------------
std::string BatchEstimator::GetGMATBuildDate()
{
   std::istringstream s(__DATE__);
   std::string smonth;
   Integer day, month, year;
   s >> smonth >> day >> year;
   switch(smonth.at(0))
   {
   case 'J':
      if (smonth == "Jun")
         month = 6;
      else if (smonth == "Jul")
         month = 7;
      else
         month = 1;
      break;
   case 'F':
      month = 2;
      break;
   case 'M':
      if (smonth == "May")
         month = 5;
      else
         month = 3;
      break;
   case 'A':
      if (smonth == "Apr")
         month = 4;
      else
         month = 8;
      break;
   case 'S':
      month = 9;
      break;
   case 'O':
      month = 10;
      break;
   case 'N':
      month = 11;
      break;
   case 'D':
      month = 12;
      break;
   }

   std::string sday = GetDayOfWeek(day, month, year);

   std::stringstream ss;
   ss << sday << " " << smonth << " " << day << ", " << year << " " << __TIME__;
   return ss.str();
}


//------------------------------------------------------------------------------
// std::string GetDayOfWeek(Integer day, Integer month, Integer year)
//------------------------------------------------------------------------------
/**
* Specify day of week for a given date month/day/year
*/
//------------------------------------------------------------------------------
std::string BatchEstimator::GetDayOfWeek(Integer day, Integer month, Integer year)
{

   Integer daysOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

   // Specify number of days of February
   if ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)))
      daysOfMonth[1] = 29;

   // Calculate number of days from day, month, year to 01/01/0001
   Integer y = year - 1;
   Integer m = month - 1;
   Integer d = day - 1;
   Integer days = 365*y + (int)(y / 4) - (int)(y / 100) + (int)(y / 400);   // total number of days from 01/01/y  to 01/01/0001
   for (Integer i = 0; i < m; ++i)
      days += daysOfMonth[i];                          // total number of days from m/01/y   to 01/01/0001
   days += d;                                          // total number of days from m/d/y    to 01/01/0001
   
   // Calculate weekday
   Integer weekdayOffset = 1;
   Integer weekday = days % 7 + weekdayOffset;

   if (weekday > 6)
      weekday -= 7;

   std::string sweekday;
   switch (weekday)
   {
   case 0:
      sweekday = "Sunday";
      break;
   case 1:
      sweekday = "Monday";
      break;
   case 2:
      sweekday = "Tuesday";
      break;
   case 3:
      sweekday = "Wednesday";
      break;
   case 4:
      sweekday = "Thusday";
      break;
   case 5:
      sweekday = "Friday";
      break;
   case 6:
      sweekday = "Saturday";
      break;
   }
   
   return sweekday;
}


//----------------------------------------------------------------------------
// std::string GetOperatingSystemName()
//----------------------------------------------------------------------------
/**
* This function is used to get computer operating system name.
*
* @return        name of OS
*/
//----------------------------------------------------------------------------
std::string BatchEstimator::GetOperatingSystemName()
{
   std::string osName = "";
#ifdef __linux__
   struct utsname uts;
   if (uname(&uts) == -1)
      throw EstimatorException("Error: cannot get OS information\n");
   
   osName.assign(uts.sysname);
#else
   #ifdef __APPLE__
   struct utsname uts;
   if (uname(&uts) == -1)
      throw EstimatorException("Error: cannot get OS information\n");

   osName.assign(uts.sysname);

   #endif
#endif

#ifdef _MSC_VER
   osName = "Windows";
#endif

   return osName;
}


//----------------------------------------------------------------------------
// std::string GetOperatingSystemVersion()
//----------------------------------------------------------------------------
/**
* This function is used to get computer operating system version.
*
* @return        version of OS
*/
//----------------------------------------------------------------------------
std::string BatchEstimator::GetOperatingSystemVersion()
{
   std::string osVersion = "";
#ifdef __linux__
   struct utsname uts;
   if (uname(&uts) == -1)
      throw EstimatorException("Error: cannot get OS information\n");

   osVersion.assign(uts.version);
#else
#ifdef __APPLE__
   struct utsname uts;
   if (uname(&uts) == -1)
      throw EstimatorException("Error: cannot get OS information\n");

   osVersion.assign(uts.version);

#endif
#endif

#ifdef _MSC_VER
   DWORD version = 0;
   DWORD maVersion = 0;
   DWORD miVersion = 0;

   version = GetVersion();
   maVersion = (DWORD)(LOBYTE(LOWORD(version)));
   miVersion = (DWORD)(HIBYTE(LOWORD(version)));

   char s[100];
   sprintf(&s[0], "%d.%d", maVersion, miVersion);
   osVersion.assign(s);
#endif

   return osVersion;
}


//----------------------------------------------------------------------------
// std::string GetHostName()
//----------------------------------------------------------------------------
/**
* This function is used to get computer name.
*
* @return   name of computer on which GMAT runs. 
*/
//----------------------------------------------------------------------------
std::string BatchEstimator::GetHostName()
{
   std::string hostName = "";
   char s[256];
#ifdef __linux__
   gethostname(s, sizeof(s));
#else
#ifdef __APPLE__
   gethostname(s, sizeof(s));
#endif
#endif

#ifdef _MSC_VER
   gethostname(s, sizeof(s));
#endif

   hostName.assign(s);
   return hostName;
}


//----------------------------------------------------------------------------
// std::string GetUserID()
//----------------------------------------------------------------------------
/**
* This function is used to get computer user ID.
*
* @return   computer user ID 
*/
//----------------------------------------------------------------------------
std::string BatchEstimator::GetUserID()
{
   std::string userName = "";
   
#ifdef __linux__
   char *name;
   name = getlogin();
   if (name != NULL)
      userName.assign(name);
#else
#ifdef __APPLE__
   char *name;
   name = getlogin();
   if (name != NULL)
      userName.assign(name);
#endif
#endif

#ifdef _MSC_VER
   char s[256];
   char name[128];
   DWORD size = sizeof(s);
   GetUserName((LPTSTR)s,&size);
   for (UnsignedInt i = 0; i < size; ++i)
      name[i] = s[2 * i];
   userName.assign(name);
#endif

   return userName;
}


//----------------------------------------------------------------------------
// void WriteReportFileHeader()
//----------------------------------------------------------------------------
/**
* This function writes estimation report header. It contains 6 parts:
*      . Part 1: contains information about GMAT build, OS, user infor
*      . Part 2: contains batch least squares initial conditions
*      . Part 2b: contains apriori covariance matrix
*      . Part 3: contains information about orbit generator
*      . Part 4: contains information about measurements
*      . Part 5: contains information about astrodynamic constants
*      . Part 6: contains information about estimation options
*/
//----------------------------------------------------------------------------
void BatchEstimator::WriteReportFileHeader()
{
   WriteReportFileHeaderPart1();
   WriteReportFileHeaderPart2();

   if (useApriori)
      WriteReportFileHeaderPart2b();
   
   WriteReportFileHeaderPart3();
   WriteReportFileHeaderPart4();
   WriteReportFileHeaderPart5();
   WriteReportFileHeaderPart6();
   WriteIterationHeader();
}


//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart1()
//------------------------------------------------------------------------------
/**
* This function is used to write GMAT release, build, OS information, and user
* information to report file.
*/
//------------------------------------------------------------------------------
void BatchEstimator::WriteReportFileHeaderPart1()
{
   /// 1. Write header 1:
   time_t now = time(NULL);
   std::string runDate = CTime(&now);
   //std::string buildTime = GetFileCreateTime("GMAT.exe");
   std::string buildTime = GetGMATBuildDate();
   std::string version = GmatGlobal::Instance()->GetGmatVersion();
   
   textFile
      << "                                              *****  G E N E R A L  M I S S I O N  A N A L Y S I S  T O O L  *****\n"
      << "\n"
      << "                                                                          Release " << version << "\n"
      << GmatStringUtil::GetAlignmentString("", 59) + "Build Date : " << buildTime << "\n"
      << "\n"
      << GmatStringUtil::GetAlignmentString("", 36) + "Hostname : " << GmatStringUtil::GetAlignmentString(GetHostName(), 36, GmatStringUtil::LEFT) << " OS / Arch : " << GetOperatingSystemName() << " " << GetOperatingSystemVersion() << "\n"
      << GmatStringUtil::GetAlignmentString("", 36) + "User ID  : "
      << GmatStringUtil::GetAlignmentString(GetUserID(), 36, GmatStringUtil::LEFT)
      << " Run Date  : " << runDate << "\n"
      << "\n"
      << "\n";

   textFile.flush();
}


//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart2()
//------------------------------------------------------------------------------
/**
* This function is used to write batch least square initial information to the 
* report file.
*/
//------------------------------------------------------------------------------
void BatchEstimator::WriteReportFileHeaderPart2()
{
   // 1. Write state at beginning iteration:
   textFile
      << "************************************************************ BATCH LEAST SQUARES INITIAL CONDITIONS ************************************************************\n"
      << "\n"
      << " Satellite State at Beginning of Iteration :\n"
      << "\n";

   StringArray participantNames = GetMeasurementManager()->GetParticipantList();
   StringArray paramNames, paramValues, rowContent;

   // 2. Fill in parameter name:
   paramNames.push_back("Satellite Name");
   paramNames.push_back("ID");
   paramNames.push_back("");
   paramNames.push_back("Epoch (UTC)");
   paramNames.push_back("Coordinate System");
   paramNames.push_back("X  (km)");
   paramNames.push_back("Y  (km)");
   paramNames.push_back("Z  (km)");
   paramNames.push_back("VX (km/s)");
   paramNames.push_back("VY (km/s)");
   paramNames.push_back("VZ (km/s)");
   paramNames.push_back("Cr");
   paramNames.push_back("CrSigma");
   paramNames.push_back("Cd");
   paramNames.push_back("CdSigma");
   paramNames.push_back("DryMass  (kg)");
   paramNames.push_back("DragArea (m^2)");
   paramNames.push_back("SRPArea  (m^2)");
   Integer nameLen = 0;
   for (UnsignedInt i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());
   

   // 3. Write a table containing spacecraft initial condition:
   Integer colCount = 0;
   std::string scName, csName, s;
   Real val;
   bool found;

   for (UnsignedInt i = 0; i < participantNames.size(); ++i)
   {
      GmatBase* obj = GetConfiguredObject(participantNames[i]);

      if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         // 3.1. Get a spacecraft for processing:
         Spacecraft *sc = (Spacecraft *)obj;
         scName = participantNames[i];
         
         // 3.2. Fill in parameter's value and unit:
         paramValues.push_back(sc->GetName());
         paramValues.push_back(sc->GetStringParameter("Id"));
         paramValues.push_back("");
         paramValues.push_back(sc->GetEpochString());
         paramValues.push_back(sc->GetStringParameter("CoordinateSystem"));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("CartesianX"), false, false, true, 8, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("CartesianY"), false, false, true, 8, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("CartesianZ"), false, false, true, 8, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("CartesianVX"), false, false, true, 12, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("CartesianVY"), false, false, true, 12, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("CartesianVZ"), false, false, true, 12, 22));
         
         s = GmatStringUtil::RealToString(sc->GetRealParameter("Cr"), false, false, false, 8, 22);
         if (s.find('.') == s.npos)
            s = s + ".00";
         paramValues.push_back(s);
       
         StringArray solveforList = obj->GetStringArrayParameter("SolveFors");
         found = false;
         for (Integer j = 0; j < solveforList.size(); ++j)
         {
            if (solveforList[j] == "Cr")
            {
               found = true;
               break;
            }
         }
         if (found)
         {
            val = sc->GetRealParameter("CrSigma");
            if ((1.0e-5 < val) && (val < 1.0e10))
               s = GmatStringUtil::RealToString(val, false, false, true, 6, 22);
            else
               s = GmatStringUtil::RealToString(val, false, true, true, 10, 22);
         }
         else
            s = "Not estimated";
         paramValues.push_back(s);


         s = GmatStringUtil::RealToString(sc->GetRealParameter("Cd"), false, false, false, 8, 22);
         if (s.find('.') == s.npos)
            s = s + ".00";
         paramValues.push_back(s);
         
         found = false;
         for (Integer j = 0; j < solveforList.size(); ++j)
         {
            if (solveforList[j] == "Cd")
            {
               found = true;
               break;
            }
         }
         if (found)
         {
            val = sc->GetRealParameter("CdSigma");
            if ((1.0e-5 < val) && (val < 1.0e10))
               s = GmatStringUtil::RealToString(val, false, false, true, 6, 22);
            else
               s = GmatStringUtil::RealToString(val, false, true, true, 10, 22);
         }
         else
            s = "Not estimated";
         paramValues.push_back(s);

         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("DryMass"), false, false, false, 8, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("DragArea"), false, false, false, 8, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("SRPArea"), false, false, false, 8, 22));

         // 3.3. Increasing column count by 1
         ++colCount;

         // 3.4. Write information of the spacecraft on the column
         for (UnsignedInt j = 0; j < paramNames.size(); ++j)
         {
            if (j == rowContent.size())
               rowContent.push_back("");

            if (colCount == 1)
               rowContent[j] += (" " + GmatStringUtil::GetAlignmentString(paramNames[j], nameLen, GmatStringUtil::LEFT) + " ");

            rowContent[j] += (GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(paramValues[j]), 25, GmatStringUtil::RIGHT) + " ");
         }

         // 3.5. Beak up columns in a table 
         //if (colCount == SPACECRAFT_TABLE_COLUMN_BREAK_UP)
         if ((nameLen+2 + colCount*26) > (160-26))
         {
            for (UnsignedInt j = 0; j < rowContent.size(); ++j)
               textFile << rowContent[j] << "\n";
            textFile << "\n";

            rowContent.clear();
            colCount = 0;
         }

         // 3.6. Clear paramValues and paramUnits
         paramValues.clear();
      }
   }

   for (UnsignedInt j = 0; j < rowContent.size(); ++j)
      textFile << rowContent[j] << "\n";
   textFile << "\n";
   textFile << "\n";

   textFile.flush();
}



//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart2b()
//------------------------------------------------------------------------------
/**
* This function is used to write apriori covariance matrix to report file.
*/
//------------------------------------------------------------------------------
void BatchEstimator::WriteReportFileHeaderPart2b()
{
   // 1.1 Write subheader
   textFile
      << "*****************************************************************  APRIORI COVARIANCE MATRIX  ******************************************************************\n"
      << "\n";

   // 1.2. Prepare for writing
   const std::vector<ListItem*> *map = esm.GetStateMap();

   Rmatrix aprioriCov = *(stateCovariance->GetCovariance());

   // Convert covariance from CrEpsilon to Cr, from CdEpsilon to Cd
   for (Integer i = 0; i < map->size(); ++i)
   {
      if ((*map)[i]->object->IsOfType(Gmat::SPACECRAFT))
      {
         GmatBase* obj = (*map)[i]->object;
         if ((*map)[i]->elementName == "Cr_Epsilon")
         {
            // Convert covariance from CrEpsilon to Cr
            Real ratio = obj->GetRealParameter("Cr");
            for (Integer row = 0; row < aprioriCov.GetNumRows(); ++row)
               aprioriCov(row, i) = aprioriCov(row, i) *ratio;
            for (Integer col = 0; col < aprioriCov.GetNumColumns(); ++col)
               aprioriCov(i, col) = aprioriCov(i, col) *ratio;
         }

         if ((*map)[i]->elementName == "Cd_Epsilon")
         {
            // Convert covariance from CdEpsilon to Cd
            Real ratio = obj->GetRealParameter("Cd");
            for (Integer row = 0; row < aprioriCov.GetNumRows(); ++row)
               aprioriCov(row, i) = aprioriCov(row, i) *ratio;
            for (Integer col = 0; col < aprioriCov.GetNumColumns(); ++col)
               aprioriCov(i, col) = aprioriCov(i, col) *ratio;
         }
      }
   }

   // 2. Write standard deviation
   // 2.1. Specify maximum len of elements' names (Cartisian element names)
   Integer max_len = 27;         // 27 is the maximum lenght of ancillary element names
   for (int i = 0; i < map->size(); ++i)
   {
      std::stringstream ss;
      if (((*map)[i]->object->IsOfType(Gmat::MEASUREMENT_MODEL)) &&
         ((*map)[i]->elementName == "Bias"))
      {
         MeasurementModel* mm = (MeasurementModel*)((*map)[i]->object);
         StringArray sa = mm->GetStringArrayParameter("Participants");
         ss << mm->GetStringParameter("Type") << " ";
         for (UnsignedInt j = 0; j < sa.size(); ++j)
            ss << sa[j] << (((j + 1) != sa.size()) ? "," : " Bias.");
         ss << (*map)[i]->subelement;
      }
      else
         ss << GetElementFullName((*map)[i], false);
      max_len = (Integer)GmatMathUtil::Max(max_len, ss.str().size());
   }
   textFile << " " << GmatStringUtil::GetAlignmentString("State Component", max_len+3) << " " << GmatStringUtil::GetAlignmentString("Units", 8) << GmatStringUtil::GetAlignmentString("Standard Dev.", 19, GmatStringUtil::RIGHT) << "\n";
   textFile << "\n";

   // 2.2. Write a table containing solve-for name, unit, and standard deviation
   for (Integer i = 0; i < map->size(); ++i)
   {
      std::stringstream ss;
      if (((*map)[i]->object->IsOfType(Gmat::MEASUREMENT_MODEL)) &&
         ((*map)[i]->elementName == "Bias"))
      {
         // Get full name for Bias
         MeasurementModel* mm = (MeasurementModel*)((*map)[i]->object);
         StringArray sa = mm->GetStringArrayParameter("Participants");
         ss << mm->GetStringParameter("Type") << " ";
         for (UnsignedInt j = 0; j < sa.size(); ++j)
            ss << sa[j] << (((j + 1) != sa.size()) ? "," : " Bias.");
         ss << (*map)[i]->subelement;

         // Get Bias unit. It is km for Range_KM, RU for DSNRange, km/s for Doppler_RangeRate, and Hz for Doppler_HZ 
      }
      else
      {
         // Get full name for Bias
         ss << GetElementFullName((*map)[i], false);
      }

      std::string unit = GetElementUnit((*map)[i]);
      int precision = GetElementPrecision(unit);

      textFile << GmatStringUtil::ToString(i + 1, 3);
      textFile << " ";
      textFile << GmatStringUtil::GetAlignmentString(ss.str(), max_len + 1, GmatStringUtil::LEFT);
      textFile << GmatStringUtil::GetAlignmentString(unit, 8, GmatStringUtil::LEFT);

      textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(GmatMathUtil::Sqrt(aprioriCov.GetElement(i,i)), false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);     // standard deviation
      textFile << "\n";
   }
   textFile << "\n";


   // 3. write apriori covariance matrix
   textFile
      << "                                                    Apriori Covariance Matrix in Cartesian Coordinate System\n"
      << "\n";

   // 3.1 Specify who many digit of index
   Integer indexLen = 1;
   for (; GmatMathUtil::Pow(10, indexLen) < map->size(); ++indexLen);

   // 3.2. Write apriori covariance to report file 
   for (Integer startIndex = 0; startIndex < aprioriCov.GetNumColumns(); startIndex += MAX_COLUMNS)
   {

      textFile << "               ";
      for (Integer i = startIndex; i < GmatMathUtil::Min(startIndex + MAX_COLUMNS, aprioriCov.GetNumColumns()); ++i)
      {
         textFile << GmatStringUtil::ToString(i + 1, 3);
         if (i < aprioriCov.GetNumColumns() - 1)
            textFile << "                  ";
      }
      textFile << "\n";

      // write all rows from columns startIndex to startIndex+MAX_COLUMNS-1
      for (Integer i = 0; i < aprioriCov.GetNumRows(); ++i)
      {
         textFile << "  " << GmatStringUtil::ToString(i + 1, indexLen) << "  ";
         for (Integer j = startIndex; j < GmatMathUtil::Min(startIndex + MAX_COLUMNS, aprioriCov.GetNumColumns()); ++j)
         {
            textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(aprioriCov.GetElement(i, j), false, true, true, 12, 20), 21, GmatStringUtil::RIGHT);
         }
         textFile << "\n";
      }
      textFile << "\n";
   }

}

//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart3()
//------------------------------------------------------------------------------
/**
* This function is used to write force modeling options to the report file.
*/
//------------------------------------------------------------------------------
void BatchEstimator::WriteReportFileHeaderPart3()
{
   // 1. Write subheader
   textFile << "**************************************************************  SATELLITE FORCE MODELING OPTIONS  **************************************************************\n";
   textFile << "\n";

   StringArray participantNames = GetMeasurementManager()->GetParticipantList();
   PropSetup *prop = GetPropagator();
   StringArray paramNames, paramValues, rowContent;

   // 2. Fill in parameter's name
   paramNames.push_back("Satellite Name");
   paramNames.push_back("");
   paramNames.push_back("Orbit Generator");
   paramNames.push_back("Central Body of Integration");
   paramNames.push_back("System of Integration");
   paramNames.push_back("Integrator");
   paramNames.push_back("  Error Control");
   paramNames.push_back("  Initial Step Size (sec)");
   paramNames.push_back("  Accuracy (Km)");
   paramNames.push_back("  Minimum Step Size (sec)");
   paramNames.push_back("  Maximum Step Size (sec)");
   paramNames.push_back("  Maximum Attempts");
   paramNames.push_back("  Stop if Accuracy is Violated");
   paramNames.push_back("Central Body Gravity Model");
   paramNames.push_back("  Degree and Order");
   paramNames.push_back("Non - Central Bodies");
   paramNames.push_back("Solar Radiation Pressure");
   paramNames.push_back("  Solar Radiation Model");
   paramNames.push_back("  Solar Irradiance (W/m^2)");
   paramNames.push_back("  Astronomical Unit (km)");
   paramNames.push_back("Drag");
   paramNames.push_back("  Atmospheric Density Model");
   paramNames.push_back("Central Body Solid Tides");
   paramNames.push_back("Central Body Albedo");
   paramNames.push_back("Central Body Thermal Radiation");
   paramNames.push_back("Spacecraft Thermal Radiation Pressure");
   paramNames.push_back("Relativistic Accelerations");

   // Set flag to skip some section in force model table
   bool skipGravityModel = true;
   bool skipRadPressure = true;
   bool skipDrag = true;
   Integer gmIndex = 0;                // index such as paramNames[index] == "Central Body Gravity Model"
   Integer rpIndex = 0;                // index such as paramNames[index] == "Solar Radiation Pressure"
   Integer dragIndex = 0;              // index such as paramNames[index] == "Drag"

   Integer nameLen = 0;
   for (UnsignedInt i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());


   // 3. Write table containing force model information for each spacecraft
   Integer colCount = 0;
   std::string scName;
   std::string val;
   std::stringstream ss;

   Integer max_size = 1;
   for (UnsignedInt i = 0; i < participantNames.size(); ++i)
   {
      GmatBase* obj = GetConfiguredObject(participantNames[i]);
      if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         // 3.1. Get a spacecraft for processing:
         Spacecraft *sc = (Spacecraft *)obj;
         scName = participantNames[i];

         // 3.2. Fill in parameter's value and unit
         paramValues.push_back(sc->GetName());                                                            // Satellite Name
         paramValues.push_back("");

         // Get PropSetup
         PropSetup  *ps   = GetPropagator();
         ODEModel   *ode  = ps->GetODEModel();
         Propagator *prop = ps->GetPropagator();
         StringArray bodyNames = ode->GetStringArrayParameter("PointMasses");

         // Get all needed info from force model

         paramValues.push_back("");                                                                       // Orbit Generator
         paramValues.push_back(ode->GetStringParameter("CentralBody"));                                   // Central Body of Integration
         paramValues.push_back("J2000Eq");                                                                // System of Integration    // for current GMAT version, only J2000Eq is used for force model
         paramValues.push_back(prop->GetTypeName());                                                      // Integrator
         paramValues.push_back(ode->GetStringParameter("ErrorControl"));                                  //   Error Control
         ss.str(""); ss << GmatStringUtil::RealToString(ps->GetRealParameter("InitialStepSize"),false, false, false, 8); paramValues.push_back(ss.str());      //   Initial Step Size
         ss.str(""); ss << GmatStringUtil::RealToString(ps->GetRealParameter("Accuracy"), false, true, false, 8); paramValues.push_back(ss.str());             //   Accuracy
         ss.str(""); ss << GmatStringUtil::RealToString(ps->GetRealParameter("MinStep"), false, false, false, 8); paramValues.push_back(ss.str());              //   Minimum Step Size
         ss.str(""); ss << GmatStringUtil::RealToString(ps->GetRealParameter("MaxStep"), false, false, false, 8); paramValues.push_back(ss.str());              //   Maximum Step Size
         ss.str(""); ss << GmatStringUtil::RealToString(ps->GetIntegerParameter("MaxStepAttempts"), false, false, false, 8); paramValues.push_back(ss.str());   //   Maximum Attempts
         
         val = (ps->GetBooleanParameter("StopIfAccuracyIsViolated") ? "True" : "False");
         paramValues.push_back(val);                                                                      //   Stop if Accuracy is Violated

         PhysicalModel *force   = NULL;
         GravityField  *gvForce = NULL;
         for (Integer j = 0; j < ode->GetNumForces(); ++j)
         {
            PhysicalModel *force = ode->GetForce(j);
            if (force->GetTypeName() == "GravityField")
            {
               gvForce = (GravityField*)force;
            }
         }

         
         if (gvForce != NULL)
         {
            Integer deg = gvForce->GetIntegerParameter("Degree");
            Integer ord = gvForce->GetIntegerParameter("Order");
            std::string potentialFile = gvForce->GetStringParameter("Model");
            
            std::string::size_type pos = potentialFile.find_last_of('\\');
            if (pos == std::string::npos)
               pos = potentialFile.find_last_of('/');
            if (pos != std::string::npos)
               potentialFile = potentialFile.substr(pos+1);

            pos = potentialFile.find_first_of('.');
            potentialFile = potentialFile.substr(0, pos);

            ss.str(""); ss << deg << "x" << ord;
            paramValues.push_back(potentialFile);                                                        // Central Body Gravity Model
            paramValues.push_back(ss.str());                                                             //   Degree and Order

            skipGravityModel = false;
         }
         else
         {
            paramValues.push_back("None");                                                               // Central Body Gravity Model
            paramValues.push_back("N/A");                                                                //   Degree and Order
         }
         gmIndex = paramValues.size() - 2;

         
         // fill blanks
         if (bodyNames.size() <= max_size)
         {
            // Fill in Values and Units
            for (UnsignedInt index = 0; index < bodyNames.size(); ++index)
            {
               paramValues.push_back(bodyNames[index]);
            }
            
            // Fill blanks for the remain
            for (UnsignedInt index = bodyNames.size(); index < max_size; ++index)
            {
               paramValues.push_back("");
            }
         }
         else
         {  // For max_size < bodyNames.size()
            // Specify the start index
            Integer k = 0;
            for (; k < paramNames.size(); ++k)
            {
               if (paramNames[k] == "Non - Central Bodies")
                  break;
            }
            // Specify the end index
            Integer k1 = k+1;
            for (; k1 < paramNames.size(); ++k1)
            {
               if (paramNames[k1] != "")
                  break;
            }
            
            // Insert blank lines to paramNames as needed
            StringArray::iterator pos = paramNames.begin() + k1;
            Integer mm = bodyNames.size() - (k1 - k);
            if (mm > 0)
            {
               paramNames.insert(pos, mm, "");

               // Insert blank line to rowContent as needed
               if (colCount != 0)
               {
                  // Add blanks lines to rowContent as needed
                  StringArray::iterator pos1 = rowContent.begin() + k1;
                  rowContent.insert(pos1, mm, GmatStringUtil::GetAlignmentString("", (pos1-1)->size()));
               }
            }


            // Set value to paramValues and paramUnits
            if (bodyNames.size() == 0)
            {
               paramValues.push_back("");
               for (Integer j = 1; j < max_size; ++j)
               {
                  paramValues.push_back("");
               }
            }
            else
            {
               for (Integer j = 0; j < bodyNames.size(); ++j)
               {
                  paramValues.push_back(bodyNames[j]);
               }
               for (Integer j = bodyNames.size(); j < max_size; ++j)
               {
                  paramValues.push_back("");
               }
            }

            // Reset max_size
            max_size = bodyNames.size();
         }
         
         
         std::string srp = ode->GetOnOffParameter("SRP");
         if (srp == "On")
         {
            paramValues.push_back("Yes");                                                                 // Solar Radiation Pressure

            PhysicalModel *force;
            UnsignedInt numForces = ode->GetNumForces();
            for (UnsignedInt index = 0; index < numForces; ++index)
            {
               force = ode->GetForce(index);
               if (force != NULL)
               {
                  if (force->IsOfType("SolarRadiationPressure"))
                     break;
               }
            }

            SolarRadiationPressure *srp = (SolarRadiationPressure*)force;
            paramValues.push_back(srp->GetStringParameter(srp->GetParameterID("SRPModel")));              // Solar Radiation Model
            ss.str(""); ss << GmatStringUtil::RealToString(srp->GetRealParameter(srp->GetParameterID("Flux")), false, false, false, 8);
            paramValues.push_back(ss.str());                                                              // Solar Irradiance
            ss.str(""); ss << GmatStringUtil::RealToString(srp->GetRealParameter(srp->GetParameterID("Nominal_Sun"))*GmatMathConstants::M_TO_KM, false, true, false, 8); 
            paramValues.push_back(ss.str());                                                              // Astronomical Unit

            skipRadPressure = false;
         }
         else
         {
            paramValues.push_back("No");                                                                   // Solar Radiation Pressure
            paramValues.push_back("");                                                                     // Solar Radiation Model
            paramValues.push_back("");                                                                     // Solar Irradiance
            paramValues.push_back("");                                                                     // Astronomical Unit
         }
         rpIndex = paramValues.size() - 4;
         

         std::string drag = ode->GetStringParameter("Drag");
         if (drag == "None")
         {
            paramValues.push_back("No");                                                                   // Drag
            paramValues.push_back("None");                                                                 // Atmospheric Density Model
         }
         else
         {
            paramValues.push_back("Yes");                                                                  // Drag
            paramValues.push_back(drag);                                                                   // Atmospheric Density Model

            skipDrag = false;
         }
         dragIndex = paramValues.size() - 2;
         

         if (gvForce != NULL)
            paramValues.push_back(gvForce->GetStringParameter("EarthTideModel"));
         else
            paramValues.push_back("");                                                                     // Central Body Solid Tides

         //@todo: It needs to modify this code when Central Body Albedo, Central Body Thermal Radiation, 
         //       and Spacecraft Thermal Radiation Pressure options are added to GMAT
         paramValues.push_back("No");                                                                     // Central Body Albedo
         paramValues.push_back("No");                                                                     // Central Body Thermal Radiation
         paramValues.push_back("No");                                                                     // Spacecraft Thermal Radiation Pressure

         val = ((ode->GetOnOffParameter("RelativisticCorrection") == "On") ? "Yes" : "No");
         paramValues.push_back(val);                                                                       // Relativistic Accelerations

         // 3.3. Increasing column count by 1
         ++colCount;

         // 3.4. Write information of the spacecraft on the column
         for (UnsignedInt j = 0; j < paramNames.size(); ++j)
         {
            if (j == rowContent.size())
               rowContent.push_back("");

            if (colCount == 1)
               rowContent[j] += (" " + GmatStringUtil::GetAlignmentString(paramNames[j], nameLen) + " ");

            rowContent[j] += (GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(paramValues[j]), 25) + " ");
         }
         
         // 3.5. Beak up columns in a table 
         if ((nameLen+2 + colCount*26) > (160-26))
         {
            for (UnsignedInt j = 0; j < rowContent.size(); ++j)
            {
               // skip 1 line showing the details of central body gravity model
               if (skipGravityModel && (j == (gmIndex + 1)))
                  continue;

               // skip 3 lines showing the details of solar radiation pressure 
               if (skipRadPressure && (rpIndex + 1 <= j) && (j <= rpIndex + 3))
                  continue;

               // skip 1 line showing the details of drag model
               if (skipDrag && (j == (dragIndex + 1)))
                  continue;

               textFile << rowContent[j] << "\n";
            }
            textFile << "\n";
            textFile << "\n";

            rowContent.clear();
            paramNames.clear();
            colCount = 0;
            max_size = 1;

            // Fill in parameter's name
            paramNames.push_back("Satellite Name");
            paramNames.push_back("");
            paramNames.push_back("Orbit Generator");
            paramNames.push_back("Central Body of Integration");
            paramNames.push_back("System of Integration");
            paramNames.push_back("Integrator");
            paramNames.push_back("  Error Control");
            paramNames.push_back("  Initial Step Size (sec)");
            paramNames.push_back("  Accuracy (Km)");
            paramNames.push_back("  Minimum Step Size (sec)");
            paramNames.push_back("  Maximum Step Size (sec)");
            paramNames.push_back("  Maximum Attempts");
            paramNames.push_back("  Stop if Accuracy is Violated");
            paramNames.push_back("Central Body Gravity Model");
            paramNames.push_back("  Degree and Order");
            paramNames.push_back("Non - Central Bodies");
            paramNames.push_back("Solar Radiation Pressure");
            paramNames.push_back("  Solar Radiation Model");
            paramNames.push_back("  Solar Irradiance (W/m^2)");
            paramNames.push_back("  Astronomical Unit (km)");
            paramNames.push_back("Drag");
            paramNames.push_back("  Atmospheric Density Model");
            paramNames.push_back("Central Body Solid Tides");
            paramNames.push_back("Central Body Albedo");
            paramNames.push_back("Central Body Thermal Radiation");
            paramNames.push_back("Spacecraft Thermal Radiation Pressure");
            paramNames.push_back("Relativistic Accelerations");

            skipGravityModel = true;
            skipRadPressure = true;
            skipDrag = true;
            gmIndex = 0;
            rpIndex = 0;
            dragIndex = 0;
         }
         
         // 3.6. Clear paramValues and paramUnits
         paramValues.clear();
      }
   }

   for (UnsignedInt j = 0; j < rowContent.size(); ++j)
   {
      // skip 1 line showing the details of central body gravity model
      if (skipGravityModel && (j == (gmIndex + 1)))
         continue;

      // skip 3 lines showing the details of solar radiation pressure 
      if (skipRadPressure && (rpIndex + 1 <= j) &&(j <= rpIndex + 3))
         continue;

      // skip 1 line showing the details of drag model
      if (skipDrag && (j == (dragIndex + 1)))
         continue;

      textFile << rowContent[j] << "\n";
   }
   textFile << "\n";
   textFile << "\n";

   textFile.flush();
}


void BatchEstimator::WriteReportFileHeaderPart4_1()
{
   // 1. Write sub header
   textFile << GmatStringUtil::GetAlignmentString("", 66) << "Tracking Data Configuration\n";
   textFile << "\n";

   StringArray paramNames, paramValues, rowContent;

   // 2. Set values to paramNames
   paramNames.push_back("Tracking File Set");
   paramNames.push_back("");
   paramNames.push_back("Tracking Data");
   paramNames.push_back("Light Time");
   paramNames.push_back("Relativistic Corrections");
   paramNames.push_back("ET-TAI Corrections");
   paramNames.push_back("Frequency Model");
   paramNames.push_back("  Ramp Table");
   
   Integer nameLen = 0;
   for (Integer i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());
   
   // 3. Set values to rowContent
   Integer colCount = 0;
   Integer maxNumConfig = 1;
   std::vector<TrackingFileSet*> tfsList = GetMeasurementManager()->GetAllTrackingFileSets();
   for (Integer i = 0; i < tfsList.size(); ++i)
   {
      // 3.1. Set values to paramValues
      // TrackingFileSet name
      paramValues.push_back(tfsList[i]->GetName());
      paramValues.push_back("");
      
      // Tracking configurations
      StringArray trackingConfigs = tfsList[i]->GetStringArrayParameter("AddTrackingConfig");
      if (trackingConfigs.size() == 0)
      {
         paramValues.push_back("All");
         for (Integer j = 1; j < maxNumConfig; ++j)
            paramValues.push_back("");
      }
      else
      {
         
         if (maxNumConfig >= trackingConfigs.size())
         {
            for (Integer j = 0; j < trackingConfigs.size(); ++j)
               paramValues.push_back(trackingConfigs[j]);
            for (Integer j = trackingConfigs.size(); j < maxNumConfig; ++j)
               paramValues.push_back("");
         }
         else
         {
            for (Integer j = 0; j < trackingConfigs.size(); ++j)
               paramValues.push_back(trackingConfigs[j]);

            // Insert blank lines to paramNames
            StringArray::iterator pos1 = paramNames.begin() + 2 + maxNumConfig;
            paramNames.insert(pos1, trackingConfigs.size() - maxNumConfig, "");

            if (colCount != 0)
            {
               StringArray::iterator pos2 = rowContent.begin() + 2 + maxNumConfig;
               rowContent.insert(pos2, trackingConfigs.size() - maxNumConfig, GmatStringUtil::GetAlignmentString("", (pos2-1)->size()));
            }

            maxNumConfig = trackingConfigs.size();
         }
      }
      
      paramValues.push_back(((tfsList[i]->GetBooleanParameter("UseLightTime")) ? "Yes" : "No"));               // Light Time
      paramValues.push_back(((tfsList[i]->GetBooleanParameter("UseRelativityCorrection")) ? "Yes" : "No"));    // Relativistic Corrections
      paramValues.push_back(((tfsList[i]->GetBooleanParameter("UseETminusTAI")) ? "Yes" : "No"));              // ET-TAI Corrections

      // Ramp Table
      StringArray rtList = tfsList[i]->GetStringArrayParameter("RampTable");
      if (rtList.size() == 0)
      {
         paramValues.push_back("Constant Frequency");
         paramValues.push_back("N/A");
      }
      else
      {
         paramValues.push_back("Ramp Table");
         paramValues.push_back("'" + rtList[0] + "'");
      }
      
      // 3.2. Set values to rowContent
      Integer valueLen = 0;
      for (Integer j = 0; j < paramNames.size(); ++j)
         valueLen = (Integer)GmatMathUtil::Max(valueLen, paramValues[j].size());

      std::string s;
      for (Integer j = 0; j < paramNames.size(); ++j)
      {
         if (colCount == 0)
         {
            s = " " + GmatStringUtil::GetAlignmentString(paramNames[j], nameLen + 3) + GmatStringUtil::GetAlignmentString(paramValues[j], valueLen);
            rowContent.push_back(s);
         }
         else
            rowContent[j] += ("   " + GmatStringUtil::GetAlignmentString(paramValues[j], valueLen));
      }

      // 3.3. Increase column count by 1
      ++colCount;
      
      // 3.4. Break the column when the size is too big
      if (colCount == 2)
      {
         // Write rowContent to report file
         for (Integer j = 0; j < rowContent.size(); ++j)
            textFile << rowContent[j] << "\n";
         textFile << "\n";

         colCount = 0;
         rowContent.clear();

         // reset paramNames
         paramNames.clear();
         paramNames.push_back("Tracking File Set");
         paramNames.push_back("");
         paramNames.push_back("Tracking Data");
         paramNames.push_back("Light Time");
         paramNames.push_back("Relativistic Corrections");
         paramNames.push_back("ET-TAI Corrections");
         paramNames.push_back("Frequency Model");
         paramNames.push_back("  Ramp Table");
      }
      
      // 3.5. Clear paramValues
      paramValues.clear();
   }
   
   // 4. Write rowContent to report file
   for (Integer j = 0; j < rowContent.size(); ++j)
      textFile << rowContent[j] << "\n";
   textFile << "\n";

   textFile.flush();
}


//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart4_2()
//------------------------------------------------------------------------------
/**
* This function is used to write measurement modeling options to the report file.
*/
//------------------------------------------------------------------------------
void BatchEstimator::WriteReportFileHeaderPart4_2()
{
   StringArray paramNames, paramValues, rowContent;
   
   // 2. Fill in parameter's name
   paramNames.push_back("Name");
   paramNames.push_back("Central Body");
   paramNames.push_back("State Type");
   paramNames.push_back("Horizon Reference");
   paramNames.push_back("Location1 (km)");
   paramNames.push_back("Location2 (km)");
   paramNames.push_back("Location3 (km)");
   paramNames.push_back("Pad ID");
   paramNames.push_back("Min.Elevation Angle (deg)");
   paramNames.push_back("Ionosphere Model");
   paramNames.push_back("Troposphere Model");
   paramNames.push_back("  Temperature (K)");
   paramNames.push_back("  Pressure    (hPa)");
   paramNames.push_back("  Humidity    (%)");
   paramNames.push_back("Measurement Error Models");

   Integer nameLen = 0;
   for (Integer i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());
   
   
   // 3. Write table containing ground stations' information
   textFile << GmatStringUtil::GetAlignmentString("", 66) + "Ground Station Configuration\n";
   textFile << "\n";
   
   bool skipTropoDesc = true;
   Integer tropoIndex = 10;           // index such as paramNames[index] == "Troposphere Model"

   Integer colCount = 0;
   std::stringstream ss;
   std::string gsName;
   
   Integer maxNumErrorModels = 1;
   StringArray participantNames = GetMeasurementManager()->GetParticipantList();
   for (UnsignedInt i = 0; i < participantNames.size(); ++i)
   {
      GmatBase* obj = GetConfiguredObject(participantNames[i]);
      if (obj->IsOfType(Gmat::GROUND_STATION))
      {
         // 3.1. Get a ground station for processing:
         GroundstationInterface *gs = (GroundstationInterface *)obj;
         gsName = participantNames[i];

         // 3.2. Fill in parameter's value
         paramValues.push_back(gs->GetName());
         paramValues.push_back(gs->GetStringParameter("CentralBody"));
         paramValues.push_back(gs->GetStringParameter("StateType"));
         paramValues.push_back(gs->GetStringParameter("HorizonReference"));
         ss.str(""); ss << GmatStringUtil::RealToString(gs->GetRealParameter("Location1"), false, false, false, 8); paramValues.push_back(ss.str());
         ss.str(""); ss << GmatStringUtil::RealToString(gs->GetRealParameter("Location2"), false, false, false, 8); paramValues.push_back(ss.str());
         ss.str(""); ss << GmatStringUtil::RealToString(gs->GetRealParameter("Location3"), false, false, false, 8); paramValues.push_back(ss.str());
         paramValues.push_back(gs->GetStringParameter("Id"));
         ss.str(""); ss << GmatStringUtil::RealToString(gs->GetRealParameter("MinimumElevationAngle"), false, false, false, 8); paramValues.push_back(ss.str());
         paramValues.push_back(gs->GetStringParameter("IonosphereModel"));
         paramValues.push_back(gs->GetStringParameter("TroposphereModel"));

         if (gs->GetStringParameter("TroposphereModel") != "None")
         {
            ss.str(""); ss << GmatStringUtil::RealToString(gs->GetRealParameter("Temperature"), false, false, false, 8); paramValues.push_back(ss.str());
            ss.str(""); ss << GmatStringUtil::RealToString(gs->GetRealParameter("Pressure"), false, false, false, 8); paramValues.push_back(ss.str());
            ss.str(""); ss << GmatStringUtil::RealToString(gs->GetRealParameter("Humidity"), false, false, false, 8); paramValues.push_back(ss.str());
            
            skipTropoDesc = false;
         }
         else
         {
            paramValues.push_back("");         // Temperature
            paramValues.push_back("");         // Pressure
            paramValues.push_back("");         // Humidity
         }

         StringArray emList = gs->GetStringArrayParameter("ErrorModels");
         if (emList.size() == 0)
         {
            paramValues.push_back("None");
            for (Integer j = 1; j < maxNumErrorModels; ++j)
               paramValues.push_back("");
         }
         else
         {
            if (maxNumErrorModels >= emList.size())
            {
               for (Integer j = 0; j < emList.size(); ++j)
                  paramValues.push_back(emList[j]);

               for (Integer j = emList.size(); j < maxNumErrorModels; ++j)
                  paramValues.push_back("");
            }
            else
            {
               for (Integer j = 0; j < emList.size(); ++j)
               {
                  paramValues.push_back(emList[j]);
               }
               
               // Insert blank lines to paramNames
               for (Integer j = 0; j < emList.size() - maxNumErrorModels; ++j)
               {
                  paramNames.push_back(GmatStringUtil::GetAlignmentString("", nameLen));
               }
               
               maxNumErrorModels = emList.size();
            }
         }
         
         // 3.3. Increasing column count by 1
         ++colCount;
         
         // 3.4. Write information of the ground station to the column
         for (UnsignedInt j = 0; j < paramNames.size(); ++j)
         {
            if (j == rowContent.size())
            {
               if (colCount == 1)
                  rowContent.push_back("");
               else
                  rowContent.push_back(GmatStringUtil::GetAlignmentString("", rowContent.at(rowContent.size() - 1).size()));
            }
         }
         for (UnsignedInt j = 0; j < paramNames.size(); ++j)
         {
            if (colCount == 1)
               rowContent[j] += (" " + GmatStringUtil::GetAlignmentString(paramNames[j], nameLen, GmatStringUtil::LEFT) + "  ");
            
            rowContent[j] += (GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(paramValues[j]), 27, GmatStringUtil::LEFT)+" ");      // each column has size of 28
         }
         

         // 3.5. Beak up columns in a table
         if ((nameLen+3+ colCount*24) > (160-48))
         {
            for (UnsignedInt j = 0; j < rowContent.size(); ++j)
            {
               // Remove 3 lines containing information about Temperature, Pressure, and Humidity when Troposphere model set to None for all stations in table
               if (skipTropoDesc && (tropoIndex < j) && (j <= tropoIndex + 3))
                  continue;

               textFile << rowContent[j] << "\n";
            }
            textFile << "\n";
            textFile << "\n";

            rowContent.clear();
            colCount = 0;

            // Reset paramNames
            paramNames.clear();
            paramNames.push_back("Name");
            paramNames.push_back("Central Body");
            paramNames.push_back("State Type");
            paramNames.push_back("Horizon Reference");
            paramNames.push_back("Location1 (km)");
            paramNames.push_back("Location2 (km)");
            paramNames.push_back("Location3 (km)");
            paramNames.push_back("Pad ID");
            paramNames.push_back("Min.Elevation Angle (deg)");
            paramNames.push_back("Ionosphere Model");
            paramNames.push_back("Troposphere Model");
            paramNames.push_back("  Temperature (K)");
            paramNames.push_back("  Pressure    (hPa)");
            paramNames.push_back("  Humidity    (%)");
            paramNames.push_back("Measurement Error Models");

            skipTropoDesc = true;
         }
         
         // 3.6. Clear paramValues and paramUnits
         paramValues.clear();
      }
   }
   
   for (UnsignedInt j = 0; j < rowContent.size(); ++j)
   {
      // Remove 3 lines containing information about Temperature, Pressure, and Humidity when Troposphere model set to None for all stations in table
      if (skipTropoDesc && (tropoIndex < j) && (j <= tropoIndex + 3))
         continue;

      textFile << rowContent[j] << "\n";
   }
   textFile << "\n";
   textFile << "\n";

   textFile.flush();
}


//-------------------------------------------------------------------------
// void BatchEstimator::WriteReportFileHeaderPart4_3()
//-------------------------------------------------------------------------
/**
* Write information about measurement ErrorModel to estimation report file
*/
//-------------------------------------------------------------------------
void BatchEstimator::WriteReportFileHeaderPart4_3()
{
   // 1. Get a list of all error models
   StringArray emList;
   StringArray participantNames = GetMeasurementManager()->GetParticipantList();
   for (UnsignedInt i = 0; i < participantNames.size(); ++i)
   {
      GmatBase* obj = GetConfiguredObject(participantNames[i]);
      if (obj->IsOfType(Gmat::GROUND_STATION))
      {
         // 1.1. Get a ground station
         GroundstationInterface *gs = (GroundstationInterface *)obj;

         // 1.2. Get error models used by that ground station
         StringArray errorModels = gs->GetStringArrayParameter("ErrorModels");

         // 1.3. Add those error models to list of all error models
         for (Integer j = 0; j < errorModels.size(); ++j)
         {
            bool found = false;
            for (Integer k = 0; k < emList.size(); ++k)
            {
               if (emList[k] == errorModels[j])
               {
                  found = true;
                  break;
               }
            }

            if (!found)
               emList.push_back(errorModels[j]);
         }
      }
   }


   // 2. Write table containing error models' information
   textFile << GmatStringUtil::GetAlignmentString("", 66) + "Measurement Error Models\n";
   textFile << "\n";

   StringArray paramNames, paramValues, rowContent;

   // 2.1. Fill in parameter's name
   paramNames.push_back("Name");
   paramNames.push_back("Measurement Type");
   paramNames.push_back("Noise Sigma");
   paramNames.push_back("Bias");
   paramNames.push_back("Bias Sigma");
   paramNames.push_back("Solve Fors");
   
   Integer nameLen = 0;
   for (Integer i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());

   // 2.2. Set value to paramValues
   Integer colCount = 0;
   std::stringstream ss;

   Integer maxNumSolveFors = 1;
   for (Integer i = 0; i < emList.size(); ++i)
   {
      // 2.2.1. Get ErrorModel object
      ErrorModel *em = (ErrorModel*)GetConfiguredObject(emList[i]);

      // 2.2.2. Fill in parameter's value
      paramValues.push_back(em->GetName());                                                         // Name
      paramValues.push_back(em->GetStringParameter("Type"));                                        // Measurement Type
      ss.str(""); ss << GmatStringUtil::RealToString(em->GetRealParameter("NoiseSigma"), false, false, false, 8); paramValues.push_back(ss.str());        // Noise Sigma
      ss.str(""); ss << GmatStringUtil::RealToString(em->GetRealParameter("Bias"), false, false, false, 8); paramValues.push_back(ss.str());              // Bias
      ss.str(""); ss << 0.1; paramValues.push_back(ss.str());                                       // Bias Sigma

      StringArray sfList = em->GetStringArrayParameter("SolveFors");
      if (sfList.size() == 0)
      {
         paramValues.push_back("None");
         for (Integer j = 1; j < maxNumSolveFors; ++j)
            paramValues.push_back("");
      }
      else
      {
         if (maxNumSolveFors >= sfList.size())
         {
            for (Integer j = 0; j < sfList.size(); ++j)
               paramValues.push_back(sfList[j]);                                                   // Solve Fors

            for (Integer j = sfList.size(); j < maxNumSolveFors; ++j)
               paramValues.push_back("");
         }
         else
         {
            for (Integer j = 0; j < sfList.size(); ++j)
                  paramValues.push_back(sfList[j]);                                               // Solve Fors

            // Insert blank lines to paramNames and rowContent
            StringArray::iterator pos1 = paramNames.begin() + (paramNames.size() - 1);
            paramNames.insert(pos1, sfList.size() - maxNumSolveFors, "");

            if (colCount != 0)
            {
               StringArray::iterator pos2 = rowContent.begin() + (paramNames.size() - 1);
               rowContent.insert(pos2, sfList.size()-maxNumSolveFors, GmatStringUtil::GetAlignmentString("", (pos2-1)->size()));
            }

            maxNumSolveFors = sfList.size();
         }
      }
      

      // 2.2.3. Increasing column count by 1
      ++colCount;

      // 2.2.4. Write information of the error model to the column
      for (UnsignedInt j = 0; j < paramNames.size(); ++j)
      {
         if (j == rowContent.size())
            rowContent.push_back("");

         if (colCount == 1)
            rowContent[j] += (" " + GmatStringUtil::GetAlignmentString(paramNames[j], nameLen, GmatStringUtil::LEFT) + "  ");

         rowContent[j] += (GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(paramValues[j]), 22, GmatStringUtil::LEFT) + "  ");
      }

      // 3.5. Beak up columns in a table
      if ((nameLen + 3 + colCount * 24) > (160 - 24))
      {
         for (UnsignedInt j = 0; j < rowContent.size(); ++j)
            textFile << rowContent[j] << "\n";
         textFile << "\n";
         textFile << "\n";

         rowContent.clear();
         colCount = 0;

         paramNames.clear();
         paramNames.push_back("Name");
         paramNames.push_back("Measurement Type");
         paramNames.push_back("Noise Sigma");
         paramNames.push_back("Bias");
         paramNames.push_back("Bias Sigma");
         paramNames.push_back("Solve Fors");
      }

      // 3.6. Clear paramValues and paramUnits
      paramValues.clear();
   }

   for (UnsignedInt j = 0; j < rowContent.size(); ++j)
      textFile << rowContent[j] << "\n";
   textFile << "\n";
   textFile << "\n";

   textFile.flush();
}


//----------------------------------------------------------------------
// void WriteReportFileHeaderPart4()
//----------------------------------------------------------------------
/**
* Write Measurement Modeling section to estimation report file 
*/
//----------------------------------------------------------------------
void BatchEstimator::WriteReportFileHeaderPart4()
{
   // 1. Write subheader
   textFile << "********************************************************************  MEASUREMENT MODELING  ********************************************************************\n";
   textFile << "\n";

   // 2. Write information about tracking file sets
   WriteReportFileHeaderPart4_1();
   
   // 3. Write information about ground stations
   WriteReportFileHeaderPart4_2();
   
   // 4. Write information about error models
   WriteReportFileHeaderPart4_3();
}

//*******************************************************************  ASTRODYNAMIC CONSTANTS  *******************************************************************
//
//Planetary Ephemeris                                   DE421
//Solar Irradiance(W / m ^ 2 at 1 AU)                      1358.0
//Speed of Light(km / sec)                               1234567.123
//Universal Gravitational Constant(gm*km ^ 3 / sec ^ 2)      6.6730000e-22
//
//Central Body                              Earth               Luna                 Sun
//Gravitational Constant(km ^ 3 / sec ^ 2)       3.9860044000e+06    4.90279920000e+04    1.32712200000e+12
//Mean Equatorial Radius(km)               6.3781400000e+04    6.37814000000e+04    6.37814000000e+04
//Inverse Flattening Coefficient            2.9825700000e+03    1.00000000000e+00    1.00000000000e+00
//Rotation Rate(rad / sec)                   7.2921159000e-04    7.29211590000e-04    7.29211590000e-04

//----------------------------------------------------------------------
// void WriteReportFileHeaderPart5()
//----------------------------------------------------------------------
/**
* Write Astrodynamic Constants section to estimation report file
*/
//----------------------------------------------------------------------
void BatchEstimator::WriteReportFileHeaderPart5()
{
   // 1. Write astrodynamic contants header
   textFile << "*******************************************************************  ASTRODYNAMIC CONSTANTS  *******************************************************************\n";
   textFile << "\n";
   
   textFile << " Planetary Ephemeris                                 " << solarSystem->GetStringParameter("EphemerisSource") << "\n";
   textFile << " Solar Irradiance (W/m^2 at 1 AU)                    1358.0\n";
   textFile << " Speed of Light (km/sec)                             " << GmatStringUtil::RealToString(GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / 1000.0, false, false, false, 6) << "\n";
   textFile << " Universal Gravitational Constant (km^3/kg*sec^2)    " << GmatStringUtil::RealToString(GmatPhysicalConstants::UNIVERSAL_GRAVITATIONAL_CONSTANT, false, true, true, 6) << "\n";
   textFile << "\n";

   // 2. Write information about central bodies to report file
   // 2.1. Get all central body objects
   StringArray nameList, cbNames;
   std::string name = "";
   // 2.1.1. Add central body's name from participants to cbNames
   StringArray participantNames = GetMeasurementManager()->GetParticipantList();
   for (Integer i = 0; i < participantNames.size(); ++i)
   {
      // Get name of central body from participants
      name = "";
      GmatBase* obj = GetConfiguredObject(participantNames[i]);
      if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         // Get central body objects used in spacecrafts' coordinate system
         Spacecraft *sc = (Spacecraft*)obj;
         std::string csName = sc->GetStringParameter("CoordinateSystem");
         CoordinateSystem *cs = NULL;
         // undo code to handle bug GMT-5619 due to it was handle by Spacecraft's code
         //try
         //{
            cs = (CoordinateSystem *)GetConfiguredObject(csName);
         //}
         //catch (...)
         //{
         //   throw EstimatorException("Error: CoordinateSystem object with name '" + csName + "' set to " + sc->GetName() + ".CoordinateSystem was not defined in GMAT script.\n");
         //}
         name = cs->GetStringParameter("Origin");
      }
      else if (obj->IsOfType(Gmat::GROUND_STATION))
      {
         // Get central body objects used in ground stations' contral body
         GroundstationInterface *gs = (GroundstationInterface*)obj;
         name = gs->GetStringParameter("CentralBody");
      }
      
      nameList.push_back(name);
   }

   // 2.1.2. Add central body's name used in force models to cbNames
   PropSetup* propSetup = GetPropagator();
   ODEModel* ode = propSetup->GetODEModel();
   name = ode->GetStringParameter("CentralBody");
   nameList.push_back(name);

   // 2.1.3. Add all point masses
   StringArray sa = ode->GetStringArrayParameter("PointMasses");
   for (UnsignedInt i = 0; i < sa.size(); ++i)
      nameList.push_back(sa[i]);

   // 2.1.4. Create a list of all celestial bodies
   bool found;
   for (UnsignedInt i = 0; i < nameList.size(); ++i)
   {
      if (nameList[i] == "")
         continue;

      found = false;
      for (Integer j = 0; j < cbNames.size(); ++j)
      {
         if (cbNames[j] == nameList[i])
         {
            found = true;
            break;
         }
      }
      if (!found)
         cbNames.push_back(nameList[i]);
   }


   // 2.2. Write information about central body
   StringArray paramNames, paramValues, rowContent;
   
   // 2.2.1. Set value to paramNames
   paramNames.push_back("Celestial Body");
   paramNames.push_back("Gravitational Constant (km^3/sec^2)");
   paramNames.push_back("Mean Equatorial Radius (km)");
   paramNames.push_back("Inverse Flattening Coefficient");
   paramNames.push_back("Rotation Rate (deg/day)");

   Integer nameLen = 0;
   for (Integer i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());

   std::stringstream ss;
   Integer colCount = 0;
   for (Integer i = 0; i < cbNames.size(); ++i)
   {
      // Get central body object
      CelestialBody* cb = solarSystem->GetBody(cbNames[i]);

      // Set value to paramValues
      paramValues.push_back(cb->GetName());                                                                                 // Central Body
      ss.str(""); ss << GmatStringUtil::RealToString(cb->GetRealParameter(cb->GetParameterID("Mu")), false, false, false, 8); paramValues.push_back(ss.str());                    // Gravitational Constant
      ss.str(""); ss << GmatStringUtil::RealToString(cb->GetRealParameter(cb->GetParameterID("EquatorialRadius")), false, false, false, 8); paramValues.push_back(ss.str());      // Mean Equatorial Radius
      ss.str(""); ss << GmatStringUtil::RealToString(1.0 / cb->GetRealParameter(cb->GetParameterID("Flattening")), false, false, false, 8); paramValues.push_back(ss.str());      // Inverse Flattening Cofficient
      ss.str(""); ss << GmatStringUtil::RealToString(cb->GetRealParameter(cb->GetParameterID("RotationRate")), false, false, false, 8); paramValues.push_back(ss.str());          // Rotation Rate

      Integer valueLen = 20;
      for (Integer j = 0; j < paramValues.size(); ++j)
         valueLen = (Integer)GmatMathUtil::Max(valueLen, paramValues[j].size());

      // Set value for rowContent
      if (colCount == 0)
      {
         for (Integer j = 0; j < paramNames.size(); ++j)
            rowContent.push_back(GmatStringUtil::GetAlignmentString(paramNames[j], nameLen+5));
      }

      for (Integer j = 0; j < paramNames.size(); ++j)
         rowContent[j] += GmatStringUtil::GetAlignmentString(paramValues[j], valueLen+1);
      
      // increase colCount by 1
      ++colCount;

      // break the table as needed
      if (colCount == CELESTIAL_BODIES_TABLE_COLUMN_BREAK_UP)
      {
         for (Integer j = 0; j < rowContent.size(); ++j)
            textFile << " " << rowContent[j] << "\n";
         textFile << "\n";

         rowContent.clear();
         colCount = 0;
      }
      
      // clear paramValues
      paramValues.clear();
   }
   
   for (Integer j = 0; j < rowContent.size(); ++j)
      textFile << " " << rowContent[j] << "\n";
   textFile << "\n";

   textFile.flush();
}


//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart6()
//------------------------------------------------------------------------------
/**
* This function is used to write estimation options to the report file.
*/
//------------------------------------------------------------------------------
void BatchEstimator::WriteReportFileHeaderPart6()
{
   // 1. Write estimation options header
   textFile << "*********************************************************************  ESTIMATION OPTIONS  *********************************************************************\n";
   textFile << "\n";

   std::stringstream ss;
   StringArray sa1, sa2, sa3;
   
   
   // 2. Write data to the first and second columns
   ss.str("");
   if ((maxResidualMult == 0.0) || ((GmatMathUtil::Abs(maxResidualMult) < 1.0e6) && (GmatMathUtil::Abs(maxResidualMult) > 1.0e-2)))
      ss << maxResidualMult;
   else
      ss << GmatStringUtil::RealToString(maxResidualMult, false, true);
   sa1.push_back("OLSE Initial RMS Sigma"); sa2.push_back(ss.str());

   ss.str("");
   if ((constMult == 0.0) || ((GmatMathUtil::Abs(constMult) < 1.0e6) && (GmatMathUtil::Abs(constMult) > 1.0e-2)))
      ss << constMult;
   else
      ss << GmatStringUtil::RealToString(constMult, false, true);
   sa1.push_back("OLSE Multiplicative Constant"); sa2.push_back(ss.str());

   ss.str("");
   if ((additiveConst == 0.0) || ((GmatMathUtil::Abs(additiveConst) < 1.0e6) && (GmatMathUtil::Abs(additiveConst) > 1.0e-2)))
      ss << additiveConst;
   else
      ss << GmatStringUtil::RealToString(additiveConst, false, true);
   sa1.push_back("OLSE Additive Constant"); sa2.push_back(ss.str());

   ss.str(""); ss << GetRealParameter("AbsoluteTol"); sa1.push_back("Absolute Tolerance for Convergence"); sa2.push_back(ss.str());
   ss.str(""); ss << GetRealParameter("RelativeTol"); sa1.push_back("Relative Tolerance for Convergence"); sa2.push_back(ss.str());
   ss.str(""); ss << GetIntegerParameter("MaximumIterations"); sa1.push_back("Maximum Iterations"); sa2.push_back(ss.str());
   ss.str(""); ss << GetIntegerParameter("MaxConsecutiveDivergences"); sa1.push_back("Maximum Consecutive Divergences"); sa2.push_back(ss.str());


   // 3. Write the 3rd column
   Real taiMjdEpoch, utcMjdEpoch;
   std::string utcEpoch;

   sa3.push_back("Estimation Epoch :");
   sa3.push_back("");

   ss.precision(15);
   if (estEpochFormat != "FromParticipants")
   {
      ss.str(""); ss << "   " << estEpoch << " " << estEpochFormat; sa3.push_back(ss.str());
      sa3.push_back("");
      sa3.push_back("");
   }
   else
   {
      char s[100];
      taiMjdEpoch = TimeConverterUtil::Convert(estimationEpoch, TimeConverterUtil::A1MJD, TimeConverterUtil::TAIMJD);
      utcMjdEpoch = TimeConverterUtil::Convert(estimationEpoch, TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD);
      bool handleLeapSecond = TimeConverterUtil::HandleLeapSecond();
      utcEpoch = TimeConverterUtil::ConvertMjdToGregorian(utcMjdEpoch, handleLeapSecond);

      ss.str(""); ss << utcEpoch << " UTCG"; sa3.push_back(ss.str());
      ss.str(""); ss << estimationEpoch << " A.1 Mod. Julian"; sa3.push_back(ss.str());
      ss.str(""); ss << taiMjdEpoch << " TAI Mod. Julian"; sa3.push_back(ss.str());
   }
   sa3.push_back("");
   sa3.push_back("");

   // 4. Write to text file
   Integer nameLen = 0;
   for (Integer i = 0; i < sa1.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, sa1[i].size());

   for (Integer i = 0; i < sa1.size(); ++i)
   {
      textFile << GmatStringUtil::GetAlignmentString("", 33)
         << GmatStringUtil::GetAlignmentString(sa1[i], nameLen + 2, GmatStringUtil::LEFT)
         << GmatStringUtil::GetAlignmentString(sa2[i], 95-(35 + nameLen), GmatStringUtil::LEFT)
         << sa3[i] << "\n";
   }
   textFile << "\n";

   textFile.flush();
}


//----------------------------------------------------------------------
// void WriteIterationHeader()
//----------------------------------------------------------------------
/**
* Write iteration header
*/
//----------------------------------------------------------------------
void BatchEstimator::WriteIterationHeader()
{
   /// 1. Write iteration header
   textFile
      << "************************************************************  ITERATION " << GmatStringUtil::ToString(iterationsTaken, 3) << ":  MEASUREMENT RESIDUALS  ***********************************************************\n"
      << "\n"
      << "                                                                  Notations Used In Report File\n"
      << "\n"
      << "                  - : Not edited                                                     BXY  : Blocked, X = Path index, Y = Count index(Doppler)\n"
      << "                  U : Unused because no computed value configuration available       IRMS : Edited by initial RMS sigma filter\n"
      << "                  R : Out of ramp table range                                        OLSE : Edited by outer-loop sigma editor\n"
      << "\n"
      << "                                                                  Measurement and Residual Units\n"
      << "\n"
      << "              Obs-Type            Obs/Computed Units   Residual Units                      Obs-Type            Obs/Computed Units   Residual Units\n"
      << "              Doppler_RangeRate   kilometers/second    kilometers/second                   Range_KM            kilometers           kilometers\n"
      << "              Doppler             Hertz                Hertz                               DSNRange            Range Units          Range Units\n";

   textFile.flush();

   WritePageHeader();
}


//----------------------------------------------------------------------
// void WritePageHeader()
//----------------------------------------------------------------------
/**
* Write page header
*/
//----------------------------------------------------------------------
void BatchEstimator::WritePageHeader()
{
   /// 4.1. Write page header
   textFile << "\n";
   if (textFileMode == "Normal")
   {
      textFile << "Iter RecNum  UTCGregorian-Epoch        Obs-Type            " << GmatStringUtil::GetAlignmentString("Participants", pcolumnLen) << " Edit           Observed(O)          Computed (C)       Residual (O-C)  Elev.\n";
   }
   else
   {
      textFile << "Iter   RecNum  UTCGregorian-Epoch        TAIModJulian-Epoch Obs Type            Units  " << GmatStringUtil::GetAlignmentString("Participants", pcolumnLen) << " Edit               Obs (O)     Obs-Correction(O)               Cal (C)     Residual (O-C)            Weight (W)             W*(O-C)^2         sqrt(W)*|O-C|    Elevation-Angle Partial-Derivatives";
      // fill out N/A for partial derivative
      for (int i = 0; i < esm.GetStateMap()->size() - 1; ++i)
         textFile << GmatStringUtil::GetAlignmentString(" ", 20);
      textFile << "  Uplink-Band         Uplink-Frequency             Range-Modulo         Doppler-Interval\n";
   }
   textFile << "\n";

   textFile.flush();
}


//----------------------------------------------------------------------
// void WriteIterationSummaryPart1(Solver::SolverState sState)
//----------------------------------------------------------------------
/**
* Write statistics summary for estimation
*/
//----------------------------------------------------------------------
void BatchEstimator::WriteIterationSummaryPart1(Solver::SolverState sState)
{
   if (sState == ESTIMATING)
   {
      // 1. Write summary part 1 header:
      textFile0 << "\n";
      textFile0 << "***********************************************************  ITERATION " << GmatStringUtil::ToString(iterationsTaken, 3) << ":  MEASUREMENT STATISTICS  ***********************************************************\n";
      textFile0 << "\n";
      textFile0.flush();
   }

   if (sState == FINISHED)
   {
      std::stringstream ss;

      /// 1.1. Write estimation status
      ss << "***  Estimation ";
      switch (estimationStatus)
      {
      case ABSOLUTETOL_CONVERGED:
      case RELATIVETOL_CONVERGED:
      case ABS_AND_REL_TOL_CONVERGED:
         ss << "converged!";
         break;
      case MAX_CONSECUTIVE_DIVERGED:
      case MAX_ITERATIONS_DIVERGED:
      case CONVERGING:
      case DIVERGING:
         ss << "did not converge!";
         break;
      case UNKNOWN:
         break;
      };
      ss << "  ***";
      textFile0 << GmatStringUtil::GetAlignmentString(ss.str(), 160, GmatStringUtil::CENTER) << "\n";

      // 1.2. Write reason for convergence
      textFile0 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(convergenceReason.substr(0,convergenceReason.size()-1), GmatStringUtil::BOTH), 160, GmatStringUtil::CENTER) << "\n";

      // 1.3. Write number of iterations was run for estimation
      ss.str("");
      ss << "Estimating completed in " << iterationsTaken << " iterations";
      textFile0 << GmatStringUtil::GetAlignmentString(ss.str(), 160, GmatStringUtil::CENTER);
      textFile0 << "\n";
      textFile0 << "\n";
      textFile0.flush();
   }

   if (sState == ESTIMATING)
   {
      std::stringstream ss;
      ss << "Total Number Of Observations              : " << GetMeasurementManager()->GetObservationDataList()->size();
      textFile1 << GmatStringUtil::GetAlignmentString("", 33) << GmatStringUtil::GetAlignmentString(ss.str(), 60) << "Current WRMS Residuals   : " << newResidualRMS << "\n";
      ss.str(""); ss << "Observations Used For Estimation          : " << measurementResiduals.size();
      textFile1 << GmatStringUtil::GetAlignmentString("", 33) << GmatStringUtil::GetAlignmentString(ss.str(), 60) << "Predicted WRMS Residuals : " << predictedRMS << "\n";

      ss.str(""); ss << "No Computed Value Configuration Available : " << numRemovedRecords["U"];
      textFile1 << GmatStringUtil::GetAlignmentString("", 33) << GmatStringUtil::GetAlignmentString(ss.str(), 60);
      if (iterationsTaken != 0)
         textFile1 << "Previous WRMS Residuals  : " << oldResidualRMS << "\n";
      else
         textFile1 << "Previous WRMS Residuals  : " << "N/A" << "\n";

      
      ss.str(""); ss << "Out of Ramp Table Range                   : " << numRemovedRecords["R"] << " ";
      textFile1 << GmatStringUtil::GetAlignmentString("", 33) << GmatStringUtil::GetAlignmentString(ss.str(), 60) << "Smallest WRMS Residuals  : " << bestResidualRMS << "\n";

      // 2. Write data records usage summary:
      ss.str("");  ss << "Signal Blocked                            : " << numRemovedRecords["B"];
      textFile1 << GmatStringUtil::GetAlignmentString("", 33) << GmatStringUtil::GetAlignmentString(ss.str(), 60);
      ss.str(""); ss << "Sigma Editing                             : " << ((iterationsTaken == 0) ? numRemovedRecords["IRMS"] : numRemovedRecords["OLSE"]);
      textFile1_1 << GmatStringUtil::GetAlignmentString("", 33) << GmatStringUtil::GetAlignmentString(ss.str(), 60) << "\n";
      textFile1_1 << "\n";
      textFile1_1.flush();

      textFile1.flush();
   }
   
   if ((sState == CHECKINGRUN) || (sState == FINISHED))
   {
      // 4. Convergence status summary:
      textFile1 << "DC Status                : ";
      switch (estimationStatus)
      {
      case ABSOLUTETOL_CONVERGED:
         textFile1 << "Absolute Tolerance Converged";
         break;
      case RELATIVETOL_CONVERGED:
         textFile1 << "Relative Tolerance Converged";
         break;
      case ABS_AND_REL_TOL_CONVERGED:
         textFile1 << "Absolute and Relative Tolerance Converged";
         break;
      case MAX_CONSECUTIVE_DIVERGED:
         textFile1 << "Maximum Consecutive Diverged";
         break;
      case MAX_ITERATIONS_DIVERGED:
         textFile1 << "Maximum Iterations Diverged";
         break;
      case CONVERGING:
         textFile1 << "Converging";
         break;
      case DIVERGING:
         textFile1 << "Diverging";
         break;
      case UNKNOWN:
         textFile1 << "Unknown";
         break;
      }
      textFile1 << "\n";

      textFile1.flush();
   }
}


//----------------------------------------------------------------------
// std::string GetUnit(std::string type)
//----------------------------------------------------------------------
/**
* Get unit for a given observation data type
*/
//----------------------------------------------------------------------
std::string BatchEstimator::GetUnit(std::string type)
{
   std::string unit = "";
   if (type == "DSNRange")
      unit = "RU";
   else if (type == "Doppler")
      unit = "Hz";
   else if (type == "Range_KM")
      unit = "km";
   else if (type == "Doppler_HZ")
      unit = "Hz";
   else if (type == "Doppler_RangeRate")
      unit = "km/s";
   else if (type == "TDRSDoppler_HZ")
      unit = "Hz";
   else if (type == "DSNTwoWayRange")
      unit = "RU";
   else if (type == "DSNTwoWayDoppler")
      unit = "Hz";
   else if (type == "USNTwoWayRange")
      unit = "km";
   else if (type == "USNTwoWayDopple")
      unit = "Hz";

   return unit;
}


//----------------------------------------------------------------------
// void WriteIterationSummaryPart2(Solver::SolverState sState)
//----------------------------------------------------------------------
/**
* Write observation statistics summary
*/
//----------------------------------------------------------------------
void BatchEstimator::WriteIterationSummaryPart2(Solver::SolverState sState)
{
   if (sState == ESTIMATING)
   {
      /// 0. Get a list of ground station objects
      StringArray participants = GetMeasurementManager()->GetParticipantList();
      ObjectArray stations;
      for (Integer i = 0; i < participants.size(); ++i)
      {
         GmatBase* obj = GetConfiguredObject(participants[i]);
         if (obj->IsOfType(Gmat::GROUND_STATION))
            stations.push_back(obj);
      }

      /// 1. Write observation summary by station and data type
      // 1.1. Write table header
      textFile2 << "\n";
      textFile2 << GmatStringUtil::GetAlignmentString("", 58) + "Observation Summary by Station and Data Type\n";
      textFile2 << "\n";
      textFile2 << "                                                                                Mean      Standard      Weighted     User          Mean      Standard\n";
      textFile2 << " Pad# Station        Data Type             Total   Accepted    Percent      Residual     Deviation           RMS   Edited      Residual     Deviation  Units\n";
      textFile2 << " --------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
      StringArray stList, typeList, keyList;
      IntegerArray sumRec, sumAccRec, sumSERec;
      RealArray sumRes, sumRes2, sumWRes2;
      RealArray sumSERes, sumSERes2, sumSEWRes2;

      // 1.2. Sort the table based on station and data type
      for (UnsignedInt i = 0; i < stationsList.size(); ++i)
      {
         // 1.2.1. Get keyword for a statistics record
         std::string keyword = stationsList[i] + " " + measTypesList[i];

         // 1.2.2. Search on keyList to find location to store the record
         UnsignedInt j = 0;
         for (; j < keyList.size(); ++j)
         {
            if (keyword < keyList[j])
               break;
         }

         // 1.2.3. Insert statistics records
         if (j == keyList.size())
         {
            keyList.push_back(keyword);
            stList.push_back(stationsList[i]);
            typeList.push_back(measTypesList[i]);
            sumRec.push_back(sumAllRecords[i]);
            sumAccRec.push_back(sumAcceptRecords[i]);
            sumRes.push_back(sumResidual[i]);
            sumRes2.push_back(sumResidualSquare[i]);
            sumWRes2.push_back(sumWeightResidualSquare[i]);

            sumSERec.push_back(sumSERecords[i]);
            sumSERes.push_back(sumSEResidual[i]);
            sumSERes2.push_back(sumSEResidualSquare[i]);
            sumSEWRes2.push_back(sumSEWeightResidualSquare[i]);
         }
         else
         {
            StringArray::iterator pos;
            pos = keyList.begin(); pos += j; keyList.insert(pos, keyword);
            pos = stList.begin(); pos += j; stList.insert(pos, stationsList[i]);
            pos = typeList.begin(); pos += j; typeList.insert(pos, measTypesList[i]);

            IntegerArray::iterator pos1;
            pos1 = sumRec.begin(); pos1 += j; sumRec.insert(pos1, sumAllRecords[i]);
            pos1 = sumAccRec.begin(); pos1 += j; sumAccRec.insert(pos1, sumAcceptRecords[i]);
            pos1 = sumSERec.begin(); pos1 += j; sumSERec.insert(pos1, sumSERecords[i]);

            RealArray::iterator pos2;
            pos2 = sumRes.begin(); pos2 += j; sumRes.insert(pos2, sumResidual[i]);
            pos2 = sumRes2.begin(); pos2 += j; sumRes2.insert(pos2, sumResidualSquare[i]);
            pos2 = sumWRes2.begin(); pos2 += j; sumWRes2.insert(pos2, sumWeightResidualSquare[i]);
            pos2 = sumSERes.begin(); pos2 += j; sumSERes.insert(pos2, sumSEResidual[i]);
            pos2 = sumSERes2.begin(); pos2 += j; sumSERes2.insert(pos2, sumSEResidualSquare[i]);
            pos2 = sumSEWRes2.begin(); pos2 += j; sumSEWRes2.insert(pos2, sumSEWeightResidualSquare[i]);
         }
      }

      // 1.3. Calculate and write statistics table:
      Integer sumRecTotal = 0;
      Integer sumAccRecTotal = 0;
      Real sumResTotal = 0.0;
      Real sumRes2Total = 0.0;
      Real sumWRes2Total = 0.0;

      Integer sumSERecTotal = 0;
      Real sumSEResTotal = 0.0;
      Real sumSERes2Total = 0.0;
      Real sumSEWRes2Total = 0.0;

      std::stringstream lines;
      std::string gsName = stList[0];
      std::string gsName1;
      for (UnsignedInt i = 0; i < stList.size(); ++i)
      {
         std::string keyword = stList[i] + " " + typeList[i];

         if (stList[i] == gsName)
         {
            // write a line on statistics table
            Real average = sumRes[i] / sumAccRec[i];
            Real stdev = GmatMathUtil::Sqrt(sumRes2[i] / sumAccRec[i] - average * average);
            Real wrms = GmatMathUtil::Sqrt(sumWRes2[i] / sumAccRec[i]);
            lines << " " 
               << GmatStringUtil::GetAlignmentString("", 19, GmatStringUtil::LEFT) << " "
               << GmatStringUtil::GetAlignmentString(typeList[i], 19, GmatStringUtil::LEFT) << "  "
               << GmatStringUtil::ToString(sumRec[i], 6) << "     "
               << GmatStringUtil::ToString(sumAccRec[i], 6) << "    "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(sumAccRec[i] * 100.0 / sumRec[i], false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(average, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(stdev, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(wrms, 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               //<< GmatStringUtil::ToString(sumSERec[i] + sumAccRec[i], 6) << "    "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERec[i] + sumAccRec[i]) * 100.0 / sumRec[i], false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERes[i] + sumRes[i]) / (sumSERec[i] + sumAccRec[i]), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSERes2[i] + sumRes2[i]) / (sumSERec[i] + sumAccRec[i])), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSEWRes2[i] + sumWRes2[i]) / (sumSERec[i] + sumAccRec[i])), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               << GmatStringUtil::GetAlignmentString("", 8, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "
               << GmatStringUtil::GetAlignmentString(GetUnit(typeList[i]), 6, GmatStringUtil::LEFT) << "\n";

            // add to total
            sumRecTotal += sumRec[i];
            sumAccRecTotal += sumAccRec[i];
            sumResTotal += sumRes[i];
            sumRes2Total += sumRes2[i];
            sumWRes2Total += sumWRes2[i];

            sumSERecTotal += sumSERec[i];
            sumSEResTotal += sumSERes[i];
            sumSERes2Total += sumSERes2[i];
            sumSEWRes2Total += sumSEWRes2[i];
         }
         else
         {
            // Write total for all data type
            gsName1 = "";
            for (Integer j = 0; j < stations.size(); ++j)
            {
               if (stations[j]->GetStringParameter("Id") == gsName)
               {
                  gsName1 = stations[j]->GetName();
                  break;
               }
            }
            
            textFile2 << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::GetAlignmentString(gsName, 4) + " " + gsName1, 19) << " "
               << GmatStringUtil::GetAlignmentString("All", 19, GmatStringUtil::LEFT) << "  "
               << GmatStringUtil::ToString(sumRecTotal, 6) << "     "
               << GmatStringUtil::ToString(sumAccRecTotal, 6) << "    "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(sumAccRecTotal * 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt(sumWRes2Total / sumAccRecTotal), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               //<< GmatStringUtil::ToString(sumSERecTotal + sumAccRecTotal, 6) << "    "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERecTotal + sumAccRecTotal)* 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               //<< GmatStringUtil::GetAlignmentString("N/A", 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString("N/A", 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSEWRes2Total + sumWRes2Total) / (sumSERecTotal + sumAccRecTotal)), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               << GmatStringUtil::GetAlignmentString("", 8, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "
               << GmatStringUtil::GetAlignmentString("", 6, GmatStringUtil::LEFT) << "\n";
            textFile2 << lines.str();
            textFile2 << "\n";

            // reset total
            sumRecTotal = 0;
            sumAccRecTotal = 0;
            sumResTotal = 0.0;
            sumRes2Total = 0.0;
            sumWRes2Total = 0.0;

            sumSERecTotal = 0;
            sumSEResTotal = 0.0;
            sumSERes2Total = 0.0;
            sumSEWRes2Total = 0.0;

            gsName = stList[i];
            lines.str("");

            // write a line on statistics table
            Real average = sumRes[i] / sumAccRec[i];
            Real stdev = GmatMathUtil::Sqrt(sumRes2[i] / sumAccRec[i] - average * average);
            Real wrms = GmatMathUtil::Sqrt(sumWRes2[i] / sumAccRec[i]);
            lines << " "
               << GmatStringUtil::GetAlignmentString("", 19, GmatStringUtil::LEFT) << " "
               << GmatStringUtil::GetAlignmentString(typeList[i], 19, GmatStringUtil::LEFT) << "  "
               << GmatStringUtil::ToString(sumRec[i], 6) << "     "
               << GmatStringUtil::ToString(sumAccRec[i], 6) << "    "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(sumAccRec[i] * 100.0 / sumRec[i], false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(average, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(stdev, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(wrms, 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               //<< GmatStringUtil::ToString(sumSERec[i] + sumAccRec[i], 6) << "    "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERec[i] + sumAccRec[i]) * 100.0 / sumRec[i], false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERes[i] + sumRes[i]) / (sumSERec[i] + sumAccRec[i]), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSERes2[i] + sumRes2[i]) / (sumSERec[i] + sumAccRec[i])), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSEWRes2[i] + sumWRes2[i]) / (sumSERec[i] + sumAccRec[i])), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               << GmatStringUtil::GetAlignmentString("", 8, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "
               << GmatStringUtil::GetAlignmentString(GetUnit(typeList[i]), 6, GmatStringUtil::LEFT) << "\n";

            // add to total
            sumRecTotal += sumRec[i];
            sumAccRecTotal += sumAccRec[i];
            sumResTotal += sumRes[i];
            sumRes2Total += sumRes2[i];
            sumWRes2Total += sumWRes2[i];

            sumSERecTotal += sumSERec[i];
            sumSEResTotal += sumSERes[i];
            sumSERes2Total += sumSERes2[i];
            sumSEWRes2Total += sumSEWRes2[i];
         }

      }
      // write total for all data type
      gsName1 = "";
      for (Integer j = 0; j < stations.size(); ++j)
      {
         if (stations[j]->GetStringParameter("Id") == gsName)
         {
            gsName1 = stations[j]->GetName();
            break;
         }
      }

      textFile2 << " "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::GetAlignmentString(gsName, 4) + " " + gsName1, 19) << " "
         << GmatStringUtil::GetAlignmentString("All", 19, GmatStringUtil::LEFT) << "  "
         << GmatStringUtil::ToString(sumRecTotal, 6) << "     "
         << GmatStringUtil::ToString(sumAccRecTotal, 6) << "    "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(sumAccRecTotal * 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
         << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt(sumWRes2Total / sumAccRecTotal), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

         //<< GmatStringUtil::ToString(sumSERecTotal + sumAccRecTotal, 6) << "    "
         //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERecTotal + sumAccRecTotal)* 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
         //<< GmatStringUtil::GetAlignmentString("N/A", 13, GmatStringUtil::RIGHT) << " "
         //<< GmatStringUtil::GetAlignmentString("N/A", 13, GmatStringUtil::RIGHT) << " "
         //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSEWRes2Total + sumWRes2Total) / (sumSERecTotal + sumAccRecTotal)), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

         << GmatStringUtil::GetAlignmentString("", 8, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "
         << GmatStringUtil::GetAlignmentString("", 6, GmatStringUtil::LEFT) << "\n";
      textFile2 << lines.str();
      textFile2 << "\n";


      /// 2. Write observation summary by measurement type and station
      // 2.1. Write table header
      textFile2 << "\n";
      textFile2 << GmatStringUtil::GetAlignmentString("", 58) + "Observation Summary by Data Type and Station\n";
      textFile2 << "\n";
      textFile2 << "                                                                                Mean      Standard      Weighted     User          Mean      Standard\n";
      textFile2 << " Data Type           Pad# Station          Total   Accepted    Percent      Residual     Deviation           RMS   Edited      Residual     Deviation  Units\n";
      textFile2 << " --------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

      stList.clear(); typeList.clear(); keyList.clear();
      sumRec.clear(); sumAccRec.clear(); sumSERec.clear();
      sumRes.clear(); sumRes2.clear(); sumWRes2.clear();
      sumSERes.clear(); sumSERes2.clear(); sumSEWRes2.clear();

      // 2.2. Sort the table based on data type and station
      for (UnsignedInt i = 0; i < measTypesList.size(); ++i)
      {
         // 2.2.1. Get keyword for a statistics record
         std::string keyword = measTypesList[i] + " " + stationsList[i];
         
         // 2.2.2. Search on keyList to find location to store the record
         UnsignedInt j = 0;
         for (; j < keyList.size(); ++j)
         {
            if (keyword < keyList[j])
               break;
         }

         // 2.2.3. Insert statistics records
         if (j == keyList.size())
         {
            keyList.push_back(keyword);
            stList.push_back(stationsList[i]);
            typeList.push_back(measTypesList[i]);
            sumRec.push_back(sumAllRecords[i]);
            sumAccRec.push_back(sumAcceptRecords[i]);
            sumRes.push_back(sumResidual[i]);
            sumRes2.push_back(sumResidualSquare[i]);
            sumWRes2.push_back(sumWeightResidualSquare[i]);

            sumSERec.push_back(sumSERecords[i]);
            sumSERes.push_back(sumSEResidual[i]);
            sumSERes2.push_back(sumSEResidualSquare[i]);
            sumSEWRes2.push_back(sumSEWeightResidualSquare[i]);
         }
         else
         {
            StringArray::iterator pos;
            pos = keyList.begin(); pos += j; keyList.insert(pos, keyword);
            pos = stList.begin(); pos += j; stList.insert(pos, stationsList[i]);
            pos = typeList.begin(); pos += j; typeList.insert(pos, measTypesList[i]);

            IntegerArray::iterator pos1;
            pos1 = sumRec.begin(); pos1 += j; sumRec.insert(pos1, sumAllRecords[i]);
            pos1 = sumAccRec.begin(); pos1 += j; sumAccRec.insert(pos1, sumAcceptRecords[i]);
            pos1 = sumSERec.begin(); pos1 += j; sumSERec.insert(pos1, sumSERecords[i]);

            RealArray::iterator pos2;
            pos2 = sumRes.begin(); pos2 += j; sumRes.insert(pos2, sumResidual[i]);
            pos2 = sumRes2.begin(); pos2 += j; sumRes2.insert(pos2, sumResidualSquare[i]);
            pos2 = sumWRes2.begin(); pos2 += j; sumWRes2.insert(pos2, sumWeightResidualSquare[i]);
            pos2 = sumSERes.begin(); pos2 += j; sumSERes.insert(pos2, sumSEResidual[i]);
            pos2 = sumSERes2.begin(); pos2 += j; sumSERes2.insert(pos2, sumSEResidualSquare[i]);
            pos2 = sumSEWRes2.begin(); pos2 += j; sumSEWRes2.insert(pos2, sumSEWeightResidualSquare[i]);
         }
      }

      // 2.3. Calculate and write statistics table:
      sumRecTotal = 0;
      sumAccRecTotal = 0;
      sumResTotal = 0.0;
      sumRes2Total = 0.0;
      sumWRes2Total = 0.0;

      sumSERecTotal = 0;
      sumSEResTotal = 0.0;
      sumSERes2Total = 0.0;
      sumSEWRes2Total = 0.0;

      std::string unit = "";

      lines.str("");
      std::string typeName = typeList[0];
      for (UnsignedInt i = 0; i < stList.size(); ++i)
      {
         std::string keyword = typeList[i] + " " + stList[i];
         if (typeList[i] == typeName)
         {
            // write a line on statistics table
            gsName1 = "";
            for (Integer j = 0; j < stations.size(); ++j)
            {
               if (stations[j]->GetStringParameter("Id") == stList[i])
               {
                  gsName1 = stations[j]->GetName();
                  break;
               }
            }

            Real average = sumRes[i] / sumAccRec[i];
            Real stdev = GmatMathUtil::Sqrt(sumRes2[i] / sumAccRec[i] - average * average);
            Real wrms = GmatMathUtil::Sqrt(sumWRes2[i] / sumAccRec[i]);
            lines << " "
               << GmatStringUtil::GetAlignmentString("", 19, GmatStringUtil::LEFT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::GetAlignmentString(stList[i], 4) + " " + gsName1, 19) << "  "
               << GmatStringUtil::ToString(sumRec[i], 6) << "     "
               << GmatStringUtil::ToString(sumAccRec[i], 6) << "    "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(sumAccRec[i] * 100.0 / sumRec[i], false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(average, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(stdev, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(wrms, 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               //<< GmatStringUtil::ToString(sumSERec[i] + sumAccRec[i], 6) << "    "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERec[i] + sumAccRec[i]) * 100.0 / sumRec[i], false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERes[i] + sumRes[i]) / (sumSERec[i] + sumAccRec[i]), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSERes2[i] + sumRes2[i]) / (sumSERec[i] + sumAccRec[i])), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSEWRes2[i] + sumWRes2[i]) / (sumSERec[i] + sumAccRec[i])), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               << GmatStringUtil::GetAlignmentString("", 8, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "
               << GmatStringUtil::GetAlignmentString(GetUnit(typeList[i]), 6, GmatStringUtil::LEFT) << "\n";

            // add to total
            sumRecTotal += sumRec[i];
            sumAccRecTotal += sumAccRec[i];
            sumResTotal += sumRes[i];
            sumRes2Total += sumRes2[i];
            sumWRes2Total += sumWRes2[i];

            sumSERecTotal += sumSERec[i];
            sumSEResTotal += sumSERes[i];
            sumSERes2Total += sumSERes2[i];
            sumSEWRes2Total += sumSEWRes2[i];

            unit = GetUnit(typeList[i]);
         }
         else
         {
            // write total for all data type
            Real average = sumResTotal / sumAccRecTotal;
            Real stdev = GmatMathUtil::Sqrt(sumRes2Total / sumAccRecTotal - average * average);
            Real wrms = GmatMathUtil::Sqrt(sumWRes2Total / sumAccRecTotal);

            textFile2 << " "
               << GmatStringUtil::GetAlignmentString(typeName, 19, GmatStringUtil::LEFT) << " "
               << GmatStringUtil::GetAlignmentString("All", 19, GmatStringUtil::LEFT) << "  "
               << GmatStringUtil::ToString(sumRecTotal, 6) << "     "
               << GmatStringUtil::ToString(sumAccRecTotal, 6) << "    "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(sumAccRecTotal * 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(average, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(stdev, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(wrms, 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               //<< GmatStringUtil::ToString(sumSERecTotal + sumAccRecTotal, 6) << "    "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERecTotal + sumAccRecTotal) * 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSEResTotal + sumResTotal) / (sumSERecTotal + sumAccRecTotal), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSERes2Total + sumRes2Total) / (sumSERecTotal + sumAccRecTotal)), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSEWRes2Total + sumWRes2Total) / (sumSERecTotal + sumAccRecTotal)), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               << GmatStringUtil::GetAlignmentString("", 8, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "
               << GmatStringUtil::GetAlignmentString(unit, 6, GmatStringUtil::LEFT) << "\n";
            textFile2 << lines.str();
            textFile2 << "\n";

            // reset total
            sumRecTotal = 0;
            sumAccRecTotal = 0;
            sumResTotal = 0.0;
            sumRes2Total = 0.0;
            sumWRes2Total = 0.0;

            sumSERecTotal = 0;
            sumSEResTotal = 0.0;
            sumSERes2Total = 0.0;
            sumSEWRes2Total = 0.0;

            typeName = typeList[i];
            lines.str("");

            // write a line on statistics table
            gsName1 = "";
            for (Integer j = 0; j < stations.size(); ++j)
            {
               if (stations[j]->GetStringParameter("Id") == stList[i])
               {
                  gsName1 = stations[j]->GetName();
                  break;
               }
            }

            average = sumRes[i] / sumAccRec[i];
            stdev = GmatMathUtil::Sqrt(sumRes2[i] / sumAccRec[i] - average * average);
            wrms = GmatMathUtil::Sqrt(sumWRes2[i] / sumAccRec[i]);
            lines << " "
               << GmatStringUtil::GetAlignmentString("", 19, GmatStringUtil::LEFT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::GetAlignmentString(stList[i], 4) + " " + gsName1, 19) << "  "
               << GmatStringUtil::ToString(sumRec[i], 6) << "     "
               << GmatStringUtil::ToString(sumAccRec[i], 6) << "    "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(sumAccRec[i] * 100.0 / sumRec[i], false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(average, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(stdev, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(wrms, 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               //<< GmatStringUtil::ToString(sumSERec[i] + sumAccRec[i], 6) << "    "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERec[i] + sumAccRec[i]) * 100.0 / sumRec[i], false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERes[i] + sumRes[i]) / (sumSERec[i] + sumAccRec[i]), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSERes2[i] + sumRes2[i]) / (sumSERec[i] + sumAccRec[i])), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSEWRes2[i] + sumWRes2[i]) / (sumSERec[i] + sumAccRec[i])), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               << GmatStringUtil::GetAlignmentString("", 8, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "
               << GmatStringUtil::GetAlignmentString(GetUnit(typeList[i]), 6, GmatStringUtil::LEFT) << "\n";

            // add to total
            sumRecTotal += sumRec[i];
            sumAccRecTotal += sumAccRec[i];
            sumResTotal += sumRes[i];
            sumRes2Total += sumRes2[i];
            sumWRes2Total += sumWRes2[i];

            sumSERecTotal += sumSERec[i];
            sumSEResTotal += sumSERes[i];
            sumSERes2Total += sumSERes2[i];
            sumSEWRes2Total += sumSEWRes2[i];
         }

      }
      // write total for all data type
      Real average = sumResTotal / sumAccRecTotal;
      Real stdev = GmatMathUtil::Sqrt(sumRes2Total / sumAccRecTotal - average * average);
      Real wrms = GmatMathUtil::Sqrt(sumWRes2Total / sumAccRecTotal);

      textFile2 << " "
         << GmatStringUtil::GetAlignmentString(typeName, 19, GmatStringUtil::LEFT) << " "
         << GmatStringUtil::GetAlignmentString("All", 19, GmatStringUtil::LEFT) << "  "
         << GmatStringUtil::ToString(sumRecTotal, 6) << "     "
         << GmatStringUtil::ToString(sumAccRecTotal, 6) << "    "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(sumAccRecTotal * 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(average, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(stdev, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(wrms, 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

         //<< GmatStringUtil::ToString(sumSERecTotal + sumAccRecTotal, 6) << "    "
         //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERecTotal + sumAccRecTotal) * 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
         //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSEResTotal + sumResTotal) / (sumSERecTotal + sumAccRecTotal), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
         //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSERes2Total + sumRes2Total) / (sumSERecTotal + sumAccRecTotal)), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
         //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSEWRes2Total + sumWRes2Total) / (sumSERecTotal + sumAccRecTotal)), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

         << GmatStringUtil::GetAlignmentString("", 8, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "
         << GmatStringUtil::GetAlignmentString(unit, 6, GmatStringUtil::LEFT) << "\n";

      textFile2 << lines.str();
      textFile2 << "\n";


      /// 3. Write observation summary by station
      // 3.1. Write table header
      textFile2 << "\n";
      textFile2 << GmatStringUtil::GetAlignmentString("", 65) + "Observation Summary by Station\n";
      textFile2 << "\n";
      textFile2 << "                                                                                Mean      Standard      Weighted     User          Mean      Standard\n";
      textFile2 << " Pad# Station        Data Type             Total   Accepted    Percent      Residual     Deviation           RMS   Edited      Residual     Deviation  Units\n";
      textFile2 << " --------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

      stList.clear(); typeList.clear(); keyList.clear();
      sumRec.clear(); sumAccRec.clear(); sumSERec.clear();
      sumRes.clear(); sumRes2.clear(); sumWRes2.clear();
      sumSERes.clear(); sumSERes2.clear(); sumSEWRes2.clear();

      // 3.2. Sort the table based on station and data type
      for (UnsignedInt i = 0; i < stationsList.size(); ++i)
      {
         // 3.2.1. Get keyword for a statistics record
         std::string keyword = stationsList[i] + " " + measTypesList[i];

         // 3.2.2. Search on keyList to find location to store the record
         UnsignedInt j = 0;
         for (; j < keyList.size(); ++j)
         {
            if (keyword < keyList[j])
               break;
         }

         // 3.2.3. Insert statistics records
         if (j == keyList.size())
         {
            keyList.push_back(keyword);
            stList.push_back(stationsList[i]);
            typeList.push_back(measTypesList[i]);
            sumRec.push_back(sumAllRecords[i]);
            sumAccRec.push_back(sumAcceptRecords[i]);
            sumRes.push_back(sumResidual[i]);
            sumRes2.push_back(sumResidualSquare[i]);
            sumWRes2.push_back(sumWeightResidualSquare[i]);

            sumSERec.push_back(sumSERecords[i]);
            sumSERes.push_back(sumSEResidual[i]);
            sumSERes2.push_back(sumSEResidualSquare[i]);
            sumSEWRes2.push_back(sumSEWeightResidualSquare[i]);
         }
         else
         {
            StringArray::iterator pos;
            pos = keyList.begin(); pos += j; keyList.insert(pos, keyword);
            pos = stList.begin(); pos += j; stList.insert(pos, stationsList[i]);
            pos = typeList.begin(); pos += j; typeList.insert(pos, measTypesList[i]);

            IntegerArray::iterator pos1;
            pos1 = sumRec.begin(); pos1 += j; sumRec.insert(pos1, sumAllRecords[i]);
            pos1 = sumAccRec.begin(); pos1 += j; sumAccRec.insert(pos1, sumAcceptRecords[i]);
            pos1 = sumSERec.begin(); pos1 += j; sumSERec.insert(pos1, sumSERecords[i]);

            RealArray::iterator pos2;
            pos2 = sumRes.begin(); pos2 += j; sumRes.insert(pos2, sumResidual[i]);
            pos2 = sumRes2.begin(); pos2 += j; sumRes2.insert(pos2, sumResidualSquare[i]);
            pos2 = sumWRes2.begin(); pos2 += j; sumWRes2.insert(pos2, sumWeightResidualSquare[i]);
            pos2 = sumSERes.begin(); pos2 += j; sumSERes.insert(pos2, sumSEResidual[i]);
            pos2 = sumSERes2.begin(); pos2 += j; sumSERes2.insert(pos2, sumSEResidualSquare[i]);
            pos2 = sumSEWRes2.begin(); pos2 += j; sumSEWRes2.insert(pos2, sumSEWeightResidualSquare[i]);
         }
      }

      // 3.3. Calculate and write statistics table:
      sumRecTotal = 0;
      sumAccRecTotal = 0;
      sumResTotal = 0.0;
      sumRes2Total = 0.0;
      sumWRes2Total = 0.0;

      sumSERecTotal = 0;
      sumSEResTotal = 0.0;
      sumSERes2Total = 0.0;
      sumSEWRes2Total = 0.0;

      gsName = stList[0];
      for (UnsignedInt i = 0; i < stList.size(); ++i)
      {
         std::string keyword = stList[i] + " " + typeList[i];
         if (stList[i] == gsName)
         {
            // add to total
            sumRecTotal += sumRec[i];
            sumAccRecTotal += sumAccRec[i];
            sumResTotal += sumRes[i];
            sumRes2Total += sumRes2[i];
            sumWRes2Total += sumWRes2[i];

            sumSERecTotal += sumSERec[i];
            sumSEResTotal += sumSERes[i];
            sumSERes2Total += sumSERes2[i];
            sumSEWRes2Total += sumSEWRes2[i];

            unit = GetUnit(typeList[i]);
         }
         else
         {
            // write total for all data type
            gsName1 = "";
            for (Integer j = 0; j < stations.size(); ++j)
            {
               if (stations[j]->GetStringParameter("Id") == gsName)
               {
                  gsName1 = stations[j]->GetName();
                  break;
               }
            }

            textFile2 << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::GetAlignmentString(gsName, 4) + " " + gsName1, 19) << " "
               << GmatStringUtil::GetAlignmentString("All", 19, GmatStringUtil::LEFT) << "  "
               << GmatStringUtil::ToString(sumRecTotal, 6) << "     "
               << GmatStringUtil::ToString(sumAccRecTotal, 6) << "    "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(sumAccRecTotal * 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt(sumWRes2Total / sumAccRecTotal), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               //<< GmatStringUtil::ToString(sumSERecTotal + sumAccRecTotal, 6) << "    "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERecTotal + sumAccRecTotal)* 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               //<< GmatStringUtil::GetAlignmentString("N/A", 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString("N/A", 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSEWRes2Total + sumWRes2Total) / (sumSERecTotal + sumAccRecTotal)), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               << GmatStringUtil::GetAlignmentString("", 8, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "
               << GmatStringUtil::GetAlignmentString("", 6, GmatStringUtil::LEFT) << "\n";

            // reset total
            sumRecTotal = 0;
            sumAccRecTotal = 0;
            sumResTotal = 0.0;
            sumRes2Total = 0.0;
            sumWRes2Total = 0.0;

            sumSERecTotal = 0;
            sumSEResTotal = 0.0;
            sumSERes2Total = 0.0;
            sumSEWRes2Total = 0.0;

            gsName = stList[i];

            // add to total
            sumRecTotal += sumRec[i];
            sumAccRecTotal += sumAccRec[i];
            sumResTotal += sumRes[i];
            sumRes2Total += sumRes2[i];
            sumWRes2Total += sumWRes2[i];

            sumSERecTotal += sumSERec[i];
            sumSEResTotal += sumSERes[i];
            sumSERes2Total += sumSERes2[i];
            sumSEWRes2Total += sumSEWRes2[i];
         }

      }
      // write total for all data type
      gsName1 = "";
      for (Integer j = 0; j < stations.size(); ++j)
      {
         if (stations[j]->GetStringParameter("Id") == gsName)
         {
            gsName1 = stations[j]->GetName();
            break;
         }
      }

      textFile2 << " "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::GetAlignmentString(gsName, 4) + " " + gsName1, 19) << " "
         << GmatStringUtil::GetAlignmentString("All", 19, GmatStringUtil::LEFT) << "  "
         << GmatStringUtil::ToString(sumRecTotal, 6) << "     "
         << GmatStringUtil::ToString(sumAccRecTotal, 6) << "    "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(sumAccRecTotal * 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
         << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt(sumWRes2Total / sumAccRecTotal), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

         //<< GmatStringUtil::ToString(sumSERecTotal + sumAccRecTotal, 6) << "    "
         //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERecTotal + sumAccRecTotal)* 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
         //<< GmatStringUtil::GetAlignmentString("N/A", 13, GmatStringUtil::RIGHT) << " "
         //<< GmatStringUtil::GetAlignmentString("N/A", 13, GmatStringUtil::RIGHT) << " "
         //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSEWRes2Total + sumWRes2Total) / (sumSERecTotal + sumAccRecTotal)), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

         << GmatStringUtil::GetAlignmentString("", 8, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "
         << GmatStringUtil::GetAlignmentString("", 6, GmatStringUtil::LEFT) << "\n";
      textFile2 << "\n";


      /// 4. Write observation summary by measurement type
      // 4.1. Write table header
      textFile2 << "\n";
      textFile2 << GmatStringUtil::GetAlignmentString("", 64) + "Observation Summary by Data Type\n";
      textFile2 << "\n";
      textFile2 << "                                                                                Mean      Standard      Weighted     User          Mean      Standard\n";
      textFile2 << " Data Type           Station               Total   Accepted    Percent      Residual     Deviation           RMS   Edited      Residual     Deviation  Units\n";
      textFile2 << " --------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

      stList.clear(); typeList.clear(); keyList.clear();
      sumRec.clear(); sumAccRec.clear(); sumSERec.clear();
      sumRes.clear(); sumRes2.clear(); sumWRes2.clear();
      sumSERes.clear(); sumSERes2.clear(); sumSEWRes2.clear();

      // 4.2. Sort the table based on data type and station
      for (UnsignedInt i = 0; i < measTypesList.size(); ++i)
      {
         // 4.2.1. Get keyword for a statistics record
         std::string keyword = measTypesList[i] + " " + stationsList[i];

         // 4.2.2. Search on keyList to find location to store the record
         UnsignedInt j = 0;
         for (; j < keyList.size(); ++j)
         {
            if (keyword < keyList[j])
               break;
         }

         // 4.2.3. Insert statistics records
         if (j == keyList.size())
         {
            keyList.push_back(keyword);
            stList.push_back(stationsList[i]);
            typeList.push_back(measTypesList[i]);
            sumRec.push_back(sumAllRecords[i]);
            sumAccRec.push_back(sumAcceptRecords[i]);
            sumRes.push_back(sumResidual[i]);
            sumRes2.push_back(sumResidualSquare[i]);
            sumWRes2.push_back(sumWeightResidualSquare[i]);

            sumSERec.push_back(sumSERecords[i]);
            sumSERes.push_back(sumSEResidual[i]);
            sumSERes2.push_back(sumSEResidualSquare[i]);
            sumSEWRes2.push_back(sumSEWeightResidualSquare[i]);
         }
         else
         {
            StringArray::iterator pos;
            pos = keyList.begin(); pos += j; keyList.insert(pos, keyword);
            pos = stList.begin(); pos += j; stList.insert(pos, stationsList[i]);
            pos = typeList.begin(); pos += j; typeList.insert(pos, measTypesList[i]);

            IntegerArray::iterator pos1;
            pos1 = sumRec.begin(); pos1 += j; sumRec.insert(pos1, sumAllRecords[i]);
            pos1 = sumAccRec.begin(); pos1 += j; sumAccRec.insert(pos1, sumAcceptRecords[i]);
            pos1 = sumSERec.begin(); pos1 += j; sumSERec.insert(pos1, sumSERecords[i]);

            RealArray::iterator pos2;
            pos2 = sumRes.begin(); pos2 += j; sumRes.insert(pos2, sumResidual[i]);
            pos2 = sumRes2.begin(); pos2 += j; sumRes2.insert(pos2, sumResidualSquare[i]);
            pos2 = sumWRes2.begin(); pos2 += j; sumWRes2.insert(pos2, sumWeightResidualSquare[i]);
            pos2 = sumSERes.begin(); pos2 += j; sumSERes.insert(pos2, sumSEResidual[i]);
            pos2 = sumSERes2.begin(); pos2 += j; sumSERes2.insert(pos2, sumSEResidualSquare[i]);
            pos2 = sumSEWRes2.begin(); pos2 += j; sumSEWRes2.insert(pos2, sumSEWeightResidualSquare[i]);
         }
      }

      // 4.3. Calculate and write statistics table:
      sumRecTotal = 0;
      sumAccRecTotal = 0;
      sumResTotal = 0.0;
      sumRes2Total = 0.0;
      sumWRes2Total = 0.0;

      sumSERecTotal = 0;
      sumSEResTotal = 0.0;
      sumSERes2Total = 0.0;
      sumSEWRes2Total = 0.0;

      typeName = typeList[0];
      for (UnsignedInt i = 0; i < stList.size(); ++i)
      {
         std::string keyword = typeList[i] + " " + stList[i];
         if (typeList[i] == typeName)
         {
            // add to total
            sumRecTotal += sumRec[i];
            sumAccRecTotal += sumAccRec[i];
            sumResTotal += sumRes[i];
            sumRes2Total += sumRes2[i];
            sumWRes2Total += sumWRes2[i];

            sumSERecTotal += sumSERec[i];
            sumSEResTotal += sumSERes[i];
            sumSERes2Total += sumSERes2[i];
            sumSEWRes2Total += sumSEWRes2[i];

            unit = GetUnit(typeList[i]);
         }
         else
         {
            // write total for all data type
            Real average = sumResTotal / sumAccRecTotal;
            Real stdev = GmatMathUtil::Sqrt(sumRes2Total / sumAccRecTotal - average * average);
            Real wrms = GmatMathUtil::Sqrt(sumWRes2Total / sumAccRecTotal);
            textFile2 << " "
               << GmatStringUtil::GetAlignmentString(typeName, 19, GmatStringUtil::LEFT) << " "
               << GmatStringUtil::GetAlignmentString("All", 19, GmatStringUtil::LEFT) << "  "
               << GmatStringUtil::ToString(sumRecTotal, 6) << "     "
               << GmatStringUtil::ToString(sumAccRecTotal, 6) << "    "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(sumAccRecTotal * 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(average, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(stdev, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(wrms, 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               //<< GmatStringUtil::ToString(sumSERec[i] + sumAccRec[i], 6) << "    "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERec[i] + sumAccRec[i]) * 100.0 / sumRec[i], false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERes[i] + sumRes[i]) / (sumSERec[i] + sumAccRec[i]), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSERes2[i] + sumRes2[i]) / (sumSERec[i] + sumAccRec[i])), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
               //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSEWRes2[i] + sumWRes2[i]) / (sumSERec[i] + sumAccRec[i])), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

               << GmatStringUtil::GetAlignmentString("", 8, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
               << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "
               << GmatStringUtil::GetAlignmentString(unit, 6, GmatStringUtil::LEFT) << "\n";

            // reset total
            sumRecTotal = 0;
            sumAccRecTotal = 0;
            sumResTotal = 0.0;
            sumRes2Total = 0.0;
            sumWRes2Total = 0.0;

            sumSERecTotal = 0;
            sumSEResTotal = 0.0;
            sumSERes2Total = 0.0;
            sumSEWRes2Total = 0.0;

            typeName = typeList[i];

            // add to total
            sumRecTotal += sumRec[i];
            sumAccRecTotal += sumAccRec[i];
            sumResTotal += sumRes[i];
            sumRes2Total += sumRes2[i];
            sumWRes2Total += sumWRes2[i];

            sumSERecTotal += sumSERec[i];
            sumSEResTotal += sumSERes[i];
            sumSERes2Total += sumSERes2[i];
            sumSEWRes2Total += sumSEWRes2[i];
         }

      }
      // write total for all data type
      average = sumResTotal / sumAccRecTotal;
      stdev = GmatMathUtil::Sqrt(sumRes2Total / sumAccRecTotal - average * average);
      wrms = GmatMathUtil::Sqrt(sumWRes2Total / sumAccRecTotal);
      textFile2 << " "
         << GmatStringUtil::GetAlignmentString(typeName, 19, GmatStringUtil::LEFT) << " "
         << GmatStringUtil::GetAlignmentString("All", 19, GmatStringUtil::LEFT) << "  "
         << GmatStringUtil::ToString(sumRecTotal, 6) << "     "
         << GmatStringUtil::ToString(sumAccRecTotal, 6) << "    "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(sumAccRecTotal * 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(average, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(stdev, 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(wrms, 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

         //<< GmatStringUtil::ToString(sumSERecTotal + sumAccRecTotal, 6) << "    "
         //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSERecTotal + sumAccRecTotal) * 100.0 / sumRecTotal, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "
         //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString((sumSEResTotal + sumResTotal) / (sumSERecTotal + sumAccRecTotal), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
         //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSERes2Total + sumRes2Total) / (sumSERecTotal + sumAccRecTotal)), 6, true, 13), 13, GmatStringUtil::RIGHT) << " "
         //<< GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(GmatMathUtil::Sqrt((sumSEWRes2Total + sumWRes2Total) / (sumSERecTotal + sumAccRecTotal)), 3, true, 13), 13, GmatStringUtil::RIGHT) << " "

         << GmatStringUtil::GetAlignmentString("", 8, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "
         << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "
         << GmatStringUtil::GetAlignmentString(unit, 6, GmatStringUtil::LEFT) << "\n";
      textFile2 << "\n";

      textFile2.flush();
   }
}


//------------------------------------------------------------------------------------------
// void WriteIterationSummaryPart3(Solver::SolverState sState)
//------------------------------------------------------------------------------------------
/**
* Write State Information section
*/
//------------------------------------------------------------------------------------------
void BatchEstimator::WriteIterationSummaryPart3(Solver::SolverState sState)
{
   const std::vector<ListItem*> *map = esm.GetStateMap();
   GmatState outputEstimationState;

   if (sState == ESTIMATING)
   {
      // 1. Write state summary header
      textFile3 << "\n";
      //textFile3 << "**************************************************************  ITERATION " << GmatStringUtil::ToString(iterationsTaken, 3) << ": STATE INFORMATION  **************************************************************\n";
      textFile3 << "**************************************************************  ITERATION " << GmatStringUtil::ToString(iterationsTaken+1, 3) << ": STATE INFORMATION  **************************************************************\n";
      textFile3 << "\n";


      // 2. Write estimation time
      Real utcMjdEpoch = TimeConverterUtil::Convert(estimationEpoch, TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD);
      bool handleLeapSecond = TimeConverterUtil::HandleLeapSecond();
      std::string utcEpoch = TimeConverterUtil::ConvertMjdToGregorian(utcMjdEpoch, handleLeapSecond);
      textFile3 << " Estimation Epoch : " << utcEpoch << " UTCG\n";
      textFile3 << "\n";


      // 3. Convert state to participants' coordinate system
      GetEstimationStateForReport(outputEstimationState);


      // 4. Specify maximum len of elements' names (Cartisian element names)
      Integer max_len = 27;         // 27 is the maximum lenght of ancillary element names
      for (int i = 0; i < map->size(); ++i)
      {
         std::stringstream ss;
         if (((*map)[i]->object->IsOfType(Gmat::MEASUREMENT_MODEL)) &&
            ((*map)[i]->elementName == "Bias"))
         {
            MeasurementModel* mm = (MeasurementModel*)((*map)[i]->object);
            StringArray sa = mm->GetStringArrayParameter("Participants");
            ss << mm->GetStringParameter("Type") << " ";
            for (UnsignedInt j = 0; j < sa.size(); ++j)
               ss << sa[j] << (((j + 1) != sa.size()) ? "," : " Bias.");
            ss << (*map)[i]->subelement;
         }
         else
            ss << GetElementFullName((*map)[i], false);
         max_len = (Integer)GmatMathUtil::Max(max_len, ss.str().size());
      }

      
      // 5.1. Calculate Keplerian state for apriori, previous, current states:
      std::map<GmatBase*, Rvector6> aprioriKeplerianStateMap = CalculateKeplerianStateMap(map, aprioriSolveForState);
      std::map<GmatBase*, Rvector6> previousKeplerianStateMap = CalculateKeplerianStateMap(map, previousSolveForState);
      std::map<GmatBase*, Rvector6> currentKeplerianStateMap = CalculateKeplerianStateMap(map, currentSolveForState);

      // 5.2. Calculate ancillary elements for apriori, previous, current states:
      std::map<GmatBase*, RealArray> aprioriAEStateMap = CalculateAncillaryElements(map, aprioriSolveForState);
      std::map<GmatBase*, RealArray> previousAEStateMap = CalculateAncillaryElements(map, previousSolveForState);
      std::map<GmatBase*, RealArray> currentAEStateMap = CalculateAncillaryElements(map, currentSolveForState);

      // 5.3. Get Cartesian state for the current state: 
      std::map<GmatBase*, Rvector6> currentCartesianStateMap = CalculateCartesianStateMap(map, currentSolveForState);


      
      // 6. Specify maximum len of elements' names (Keplerian element names)
      Integer len = 0;
      for (std::map<GmatBase*, Rvector6>::iterator i = aprioriKeplerianStateMap.begin(); i != aprioriKeplerianStateMap.end(); ++i)
      {
         Integer csNameSize = ((Spacecraft*)(i->first))->GetRefObject(Gmat::COORDINATE_SYSTEM, "")->GetName().size();
         len = (Integer)GmatMathUtil::Max(len, i->first->GetName().size() + csNameSize + 6);
      }
      max_len = (Integer)GmatMathUtil::Max(max_len, len);


      // 7. Write state information
      textFile3   << " " << GmatStringUtil::GetAlignmentString("State Component", max_len + 4, GmatStringUtil::LEFT)
         << "Units           Current State        Apriori State      Standard Dev.       Previous State    Current-Apriori   Current-Previous\n";
      textFile3 << "\n";
      textFile3.precision(8);

      // covariance matrix w.r.t. Cr_Epsilon and Cd_Epsilon
      Rmatrix covar = information.Inverse();

      // covariance matrix w.r.t. Cr and Cd
      for (UnsignedInt i = 0; i < map->size(); ++i)
      {
         if ((*map)[i]->elementName == "Cr_Epsilon")
         {
            // Get Cr0
            Real Cr0 = (*map)[i]->object->GetRealParameter("Cr") / (1 + (*map)[i]->object->GetRealParameter("Cr_Epsilon"));

            // multiply row and column i with Cr0
            for (UnsignedInt j = 0; j < covar.GetNumColumns(); ++j)
               covar(i, j) *= Cr0;
            for (UnsignedInt j = 0; j < covar.GetNumRows(); ++j)
               covar(j, i) *= Cr0;
         }
         if ((*map)[i]->elementName == "Cd_Epsilon")
         {
            // Get Cd0
            Real Cd0 = (*map)[i]->object->GetRealParameter("Cd") / (1 + (*map)[i]->object->GetRealParameter("Cd_Epsilon"));

            // multiply row and column i with Cd0
            for (UnsignedInt j = 0; j < covar.GetNumColumns(); ++j)
               covar(i, j) *= Cd0;
            for (UnsignedInt j = 0; j < covar.GetNumRows(); ++j)
               covar(j, i) *= Cd0;
         }
      }

      
      for (Integer i = 0; i < map->size(); ++i)
      {
         std::stringstream ss;
         if (((*map)[i]->object->IsOfType(Gmat::MEASUREMENT_MODEL)) &&
            ((*map)[i]->elementName == "Bias"))
         {
            // Get full name for Bias
            MeasurementModel* mm = (MeasurementModel*)((*map)[i]->object);
            StringArray sa = mm->GetStringArrayParameter("Participants");
            ss << mm->GetStringParameter("Type") << " ";
            for (UnsignedInt j = 0; j < sa.size(); ++j)
               ss << sa[j] << (((j + 1) != sa.size()) ? "," : " Bias.");
            ss << (*map)[i]->subelement;

            // Get Bias unit. It is km for Range_KM, RU for DSNRange, km/s for Doppler_RangeRate, and Hz for Doppler_HZ 
         }
         else
         {
            // Get full name for Bias
            ss << GetElementFullName((*map)[i], false);
         }

         std::string unit = GetElementUnit((*map)[i]);
         int precision = GetElementPrecision(unit);

         textFile3 << GmatStringUtil::ToString(i+1, 3);
         textFile3 << " ";
         textFile3 << GmatStringUtil::GetAlignmentString(ss.str(), max_len + 1, GmatStringUtil::LEFT);
         textFile3 << GmatStringUtil::GetAlignmentString(unit, 8, GmatStringUtil::LEFT);
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentSolveForState[i], false, false, true, precision, 20)), 21, GmatStringUtil::RIGHT);                    // current state
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(aprioriSolveForState[i], false, false, true, precision, 20)), 21, GmatStringUtil::RIGHT);                    // apriori state
         if (covar(i, i) >= 0.0)
            textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(GmatMathUtil::Sqrt(covar(i, i)), false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);          // standard deviation
         else
            textFile3 << GmatStringUtil::GetAlignmentString("N/A", 19, GmatStringUtil::RIGHT);
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(previousSolveForState[i], false, false, true, precision, 20)), 21, GmatStringUtil::RIGHT);                   // previous state
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentSolveForState[i] - aprioriSolveForState[i], false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);   // current state - apriori
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentSolveForState[i] - previousSolveForState[i], false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);  // current state - previous state
         textFile3 << "\n";
      }
      textFile3 << "\n";

      // 8. Caluclate Keplerian covariance matrix
      Rmatrix convmatrix;
      bool valid = true;
      try
      {
         convmatrix = CovarianceConvertionMatrix(currentCartesianStateMap);
      }
      catch (...)
      {
         valid = false;
      }


      // 9. Write Keplerian state
      if (valid)
      {
         // 9.1. Calculate Keplerian covariance matrix
         Rmatrix keplerianCovar = convmatrix * covar * convmatrix.Transpose();                 // Equation 8-49 GTDS MathSpec

         // 9.2. Write Keplerian apriori, previous, current states
         std::vector<std::string> nameList;
         std::vector<std::string> unitList;
         RealArray aprioriArr, previousArr, currentArr, stdArr;
         for (std::map<GmatBase*, Rvector6>::iterator i = aprioriKeplerianStateMap.begin(); i != aprioriKeplerianStateMap.end(); ++i)
         {
            std::string csName = ((Spacecraft*)(i->first))->GetRefObject(Gmat::COORDINATE_SYSTEM, "")->GetName();
            nameList.push_back(i->first->GetName() + "." + csName + ".SMA"); unitList.push_back("km");
            nameList.push_back(i->first->GetName() + "." + csName + ".ECC"); unitList.push_back("");
            nameList.push_back(i->first->GetName() + "." + csName + ".INC"); unitList.push_back("deg");
            nameList.push_back(i->first->GetName() + "." + csName + ".RAAN"); unitList.push_back("deg");
            nameList.push_back(i->first->GetName() + "." + csName + ".AOP"); unitList.push_back("deg");
            nameList.push_back(i->first->GetName() + "." + csName + ".MA"); unitList.push_back("deg");
            for (UnsignedInt j = 0; j < 6; ++j)
               aprioriArr.push_back(i->second[j]);
         }

         for (std::map<GmatBase*, Rvector6>::iterator i = previousKeplerianStateMap.begin(); i != previousKeplerianStateMap.end(); ++i)
         {
            for (UnsignedInt j = 0; j < 6; ++j)
               previousArr.push_back(i->second[j]);
         }

         for (std::map<GmatBase*, Rvector6>::iterator i = currentKeplerianStateMap.begin(); i != currentKeplerianStateMap.end(); ++i)
         {
            for (UnsignedInt j = 0; j < 6; ++j)
               currentArr.push_back(i->second[j]);

            UnsignedInt k = 0;
            for (; k < map->size(); ++k)
            {
               if (((*map)[k]->elementName == "CartesianState") && ((*map)[k]->object == i->first))
                  break;
            }

            for (UnsignedInt j = 0; j < 6; ++j)
            {
               if (keplerianCovar(k, k) >= 0.0)
                  stdArr.push_back(GmatMathUtil::Sqrt(keplerianCovar(k, k)));
               else
                  stdArr.push_back(-1.0);
               ++k;
            }
         }

         for (Integer i = 0; i < nameList.size(); ++i)
         {
            int precision = GetElementPrecision(unitList[i]);
            textFile3 << GmatStringUtil::ToString(i+1, 3);
            textFile3 << " ";
            textFile3 << GmatStringUtil::GetAlignmentString(nameList[i], max_len + 1, GmatStringUtil::LEFT);
            textFile3 << GmatStringUtil::GetAlignmentString(unitList[i], 8, GmatStringUtil::LEFT);
            textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentArr[i], false, false, true, precision, 20)), 21, GmatStringUtil::RIGHT);             // current state
            textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(aprioriArr[i], false, false, true, precision, 20)), 21, GmatStringUtil::RIGHT);             // apriori state
            if (stdArr[i] >= 0.0)
               textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(stdArr[i], false, true, true, 8, 18)), 19, GmatStringUtil::RIGHT);                       // standard deviation
            else
               textFile3 << GmatStringUtil::GetAlignmentString("N/A", 19, GmatStringUtil::RIGHT);

            textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(previousArr[i], false, false, true, precision, 20)), 21, GmatStringUtil::RIGHT);            // previous state
            textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentArr[i] - aprioriArr[i], false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);   // current state - apriori 
            textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentArr[i] - previousArr[i], false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);  // current state - previous state
            textFile3 << "\n";
         }
      }

      textFile3 << "\n";

      // 10. Write ancillary elements to the summary:
      StringArray nameList1, units;

      nameList1.push_back("Right Ascension"); units.push_back("deg");
      nameList1.push_back("Declination"); units.push_back("deg");
      nameList1.push_back("Vertical Flight Path Angle"); units.push_back("deg");
      nameList1.push_back("Azimuth Angle"); units.push_back("deg");
      nameList1.push_back("Magnitude of Radius Vector"); units.push_back("km");
      nameList1.push_back("Magnitude of Velocity"); units.push_back("km/s");

      nameList1.push_back("Eccentric Anomaly"); units.push_back("deg");
      nameList1.push_back("True Anomaly"); units.push_back("deg");
      nameList1.push_back("Period"); units.push_back("min");
      nameList1.push_back("Period Dot"); units.push_back("min/day");
      nameList1.push_back("Perifocal Height"); units.push_back("km");
      nameList1.push_back("Perifocal Radius"); units.push_back("km");
      nameList1.push_back("Apofocal Height"); units.push_back("km");
      nameList1.push_back("Apofocal Radius"); units.push_back("km");
      nameList1.push_back("Mean Motion"); units.push_back("deg/day");
      nameList1.push_back("Arg Perigee Dot"); units.push_back("deg/day");
      nameList1.push_back("Ascending Node Dot"); units.push_back("deg/day");
      nameList1.push_back("Velocity at Apogee"); units.push_back("km/s");
      nameList1.push_back("Velocity at Perigee"); units.push_back("km/s");
      nameList1.push_back("Geocentric Latitude"); units.push_back("deg");
      nameList1.push_back("Geodetic Latitude"); units.push_back("deg");
      nameList1.push_back("Longitude"); units.push_back("deg");
      nameList1.push_back("Height"); units.push_back("km");
      nameList1.push_back("C3 Energy"); units.push_back("km2/s2");

      Integer nameLen = 0;
      Integer unitLen = 0;
      for (Integer i = 0; i < nameList1.size(); ++i)
      {
         nameLen = (Integer)GmatMathUtil::Max(nameLen, nameList1[i].size());
         unitLen = (Integer)GmatMathUtil::Max(unitLen, units[i].size());
      }

      textFile3 << " " << GmatStringUtil::GetAlignmentString("Ancillary Elements", max_len + 4, GmatStringUtil::LEFT)
         << "Units           Current State        Apriori State      Standard Dev.       Previous State    Current-Apriori   Current-Previous\n";
      textFile3 << "\n";

      // Specify value of all elements:
      
      // Write each element to report file
      for (std::map<GmatBase*, RealArray>::iterator mapIndex = currentAEStateMap.begin(); mapIndex != currentAEStateMap.end(); ++mapIndex)
      {
         // Get spacecraft and its ancillary elements
         GmatBase* sc = (*mapIndex).first;
         RealArray currentAE = (*mapIndex).second;
         RealArray aprioriAE = aprioriAEStateMap[sc];
         RealArray previousAE = previousAEStateMap[sc];

         // Write ancillary elements information for this spacecraft to report file 
         for (Integer i = 0; i < nameList1.size(); ++i)
         {
            int precision = GetElementPrecision(units[i]);

            textFile3 << GmatStringUtil::GetAlignmentString("", 4, GmatStringUtil::LEFT);
            textFile3 << GmatStringUtil::GetAlignmentString(nameList1[i], max_len + 1, GmatStringUtil::LEFT);
            textFile3 << GmatStringUtil::GetAlignmentString(units[i], 8, GmatStringUtil::LEFT);
            if (currentAE[i] == 0.0)
            {
               //textFile3 << "               ****               ****               ****               ****               ****               ****\n";
               textFile3 << "                                                                                                                  \n";
            }
            else
            {
               textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentAE[i], false, false, true, precision, 20)), 21, GmatStringUtil::RIGHT);             // current state
               textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(aprioriAE[i], false, false, true, precision, 20)), 21, GmatStringUtil::RIGHT);             // apriori state
               //if (stdArr[i] >= 0.0)
               //   textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(stdArr[i], false, true, true, 8, 18)), 19, GmatStringUtil::RIGHT);                       // standard deviation
               //else
               //   textFile3 << GmatStringUtil::GetAlignmentString("N/A", 19, GmatStringUtil::RIGHT);
               textFile3 << GmatStringUtil::GetAlignmentString("", 19, GmatStringUtil::RIGHT);

               textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(previousAE[i], false, false, true, precision, 20)), 21, GmatStringUtil::RIGHT);            // previous state
               textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentAE[i] - aprioriAE[i], false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);   // current state - apriori 
               textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentAE[i] - previousAE[i], false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);  // current state - previous state
               textFile3 << "\n";
            }

         }
         textFile3 << "\n";
      }
      textFile3.flush();
   }
}


//------------------------------------------------------------------------------------------
// void WriteIterationSummaryPart4(Solver::SolverState sState)
//------------------------------------------------------------------------------------------
/**
* Write Covariance/Correlation Matrix section
*/
//------------------------------------------------------------------------------------------
void BatchEstimator::WriteIterationSummaryPart4(Solver::SolverState sState)
{
   const std::vector<ListItem*> *map = esm.GetStateMap();

   /// Write covariance matrix and correlation matrix
   if (sState == ESTIMATING)
   {
      // 1. Write header:
      textFile4 << "\n";
      //textFile4 << "********************************************************  ITERATION " << GmatStringUtil::ToString(iterationsTaken, 3) << ": COVARIANCE/CORRELATION MATRIX  ********************************************************\n";
      textFile4 << "********************************************************  ITERATION " << GmatStringUtil::ToString(iterationsTaken+1, 3) << ": COVARIANCE/CORRELATION MATRIX  ********************************************************\n";
      textFile4 << "\n";

      // 2. Write covariance and correlation matrxies in Cartesian coordiante system:
      Integer indexLen = 1;
      for (; GmatMathUtil::Pow(10, indexLen) < map->size(); ++indexLen);


      // 2.1. Calculate current Cartesian state map:
      std::map<GmatBase*, Rvector6> currentCartesianStateMap = CalculateCartesianStateMap(map, currentSolveForState);


      // 2.2 Get covariance matrix w.r.t. Cr_Epsilon and Cd_Epsilon 
      Rmatrix finalCovariance = information.Inverse();

      // 2.3. Convert covariance matrix for Cr_Epsilon and Cd_Epsilon to covariance matrix for Cr and Cd
      for (UnsignedInt i = 0; i < map->size(); ++i)
      {
         if ((*map)[i]->elementName == "Cr_Epsilon")
         {
            // Get Cr0
            Real Cr0 = (*map)[i]->object->GetRealParameter("Cr") / (1 + (*map)[i]->object->GetRealParameter("Cr_Epsilon"));

            // multiply row and column i with Cr0
            for (UnsignedInt j = 0; j < finalCovariance.GetNumColumns(); ++j)
               finalCovariance(i, j) *= Cr0;
            for (UnsignedInt j = 0; j < finalCovariance.GetNumRows(); ++j)
               finalCovariance(j, i) *= Cr0;
         }
         if ((*map)[i]->elementName == "Cd_Epsilon")
         {
            // Get Cd0
            Real Cd0 = (*map)[i]->object->GetRealParameter("Cd") / (1 + (*map)[i]->object->GetRealParameter("Cd_Epsilon"));

            // multiply row and column i with Cd0
            for (UnsignedInt j = 0; j < finalCovariance.GetNumColumns(); ++j)
               finalCovariance(i, j) *= Cd0;
            for (UnsignedInt j = 0; j < finalCovariance.GetNumRows(); ++j)
               finalCovariance(j, i) *= Cd0;
         }
      }

      // 2.4. Write covariance matrix:
      textFile4 << GmatStringUtil::GetAlignmentString("Covariance Matrix in Cartesian Coordinate System", 160, GmatStringUtil::CENTER) << "\n";
      textFile4 << "\n";
      for (Integer startIndex = 0; startIndex < finalCovariance.GetNumColumns(); startIndex += MAX_COLUMNS)
      {

         textFile4 << "               ";
         for (Integer i = startIndex; i < GmatMathUtil::Min(startIndex + MAX_COLUMNS, finalCovariance.GetNumColumns()); ++i)
         {
            textFile4 << GmatStringUtil::ToString(i + 1, 3);
            if (i < finalCovariance.GetNumColumns() -1)
               textFile4 << "                  ";
         }
         textFile4 << "\n";

         // write all rows from columns startIndex to startIndex+MAX_COLUMNS-1
         for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
         {
            textFile4 << "  " << GmatStringUtil::ToString(i + 1, indexLen) << "  ";
            for (Integer j = startIndex; j < GmatMathUtil::Min(startIndex + MAX_COLUMNS, finalCovariance.GetNumColumns()); ++j)
            {
               textFile4 << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(finalCovariance(i, j), false, true, true, 12, 20), 21, GmatStringUtil::RIGHT);
            }
            textFile4 << "\n";
         }
         textFile4 << "\n";
      }

      // 2.5. Write correlation matrix:
      textFile4 << GmatStringUtil::GetAlignmentString("Correlation Matrix in Cartesian Coordinate System", 160, GmatStringUtil::CENTER) << "\n";
      textFile4 << "\n";
      for (Integer startIndex = 0; startIndex < finalCovariance.GetNumColumns(); startIndex += MAX_COLUMNS)
      {
         textFile4 << "                 ";
         for (Integer i = startIndex; i < GmatMathUtil::Min(startIndex + MAX_COLUMNS, finalCovariance.GetNumColumns()); ++i)
         {
            textFile4 << GmatStringUtil::ToString(i + 1, 3);
            if (i < finalCovariance.GetNumColumns() - 1)
               textFile4 << "                  ";
         }
         textFile4 << "\n";

         for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
         {
            textFile4 << "  " << GmatStringUtil::ToString(i + 1, indexLen) << "  ";
            for (Integer j = startIndex; j < GmatMathUtil::Min(startIndex + MAX_COLUMNS, finalCovariance.GetNumColumns()); ++j)
            {
               char s[100];
               sprintf(&s[0], " %20.12lf\0", finalCovariance(i, j) / sqrt(finalCovariance(i, i)*finalCovariance(j, j)));
               std::string ss(s);
               textFile4 << ss;
            }
            textFile4 << "\n";
         }
         textFile4 << "\n";
      }
      textFile4 << "\n";


      // 3. Calculate Keplerian covariance matrix
      Rmatrix convmatrix;
      bool valid = true;
      try
      {
         convmatrix = CovarianceConvertionMatrix(currentCartesianStateMap);
      }
      catch (...)
      {
         valid = false;
      }

      // 4. Write final covariance and correlation matrix for Keplerian coordinate system:
      if (valid)
      {
         // 4.2. Calculate covariance matrix w.r.t. Cr_Epsilon and Cd_Epsilon
         Rmatrix finalKeplerCovariance = convmatrix * information.Inverse() * convmatrix.Transpose();          // Equation 8-49 GTDS MathSpec

         // 4.3 Convert covariance matrix for Cr_Epsilon and Cd_Epsilon to covariance matrix for Cr and Cd
         for (UnsignedInt i = 0; i < map->size(); ++i)
         {
            if ((*map)[i]->elementName == "Cr_Epsilon")
            {
               // Get Cr0
               Real Cr0 = (*map)[i]->object->GetRealParameter("Cr") / (1 + (*map)[i]->object->GetRealParameter("Cr_Epsilon"));

               // multiply row and column i with Cr0
               for (UnsignedInt j = 0; j < finalKeplerCovariance.GetNumColumns(); ++j)
                  finalKeplerCovariance(i, j) *= Cr0;
               for (UnsignedInt j = 0; j < finalKeplerCovariance.GetNumRows(); ++j)
                  finalKeplerCovariance(j, i) *= Cr0;
            }
            if ((*map)[i]->elementName == "Cd_Epsilon")
            {
               // Get Cd0
               Real Cd0 = (*map)[i]->object->GetRealParameter("Cd") / (1 + (*map)[i]->object->GetRealParameter("Cd_Epsilon"));

               // multiply row and column i with Cd0
               for (UnsignedInt j = 0; j < finalKeplerCovariance.GetNumColumns(); ++j)
                  finalKeplerCovariance(i, j) *= Cd0;
               for (UnsignedInt j = 0; j < finalKeplerCovariance.GetNumRows(); ++j)
                  finalKeplerCovariance(j, i) *= Cd0;
            }
         }


         // 4.4. Write to report file covariance matrix in Keplerian Coordinate System:
         textFile4 << GmatStringUtil::GetAlignmentString("Covariance Matrix in Keplerian Coordinate System", 160, GmatStringUtil::CENTER) << "\n";
         textFile4 << "\n";
         for (Integer startIndex = 0; startIndex < finalCovariance.GetNumColumns(); startIndex += MAX_COLUMNS)
         {
            textFile4 << "               ";
            for (Integer i = startIndex; i < GmatMathUtil::Min(startIndex + MAX_COLUMNS, finalKeplerCovariance.GetNumColumns()); ++i)
            {
               textFile4 << GmatStringUtil::ToString(i + 1, 3);
               if (i < finalCovariance.GetNumColumns() - 1)
                  textFile4 << "                  ";
            }
            textFile4 << "\n";

            for (Integer i = 0; i < finalKeplerCovariance.GetNumRows(); ++i)
            {
               textFile4 << "  " << GmatStringUtil::ToString(i + 1, indexLen) << "  ";
               for (Integer j = startIndex; j < GmatMathUtil::Min(startIndex + MAX_COLUMNS, finalKeplerCovariance.GetNumColumns()); ++j)
               {
                  textFile4 << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(finalKeplerCovariance(i, j), false, true, true, 12, 20), 21, GmatStringUtil::RIGHT);
               }
               textFile4 << "\n";
            }
            textFile4 << "\n";
         }


         // 4.5. Write to report file correlation matrix in Keplerian Coordinate System:
         textFile4 << GmatStringUtil::GetAlignmentString("Correlation Matrix in Keplerian Coordinate System", 160, GmatStringUtil::CENTER) << "\n";
         textFile4 << "\n";
         for (Integer startIndex = 0; startIndex < finalCovariance.GetNumColumns(); startIndex += MAX_COLUMNS)
         {
            textFile4 << "                 ";
            for (Integer i = startIndex; i < GmatMathUtil::Min(startIndex + MAX_COLUMNS, finalKeplerCovariance.GetNumColumns()); ++i)
            {
               textFile4 << GmatStringUtil::ToString(i + 1, 3);
               if (i < finalCovariance.GetNumColumns() - 1)
                  textFile4 << "                  ";
            }
            textFile4 << "\n";

            for (Integer i = 0; i < finalKeplerCovariance.GetNumRows(); ++i)
            {
               textFile4 << "  " << GmatStringUtil::ToString(i + 1, indexLen) << "  ";
               for (Integer j = startIndex; j < GmatMathUtil::Min(startIndex + MAX_COLUMNS, finalKeplerCovariance.GetNumColumns()); ++j)
               {
                  char s[100];
                  sprintf(&s[0], " %20.12lf\0", finalKeplerCovariance(i, j) / sqrt(finalKeplerCovariance(i, i)*finalKeplerCovariance(j, j)));
                  std::string ss(s);
                  textFile4 << ss;
               }
               textFile4 << "\n";
            }
            textFile4 << "\n";
         }
         textFile4 << "\n";
      }
      
      textFile4.flush();
   }
}


//------------------------------------------------------------------------------------------
// void WriteReportFileSummary(Solver::SolverState sState)
//------------------------------------------------------------------------------------------
/**
* Write estimation summary
*/
//------------------------------------------------------------------------------------------
void BatchEstimator::WriteReportFileSummary(Solver::SolverState sState)
{
   WriteIterationSummaryPart1(sState);
   WriteIterationSummaryPart2(sState);
   WriteIterationSummaryPart3(sState);
   WriteIterationSummaryPart4(sState);
}


//------------------------------------------------------------------------------------------
// std::map<GmatBase*, Rvector6> CalculateCartesianStateMap(
//                                  const std::vector<ListItem*> *map, GmatState state)
//------------------------------------------------------------------------------------------
/**
* This function is used to calculate state in Cartesian coordinate system
*/
//------------------------------------------------------------------------------------------
std::map<GmatBase*, Rvector6> BatchEstimator::CalculateCartesianStateMap(const std::vector<ListItem*> *map, GmatState state)
{
   static std::map<GmatBase*, Rvector6> stateMap;
   stateMap.clear();

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if ((*map)[i]->elementName == "CartesianState")
      {
         Rvector6 cState;
         cState.Set(state[i], state[i+1], state[i+2], state[i+3], state[i+4], state[i+5]);
         stateMap[(*map)[i]->object] = cState;
         i = i + 5;
      }
   }
   return stateMap;
}


//------------------------------------------------------------------------------------------
// std::map<GmatBase*, Rvector6> CalculateKeplerianStateMap(
//                                  const std::vector<ListItem*> *map, GmatState state)
//------------------------------------------------------------------------------------------
/**
* This function is used to calculate state in Keplerian coordinate system
*/
//------------------------------------------------------------------------------------------
std::map<GmatBase*, Rvector6> BatchEstimator::CalculateKeplerianStateMap(const std::vector<ListItem*> *map, GmatState state)
{
   static std::map<GmatBase*, Rvector6> stateMap;
   stateMap.clear();

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if ((*map)[i]->elementName == "CartesianState")
      {
         Rvector6 cState,kState;

         cState.Set(state[i], state[i+1], state[i+2], state[i+3], state[i+4], state[i+5]);
         GmatBase* cs = ((Spacecraft*)((*map)[i]->object))->GetRefObject(Gmat::COORDINATE_SYSTEM, "");
         CelestialBody * body = (CelestialBody*)(((CoordinateSystem*)cs)->GetOrigin());
         Real mu = body->GetRealParameter(body->GetParameterID("Mu"));
         kState = StateConversionUtil::CartesianToKeplerian(mu, cState, "MA");

         if ((kState[1] <= 0)||(kState[1] >= 1.0))
            MessageInterface::ShowMessage("Warning: eccentricity (%lf) is out of range (0,1) when converting Cartesian state (%lf, %lf, %lf, %lf, %lf, %lf) to Keplerian state.\n", kState[1], state[i], state[i+1], state[i+2], state[i+3], state[i+4], state[i+5]);

         stateMap[(*map)[i]->object] = kState;
         i = i + 5;
      }
   }
   return stateMap;
}


//------------------------------------------------------------------------------------------
// std::map<GmatBase*, Rvector6> CalculateAncillaryElements(
//                                  const std::vector<ListItem*> *map, GmatState state)
//------------------------------------------------------------------------------------------
/**
* This function is used to calculate ancillary elements
*/
//------------------------------------------------------------------------------------------
std::map<GmatBase*, RealArray> BatchEstimator::CalculateAncillaryElements(const std::vector<ListItem*> *map, GmatState state)
{
   static std::map<GmatBase*, RealArray> stateMap;
   stateMap.clear();
   Real elementValue;

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if ((*map)[i]->elementName == "CartesianState")
      {
         Rvector6 cState;
         RealArray elements;
         // 1. Get spacecraft cartisian state
         cState.Set(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);

         // 2. Calculation
         GmatBase* cs = ((Spacecraft*)((*map)[i]->object))->GetRefObject(Gmat::COORDINATE_SYSTEM, "");
         CelestialBody * body = (CelestialBody*)(((CoordinateSystem*)cs)->GetOrigin());
         Real originMu = body->GetRealParameter(body->GetParameterID("Mu"));
         Real originRadius = body->GetRealParameter(body->GetParameterID("EquatorialRadius"));
         Real originFlattening = body->GetRealParameter(body->GetParameterID("Flattening"));

         Rvector6 sphStateAZFPA = StateConversionUtil::Convert(cState, "Cartesian", "SphericalAZFPA",
            originMu, originFlattening, originRadius);
         Rvector6 sphStateRADEC = StateConversionUtil::Convert(cState, "Cartesian", "SphericalRADEC",
            originMu, originFlattening, originRadius);

         Rvector6 kepState = StateConversionUtil::CartesianToKeplerian(originMu, cState);

         Real ea = 0.0;
         Real ha = 0.0;
         Real ma = 0.0;
         bool isEccentric = false;
         bool isHyperbolic = false;
         if (kepState[1] < (1.0 - GmatOrbitConstants::KEP_ECC_TOL))
         {
            ea = StateConversionUtil::TrueToEccentricAnomaly(kepState[5] * GmatMathConstants::RAD_PER_DEG,
               kepState[1], true) * GmatMathConstants::DEG_PER_RAD;
            isEccentric = true;
         }
         else if (kepState[1] > (1.0 + GmatOrbitConstants::KEP_TOL)) // *** or KEP_ECC_TOL or need new tolerance for this?  1.0e-10
         {
            ha = StateConversionUtil::TrueToHyperbolicAnomaly(kepState[5] * GmatMathConstants::RAD_PER_DEG,
               kepState[1], true) * GmatMathConstants::DEG_PER_RAD;
            isHyperbolic = true;
         }
         ma = StateConversionUtil::TrueToMeanAnomaly(kepState[5] * GmatMathConstants::RAD_PER_DEG,
            kepState[1], !isHyperbolic) * GmatMathConstants::DEG_PER_RAD;


         // 3. Calculate ancillary elements' vector
         elementValue = sphStateAZFPA[1];
         elements.push_back(elementValue);           // Right Ascension                unit: deg

         elementValue = sphStateAZFPA[2];
         elements.push_back(elementValue);           // Declination                    unit: deg

         elementValue = sphStateAZFPA[5];
         elements.push_back(elementValue);           // Vertical Flight Path Angle     unit: deg

         elementValue = sphStateAZFPA[4];
         elements.push_back(elementValue);           // Azimuth Angle                  unit: deg

         elementValue = sphStateAZFPA[0];
         elements.push_back(elementValue);           // Magnitude of Radius Vector     unit: km

         elementValue = sphStateAZFPA[3];
         elements.push_back(elementValue);           // Magnitude of Velocity          unit: km/s

         elementValue = ea;
         elements.push_back(elementValue);           // Eccentric Anomaly              unit: deg

         elementValue = kepState[5];
         elements.push_back(elementValue);           // True Anomaly                   unit: deg
         
         elementValue = GmatCalcUtil::CalculateKeplerianData("OrbitPeriod", cState, originMu)/GmatTimeConstants::SECS_PER_MINUTE;
         elements.push_back(elementValue);           // Period                         unit: min
         
         elements.push_back(0.0);                    // Period Dot                     unit: min/day
         elements.push_back(0.0);                    // Perifocal Height               unit: km
         elements.push_back(0.0);                    // Perifocal Radius               unit: km
         elements.push_back(0.0);                    // Apofocal Height                unit: km
         elements.push_back(0.0);                    // Apofocal Radius                unit: km

         elementValue = GmatCalcUtil::CalculateKeplerianData("MeanMotion", cState, originMu)*GmatTimeConstants::SECS_PER_DAY;
         elements.push_back(elementValue);           // Mean Motion                    unit: deg/day

         elements.push_back(0.0);                    // Arg Perigee Dot                unit: deg/day
         elements.push_back(0.0);                    // Ascending Node Dot             unit: deg/day
         elements.push_back(0.0);                    // Velocity at Apogee             unit: km/s
         elements.push_back(0.0);                    // Velocity at Perigee            unit: km/s
         
         elementValue = GmatCalcUtil::CalculatePlanetData("Latitude", cState, originRadius, 0.0, 0.0);    // set flattenning = 0
         elements.push_back(elementValue);           // Geocentric Latitude            unit: deg

         elementValue = GmatCalcUtil::CalculatePlanetData("Latitude", cState, originRadius, originFlattening, 0.0);
         elements.push_back(elementValue);           // Geodetic Latitude              unit: deg

         elementValue = GmatCalcUtil::CalculatePlanetData("Longitude", cState, originRadius, originFlattening, 0.0);
         elements.push_back(elementValue);           // Longitude                      unit: deg
         
         elementValue = GmatCalcUtil::CalculatePlanetData("Altitude", cState, originRadius, originFlattening, 0.0);
         elements.push_back(elementValue);           // Height                         unit: km

         elementValue = GmatCalcUtil::CalculateKeplerianData("C3Energy", cState, originMu);
         elements.push_back(elementValue);           // C3 Energy                      unit: km2/s2

         // 4. Set value to state map
         stateMap[(*map)[i]->object] = elements;

         // 5. Skip to the next spacecraft
         i = i + 5;
      }
   }
   return stateMap;
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
* return          6x6 derivative state conversion matrix
*/
//--------------------------------------------------------------------------------------------
Rmatrix66 BatchEstimator::CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state)
{
   // 1. Get mu value 
   Spacecraft* spacecraft = (Spacecraft*)obj;
   CoordinateSystem* cs = (CoordinateSystem*)(spacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
   CelestialBody* body = (CelestialBody*)(cs->GetOrigin());
   Real mu = body->GetRealParameter(body->GetParameterID("Mu"));
   
   // 2. Specify conversion matrix
   Rmatrix66 convMatrix = StateConversionUtil::CartesianToKeplerianDerivativeConversion(mu, state);
   
   return convMatrix;
}


//--------------------------------------------------------------------------------------------
// Rmatrix BatchEstimator::CovarianceConvertionMatrix(std::map<GmatBase*, Rvector6> stateMap)
//--------------------------------------------------------------------------------------------
/**
* This function is use to calculate derivative state conversion matrix for all solve-for variables. 
* It converts from Cartesian to Keplerian (with mean anomaly) coordiate system.
*
* @param stateMap a map of spacecrafts and their Cartisian state
*
* return          derivative state conversion matrix
*/
//--------------------------------------------------------------------------------------------
Rmatrix BatchEstimator::CovarianceConvertionMatrix(std::map<GmatBase*, Rvector6> stateMap)
{
   const std::vector<ListItem*> *map = esm.GetStateMap();

   // 1. Specify conversion sub matrixes for all spacecrafts
   std::map<GmatBase*, Rmatrix66> matrixMap;
   for (std::map<GmatBase*, Rvector6>::iterator i = stateMap.begin(); i != stateMap.end(); ++i)
      matrixMap[i->first] = CartesianToKeplerianCoverianceConvertionMatrix(i->first, i->second);

   // 2. Assemly whole conversion matrix
   Rmatrix conversionMatrix(map->size(),map->size());        // It is a zero matrix
   for(UnsignedInt i = 0; i < map->size(); )
   {
      if ((*map)[i]->elementName == "CartesianState")
      {
         // fill in conversion submatrix
         Rmatrix66 m = matrixMap[(*map)[i]->object];
         for(UnsignedInt row = 0; row < 6; ++row)
         {
            for(UnsignedInt col = 0; col < 6; ++col)
            {
               conversionMatrix.SetElement(i+row, i+col, m(row,col));
            }
         }
         // skip to next
         i = i + 6;
      }
      else
      {
         conversionMatrix(i,i) = 1.0;
         ++i;
      }
   }
   
   return conversionMatrix;
}


//-------------------------------------------------------------------------
// void DataFilter()
//-------------------------------------------------------------------------
/**
* This function is used to filter bad observation data records. It has
*   1. Data filter based on OLSEInitialRMSSigma
*   2. Data filter based on outer-loop sigma editting
*/
//-------------------------------------------------------------------------
bool BatchEstimator::DataFilter()
{
   const ObservationData *currentObs =  measManager.GetObsData();                        // Get observation measurement data O
   const MeasurementData *calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);      // Get calculated measurement data C

   bool retVal = false;
   if (iterationsTaken == 0)
   {
      for (Integer i=0; i < currentObs->value.size(); ++i)
     {
         // 1. Data filtered based on OLSEInitialRMSSigma
         // 1.1. Specify Weight
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
         
         // 1.2. Filter based on maximum residual multiplier
#ifdef DEBUG_DATA_FILTER
         MessageInterface::ShowMessage("Epoch = %.12lf A1Mjd   O = %.6lf   C = %.6lf   w = %le    sqrt(w)*Abs(O-C) = %.6lf   maxResidualMult = %lf\n", calculatedMeas->epoch, currentObs->value[i], calculatedMeas->value[i], weight, sqrt(weight)*GmatMathUtil::Abs(currentObs->value[i] - calculatedMeas->value[i]), maxResidualMult);
#endif
         if (sqrt(weight)*GmatMathUtil::Abs(currentObs->value[i] - calculatedMeas->value[i]) > maxResidualMult)   // if (Wii*GmatMathUtil::Abs(O-C) > maximum residual multiplier) then throw away this data record
         {
            measManager.GetObsDataObject()->inUsed = false;
            measManager.GetObsDataObject()->removedReason = "IRMS";            // "IRMS": represent for OLSEInitialRMSSigma
            std::string filterName = "IRMS";
#ifdef DEBUG_DATA_FILTER
            MessageInterface::ShowMessage("This record is fillted.\n");
#endif
            retVal = true;
            break;
         }
      }
   }
   else
   {
     for (Integer i=0; i < currentObs->value.size(); ++i)
     {
         // 2. Data filtered based on outer-loop sigma editting
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
         
         // 2.2. Filter based on n-sigma
         Real sigmaVal = (chooseRMSP ? predictedRMS : newResidualRMS);
         if (sqrt(weight)*GmatMathUtil::Abs(currentObs->value[i] - calculatedMeas->value[i]) > (constMult*sigmaVal + additiveConst))   // if (Wii*GmatMathUtil::Abs(O-C) > k*sigma+ K) then throw away this data record
         {
            measManager.GetObsDataObject()->inUsed = false;
            measManager.GetObsDataObject()->removedReason = "OLSE";                     // "OLSE": represent for outer-loop sigma filter

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
               sum1[j1-1] = -delta[j-1] * sum1[nn-1];          // Calculate [H12];   GTDS MatSpec  Eq 8-162b
               j1 += rowCount - j;
            }

            // Compute X
            i1 = n;
            for (i=1; i <= nMinus1; ++i)
            {
               j1 = i;
               for (j=1; j <= i; ++j)
               {
                  sum1[j1-1] -= (sum1[i1-1] * delta[j-1]);       // Calculate [H22];   GTDS MatSpec Eq
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
         throw EstimatorException("Schur inversion cannot proceed; the size of "
               "the array being inverted is zero");

      if (sum1[0] == 0.0)
         throw EstimatorException("Schur inversion cannot proceed; the upper "
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
      tolerance = GmatMathUtil::Abs(epsilon * sum1[j-1]);
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


//------------------------------------------------------------------------------
// bool WriteMatData()
//------------------------------------------------------------------------------
/**
 * Method used to write the MATLAB .mat file
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool BatchEstimator::WriteMatData()
{
   bool retval = true;

   // Set the top level label
   std::stringstream name;
   name << "Iteration" << iterationsTaken;

   StringArray dataDesc;

   // Package the data lists
   std::vector<WriterData*> containers;

   // matData.elementStatus
   dataDesc.push_back("Status");
   WriterData* writerData = matWriter->GetContainer(Gmat::REAL_TYPE, "Status");
   std::vector<RealArray> statData;
   statData.push_back(matData.elementStatus);
   writerData->AddData(statData);
   containers.push_back(writerData);

   // The Real data containers
   for (UnsignedInt i = 0; i < matData.realNames.size(); ++i)
   {
      dataDesc.push_back(matData.realNames[i]);
      writerData = matWriter->GetContainer(Gmat::REAL_TYPE, matData.realNames[i]);
      std::vector<RealArray> vecData;
      vecData.push_back(matData.realValues[i]);
      writerData->AddData(vecData);
      containers.push_back(writerData);
   }

   for (UnsignedInt i = 0; i < matData.stringNames.size(); ++i)
   {
      dataDesc.push_back(matData.stringNames[i]);
      writerData = matWriter->GetContainer(Gmat::STRING_TYPE, matData.stringNames[i]);
      std::vector<StringArray> strData;

      #ifdef DEBUG_MAT_WRITER
         MessageInterface::ShowMessage("%s has %d strings\n",
               matData.stringNames[i].c_str(), matData.stringValues[i].size());
         MessageInterface::ShowMessage("   %d:  %s\n", 0,
               matData.stringValues[i][0].c_str());
      #endif

      strData.push_back(matData.stringValues[i]);

      #ifdef DEBUG_MAT_WRITER
         MessageInterface::ShowMessage("   vecData has %d entries\n",
               vecData.size());
         MessageInterface::ShowMessage("   vecData[0] has %d entries\n",
               vecData[0].size());
         MessageInterface::ShowMessage("      %d:  %s\n", 0,
               vecData[0][0].c_str());
      #endif

      writerData->AddData(strData);
      containers.push_back(writerData);
   }

   // Write it
   matWriter->DescribeData(dataDesc);
   for (UnsignedInt i = 0; i < containers.size(); ++i)
      matWriter->AddData(containers[i]);

   matWriter->WriteData(name.str());

   // Clean up for the next pass
   matData.Clear();

   return retval;
}
