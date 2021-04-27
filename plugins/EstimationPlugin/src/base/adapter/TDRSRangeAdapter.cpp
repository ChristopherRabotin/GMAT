//$Id$
//------------------------------------------------------------------------------
//                           TDRSRangeAdapter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Tuan D. Nguyen, GSFC/NASA.
// Created: Jan 12, 2017
/**
 * A measurement adapter for SN range measurement
 */
//------------------------------------------------------------------------------

#include "TDRSRangeAdapter.hpp"
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
// TDRSRangeAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
TDRSRangeAdapter::TDRSRangeAdapter(const std::string& name) :
   RangeAdapterKm(name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRSRangeAdapter default constructor <%p>\n", this);
#endif

   typeName = "SN_Range";           // change measurement type from "RangeKm" to "SN_Range" for SN Range 
}


//------------------------------------------------------------------------------
// ~TDRSRangeAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TDRSRangeAdapter::~TDRSRangeAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRSRangeAdapter default destructor  <%p>\n", this);
#endif
}


//------------------------------------------------------------------------------
// TDRSRangeAdapter(const TDRSRangeAdapter& rak)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rak The adapter copied to make this one
 */
//------------------------------------------------------------------------------
TDRSRangeAdapter::TDRSRangeAdapter(const TDRSRangeAdapter& rak) :
   RangeAdapterKm      (rak)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRSRangeAdapter copy constructor   from <%p> to <%p>\n", &rak, this);
#endif

}


//------------------------------------------------------------------------------
// TDRSRangeAdapter& operator=(const TDRSRangeAdapter& rak)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rak The adapter copied to make this one match it
 *
 * @return This adapter made to look like rak
 */
//------------------------------------------------------------------------------
TDRSRangeAdapter& TDRSRangeAdapter::operator=(const TDRSRangeAdapter& rak)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRSRangeAdapter operator =   set <%p> = <%p>\n", this, &rak);
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
GmatBase* TDRSRangeAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRSRangeAdapter::Clone() clone this <%p>\n", this);
#endif

   return new TDRSRangeAdapter(*this);
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
const MeasurementData& TDRSRangeAdapter::CalculateMeasurement(bool withEvents,
         ObservationData* forObservation, std::vector<RampTableData>* rampTB,
         bool forSimulation)
{
#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("TDRSRangeAdapter::CalculateMeasurement(%s, "
      "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
      rampTB);
#endif

   if (!calcData)
      throw MeasurementException("Measurement data was requested for " +
      instanceName + " before the measurement was set");

   // Fire the measurement model to build the collection of signal data
   if (calcData->CalculateMeasurement(withLighttime, withMediaCorrection, forObservation, rampTB, forSimulation))
   {
      // QA Media correction:
      cMeasurement.isIonoCorrectWarning = false;
      cMeasurement.ionoCorrectRawValue = 0.0;
      cMeasurement.ionoCorrectValue = 0.0;
      cMeasurement.isTropoCorrectWarning = false;
      cMeasurement.tropoCorrectRawValue = 0.0;
      cMeasurement.tropoCorrectValue = 0.0;

      if (withMediaCorrection)
      {
         Real correction = GetIonoCorrection();                                  // unit: km

         // Set a warning to measurement data when ionosphere correction is outside of range [0 km , 0.04 km]
         cMeasurement.isIonoCorrectWarning = (correction < 0.0) || (correction > 0.04);
         cMeasurement.ionoCorrectRawValue = correction;                   // unit: km

         correction = GetTropoCorrection();                                      // unit: km

         // Set a warning to measurement data when troposphere correction is outside of range [0 km , 0.12 km]
         cMeasurement.isTropoCorrectWarning = (correction < 0.0) || (correction > 0.12);
         cMeasurement.tropoCorrectRawValue = correction;                  // unit: km
      }

      std::vector<SignalBase*> paths = calcData->GetSignalPaths();
      std::string unfeasibilityReason;
      Real        unfeasibilityValue;

      // set to default
      cMeasurement.isFeasible = false;
      cMeasurement.unfeasibleReason = "";
      cMeasurement.feasibilityValue = 90.0;

      GmatTime transmitEpoch, receiveEpoch;
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
            if (current->feasibilityReason[0] == 'N')
            {
               if ((current->stationParticipant) && (cMeasurement.unfeasibleReason == ""))
               {
                  cMeasurement.isFeasible = true;
                  cMeasurement.unfeasibleReason = "N";
                  cMeasurement.feasibilityValue = current->feasibilityValue;
               }
            }
            else if (current->feasibilityReason[0] == 'B')
            {
               std::stringstream ss;
               ss << "B" << legIndex << current->feasibilityReason.substr(1);
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
               if (current->useCorrection[j] && current->correctionTypes[j] == "Range")
                  values[i] += current->corrections[j];
            }// for j loop

            // accumulate all hardware delays for signal path ith
            values[i] += ((current->tDelay + current->rDelay)*
               GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);

            // Get measurement epoch in the first signal path. It will apply for all other paths
            if (i == 0)
            {
               transmitEpoch = first->tPrecTime - first->tDelay / GmatTimeConstants::SECS_PER_DAY;
               receiveEpoch = current->rPrecTime + current->rDelay / GmatTimeConstants::SECS_PER_DAY;
               if (calcData->GetTimeTagFlag())
               {
                  // Measurement epoch will be at the end of signal path when time tag is at the receiver
                  if (current->next == NULL)
                  {
                     cMeasurement.epochGT = receiveEpoch;
                     cMeasurement.epoch   = receiveEpoch.GetMjd();
                  }
               }
               else
               {
                  // Measurement epoch will be at the begin of signal path when time tag is at the transmiter
                  cMeasurement.epochGT = transmitEpoch;
                  cMeasurement.epoch   = transmitEpoch.GetMjd();
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

      if (measurementType == "SN_Range")
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
         MessageInterface::ShowMessage("====  TDRSRangeAdapter (%s): Range Calculation for Measurement Data %dth  \n", GetName().c_str(), i);
         MessageInterface::ShowMessage("===================================================================\n");

         MessageInterface::ShowMessage("      . Path : ");
         for (UnsignedInt k = 0; k < participantLists[i]->size(); ++k)
            MessageInterface::ShowMessage("%s,  ", participantLists[i]->at(k).c_str());
         MessageInterface::ShowMessage("\n");

         MessageInterface::ShowMessage("      . Measurement epoch          : %.12lf\n", cMeasurement.epochGT.GetMjd());
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

         // This section is only done when measurement type is "SN_Range". For other types such as DSN_SeqRange or DSN_TCP, it will be done in their adapters  
         if (measurementType == "SN_Range")
         {
            // Apply multiplier for ("SN_Range") measurement model. This step has to
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

      cMeasurement.ionoCorrectValue = cMeasurement.ionoCorrectRawValue;
      cMeasurement.tropoCorrectValue = cMeasurement.tropoCorrectRawValue;

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
   MessageInterface::ShowMessage("TDRSRangeAdapter::CalculateMeasurement(%s, "
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
const std::vector<RealArray>& TDRSRangeAdapter::CalculateMeasurementDerivatives(
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
   MessageInterface::ShowMessage("TDRSRangeAdapter::CalculateMeasurement"
      "Derivatives(%s, %d) called; parm ID is %d; Epoch %.12lf\n",
      obj->GetFullName().c_str(), id, parameterID, cMeasurement.epoch);
#endif

   // Clear derivative variable
   for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      theDataDerivatives[i].clear();
   theDataDerivatives.clear();

   if (paramName == "Bias")
   {
      if (((ErrorModel*)obj)->GetStringParameter("Type") == "SN_Range")
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
      if (measurementType == "SN_Range")
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
   MessageInterface::ShowMessage("Exit TDRSRangeAdapter::CalculateMeasurementDerivatives():\n");
#endif

   return theDataDerivatives;
}

