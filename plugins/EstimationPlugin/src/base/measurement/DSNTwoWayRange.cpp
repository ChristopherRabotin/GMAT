//$Id: DSNTwoWayRange.cpp 67 2010-03-05 21:56:16Z  $
//------------------------------------------------------------------------------
//                         DSNTwoWayRange
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2010/03/08
//
/**
 * The DSN 2-way range core measurement model.
 */
//------------------------------------------------------------------------------

// this needs to be at the top for Ionosphere to work on Mac!
#include "RandomNumber.hpp" 

#include "DSNTwoWayRange.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "Spacecraft.hpp"
#include "GroundstationInterface.hpp"
#include "Antenna.hpp"
#include "Transmitter.hpp"
#include "Receiver.hpp"
#include "Transponder.hpp"
#include "Troposphere.hpp"
#include <sstream>

//#define DEBUG_RANGE_CALC_WITH_EVENTS
//#define VIEW_PARTICIPANT_STATES_WITH_EVENTS
//#define DEBUG_RANGE_CALC
//#define SHOW_RANGE_CALC
//#define DEBUG_DERIVATIVES
//#define VIEW_PARTICIPANT_STATES
//#define CHECK_PARTICIPANT_LOCATIONS
//#define DEBUG_CURRENT_MEASUREMENT


//------------------------------------------------------------------------------
// Static data initialization
//------------------------------------------------------------------------------

const std::string DSNTwoWayRange::PARAMETER_TEXT[] =
{
   "RangeModuloConstant",
};


const Gmat::ParameterType DSNTwoWayRange::PARAMETER_TYPE[] =
{
   Gmat::REAL_TYPE,
};

//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// DSNTwoWayRange(const std::string nomme)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param nomme The name of the core measurement model
 */
//------------------------------------------------------------------------------
DSNTwoWayRange::DSNTwoWayRange(const std::string nomme) :
   TwoWayRange          ("DSNTwoWayRange", nomme),
   rangeModulo          (1.0e18)
{
   objectTypeNames.push_back("DSNTwoWayRange");

   // Prep value array in measurement
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "DSNTwoWayRange";
   currentMeasurement.type = Gmat::DSN_TWOWAYRANGE;

   // Default to no delays; these are obtained from the participant hardware
   transmitDelay = 0.0;    // DSN 2-way includes electronics delays
   targetDelay   = 0.0;    // Needed for light iteration, not used otherwise
   receiveDelay  = 0.0;    // DSN 2-way includes electronics delays

   covariance.SetDimension(1);
   covariance(0,0) = 1.0;

   // If we code in specific stations, we could do it like this:
//   freqMap["DSN43"] = 2090659968.0;
//   freqMap["DSN14"] = 2090659968.0;
}


//------------------------------------------------------------------------------
// ~DSNTwoWayRange()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DSNTwoWayRange::~DSNTwoWayRange()
{
}



//------------------------------------------------------------------------------
// DSNTwoWayRange(const DSNTwoWayRange& dsn)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param dsn The model that is getting copied
 */
//------------------------------------------------------------------------------
DSNTwoWayRange::DSNTwoWayRange(const DSNTwoWayRange& dsn) :
   TwoWayRange       (dsn),
   freqMap           (dsn.freqMap),
   rangeModulo       (dsn.rangeModulo)
{
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "DSNTwoWayRange";
   currentMeasurement.type = Gmat::DSN_TWOWAYRANGE;
   currentMeasurement.uniqueID = dsn.currentMeasurement.uniqueID;
   currentMeasurement.participantIDs.push_back("NotSet");
   currentMeasurement.participantIDs.push_back("NotSet");

   covariance = dsn.covariance;
   freqMap    = dsn.freqMap;
}


//------------------------------------------------------------------------------
// DSNTwoWayRange& operator=(const DSNTwoWayRange& dsn)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param dsn The model that is getting copied
 *
 * @return This DSN 2-way range model, configured to match dsn.
 */
//------------------------------------------------------------------------------
DSNTwoWayRange& DSNTwoWayRange::operator=(const DSNTwoWayRange& dsn)
{
   if (this != &dsn)
   {
      TwoWayRange::operator=(dsn);

      // Allocate exactly one value in current measurement for range
      currentMeasurement.value.clear();
      currentMeasurement.value.push_back(0.0);
      currentMeasurement.typeName = "DSNTwoWayRange";
      currentMeasurement.type = Gmat::DSN_TWOWAYRANGE;
      currentMeasurement.uniqueID = dsn.currentMeasurement.uniqueID;

      covariance  = dsn.covariance;
      freqMap     = dsn.freqMap;
      rangeModulo = dsn.rangeModulo;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new model that matches this one, and returns it as a GmatBase
 * pointer
 *
 * @return A new DSN 2-way range model configured to match this one
 */
//------------------------------------------------------------------------------
GmatBase* DSNTwoWayRange::Clone() const
{
   return new DSNTwoWayRange(*this);
}


//------------------------------------------------------------------------------
// Parameter handling code
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer DSNTwoWayRange::GetParameterID(const std::string & str) const
{
   for (Integer i = TwoWayRangeParamCount; i < DSNTwoWayRangeParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - TwoWayRangeParamCount])
         return i;
   }

   return TwoWayRange::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string DSNTwoWayRange::GetParameterText(const Integer id) const
{
   if (id >= TwoWayRangeParamCount && id < DSNTwoWayRangeParamCount)
      return PARAMETER_TEXT[id - TwoWayRangeParamCount];

   return TwoWayRange::GetParameterText(id);
}


//---------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the enumerated type of the object.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The enumeration for the type of the parameter, or
 *         UNKNOWN_PARAMETER_TYPE.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType DSNTwoWayRange::GetParameterType(const Integer id) const
{
   if (id >= TwoWayRangeParamCount && id < DSNTwoWayRangeParamCount)
      return PARAMETER_TYPE[id - TwoWayRangeParamCount];

   return TwoWayRange::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string DSNTwoWayRange::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
std::string DSNTwoWayRange::GetParameterUnit(const Integer id) const
{
   if (id == RangeModuloConstant)
      return "RU";

   return TwoWayRange::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterCount() const
//------------------------------------------------------------------------------
/**
 * Override the default method and retrieve the total number of parameters that
 * are scriptable for the MeasurementModel plus the CoreMeasurement object.
 *
 * @return The total number of scriptable parameters from the composite object
 *
 * @note Anyone that derives a class off of MeasurementModel will need to
 *       rework this method to accommodate any new parameters added to the
 *       derived class
 */
//------------------------------------------------------------------------------
Integer DSNTwoWayRange::GetParameterCount() const
{
   return DSNTwoWayRangeParamCount - GmatBaseParamCount;
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Get value of a real parameter
* 
* @param id      index number of parameter
*
* @return        value of the parameter
*/
//------------------------------------------------------------------------------
Real DSNTwoWayRange::GetRealParameter(const Integer id) const
{
   if (id == RangeModuloConstant)
      return rangeModulo;

   return TwoWayRange::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
* Set value for a real parameter
* 
* @param id       index number of parameter
* @param value    value which is used to set to the parameter 
*
* @return         the value which is set to the parameter
*/
//------------------------------------------------------------------------------
Real DSNTwoWayRange::SetRealParameter(const Integer id, const Real value)
{
   if (id == RangeModuloConstant)
   {
      if (value <= 0)
      {
         std::stringstream ss;
         ss << "Error: RangeModuloConstant parameter has an invalid value (" << value << "). It's value has to be a positive real number\n";
         throw MeasurementException(ss.str());
      }
      else
         rangeModulo = value;
      return value;
   }

   return TwoWayRange::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string& value) const
//------------------------------------------------------------------------------
/**
* Get value of a real parameter
* 
* @param label   name of parameter
*
* @return        value of the parameter
*/
//------------------------------------------------------------------------------
Real DSNTwoWayRange::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string& lable, const Real value)
//------------------------------------------------------------------------------
/**
* Set value for a real parameter
* 
* @param lable   name of parameter
* @param value   value which is used to set to the parameter 
*
* @return        the value which is set to the parameter
*/
//------------------------------------------------------------------------------
Real DSNTwoWayRange::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the model prior to performing measurement computations
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DSNTwoWayRange::Initialize()
{
   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("Entered DSNTwoWayRange::Initialize(); "
            "this = %p\n", this);
   #endif

   bool retval = false;

   if (TwoWayRange::Initialize())
      retval = true;

   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("   Initialization %s with %d "
            "participants\n", (retval ? "succeeded" : "failed"),
            participants.size());
   #endif

   return retval;
}

//------------------------------------------------------------------------------
// const std::vector<RealArray>& CalculateMeasurementDerivatives(
//       GmatBase *obj, Integer id)
//------------------------------------------------------------------------------
/**
 * Calculates the measurement derivatives for the model
 *
 * @param obj The object supplying the "with respect to" parameter
 * @param id The ID of the parameter
 *
 * @return A matrix of the derivative data, contained in a vector of Real
 *         vectors
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& DSNTwoWayRange::CalculateMeasurementDerivatives(
      GmatBase *obj, Integer id)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("\nDSNTwoWayRange::CalculateMeasurement"
            "Derivatives(%s, %d) called\n", obj->GetName().c_str(), id);
   #endif

   if (obj == NULL)
      throw MeasurementException("Error: a NULL object inputs to DSNTwoWayRange::CalculateMeasurementDerivatives() function\n");
   

   if (!initialized)
      InitializeMeasurement();

   GmatBase *objPtr = NULL;

   Integer size = obj->GetEstimationParameterSize(id);
   Integer objNumber = -1;

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   object name = %s\n", obj->GetName().c_str());
      MessageInterface::ShowMessage("   ParameterSize = %d\n", size);
   #endif

   if (size <= 0)
      throw MeasurementException("The derivative parameter on derivative "
            "object " + obj->GetName() + "is not recognized");

   // Check to see if obj is a participant
   for (UnsignedInt i = 0; i < this->participants.size(); ++i)
   {
      if (participants[i] == obj)
      {
         objPtr = participants[i];
         objNumber = i + 1;
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Participant %s found:  objNumber = %d\n",
                  objPtr->GetName().c_str(), objNumber);
         #endif
         break;
      }
   }

   // Or if it is the measurement model for this object
   if (obj->IsOfType(Gmat::MEASUREMENT_MODEL))
      if (obj->GetRefObject(Gmat::CORE_MEASUREMENT, "") == this)
      {
         objPtr = obj;
         objNumber = 0;
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   The measurement is the object\n",
                  objPtr->GetName().c_str());
         #endif
      }


   RealArray oneRow;
   oneRow.assign(size, 0.0);
   currentDerivatives.clear();
   currentDerivatives.push_back(oneRow);

   if (objNumber == -1)
      return currentDerivatives;         // return zero vector when variable is independent of DSNTwoWay range

   Integer parameterID = GetParmIdFromEstID(id, obj);

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   Looking up id %d\n", parameterID);
   #endif

   // At this point, objPtr points to obj. Therefore, its value is not NULL
   if (objPtr != NULL)
   {
      Real fFactor = GetFrequencyFactor(frequency)/
               (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M);
     
      #ifdef DEBUG_DERIVATIVES
         if (objNumber == 2)
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of Participant %d"
            " at measurment time: tm = %.12lf\n", objPtr->GetParameterText(parameterID).c_str(), objNumber, ((Spacecraft*) objPtr)->GetEpoch());
         else
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of Participant %d\n", objPtr->GetParameterText(parameterID).c_str(), objNumber);
       
         MessageInterface::ShowMessage("   freq = %15lf, F = %15lf, F/c = "
                  "%15lf\n", frequency, GetFrequencyFactor(frequency), fFactor);
      #endif
      
      if (objNumber == 1) // participant number 1, either a GroundStation or a Spacecraft
      {
         if (objPtr->GetParameterText(parameterID) == "Position")
         {
            // Get the inverse of the orbit STM at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);

            // Uplink leg derivative:
            Rvector3 uplinkRderiv;
            GetRangeDerivative(uplinkLeg, stmInv, uplinkRderiv, true, 0, 1, true, false);

            // Downlink leg derivative:
            Rvector3 downlinkRderiv;
            GetRangeDerivative(downlinkLeg, stmInv, downlinkRderiv, false, 1, 0, true, false);

            // Add 'em up per eq 7.80
            for (Integer i = 0; i < 3; ++i)
               currentDerivatives[0][i] =
                     fFactor * (uplinkRderiv[i] + downlinkRderiv[i]);
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("uplinkDeriv[]   = %s\n",uplinkRderiv.ToString().c_str());
               MessageInterface::ShowMessage("downlinkDeriv[] = %s\n",downlinkRderiv.ToString().c_str());
               MessageInterface::ShowMessage("Position Derivative: [%.12lf "
                     "%.12lf %.12lf]\n", currentDerivatives[0][0],
                     currentDerivatives[0][1], currentDerivatives[0][2]);
            #endif

         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            // Get the inverse of the orbit STM at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);

            // Downlink leg derivative:
            Rvector3 uplinkVderiv;
            GetRangeDerivative(uplinkLeg, stmInv, uplinkVderiv, true, 0, 1, false, true);

            // Downlink leg derivative:
            Rvector3 downlinkVderiv;
            GetRangeDerivative(downlinkLeg, stmInv, downlinkVderiv, false, 1, 0, false, true);

            // Add 'em up per eq 7.81
            for (Integer i = 0; i < 3; ++i)
               currentDerivatives[0][i] =
                     fFactor * (uplinkVderiv[i] + downlinkVderiv[i]);
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("uplinkDeriv[]   = %s\n",uplinkVderiv.ToString().c_str());
               MessageInterface::ShowMessage("downlinkDeriv[] = %s\n",downlinkVderiv.ToString().c_str());
               MessageInterface::ShowMessage("Velocity Derivative: [%.12lf "
                     "%.12lf %.12lf]\n", currentDerivatives[0][0],
                     currentDerivatives[0][1], currentDerivatives[0][2]);
            #endif
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {

            // Get the inverse of the orbit STM at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);
         
            // Uplink leg derivative:
            Rvector6 uplinkDeriv;
            GetRangeDerivative(uplinkLeg, stmInv, uplinkDeriv, true, 0, 1, true, true);
         
            // Downlink leg derivative:
            Rvector6 downlinkDeriv;
            GetRangeDerivative(downlinkLeg, stmInv, downlinkDeriv, false, 1, 0, true, true);
         
            // Add 'em up per eq 7.80 and 7.81
            for (Integer i = 0; i < 6; ++i)
               currentDerivatives[0][i] =
                     fFactor * (uplinkDeriv[i] + downlinkDeriv[i]);
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("uplinkDeriv[]   = %s\n",uplinkDeriv.ToString().c_str());
               MessageInterface::ShowMessage("downlinkDeriv[] = %s\n",downlinkDeriv.ToString().c_str());
               MessageInterface::ShowMessage("CartesianState Derivative: "
                     "[%.12lf %.12lf %.12lf %.12lf %.12lf %.12lf]\n",
                     currentDerivatives[0][0], currentDerivatives[0][1],
                     currentDerivatives[0][2], currentDerivatives[0][3],
                     currentDerivatives[0][4], currentDerivatives[0][5]);
            #endif

         }
         else if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] = 1.0;
         }
         else
         {
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("   Deriv is w.r.t. something "
                        "independent, so zero\n");
            #endif
            for (UnsignedInt i = 0; i < 3; ++i)
               currentDerivatives[0][i] = 0.0;
         }
      }
      else if (objNumber == 2) // participant 2, always a Spacecraft
      {
         if (objPtr->GetParameterText(parameterID) == "Position")
         {
            // Get the inverse of the orbit STM at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);

            // Uplink leg derivative:
            Rvector3 uplinkRderiv;
            GetRangeDerivative(uplinkLeg, stmInv, uplinkRderiv, false, 0, 1, true, false);

            // Downlink leg derivative:
            Rvector3 downlinkRderiv;
            GetRangeDerivative(downlinkLeg, stmInv, downlinkRderiv, true, 1, 0, true, false);

            // Add 'em up per eq 7.80
            for (Integer i = 0; i < 3; ++i)
               currentDerivatives[0][i] =
                     fFactor * (uplinkRderiv[i] + downlinkRderiv[i]);
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("uplinkDeriv[]   = %s\n",uplinkRderiv.ToString().c_str());
               MessageInterface::ShowMessage("downlinkDeriv[] = %s\n",downlinkRderiv.ToString().c_str());
               MessageInterface::ShowMessage("Position Derivative: [%.12lf "
                     "%.12lf %.12lf]\n", currentDerivatives[0][0],
                     currentDerivatives[0][1], currentDerivatives[0][2]);
            #endif
         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            // Get the inverse of the orbit STM at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);

            // Downlink leg derivative:
            Rvector3 uplinkVderiv;
            GetRangeDerivative(uplinkLeg, stmInv, uplinkVderiv, false, 0, 1, false, true);

            // Downlink leg derivative:
            Rvector3 downlinkVderiv;
            GetRangeDerivative(downlinkLeg, stmInv, downlinkVderiv, true, 1, 0, false, true);

            // Add 'em up per eq 7.81
            for (Integer i = 0; i < 3; ++i)
               currentDerivatives[0][i] =
                     fFactor * (uplinkVderiv[i] + downlinkVderiv[i]);
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("uplinkDeriv[]   = %s\n",uplinkVderiv.ToString().c_str());
               MessageInterface::ShowMessage("downlinkDeriv[] = %s\n",downlinkVderiv.ToString().c_str());
               MessageInterface::ShowMessage("Velocity Derivative: [%.12lf "
                     "%.12lf %.12lf]\n", currentDerivatives[0][0],
                     currentDerivatives[0][1], currentDerivatives[0][2]);
            #endif
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            // Get the inverse of the orbit STM at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);
         
            // Uplink leg derivative:
            Rvector6 uplinkDeriv;
            GetRangeDerivative(uplinkLeg, stmInv, uplinkDeriv, false, 0, 1, true, true);
         
            // Downlink leg derivative:
            Rvector6 downlinkDeriv;
            GetRangeDerivative(downlinkLeg, stmInv, downlinkDeriv, true, 1, 0, true, true);
         
            // Add 'em up per eq 7.80 and 7.81
            for (Integer i = 0; i < 6; ++i)
               currentDerivatives[0][i] =
                     fFactor * (uplinkDeriv[i] + downlinkDeriv[i]);
         
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("uplinkDeriv[]   = %s\n",uplinkDeriv.ToString().c_str());
               MessageInterface::ShowMessage("downlinkDeriv[] = %s\n",downlinkDeriv.ToString().c_str());
               MessageInterface::ShowMessage("CartesianState Derivative: "
                     "[%.12lf %.12lf %.12lf %.12lf %.12lf %.12lf]\n",
                     currentDerivatives[0][0], currentDerivatives[0][1],
                     currentDerivatives[0][2], currentDerivatives[0][3],
                     currentDerivatives[0][4], currentDerivatives[0][5]);
            #endif
         }
         else if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] = 1.0;
         }
         else
         {
            for (UnsignedInt i = 0; i < 3; ++i)
               currentDerivatives[0][i] = 0.0;
         }

      }
      else if (objNumber == 0) // measurement model
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of the "
                  "measurement model\n",
                  objPtr->GetParameterText(parameterID).c_str());
         #endif
         if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] = 1.0;
         }
      }
      else
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of a non-"
                  "Participant\n",
                  objPtr->GetParameterText(parameterID).c_str());
         #endif
         for (UnsignedInt i = 0; i < 3; ++i)
            currentDerivatives[0][i] = 0.0;
      }

      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   Deriv =\n   ");
         for (Integer i = 0; i < size; ++i)
            MessageInterface::ShowMessage("   %.12lf", currentDerivatives[0][i]);
         MessageInterface::ShowMessage("\n");
      #endif
   }

   return currentDerivatives;
}


//------------------------------------------------------------------------------
// bool Evaluate(bool withEvents)
//------------------------------------------------------------------------------
/**
 * Calculates measurement values based on the current state of the participants.
 *
 * This method can perform the calculations either with or without event
 * corrections. When calculating without events, the purpose of the calculation
 * is to determine feasibility of the measurement.
 *
 * @param withEvents Flag used to toggle event inclusion
 *
 * @return true if the measurement was calculated, false if not
 */
//------------------------------------------------------------------------------
//#define USE_EARTHMJ2000EQ_CS
bool DSNTwoWayRange::Evaluate(bool withEvents)
{
   bool retval = false;

   if (!initialized)
      InitializeMeasurement();

   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("Entered DSNTwoWayRange::Evaluate(%s)\n",
            (withEvents ? "true" : "false"));
      MessageInterface::ShowMessage("  ParticipantCount: %d\n",
            participants.size());
   #endif

   // Get minimum elevation angle for ground station
   Real minAngle;
   if (participants[0]->IsOfType(Gmat::SPACECRAFT) == false)
      minAngle = ((GroundstationInterface*)participants[0])->GetRealParameter("MinimumElevationAngle");
   else if (participants[1]->IsOfType(Gmat::SPACECRAFT) == false)
      minAngle = ((GroundstationInterface*)participants[1])->GetRealParameter("MinimumElevationAngle");

   if (withEvents == false)
   {
      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("DSN 2-Way Range Calculation without "
               "events\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES
         DumpParticipantStates("++++++++++++++++++++++++++++++++++++++++++++\n"
               "Evaluating DSN 2-Way Range without events");
      #endif

      CalculateRangeVectorInertial();
      Rvector3 outState;

      // Set feasibility off of topocentric horizon, set by the Z value in topo
      // coords
      std::string updateAll = "All";
      UpdateRotationMatrix(currentMeasurement.epoch, updateAll);
      //     outState = R_o_j2k * rangeVecInertial;
      //    currentMeasurement.feasibilityValue = outState[2];
      outState = (R_o_j2k * rangeVecInertial).GetUnitVector();
      currentMeasurement.feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;      // elevation angle in degree

      #ifdef CHECK_PARTICIPANT_LOCATIONS
         MessageInterface::ShowMessage("Evaluating without events\n");
         MessageInterface::ShowMessage("Calculating DSN 2-Way Range at epoch "
               "%.12lf\n", currentMeasurement.epoch);
         MessageInterface::ShowMessage("   J2K Location of %s, id = '%s':  %s",
               participants[0]->GetName().c_str(),
               currentMeasurement.participantIDs[0].c_str(),
               p1Loc.ToString().c_str());
         MessageInterface::ShowMessage("   J2K Location of %s, id = '%s':  %s",
               participants[1]->GetName().c_str(),
               currentMeasurement.participantIDs[1].c_str(),
               p2Loc.ToString().c_str());
         Rvector3 bfLoc1 = R_o_j2k * p1Loc;
         MessageInterface::ShowMessage("   BodyFixed Location of %s:  %s",
               participants[0]->GetName().c_str(),
               bfLoc1.ToString().c_str());
         Rvector3 bfLoc2 = R_o_j2k * p2Loc;
         MessageInterface::ShowMessage("   BodyFixed Location of %s:  %s\n",
               participants[1]->GetName().c_str(),
               bfLoc2.ToString().c_str());
         Rvector3 bfRvec = bfLoc2 - bfLoc1; 
         MessageInterface::ShowMessage("   Range vector in BodyFixed coordinate system: %s\n", bfRvec.ToString().c_str());
         MessageInterface::ShowMessage("   outState: %s\n", outState.ToString().c_str());
      #endif

//      if (currentMeasurement.feasibilityValue > minAngle)
      {
         currentMeasurement.isFeasible = true;
         currentMeasurement.value[0] = 2*rangeVecInertial.GetMagnitude();
         currentMeasurement.eventCount = 2;

         SetHardwareDelays(false);

         retval = true;
      }
//      else
//      {
//         currentMeasurement.isFeasible = false;
//         currentMeasurement.value[0] = 0.0;
//         currentMeasurement.eventCount = 0;    
//      }

      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("Calculating Range at epoch %.12lf\n",
               currentMeasurement.epoch);
         MessageInterface::ShowMessage("   Location of %s, id = '%s':  %s",
               participants[0]->GetName().c_str(),
               currentMeasurement.participantIDs[0].c_str(),
               p1Loc.ToString().c_str());
         MessageInterface::ShowMessage("   Location of %s, id = '%s':  %s",
               participants[1]->GetName().c_str(),
               currentMeasurement.participantIDs[1].c_str(),
               p2Loc.ToString().c_str());
         MessageInterface::ShowMessage("   Range Vector:  %s\n",
               rangeVecInertial.ToString().c_str());
         MessageInterface::ShowMessage("   Elevation angle =  %lf degree\n",
               currentMeasurement.feasibilityValue);
         MessageInterface::ShowMessage("   Feasibility:  %s\n",
               (currentMeasurement.isFeasible ? "true" : "false"));
         MessageInterface::ShowMessage("   Range is %.12lf\n",
               currentMeasurement.value[0]);
         MessageInterface::ShowMessage("   EventCount is %d\n",
               currentMeasurement.eventCount);
      #endif

      #ifdef SHOW_RANGE_CALC
         MessageInterface::ShowMessage("Range at epoch %.12lf is ",
               currentMeasurement.epoch);
         if (currentMeasurement.isFeasible)
            MessageInterface::ShowMessage("feasible, value = %.12lf\n",
               currentMeasurement.value[0]);
         else
            MessageInterface::ShowMessage("not feasible\n");
      #endif
   }
   else
   {
      // Calculate the corrected range measurement
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("\n\n DSN 2-Way Range Calculation:\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES_WITH_EVENTS
         DumpParticipantStates("********************************************\n"
               "Evaluating DSN 2-Way Range with located events");
      #endif


      SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
      std::string cbName1 = ((SpacePoint*)participants[0])->GetJ2000BodyName();
      CelestialBody* cb1 = solarSystem->GetBody(cbName1);
      std::string cbName2 = ((SpacePoint*)participants[1])->GetJ2000BodyName();
      CelestialBody* cb2 = solarSystem->GetBody(cbName2);

      // 1. Get the range from the down link
      Rvector3 r1, r2;                  // position of downlink leg's participants in central body MJ2000Eq coordinate system 
      Rvector3 r1B, r2B;                // position of downlink leg's participants in solar system bary center MJ2000Eq coordinate system

      r1 = downlinkLeg.GetPosition(participants[0]);                              // position of station at reception time t3R in central body MJ2000Eq coordinate system
      r2 = downlinkLeg.GetPosition(participants[1]);                              // position of spacecraft at transmit time t2T in central body MJ2000Eq coordinate system
      t3R = downlinkLeg.GetEventData((GmatBase*) participants[0]).epoch;          // reception time at station for downlink leg
      t2T = downlinkLeg.GetEventData((GmatBase*) participants[1]).epoch;          // transmit time at spacecraft for downlink leg

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("Debug downlinkLeg <'%s',%p>: r1 = (%lf  %lf  %lf)\n", downlinkLeg.GetName().c_str(), &downlinkLeg, r1[0], r1[1], r1[2]);
         MessageInterface::ShowMessage("                             r2 = (%lf  %lf  %lf)\n", r2[0], r2[1], r2[2]);
      #endif

      Rvector3 ssb2cb_t3R = cb1->GetMJ2000Position(t3R) - ssb->GetMJ2000Position(t3R);      // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t3R
      Rvector3 ssb2cb_t2T = cb2->GetMJ2000Position(t2T) - ssb->GetMJ2000Position(t2T);      // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t2T
      
      r1B = ssb2cb_t3R + r1;                                                      // position of station at reception time t3R in SSB coordinate system
      r2B = ssb2cb_t2T + r2;                                                      // position of spacecraft at transmit time t2T in SSB coordinate system


      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         Rmatrix33 mt = downlinkLeg.GetEventData((GmatBase*) participants[0]).rInertial2obj.Transpose();

         MessageInterface::ShowMessage("1. Get downlink leg range:\n");
         MessageInterface::ShowMessage("   Station %s position in %sMJ2000 coordinate system    : r1 = (%.12lf, %.12lf, %.12lf)km  at epoch t3R = %.12lf\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), r1.Get(0), r1.Get(1), r1.Get(2), t3R);
         MessageInterface::ShowMessage("   Spacecraft %s position in %sMJ2000 coordinate system : r2 = (%.12lf, %.12lf, %.12lf)km  at epoch t2T = %.12lf\n", participants[1]->GetName().c_str(),  participants[1]->GetJ2000BodyName().c_str(), r2.Get(0), r2.Get(1), r2.Get(2), t2T);
         MessageInterface::ShowMessage("   Station %s position in SSB coordinate system         : r1B = (%.12lf, %.12lf, %.12lf)km  at epoch t3R = %.12lf\n", participants[0]->GetName().c_str(), r1B.Get(0), r1B.Get(1), r1B.Get(2), t3R);
         MessageInterface::ShowMessage("   Spacecraft %s position in  SSB coordinate system     : r2B = (%.12lf, %.12lf, %.12lf)km  at epoch t2T = %.12lf\n", participants[1]->GetName().c_str(), r2B.Get(0), r2B.Get(1), r2B.Get(2), t2T);

         MessageInterface::ShowMessage("   Transformation matrix from Earth fixed coordinate system to EarthFK5 coordinate system at epoch t3R = %.12lf:\n", t3R);
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt(0,0), mt(0,1), mt(0,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt(1,0), mt(1,1), mt(1,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt(2,0), mt(2,1), mt(2,2));
      #endif
#ifdef USE_EARTHMJ2000EQ_CS
      Rvector3 downlinkVector = r2 - r1;
#else
      Rvector3 downlinkVector = r2B - r1B;      // rVector = r2 - r1;
#endif
      downlinkRange = downlinkVector.GetMagnitude();

      // Calculate ET-TAI at t3R:
      Real ettaiT3 = downlinkLeg.ETminusTAI(t3R, (GmatBase*)participants[0]);

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         #ifdef USE_EARTHMJ2000EQ_CS
         MessageInterface::ShowMessage("   Downlink range without relativity correction = r2-r1:  %.12lf km\n", downlinkRange);
         #else
         MessageInterface::ShowMessage("   Downlink range without relativity correction = r2B-r1B:  %.12lf km\n", downlinkRange);
         #endif
         MessageInterface::ShowMessage("   Relativity correction for downlink leg    = %.12lf km\n", downlinkLeg.GetRelativityCorrection());
         MessageInterface::ShowMessage("   Downlink range with relativity correction = %.12lf km\n", downlinkRange + downlinkLeg.GetRelativityCorrection());
         MessageInterface::ShowMessage("   (ET-TAI) at t3R = %.12le s\n", ettaiT3);
      #endif


      // 2. Calculate down link range rate:
      Rvector3 p1V = downlinkLeg.GetVelocity(participants[0]);                        // velocity of station at reception time t3R in central body MJ2000 coordinate system
      Rvector3 p2V = downlinkLeg.GetVelocity(participants[1]);                        // velocity of specraft at transmit time t2T in central body MJ2000 coordinate system

      Rvector3 ssb2cbV_t3R = cb1->GetMJ2000Velocity(t3R) - ssb->GetMJ2000Velocity(t3R);      // velocity of central body at time t3R w.r.t SSB MJ2000Eq coordinate system
      Rvector3 ssb2cbV_t2T = cb2->GetMJ2000Velocity(t2T) - ssb->GetMJ2000Velocity(t2T);      // velocity of central body at time t2T w.r.t SSB MJ2000Eq coordinate system
      
      Rvector3 p1VB = ssb2cbV_t3R + p1V;                                       // velocity of station at reception time t3R in SSB coordinate system
      Rvector3 p2VB = ssb2cbV_t2T + p2V;                                       // velocity of spacecraft at transmit time t2T in SSB coordinate system

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("2. Get downlink leg range rate:\n");
         MessageInterface::ShowMessage("   Station %s velocity in %sMJ2000 coordinate system    : p1V = (%.12lf, %.12lf, %.12lf)km/s\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), p1V.Get(0), p1V.Get(1), p1V.Get(2));
         MessageInterface::ShowMessage("   Spacecraft %s velocity in %sMJ2000 coordinate system : p2V = (%.12lf, %.12lf, %.12lf)km/s\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), p2V.Get(0), p2V.Get(1), p2V.Get(2));
         MessageInterface::ShowMessage("   Station %s velocity in SSB coordinate system         : p1VB = (%.12lf, %.12lf, %.12lf)km/s\n", participants[0]->GetName().c_str(), p1VB.Get(0), p1VB.Get(1), p1VB.Get(2));
         MessageInterface::ShowMessage("   Spacecraft %s velocity in SSB coordinate system      : p2VB = (%.12lf, %.12lf, %.12lf)km/s\n", participants[1]->GetName().c_str(), p2VB.Get(0), p2VB.Get(1), p2VB.Get(2));
      #endif

      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
#ifdef USE_EARTHMJ2000EQ_CS
      Rvector downRRateVec = p2V - p1V /* - r12_j2k_vel*/;
#else
      Rvector downRRateVec = p2VB - p1VB /* - r12_j2k_vel*/;
#endif
      Rvector3 rangeUnit = downlinkVector.GetUnitVector();
      downlinkRangeRate = downRRateVec * rangeUnit;
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Downlink Range Rate:  %.12lf km/s\n",
               downlinkRangeRate);
      #endif


      // 3. Get the range from the uplink
      Rvector3 r3, r4;                  // position of uplink leg's participants in central body MJ2000Eq coordinate system
      Rvector3 r3B, r4B;                // position of uplink leg's participants in solar system bary center MJ2000Eq coordinate system

      r3 = uplinkLeg.GetPosition(participants[0]);                                       // position of station at transmit time t1T in central body MJ2000Eq coordinate system
      r4 = uplinkLeg.GetPosition(participants[1]);                                       // position of spacecraft at reception time t2R in central body MJ2000Eq coordinate system
      t1T = uplinkLeg.GetEventData((GmatBase*) participants[0]).epoch;                   // transmit time at station for uplink leg
      t2R = uplinkLeg.GetEventData((GmatBase*) participants[1]).epoch;                   // reception time at spacecraft for uplink leg
     
      Rvector3 ssb2cb_t2R = cb2->GetMJ2000Position(t2R) - ssb->GetMJ2000Position(t2R);   // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t2R
      Rvector3 ssb2cb_t1T = cb1->GetMJ2000Position(t1T) - ssb->GetMJ2000Position(t1T);   // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t1T
      
      r3B = ssb2cb_t1T + r3;                                                             // position of station at transmit time t1T in SSB coordinate system
      r4B = ssb2cb_t2R + r4;                                                             // position of spacecraft at reception time t2R in SSB coordinate system


      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         Rmatrix33 mt1 = uplinkLeg.GetEventData((GmatBase*) participants[0]).rInertial2obj.Transpose();

         MessageInterface::ShowMessage("3. Get uplink leg range:\n");
         MessageInterface::ShowMessage("   Spacecraft %s position in %sMJ2000 coordinate system : r4 = (%.12lf, %.12lf, %.12lf)km   at epoch t2R = %.12lf\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), r4.Get(0), r4.Get(1), r4.Get(2), t2R);
         MessageInterface::ShowMessage("   Station %s position in %sMJ2000 coordinate system    : r3 = (%.12lf, %.12lf, %.12lf)km   at epoch t1T = %.12lf\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), r3.Get(0), r3.Get(1), r3.Get(2), t1T);
         MessageInterface::ShowMessage("   Spacecraft %s position in SSB coordinate system      : r4B = (%.12lf, %.12lf, %.12lf)km   at epoch t2R = %.12lf\n", participants[1]->GetName().c_str(), r4B.Get(0), r4B.Get(1), r4B.Get(2), t2R);
         MessageInterface::ShowMessage("   Station %s position in SSB coordinate system         : r3B = (%.12lf, %.12lf, %.12lf)km   at epoch t1T = %.12lf\n", participants[0]->GetName().c_str(), r3B.Get(0), r3B.Get(1), r3B.Get(2), t1T);

         MessageInterface::ShowMessage("   Transformation matrix from Earth fixed coordinate system to EarthFK5 coordinate system at epoch t1T = %.12lf:\n", t1T);
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt1(0,0), mt1(0,1), mt1(0,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt1(1,0), mt1(1,1), mt1(1,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt1(2,0), mt1(2,1), mt1(2,2));
      #endif
#ifdef USE_EARTHMJ2000EQ_CS
      Rvector3 uplinkVector = r4 - r3;
#else
      Rvector3 uplinkVector = r4B - r3B;
#endif
      uplinkRange = uplinkVector.GetMagnitude();

      // Calculate ET-TAI at t1T:
      Real ettaiT1 = downlinkLeg.ETminusTAI(t1T, (GmatBase*)participants[0]);

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         #ifdef USE_EARTHMJ2000EQ_CS
         MessageInterface::ShowMessage("   Uplink range without relativity correction = r4-r3:  %.12lf km\n", uplinkRange);
         #else
         MessageInterface::ShowMessage("   Uplink range without relativity correction = r4B-r3B:  %.12lf km\n", uplinkRange);
         #endif
         MessageInterface::ShowMessage("   Relativity correction for uplink leg    = %.12lf km\n", uplinkLeg.GetRelativityCorrection());
         MessageInterface::ShowMessage("   Uplink range without relativity correction = %.12lf km\n", uplinkRange + uplinkLeg.GetRelativityCorrection());
         MessageInterface::ShowMessage("   (ET-TAI) at t1T = %.12le s\n", ettaiT1);
      #endif


      // 4. Calculate up link range rate
      Rvector3 p3V = uplinkLeg.GetVelocity(participants[0]);
      Rvector3 p4V = uplinkLeg.GetVelocity(participants[1]);


      Rvector3 ssb2cbV_t2R = cb2->GetMJ2000Velocity(t2R) - ssb->GetMJ2000Velocity(t2R);      // velocity of central body at time t2R w.r.t SSB MJ2000Eq coordinate system
      Rvector3 ssb2cbV_t1T = cb1->GetMJ2000Velocity(t1T) - ssb->GetMJ2000Velocity(t1T);      // velocity of central body at time t1T w.r.t SSB MJ2000Eq coordinate system
      
      Rvector3 p3VB = ssb2cbV_t1T + p3V;                                       // velocity of station at reception time t1T in SSB coordinate system
      Rvector3 p4VB = ssb2cbV_t2R + p4V;                                       // velocity of spacecraft at transmit time t2R in SSB coordinate system

      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
#ifdef USE_EARTHMJ2000EQ_CS
      Rvector upRRateVec = p4V - p3V /* - r12_j2k_vel*/ ;
#else
      Rvector upRRateVec = p4VB - p3VB /* - r12_j2k_vel*/ ;
#endif
      rangeUnit = uplinkVector.GetUnitVector();
      uplinkRangeRate = upRRateVec * rangeUnit;

      // 4.1. Target range rate: Do we need this as well?
      targetRangeRate = (downlinkRangeRate + uplinkRangeRate) / 2.0;

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("4. Get uplink leg range rate:\n");
         MessageInterface::ShowMessage("   Station %s velocity in %sMJ2000 coordinate system    : p3V = (%.12lf, %.12lf, %.12lf)km/s\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), p3V.Get(0), p3V.Get(1), p3V.Get(2));
         MessageInterface::ShowMessage("   Spacecraft %s velocity in %sMJ2000 coordinate system : p4V = (%.12lf, %.12lf, %.12lf)km/s\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), p4V.Get(0), p4V.Get(1), p4V.Get(2));
         MessageInterface::ShowMessage("   Station %s velocity in SSB coordinate system         : p3VB = (%.12lf, %.12lf, %.12lf)km/s\n", participants[0]->GetName().c_str(), p3VB.Get(0), p3VB.Get(1), p3VB.Get(2));
         MessageInterface::ShowMessage("   Spacecraft %s velocity in SSB coordinate system      : p4VB = (%.12lf, %.12lf, %.12lf)km/s\n", participants[1]->GetName().c_str(), p4VB.Get(0), p4VB.Get(1), p4VB.Get(2));
         MessageInterface::ShowMessage("   Uplink Range Rate:  %.12lf km/s\n", uplinkRangeRate);
         MessageInterface::ShowMessage("   Target Range Rate:  %.12lf km/s\n", targetRangeRate);
         MessageInterface::ShowMessage("   Delay between transmiting signal and receiving signal at transponder:  t2T - t2R = %.12le s\n", (t2T-t2R)*86400);
      #endif


      // 5. Get sensors used in DSN 2-ways range
      UpdateHardware();
      if (participantHardware.empty()||
           ((!participantHardware.empty())&&
              participantHardware[0].empty()&&
              participantHardware[1].empty()
           )
        )
      {
         // Throw an exception when no hardware is defined in measurement due to frequency factor specified 
         // based on transmitted frequency from transmitter.
         throw MeasurementException("No transmmitter, transponder, and receiver is defined in measurement participants.\n");
         return false;
      }
      
      ObjectArray objList1;
      ObjectArray objList2;
      ObjectArray objList3;
      //         objList1 := all transmitters in participantHardware list
      //         objList2 := all receivers in participantHardware list
      //         objList3 := all transponders in participantHardware list
      for(std::vector<Hardware*>::iterator hw = this->participantHardware[0].begin();
              hw != this->participantHardware[0].end(); ++hw)
      {
         if ((*hw) != NULL)
         {
            if ((*hw)->GetTypeName() == "Transmitter")
               objList1.push_back(*hw);
            if ((*hw)->GetTypeName() == "Receiver")
               objList2.push_back(*hw);
         }
         else
            MessageInterface::ShowMessage(" sensor = NULL\n");
      }

      for(std::vector<Hardware*>::iterator hw = this->participantHardware[1].begin();
              hw != this->participantHardware[1].end(); ++hw)
      {
         if ((*hw) != NULL)
         {
            if ((*hw)->GetTypeName() == "Transponder")
               objList3.push_back(*hw);
         }
         else
            MessageInterface::ShowMessage(" sensor = NULL\n");
      }

      if (objList1.size() != 1)
         throw MeasurementException(((objList1.size() == 0)?"Error: The first participant does not have a transmitter to send signal.\n":"Error: The first participant has more than one transmitter.\n"));
      if (objList2.size() != 1)
         throw MeasurementException(((objList2.size() == 0)?"Error: The first participant does not have a receiver to receive signal.\n":"Error: The first participant has more than one receiver.\n"));
      if (objList3.size() != 1)
         throw MeasurementException((objList3.size() == 0)?"Error: The second participant does not have a transponder to transpond signal.\n":"Error: The second participant has more than one transponder.\n");

      Transmitter*    gsTransmitter    = (Transmitter*)objList1[0];
      Receiver*       gsReceiver       = (Receiver*)objList2[0];
      Transponder*    scTransponder    = (Transponder*)objList3[0];
      if (gsTransmitter == NULL)
         throw GmatBaseException("Transmitter is NULL object.\n");
      if (gsReceiver == NULL)
         throw GmatBaseException("Receiver is NULL object.\n");
      if (scTransponder == NULL)
         throw GmatBaseException("Transponder is NULL object.\n");

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("5. Sensors, delays, and signals:\n");
         MessageInterface::ShowMessage("   List of sensors: %s, %s, %s\n",
            gsTransmitter->GetName().c_str(), gsReceiver->GetName().c_str(),
            scTransponder->GetName().c_str());
      #endif


      // 6. Get transmitter, receiver, and transponder delays:
      transmitDelay = gsTransmitter->GetDelay();
      receiveDelay = gsReceiver->GetDelay();
      targetDelay = scTransponder->GetDelay();

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Transmitter delay = %le s\n", gsTransmitter->GetDelay());
         MessageInterface::ShowMessage("   Receiver delay = %le s\n", gsReceiver->GetDelay());
         MessageInterface::ShowMessage("   Transponder delay = %le s\n", scTransponder->GetDelay());
      #endif
     

      // 7. Get frequency from transmitter of ground station (participants[0])
      Real uplinkFreq;                                  // uplink frequency at transmit epoch
      Real uplinkFreqAtRecei;                           // uplink frequency at receive epoch
      if (obsData == NULL)
      {
         if (rampTB == NULL)
         {
            // Get uplink frequency from GMAT script when ramp table is not used
            Signal* uplinkSignal = gsTransmitter->GetSignal();
            uplinkFreq        = uplinkSignal->GetValue();                  // unit: MHz
            uplinkFreqAtRecei = uplinkFreq;                                // unit: MHz      // for constant frequency
            frequency         = uplinkFreq*1.0e6;                          // unit: Hz

            // Get uplink band based on definition of frequency range
            freqBand = FrequencyBand(frequency);

            // Range modulo constant is specified by GMAT script
            #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
               MessageInterface::ShowMessage("   Uplink frequency is gotten from GMAT script...\n"); 
            #endif
         }
         else
         {
            // Get uplink frequency at a given time from ramped frequency table
            frequency         = GetFrequencyFromRampTable(t1T);            // unit: Hz      // Get frequency at transmit time t1T
            uplinkFreq        = frequency/1.0e6;                           // unit MHz
            uplinkFreqAtRecei = GetFrequencyFromRampTable(t3R) / 1.0e6;    // unit MHz      // for ramped frequency

            // Get frequency band from ramp table at given time
            freqBand = GetUplinkBandFromRampTable(t1T);

            // Range modulo constant is specified by GMAT script
            #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
               MessageInterface::ShowMessage("   Uplink frequency is gotten from ramp table...: frequency = %.12le\n", frequency); 
            #endif
         }
      }
      else
      {
         if (rampTB == NULL)
         {
            //// Get uplink frequency at a given time from observation data
            //frequency = obsData->uplinkFreq;                       // unit: Hz   

            //#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            //MessageInterface::ShowMessage("   Uplink frequency is gotten from observation data...: frequency = %.12le\n", frequency); 
            //#endif

            // Get uplink frequency from GMAT script when ramp table is not used
            Signal* uplinkSignal = gsTransmitter->GetSignal();
            uplinkFreq        = uplinkSignal->GetValue();                  // unit: MHz
            uplinkFreqAtRecei = uplinkFreq;                                // unit: MHz        // for constant frequency
            frequency         = uplinkFreq*1.0e6;                          // unit: Hz

            // Get uplink band based on definition of frequency range
            freqBand = FrequencyBand(frequency);

            // Range modulo constant is specified by GMAT script
            #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("   Uplink frequency is gotten from GMAT script...\n");
            #endif
         }
         else
         {
            // Get uplink frequency at a given time from ramped frequency table
            frequency = GetFrequencyFromRampTable(t1T);                      // unit: Hz      // Get frequency at transmit time t1T
            uplinkFreq = frequency / 1.0e6;                                  // unit: MHz
            uplinkFreqAtRecei = GetFrequencyFromRampTable(t3R) / 1.0e6;      // unit: MHz     // for ramped frequency

            // Get frequency band from ramp table at given time
            freqBand = GetUplinkBandFromRampTable(t1T);

            #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
               MessageInterface::ShowMessage("   Uplink frequency is gotten from ramp table...: frequency = %.12le\n", frequency); 
            #endif
         }

         //uplinkFreq = frequency/1.0e6;                            // unit: MHz
         //freqBand = obsData->uplinkBand;
         rangeModulo = obsData->rangeModulo;                      // unit: range unit
         obsValue = obsData->value;                               // unit: range unit

         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("   Uplink band, range modulo constant, and observation value are gotten from observation data...\n"); 
         #endif

      }
     

      // 8. Calculate media correction for uplink leg:
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS   
         MessageInterface::ShowMessage("6. Media correction for uplink leg\n");
         MessageInterface::ShowMessage("   UpLink signal frequency = %.12lf MHz\n", uplinkFreq);
      #endif

      // r3B and r4B are location of station and spacecraft in SSBMJ2000Eq coordinate system for uplink leg
      RealArray uplinkCorrection = CalculateMediaCorrection(uplinkFreq, r3B, r4B, t1T, t2R, minAngle);

      Real uplinkRangeCorrection = uplinkCorrection[0]*GmatMathConstants::M_TO_KM + uplinkLeg.GetRelativityCorrection();
      Real uplinkRealRange = uplinkRange + uplinkRangeCorrection;
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Uplink media correction           = %.12lf m\n",uplinkCorrection[0]);
         MessageInterface::ShowMessage("   Uplink relativity correction      = %.12lf km\n",uplinkLeg.GetRelativityCorrection());
         MessageInterface::ShowMessage("   Uplink total range correction     = %.12lf km\n",uplinkRangeCorrection);
         MessageInterface::ShowMessage("   Uplink precision light time range = %.12lf km\n",uplinkRange);
         MessageInterface::ShowMessage("   Uplink real range                 = %.12lf km\n",uplinkRealRange);
      #endif


      // 9. Doppler shift the frequency from the transmitter using uplinkRangeRate:
      Real uplinkDSFreq = (1 - uplinkRangeRate*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*uplinkFreq;

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("7. Transponder input and output frequencies\n");
         MessageInterface::ShowMessage("   Uplink Doppler shift frequency = %.12lf MHz\n", uplinkDSFreq);
      #endif


      // 10.Set frequency for the input signal of transponder
      Signal* inputSignal = scTransponder->GetSignal(0);
      inputSignal->SetValue(uplinkDSFreq);
      scTransponder->SetSignal(inputSignal, 0);


      // 11. Check the transponder feasibility to receive the input signal:
      if (scTransponder->IsFeasible(0) == false)
      {
         currentMeasurement.isFeasible = false;
         currentMeasurement.value[0] = 0;
         throw MeasurementException("The transponder is unfeasible to receive uplink signal.\n");
      }


      // 12. Get frequency of transponder output signal
      Signal* outputSignal = scTransponder->GetSignal(1);
      Real downlinkFreq = outputSignal->GetValue();

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("  Downlink frequency = %.12lf Mhz\n", downlinkFreq);
      #endif


      // 13. Doppler shift the transponder output frequency:
      Real downlinkDSFreq = (1 - downlinkRangeRate*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*downlinkFreq;

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("  Downlink Doppler shift frequency = %.12lf MHz\n", downlinkDSFreq);
      #endif


      // 14. Set frequency on receiver
      Signal* downlinkSignal = gsReceiver->GetSignal();
      downlinkSignal->SetValue(downlinkDSFreq);


      // 15. Check the receiver feasibility to receive the downlink signal
      if (gsReceiver->IsFeasible() == false)
      {
         currentMeasurement.isFeasible = false;
         currentMeasurement.value[0] = 0;
         throw MeasurementException("The receiver is unfeasible to receive downlink signal.\n");
      }


      // 16. Calculate media correction for downlink leg:
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("8. Media correction for downlink leg\n");
      #endif
      // r1B and r2B are location of station and spacecraft in SSBMJ2000Eq coordinate system for downlink leg
      RealArray downlinkCorrection = CalculateMediaCorrection(downlinkDSFreq, r1B, r2B, t3R, t2T, minAngle);

      Real downlinkRangeCorrection = downlinkCorrection[0]*GmatMathConstants::M_TO_KM + downlinkLeg.GetRelativityCorrection();
      Real downlinkRealRange = downlinkRange + downlinkRangeCorrection;
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Downlink media correction           = %.12lf m\n",downlinkCorrection[0]);
         MessageInterface::ShowMessage("   Downlink relativity correction      = %.12lf km\n",downlinkLeg.GetRelativityCorrection());
         MessageInterface::ShowMessage("   Downlink total range correction     = %.12lf km\n",downlinkRangeCorrection);
         MessageInterface::ShowMessage("   Downlink precision light time range = %.12lf km\n",downlinkRange);
         MessageInterface::ShowMessage("   Downlink real range                 = %.12lf km\n",downlinkRealRange);
      #endif

      // 17. Calculate travel time 
      // 17.1. Calculate uplink time and down link time:
      uplinkTime   = uplinkRealRange*GmatMathConstants::KM_TO_M / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;
      downlinkTime = downlinkRealRange*GmatMathConstants::KM_TO_M / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;
      // 17.2. Calculate ET-TAI correction
      Real ettaiCorrection = (useETminusTAICorrection?(ettaiT1 - ettaiT3):0.0);
      // 17.3 Calculate travel time
      Real realTravelTime = uplinkTime + downlinkTime + ettaiCorrection + receiveDelay + transmitDelay + targetDelay;      // unit: second

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("9. Travel time:\n");
         MessageInterface::ShowMessage("   Uplink time         = %.12lf s\n",uplinkTime);
         MessageInterface::ShowMessage("   Downlink time       = %.12lf s\n",downlinkTime);
         MessageInterface::ShowMessage("   (ET-TAI) correction = %.12le s\n", ettaiCorrection);
         MessageInterface::ShowMessage("   Transmit delay      = %.12le s\n", transmitDelay);
         MessageInterface::ShowMessage("   Transpond delay     = %.12le s\n", targetDelay);
         MessageInterface::ShowMessage("   Receive delay       = %.12le s\n", receiveDelay);
         MessageInterface::ShowMessage("   Real travel time    = %.15lf s\n",realTravelTime);
      #endif


      //18. Verify uplink leg light path not to be blocked by station's central body
      // UpdateRotationMatrix(t1T, "R_o_j2k");
      UpdateRotationMatrix(t1T, "o_j2k");
      // Rvector3 outState = (R_o_j2k * (r4 - r3)).GetUnitVector();
      Rvector3 rVec = r4B - r3B;
      Rvector3 obsVec = R_o_j2k * rVec;
      Rvector3 outState = obsVec.GetUnitVector();           // It has to use range vector in SSB coordinate system
      currentMeasurement.feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;      // elevation angle in degree
      if (currentMeasurement.feasibilityValue > minAngle)
      {
         // UpdateRotationMatrix(t3R, "R_o_j2k");
         UpdateRotationMatrix(t3R, "o_j2k");
         // outState = (R_o_j2k * (r2 - r1)).GetUnitVector();
         rVec = r2B - r1B;
         obsVec = R_o_j2k * rVec;
         outState = obsVec.GetUnitVector();                 // It has to use range vector in SSB coordinate system
         Real feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;                 // elevation angle in degree

         if (feasibilityValue > minAngle)
         {
            currentMeasurement.unfeasibleReason = "N";
            currentMeasurement.isFeasible = true;
            //MessageInterface::ShowMessage(" N up   :  uplink:  %.12lf  downlink:  %.12lf\n", currentMeasurement.feasibilityValue, feasibilityValue);
         }
         else
         {
            //MessageInterface::ShowMessage(" B2 down:  uplink:  %.12lf  downlink:  %.12lf\n", currentMeasurement.feasibilityValue, feasibilityValue);
            currentMeasurement.feasibilityValue = feasibilityValue;
            currentMeasurement.unfeasibleReason = "B2";
            currentMeasurement.isFeasible = false;
         }
      }
      else
      {
         currentMeasurement.unfeasibleReason = "B1";
         currentMeasurement.isFeasible = false;

         //UpdateRotationMatrix(t3R, "o_j2k");
         //outState = (R_o_j2k * (r2B - r1B)).GetUnitVector();                 // It has to use range vector in SSB coordinate system
         //Real feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;                 // elevation angle in degree
         
      }

      // 19. Calculate real range
      Real freqConversionFactor = GetFrequencyFactor(frequency);
      Real realRangeKm = realTravelTime * GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM/1000.0;          // unit: km
      Real realRange;                                                                                   // unit: range unit
      Real realRangeFull;                                                                               // unit: range unit

      if (rampTB == NULL)
      {
         // unrapmed frequency: Moyer's eq 13-122
         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("   Unramped frequency calculation is used\n");
         #endif

         realRangeFull = realTravelTime*freqConversionFactor;                                 // unit: range unit
      }
      else
      {
         // ramped frequency: Moyer's eq 13-120
         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("   Ramped frequency calculation is used\n");
         #endif
         Integer errnum;
         try
         {
            realRangeFull = IntegralRampedFrequency(t3R, realTravelTime, errnum);                  // unit: range unit
         } catch (MeasurementException exp)
         {
            currentMeasurement.value[0] = 0.0;               // It has no C-value due to the failure of calculation of IntegralRampedFrequency()
            currentMeasurement.uplinkFreq = frequency;                              // unit: Hz
            currentMeasurement.uplinkFreqAtRecei = uplinkFreqAtRecei * 1.0e6;       // unit: Hz
            currentMeasurement.uplinkBand = freqBand;
            currentMeasurement.rangeModulo = rangeModulo;
            currentMeasurement.isFeasible = false;
            currentMeasurement.unfeasibleReason = "R";
//          currentMeasurement.feasibilityValue is set to elevation angle as shown in section 18
            if ((errnum == 2)||(errnum == 3))
               throw exp;
            else
               return false;
         }
      }

      // Add noise to calculated measurement
      if (noiseSigma != NULL)
      {
         Real v_sign = ((realRangeFull < 0.0)?-1.0:1.0);
         RandomNumber* rn = RandomNumber::Instance();
         Real val = rn->Gaussian(realRangeFull, noiseSigma->GetElement(0));
         while (val*v_sign <= 0.0)
           val = rn->Gaussian(realRangeFull, noiseSigma->GetElement(0));
         realRangeFull = val;
      }
      // Get range
      realRange = GmatMathUtil::Mod(realRangeFull, rangeModulo);                              // unit: range unit
     

      //if (obsData != NULL)
      //{
      //   if (GmatMathUtil::Abs(realRange - obsValue[0]) > rangeModulo/2)
      //   {
      //      if (realRange > obsValue[0])
      //         realRange = realRange - rangeModulo;
      //      else
      //         realRange = realRange + rangeModulo;
      //   }
      //}
     

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Range modulo constant (in range unit)       = %.12le\n", rangeModulo);
         MessageInterface::ShowMessage("   Uplink frequency                            = %.12le Hz\n", frequency);
         MessageInterface::ShowMessage("   Frequency conversion factor                 = %.12le Hz\n", freqConversionFactor);
         MessageInterface::ShowMessage("   Calculated round trip range (in km)         = %.8lf km\n", realRangeKm);
         MessageInterface::ShowMessage("   Calculated full range value (in range unit) = %.8lf\n", realRangeFull);
         MessageInterface::ShowMessage("   Calculated real range (in range unit)       = %.8lf\n", realRange);
         if (obsData != NULL)
            MessageInterface::ShowMessage("   Observation range     (in range unit)       = %.8lf\n", obsValue[0]);
      #endif



      // 20. Set value for currentMeasurement
      // currentMeasurement.value[0] = realRange;
      currentMeasurement.value[0] = realRangeFull;
      currentMeasurement.uplinkFreq = frequency;                                // unit: Hz
      currentMeasurement.uplinkFreqAtRecei = uplinkFreqAtRecei * 1.0e6;         // unit: Hz
      currentMeasurement.uplinkBand = freqBand;
      currentMeasurement.rangeModulo = rangeModulo;


      #ifdef DEBUG_CURRENT_MEASUREMENT
         MessageInterface::ShowMessage("   currentMeasurement = <%p>\n", currentMeasurement);
         MessageInterface::ShowMessage("   currentMeasurement.rangeModulo = %.12le\n", currentMeasurement.rangeModulo);
         MessageInterface::ShowMessage("   currentMeasurement.value[0] = %.12le\n", currentMeasurement.value[0]);
         MessageInterface::ShowMessage("   currentMeasurement.isFeasible = %s\n", (currentMeasurement.isFeasible ? "true" : "false"));
         MessageInterface::ShowMessage("   currentMeasurement.feasibilityValue = %.12le\n", currentMeasurement.feasibilityValue);
      #endif

      
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// Real GetFrequencyFactor(Real frequency)
//------------------------------------------------------------------------------
/**
 * Constructs the multiplier used to convert into range units
 *
 * @param frequency F_T used in the computation.  The default (0.0) generates
 *                  the factor for DSN14.
 *
 * @return The factor used in the conversion.
 */
//------------------------------------------------------------------------------
Real DSNTwoWayRange::GetFrequencyFactor(Real frequency)
{
   Real factor;

   if ((obsData == NULL)&&(rampTB == NULL))
   {
      // Map the frequency to the corresponding factor here
      if ((frequency >= 2000000000.0) && (frequency <= 4000000000.0))
      {
         // S-band
         factor = frequency / 2.0;
         if (freqBand == 0)
            freqBand = 1;               // 1 for S-band
      }
      else if ((frequency >= 7000000000.0) && (frequency <= 8400000000.0))
      {
         // X-band (Band values from Wikipedia; check them!
         // factor = frequency * 11.0 / 75.0;            // for X-band with BVE and HEF attenna mounted before BVE:    Moyer's eq 13-109
         factor = frequency * 221.0 / 1498.0;            // for X-band with BVE:   Moyer's eq 13-110
         if (freqBand == 0)
            freqBand = 2;               // 2 for X-band
      }
      else
      {
         std::stringstream ss;
         ss << "Error: No frequency band was specified for frequency = " << frequency << "Hz\n";
         throw MeasurementException(ss.str());
      }
      // Todo: Figure out how to detect HEV and BVE
   }
   else
   {
      switch (freqBand)
      {
         case 1:
            factor = frequency/ 2.0;
            break;
         case 2:
            factor = frequency *221.0/1498.0;
            break;
      }
   }

   return factor;
}


//------------------------------------------------------------------------------
// Real RampedFrequencyIntergration(Real t0, Real delta_t)
//------------------------------------------------------------------------------
/**
 * Calculate the tetegration of ramped frequency in range from time t0 to time t1
 *
 * @param t1         The end time for integration (unit: A1Mjd)
 * @param delta_t    Elapse time (unit: second)
 * @param err        Error number
 *
 * @return The integration of ramped frequency.
 * Assumptions: ramp table had been sorted by epoch 
 */
//------------------------------------------------------------------------------
Real DSNTwoWayRange::IntegralRampedFrequency(Real t1, Real delta_t, Integer& err)
{
   err = 0;
   if (delta_t < 0)
   {
      err = 1;
      throw MeasurementException("Error: Elapse time has to be a non negative number\n");
   }

   if (rampTB == NULL)
   {
      err = 2;
      throw MeasurementException("Error: No ramp table available for measurement calculation\n");
   } 
      
   if ((*rampTB).size() == 0)
   {
      err = 3;
      std::stringstream ss;
      ss << "Error: Ramp table has " << (*rampTB).size() << " data records. It needs at least 2 records\n";
      throw MeasurementException(ss.str());
   }

   // Get the beginning index and the ending index for frequency data records for this measurement model 
   UnsignedInt beginIndex, endIndex;
   BeginEndIndexesOfRampTable(beginIndex, endIndex, err);



   Real t0 = t1 - delta_t/GmatTimeConstants::SECS_PER_DAY; 
//   Real time_min = (*rampTB)[0].epoch;
//   Real time_max = (*rampTB)[(*rampTB).size() -1 ].epoch;
   Real time_min = (*rampTB)[beginIndex].epoch;
   Real time_max = (*rampTB)[endIndex-1].epoch;

#ifdef RAMP_TABLE_EXPANDABLE
   Real correct_val = 0;
   if (t1 < time_min)
   {
      // t0 and t1 < time_min
      //return delta_t*(*rampTB)[0].rampFrequency;
      return delta_t*(*rampTB)[beginIndex].rampFrequency;
   }
   else if (t1 > time_max)
   {
      if (t0 < time_min)
      {
         // t0 < time_min < time_max < t1
         //correct_val = (*rampTB)[0].rampFrequency * (time_min-t0)*GmatTimeConstants::SECS_PER_DAY;
         correct_val = (*rampTB)[beginIndex].rampFrequency * (time_min-t0)*GmatTimeConstants::SECS_PER_DAY;
         t0 = time_min;
      }
      else if (t0 > time_max)
      {
         // t0 and t1 > time_max
         //return delta_t*(*rampTB)[(*rampTB).size()-1].rampFrequency;
         return delta_t*(*rampTB)[endIndex-1].rampFrequency;
      }
      else
      {
         // time_min <= t0 <= time_max < t1
         //correct_val = (*rampTB)[(*rampTB).size() -1].rampFrequency * (t1-time_max)*GmatTimeConstants::SECS_PER_DAY;
         correct_val = (*rampTB)[endIndex -1].rampFrequency * (t1-time_max)*GmatTimeConstants::SECS_PER_DAY;
         t1 = time_max;
      }
   }
   else
   {
      if (t0 < time_min)
      {
         // t0 < time_min <= t1 <= time_max
         //correct_val = (*rampTB)[0].rampFrequency * (time_min-t0)*GmatTimeConstants::SECS_PER_DAY;
         correct_val = (*rampTB)[beginIndex].rampFrequency * (time_min-t0)*GmatTimeConstants::SECS_PER_DAY;
         t0 = time_min;
      }
   }
#endif

   if ((t1 < time_min)||(t1 > time_max))
   {
      char s[200];
      sprintf(&s[0], "Error: End epoch t3R = %.12lf is out of range [%.12lf , %.12lf] of ramp table\n", t1, time_min, time_max);
      std::string st(&s[0]);
      //MessageInterface::ShowMessage("%s", st.c_str());
      err = 4;
      throw MeasurementException(st);
   }

   if ((t0 < time_min)||(t0 > time_max))
   {
      char s[200];
      sprintf(&s[0], "Error: Start epoch t1T = %.12lf is out of range [%.12lf , %.12lf] of ramp table\n", t0, time_min, time_max);
      std::string st(&s[0]);
      err = 5;
      throw MeasurementException(st);
   }

   // search for end interval:
   UnsignedInt end_interval = beginIndex;
   for (UnsignedInt i = beginIndex; i < endIndex; ++i)
   {
      if (t1 >= (*rampTB)[i].epoch)
         end_interval = i;
      else
         break;
   }

   // search for end interval:
   Real f0, f1, f_dot;
   Real value1;
   Real interval_len;

   Real basedFreq = (*rampTB)[end_interval].rampFrequency;
   Real basedFreqFactor = GetFrequencyFactor(basedFreq);
   Real value = 0.0;
   Real dt = delta_t;
   Integer i = end_interval;
   while (dt > 0)
   {
      f_dot = (*rampTB)[i].rampRate;

      // Specify frequency at the begining and lenght of the current interval   
      if (i == end_interval)
         interval_len = (t1 - (*rampTB)[i].epoch)*GmatTimeConstants::SECS_PER_DAY;
      else
         interval_len = ((*rampTB)[i+1].epoch - (*rampTB)[i].epoch)*GmatTimeConstants::SECS_PER_DAY;

      f0 = (*rampTB)[i].rampFrequency;
      if (dt < interval_len)
      {
         f0 = (*rampTB)[i].rampFrequency + f_dot*(interval_len - dt);
         interval_len = dt;
      }

      // Specify frequency at the end of the current interval
      f1 = f0 + f_dot*interval_len;

      // Take integral for the current interval
      value1 = ((GetFrequencyFactor(f0) + GetFrequencyFactor(f1))/2 - basedFreqFactor) * interval_len;
      value  = value + value1;

//      MessageInterface::ShowMessage("interval i = %d:    value1 = %.12lf    f0 = %.12lf  %.12lf     f1 = %.12lf   %.12lf     f_ave = %.12lfHz   width = %.12lfs \n", i, value1, f0, GetFrequencyFactor(f0), f1, GetFrequencyFactor(f1), (f0+f1)/2, interval_len);
//      MessageInterface::ShowMessage("interval i = %d: Start: epoch = %.12lf     band = %d    ramp type = %d   ramp freq = %.12le    ramp rate = %.12le\n", i,
//      (*rampTB)[i].epoch,  (*rampTB)[i].uplinkBand, (*rampTB)[i].rampType, (*rampTB)[i].rampFrequency, (*rampTB)[i].rampRate);
//      MessageInterface::ShowMessage("interval i = %d:   End: epoch = %.12lf     band = %d    ramp type = %d   ramp freq = %.12le    ramp rate = %.12le\n\n", i+1,
//      (*rampTB)[i+1].epoch,  (*rampTB)[i+1].uplinkBand, (*rampTB)[i+1].rampType, (*rampTB)[i+1].rampFrequency, (*rampTB)[i+1].rampRate);


      // Specify dt 
      dt = dt - interval_len;

      i--;
   }
   value = value + basedFreqFactor*delta_t;

//   MessageInterface::ShowMessage("value = %.12lf\n", value);

#ifdef RAP_TABLE_EXPANDABLE
   return value + correct_val;
#else
   return value;
#endif
}


