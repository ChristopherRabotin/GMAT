//$Id$
//------------------------------------------------------------------------------
//                           GNRangeAdapter
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
// Author: Tuan D. Nguyen, GSFC/NASA.
// Created: Jan 12, 2017
/**
 * A measurement adapter for GN range measurement
 */
//------------------------------------------------------------------------------

#include "GNRangeAdapter.hpp"
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
// GNRangeAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
GNRangeAdapter::GNRangeAdapter(const std::string& name) :
   RangeAdapterKm(name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNRangeAdapter default constructor <%p>\n", this);
#endif

   typeName = "Range";           // change measurement type from "RangeKm" to "Range" for GN Range 
}


//------------------------------------------------------------------------------
// ~GNRangeAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GNRangeAdapter::~GNRangeAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNRangeAdapter default destructor  <%p>\n", this);
#endif
}


//------------------------------------------------------------------------------
// GNRangeAdapter(const GNRangeAdapter& rak)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rak The adapter copied to make this one
 */
//------------------------------------------------------------------------------
GNRangeAdapter::GNRangeAdapter(const GNRangeAdapter& rak) :
   RangeAdapterKm      (rak)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNRangeAdapter copy constructor   from <%p> to <%p>\n", &rak, this);
#endif

}


//------------------------------------------------------------------------------
// GNRangeAdapter& operator=(const GNRangeAdapter& rak)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rak The adapter copied to make this one match it
 *
 * @return This adapter made to look like rak
 */
//------------------------------------------------------------------------------
GNRangeAdapter& GNRangeAdapter::operator=(const GNRangeAdapter& rak)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNRangeAdapter operator =   set <%p> = <%p>\n", this, &rak);
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
GmatBase* GNRangeAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNRangeAdapter::Clone() clone this <%p>\n", this);
#endif

   return new GNRangeAdapter(*this);
}


const MeasurementData& GNRangeAdapter::CalculateMeasurement(bool withEvents,
   ObservationData* forObservation, std::vector<RampTableData>* rampTB)
{
#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("GNRangeAdapter::CalculateMeasurement(%s, "
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
         SignalData *current = ((currentleg == NULL) ? NULL : (currentleg->GetSignalDataObject()));
         SignalData *first = current;
         UnsignedInt legIndex = 0;

         while (currentleg != NULL)
         {
            ++legIndex;
            // Set feasibility value
            if (current->feasibilityReason == "N")
            {
               if ((current->stationParticipant) && (cMeasurement.unfeasibleReason == ""))
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
               if ((cMeasurement.unfeasibleReason == "") || (cMeasurement.unfeasibleReason == "N"))
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
               receiveEpoch = current->rPrecTime.GetMjd() + current->rDelay / GmatTimeConstants::SECS_PER_DAY;
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
            current = ((currentleg == NULL) ? NULL : (currentleg->GetSignalDataObject()));

         }// while loop

      }// for i loop

      // Caluclate uplink frequency at received time and transmit time
      cMeasurement.uplinkFreq = calcData->GetUplinkFrequency(0, rampTB) * 1.0e6;                        // unit: Hz
      cMeasurement.uplinkFreqAtRecei = calcData->GetUplinkFrequencyAtReceivedEpoch(0, rampTB) * 1.0e6;   // unit: Hz
      cMeasurement.uplinkBand = calcData->GetUplinkFrequencyBand(0, rampTB);

      if (measurementType == "Range")
      {
         // @todo: it needs to specify number of trips instead of using 2
         ComputeMeasurementBias("Bias", measurementType, 2);
         ComputeMeasurementNoiseSigma("NoiseSigma", measurementType, 2);
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
         MessageInterface::ShowMessage("====  GNRangeAdapter (%s): Range Calculation for Measurement Data %dth  \n", GetName().c_str(), i);
         MessageInterface::ShowMessage("===================================================================\n");

         MessageInterface::ShowMessage("      . Path : ");
         for (UnsignedInt k = 0; k < participantLists[i]->size(); ++k)
            MessageInterface::ShowMessage("%s,  ", participantLists[i]->at(k).c_str());
         MessageInterface::ShowMessage("\n");

         MessageInterface::ShowMessage("      . Measurement type           : <%s>\n", measurementType.c_str());
         MessageInterface::ShowMessage("      . C-value w/o noise and bias : %.12lf km \n", values[i]);
         MessageInterface::ShowMessage("      . Noise adding option        : %s\n", (addNoise ? "true" : "false"));
         MessageInterface::ShowMessage("      . Bias adding option        : %s\n", (addBias ? "true" : "false"));
         //if (measurementType == "Range_KM")
         if (measurementType == "Range")
         {
            MessageInterface::ShowMessage("      . Range noise sigma          : %.12lf km \n", noiseSigma[i]);
            MessageInterface::ShowMessage("      . Range bias                 : %.12lf km \n", measurementBias[i]);
            MessageInterface::ShowMessage("      . Multiplier                 : %.12lf \n", multiplier);
         }
#endif

         // This section is only done when measurement type is ("Range_KM") "Range". For other types such as DSN_SeqRange or DSN_TCP, it will be done in their adapters  
         //if (measurementType == "Range_KM")
         if (measurementType == "Range")
         {
            // Apply multiplier for ("Range_KM") "Range" measurement model. This step has to
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
         MessageInterface::ShowMessage("      . Measurement is %s\n", (cMeasurement.isFeasible ? "feasible" : "unfeasible"));
         MessageInterface::ShowMessage("      . Feasibility reason          : %s\n", cMeasurement.unfeasibleReason.c_str());
         MessageInterface::ShowMessage("      . Elevation angle             : %.12lf degree\n", cMeasurement.feasibilityValue);
         MessageInterface::ShowMessage("      . Covariance matrix           : <%p>\n", cMeasurement.covariance);
         if (cMeasurement.covariance)
         {
            MessageInterface::ShowMessage("      . Covariance matrix size = %d\n", cMeasurement.covariance->GetDimension());
            MessageInterface::ShowMessage("     [ ");
            for (UnsignedInt i = 0; i < cMeasurement.covariance->GetDimension(); ++i)
            {
               if (i > 0)
                  MessageInterface::ShowMessage("\n");
               for (UnsignedInt j = 0; j < cMeasurement.covariance->GetDimension(); ++j)
                  MessageInterface::ShowMessage("%le   ", cMeasurement.covariance->GetCovariance()->GetElement(i, j));
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
   MessageInterface::ShowMessage("GNRangeAdapter::CalculateMeasurement(%s, "
      "<%p>, <%p>) exit\n", (withEvents ? "true" : "false"), forObservation,
      rampTB);
#endif

   return cMeasurement;
}



