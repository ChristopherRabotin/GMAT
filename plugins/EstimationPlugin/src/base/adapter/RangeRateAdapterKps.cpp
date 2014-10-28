//$Id$
//------------------------------------------------------------------------------
//                           RangeRateAdapterKps
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Michael Barrucco, Thinking Systems, Inc.
// Created: Oct 1st, 2014
/**
 * A measurement adapter for rangerates in Km/s
 */
//------------------------------------------------------------------------------

#include "RangeRateAdapterKps.hpp"
#include "RandomNumber.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include <iostream>

//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_RANGE_CALCULATION
//#define DEBUG_DERIV

//#define USE_STM_DERIVATIVES


//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
RangeRateAdapterKps::PARAMETER_TEXT[RangeRateAdapterParamCount - RangeAdapterKmParamCount] =
{
   "DopplerInterval",                   
};

const Gmat::ParameterType
RangeRateAdapterKps::PARAMETER_TYPE[RangeRateAdapterParamCount - RangeAdapterKmParamCount] =
{
   Gmat::REAL_TYPE,   
};



//------------------------------------------------------------------------------
// RangeRateAdapterKps(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
RangeRateAdapterKps::RangeRateAdapterKps(const std::string& name) :
   RangeAdapterKm       (name),
   dopplerInterval      (1.0),
   prev_range_rate      (0.0),
   valueReady           (false),
   lastComputedEpoch    (0.0),
   targetSat            (NULL)
{
   typeName="RangeRate";
}


//------------------------------------------------------------------------------
// ~RangeRateAdapterKps()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RangeRateAdapterKps::~RangeRateAdapterKps()
{
}


//------------------------------------------------------------------------------
// RangeRateAdapterKps(const RangeRateAdapterKps& rr)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rr The adapter copied to make this one
 */
//------------------------------------------------------------------------------
RangeRateAdapterKps::RangeRateAdapterKps(const RangeRateAdapterKps& rr) :
   RangeAdapterKm       (rr),
   dopplerInterval      (rr.dopplerInterval),
   prev_range_rate      (rr.prev_range_rate),
   valueReady           (rr.valueReady),
   lastComputedEpoch    (rr.lastComputedEpoch),
   targetSat            (NULL)
{
}


//------------------------------------------------------------------------------
// RangeRateAdapterKps& operator=(const RangeRateAdapterKps& rr)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rr The adapter copied to make this one match it
 *
 * @return This adapter made to look like rr
 */
//------------------------------------------------------------------------------
RangeRateAdapterKps& RangeRateAdapterKps::operator=(const RangeRateAdapterKps& rr)
{
   if (this != &rr)
   {
      RangeAdapterKm::operator=(rr);
      dopplerInterval = rr.dopplerInterval;
      targetSat = NULL;
      valueReady = rr.valueReady;
      lastComputedEpoch = rr.lastComputedEpoch;
      prev_range_rate = rr.prev_range_rate;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new adapter that matches this one
 *
 * @return A new adapter set to match this one
 */
//------------------------------------------------------------------------------
GmatBase* RangeRateAdapterKps::Clone() const
{
   return new RangeRateAdapterKps(*this);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the script name for the parameter
 *
 * @param id The id of the parameter
 *
 * @return The script name
 */
//------------------------------------------------------------------------------
std::string RangeRateAdapterKps::GetParameterText(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < RangeRateAdapterParamCount)
      return PARAMETER_TEXT[id - RangeAdapterKmParamCount];
   return RangeAdapterKm::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a scriptable parameter
 *
 * @param str The string used for the parameter
 *
 * @return The parameter ID
 */
//------------------------------------------------------------------------------
Integer RangeRateAdapterKps::GetParameterID(const std::string& str) const
{
   for (Integer i = RangeAdapterKmParamCount; i < RangeRateAdapterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - RangeAdapterKmParamCount])
         return i;
   }
   return RangeAdapterKm::GetParameterID(str);
}



//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type for a specified parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType RangeRateAdapterKps::GetParameterType(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < RangeRateAdapterParamCount)
      return PARAMETER_TYPE[id - RangeAdapterKmParamCount];

   return RangeAdapterKm::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a text description for a parameter
 *
 * @param id The ID for the parameter
 *
 * @return The description string
 */
//------------------------------------------------------------------------------
std::string RangeRateAdapterKps::GetParameterTypeString(const Integer id) const
{
   return MeasurementModelBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// std::string GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param id The ID for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real RangeRateAdapterKps::GetRealParameter(const Integer id) const
{
   if (id == DOPPLER_INTERVAL)
      return dopplerInterval;

   return RangeAdapterKm::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real RangeRateAdapterKps::SetRealParameter(const Integer id, const Real value)
{
   if (id == DOPPLER_INTERVAL)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: Doppler interval must be a "
               "positive value\n");

      dopplerInterval = value;
      return dopplerInterval;
   }

   return RangeAdapterKm::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param label The name for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real RangeRateAdapterKps::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real RangeRateAdapterKps::SetRealParameter(const std::string &label,
      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type, const std::string& oldName,
//       const std::string& newName)
//------------------------------------------------------------------------------
/**
 * Method used to rename reference objects
 *
 * @param type The type of the object that is renamed
 * @param oldName The old object name
 * @param newName The new name
 *
 * @return true if a rename happened, false if not
 */
//------------------------------------------------------------------------------
bool RangeRateAdapterKps::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = RangeAdapterKm::RenameRefObject(type, oldName, newName);

   // Handle additional renames specific to this adapter (currently none)

   return retval;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the adapter for use
 *
 * @return true if the initialization succeeds, false if it fails
 */
//------------------------------------------------------------------------------
bool RangeRateAdapterKps::Initialize()
{
   bool retval = false;

   if (RangeAdapterKm::Initialize())
   {
      // Handle additional initialization specific to this adapter (currently none)
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// const MeasurementData& CalculateMeasurement(bool withEvents,
//       ObservationData* forObservation, std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------
/**
 * Computes the measurement associated with this adapter
 *
 * @note: The parameters associated with this call will probably be removed;
 * they are here to support compatibility with the old measurement models
 *
 * @param withEvents Flag indicating is the light time solution should be
 *                   included
 * @param forObservation The observation data associated with this measurement
 * @param rampTB Ramp table for a ramped measurement
 *
 * @return The computed measurement data
 */
//------------------------------------------------------------------------------
const MeasurementData& RangeRateAdapterKps::CalculateMeasurement(
                              bool withEvents, ObservationData* forObservation,
                              std::vector<RampTableData>* rampTB)
{
   strandData[0].clear();
   strandData[1].clear();

   // Compute range in km, at epoch and at epoch plus offset
   cMeasurement1 = RangeAdapterKm::CalculateMeasurement
         (false, NULL, NULL);
   SignalData* sigData = (calcData->GetSignalData())[0];
   while (sigData)
   {
      SignalData dat(*sigData);
      strandData[0].push_back(dat);
      sigData = sigData->next;
   }

   cMeasurement2 =
         RangeAdapterKm::CalculateMeasurementAtOffset(false, dopplerInterval,
               NULL, NULL);

   sigData = (calcData->GetSignalData())[0];
   while (sigData)
   {
      SignalData dat(*sigData);
      strandData[1].push_back(dat);
      sigData = sigData->next;
   }

   if ((cMeasurement1.isFeasible) && (cMeasurement2.isFeasible))
   {
      // two way range
      Real two_way_range  = 0.0;
      Real two_way_range2 = 0.0;

      // set range rate
      Real range_rate = 0;

      // some up the ranges
      for (UnsignedInt i = 0; i < cMeasurement1.value.size(); i++  )
      {
        two_way_range  = two_way_range  + cMeasurement1.value[i];
        two_way_range2 = two_way_range2 + cMeasurement2.value[i];
      }

      // one way range
      Real one_way_range  = two_way_range  / 2.0;
      Real one_way_range2 = two_way_range2 / 2.0;

      // Compute range-rate
      range_rate = (one_way_range2-one_way_range)/(dopplerInterval);

      // set the measurement value
      cMeasurement.value.clear();
      cMeasurement.value.push_back(range_rate);

      #ifdef DEBUG_RANGE_CALCULATION
        MessageInterface::ShowMessage("epoch %f, range %f, range rate %f, "
              "isFeasible %s, range2 %f, dRange %le\n", cMeasurement.epoch,
              one_way_range, range_rate,
              (cMeasurement.isFeasible ? "true" : "false"), one_way_range2,
              one_way_range - one_way_range2);
      #endif
   }

//if (cMeasurement.isFeasible)
//   MessageInterface::ShowMessage("%.12lf %.12lf ", cMeasurement.epoch, cMeasurement.value[0]);

   return cMeasurement;
}


//------------------------------------------------------------------------------
// const std::vector<RealArray>& CalculateMeasurementDerivatives(GmatBase* obj,
//       Integer id)
//------------------------------------------------------------------------------
/**
 * Computes measurement derivatives for a given parameter on a given object
 *
 * @param obj The object that has the w.r.t. parameter
 * @param id  The ID of the w.r.t. parameter
 *
 * @return The derivative vector
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& RangeRateAdapterKps::
            CalculateMeasurementDerivatives(GmatBase* obj, Integer id)
#ifdef USE_STM_DERIVATIVES
{
   if (!calcData)
      throw MeasurementException("Measurement derivative data was requested "
            "for " + instanceName + " before the measurement was set");

   // Perform the calculations
   #ifdef DEBUG_ADAPTER_DERIVATIVES
      Integer parmId = GetParmIdFromEstID(id, obj);
      MessageInterface::ShowMessage("RangeAdapterKm::CalculateMeasurement"
            "Derivatives(%s, %d) called; parm ID is %d; Epoch %.12lf\n",
            obj->GetName().c_str(), id, parmId, cMeasurement.epoch);
   #endif

   Integer parameterID = -1;
   UnsignedInt size = 6;


   if (obj != NULL)
   {
      if (id > 250)
         parameterID = GetParmIdFromEstID(id, obj);
      else
         parameterID = id;

   if (obj->GetParameterText(parameterID) == "Position")
   {
//      RealArray oneRow;
//      oneRow.assign(3, 0.0);
//      theDataDerivatives.clear();
//      theDataDerivatives.push_back(oneRow);
//
//      SignalData *upStrand = data[0];
//      SignalData *downStrand = data[0]->next;
//
//      Rmatrix33 I33(true);
//
//      if ((downStrand == NULL) || (downStrand->next != NULL))
//         throw MeasurementException("Range rate computations require signal "
//               "path of the form T1 -> S1 -> T1");
//
//      if (obj->IsOfType(Gmat::GROUND_STATION))
//         throw MeasurementException("Derivatives w.r.t. Station location "
//               "parameters are not yet supported");
//      else
//      {
//         Real upRange = upStrand->rangeVecInertial.GetMagnitude();
//         Real upRangeCubed = upRange * upRange * upRange;
//         Real upRDotV =
//               upStrand->rangeVecInertial(0) * upStrand->rangeRateVecInertial(0) +
//               upStrand->rangeVecInertial(1) * upStrand->rangeRateVecInertial(1) +
//               upStrand->rangeVecInertial(2) * upStrand->rangeRateVecInertial(2);
//
//         Real downRange = downStrand->rangeVecInertial.GetMagnitude();
//         Real downRangeCubed = downRange * downRange * downRange;
//         Real downRDotV =
//               downStrand->rangeVecInertial(0) * downStrand->rangeRateVecInertial(0) +
//               downStrand->rangeVecInertial(1) * downStrand->rangeRateVecInertial(1) +
//               downStrand->rangeVecInertial(2) * downStrand->rangeRateVecInertial(2);
//
//         for (UnsignedInt ii = 0; ii < 3; ++ii)
//         {
//            theDataDerivatives[0][ii]   = 0.5 * (
//                  (upStrand->rangeRateVecInertial(ii)   / upRange   -
//                   upStrand->rangeVecInertial(ii)   * upRDotV   / upRangeCubed) -
//                  (downStrand->rangeRateVecInertial(ii) / downRange -
//                   downStrand->rangeVecInertial(ii) * downRDotV / downRangeCubed));
//         }
//      }
   }
   else if (obj->GetParameterText(parameterID) == "Velocity")
   {
//      RealArray oneRow;
//      oneRow.assign(3, 0.0);
//      theDataDerivatives.clear();
//      theDataDerivatives.push_back(oneRow);
//
//      SignalData *upStrand = data[0];
//      SignalData *downStrand = data[0]->next;
//
//      if ((downStrand == NULL) || (downStrand->next != NULL))
//         throw MeasurementException("Range rate computations require signal "
//               "path of the form T1 -> S1 -> T1");
//
//      if (obj->IsOfType(Gmat::GROUND_STATION))
//         throw MeasurementException("Derivatives w.r.t. Station location "
//               "parameters are not yet supported");
//      else
//      {
//         Real upRange = upStrand->rangeVecInertial.GetMagnitude();
//         Real downRange = downStrand->rangeVecInertial.GetMagnitude();
//         for (UnsignedInt ii = 0; ii < 3; ++ii)
//         {
//            theDataDerivatives[0][ii]   = 0.5 *
//                  (upStrand->rangeVecInertial(ii)   / upRange -
//                   downStrand->rangeVecInertial(ii) / downRange);
//         }
//      }
   }
   else if (obj->GetParameterText(parameterID) == "CartesianX")
   {
      RealArray oneRow;
      oneRow.assign(6, 0.0);
      theDataDerivatives.clear();
      theDataDerivatives.push_back(oneRow);

      RealArray leg1Derivatives, leg2Derivatives;
      if (strandData[0].size() == 2)
      {
         RealArray sigDv;
         CalculateCartesianDerivative(&(strandData[0][0]), leg1Derivatives, false);
         CalculateCartesianDerivative(&(strandData[0][1]), sigDv, true);
         for (UnsignedInt i = 0; i < leg1Derivatives.size(); ++i)
         {
            leg1Derivatives[i] += sigDv[i];
            leg1Derivatives[i] *= 0.5;
         }
      }
      else
         throw MeasurementException("The 2-way range rate measurement is "
               "improperly sized");
      if (strandData[1].size() == 2)
      {
         RealArray sigDv;
         CalculateCartesianDerivative(&(strandData[1][0]), leg2Derivatives, false);
         CalculateCartesianDerivative(&(strandData[1][1]), sigDv, true);
         for (UnsignedInt i = 0; i < leg2Derivatives.size(); ++i)
         {
            leg2Derivatives[i] += sigDv[i];
            leg2Derivatives[i] *= 0.5;
         }
      }
      else
         throw MeasurementException("The 2-way range rate measurement is "
               "improperly sized");

      // Now put the two strands together
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         theDataDerivatives[0][i] = (leg2Derivatives[i] - leg1Derivatives[i]) /
               dopplerInterval;
      }

      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   Range [%.12lf  %.12f] Range Vec "
               "[%s] [%s] Range rate [%s] [%s] Range Vec: [%s] [%s] "
               "Range rate [%s] [%s]\n", upRange, downRange,
               upStrand->rangeVecInertial.ToString(15).c_str(),
               downStrand->rangeVecInertial.ToString(15).c_str(),
               upStrand->rangeRateVecInertial.ToString(15).c_str(),
               downStrand->rangeRateVecInertial.ToString(15).c_str(),
               upStrand->rangeVecInertial.ToString(15).c_str(),
               downStrand->rangeVecInertial.ToString(15).c_str(),
               upStrand->rangeRateVecInertial.ToString(15).c_str(),
               downStrand->rangeRateVecInertial.ToString(15).c_str());
      #endif
   }
   else if (obj->GetParameterText(parameterID) == "Bias")
   {
      RealArray oneRow;
      oneRow.assign(1, 0.0);
      theDataDerivatives.clear();
      theDataDerivatives.push_back(oneRow);
      theDataDerivatives[0][0] = 1.0;
   }
   else
   {
      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   Deriv is w.r.t. something "
                  "independent, so zero\n");
      #endif
      for (UnsignedInt i = 0; i < 3; ++i)
         theDataDerivatives[0][i] += 0.0;
   }
}


   return theDataDerivatives;
}
#else
{

    // assign current range rate
    Real current_range_rate = cMeasurement.value[0];

    // compute delta range rate
    Real delta_range_rate = (current_range_rate - prev_range_rate);

    // reassign prev range rate
    prev_range_rate = current_range_rate;

    //MessageInterface::ShowMessage("current_range_rate %f\n", current_range_rate);

    std::vector<SignalData*> data = calcData->GetSignalData();

    // Set pointers to the uplink and downlink data for convenience
    SignalData* upData = data[0];
    SignalData* downData = data[0]->next;

    // Epoch doesn't matter here -- for spacecraft, the call retrieves the current state from the spacecraft
    Rvector6 startState = targetSat->GetMJ2000State(0.0);


    // STM for the whole signal path = stm up * stm down, both for the spacecraft
    Rmatrix66 propStm = upData->rSTM * downData->tSTM;

    // Use it to propagate the spacecraft
    Rvector6 endState = propStm * startState;

    //compute derivatives
    RealArray oneRow;
    oneRow.assign(6, 0.0);
    theDataDerivatives.clear();
    theDataDerivatives.push_back(oneRow);

    for (UnsignedInt i = 0; i <6; ++i)
    {
        Real delta_state = (endState[i] - startState[i])/1000;
        Real current_deriv = delta_range_rate / delta_state;
        MessageInterface::ShowMessage("delta cartesian state %f\n", delta_state);
        MessageInterface::ShowMessage("End state [%f %f %f] [%f %f %f]\n", endState[0], endState[1],endState[2], endState[3], endState[4], endState[5]);
        MessageInterface::ShowMessage("Start state [%f %f %f] [%f %f %f]\n", startState[0], startState[1],startState[2], startState[3], startState[4], startState[5]);
        MessageInterface::ShowMessage("current deriv %f\n", current_deriv);
        MessageInterface::ShowMessage("i %i\n", i);

        theDataDerivatives[0][i]=current_deriv;

    }

   return theDataDerivatives;
}
#endif

//------------------------------------------------------------------------------
// bool WriteMeasurements()
//------------------------------------------------------------------------------
/**
 * Method to write measurements
 *
 * @todo

 Implement this method
 *
 * @return true if written, false if not
 */
//------------------------------------------------------------------------------
bool RangeRateAdapterKps::WriteMeasurements()
{
   bool retval = false;
   return retval;
}


//------------------------------------------------------------------------------
// bool WriteMeasurement(Integer id)
//------------------------------------------------------------------------------
/**
 * Method to write a specific measurement
 *
 * @todo Implement this method
 *
 * @param id ID of the parameter to write
 *
 * @return true if written, false if not
 */
//------------------------------------------------------------------------------
bool RangeRateAdapterKps::WriteMeasurement(Integer id)
{
   bool retval = false;
   return retval;
}


//------------------------------------------------------------------------------
// Integer HasParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
/**
 * Checks to see if adapter has covariance data for a specified parameter ID
 *
 * @param paramID The parameter ID
 *
 * @return Size of the covariance data that is available
 */
//------------------------------------------------------------------------------
Integer RangeRateAdapterKps::HasParameterCovariances(Integer parameterId)
{
   Integer retval = 0;
   return retval;
}


//------------------------------------------------------------------------------
// Integer GetEventCount()
//------------------------------------------------------------------------------
/**
 * Returns the number of light time solution events in the measurement
 *
 * @return The event count
 */
//------------------------------------------------------------------------------
Integer RangeRateAdapterKps::GetEventCount()
{
   Integer retval = 0;
   return retval;
}


//------------------------------------------------------------------------------
// void SetCorrection(const std::string& correctionName,
//       const std::string& correctionType)
//------------------------------------------------------------------------------
/**
 * Passes a correction name into the owned CoreMeasurement
 *
 * @param correctionName The name of the correction
 * @param correctionType The type of correction
 *
 * @note This information originally was not passed via SetStringParameter
 *       because it wasn't managed by scripting on MeasurementModels.  It was
 *       managed in the TrackingSystem code.  Since it is now scripted on the
 *       measurement -- meaning on the adapter -- this code should move into the
 *       Get/SetStringParameter methods.  It is included here !!TEMPORARILY!!
 *       to get a scripted adapter functioning in GMAT Nav.
 */
//------------------------------------------------------------------------------
void RangeRateAdapterKps::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   TrackingDataAdapter::SetCorrection(correctionName, correctionType);            // made changes by TUAN NGUYEN
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
//       const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets reference objects: overridden from base class to facilitate epoch
 * generation
 *
 * @param obj The reference object
 * @param type The object type
 * @param name The name of the object
 *
 * @return true is the pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool RangeRateAdapterKps::SetRefObject(GmatBase* obj,
      const Gmat::ObjectType type, const std::string& name)
{
   if (obj->IsOfType(Gmat::SPACECRAFT))
      targetSat = (SpaceObject*) obj;
   return RangeAdapterKm::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
//       const std::string& name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets reference objects: overridden from base class to facilitate epoch
 * generation
 *
 * @param obj The reference object
 * @param type The object type
 * @param name The name of the object
 * @param index Index into the array of objects that receive this one
 *
 * @return true is the pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool RangeRateAdapterKps::SetRefObject(GmatBase* obj,
      const Gmat::ObjectType type, const std::string& name, const Integer index)
{
   if (obj->IsOfType(Gmat::SPACECRAFT))
      targetSat = (SpaceObject*) obj;
   return RangeAdapterKm::SetRefObject(obj, type, name, index);
}

//------------------------------------------------------------------------------
// void RangeRateAdapterKps::CalculateCartesianDerivative(SignalData* sigData,
//       RealArray toArray)
//------------------------------------------------------------------------------
/**
 * Computes the range derivative w.r.t. Cartesian state
 *
 * @param sigData The signal data for the computation
 * @param toArray The array receiving the derivative; it should have size 0 on
 *                input
 * @param forTransmitter True is the derivative is w.r.t. transmitter state
 */
//------------------------------------------------------------------------------
void RangeRateAdapterKps::CalculateCartesianDerivative(SignalData* sigData,
      RealArray &toArray, bool forTransmitter)
{
   toArray.clear();
   CalculatePositionDerivative(sigData, toArray, forTransmitter);
   RealArray vData;
   CalculateVelocityDerivative(sigData, vData, forTransmitter);
   if (vData.size() == 3)
      for (UnsignedInt i = 0; i < 3; ++i)
         toArray.push_back(vData[i]);
}

//------------------------------------------------------------------------------
// void RangeRateAdapterKps::CalculatePositionDerivative(SignalData* sigData,
//       RealArray& toArray)
//------------------------------------------------------------------------------
/**
 * Computes the range derivative w.r.t. position
 *
 * Note:  Current implementation is for the inertial frame
 *
 * @param sigData The signal data for the computation
 * @param toArray The array receiving the derivative; it should have size 0 on
 *                input
 * @param forTransmitter True is the derivative is w.r.t. transmitter state
 */
//------------------------------------------------------------------------------
void RangeRateAdapterKps::CalculatePositionDerivative(SignalData* sigData,
      RealArray& toArray, bool forTransmitter)
{
   Rmatrix33 A;
   Real range = sigData->rangeVecInertial.GetMagnitude();
   Rvector3 deriv;

   if (forTransmitter)
   {
      A = sigData->tSTM.UpperLeft();
      for (UnsignedInt i = 0; i < 3; ++i)
         deriv(i) = - sigData->rangeVecInertial(0) + A(i,0) +
                      sigData->rangeVecInertial(1) + A(i,1) +
                      sigData->rangeVecInertial(2) + A(i,2);
   }
   else
   {
      A = sigData->rSTM.UpperLeft();
      for (UnsignedInt i = 0; i < 3; ++i)
         deriv(i) = sigData->rangeVecInertial(0) + A(i,0) +
                    sigData->rangeVecInertial(1) + A(i,1) +
                    sigData->rangeVecInertial(2) + A(i,2);
   }

   for (UnsignedInt i = 0; i < 3; ++i)
      toArray.push_back(deriv[i] / range);
}

//------------------------------------------------------------------------------
// void RangeRateAdapterKps::CalculateVelocityDerivative(SignalData* sigData,
//       RealArray& toArray)
//------------------------------------------------------------------------------
/**
 * Computes the range derivative w.r.t. velocity
 *
 * @param sigData The signal data for the computation
 * @param toArray The array receiving the derivative; it should have size 0 on
 *                input
 * @param forTransmitter True is the derivative is w.r.t. transmitter state
 */
//------------------------------------------------------------------------------
void RangeRateAdapterKps::CalculateVelocityDerivative(SignalData* sigData,
      RealArray& toArray, bool forTransmitter)
{
   Rmatrix33 B;
   Real range = sigData->rangeVecInertial.GetMagnitude();
   Rvector3 deriv;

   if (forTransmitter)
   {
      B = sigData->tSTM.UpperRight();
      for (UnsignedInt i = 0; i < 3; ++i)
         deriv(i) = - sigData->rangeVecInertial(0) + B(i,0) +
                      sigData->rangeVecInertial(1) + B(i,1) +
                      sigData->rangeVecInertial(2) + B(i,2);
   }
   else
   {
      B = sigData->rSTM.UpperRight();
      for (UnsignedInt i = 0; i < 3; ++i)
         deriv(i) = sigData->rangeVecInertial(0) + B(i,0) +
                    sigData->rangeVecInertial(1) + B(i,1) +
                    sigData->rangeVecInertial(2) + B(i,2);
   }

   for (UnsignedInt i = 0; i < 3; ++i)
      toArray.push_back(deriv[i] / range);
}
