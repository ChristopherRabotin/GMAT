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
#include "ErrorModel.hpp"
#include <sstream>


//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_CONSTRUCTION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION
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
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RangeAdapterKm default constructor <%p>\n", this);
#endif

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
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RangeAdapterKm default destructor  <%p>\n", this);
#endif
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
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RangeAdapterKm copy constructor   from <%p> to <%p>\n", &rak, this);
#endif

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
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RangeAdapterKm operator =   set <%p> = <%p>\n", this, &rak);
#endif

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
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RangeAdapterKm::Clone() clone this <%p>\n", this);
#endif

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
// bool SetMeasurement(MeasureModel* meas)
//------------------------------------------------------------------------------
/**
 * Sets the measurement model pointer
 *
 * @param meas The pointer
 *
 * @return true if set, false if not
 */
//------------------------------------------------------------------------------
bool RangeAdapterKm::SetMeasurement(MeasureModel* meas)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("RangeAdapterKm<%p>::SetMeasurement(meas = <%p,%s>)\n", this, meas, meas->GetName().c_str()); 
#endif

   return TrackingDataAdapter::SetMeasurement(meas);
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
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Start Initializing a RangeAdapterKm <%p>\n", this);
   #endif

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

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("End Initializing a RangeAdapterKm <%p>\n", this);
   #endif

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
 * @param withEvents          Flag indicating is the light time solution should be
 *                            included
 * @param forObservation      The observation data associated with this measurement
 * @param rampTB              Ramp table for a ramped measurement
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
   if (calcData->CalculateMeasurement(withLighttime, withMediaCorrection, forObservation, rampTB))
   {
      // QA Media correction:
      cMeasurement.isIonoCorrectWarning = false;
      cMeasurement.ionoCorrectWarningValue = 0.0;
      cMeasurement.isTropoCorrectWarning = false;
      cMeasurement.tropoCorrectWarningValue = 0.0;

      if (withMediaCorrection)
      {  
         Real correction = GetIonoCorrection();                                  // unit: km
         if ((correction < 0.0) || (correction > 0.04))
         {
            // Set a warning to measurement data when ionosphere correction is outside of range [0 km , 0.04 km]
            cMeasurement.isIonoCorrectWarning = true;
            cMeasurement.ionoCorrectWarningValue = correction;                   // unit: km
         }
         
         correction = GetTropoCorrection();                                      // unit: km
         if ((correction < 0.0) || (correction > 0.12))
         {
            // Set a warning to measurement data when troposphere correction is outside of range [0 km , 0.12 km]
            cMeasurement.isTropoCorrectWarning = true;
            cMeasurement.tropoCorrectWarningValue = correction;                  // unit: km
         }
      }

      std::vector<SignalBase*> paths = calcData->GetSignalPaths();
      std::string unfeasibilityReason;
      Real        unfeasibilityValue;

      // set to default
      cMeasurement.isFeasible = false;
      cMeasurement.unfeasibleReason = "";
      cMeasurement.feasibilityValue = 90.0;
      
      GmatEpoch transmitEpoch, receiveEpoch;
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

            // accumulate all hardware delays for signal path ith
            values[i] += ((current->tDelay + current->rDelay)*
                  GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);
            
            // Get measurement epoch in the first signal path. It will apply for all other paths
            if (i == 0)
            {
               transmitEpoch = first->tPrecTime.GetMjd() - first->tDelay / GmatTimeConstants::SECS_PER_DAY;
               receiveEpoch  = current->rPrecTime.GetMjd() + current->rDelay / GmatTimeConstants::SECS_PER_DAY;
               if (calcData->GetTimeTagFlag())
               {
                  // Measurement epoch will be at the end of signal path when time tag is at the receiver
                  if (current->next == NULL)
                     cMeasurement.epoch = receiveEpoch;
               }
               else
               {
                  // Measurement epoch will be at the begin of signal path when time tag is at the transmiter
                  cMeasurement.epoch = transmitEpoch;
               }
            }
            
            currentleg = currentleg->GetNext();
            current = ((currentleg == NULL)?NULL:(currentleg->GetSignalDataObject()));

         }// while loop
         
      }// for i loop
      

      // Caluclate uplink frequency at received time and transmit time
      cMeasurement.uplinkFreq = calcData->GetUplinkFrequency(0, rampTB) * 1.0e6;                        // unit: Hz
      cMeasurement.uplinkFreqAtRecei = calcData->GetUplinkFrequencyAtReceivedEpoch(0,rampTB) * 1.0e6;   // unit: Hz
      cMeasurement.uplinkBand = calcData->GetUplinkFrequencyBand(0, rampTB);
      

      if (measurementType == "Range_KM")
      {
         // @todo: it needs to specify number of trips instead of using 2
         ComputeMeasurementBias("Bias", "Range_KM", 2);
         ComputeMeasurementNoiseSigma("NoiseSigma", "Range_KM", 2);
         ComputeMeasurementErrorCovarianceMatrix();
      }

      // Set measurement values
      cMeasurement.value.clear();
      for (UnsignedInt i = 0; i < values.size(); ++i)
         cMeasurement.value.push_back(0.0);

      for (UnsignedInt i = 0; i < values.size(); ++i)
      {
         Real measVal = values[i];
         #ifdef DEBUG_RANGE_CALCULATION
            MessageInterface::ShowMessage("===================================================================\n");
            MessageInterface::ShowMessage("====  RangeAdapterKm (%s): Range Calculation for Measurement Data %dth  \n", GetName().c_str(), i);
            MessageInterface::ShowMessage("===================================================================\n");

            MessageInterface::ShowMessage("      . Path : ");
            for (UnsignedInt k=0; k < participantLists[i]->size(); ++k)
               MessageInterface::ShowMessage("%s,  ", participantLists[i]->at(k).c_str());
            MessageInterface::ShowMessage("\n");

            MessageInterface::ShowMessage("      . Measurement type           : <%s>\n", measurementType.c_str());            
            MessageInterface::ShowMessage("      . C-value w/o noise and bias : %.12lf km \n", values[i]);
            MessageInterface::ShowMessage("      . Noise adding option        : %s\n", (addNoise?"true":"false"));
            MessageInterface::ShowMessage("      . Bias adding option        : %s\n", (addBias?"true":"false"));
            if (measurementType == "Range_KM")
            {
               MessageInterface::ShowMessage("      . Range noise sigma          : %.12lf km \n", noiseSigma[i]);
               MessageInterface::ShowMessage("      . Range bias                 : %.12lf km \n", measurementBias[i]);
               MessageInterface::ShowMessage("      . Multiplier                 : %.12lf \n", multiplier);
            }
         #endif

         // This section is only done when measurement type is "Range_KM". For other types such as DSNRange or Doppler, it will be done in their adapters  
         if (measurementType == "Range_KM")
         {
            // Apply multiplier for "Range_KM" measurement model. This step has to
            // be done before adding bias and noise
            measVal = measVal*multiplier;

            // if need range value only, skip this section, otherwise add noise and bias as possible
            // Note: for Doppler measurement for E and S paths, we only need range value only and no noise and bias are added to measurement value. 
            if (!rangeOnly)
            {
               // Add noise to measurement value
               if (addNoise)
               {
                  // Add noise here
                  RandomNumber* rn = RandomNumber::Instance();
                  Real val = rn->Gaussian(measVal, noiseSigma[i]);                  // noise sigma unit: Km
                  //val = rn->Gaussian(measVal, noiseSigma[i]);
                  measVal = val;
               }

               // Add bias to measurement value only after noise had been added in order to avoid adding bias' noise 
               if (addBias)
                  measVal = measVal + measurementBias[i];                          // bias unit: Km
            }
         }
         cMeasurement.value[i] = measVal;

         #ifdef DEBUG_RANGE_CALCULATION
            MessageInterface::ShowMessage("      . C-value with noise and bias : %.12lf km\n", cMeasurement.value[i]);
            MessageInterface::ShowMessage("      . Measurement epoch A1Mjd     : %.12lf\n", cMeasurement.epoch); 
            MessageInterface::ShowMessage("      . Transmit frequency at receive epoch  : %.12le Hz\n", cMeasurement.uplinkFreqAtRecei); 
            MessageInterface::ShowMessage("      . Transmit frequency at transmit epoch : %.12le Hz\n", cMeasurement.uplinkFreq); 
            MessageInterface::ShowMessage("      . Measurement is %s\n", (cMeasurement.isFeasible?"feasible":"unfeasible"));
            MessageInterface::ShowMessage("      . Feasibility reason          : %s\n", cMeasurement.unfeasibleReason.c_str());
            MessageInterface::ShowMessage("      . Elevation angle             : %.12lf degree\n", cMeasurement.feasibilityValue);
            MessageInterface::ShowMessage("      . Covariance matrix           : <%p>\n", cMeasurement.covariance);
            if (cMeasurement.covariance)
            {
               MessageInterface::ShowMessage("      . Covariance matrix size = %d\n", cMeasurement.covariance->GetDimension());
               MessageInterface::ShowMessage("     [ ");
               for (UnsignedInt i = 0; i < cMeasurement.covariance->GetDimension(); ++i)
               {
                  if ( i > 0)
                     MessageInterface::ShowMessage("\n");
                  for (UnsignedInt j = 0; j < cMeasurement.covariance->GetDimension(); ++j)
                     MessageInterface::ShowMessage("%le   ", cMeasurement.covariance->GetCovariance()->GetElement(i,j));
               }
               MessageInterface::ShowMessage("]\n");
            }

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


//-------------------------------------------------------------------------------------
// Real GetIonoCorrection()
//-------------------------------------------------------------------------------------
/**
* This function is used to get ionosphere correction (unit: km)
*
* @return     ionosphere correction of a measurment in km
*/
//-------------------------------------------------------------------------------------
Real RangeAdapterKm::GetIonoCorrection()
{
   Real correction = 0.0;

   std::vector<SignalBase*> paths = calcData->GetSignalPaths();
   SignalBase *currentleg = paths[0]; // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths 
   SignalData *current = ((currentleg == NULL) ? NULL : (currentleg->GetSignalDataObject()));

   while (currentleg != NULL)
   {
      // accumulate all range corrections for signal path ith
      for (UnsignedInt j = 0; j < current->correctionIDs.size(); ++j)
      {
         if ((current->useCorrection[j]) && (current->correctionIDs[j] == "Ionosphere"))
            correction += current->corrections[j];
      }// for j loop

      currentleg = currentleg->GetNext();
      current = ((currentleg == NULL) ? NULL : (currentleg->GetSignalDataObject()));
   }
   
   return correction;
}


//-------------------------------------------------------------------------------------
// Real GetTropoCorrection()
//-------------------------------------------------------------------------------------
/**
* This function is used to get troposphere correction (unit: km)
*
* @return     troposphere correction of a measurment in km
*/
//-------------------------------------------------------------------------------------
Real RangeAdapterKm::GetTropoCorrection()
{
   Real correction = 0.0;

   std::vector<SignalBase*> paths = calcData->GetSignalPaths();
   SignalBase *currentleg = paths[0]; // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths 
   SignalData *current = ((currentleg == NULL) ? NULL : (currentleg->GetSignalDataObject()));

   while (currentleg != NULL)
   {
      // accumulate all range corrections for signal path ith
      for (UnsignedInt j = 0; j < current->correctionIDs.size(); ++j)
      {
         if ((current->useCorrection[j]) && (current->correctionIDs[j] == "Troposphere"))
            correction += current->corrections[j];
      }// for j loop

      currentleg = currentleg->GetNext();
      current = ((currentleg == NULL) ? NULL : (currentleg->GetSignalDataObject()));
   }

   return correction;
}


//------------------------------------------------------------------------------
// bool ReCalculateFrequencyAndMediaCorrection(UnsignedInt pathIndex, 
//        Real uplinkFrequency, std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------
/**
* This function is used to recalculate frequency and media correction for TDRS 
* Doppler measurement.
*
* @param pathIndex           Calculation for the given signal path specified by
*                            pathIndex
* @param uplinkFrequency     Transmit frequency
* @param rampTB              Ramp table for a ramped measurement
*
* @return                    true if no error ocurrs, false otherwise
*/
//------------------------------------------------------------------------------
bool RangeAdapterKm::ReCalculateFrequencyAndMediaCorrection(UnsignedInt pathIndex, 
                         Real uplinkFrequency, std::vector<RampTableData>* rampTB)
{
   bool retval = false;

   // Recalculate frequency and mediacorrection
   calcData->ReCalculateFrequencyAndMediaCorrection(pathIndex, uplinkFrequency, rampTB);
   
   // Add media correction to C-value
   std::vector<SignalBase*> paths = calcData->GetSignalPaths();
   SignalBase *currentleg = paths[pathIndex];
   SignalData *current = ((currentleg == NULL)?NULL:(currentleg->GetSignalDataObject()));
   
   Real correction = 0.0;
   while (currentleg != NULL)
   {
      for (UnsignedInt j = 0; j < current->correctionIDs.size(); ++j)
      {
         if (current->useCorrection[j])
         {
            if ((current->correctionIDs[j] == "Troposphere") || (current->correctionIDs[j] == "Ionosphere"))
               correction += current->corrections[j];
         }
      }
      currentleg = currentleg->GetNext();
      current = ((currentleg == NULL)?NULL:(currentleg->GetSignalDataObject()));
   }
   
   cMeasurement.value[pathIndex] += correction;
   
   retval = true;

   return retval;
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
   if (calcData->CalculateMeasurement(withLighttime, withMediaCorrection, forObservation, rampTB, dt))
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

            // accumulate all hardware delays for signal path ith
            values[i] += ((current->tDelay + current->rDelay)*
               GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);

            // Get measurement epoch in the first signal path. It will apply for all other paths
            if (i == 0)
            {
               if (calcData->GetTimeTagFlag())
               {
                  // Measurement epoch will be at the end of signal path when time tag is at the receiver
                  if (current->next == NULL)
                     offsetMeas.epoch = current->rPrecTime.GetMjd() + current->rDelay/GmatTimeConstants::SECS_PER_DAY;
               }
               else
               {
                  // Measurement epoch will be at the begin of signal path when time tag is at the transmiter
                  offsetMeas.epoch = first->tPrecTime.GetMjd() - first->tDelay/GmatTimeConstants::SECS_PER_DAY;
               }
            }

            current = current->next;
         }// while loop

      }// for i loop


      // Set measurement values
      offsetMeas.value.clear();
      for (UnsignedInt i = 0; i < values.size(); ++i)
         offsetMeas.value.push_back(0.0);

      Real nsigma;
      for (UnsignedInt i = 0; i < values.size(); ++i)
      {
         Real measVal = values[i];

         //@Todo: write code to add bias to measuement value here
         #ifdef DEBUG_RANGE_CALCULATION
            MessageInterface::ShowMessage("      . No bias was implemented in this GMAT version.\n");
         #endif

         // Add noise to measurement value
         nsigma = noiseSigma[i];                  // noiseSigma[i] is noise sigma associated with measurement values[i]
         if (addNoise)
         {
            // Add noise here
            RandomNumber* rn = RandomNumber::Instance();
            Real val = rn->Gaussian(measVal, nsigma);
            while (val <= 0.0)
               val = rn->Gaussian(measVal, nsigma);
            measVal = val;
         }
         offsetMeas.value[i] = measVal;

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

   //Integer parmId = GetParmIdFromEstID(id, obj);
   Integer parameterID;
   if (id > 250)
      parameterID = id - obj->GetType() * 250; //GetParmIdFromEstID(id, obj);
   else
      parameterID = id;
   std::string paramName = obj->GetParameterText(parameterID);

   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage("Solve-for parameter: %s\n", paramName.c_str());
   #endif

   // Perform the calculations
   #ifdef DEBUG_ADAPTER_DERIVATIVES
      MessageInterface::ShowMessage("RangeAdapterKm::CalculateMeasurement"
            "Derivatives(%s, %d) called; parm ID is %d; Epoch %.12lf\n",
            obj->GetFullName().c_str(), id, parameterID, cMeasurement.epoch);
   #endif

   // Clear derivative variable
   for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      theDataDerivatives[i].clear();
   theDataDerivatives.clear();

   if (paramName == "Bias")
   {
      if (((ErrorModel*)obj)->GetStringParameter("Type") == "Range_KM")
         theDataDerivatives = calcData->CalculateMeasurementDerivatives(obj, id);
      else
      {
         Integer size = obj->GetEstimationParameterSize(id);
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         theDataDerivatives.push_back(oneRow);
      }

   }
   else
   {

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
      // Note that: multiplier is only applied for elements of spacecraft's state, position, and velocity
      Real factor = 1.0;
      if (measurementType == "Range_KM")
      {
         if (obj->IsOfType(Gmat::SPACECRAFT))
         {
            factor = multiplier;
         }
      }

      UnsignedInt size = derivativeData->at(0).size();
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
            theDataDerivatives[i][j] = (derivativeData->at(i))[j] * factor;
         }
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
   TrackingDataAdapter::SetCorrection(correctionName, correctionType);
}
