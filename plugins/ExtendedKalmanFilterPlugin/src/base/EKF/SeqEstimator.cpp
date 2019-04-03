//$Id: SeqEstimator.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         SeqEstimator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
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
// Created: 2009/09/03
//
/**
 * Base class for sequential estimation
 */
//------------------------------------------------------------------------------

#include "SeqEstimator.hpp"

#include "GmatConstants.hpp"
#include <sstream>
#include "MessageInterface.hpp"
#include "EstimatorException.hpp"
#include "Solver.hpp"

// EKF mod 12/16
#include "GroundstationInterface.hpp"
#include "Spacecraft.hpp" 
#include "SpaceObject.hpp"
#include "StringUtil.hpp"
#include "Rmatrix66.hpp"
#include "StateConversionUtil.hpp"

#include <algorithm>
#include <sstream>
#include <iostream>
#include <string>
// End EKF mod

//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION
//#define DEBUG_REPORTS

// Macros for debugging of the state machine
//#define WALK_STATE_MACHINE
//#define DUMP_RESIDUALS
//#define DEBUG_EVENT
//#define DEBUG_RUNCOMPLETE

#define DEFAULT_POSITION_COVARIANCE 100.0
#define DEFAULT_VELOCITY_COVARIANCE 0.0001
#define DEFAULT_OTHER_COVARIANCE    1.0

//------------------------------------------------------------------------------
// static data - EKF mod 12/16
//------------------------------------------------------------------------------

const std::string MeasNoiseType::Hardware("Hardware");
const std::string MeasNoiseType::Filter("Filter");

const std::string ProcessNoiseType::None("None");
const std::string ProcessNoiseType::Constant("Constant");
const std::string ProcessNoiseType::BasicTime("BasicTime");
const std::string ProcessNoiseType::SingerModel("SingerModel");
const std::string ProcessNoiseType::SNC("SNC");

const UnsignedInt SeqEstimator::truthStateSize =  6;
const UnsignedInt SeqEstimator::stdColLen      = 25;
const UnsignedInt SeqEstimator::minPartSize    = 18;
const UnsignedInt SeqEstimator::strSizeMin     =  4;
const double SeqEstimator::defaultMeasSigma    =  1e-4;



const std::string
SeqEstimator::PARAMETER_TEXT[] =
{
   "MeasNoiseType",                 // The measurement noise type
   "MeasNoiseSigma",                // The range measurement noise 1-sigma value
   "ProcessNoiseType",              // The process noise type
   "ProcessNoiseConstantVector",    // The constant process noise vector - used to apply process noise to diagonals of error covariance
   "ProcessPosNoiseTimeRate",       // For BasicTime, the position noise time rate / sec
   "ProcessVelNoiseTimeRate",       // For BasicTime, the velocity noise time rate / sec
   "ProcessSingerTimeConst",        // For Singer Model, the maneuver correlation time constant (sec)
   "ProcessSingerSigma",            // For Singer Model, the sigma value
};

const Gmat::ParameterType
SeqEstimator::PARAMETER_TYPE[] =
{
   Gmat::ENUMERATION_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::ENUMERATION_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
};
// End EKF mod

//------------------------------------------------------------------------------
// SeqEstimator(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The subtype of the derived class
 * @param name The name of the instance
 */
//------------------------------------------------------------------------------
SeqEstimator::SeqEstimator(const std::string &type,
      const std::string &name) :
   Estimator               (type, name),
   measCovariance          (NULL),
// EKF mod 12/16
   measSize                (0),
   measNoiseType           (MeasNoiseType::Hardware),
   measNoiseSigma          (1),
   processNoiseType        (ProcessNoiseType::None),
   processNoiseConstantVector(1),
   processPosNoiseTimeRate (8.33e-7), // ~ 3 meters / hour
   processVelNoiseTimeRate (9.00e-9), // ~ 9 um / sec^2 (micrometers)
   processSingerTimeConst  (0.0),
   processSingerSigma      (0.0)
// End EKF mod
{
   hiLowData.push_back(&sigma);
   showErrorBars = true;
}


//------------------------------------------------------------------------------
// ~SeqEstimator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SeqEstimator::~SeqEstimator()
{
}


//------------------------------------------------------------------------------
// SeqEstimator::SeqEstimator(const SeqEstimator & se) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param se The estimator that provides configuration information for this one
 */
//------------------------------------------------------------------------------
SeqEstimator::SeqEstimator(const SeqEstimator & se) :
   Estimator               (se),
   measCovariance          (NULL),
// EKF mod 12/16
   measSize                (se.measSize),
   measNoiseType           (se.measNoiseType),
   processNoiseType        (se.processNoiseType),
   processPosNoiseTimeRate (se.processPosNoiseTimeRate),
   processVelNoiseTimeRate (se.processVelNoiseTimeRate),
   processSingerTimeConst  (se.processSingerTimeConst),
   processSingerSigma      (se.processSingerSigma)
// End EKF mod
{
   hiLowData.push_back(&sigma);

// EKF mod 12/16
   measNoiseSigma.SetSize(se.measNoiseSigma.GetSize());
   measNoiseSigma = se.measNoiseSigma;

   processNoiseConstantVector.SetSize(se.processNoiseConstantVector.GetSize());
   processNoiseConstantVector = se.processNoiseConstantVector;
// End EKF mod
}


//------------------------------------------------------------------------------
// SeqEstimator& operator=(const SeqEstimator &se)
//------------------------------------------------------------------------------
/**
 * Assignemtn operator
 *
 * @param se The estimator that provides configuration information for this one
 *
 * @return This estimator, configured to match se
 */
//------------------------------------------------------------------------------
SeqEstimator& SeqEstimator::operator=(
      const SeqEstimator &se)
{
   if (this != &se)
   {
      Estimator::operator=(se);
      measCovariance = NULL;
   }
   return *this;
}

// EKF Mod 12/16
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
std::string SeqEstimator::GetParameterText(const Integer id) const
{
   if (id >= EstimatorParamCount && id < SeqEstimatorParamCount)
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
std::string SeqEstimator::GetParameterUnit(const Integer id) const
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
Integer SeqEstimator::GetParameterID(const std::string &str) const
{
   for (Integer i = EstimatorParamCount; i < SeqEstimatorParamCount; i++)
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
Gmat::ParameterType SeqEstimator::GetParameterType(const Integer id) const
{
   if (id >= EstimatorParamCount && id < SeqEstimatorParamCount)
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
std::string SeqEstimator::GetParameterTypeString(const Integer id) const
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
Integer SeqEstimator::GetIntegerParameter(const Integer id) const
{
//   if (id == MAX_CONSECUTIVE_DIVERGENCES)
//      return maxConsDivergences;

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
Integer SeqEstimator::SetIntegerParameter(const Integer id, const Integer value)
{
//   if (id == MAX_CONSECUTIVE_DIVERGENCES)
//   {
//      if (value < 1)
//      {
//         std::stringstream ss;
//         ss << "Error: " << GetName() << ".MaxConsecutiveDivergences has invalid value (" << value << "). It has to be a positive integer greater than 0.\n";
//         throw EstimatorException(ss.str());
//         return value;
//      }
//
//      maxConsDivergences = value;
//      return value;
//   }

   return Estimator::SetIntegerParameter(id, value);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//---------------------------------------------------------------------------
Real SeqEstimator::GetRealParameter(const Integer id) const
{
   if (id == PROCESS_POS_NOISE_TIME_RATE)
   {
      return processPosNoiseTimeRate;
   }

   if (id == PROCESS_VEL_NOISE_TIME_RATE)
   {
      return processVelNoiseTimeRate;
   }

   if (id == PROCESS_SINGER_TIME_CONST)
   {
      return processSingerTimeConst;
   }

   if (id == PROCESS_SINGER_SIGMA)
   {
      return processSingerSigma;
   }

   return Estimator::GetRealParameter(id);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id,
//                        const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve the Real value for a Rvector parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <index> The integer index for the Rvector parameter
 *
 * @return The parameter's value.
 */
//---------------------------------------------------------------------------
Real SeqEstimator::GetRealParameter(const Integer id,
                                           const Integer index) const
{
   if (id == MEAS_NOISE_SIGMA)
   {
      return measNoiseSigma(index);
   }

   if (id == PROCESS_NOISE_CONSTANT_VECTOR)
   {
      return processNoiseConstantVector(index);
   }

   return GmatBase::GetRealParameter(id, index);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <label> The label of the parameter.
 *
 * @return The parameter's value.
 */
//---------------------------------------------------------------------------
Real SeqEstimator::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label,
//                       const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve the Real value for a Rvector parameter.
 *
 * @param <label> The label of the parameter.
 * @param <index> The index of the Rvector parameter
 *
 * @return The parameter's value.
 */
//---------------------------------------------------------------------------
Real SeqEstimator::GetRealParameter(const std::string &label,
                                           const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//---------------------------------------------------------------------------
Real SeqEstimator::SetRealParameter(const Integer id,
                                           const Real value)
{
   if (id == PROCESS_POS_NOISE_TIME_RATE)
   {
      if (value <= std::numeric_limits<double>::epsilon())
      {
         throw SolverException("ProcessPosNoiseTimeRate must be greater than 0");
      }
      processPosNoiseTimeRate = value;
      return processPosNoiseTimeRate;
   }

   if (id == PROCESS_VEL_NOISE_TIME_RATE)
   {
      if (value <= std::numeric_limits<double>::epsilon())
      {
         throw SolverException("ProcessVelNoiseTimeRate must be greater than 0");
      }
      processVelNoiseTimeRate = value;
      return processVelNoiseTimeRate;
   }

   if (id == PROCESS_SINGER_TIME_CONST)
   {
      if (value <= std::numeric_limits<double>::epsilon())
      {
         throw SolverException("ProcessSingerTimeConst must be greater than 0");
      }
      processSingerTimeConst = value;
      return processSingerTimeConst;
   }

   if (id == PROCESS_SINGER_SIGMA)
   {
      if (value <= std::numeric_limits<double>::epsilon())
      {
         throw SolverException("ProcessSingerSigma must be greater than 0");
      }
      processSingerSigma = value;
      return processSingerSigma;
   }

   return Estimator::SetRealParameter(id, value);
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value,
//                        const Integer index)
//---------------------------------------------------------------------------
/**
 * Set the Real value for a Rvector parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 * @param <index> The index of the Rvector parameter
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//---------------------------------------------------------------------------
Real SeqEstimator::SetRealParameter(const Integer id,
                                           const Real value,
                                           const Integer index)
{
   if (id == MEAS_NOISE_SIGMA)
   {
      // resize vector if necessary
      if (index >= measNoiseSigma.GetSize())
      {
         measNoiseSigma.Resize(index+1);
      }
      measNoiseSigma.SetElement(index, value);
      return value;
   }

   if (id == PROCESS_NOISE_CONSTANT_VECTOR)
   {
      // resize vector if necessary
      if (index >= processNoiseConstantVector.GetSize())
      {
         processNoiseConstantVector.Resize(index+1);
      }
      processNoiseConstantVector.SetElement(index, value);
      return value;
   }

   return GmatBase::SetRealParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param <label> The label of the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real SeqEstimator::SetRealParameter(const std::string &label,
                                           const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value,
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * Set the Real value for a Rvector parameter.
 *
 * @param <label> The label of the parameter.
 * @param <value> The new parameter value.
 * @param <index> The index of the Rvector parameter
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real SeqEstimator::SetRealParameter(const std::string &label,
                                           const Real value,
                                           const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const Integer Id) const;
//------------------------------------------------------------------------------
/**
 * Get the value for a Rvector parameter
 *
 * @param <Id> The integer ID of the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
const Rvector& SeqEstimator::GetRvectorParameter(const Integer Id) const
{
   if (Id == MEAS_NOISE_SIGMA)
   {
      return measNoiseSigma;
   }

   if (Id == PROCESS_NOISE_CONSTANT_VECTOR)
   {
      return processNoiseConstantVector;
   }

   return GmatBase::GetRvectorParameter(Id);
}

//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const std::string &label) const;
//------------------------------------------------------------------------------
/**
 * Get the value for a Rvector parameter
 *
 * @param <label> The label of the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
const Rvector& SeqEstimator::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// const Rvector& SetRvectorParameter(const Integer Id,
//                                    const Rvector& value) const;
//------------------------------------------------------------------------------
/**
 * Set the value for a Rvector parameter
 *
 * @param <Id> The integer ID of the parameter.
 * @param <value> The value to set
 *
 * @return The parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
const Rvector& SeqEstimator::SetRvectorParameter(const Integer Id,
                                                        const Rvector& value)
{
   if (Id == MEAS_NOISE_SIGMA)
   {
      measNoiseSigma = value;
      return measNoiseSigma;
   }

   if (Id == PROCESS_NOISE_CONSTANT_VECTOR)
   {
      processNoiseConstantVector = value;
      return processNoiseConstantVector;
   }

   return GmatBase::SetRvectorParameter(Id, value);
}

//------------------------------------------------------------------------------
// const Rvector& SetRvectorParameter(const std::string &label,
//                                    const Rvector& value) const;
//------------------------------------------------------------------------------
/**
 * Set the value for a Rvector parameter
 *
 * @param <label> The label of the parameter.
 * @param <value> The value to set.
 *
 * @return The parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
const Rvector& SeqEstimator::SetRvectorParameter(const std::string &label,
                                                        const Rvector& value)
{
   return SetRvectorParameter(GetParameterID(label), value);
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
std::string SeqEstimator::GetStringParameter(const Integer id) const
{
   if (id == MEAS_NOISE_TYPE)
   {
      return measNoiseType;
   }

   if (id == PROCESS_NOISE_TYPE)
   {
      return processNoiseType;
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
bool SeqEstimator::SetStringParameter(const Integer id,
         const std::string &value)
{
   if (id == MEAS_NOISE_TYPE)
   {
      if (value != MeasNoiseType::Hardware && value != MeasNoiseType::Filter)
      {
         throw SolverException("Unknown measurement noise type: " +
               value);
      }
      measNoiseType = value;
      return true;
   }

   if (id == PROCESS_NOISE_TYPE)
   {
      if (value != ProcessNoiseType::None &&
            value != ProcessNoiseType::Constant &&
            value != ProcessNoiseType::BasicTime &&
            value != ProcessNoiseType::SingerModel &&
            value != ProcessNoiseType::SNC)
      {
         throw SolverException("Unknown process noise type: " +
               value);
      }
      processNoiseType = value;
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
std::string SeqEstimator::GetStringParameter(const Integer id,
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
bool SeqEstimator::SetStringParameter(const Integer id,
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
std::string SeqEstimator::GetStringParameter(const std::string &label) const
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
bool SeqEstimator::SetStringParameter(const std::string &label,
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
std::string SeqEstimator::GetStringParameter(const std::string &label,
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
bool SeqEstimator::SetStringParameter(const std::string &label,
                                        const std::string &value,
                                        const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// std::string SeqEstimator::GetOnOffParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method gets "On" or "Off" value
 *
 * @param id   The id number of a parameter
 *
 * @return "On" or "Off" value
 */
//------------------------------------------------------------------------------
std::string SeqEstimator::GetOnOffParameter(const Integer id) const
{
//   if (id == USE_PRIORI_ESTIMATE)
//      return (useApriori ? "On" : "Off");

   return Estimator::GetOnOffParameter(id);
}


//------------------------------------------------------------------------------
// bool SeqEstimator::SetOnOffParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method gets "On" or "Off" value
 *
 * @param id      The id number of a parameter
 * @param value      value "On" or "Off"
 *
 * @return true value when it successfully sets the value, false otherwise.
 */
//------------------------------------------------------------------------------
bool SeqEstimator::SetOnOffParameter(const Integer id, const std::string &value)
{
//   if (id == USE_PRIORI_ESTIMATE)
//   {
//      if (value == "On")
//      {
//         useApriori = true;
//         return true;
//      }
//      if (value == "Off")
//      {
//         useApriori = false;
//        return true;
//      }
//
//      return false;
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
const StringArray& SeqEstimator::GetPropertyEnumStrings(const Integer id) const
{
   return Estimator::GetPropertyEnumStrings(id);

}
// End EKF mod

//------------------------------------------------------------------------------
// Solver::SolverState AdvanceState()
//------------------------------------------------------------------------------
/**
 * Implements the basic sequential estimation state machine
 *
 * This method changes the SeqEstimator's finite state based on the
 * status of the elements involved in the estimation process by calling the
 * state machine to process one step in its implementation
 *
 * @return The state at the end of the state machine step.
 */
//------------------------------------------------------------------------------
Solver::SolverState SeqEstimator::AdvanceState()
{
   switch (currentState)
   {
      case INITIALIZING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the INITIALIZING state\n");
         #endif
         CompleteInitialization();
         break;

      case PROPAGATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the PROPAGATING state\n");
         #endif
         FindTimeStep();
         break;

      case CALCULATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the CALCULATING state\n");
         #endif
         CalculateData();
         break;

      case LOCATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the LOCATING state\n");
         #endif
         ProcessEvent();
         break;

      case ESTIMATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the ESTIMATING state\n");
         #endif
         Estimate();
         break;

      case CHECKINGRUN:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the CHECKINGRUN state\n");
         #endif
         CheckCompletion();
         break;

      case FINISHED:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the FINISHED state\n");
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
bool SeqEstimator::TakeAction(const std::string & action,
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
bool SeqEstimator::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("SeqEstimator::Initialize() "
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
bool SeqEstimator::Finalize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("SeqEstimator::Finalize() "
            "entered\n");
   #endif
   bool retval = false;

   if (Estimator::Finalize())
   {
      retval = true;
      measManager.Finalize();  // EKF mod 12/16
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SeqEstimator::Finalize() measManager.Finalize() finished\n");
#endif
      esm.MapVectorToObjects();  // EKF mod 12/16
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SeqEstimator::Finalize() esm.MapVectorToObjects() finished\n");
#endif
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SeqEstimator::Finalize() "
         "finished\n");
#endif
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
void SeqEstimator::CompleteInitialization()
{
// EKF mod 12/16
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("SeqEstimator state is CompleteInitialization\n");
   #endif

   ObjectArray satArray;
   esm.GetStateObjects(satArray, Gmat::SPACECRAFT);
// End EKF mod

   estimationState              = esm.GetState();
   stateSize = estimationState->GetSize();
// EKF mod 12/16
   measSize = measManager.GetObsData()->value.size();

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("stateSize = %d ", stateSize  );
   MessageInterface::ShowMessage("measSize = %d ", measSize  );
   MessageInterface::ShowMessage("GmatState got from propagator: size = %d   epoch = %.12lf   [", estimationState->GetSize(), estimationState->GetEpoch());
   for (UnsignedInt k=0; k < stateSize; ++k)
      MessageInterface::ShowMessage("%.12lf,  ", (*estimationState)[k]);
   MessageInterface::ShowMessage("]\n");
#endif
// End EKF mod

   Estimator::CompleteInitialization();

// EKF mod 12/16
   // Set the current epoch based on the first spacecraft in the ESM
   if(satArray.size() == 0)
   {
      throw EstimatorException("Cannot initialize the estimator: there are "
                               "no Spacecraft in the estimation state manager");
   }
   if (measNoiseType == MeasNoiseType::Filter && measNoiseSigma.GetSize() != measSize)
   {
      // this would be so much faster & better with C++ 11 and std::to_string...
      std::stringstream sstm;
      sstm << "measNoiseSigma.GetSize() != measSize; ";
      sstm << "measNoiseSigma.GetSize() = ";
      sstm << measNoiseSigma.GetSize();
      sstm << ", measSize = ";
      sstm << measSize;

      throw EstimatorException(sstm.str());
   }
   if (processNoiseType == ProcessNoiseType::Constant && processNoiseConstantVector.GetSize() != stateSize)
   {
      // this would be so much faster & better with C++ 11 and std::to_string...
      std::stringstream sstm;
      sstm << "processNoiseConstantVector.GetSize() != stateSize; ";
      sstm << "processNoiseConstantVector.GetSize() = ";
      sstm << processNoiseConstantVector.GetSize();
      sstm << ", stateSize = ";
      sstm << stateSize;

      throw EstimatorException(sstm.str());
   }
   currentEpoch         = ((Spacecraft*)satArray[0])->GetEpoch();

   ObjectArray objects;                                                                     
   esm.GetStateObjects(objects);                                                            
   std::vector<TrackingDataAdapter*> adapters = measManager.GetAllTrackingDataAdapters();   
   for (UnsignedInt i = 0; i < adapters.size(); ++i)
   {
      adapters[i]->SetUsedForObjects(objects);
   }
// End EKF mod

   // Now load up the observations
   measManager.PrepareForProcessing(false);
//   measManager.PrepareForProcessing(false); // added false, EKF mod 12/16

///// Make more generic?
   measManager.LoadRampTables();

// EKF mod 12/16
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("GmatState got from propagator: size = %d [", stateSize);
   for (UnsignedInt zz=0; zz < stateSize; ++zz)
   MessageInterface::ShowMessage("%lf,  ", (*estimationState)[zz]);
   MessageInterface::ShowMessage("]\n");

   MessageInterface::ShowMessage("currentEpoch = %.15lf  ", currentEpoch);
#endif
// End EKF mod

   nextMeasurementEpoch = measManager.GetEpoch();

   hAccum.clear();
   residuals.SetSize(stateSize);
   x0bar.SetSize(stateSize);
   dx.SetSize(stateSize);

   esm.MapObjectsToVector();
   GetEstimationStateForReport(aprioriSolveForState);  // EKF mod 12/16

   measurementResiduals.clear();
   isInitialized = true;

// EKF mod 12/16
   // Get list of signal paths and specify the length of participants' column
   pcolumnLen = 12;
   std::vector<StringArray> signalPaths = measManager.GetSignalPathList();
   for(UnsignedInt i = 0; i < signalPaths.size(); ++i)
   {
      Integer len = 0;
      for (UnsignedInt j = 0; j < signalPaths[i].size(); ++j)
      {
         try
         {
            GmatBase* obj = GetConfiguredObject(signalPaths[i].at(j));
            std::string id = "";
            if (obj->IsOfType(Gmat::SPACECRAFT))
               id = ((Spacecraft*)obj)->GetStringParameter("Id");
            else if (obj->IsOfType(Gmat::GROUND_STATION))
               id = ((GroundstationInterface*)obj)->GetStringParameter("Id");

            len = len + id.size() + 1;
         }
         catch (...)
         {
            // swallow the exception
         }
      }
      if (pcolumnLen < len)
      {
         pcolumnLen = len;
      }
   }
   pcolumnLen += 3;

   WriteToTextFile();
// End EKF mod
   ReportProgress();

   if (GmatMathUtil::IsEqual(currentEpoch, nextMeasurementEpoch))
   {
      currentState = CALCULATING;
   }
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
      for (UnsignedInt i = 0; i < modelNames.size(); ++i)
      {
         plotMeasurements.clear();
         plotMeasurements.push_back(modelNames[i]);
         std::string plotName = instanceName + "_" + modelNames[i] +
               "_Residuals";
         BuildResidualPlot(plotName, plotMeasurements);
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
            "CompleteInitialization complete!\n   STM = %s\n   Covariance = %s\n",
            stm->ToString().c_str(), covariance.GetCovariance()->ToString().c_str());
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
void SeqEstimator::FindTimeStep()
{
// if (GmatMathUtil::IsEqual(currentEpoch, nextMeasurementEpoch)) EKF mod 12/16 changed to next line
   if (fabs((currentEpoch - nextMeasurementEpoch)/currentEpoch) < GmatRealConstants::REAL_EPSILON)
   {
      // We're at the next measurement, so process it
      currentState = CALCULATING;
      timeStep = 0.0; // EKF mod 12/16 we are at the next measurement... so do not advance it further
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
         MessageInterface::ShowMessage("SeqEstimator::FindTimeStep   timestep = %.12lf; nextepoch = "
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
void SeqEstimator::CalculateData()
{
   // Update the STM
   esm.MapObjectsToSTM();
   esm.MapObjectsToVector();

   // Is this correct? EKF mod 12/16
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
      {
         currentState = CHECKINGRUN;
      }
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
void SeqEstimator::ProcessEvent()
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
void SeqEstimator::CheckCompletion()
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
void SeqEstimator::RunComplete()
{
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::Entering ReportProgress()\n");
   #endif
   ReportProgress();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::Completed ReportProgress()\n");
   #endif
   measManager.ProcessingComplete();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::completed measManager.ProcessingComplete()\n");
   #endif
   esm.MapVectorToObjects();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::completed esm.MapVectorToObjects()\n");
   #endif
   // Clean up memory
   for (UnsignedInt i = 0; i < hTilde.size(); ++i)
   {
      hTilde[i].clear();
   }
   hTilde.clear();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::cleared hTilde\n");
   #endif

   for (UnsignedInt i = 0; i < hAccum.size(); ++i)
   {
      hAccum[i].clear();
   }
   hAccum.clear();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::cleared hAccum\n");
   #endif
   Weight.clear();
   OData.clear();
   CData.clear();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::cleared Weight, OData, CData\n");
   #endif
   measurementResiduals.clear();
   measurementEpochs.clear();
   measurementResidualID.clear();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::cleared measurementResiduals\n");
      MessageInterface::ShowMessage("SeqEstimator::entering WriteToTextFile()\n");
   #endif
   WriteToTextFile();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::completed RunComplete()\n");
   #endif
}


//------------------------------------------------------------------------------
// void PrepareForStep()
//------------------------------------------------------------------------------
/**
 * This method performs actions common to sequential estimators prior to a
 * propagation step.  That includes resetting the STM to the identity.
 */
//------------------------------------------------------------------------------
void SeqEstimator::PrepareForStep()
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
void SeqEstimator::WriteToTextFile(Solver::SolverState sState)
{
   // Only write to report file when ReportStyle is Normal or Verbose
//   if ((textFileMode != "Normal")&&(textFileMode != "Verbose"))
//      return;

// EKF mod 12/16 big chunk
   GmatState outputEstimationState;

   if (!showProgress)
   {
      return;
   }

   if (!textFile.is_open())
   {
      OpenSolverTextFile();
   }

   Solver::SolverState theState = sState;
   if (sState == Solver::UNDEFINED_STATE)
   {
      theState = currentState;
   }

   const std::vector<ListItem*> *map = esm.GetStateMap();

   textFile.setf(std::ios::fixed, std::ios::floatfield);

   switch (theState)
   {
      case INITIALIZING:
         WriteScript();
         WriteHeader();
         break;
      case ESTIMATING:
         textFile << linesBuff;
         textFile.flush();
         break;
      case FINISHED:
         WriteSummary(theState);
         WriteConclusion();
         break;

      default:
         break;
   }
}

#include "Moderator.hpp"
void SeqEstimator::WriteScript()
{
#ifdef DEBUG_REPORTS
   MessageInterface::ShowMessage("WriteScript() started\n");
#endif
   textFile << "********************************************************\n";
   textFile << "***  GMAT Script\n";
   textFile << "********************************************************\n";
   std::string filename = Moderator::Instance()->GetScriptInterpreter()->GetMainScriptFileName();
   std::ifstream inFile;
   inFile.open(filename.c_str(),std::ios_base::out);

   char s[1000];
   while (!inFile.eof())
   {
      inFile.getline(s, 1000);
      std::string st(s);
      textFile << st << "\n";
   }
   textFile << "*** End of GMAT Script *********************************\n\n\n";
#ifdef DEBUG_REPORTS
   MessageInterface::ShowMessage("WriteScript() completed\n");
#endif

}

void SeqEstimator::WriteHeader()
{
#ifdef DEBUG_REPORTS
   MessageInterface::ShowMessage("WriteHeader() started\n");
#endif
   Real taiMjdEpoch, utcMjdEpoch;
   std::string utcEpoch;

   GmatState outputEstimationState;
   const std::vector<ListItem*> *map = esm.GetStateMap();

   const ObservationData *currentObs =  measManager.GetObsData();
   measSize = currentObs->value.size();

   /// 1. Write the estimation header
   textFile << "\n"
            << "********************************************************\n"
            << "*** Estimator Header \n"
            << "********************************************************\n\n";

   /// 2. Write state at beginning of the estimation:
   textFile << "State at Beginning of Estimation:\n";
   textFile.precision(15);
   //char s[100];
   textFile << "              Epoch:\n"
            << "   " << currentEpoch <<  " A.1 Mod. Julian\n";
   taiMjdEpoch = TimeConverterUtil::Convert(currentEpoch,
         TimeConverterUtil::A1MJD, TimeConverterUtil::TAIMJD);
   utcMjdEpoch = TimeConverterUtil::Convert(currentEpoch,
         TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD);
   textFile << "   " << taiMjdEpoch << " TAI Mod. Julian\n";
   utcEpoch = TimeConverterUtil::ConvertMjdToGregorian(utcMjdEpoch);
   textFile << "   " << utcEpoch << " UTCG\n";

   // Convert state to participants' coordinate system:
   GetEstimationStateForReport(outputEstimationState);
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
            << "   " << GetName() << ".OLSEInitialRMSSigma        = ";
   if ((maxResidualMult == 0.0)||((GmatMathUtil::Abs(maxResidualMult) < 1.0e6)&&(GmatMathUtil::Abs(maxResidualMult) > 1.0e-2)))
      textFile << maxResidualMult << "\n";
   else
      textFile << GmatStringUtil::ToString(maxResidualMult, false, true) << "\n";

   textFile << "   " << GetName() << ".OLSEMultiplicativeConstant = ";
   if ((constMult == 0.0)||((GmatMathUtil::Abs(constMult) < 1.0e6)&&(GmatMathUtil::Abs(constMult) > 1.0e-2)))
      textFile << constMult << "\n";
   else
      textFile << GmatStringUtil::ToString(constMult, false, true) << "\n";

   textFile << "   " << GetName() << ".OLSEAdditiveConstant       = ";
   if ((additiveConst == 0.0)||((GmatMathUtil::Abs(additiveConst) < 1.0e6)&&(GmatMathUtil::Abs(additiveConst) > 1.0e-2)))
      textFile << additiveConst << "\n";
   else
      textFile << GmatStringUtil::ToString(additiveConst, false, true) << "\n";

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
      textFile << "RecNum   UTCGregorian-Epoch       "
               << "Obs Type           Units  "
               << GmatStringUtil::GetAlignmentString("Participants         ",
                    GmatMathUtil::Max(pcolumnLen, minPartSize))
               << "Edit                     "
               << "Obs (o)        Obs-Correction(O)                  "
               << "Cal (C)       Residual (O-C)            "
               << "Elevation-Angle   \n";
   else
   {
      textFile << "RecNum     UTCGregorian-Epoch        "
               << "TAIModJulian-Epoch        "
               << "Obs Type           Units  "
               << GmatStringUtil::GetAlignmentString("Participants         ",
                     GmatMathUtil::Max(pcolumnLen, minPartSize))
               << "Edit                     "
               << "Obs (o)        Obs-Correction(O)                  "
               << "Cal (C)       Residual (O-C)    "
               << "Elevation-Angle            ";
      const std::vector<ListItem*> *map = esm.GetStateMap();
      for (UnsignedInt i = 0; i < map->size(); ++i)
      {
         std::stringstream tmpss1;
         std::stringstream tmpss;
         tmpss1 << (*map)[i]->elementName;
         tmpss << GmatStringUtil::GetAlignmentString(tmpss1.str(), strSizeMin);
         tmpss << "_"
               << (*map)[i]->subelement;
         textFile << GmatStringUtil::GetAlignmentString(tmpss.str(), stdColLen);
      }
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
         {
            std::stringstream tmpss;
            tmpss << "ErrCov_P_" << i+1 << "_" << j+1;
            textFile << GmatStringUtil::GetAlignmentString(tmpss.str(), stdColLen);
         }
      }
      //std::printf("\n\nmeasSize = %d\n\n", measSize);
      //exit(-5);
      for (UnsignedInt i = 0; i < measSize; ++i)
      {
         for (UnsignedInt j = 0; j < measSize; ++j)
         {
            std::stringstream tmpss;
            tmpss << "InnCov_S_" << i+1 << "_" << j+1;
            textFile << GmatStringUtil::GetAlignmentString(tmpss.str(), stdColLen);
         }
      }
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < measSize; ++j)
         {
            std::stringstream tmpss;
            tmpss << "KalmanGain_K_" << i+1 << "_" << j+1;
            textFile << GmatStringUtil::GetAlignmentString(tmpss.str(), stdColLen);
         }
      }
      textFile << GmatStringUtil::GetAlignmentString("Uplink-Band ",stdColLen)
               << GmatStringUtil::GetAlignmentString("Uplink-Frequency ", stdColLen)
               << GmatStringUtil::GetAlignmentString("Range-Modulo ", stdColLen)
               << "Doppler-Interval\n";
   }
   textFile.flush();
#ifdef DEBUG_REPORTS
   MessageInterface::ShowMessage("WriteHeader() completed\n");
#endif
}

void SeqEstimator::WriteSummary(Solver::SolverState sState)
{
#ifdef DEBUG_REPORTS
   MessageInterface::ShowMessage("WriteSummary() started\n");
#endif
   const std::vector<ListItem*> *map = esm.GetStateMap();
   GmatState outputEstimationState;

   if (currentSolveForState.GetSize()== 0)
	   currentSolveForState.SetSize(6);

   if (sState == ESTIMATING)
   {
      /// 1. Write state summary
      // Convert state to participants' coordinate system:
      GetEstimationStateForReport(outputEstimationState);
      // Write state to report file
      Integer max_len = 15;
      for (UnsignedInt i = 0; i < map->size(); ++i)
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
         {
            ss << GetElementFullName((*map)[i], false);
         }
         max_len = GmatMathUtil::Max(max_len, ss.str().length());
      }

      // Calculate Keplerian state for apriori, previous, current states:
      std::map<GmatBase*, Rvector6> aprioriKeplerianStateMap = CalculateKeplerianStateMap(map, aprioriSolveForState);
      std::map<GmatBase*, Rvector6> previousKeplerianStateMap = CalculateKeplerianStateMap(map, previousSolveForState);
      std::map<GmatBase*, Rvector6> currentKeplerianStateMap = CalculateKeplerianStateMap(map, currentSolveForState);

      std::map<GmatBase*, Rvector6> currentCartesianStateMap = CalculateCartesianStateMap(map, currentSolveForState);

      // Write state information
      textFile << "\n";
      textFile << "Iteration " << iterationsTaken << ": State Information \n"
               << "   " << GmatStringUtil::GetAlignmentString("State Component", max_len, GmatStringUtil::LEFT)
               << "               Apriori State              Previous State               Current State             Current-Apriori            Current-Previous          Standard Deviation\n";

      textFile.precision(8);

      // covariance matrix w.r.t. Cr_Epsilon and Cd_Epsilon
      Rmatrix covar = information.Inverse();

      // covariance matrix w.r.t. Cr and Cd
      for (UnsignedInt i = 0; i < map->size(); ++i)
      {
         if ((*map)[i]->elementName == "Cr_Epsilon")
         {
            // Get Cr0
            Real Cr0 = (*map)[i]->object->GetRealParameter("Cr") /
                  (1 + (*map)[i]->object->GetRealParameter("Cr_Epsilon"));

            // multiply row and column i with Cr0
            for(int j = 0; j < covar.GetNumColumns(); ++j)
               covar(i,j) *= Cr0;
            for(int j = 0; j < covar.GetNumRows(); ++j)
               covar(j,i) *= Cr0;
         }
         if ((*map)[i]->elementName == "Cd_Epsilon")
         {
            // Get Cd0
            Real Cd0 = (*map)[i]->object->GetRealParameter("Cd") /
                  (1 + (*map)[i]->object->GetRealParameter("Cd_Epsilon"));

            // multiply row and column i with Cd0
            for(int j = 0; j < covar.GetNumColumns(); ++j)
               covar(i,j) *= Cd0;
            for(int j = 0; j < covar.GetNumRows(); ++j)
               covar(j,i) *= Cd0;
         }
      }

      for (UnsignedInt i = 0; i < map->size(); ++i)
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
         textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::ToString(aprioriSolveForState[i], false, false, true, 12, 24)), 25, GmatStringUtil::RIGHT) << "   "             // Apriori state
                  << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::ToString(previousSolveForState[i], false, false, true, 12, 24)), 25, GmatStringUtil::RIGHT) << "   "            // initial state
                  << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::ToString(currentSolveForState[i], false, false, true, 12, 24)), 25, GmatStringUtil::RIGHT) << "   "            // updated state
                  << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::ToString(currentSolveForState[i] - aprioriSolveForState[i], false, true, true, 12, 24)), 25, GmatStringUtil::RIGHT)  << "   "   // Apriori - Current state
                  << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::ToString(currentSolveForState[i] - previousSolveForState[i], false, true, true, 12, 24)), 25, GmatStringUtil::RIGHT) << "   ";
         if (covar(i,i) >= 0.0)
            textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::ToString(GmatMathUtil::Sqrt(covar(i,i)), false, true, true, 12, 24)), 25, GmatStringUtil::RIGHT) << "\n";   // standard deviation
         else
            textFile << "       N/A\n";
      }
      textFile << "\n";

      // Caluclate Keplerian covariance matrix
      Rmatrix convmatrix;
      bool valid = true;
      try
      {
         convmatrix = CovarianceConvertionMatrix(currentCartesianStateMap);
      }
      catch(...)
      {
         valid = false;
      }

      if (valid)
      {
         Rmatrix keplerianCovar = convmatrix * covar * convmatrix.Transpose();                 // Equation 8-49 GTDS MathSpec

         // Display Keplerian apriori, previous, current states
         std::vector<std::string> nameList;
         RealArray aprioriArr, previousArr, currentArr, stdArr;
         for (std::map<GmatBase*,Rvector6>::iterator i = aprioriKeplerianStateMap.begin(); i != aprioriKeplerianStateMap.end(); ++i)
         {
            std::string csName = ((Spacecraft*)(i->first))->GetRefObject(Gmat::COORDINATE_SYSTEM,"")->GetName();
            nameList.push_back(i->first->GetName() + "." + csName + ".SMA");
            nameList.push_back(i->first->GetName() + "." + csName + ".ECC");
            nameList.push_back(i->first->GetName() + "." + csName + ".INC");
            nameList.push_back(i->first->GetName() + "." + csName + ".RAAN");
            nameList.push_back(i->first->GetName() + "." + csName + ".AOP");
            nameList.push_back(i->first->GetName() + "." + csName + ".MA");
            for (UnsignedInt j = 0; j < 6; ++j)
               aprioriArr.push_back(i->second[j]);
         }

         for (std::map<GmatBase*,Rvector6>::iterator i = previousKeplerianStateMap.begin(); i != previousKeplerianStateMap.end(); ++i)
         {
            for (UnsignedInt j = 0; j < 6; ++j)
               previousArr.push_back(i->second[j]);
         }

         for (std::map<GmatBase*,Rvector6>::iterator i = currentKeplerianStateMap.begin(); i != currentKeplerianStateMap.end(); ++i)
         {
            for (UnsignedInt j = 0; j < 6; ++j)
               currentArr.push_back(i->second[j]);

            UnsignedInt k = 0;
            for(; k < map->size(); ++k)
            {
               if (((*map)[k]->elementName == "CartesianState")&&((*map)[k]->object == i->first))
                  break;
            }

            for(UnsignedInt j = 0; j < 6; ++j)
            {
               if (keplerianCovar(k,k) >= 0.0)
                  stdArr.push_back(GmatMathUtil::Sqrt(keplerianCovar(k,k)));
               else
                  stdArr.push_back(-1.0);
               ++k;
            }
         }

         for(UnsignedInt i = 0; i < nameList.size(); ++i)
         {
            textFile << "   ";
            textFile << GmatStringUtil::GetAlignmentString(nameList[i], max_len + 3, GmatStringUtil::LEFT);
            textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::ToString(aprioriArr[i], false, false, true, 12, 24)), 25, GmatStringUtil::RIGHT) << "   "             // Apriori state
                     << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::ToString(previousArr[i], false, false, true, 12, 24)), 25, GmatStringUtil::RIGHT) << "   "            // initial state
                     << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::ToString(currentArr[i], false, false, true, 12, 24)), 25, GmatStringUtil::RIGHT) << "   "            // updated state
                     << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::ToString(currentArr[i] - aprioriArr[i], false, true, true, 12, 24)), 25, GmatStringUtil::RIGHT)  << "   "   // Apriori - Current state
                     << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::ToString(currentArr[i] - previousArr[i], false, true, true, 12, 24)), 25, GmatStringUtil::RIGHT) << "   ";
            if (stdArr[i] >= 0.0)
               textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::ToString(stdArr[i], false, true, true, 12, 24)), 25, GmatStringUtil::RIGHT) << "\n";   // standard deviation
            else
               textFile << "        N/A\n";
         }
      }

      /// 2. Write statistics
      /// 2.1. Write statistics summary
      textFile << "\n\n";
      textFile << "Iteration " << iterationsTaken << ":  Statistics \n"
               << "   Total Number Of Records     : " << GetMeasurementManager()->GetObservationDataList()->size() << "\n"
               << "   Records Used For Estimation : " << measurementResiduals.size() << "\n"
               << "   Records Removed Due To      : \n"
               << "      No Computed Value Configuration Available : " << numRemovedRecords["U"] << "\n"
               << "      Out of Ramped Table Range                 : " << numRemovedRecords["R"] << "\n"
               << "      Signal Blocked                            : " << numRemovedRecords["B"] << "\n"
               << "      Sigma Editing                             : " << ((iterationsTaken == 0)?numRemovedRecords["IRMS"]:numRemovedRecords["OLSE"]) << "\n\n";

      // TODO SeqEstimator stats table??
      /// 2.2. Write statistics table:
//      if (statisticsTable["TOTAL NUM RECORDS"].size() > 0)
//      {
//         // Only write out statistics table when at least 1 measurement type is used for estimation
//         // 2.2.1. Write statistics table for ground station and measuremnet type
//         std::vector<Real> numberAllRec;                 // store number of all records for each groundstation and measurement type
//         std::vector<Real> numberAcceptedRec;            // store number of accepted records for each groundstation and measurement type
//         std::vector<Real> residual;                     // store residual for each groundstation and measurement type
//         std::vector<Real> allRecSubTotal;               // store number of all records for each ground station
//         std::vector<Real> acceptedRecSubTotal;          // store number of accepted records for each groundstation
//         std::vector<Real> residualSubTotal;             // store residual for each groundstation
//
//         ObjectMap objMap = GetConfiguredObjectMap();
//         ObjectArray groundStations;
//         for (std::map<std::string, GmatBase*>::iterator element = objMap.begin(); element != objMap.end(); ++element)
//         {
//            if (element->second->IsOfType(Gmat::GROUND_STATION))
//               groundStations.push_back(element->second);
//         }
//
//         StringArray sa;
//         sa.push_back("TOTAL NUM RECORDS");
//         sa.push_back("ACCEPTED RECORDS");
//         sa.push_back("WEIGHTED RMS");
//         sa.push_back("MEAN RESIDUAL");
//         sa.push_back("STANDARD DEVIATION");
//
//         Real sumWeightedResSquare = 0.0;                // sum weighted residual quare
//         Real allNumRec = 0.0;                           // number of records
//         Real numRec = 0.0;                              // number of accepted records
//         Real sumRes = 0.0;                              // sum of residual
//         Real sumResSquare = 0.0;                        // sum of residual square
//
//         for (UnsignedInt i = 0; i < sa.size(); ++i)
//         {
//            std::string gsName;
//
//            if (i == 0)
//            {
//               // Step 1: write table header:
//               textFile << "Observation Summary by Station\n";
//               std::stringstream ss1, ss2, ss3;
//               gsName = "";
//               ss1 << "                    ";
//               ss2 << "                    ";
//               ss3 << "--------------------";
//               for (std::map<std::string, Real>::iterator column = statisticsTable[sa[i]].begin(); column != statisticsTable[sa[i]].end(); ++column)
//               {
//                  std::string name = column->first;
//                  std::string gs = name.substr(0, name.find_first_of(' '));          // ground station ID
//                  std::string typeName = name.substr(name.find_first_of(' ')+1);     // measurment type
//
//                  // Get groundstation' name
//                  std::string strName = "";
//                  for(UnsignedInt j = 0; j < groundStations.size(); ++j)
//                  {
//                     if (groundStations[j]->GetStringParameter("Id") == gs)
//                     {
//                        strName = groundStations[j]->GetName();
//                        break;
//                     }
//                  }
//
//                  if (gs != gsName)
//                  {
//                     ss1 << GmatStringUtil::GetAlignmentString(strName + "  " + gs, 20, GmatStringUtil::RIGHT);
//                     ss2 << GmatStringUtil::GetAlignmentString("All", 20, GmatStringUtil::RIGHT);
//                     ss3 << "--------------------";
//                     gsName = gs;
//                  }
//                  ss1 << GmatStringUtil::GetAlignmentString(strName + "  " + gs, 20, GmatStringUtil::RIGHT);
//                  ss2 << GmatStringUtil::GetAlignmentString(typeName, 20, GmatStringUtil::RIGHT);
//                  ss3 << "--------------------";
//               }
//               textFile << ss1.str() << "\n";
//               textFile << ss2.str() << "\n";
//               textFile << ss3.str() << "\n";
//            }
//
//            // Step 2: Write table contents:
//            gsName = statisticsTable[sa[i]].begin()->first;
//            gsName = gsName.substr(0, gsName.find_first_of(' '));
//            std::stringstream ss4;
////            MessageInterface::ShowMessage("Line %d: ", i);
//            UnsignedInt index = 0;
//            UnsignedInt index1 = 0;
//            for (std::map<std::string, Real>::iterator column = statisticsTable[sa[i]].begin(); column != statisticsTable[sa[i]].end(); ++column)
//            {
//               std::string name = column->first;
//               std::string gs = name.substr(0, name.find_first_of(' '));
//               std::string typeName = name.substr(name.find_first_of(' ')+1);
////               MessageInterface::ShowMessage("<%s %s> = %f ", gs.c_str(), typeName.c_str(), column->second);
//
//               if (column == statisticsTable[sa[i]].begin())
//                  textFile << GmatStringUtil::GetAlignmentString(sa[i],20);
//
//               if (gs != gsName)
//               {
//                  switch (i)
//                  {
//                  case 0:
//                     {
//                        allRecSubTotal.push_back(allNumRec);
//                        Integer value = allNumRec;                           // change type from real to integer
//                        textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(value), 20, GmatStringUtil::RIGHT);
//                     }
//                     break;
//                  case 1:
//                     {
//                        acceptedRecSubTotal.push_back(numRec);
//                        Integer value = numRec;                              // change type from real to integer
//                        Real percent = numRec*100/allRecSubTotal[index1];    // calculate percentage
//                        std::string sval = GmatStringUtil::ToString(percent, 2) + "% " + GmatStringUtil::ToString(value, 8);
//                        textFile << GmatStringUtil::GetAlignmentString(sval, 20, GmatStringUtil::RIGHT);
//                     }
//                     break;
//                  case 2:
//                     textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(GmatMathUtil::Sqrt(sumWeightedResSquare/acceptedRecSubTotal[index1]), 8), 20, GmatStringUtil::RIGHT);
//                     break;
//                  case 3:
//                     {
//                        residualSubTotal.push_back(sumRes);
//                        //textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(sumRes/acceptedRecSubTotal[index1], 8), 20, GmatStringUtil::RIGHT);
//                        textFile << "                 N/A";
//                     }
//                     break;
//                  case 4:
//                     {
//                        Real res_aver = residualSubTotal[index1]/acceptedRecSubTotal[index1];
//                        Real resSquare_aver = sumResSquare/acceptedRecSubTotal[index1];
//                        Real value = GmatMathUtil::Sqrt(resSquare_aver - res_aver*res_aver);
//                        //textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(value, 8), 20, GmatStringUtil::RIGHT);
//                        textFile << "                 N/A";
//                     }
//                     break;
//                  }
//                  ++index1;
//
//                  textFile << ss4.str();
//                  ss4.str("");
//                  allNumRec = 0.0;
//                  numRec = 0.0;
//                  sumWeightedResSquare = 0.0;
//                  sumRes = 0.0;
//                  sumResSquare = 0.0;
//
//                  gsName = gs;
//               }
//
//
//               switch (i)
//               {
//               case 0:
//                  {
//                     numberAllRec.push_back(column->second);
//                     allNumRec += column->second;                           // sum of all number of records
//                     Integer value = column->second;                        // convert real to integer
//                     ss4 << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(value), 20, GmatStringUtil::RIGHT);
//                  }
//                  break;
//               case 1:
//                  {
//                     numberAcceptedRec.push_back(column->second);
//                     numRec += column->second;                               // sum of all number of accepted records
//                     Integer value = column->second;                         // convert real to integer
//                     Real percent = column->second*100/numberAllRec[index];   // calculate percentage
//                     std::string sval = GmatStringUtil::ToString(percent, 2) + "% " + GmatStringUtil::ToString(value, 8);
//                     ss4 << GmatStringUtil::GetAlignmentString(sval, 20, GmatStringUtil::RIGHT);
//                  }
//                  break;
//               case 2:
//                  {
//                     sumWeightedResSquare += column->second;                 // sum of all weighted residual
//                     ss4 << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(GmatMathUtil::Sqrt(column->second/numberAcceptedRec[index]), 8), 20, GmatStringUtil::RIGHT);
//                  }
//                  break;
//               case 3:
//                  {
//                     residual.push_back(column->second);
//                     sumRes += column->second;                               // sum of all residual
//                     ss4 << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(column->second/numberAcceptedRec[index], 8), 20, GmatStringUtil::RIGHT);
//                  }
//                  break;
//               case 4:
//                  {
//                     Real res_aver = residual[index]/ numberAcceptedRec[index];
//                     sumResSquare += column->second;                         // sum of all residual square
//                     Real value = GmatMathUtil::Sqrt(column->second/numberAcceptedRec[index] - res_aver*res_aver);
//                     ss4 << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(value, 8), 20, GmatStringUtil::RIGHT);
//                  }
//                  break;
//               }
//
//               ++index;
//            }
//
//                  switch (i)
//                  {
//                  case 0:
//                     {
//                        allRecSubTotal.push_back(allNumRec);
//                        Integer value = allNumRec;                           // change type from real to integer
//                        textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(value), 20, GmatStringUtil::RIGHT);
//                     }
//                     break;
//                  case 1:
//                     {
//                        acceptedRecSubTotal.push_back(numRec);
//                        Integer value = numRec;                              // change type from real to integer
//                        Real percent = numRec*100/allRecSubTotal[index1];    // calculate percentage
//                        std::string sval = GmatStringUtil::ToString(percent, 2) + "% " + GmatStringUtil::ToString(value, 8);
//                        textFile << GmatStringUtil::GetAlignmentString(sval, 20, GmatStringUtil::RIGHT);
//                     }
//                     break;
//                  case 2:
//                     textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(GmatMathUtil::Sqrt(sumWeightedResSquare/acceptedRecSubTotal[index1]), 8), 20, GmatStringUtil::RIGHT);
//                     break;
//                  case 3:
//                     {
//                        residualSubTotal.push_back(sumRes);
//                        //textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(sumRes/acceptedRecSubTotal[index1], 8), 20, GmatStringUtil::RIGHT);
//                        textFile << "                 N/A";
//                     }
//                     break;
//                  case 4:
//                     {
//                        Real res_aver = residualSubTotal[index1]/acceptedRecSubTotal[index1];
//                        Real resSquare_aver = sumResSquare/acceptedRecSubTotal[index1];
//                        Real value = GmatMathUtil::Sqrt(resSquare_aver - res_aver*res_aver);
//                        //textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(value, 8), 20, GmatStringUtil::RIGHT);
//                        textFile << "                 N/A";
//                     }
//                     break;
//                  }
//                  ++index1;
//
//                  textFile << ss4.str();
//                  ss4.str("");
//                  allNumRec = 0.0;
//                  numRec = 0.0;
//                  sumWeightedResSquare = 0.0;
//                  sumRes = 0.0;
//                  sumResSquare = 0.0;
//
//
//            textFile << "\n";
////            MessageInterface::ShowMessage("\n");
//         }
//         textFile << "\n\n";
//
//
//         // 2.2.2. Write statistics table for measuremnet type
//         numberAllRec.clear();
//         numberAcceptedRec.clear();
//         residual.clear();
//
//         sumWeightedResSquare = 0.0;          // sum of weighted residual square
//         allNumRec = 0.0;                     // sum of number of all records
//         numRec = 0.0;                        // sum of number of accepted records
//         sumRes = 0.0;                        // sum of residual
//         sumResSquare = 0.0;                  // sum of residual square
//         for (UnsignedInt i = 0; i < sa.size(); ++i)
//         {
//            std::string typeName;
//
//            if (i == 0)
//            {
//               // Step 1: write table header:
//               textFile << "Observation Summary by Data Type\n";
//               std::stringstream ss1, ss2;
//               ss1 << "                         " << "            All";
//               ss2 << "-------------------------" << "---------------";
//               for (std::map<std::string, Real>::iterator column = statisticsTable1[sa[i]].begin(); column != statisticsTable1[sa[i]].end(); ++column)
//               {
//                  typeName = column->first;
//                  ss1 << GmatStringUtil::GetAlignmentString(typeName, 20, GmatStringUtil::RIGHT);
//                  ss2 << "--------------------";
//               }
//               textFile << ss1.str() << "\n";
//               textFile << ss2.str() << "\n";
//            }
//
//            // Step 2: Write table contents:
//            std::stringstream ss5;
//            UnsignedInt index = 0;
//            for (std::map<std::string, Real>::iterator column = statisticsTable1[sa[i]].begin(); column != statisticsTable1[sa[i]].end(); ++column)
//            {
//               switch (i)
//               {
//               case 0:
//                  {
//                     numberAllRec.push_back(column->second);
//                     allNumRec += column->second;                  // sum of all number of records
//                     Integer value = column->second;               // convert real to integer
//                     ss5 << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(value), 20, GmatStringUtil::RIGHT);
//                  }
//                  break;
//               case 1:
//                  {
//                     numberAcceptedRec.push_back(column->second);
//                     numRec += column->second;                    // sum of all accepted records
//                     Integer value = column->second;              // convert real to integer
//                     Real percent = column->second*100/ numberAllRec[index];
//                     std::string sval = GmatStringUtil::ToString(percent, 2) + "% " + GmatStringUtil::ToString(value, 8);
//                     ss5 << GmatStringUtil::GetAlignmentString(sval, 20, GmatStringUtil::RIGHT);
//                  }
//                  break;
//               case 2:
//                  {
//                     sumWeightedResSquare += column->second;      // sum of weighted residual square
//                     ss5 << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(GmatMathUtil::Sqrt(column->second/numberAcceptedRec[index]), 8), 20, GmatStringUtil::RIGHT);
//                  }
//                  break;
//               case 3:
//                  {
//                     residual.push_back(column->second);
//                     sumRes += column->second;                    // sum of residuals
//                     ss5 << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(column->second/numberAcceptedRec[index], 8), 20, GmatStringUtil::RIGHT);
//                  }
//                  break;
//               case 4:
//                  {
//                     sumResSquare += column->second;             // sum of residual square
//                     Real res_aver = residual[index]/ numberAcceptedRec[index];
//                     Real resSquare_aver = column->second/numberAcceptedRec[index];
//                     Real value = GmatMathUtil::Sqrt(resSquare_aver - res_aver*res_aver);
//                     ss5 << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(value, 8), 20, GmatStringUtil::RIGHT);
//                  }
//                  break;
//               }
//
//               ++index;
//            }
//
//            // Calculate total:
//            textFile << GmatStringUtil::GetAlignmentString(sa[i], 20);
//
//            switch(i)
//            {
//            case 0:
//               {
//                  Integer value = allNumRec;
//                  textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(value), 20, GmatStringUtil::RIGHT);
//               }
//               break;
//            case 1:
//               {
//                  Integer value = numRec;                            // convert real to integer
//                  Real percent = numRec*100/allNumRec;
//                  std::string sval = GmatStringUtil::ToString(percent, 2) + "% "+ GmatStringUtil::ToString(value, 8);
//                  textFile << GmatStringUtil::GetAlignmentString(sval, 20, GmatStringUtil::RIGHT);
//               }
//               break;
//            case 2:
//               textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(GmatMathUtil::Sqrt(sumWeightedResSquare/numRec), 8), 20, GmatStringUtil::RIGHT);
//               break;
//            case 3:
//               //textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(sumRes/numRec, 8), 20, GmatStringUtil::RIGHT);
//               textFile << "                 N/A";
//
//               break;
//            case 4:
//               {
//                  Real res_aver = sumRes/numRec;
//                  Real resSquare_aver = sumResSquare/numRec;
//                  Real value = GmatMathUtil::Sqrt(resSquare_aver - res_aver*res_aver);
//                  //textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(value, 8), 20, GmatStringUtil::RIGHT);
//                  textFile << "                 N/A";
//               }
//               break;
//            }
//
//            textFile << ss5.str() << "\n";
//            ss5.str("");
//         }
//
//
//      }
      textFile << "\n\n";

      // TODO: Plot some other form of predicted residual RMS here???
//      /// 2.3. Write WRMS and Predicted WRMS:
//      textFile.precision(12);
//      textFile << "   WeightedRMS Residuals  : " << newResidualRMS << "\n"
//               << "   PredictedRMS Residuals : " << predictedRMS << "\n";
   }


   if ((sState == CHECKINGRUN)||(sState == FINISHED))
   {
      textFile << "   DC Status              : ";
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
#ifdef DEBUG_REPORTS
   MessageInterface::ShowMessage("WriteSummary() completed\n");
#endif
}

void SeqEstimator::WriteConclusion()
{
#ifdef DEBUG_REPORTS
   MessageInterface::ShowMessage("WriteConclusion() started\n");
#endif
   Real taiMjdEpoch, utcMjdEpoch;
   std::string utcEpoch;

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

//   /// 2. Write convergence reason
//   textFile.precision(15);
//   textFile << "   " << convergenceReason << "\n"
//            << "Final Estimated State:\n";

   if (estEpochFormat != "FromParticipants")
      textFile << "   Estimation Epoch (" << estEpochFormat
               << "): " << estEpoch << "\n";
   else
   {
      textFile << "              Epoch:\n"
               << "   " << currentEpoch <<  " A.1 Mod. Julian\n";
      taiMjdEpoch = TimeConverterUtil::Convert(currentEpoch, TimeConverterUtil::A1MJD, TimeConverterUtil::TAIMJD);
      utcMjdEpoch = TimeConverterUtil::Convert(currentEpoch, TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD);
      textFile << "   " << taiMjdEpoch << " TAI Mod. Julian\n";
      utcEpoch = TimeConverterUtil::ConvertMjdToGregorian(utcMjdEpoch);
      textFile << "   " << utcEpoch << " UTCG\n";
   }

   /// 3. Write final state
   GetEstimationStateForReport(outputEstimationState);
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


//   /// 5. Write previous RMS, current RMS, and the best RMS
//   textFile.precision(12);
//   textFile << "\n   WeightedRMS residuals for previous iteration: "
//            << oldResidualRMS;
//   textFile << "\n   WeightedRMS residuals for this iteration    : "
//            << newResidualRMS ;
//   textFile << "\n   BestRMS residuals for this iteration        : "
//            << bestResidualRMS << "\n\n";

   /// 4. Write covariance matrix and correlation matrix
   /// 4.1. Write a table containing a list of solve-fors an their index
   // @todo: add code to do section 4.1. here
   textFile << "Solve-for variables and their index used in covariance and correlation matrixes in Cartesian coordinate system:\n";
   textFile << " Index      Solve-for's Name\n";
   Integer indexLen = 1;
   for (; GmatMathUtil::Pow(10,indexLen) < map->size(); ++indexLen);


   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      Integer index = i+1;
      textFile << "    " << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(index), indexLen, GmatStringUtil::RIGHT) << "     ";
      //textFile << "    " << i+1 << "     ";
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
      textFile << "\n";
   }
   textFile << "\n\n";

   // Calculate current Cartesian state map:
   std::map<GmatBase*, Rvector6> currentCartesianStateMap = CalculateCartesianStateMap(map, currentSolveForState);
   //// Calculate Keplerian covariance matrix
   //Rmatrix convmatrix = CovarianceConvertionMatrix(currentCartesianStateMap);


   /// 4.2. Write final covariance and correlation matrix
   // 4.2.1 Get covariance matrix w.r.t. Cr_Epsilon and Cd_Epsilon
   Rmatrix finalCovariance = *(stateCovariance->GetCovariance());

   // 4.2.2. Convert covariance matrix for Cr_Epsilon and Cd_Epsilon to covariance matrix for Cr and Cd
   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if ((*map)[i]->elementName == "Cr_Epsilon")
      {
         // Get Cr0
         Real Cr0 = (*map)[i]->object->GetRealParameter("Cr") / (1 + (*map)[i]->object->GetRealParameter("Cr_Epsilon"));

         // multiply row and column i with Cr0
         for(int j = 0; j < finalCovariance.GetNumColumns(); ++j)
            finalCovariance(i,j) *= Cr0;
         for(int j = 0; j < finalCovariance.GetNumRows(); ++j)
            finalCovariance(j,i) *= Cr0;
      }
      if ((*map)[i]->elementName == "Cd_Epsilon")
      {
         // Get Cd0
         Real Cd0 = (*map)[i]->object->GetRealParameter("Cd") / (1 + (*map)[i]->object->GetRealParameter("Cd_Epsilon"));

         // multiply row and column i with Cd0
         for(int j = 0; j < finalCovariance.GetNumColumns(); ++j)
            finalCovariance(i,j) *= Cd0;
         for(int j = 0; j < finalCovariance.GetNumRows(); ++j)
            finalCovariance(j,i) *= Cd0;
      }
   }

   // 4.2.3. Write covariance matrix to report file
   textFile << "Covariance Matrix in Cartesian Coordinate System:\n";
   textFile << "---------------------------------------------------------------------------------\n";
   textFile << " Row Index |                     Column Index\n";
   textFile << "           |---------------------------------------------------------------------\n";
   textFile << "           |  ";
   for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
      textFile << i+1 << "                       ";
   textFile << "\n---------------------------------------------------------------------------------\n";
   for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
   {
      //textFile << "  " << i+1 << "      ";
      textFile << "  " << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(i+1), indexLen, GmatStringUtil::RIGHT) << "    ";
      for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
      {
         char s[100];
         sprintf(&s[0],"  %22.12le", finalCovariance(i, j));
         std::string ss(s);
         textFile << ss.substr(ss.size() - 24); //finalCovariance(i, j);
      }
      textFile << "\n";
   }

   // 4.2.4. Write correlation matrix to report file
   textFile << "\nCorrelation Matrix in Cartesian Coordinate System:\n";
   textFile << "---------------------------------------------------------------------------------\n";
   textFile << " Row Index |                     Column Index\n";
   textFile << "           |---------------------------------------------------------------------\n";
   textFile << "           |      ";
   for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
      textFile << i+1 << "                       ";
   textFile << "\n---------------------------------------------------------------------------------\n";
   for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
   {
      //textFile << "  " << i+1 << "      ";
      textFile << "  " << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(i+1), indexLen, GmatStringUtil::RIGHT) << "    ";
      for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
      {
         char s[100];
         sprintf(&s[0],"  %22.12lf", finalCovariance(i, j)/ sqrt(finalCovariance(i, i)*finalCovariance(j, j)));
         std::string ss(s);
         textFile << ss.substr(ss.size() - 24); //finalCovariance(i, j)/ sqrt(finalCovariance(i, i)*finalCovariance(j, j));
      }
      textFile << "\n";
   }
   textFile << "\n\n\n";

   // Calculate and display covariance and correlation matrix for Keplerian Coordinate
   // Calculate Keplerian covariance matrix
   Rmatrix convmatrix;
   bool valid = true;
   try
   {
      convmatrix = CovarianceConvertionMatrix(currentCartesianStateMap);
   }
   catch(...)
   {
      valid = false;
   }

   if (valid)
   {
      /// 4.3. Write final covariance and correlation matrix for Keplerian coordinate system
      textFile << "Solve-for variables and their index used in covariance and correlation matrixes in Keplerian coordinate system:\n";
      textFile << "  Index      Solve-for's Name\n";
      for (UnsignedInt i = 0; i < map->size(); ++i)
      {
         Integer index = i+1;
         textFile << "    " << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(index), indexLen, GmatStringUtil::RIGHT) << "a    ";
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
            std::string name = GetElementFullName((*map)[i], false);
            Integer pos = name.find_last_of('.');
            std::string paraName = name.substr(pos + 1);
            std::string paraPrefix = name.substr(0, pos);
            if (paraName == "X")
               name = paraPrefix + ".SMA";
            else if (paraName == "Y")
               name = paraPrefix + ".ECC";
            else if (paraName == "Z")
               name = paraPrefix + ".INC";
            else if (paraName == "VX")
               name = paraPrefix + ".RAAN";
            else if (paraName == "VY")
               name = paraPrefix + ".AOP";
            else if (paraName == "VZ")
               name = paraPrefix + ".MA";
            textFile << name;
         }
         textFile << "\n";
      }
      textFile << "\n\n";

      // 4.3.1. Calculate covariance matrix w.r.t. Cr_Epsilon and Cd_Epsilon
      Rmatrix finalKeplerCovariance = convmatrix * finalCovariance * convmatrix.Transpose();          // Equation 8-49 GTDS MathSpec

      // 4.3.2. Convert covariance matrix for Cr_Epsilon and Cd_Epsilon to covariance matrix for Cr and Cd
      for (UnsignedInt i = 0; i < map->size(); ++i)
      {
         if ((*map)[i]->elementName == "Cr_Epsilon")
         {
            // Get Cr0
            Real Cr0 = (*map)[i]->object->GetRealParameter("Cr") / (1 + (*map)[i]->object->GetRealParameter("Cr_Epsilon"));

            // multiply row and column i with Cr0
            for(int j = 0; j < finalKeplerCovariance.GetNumColumns(); ++j)
               finalKeplerCovariance(i,j) *= Cr0;
            for(int j = 0; j < finalKeplerCovariance.GetNumRows(); ++j)
               finalKeplerCovariance(j,i) *= Cr0;
         }
         if ((*map)[i]->elementName == "Cd_Epsilon")
         {
            // Get Cd0
            Real Cd0 = (*map)[i]->object->GetRealParameter("Cd") / (1 + (*map)[i]->object->GetRealParameter("Cd_Epsilon"));

            // multiply row and column i with Cd0
            for(int j = 0; j < finalKeplerCovariance.GetNumColumns(); ++j)
               finalKeplerCovariance(i,j) *= Cd0;
            for(int j = 0; j < finalKeplerCovariance.GetNumRows(); ++j)
               finalKeplerCovariance(j,i) *= Cd0;
         }
      }

      textFile << "Covariance Matrix in Keplerian Coordinate System:\n";
      textFile << "---------------------------------------------------------------------------------\n";
      textFile << " Row Index |                     Column Index\n";
      textFile << "           |---------------------------------------------------------------------\n";
      textFile << "           |  ";
      for (Integer i = 0; i < finalKeplerCovariance.GetNumRows(); ++i)
         textFile << i+1 << "a                      ";
      textFile << "\n---------------------------------------------------------------------------------\n";
      for (Integer i = 0; i < finalKeplerCovariance.GetNumRows(); ++i)
      {
         textFile << "  " << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(i+1), indexLen, GmatStringUtil::RIGHT) << "a   ";
         for (Integer j = 0; j < finalKeplerCovariance.GetNumColumns(); ++j)
         {
            char s[100];
            sprintf(&s[0],"  %22.12le", finalKeplerCovariance(i, j));
            std::string ss(s);
            textFile << ss.substr(ss.size() - 24);
         }
         textFile << "\n";
      }

      textFile << "\nCorrelation Matrix in Keplerian Coordinate System:\n";
      textFile << "---------------------------------------------------------------------------------\n";
      textFile << " Row Index |                     Column Index\n";
      textFile << "           |---------------------------------------------------------------------\n";
      textFile << "           |      ";
      for (Integer i = 0; i < finalKeplerCovariance.GetNumRows(); ++i)
         textFile << i+1 << "a                      ";
      textFile << "\n---------------------------------------------------------------------------------\n";
      for (Integer i = 0; i < finalKeplerCovariance.GetNumRows(); ++i)
      {
         textFile << "  " << GmatStringUtil::GetAlignmentString(GmatStringUtil::ToString(i+1), indexLen, GmatStringUtil::RIGHT) << "a   ";
         for (Integer j = 0; j < finalKeplerCovariance.GetNumColumns(); ++j)
         {
            char s[100];
            sprintf(&s[0],"  %22.12lf", finalKeplerCovariance(i, j)/ sqrt(finalKeplerCovariance(i, i)*finalKeplerCovariance(j, j)));
            std::string ss(s);
            textFile << ss.substr(ss.size() - 24);
         }
         textFile << "\n";
      }
   }


   textFile << "\n********************************************************\n\n";
   textFile.flush();
#ifdef DEBUG_REPORTS
   MessageInterface::ShowMessage("WriteConclusion() completed\n");
#endif
}
// End EKF mod big chunk

//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
 * Generates a string for reporting the current simulator state.
 */
//------------------------------------------------------------------------------
std::string SeqEstimator::GetProgressString()
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

//-------------------------------------------------------------------------
// void GetEstimationState(GmatState& outputState)
//-------------------------------------------------------------------------
/**
 * This Method used to convert result of estimation state to participants'
 * coordinate system
 *
 * @param outState        estimation state in participants' coordinate systems
 *
*/
//-------------------------------------------------------------------------
void SeqEstimator::GetEstimationState(GmatState& outputState)
{
    const std::vector<ListItem*> *map = esm.GetStateMap();


    Real outputStateElement;
    outputState.SetSize(map->size());

    for (UnsignedInt i = 0; i < map->size(); ++i)
    {
        ConvertToPartCoordSys((*map)[i], estimationEpoch, (*estimationState)[i], &outputStateElement);
        outputState[i] = outputStateElement;
    }
   #ifdef DEBUG_RESIDUALS
      // Dump the data to screen
      MessageInterface::ShowMessage("Estimator::GetEstimationState map->size = %d\n", map->size());
   #endif
}


//------------------------------------------------------------------------------
// bool ConvertToPartCoordSys(ListItem* infor, Real epoch,
//       Real inputStateElement, Real* outputStateElement)
//------------------------------------------------------------------------------
/**
 * Method used to convert result of a state's element in A1mjd to participant's
 * coordinate system
 *
 * @param infor                 information about state's element
 * @param epoch                 the epoch at which the state is converted it's
 *                              coordinate system
 * @param inputStateElement     state's element in GMAT internal coordinate system
 *                              (A1Mjd)
 * @param outputStateElemnet    state's element in participant's coordinate system
 *
*/
//------------------------------------------------------------------------------
bool SeqEstimator::ConvertToPartCoordSys(ListItem* infor, Real epoch,
      Real inputStateElement, Real* outputStateElement)
{

   (*outputStateElement) = inputStateElement;

   if (infor->object->IsOfType(Gmat::SPACEOBJECT))
   {
      if ((infor->elementName == "CartesianState")||(infor->elementName == "Position")||(infor->elementName == "Velocity"))
      {
         SpaceObject* obj = (SpaceObject*) (infor->object);
         std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
         CoordinateSystem* cs = (CoordinateSystem*) obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
         if (cs == NULL)
            throw EstimatorException("Coordinate system for "+obj->GetName()+" is not set\n");

         SpacePoint* sp = obj->GetJ2000Body();
         CoordinateSystem* gmatcs = CoordinateSystem::CreateLocalCoordinateSystem("bodyInertial",
            "MJ2000Eq", sp, NULL, NULL, sp, cs->GetSolarSystem());

         CoordinateConverter* cv = new CoordinateConverter();
         Rvector6 inState(0.0,0.0,0.0,0.0,0.0,0.0);
         Integer index;
         if ((infor->elementName == "CartesianState")||(infor->elementName == "Position"))
            index = infor->subelement-1;
         else if (infor->elementName == "Velocity")
            index = infor->subelement+2;
         else
            throw EstimatorException("Error in Estimator object: Parameter %s has not defined in GMAT\n");

         inState.SetElement(index, inputStateElement);
         Rvector6 outState;

         cv->Convert(A1Mjd(epoch), inState, gmatcs, outState, cs);

         (*outputStateElement) = outState[index];
         delete cv;
         delete gmatcs;
      }
   }

   return true;
}


//------------------------------------------------------------------------------
// std::string SeqEstimator::GetElementFullName(ListItem* infor, bool isInternalCS) const
//------------------------------------------------------------------------------
/**
 * Name lookup for EKF reporting
 *
 * @param infor Information about the element that is being looked up
 * @param isInternalCS Flag indicating if the coordinate system is internal
 *
 * @return The name that shoule be reported
 */
//------------------------------------------------------------------------------
std::string SeqEstimator::GetElementFullName(ListItem* infor, bool isInternalCS) const
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
   else
      ss << infor->elementName << "." << infor->subelement;

   return ss.str();
}




std::map<GmatBase*, Rvector6> SeqEstimator::CalculateCartesianStateMap(const std::vector<ListItem*> *map, GmatState state)
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


std::map<GmatBase*, Rvector6> SeqEstimator::CalculateKeplerianStateMap(const std::vector<ListItem*> *map, GmatState state)
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
            MessageInterface::ShowMessage("Warning: eccentricity (%lf) is out of range (0,1) when convert Cartesian state (%lf, %lf, %lf, %lf, %lf, %lf) to Keplerian state.\n", kState[1], state[i], state[i+1], state[i+2], state[i+3], state[i+4], state[i+5]);

         stateMap[(*map)[i]->object] = kState;
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
Rmatrix66 SeqEstimator::CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state)
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
// Rmatrix Estimator::CovarianceConvertionMatrix(std::map<GmatBase*, Rvector6> stateMap)
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
Rmatrix SeqEstimator::CovarianceConvertionMatrix(std::map<GmatBase*, Rvector6> stateMap)
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
