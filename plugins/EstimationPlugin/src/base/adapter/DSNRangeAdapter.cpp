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
#include "RandomNumber.hpp"


//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_DERIVATIVE_CALCULATION
//#define DEBUG_RANGE_CALCULATION

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
DSNRangeAdapter::PARAMETER_TEXT[DSNRangeAdapterParamCount - RangeAdapterKmParamCount] =
{
   "RangeModuloConstant",                   // made changes by TUAN NGUYEN
};


const Gmat::ParameterType
DSNRangeAdapter::PARAMETER_TYPE[DSNRangeAdapterParamCount - RangeAdapterKmParamCount] =
{
   Gmat::REAL_TYPE,                        // made changes by TUAN NGUYEN
};




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
   RangeAdapterKm      (name),
   rangeModulo         (1.0e18)
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
   RangeAdapterKm      (dsnr),
   rangeModulo         (dsnr.rangeModulo)
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

      rangeModulo = dsnr.rangeModulo;
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
std::string DSNRangeAdapter::GetParameterText(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < DSNRangeAdapterParamCount)
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
Integer DSNRangeAdapter::GetParameterID(const std::string& str) const
{
   for (Integer i = RangeAdapterKmParamCount; i < DSNRangeAdapterParamCount; i++)
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
Gmat::ParameterType DSNRangeAdapter::GetParameterType(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < DSNRangeAdapterParamCount)
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
std::string DSNRangeAdapter::GetParameterTypeString(const Integer id) const
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
Real DSNRangeAdapter::GetRealParameter(const Integer id) const
{
   if (id == RANGE_MODULO_CONSTANT)
      return rangeModulo;

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
Real DSNRangeAdapter::SetRealParameter(const Integer id, const Real value)
{
   if (id == RANGE_MODULO_CONSTANT)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: range modulo constant has a nonpositive value\n");

      rangeModulo = value;
      return rangeModulo;
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
Real DSNRangeAdapter::GetRealParameter(const std::string &label) const
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
Real DSNRangeAdapter::SetRealParameter(const std::string &label, const Real value)
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
      // Calculate measurement covariance again due to it uses RU.
      std::vector<SignalData*> data = calcData->GetSignalData();
      Integer measurementSize = data.size();
      measErrorCovariance.SetDimension(measurementSize);
      for (Integer i = 0; i < measurementSize; ++i)
      {
         for (Integer j = 0; j < measurementSize; ++j)
         {
            measErrorCovariance(i,j) = (i == j ?
                        (noiseSigma[2] != 0.0 ? (noiseSigma[2] * noiseSigma[2]) : 1.0) :            // noiseSigma[2] is used for DSNRange. Its unit is RU
                        0.0);
         }
      }

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
      ObservationData* forObservation, std::vector<RampTableData>* rampTable)
{
   #ifdef DEBUG_RANGE_CALCULATION
      MessageInterface::ShowMessage("DSNRangeAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
            rampTable);
   #endif

   // 1. Set ramp table and observation data for adapter before doing something
   rampTB = rampTable;
   obsData = forObservation;
   
   // 2. Compute range in km
   RangeAdapterKm::CalculateMeasurement(withEvents, forObservation, rampTB);
   
   // 3. Convert range from km to RU and store in cMeasurement:
   for (UnsignedInt i = 0; i < cMeasurement.value.size(); ++i)
   {
      // 3.1. Specify uplink frequency
      // Note that: In the current version, only one signal path is used in AdapterConfiguration. Therefore, path index is 0 
      uplinkFreq = calcData->GetUplinkFrequency(0, rampTB);
      freqBand = calcData->GetUplinkFrequencyBand(0, rampTB);
      
      // 3.2. Specify multiplier (at time t1)
      // multiplier only changes its value only after range in km is computed
      multiplier = GetFrequencyFactor(uplinkFreq*1.0e6);
      
      // 3.3. Convert range from km to RU in cMeasurement.value[0]
      Integer errnum = 0;
      Real lightspeed = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM;                  // unit: km/s
      Real realTravelTime = cMeasurement.value[i]/lightspeed;                                                     // unit: seconds
      
      if (rampTB != NULL)
      {
         // ramped frequency
         #ifdef DEBUG_RANGE_CALCULATION
            MessageInterface::ShowMessage("Calculate DSNRange based on ramped table\n");
         #endif

         try
         {
            cMeasurement.value[i] = IntegralRampedFrequency(cMeasurement.epoch, realTravelTime, errnum);                  // unit: range unit
         } catch (MeasurementException exp)
         {
            cMeasurement.value[i] = 0.0;               // It has no C-value due to the failure of calculation of IntegralRampedFrequency()
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = "R";
            if ((errnum == 2)||(errnum == 3))
               throw exp;
         }
      }
      else
      {
         // constant frequency
         #ifdef DEBUG_RANGE_CALCULATION
            MessageInterface::ShowMessage("Calculate DSNRange based on constant frequency\n");
         #endif

         cMeasurement.value[i] = multiplier*realTravelTime;
      }
      
      if (measurementType == "DSNRange")
      {
         //@Todo: write code to add bias to measuement value here
         #ifdef DEBUG_RANGE_CALCULATION
            MessageInterface::ShowMessage("      . No bias was implemented in this GMAT version.\n");
         #endif

         // Add noise to measurement value
         if (addNoise)
         {
//            MessageInterface::ShowMessage("Add noise\n");
            // Add noise here
            if (cMeasurement.unfeasibleReason != "R")
            {
//               MessageInterface::ShowMessage("create noise: c = %.12lf\n", cMeasurement.value[i]);
               RandomNumber* rn = RandomNumber::Instance();
               Real val = rn->Gaussian(cMeasurement.value[i], noiseSigma[2]);
               cMeasurement.value[i] = val;
            }
         }
      }

      cMeasurement.uplinkFreq = uplinkFreq*1.0e6;         // convert Mhz to Hz due cMeasurement.uplinkFreq's unit is Hz
      cMeasurement.uplinkBand = freqBand;
      cMeasurement.rangeModulo = rangeModulo;


      #ifdef DEBUG_RANGE_CALCULATION
         MessageInterface::ShowMessage("===================================================================\n");
         MessageInterface::ShowMessage("====  DSNRangeAdapter: Range Calculation for Measurement Data %dth  \n", i);
         MessageInterface::ShowMessage("===================================================================\n");
         MessageInterface::ShowMessage("      . Measurement type       : <%s>\n", measurementType.c_str());
         MessageInterface::ShowMessage("      . Noise adding option    : %s\n", (addNoise?"true":"false"));
         MessageInterface::ShowMessage("      . Range modulo constant  : %.12lf RU\n", rangeModulo);
         MessageInterface::ShowMessage("      . Real travel time       : %.12lf seconds\n", realTravelTime);
         MessageInterface::ShowMessage("      . Multiplier factor      : %.12lf\n", GetMultiplierFactor());
         MessageInterface::ShowMessage("      . C-value (with noise)   : %.12lf RU\n", cMeasurement.value[i]);
         MessageInterface::ShowMessage("      . Measurement epoch A1Mjd: %.12lf\n", cMeasurement.epoch); 
         MessageInterface::ShowMessage("      . Measurement is %s\n", (cMeasurement.isFeasible?"feasible":"unfeasible"));
         MessageInterface::ShowMessage("      . Feasibility reason     : %s\n", cMeasurement.unfeasibleReason.c_str());
         MessageInterface::ShowMessage("      . Elevation angle        : %.12lf degree\n", cMeasurement.feasibilityValue);
         MessageInterface::ShowMessage("===================================================================\n");
      #endif

   }

   #ifdef DEBUG_RANGE_CALCULATION
      MessageInterface::ShowMessage("DSNRangeAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) exit\n", (withEvents ? "true" : "false"), forObservation,
            rampTable);
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
   #ifdef DEBUG_DERIVATIVE_CALCULATION
      Integer parmId = GetParmIdFromEstID(id, obj);
      MessageInterface::ShowMessage("Enter DSNRangeAdapter::CalculateMeasurementDerivatives(%s, %d) called; parm ID is %d; Epoch %.12lf\n", obj->GetName().c_str(), id, parmId, cMeasurement.epoch);
   #endif

   // Compute measurement derivatives in km:
   RangeAdapterKm::CalculateMeasurementDerivatives(obj, id);

   // Convert measurement derivatives from km to RU
   Real freqFactor = multiplier/(GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM * GmatMathConstants::M_TO_KM);
   for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
   {
      for (UnsignedInt j = 0; j < theDataDerivatives[i].size(); ++j)
         theDataDerivatives[i][j] = theDataDerivatives[i][j] * freqFactor;
   }
   
   #ifdef DEBUG_DERIVATIVE_CALCULATION
      for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      {
         MessageInterface::ShowMessage("Derivative for path %dth:\n", i);
         MessageInterface::ShowMessage("[");
         for (UnsignedInt j = 0; j < theDataDerivatives[i].size(); ++j)
         {
            MessageInterface::ShowMessage("    %.12lf", theDataDerivatives[i][j]);
            MessageInterface::ShowMessage("%s", ((j == theDataDerivatives[i].size()-1)?"":","));
         }
         MessageInterface::ShowMessage("]\n");
      }
      
   #endif

   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage("Exit DSNRangeAdapter::CalculateMeasurementDerivatives():\n");
   #endif

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
