//$Id$
//------------------------------------------------------------------------------
//                           USNRangeAdapter
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
// Author: Tuan Dang Nguyen, NASA/GSFC
// Created: Aug 28, 2014
/**
 * A measurement adapter for USN ranges
 */
//------------------------------------------------------------------------------

#include "USNRangeAdapter.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_RANGE_CALCULATION

//------------------------------------------------------------------------------
// DSNRangeAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
USNRangeAdapter::USNRangeAdapter(const std::string& name) :
   RangeAdapterKm      (name)
{
   typeName = "USNRange";              // change type name from "RangeKm" to "USNRange"
}


//------------------------------------------------------------------------------
// ~DSNRangeAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
USNRangeAdapter::~USNRangeAdapter()
{
}


//------------------------------------------------------------------------------
// USNRangeAdapter(const USNRangeAdapter& usnr)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param usnr The adapter copied to make this one
 */
//------------------------------------------------------------------------------
USNRangeAdapter::USNRangeAdapter(const USNRangeAdapter& usnr) :
   RangeAdapterKm      (usnr)
{
}


//------------------------------------------------------------------------------
// USNRangeAdapter& operator=(const USNRangeAdapter& usnr)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param usnr The adapter copied to make this one match it
 *
 * @return This adapter made to look like rak
 */
//------------------------------------------------------------------------------
USNRangeAdapter& USNRangeAdapter::operator=(const USNRangeAdapter& usnr)
{
   if (this != &usnr)
   {
      RangeAdapterKm::operator=(usnr);
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
GmatBase* USNRangeAdapter::Clone() const
{
   return new USNRangeAdapter(*this);
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
bool USNRangeAdapter::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = RangeAdapterKm::RenameRefObject(type, oldName, newName);

   // Handle additional renames specific to this adapter

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
bool USNRangeAdapter::Initialize()
{
   bool retval = false;

   if (RangeAdapterKm::Initialize())
   {
      retval = true;

      // @todo: initialize all needed variables
      SetMultiplierFactor(0.5);
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
const MeasurementData& USNRangeAdapter::CalculateMeasurement(bool withEvents,
      ObservationData* forObservation, std::vector<RampTableData>* rampTB)
{
   #ifdef DEBUG_ADAPTER_EXECUTION
      MessageInterface::ShowMessage("USNRangeAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
            rampTB);
   #endif

   // Compute range in km
   RangeAdapterKm::CalculateMeasurement(withEvents, forObservation, rampTB);

   // Convert range in km from measurement type "Range" to "USNRange":
   for (UnsignedInt i = 0; i < cMeasurement.value.size(); ++i)
   {
      Real realRange = cMeasurement.value[i];
      cMeasurement.value[i] = realRange * GetMultiplierFactor();

      #ifdef DEBUG_RANGE_CALCULATION
         MessageInterface::ShowMessage("===================================================================\n");
         MessageInterface::ShowMessage("====  USNRangeAdapter: Range Calculation for Measurement Data %dth  \n", i);
         MessageInterface::ShowMessage("===================================================================\n");
         MessageInterface::ShowMessage("      . Measurement type        : <%s>\n", measurementType.c_str());
         MessageInterface::ShowMessage("      . Noise adding option     : %s\n", (addNoise?"true":"false"));
         MessageInterface::ShowMessage("      . Real range              : %.12lf km\n", realRange);
         MessageInterface::ShowMessage("      . Multiplier factor       : %.12lf\n", GetMultiplierFactor());
         MessageInterface::ShowMessage("      . C-value[%d] (with noise): %.12lf km\n", i, cMeasurement.value[i]);
         MessageInterface::ShowMessage("      . Measurement epoch A1Mjd : %.12lf\n", cMeasurement.epoch); 
         MessageInterface::ShowMessage("      . Measurement is %s\n", (cMeasurement.isFeasible?"feasible":"unfeasible"));
         MessageInterface::ShowMessage("      . Feasibility reason      : %s\n", cMeasurement.unfeasibleReason.c_str());
         MessageInterface::ShowMessage("      . Elevation angle         : %.12lf degree\n", cMeasurement.feasibilityValue);
         MessageInterface::ShowMessage("===================================================================\n");
      #endif
   }

   #ifdef DEBUG_ADAPTER_EXECUTION
      MessageInterface::ShowMessage("USNRangeAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) exit\n", (withEvents ? "true" : "false"), forObservation,
            rampTB);
   #endif

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
const std::vector<RealArray>& USNRangeAdapter::CalculateMeasurementDerivatives(
      GmatBase* obj, Integer id)
{
   // Compute measurement derivatives in km:
   RangeAdapterKm::CalculateMeasurementDerivatives(obj, id);

   // Convert measurement derivatives from type "Range" to type "USNRange"
   for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
   {

      for ( UnsignedInt j = 0; j < theDataDerivatives[i].size(); ++j)
      {
         theDataDerivatives[i][j] = theDataDerivatives[i][j] * multiplier;
      }
   }

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
bool USNRangeAdapter::WriteMeasurements()
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
bool USNRangeAdapter::WriteMeasurement(Integer id)
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
Integer USNRangeAdapter::HasParameterCovariances(Integer parameterId)
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
Integer USNRangeAdapter::GetEventCount()
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
void USNRangeAdapter::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   RangeAdapterKm::SetCorrection(correctionName, correctionType);
}

