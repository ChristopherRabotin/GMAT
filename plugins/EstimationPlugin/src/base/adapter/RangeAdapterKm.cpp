//$Id$
//------------------------------------------------------------------------------
//                           RangeAdapterKm
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 12, 2014
/**
 * A measurement adapter for ranges in Km
 */
//------------------------------------------------------------------------------

#include "RangeAdapterKm.hpp"
#include "RandomNumber.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "SignalBase.hpp"
#include <sstream>


//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_RANGE_CALCULATION

//------------------------------------------------------------------------------
// RangeAdapterKm(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
RangeAdapterKm::RangeAdapterKm(const std::string& name) :
   TrackingDataAdapter      ("RangeKm", name)
{
}


//------------------------------------------------------------------------------
// ~RangeAdapterKm()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RangeAdapterKm::~RangeAdapterKm()
{
}


//------------------------------------------------------------------------------
// RangeAdapterKm(const RangeAdapterKm& rak)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rak The adapter copied to make this one
 */
//------------------------------------------------------------------------------
RangeAdapterKm::RangeAdapterKm(const RangeAdapterKm& rak) :
   TrackingDataAdapter      (rak)
{
}


//------------------------------------------------------------------------------
// RangeAdapterKm& operator=(const RangeAdapterKm& rak)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rak The adapter copied to make this one match it
 *
 * @return This adapter made to look like rak
 */
//------------------------------------------------------------------------------
RangeAdapterKm& RangeAdapterKm::operator=(const RangeAdapterKm& rak)
{
   if (this != &rak)
   {
      TrackingDataAdapter::operator=(rak);
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
GmatBase* RangeAdapterKm::Clone() const
{
   return new RangeAdapterKm(*this);
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
std::string RangeAdapterKm::GetParameterText(const Integer id) const
{
   if (id >= AdapterParamCount && id < RangeAdapterKmParamCount)
      return PARAMETER_TEXT[id - AdapterParamCount];
   return TrackingDataAdapter::GetParameterText(id);
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
Integer RangeAdapterKm::GetParameterID(const std::string& str) const
{
   for (Integer i = AdapterParamCount; i < RangeAdapterKmParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - AdapterParamCount])
         return i;
   }
   return TrackingDataAdapter::GetParameterID(str);
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
Gmat::ParameterType RangeAdapterKm::GetParameterType(const Integer id) const
{
   if (id >= AdapterParamCount && id < RangeAdapterKmParamCount)
      return PARAMETER_TYPE[id - AdapterParamCount];

   return TrackingDataAdapter::GetParameterType(id);
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
std::string RangeAdapterKm::GetParameterTypeString(const Integer id) const
{
   return MeasurementModelBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
bool RangeAdapterKm::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = TrackingDataAdapter::RenameRefObject(type, oldName, newName);

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
bool RangeAdapterKm::Initialize()
{
   bool retval = false;

   if (TrackingDataAdapter::Initialize())
   {
      retval = true;

      if (participantLists.size() > 1)
         MessageInterface::ShowMessage("Warning: .gmd files do not support "
               "multiple strands\n");

      for (UnsignedInt i = 0; i < participantLists.size(); ++i)
      {
         for (UnsignedInt j = 0; j < participantLists[i]->size(); ++j)
         {
            std::string theId;

            for (UnsignedInt k = 0; k < refObjects.size(); ++k)
            {
               if (refObjects[k]->GetName() == participantLists[i]->at(j))
               {
                  theId = refObjects[k]->GetStringParameter("Id");
                  break;
               }
            }
            cMeasurement.participantIDs.push_back(theId);
         }
      }
   }

   
   // Calculate measurement covariance
   std::vector<SignalData*> data = calcData->GetSignalData();
   Integer measurementSize = data.size();
   measErrorCovariance.SetDimension(measurementSize);
   for (Integer i = 0; i < measurementSize; ++i)
   {
      for (Integer j = 0; j < measurementSize; ++j)
      {
         measErrorCovariance(i,j) = (i == j ?
                        (noiseSigma[0] != 0.0 ? (noiseSigma[0] * noiseSigma[0]) : 1.0) :            // noiseSigma[0] is used for Range in Km. Its unit is Km
                        0.0);
      }
   }
   //MessageInterface::ShowMessage("Covariance size = %d\n", measurementSize);
   
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
const MeasurementData& RangeAdapterKm::CalculateMeasurement(bool withEvents,
      ObservationData* forObservation, std::vector<RampTableData>* rampTB)
{
   #ifdef DEBUG_ADAPTER_EXECUTION
      MessageInterface::ShowMessage("RangeAdapterKm::CalculateMeasurement(%s, "
            "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
            rampTB);
   #endif

   if (!calcData)
      throw MeasurementException("Measurement data was requested for " +
            instanceName + " before the measurement was set");
   
   // Fire the measurement model to build the collection of signal data
   if (calcData->CalculateMeasurement(withLighttime, forObservation, rampTB))   // made changes by TUAN NGUYEN
   {
      std::vector<SignalBase*> paths = calcData->GetSignalPaths();
      std::string unfeasibilityReason;
      Real        unfeasibilityValue;

      // set to default
      cMeasurement.isFeasible = false;
      cMeasurement.unfeasibleReason = "";
      cMeasurement.feasibilityValue = 90.0;

      RealArray values;
      for (UnsignedInt i = 0; i < paths.size(); ++i)           // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths 
      {
         // Calculate C-value for signal path ith:
         values.push_back(0.0);
         SignalBase *currentleg = paths[i];
         SignalData *current = ((currentleg == NULL)?NULL:(currentleg->GetSignalDataObject()));
         SignalData *first = current;
         UnsignedInt legIndex = 0;
         
         while (currentleg != NULL)
         {
            ++legIndex;
            // Set feasibility value
            if (current->feasibilityReason == "N")
            {
               if ((current->stationParticipant)&&(cMeasurement.unfeasibleReason == ""))
               {
                  cMeasurement.isFeasible = true;
                  cMeasurement.unfeasibleReason = "N";
                  cMeasurement.feasibilityValue = current->feasibilityValue;
               }
            }
            else if (current->feasibilityReason == "B")
            {
               std::stringstream ss;
               ss << "B" << legIndex;
               current->feasibilityReason = ss.str();
               if ((cMeasurement.unfeasibleReason == "")||(cMeasurement.unfeasibleReason == "N"))
               {
                  cMeasurement.unfeasibleReason = current->feasibilityReason; 
                  cMeasurement.isFeasible = false;
                  cMeasurement.feasibilityValue = current->feasibilityValue;
               }
            }

            // accumulate all light time range for signal path ith 
            Rvector3 signalVec = current->rangeVecInertial;
            values[i] += signalVec.GetMagnitude();

            // accumulate all range corrections for signal path ith
            for (UnsignedInt j = 0; j < current->correctionIDs.size(); ++j)
            {
               if (current->useCorrection[j])
                  values[i] += current->corrections[j];
            }// for j loop

            // accumulate all hardware delays for signal path ith                          // made changes by TUAN NGUYEN
            values[i] += ((current->tDelay + current->rDelay)*                             // made changes by TUAN NGUYEN
               GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);   // made changes by TUAN NGUYEN

            // Get measurement epoch in the first signal path. It will apply for all other paths
            if (i == 0)
            {
#ifdef USE_PRECISION_TIME
               // cMeasurement.epoch = current->rPrecTime.GetMjd();                                                     // made changes by TUAN NGUYEN
               if (calcData->GetTimeTagFlag())                                                                          // made changes by TUAN NGUYEN
               {
                  // Measurement epoch will be at the end of signal path when time tag is at the receiver
                  if (current->next == NULL)                                                                            // made changes by TUAN NGUYEN
                     cMeasurement.epoch = current->rPrecTime.GetMjd() + current->rDelay/GmatTimeConstants::SECS_PER_DAY;// made changes by TUAN NGUYEN
               }
               else                                                                                                     // made changes by TUAN NGUYEN
               {
                  // Measurement epoch will be at the begin of signal path when time tag is at the transmiter
                  cMeasurement.epoch = first->tPrecTime.GetMjd() - first->tDelay/GmatTimeConstants::SECS_PER_DAY;       // made changes by TUAN NGUYEN
               }
#else
               //cMeasurement.epoch = current->rTime;                                                                   // made changes by TUAN NGUYEN
               if (calcData->GetTimeTagFlag())                                                                          // made changes by TUAN NGUYEN
               {
                  // Measurement epoch will be at the end of signal path when time tag is at the receiver
                  if (current->next == NULL)                                                                            // made changes by TUAN NGUYEN
                     cMeasurement.epoch = current->rTime + current->rDelay/GmatTimeConstants::SECS_PER_DAY;             // made changes by TUAN NGUYEN
               }
               else                                                                                                     // made changes by TUAN NGUYEN
               {
                  // Measurement epoch will be at the begin of signal path when time tag is at the transmiter
                  cMeasurement.epoch = first->tTime - first->tDelay/GmatTimeConstants::SECS_PER_DAY;                    // made changes by TUAN NGUYEN
               }
#endif
            }

            currentleg = currentleg->GetNext();
            current = ((currentleg == NULL)?NULL:(currentleg->GetSignalDataObject()));

         }// while loop

      }// for i loop
      
      // Specify noise sigma value
      Real nsigma = 0.0;
      if ((measurementType == "Range")||(measurementType == "DSNRange"))
         nsigma = noiseSigma[0];                               // unit: km
      else if (measurementType == "Doppler")
         nsigma = noiseSigma[1];                               // unit: Hz

      // Set measurement values
      cMeasurement.value.clear();
      for (UnsignedInt i = 0; i < values.size(); ++i)
         cMeasurement.value.push_back(0.0);

      for (UnsignedInt i = 0; i < values.size(); ++i)
      {
         Real measVal = values[i];
         #ifdef DEBUG_RANGE_CALCULATION
            MessageInterface::ShowMessage("===================================================================\n");
            MessageInterface::ShowMessage("====  RangeAdapterKm: Range Calculation for Measurement Data %dth  \n", i);
            MessageInterface::ShowMessage("===================================================================\n");
            MessageInterface::ShowMessage("      . Measurement type : <%s>  nsigma = %lf\n", measurementType.c_str(), nsigma);
            MessageInterface::ShowMessage("      . Noise adding option: %s\n", (addNoise?"true":"false"));
            MessageInterface::ShowMessage("      . Real range for path %dth: %.12lf km \n", i, values[i]);
         #endif

         // No action is neede for multiplier in RangeAdapterKm due to it calculates full range of signal path.
         // Multiplier will be used in any class derived from RangeAdapterKm
         #ifdef DEBUG_RANGE_CALCULATION
            MessageInterface::ShowMessage("      . No action is needed for multiplier in RangeAdapterKm object.\n");
         #endif

         // This section is only done when measurement type is 'Range'. For other types such as DSNRange or Doppler, it will be done in their adapters  
         if (measurementType == "Range")
         {
            //@Todo: write code to add bias to measuement value here
            #ifdef DEBUG_RANGE_CALCULATION
               MessageInterface::ShowMessage("      . No bias was implemented in this GMAT version.\n");
            #endif

            // Add noise to measurement value                                              // made changes by TUAN NGUYEN
            if (addNoise)                                                                  // made changes by TUAN NGUYEN
            {                                                                              // made changes by TUAN NGUYEN
               // Add noise here                                                           // made changes by TUAN NGUYEN
               RandomNumber* rn = RandomNumber::Instance();                                // made changes by TUAN NGUYEN
               Real val = rn->Gaussian(measVal, nsigma);                                   // made changes by TUAN NGUYEN
               while (val <= 0.0)                                                          // made changes by TUAN NGUYEN
                  val = rn->Gaussian(measVal, nsigma);                                     // made changes by TUAN NGUYEN
               measVal = val;                                                              // made changes by TUAN NGUYEN
            }                                                                              // made changes by TUAN NGUYEN
         }
         cMeasurement.value[i] = measVal;                                               // made changes by TUAN NGUYEN

         #ifdef DEBUG_RANGE_CALCULATION
            MessageInterface::ShowMessage("      . C-value (with noise)   : %.12lf km\n", cMeasurement.value[i]);
            MessageInterface::ShowMessage("      . Measurement epoch A1Mjd: %.12lf\n", cMeasurement.epoch); 
            MessageInterface::ShowMessage("      . Measurement is %s\n", (cMeasurement.isFeasible?"feasible":"unfeasible"));
            MessageInterface::ShowMessage("      . Feasibility reason     : %s\n", cMeasurement.unfeasibleReason.c_str());
            MessageInterface::ShowMessage("      . Elevation angle        : %.12lf degree\n", cMeasurement.feasibilityValue);
            MessageInterface::ShowMessage("===================================================================\n");
         #endif

      }

      // Calculate measurement covariance
      cMeasurement.covariance = &measErrorCovariance;


      #ifdef DEBUG_ADAPTER_EXECUTION
         MessageInterface::ShowMessage("Computed measurement\n   Type:  %d\n   "
               "Type:  %s\n   UID:   %d\n   Epoch:%.12lf\n   Participants:\n",
               cMeasurement.type, cMeasurement.typeName.c_str(),
               cMeasurement.uniqueID, cMeasurement.epoch);
         for (UnsignedInt k = 0; k < cMeasurement.participantIDs.size(); ++k)
            MessageInterface::ShowMessage("      %s\n",
                  cMeasurement.participantIDs[k].c_str());
         MessageInterface::ShowMessage("   Values:\n");
         for (UnsignedInt k = 0; k < cMeasurement.value.size(); ++k)
            MessageInterface::ShowMessage("      %.12lf\n",
                  cMeasurement.value[k]);

         MessageInterface::ShowMessage("   Valid: %s\n",
               (cMeasurement.isFeasible ? "true" : "false"));
      #endif
   }

   #ifdef DEBUG_ADAPTER_EXECUTION
      MessageInterface::ShowMessage("RangeAdapterKm::CalculateMeasurement(%s, "
            "<%p>, <%p>) exit\n", (withEvents ? "true" : "false"), forObservation,
            rampTB);
   #endif

   return cMeasurement;
}


const MeasurementData& RangeAdapterKm::CalculateMeasurementAtOffset(
      bool withEvents, Real dt, ObservationData* forObservation,
      std::vector<RampTableData>* rampTB)
{
   static MeasurementData offsetMeas;

   if (!calcData)
      throw MeasurementException("Measurement data was requested for " +
            instanceName + " before the measurement was set");

   // Fire the measurement model to build the collection of signal data
   if (calcData->CalculateMeasurement(withLighttime, forObservation, rampTB, dt))   // made changes by TUAN NGUYEN
   {
      std::vector<SignalData*> data = calcData->GetSignalData();
      std::string unfeasibilityReason;
      Real        unfeasibilityValue;

      // set to default
      offsetMeas.isFeasible = true;
      offsetMeas.unfeasibleReason = "N";
      offsetMeas.feasibilityValue = 90.0;

      RealArray values;
      for (UnsignedInt i = 0; i < data.size(); ++i)
      {
         // Calculate C-value for signal path ith:
         values.push_back(0.0);
         SignalData *current = data[i];
         SignalData *first = current;
         UnsignedInt legIndex = 0;

         while (current != NULL)
         {
            ++legIndex;
            // Set feasibility value
            if (current->feasibilityReason == "N")
            {
               if (current->stationParticipant)
                  offsetMeas.feasibilityValue = current->feasibilityValue;
            }
            else if (current->feasibilityReason == "B")
            {
               std::stringstream ss;
               ss << "B" << legIndex;
               current->feasibilityReason = ss.str();
               if (offsetMeas.unfeasibleReason == "N")
               {
                  offsetMeas.unfeasibleReason = current->feasibilityReason;
                  offsetMeas.isFeasible = false;
                  offsetMeas.feasibilityValue = current->feasibilityValue;
               }
            }

            // accumulate all light time range for signal path ith
            Rvector3 signalVec = current->rangeVecInertial;
            values[i] += signalVec.GetMagnitude();

            // accumulate all range corrections for signal path ith
            for (UnsignedInt j = 0; j < current->correctionIDs.size(); ++j)
            {
               if (current->useCorrection[j])
                  values[i] += current->corrections[j];
            }// for j loop

            // accumulate all hardware delays for signal path ith                          // made changes by TUAN NGUYEN
            values[i] += ((current->tDelay + current->rDelay)*                             // made changes by TUAN NGUYEN
               GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);   // made changes by TUAN NGUYEN

            // Get measurement epoch in the first signal path. It will apply for all other paths
            if (i == 0)
            {
#ifdef USE_PRECISION_TIME
               // offsetMeas.epoch = current->rPrecTime.GetMjd();                                                     // made changes by TUAN NGUYEN
               if (calcData->GetTimeTagFlag())                                                                          // made changes by TUAN NGUYEN
               {
                  // Measurement epoch will be at the end of signal path when time tag is at the receiver
                  if (current->next == NULL)                                                                            // made changes by TUAN NGUYEN
                     offsetMeas.epoch = current->rPrecTime.GetMjd() + current->rDelay/GmatTimeConstants::SECS_PER_DAY;// made changes by TUAN NGUYEN
               }
               else                                                                                                     // made changes by TUAN NGUYEN
               {
                  // Measurement epoch will be at the begin of signal path when time tag is at the transmiter
                  offsetMeas.epoch = first->tPrecTime.GetMjd() - first->tDelay/GmatTimeConstants::SECS_PER_DAY;       // made changes by TUAN NGUYEN
               }
#else
               //cMeasurement.epoch = current->rTime;                                                                   // made changes by TUAN NGUYEN
               if (calcData->GetTimeTagFlag())                                                                          // made changes by TUAN NGUYEN
               {
                  // Measurement epoch will be at the end of signal path when time tag is at the receiver
                  if (current->next == NULL)                                                                            // made changes by TUAN NGUYEN
                     offsetMeas.epoch = current->rTime + current->rDelay/GmatTimeConstants::SECS_PER_DAY;             // made changes by TUAN NGUYEN
               }
               else                                                                                                     // made changes by TUAN NGUYEN
               {
                  // Measurement epoch will be at the begin of signal path when time tag is at the transmiter
                  offsetMeas.epoch = first->tTime - first->tDelay/GmatTimeConstants::SECS_PER_DAY;                    // made changes by TUAN NGUYEN
               }
#endif
            }

            current = current->next;
         }// while loop

      }// for i loop

      // Specify noise sigma value
      Real nsigma = 0.0;
      if ((measurementType == "Range")||(measurementType == "DSNRange"))
         nsigma = noiseSigma[0];
      else if ((measurementType == "Doppler")||(measurementType == "DSNDoppler"))
         nsigma = noiseSigma[1];

      // Set measurement values
      offsetMeas.value.clear();
      for (UnsignedInt i = 0; i < values.size(); ++i)
         offsetMeas.value.push_back(0.0);

      for (UnsignedInt i = 0; i < values.size(); ++i)
      {
         Real measVal = values[i];

         //@Todo: write code to add bias to measuement value here
         #ifdef DEBUG_RANGE_CALCULATION
            MessageInterface::ShowMessage("      . No bias was implemented in this GMAT version.\n");
         #endif

         // Add noise to measurement value                                              // made changes by TUAN NGUYEN
         if (addNoise)                                                                  // made changes by TUAN NGUYEN
         {                                                                              // made changes by TUAN NGUYEN
            // Add noise here                                                           // made changes by TUAN NGUYEN
            RandomNumber* rn = RandomNumber::Instance();                                // made changes by TUAN NGUYEN
            Real val = rn->Gaussian(measVal, nsigma);                                   // made changes by TUAN NGUYEN
            while (val <= 0.0)                                                          // made changes by TUAN NGUYEN
               val = rn->Gaussian(measVal, nsigma);                                     // made changes by TUAN NGUYEN
            measVal = val;                                                              // made changes by TUAN NGUYEN
         }                                                                              // made changes by TUAN NGUYEN
         offsetMeas.value[i] = measVal;                                               // made changes by TUAN NGUYEN

      }
   }

   return offsetMeas;
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
const std::vector<RealArray>& RangeAdapterKm::CalculateMeasurementDerivatives(
      GmatBase* obj, Integer id)
{
   if (!calcData)
      throw MeasurementException("Measurement derivative data was requested "
            "for " + instanceName + " before the measurement was set");

   Integer parmId = GetParmIdFromEstID(id, obj);

   // Perform the calculations
   #ifdef DEBUG_ADAPTER_DERIVATIVES
      MessageInterface::ShowMessage("RangeAdapterKm::CalculateMeasurement"
            "Derivatives(%s, %d) called; parm ID is %d; Epoch %.12lf\n",
            obj->GetName().c_str(), id, parmId, cMeasurement.epoch);
   #endif

   const std::vector<RealArray> *derivativeData =
         &(calcData->CalculateMeasurementDerivatives(obj, id));

   #ifdef DEBUG_ADAPTER_DERIVATIVES
      MessageInterface::ShowMessage("   Derivatives: [");
      for (UnsignedInt i = 0; i < derivativeData->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeData->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeData->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
   #endif

   // Now assemble the derivative data into the requested derivative
   UnsignedInt size = derivativeData->at(0).size();

   theDataDerivatives.clear();
   for (UnsignedInt i = 0; i < derivativeData->size(); ++i)
   {
      RealArray oneRow;
      oneRow.assign(size, 0.0);
      theDataDerivatives.push_back(oneRow);

      if (derivativeData->at(i).size() != size)
         throw MeasurementException("Derivative data size is a different size "
               "than expected");

      for (UnsignedInt j = 0; j < size; ++j)
      {
         theDataDerivatives[i][j] = (derivativeData->at(i))[j];
      }
   }

   #ifdef DEBUG_ADAPTER_DERIVATIVES
      MessageInterface::ShowMessage("Exit RangeAdapterKm::CalculateMeasurementDerivatives():\n");
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
bool RangeAdapterKm::WriteMeasurements()
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
bool RangeAdapterKm::WriteMeasurement(Integer id)
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
Integer RangeAdapterKm::HasParameterCovariances(Integer parameterId)
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
Integer RangeAdapterKm::GetEventCount()
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
void RangeAdapterKm::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   TrackingDataAdapter::SetCorrection(correctionName, correctionType);            // made changes by TUAN NGUYEN
}
