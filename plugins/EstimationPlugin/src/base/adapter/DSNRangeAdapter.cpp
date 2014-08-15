//$Id$
//------------------------------------------------------------------------------
//                           DSNRangeAdapter
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
// Created: Aug 13, 2014
/**
 * A measurement adapter for DSN ranges in Range Unit (RU)
 */
//------------------------------------------------------------------------------

#include "DSNRangeAdapter.hpp"
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
DSNRangeAdapter::DSNRangeAdapter(const std::string& name) :
   RangeAdapterKm      (name)
{
   typeName = "DSNRange";              // change type name from "RangeKm" to "DSNRange"
}


//------------------------------------------------------------------------------
// ~DSNRangeAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DSNRangeAdapter::~DSNRangeAdapter()
{
}


//------------------------------------------------------------------------------
// DSNRangeAdapter(const DSNRangeAdapter& dsnr)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param dsnr The adapter copied to make this one
 */
//------------------------------------------------------------------------------
DSNRangeAdapter::DSNRangeAdapter(const DSNRangeAdapter& dsnr) :
   RangeAdapterKm      (dsnr)
{
}


//------------------------------------------------------------------------------
// DSNRangeAdapter& operator=(const DSNRangeAdapter& dsnr)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param dsnr The adapter copied to make this one match it
 *
 * @return This adapter made to look like rak
 */
//------------------------------------------------------------------------------
DSNRangeAdapter& DSNRangeAdapter::operator=(const DSNRangeAdapter& dsnr)
{
   if (this != &dsnr)
   {
      RangeAdapterKm::operator=(dsnr);
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
GmatBase* DSNRangeAdapter::Clone() const
{
   return new DSNRangeAdapter(*this);
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
bool DSNRangeAdapter::RenameRefObject(const Gmat::ObjectType type,
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
bool DSNRangeAdapter::Initialize()
{
   bool retval = false;

   if (RangeAdapterKm::Initialize())
   {
      retval = true;

      // @todo: initialize all needed variables
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
const MeasurementData& DSNRangeAdapter::CalculateMeasurement(bool withEvents,
      ObservationData* forObservation, std::vector<RampTableData>* rampTB)
{
   #ifdef DEBUG_RANGE_CALCULATION
      MessageInterface::ShowMessage("DSNRangeAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
            rampTB);
   #endif

   // Compute range in km
   RangeAdapterKm::CalculateMeasurement(withEvents, forObservation, rampTB);

   // Convert range from km to RU and store in cMeasurement here:


   #ifdef DEBUG_RANGE_CALCULATION
      MessageInterface::ShowMessage("DSNRangeAdapter::CalculateMeasurement(%s, "
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
const std::vector<RealArray>& DSNRangeAdapter::CalculateMeasurementDerivatives(
      GmatBase* obj, Integer id)
{
   // Compute measurement derivatives in km:
   RangeAdapterKm::CalculateMeasurementDerivatives(obj, id);

   // @todo: Add code to convert measurement derivatives from km to RU


   //if (!calcData)
   //   throw MeasurementException("Measurement derivative data was requested "
   //         "for " + instanceName + " before the measurement was set");

   //Integer parmId = GetParmIdFromEstID(id, obj);

   //// Perform the calculations
   //#ifdef DEBUG_ADAPTER_DERIVATIVES
   //   MessageInterface::ShowMessage("RangeAdapterKm::CalculateMeasurement"
   //         "Derivatives(%s, %d) called; parm ID is %d; Epoch %.12lf\n",
   //         obj->GetName().c_str(), id, parmId, cMeasurement.epoch);
   //#endif

   //const std::vector<RealArray> *derivativeData =
   //      &(calcData->CalculateMeasurementDerivatives(obj, id)); // parmId));

   //#ifdef DEBUG_ADAPTER_DERIVATIVES
   //   MessageInterface::ShowMessage("   Derivatives: [");
   //   for (UnsignedInt i = 0; i < derivativeData->size(); ++i)
   //   {
   //      if (i > 0)
   //         MessageInterface::ShowMessage("]\n                [");
   //      for (UnsignedInt j = 0; j < derivativeData->at(i).size(); ++j)
   //      {
   //         if (j > 0)
   //            MessageInterface::ShowMessage(", ");
   //         MessageInterface::ShowMessage("%.12le", (derivativeData->at(i))[j]);
   //      }
   //   }
   //   MessageInterface::ShowMessage("]\n");
   //#endif

   //// Now assemble the derivative data into the requested derivative
   //UnsignedInt size = derivativeData->at(0).size();

   //theDataDerivatives.clear();
   //for (UnsignedInt i = 0; i < derivativeData->size(); ++i)
   //{
   //   RealArray oneRow;
   //   oneRow.assign(size, 0.0);
   //   theDataDerivatives.push_back(oneRow);

   //   if (derivativeData->at(i).size() != size)
   //      throw MeasurementException("Derivative data size is a different size "
   //            "than expected");

   //   for (UnsignedInt j = 0; j < size; ++j)
   //   {
   //      theDataDerivatives[i][j] = (derivativeData->at(i))[j];
   //   }
   //}

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
bool DSNRangeAdapter::WriteMeasurements()
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
bool DSNRangeAdapter::WriteMeasurement(Integer id)
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
Integer DSNRangeAdapter::HasParameterCovariances(Integer parameterId)
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
Integer DSNRangeAdapter::GetEventCount()
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
void DSNRangeAdapter::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   RangeAdapterKm::SetCorrection(correctionName, correctionType);
}
