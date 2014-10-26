/$Id$
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
   targetSat            (NULL),
   prev_range_rate     (0.0)
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
   targetSat            (NULL),
   prev_range_rate     (rr.prev_range_rate)
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
   // Compute range in km, at epoch and at epoch plus offset
   cMeasurement = RangeAdapterKm::CalculateMeasurement
         (false, NULL, NULL);
   MeasurementData cMeasurement2 =
         RangeAdapterKm::CalculateMeasurementAtOffset(false, dopplerInterval,
               NULL, NULL);

   if ((cMeasurement.isFeasible) && (cMeasurement2.isFeasible))
   {
      // two way range
      Real two_way_range  = 0.0;
      Real two_way_range2 = 0.0;

      // set range rate
      Real range_rate = 0;

      // some up the ranges
      for (UnsignedInt i = 0; i < cMeasurement.value.size(); i++  )
      {
        two_way_range  = two_way_range  + cMeasurement.value[i];
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
{

    // assign current range rate
    //Real current_range_rate = cMeasurement.value[0];

    // compute delta range rate
    //Real delta_range_rate = current_range_rate - prev_range_rate; 

    // reassign prev range rate
    //Real prev_range_rate = current_range_rate;

    //MessageInterface::ShowMessage("current_range_rate %f\n", current_range_rate);

    // Compute measurement derivatives in km at epoch
  // cMeasurement = RangeAdapterKm::CalculateMeasurement(false, NULL, NULL);
   RangeAdapterKm::CalculateMeasurementDerivatives(obj, id);

   return theDataDerivatives;
}


//------------------------------------------------------------------------------
// bool WriteMeasurements()
//------------------------------------------------------------------------------
/**
 * Method to write measurements
 *
 * @todo Implement this method
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
