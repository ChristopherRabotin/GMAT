//$Id$
//------------------------------------------------------------------------------
//                           RangeAdapterKm
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
#include "BodyFixedPoint.hpp"
#include "SpaceObject.hpp"
#include "PropSetup.hpp"
#include "ODEModel.hpp"
#include "Propagator.hpp"
#include <sstream>


//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_CONSTRUCTION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION
//#define DEBUG_RANGE_CALCULATION
//#define DEBUG_TAYLOR_SERIES
//#define DEBUG_API

#ifdef DEBUG_API
   #include <fstream>
   std::ofstream apiFile;
   bool apiFileOpen = false;
#endif

//---------------------------------
//  static data
//---------------------------------
const bool RangeAdapterKm::USE_TAYLOR_SERIES = true;
const bool RangeAdapterKm::USE_CHEBYSHEV_DIFFERENCE = true;

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
   TrackingDataAdapter("Range", name)                        //TrackingDataAdapter("RangeKm", name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RangeAdapterKm default constructor <%p>\n", this);
#endif

   #ifdef DEBUG_API
      if (!apiFileOpen)
      {
         apiFile.open("RangeAdapterKm_API.txt");
         apiFile << "API Debug data for RangeAdapterKm\n" << std::endl;
         apiFileOpen = true;
      }
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

   #ifdef DEBUG_API
      if (apiFileOpen)
      {
         apiFile.close();
         apiFileOpen = false;
      }
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
// bool RenameRefObject(const UnsignedInt type, const std::string& oldName,
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
bool RangeAdapterKm::RenameRefObject(const UnsignedInt type,
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
   
      isInitialized = true;
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
      ObservationData* forObservation, std::vector<RampTableData>* rampTB,
      bool forSimulation)
{
   #ifdef DEBUG_ADAPTER_EXECUTION
      MessageInterface::ShowMessage("RangeAdapterKm::CalculateMeasurement(%s, "
            "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
            rampTB);
   #endif

   #ifdef DEBUG_API
      if (apiFileOpen)
      {
         apiFile << "RangeAdapterKm::CalculateMeasurement("
                 << (withEvents ? "true, <" : "false, <")
                 << forObservation << ">, <"
                 << rampTB << ">) entered\n" << std::endl;
         apiFile << "calcData pointer: <"
                 << calcData << "> of type "
                 << (calcData!=NULL ? calcData->GetTypeName() : "undefined")
                 << std::endl;
      }
   #endif

   if (!calcData)
      throw MeasurementException("Measurement data was requested for " +
            instanceName + " before the measurement was set");
   
#ifdef DEBUG_API
   if (apiFileOpen)
   {
      apiFile << "   Calling calcData::CalculateMeasurement()" << std::endl;
   }
#endif
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

#ifdef DEBUG_API
   if (apiFileOpen)
   {
      apiFile << "   That worked; handling corrections" << std::endl;
   }
#endif

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

#ifdef DEBUG_API
   if (apiFileOpen)
   {
      apiFile << "   That worked; processing signal paths" << std::endl;
   }
#endif

      std::vector<SignalBase*> paths = calcData->GetSignalPaths();
      std::string unfeasibilityReason;
      Real        unfeasibilityValue;

      // set to default
      cMeasurement.isFeasible = false;
      cMeasurement.unfeasibleReason = "";
      cMeasurement.feasibilityValue = 90.0;

      GmatTime transmitEpoch, receiveEpoch;
      RealArray values, corrections;
      cMeasurement.rangeVecs.clear();
      cMeasurement.tBodies.clear();
      cMeasurement.rBodies.clear();
      cMeasurement.tPrecTimes.clear();
      cMeasurement.rPrecTimes.clear();
      cMeasurement.tLocs.clear();
      cMeasurement.rLocs.clear();
      for (UnsignedInt i = 0; i < paths.size(); ++i)           // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths 
      {
         // Calculate C-value for signal path ith:
         values.push_back(0.0);
         corrections.push_back(0.0);
         SignalBase *currentleg = paths[i];
         SignalData *current = ((currentleg == NULL)?NULL:(currentleg->GetSignalDataObject()));
         SignalData *first = current;
         UnsignedInt legIndex = 0;
         while (currentleg != NULL)
         {
            ++legIndex;
            // Set feasibility value
            if (current->feasibilityReason[0] == 'N')
            {
               if ((current->stationParticipant)&&(cMeasurement.unfeasibleReason == ""))
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
               if ((cMeasurement.unfeasibleReason == "")||(cMeasurement.unfeasibleReason == "N"))
               {
                  cMeasurement.unfeasibleReason = current->feasibilityReason; 
                  cMeasurement.isFeasible = false;
                  cMeasurement.feasibilityValue = current->feasibilityValue;
               }
            }

            // Get leg participants

            SpacePoint* body;
            BodyFixedPoint *bf;
            CoordinateSystem *cs;
            SpaceObject* spObj;

            if (current->tNode->IsOfType(Gmat::GROUND_STATION))
            {
               bf = (BodyFixedPoint*) current->tNode;
               cs = bf->GetBodyFixedCoordinateSystem();
               body = cs->GetOrigin();
            }
            else
            {
               if (current->tPropagator->GetPropagator()->UsesODEModel())
                  body = current->tPropagator->GetODEModel()->GetForceOrigin();
               else
                  body = current->tPropagator->GetPropagator()->GetPropOrigin();
            }
            cMeasurement.tBodies.push_back((CelestialBody*) body);

            if (current->rNode->IsOfType(Gmat::GROUND_STATION))
            {
               bf = (BodyFixedPoint*) current->rNode;
               cs = bf->GetBodyFixedCoordinateSystem();
               body = cs->GetOrigin();
            }
            else
            {
               if (current->rPropagator->GetPropagator()->UsesODEModel())
                  body = current->rPropagator->GetODEModel()->GetForceOrigin();
               else
                  body = current->rPropagator->GetPropagator()->GetPropOrigin();
            }
            cMeasurement.rBodies.push_back(body);

            cMeasurement.tPrecTimes.push_back(current->tPrecTime);
            cMeasurement.rPrecTimes.push_back(current->rPrecTime);
            cMeasurement.tLocs.push_back(new Rvector3(current->tLoc));
            cMeasurement.rLocs.push_back(new Rvector3(current->rLoc));
            
            // accumulate all light time range for signal path ith 
            Rvector3 signalVec = current->rangeVecInertial;
            cMeasurement.rangeVecs.push_back(new Rvector3(signalVec));
            values[i] += signalVec.GetMagnitude();               // unit: km

            // accumulate all range corrections for signal path ith
            for (UnsignedInt j = 0; j < current->correctionIDs.size(); ++j)
            {
               if (current->useCorrection[j] && current->correctionTypes[j] == "Range")
               {
                  values[i] += current->corrections[j];
                  corrections[i] += current->corrections[j];
               }
            }// for j loop
            
            // accumulate all hardware delays for signal path ith
            values[i] += ((current->tDelay + current->rDelay)*
                  GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);   // unit: km
            corrections[i] += ((current->tDelay + current->rDelay)*
                  GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);   // unit: km
            
            // Get measurement epoch in the first signal path. It will apply for all other paths
            if (i == 0)
            {
               transmitEpoch = first->tPrecTime - first->tDelay / GmatTimeConstants::SECS_PER_DAY;
               receiveEpoch  = current->rPrecTime + current->rDelay / GmatTimeConstants::SECS_PER_DAY;
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
            current = ((currentleg == NULL)?NULL:(currentleg->GetSignalDataObject()));

         }// while loop
         
      }// for i loop
      
#ifdef DEBUG_API
   if (apiFileOpen)
   {
      apiFile << "   Almost done: frequency handing is next" << std::endl;
   }
#endif


      // Caluclate uplink frequency at received time and transmit time
      cMeasurement.uplinkFreq = calcData->GetUplinkFrequency(0, rampTB) * 1.0e6;                        // unit: Hz
      cMeasurement.uplinkFreqAtRecei = calcData->GetUplinkFrequencyAtReceivedEpoch(0,rampTB) * 1.0e6;   // unit: Hz
      cMeasurement.uplinkBand = calcData->GetUplinkFrequencyBand(0, rampTB);
      
#ifdef DEBUG_API
   if (apiFileOpen)
   {
      apiFile << "   Then Bias, Noise, and Error Covariance" << std::endl;
   }
#endif

      //if (measurementType == "Range_KM")
      if (measurementType == "Range")
      {
         // @todo: it needs to specify number of trips instead of using 2
         //ComputeMeasurementBias("Bias", "Range_KM", 2);
         ComputeMeasurementBias("Bias", measurementType, 2);
#ifdef DEBUG_API
   if (apiFileOpen)
   {
      apiFile << "      Bias computed" << std::endl;
   }
#endif
         //ComputeMeasurementNoiseSigma("NoiseSigma", "Range_KM", 2);
         ComputeMeasurementNoiseSigma("NoiseSigma", measurementType, 2);
#ifdef DEBUG_API
   if (apiFileOpen)
   {
      apiFile << "      Noise computed" << std::endl;
   }
#endif
         ComputeMeasurementErrorCovarianceMatrix();
#ifdef DEBUG_API
   if (apiFileOpen)
   {
      apiFile << "      Error covariance computed" << std::endl;
   }
#endif
      }

#ifdef DEBUG_API
   if (apiFileOpen)
   {
      apiFile << "   Setting values" << std::endl;
   }
#endif

      // Set measurement values
      cMeasurement.value.clear();
      cMeasurement.correction.clear();
      for (UnsignedInt i = 0; i < values.size(); ++i)
      {
         cMeasurement.value.push_back(0.0);
         cMeasurement.correction.push_back(0.0);
      }

      for (UnsignedInt i = 0; i < values.size(); ++i)
      {
         Real measVal = values[i];                    // unit: km
         Real corrVal = corrections[i];               // unit: km
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
            corrVal = corrVal*multiplier;

            // if need range value only, skip this section, otherwise add noise and bias as possible
            // Note: for Doppler measurement for E and S paths, we only need range value only and no noise and bias are added to measurement value. 
            if (!rangeOnly)
            {
               // Add noise to measurement value
               if (addNoise)
               {
                  // Add noise here
                  RandomNumber* rn = RandomNumber::Instance();
                  Real val = rn->Gaussian(0.0, noiseSigma[i]);                  // noise sigma unit: Km
                  //val = rn->Gaussian(measVal, noiseSigma[i]);
                  measVal += val;
                  corrVal += val;
               }

               // Add bias to measurement value only after noise had been added in order to avoid adding bias' noise 
               if (addBias)
               {
                  measVal = measVal + measurementBias[i];                          // bias unit: Km
                  corrVal = corrVal + measurementBias[i];                          // bias unit: Km
               }
            }
         }
         cMeasurement.value[i] = measVal;                                          // unit: km
         cMeasurement.correction[i] = corrVal;                                          // unit: km

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

#ifdef DEBUG_API
   if (apiFileOpen)
   {
      apiFile << "   Covariance computations" << std::endl;
   }
#endif



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

#ifdef DEBUG_API
   if (apiFileOpen)
   {
      apiFile << "Finished; what an adventure!" << std::endl;
   }
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
      std::vector<RampTableData>* rampTB, bool forSimulation)
{
   static MeasurementData offsetMeas;

   if (!calcData)
      throw MeasurementException("Measurement data was requested for " +
            instanceName + " before the measurement was set");

   // Fire the measurement model to build the collection of signal data
   if (calcData->CalculateMeasurement(withLighttime, withMediaCorrection, forObservation, rampTB, forSimulation, dt))
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
            if (current->feasibilityReason[0] == 'N')
            {
               if (current->stationParticipant)
                  offsetMeas.feasibilityValue = current->feasibilityValue;
            }
            else if (current->feasibilityReason[0] == 'B')
            {
               std::stringstream ss;
               ss << "B" << legIndex << current->feasibilityReason.substr(1);
               current->feasibilityReason = ss.str();
               if (offsetMeas.unfeasibleReason[0] == 'N')
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
               if (current->useCorrection[j] && current->correctionTypes[j] == "Range")
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
                  {
                     offsetMeas.epochGT = current->rPrecTime + current->rDelay / GmatTimeConstants::SECS_PER_DAY;
                     offsetMeas.epoch   = current->rPrecTime.GetMjd() + current->rDelay / GmatTimeConstants::SECS_PER_DAY;
                  }
               }
               else
               {
                  // Measurement epoch will be at the begin of signal path when time tag is at the transmiter
                  offsetMeas.epochGT = first->tPrecTime - first->tDelay / GmatTimeConstants::SECS_PER_DAY;
                  offsetMeas.epoch   = first->tPrecTime.GetMjd() - first->tDelay/GmatTimeConstants::SECS_PER_DAY;
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
      //if (((ErrorModel*)obj)->GetStringParameter("Type") == "Range_KM")
      if (((ErrorModel*)obj)->GetStringParameter("Type") == "Range")
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
      //if (measurementType == "Range_KM")
      factor = ApplyMultiplier(measurementType, factor, obj);
      
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

//------------------------------------------------------------------------------
// Real ApplyMultiplier(const std::string& useMeasType, 
// const Real factor, const GmatBase* obj)
//------------------------------------------------------------------------------
/**
 * Resolves a multiplier based on the passed in measurment type
 *
 * @param useMeasType The measurement type to use
 * @param factor The initial factor
 * @param obj The GMAT object in the current path 
 *
 */
 //------------------------------------------------------------------------------
Real RangeAdapterKm::ApplyMultiplier(const std::string& useMeasType, 
     const Real factor, const GmatBase* obj)
{
  Real multFactor = factor;

  if (useMeasType == "Range")
  {
     if (obj->IsOfType(Gmat::SPACECRAFT))
     {
        multFactor = multiplier;
     }
  }

  return multFactor;
}

Real RangeAdapterKm::PathMagnitudeDelta(Rvector3 pathVec, Rvector3 delta)
{
   Real Delta = 0;
   
   Real a, b, c, D;
   Rvector3 D_unit;
   Real tolerance = 1.0e-9;
   Real rx, ry, rz, r1;

   D = delta.GetMagnitude();
   D_unit = delta / D;

   a=D_unit[0];
   b=D_unit[1];
   c=D_unit[2];

   rx=pathVec[0];
   ry=pathVec[1];
   rz=pathVec[2];
   r1=pathVec.GetMagnitude();


   Real term1, term2, term3;
   Real D2 = D*D;
   Real D3 = D2*D;
   Real r12 = r1*r1;
   Real r13 = r12*r1;
   Real r14 = r13*r1;
   Real r15 = r14*r1;

   term1 = (D*(a*rx + b*ry + c*rz))/r1;
   term2 = (D2*(r12 - GmatMathUtil::Pow(a*rx + b*ry + c*rz,2)))/(2*r13);
   term3 = -((D3*(a*rx + b*ry + c*rz)*(r12 - GmatMathUtil::Pow(a*rx + b*ry + c*rz,2)))/(2*r15));

   Delta = term1 + term2 + term3;
   
#ifdef DEBUG_TAYLOR_SERIES
   MessageInterface::ShowMessage("   Taylor series terms to tolerance %le:\n", tolerance);
   MessageInterface::ShowMessage("      term1 = %.12le\n", term1);
   MessageInterface::ShowMessage("      term2 = %.12le\n", term2);
   MessageInterface::ShowMessage("      term3 = %.12le\n", term3);
#endif

   if (GmatMathUtil::Abs(term3) < tolerance)
      return Delta;

   Real term;
   Real a2, b2, c2;
   Real a3, b3, c3;
   Real a4, b4, c4;
   Real rx2, ry2, rz2;
   Real rx4, ry4, rz4;
   a2 = a*a;
   b2 = b*b;
   c2 = c*c;
   rx2 = rx*rx;
   ry2 = ry*ry;
   rz2 = rz*rz;

   UnsignedInt numTerms = 7;

   if (numTerms >= 6U)
   {
      a3 = a2*a;
      b3 = b2*b;
      c3 = c2*c;
      a4 = a3*a;
      b4 = b3*b;
      c4 = c3*c;
      rx4 = rx2*rx2;
      ry4 = ry2*ry2;
      rz4 = rz2*rz2;
   }

   for (UnsignedInt termNumber = 4; termNumber <= numTerms; termNumber++)
   {
      switch (termNumber)
      {
      case(4) :
         term = -((1/(8*GmatMathUtil::Pow(r1,7)))*(GmatMathUtil::Pow(D,4)*(a2*(r1 - rx)*(r1 + rx) + b2*(r1 - ry)*(r1 + ry) -
                   2*b*c*ry*rz + c2*(r1 - rz)*(r1 + rz) - 2*a*rx*(b*ry + c*rz))*
                   (a2*(r12 - 5*rx2) + b2*(r12 - 5*ry2) - 10*b*c*ry*rz -
                   10*a*rx*(b*ry + c*rz) + c2*(r12 - 5*rz2))));
         break;
      case(5) :
         term = (1/(8*GmatMathUtil::Pow(r1,9)))*(GmatMathUtil::Pow(D,5)*(a*rx + b*ry + c*rz)*(a2*(-r12 + rx2) + b2*(-r12 + ry2) +
                   2*b*c*ry*rz + 2*a*rx*(b*ry + c*rz) + c2*(-r12 + rz2))*
                   (a2*(-3*r12 + 7*rx2) + b2*(-3*r12 + 7*ry2) + 14*b*c*ry*rz +
                   14*a*rx*(b*ry + c*rz) + c2*(-3*r12 + 7*rz2)));
         break;
      case(6) :
         term = (1/(16*GmatMathUtil::Pow(r1,11)))*(GmatMathUtil::Pow(D,6)*(a2*(r1 - rx)*(r1 + rx) + b2*(r1 - ry)*(r1 + ry) -
                   2*b*c*ry*rz + c2*(r1 - rz)*(r1 + rz) - 2*a*rx*(b*ry + c*rz))*
                   (a4*(r14 - 14*r12*rx2 + 21*rx4) + b4*(r14 - 14*r12*ry2 + 21*ry4) -
                   28*b3*c*ry*(r12 - 3*ry2)*rz - 28*a3*rx*(r12 - 3*rx2)*(b*ry + c*rz) -
                   28*b*c3*ry*rz*(r12 - 3*rz2) + c4*(r14 - 14*r12*rz2 + 21*rz4) -
                   28*a*rx*(b*ry + c*rz)*(b2*(r12 - 3*ry2) - 6*b*c*ry*rz +
                   c2*(r12 - 3*rz2)) + 2*b2*c2*(r14 + 63*ry2*rz2 -
                   7*r12*(ry2 + rz2)) +
                   2*a2*(b2*(r14 + 63*rx2*ry2 - 7*r12*(rx2 + ry2)) -
                   14*b*c*(r12 - 9*rx2)*ry*rz + c2*(r14 + 63*rx2*rz2 -
                   7*r12*(rx2 + rz2)))));
         break;
      case(7) :
         term = (1/(16*GmatMathUtil::Pow(r1,13)))*(GmatMathUtil::Pow(D,7)*(a*rx + b*ry + c*rz)*(a2*(-r12 + rx2) +
                   b2*(-r12 + ry2) + 2*b*c*ry*rz + 2*a*rx*(b*ry + c*rz) + c2*(-r12 + rz2))*
                   (a4*(5*r14 - 30*r12*rx2 + 33*rx4) +
                   b4*(5*r14 - 30*r12*ry2 + 33*ry4) + 12*b3*c*ry*(-5*r12 + 11*ry2)*rz +
                   12*a3*rx*(-5*r12 + 11*rx2)*(b*ry + c*rz) +
                   12*b*c3*ry*rz*(-5*r12 + 11*rz2) + c4*(5*r14 - 30*r12*rz2 + 33*rz4) +
                   2*b2*c2*(5*r14 + 99*ry2*rz2 - 15*r12*(ry2 + rz2)) +
                   12*a*rx*(b*ry + c*rz)*(b2*(-5*r12 + 11*ry2) + 22*b*c*ry*rz +
                   c2*(-5*r12 + 11*rz2)) +
                   2*a2*(b2*(5*r14 + 99*rx2*ry2 - 15*r12*(rx2 + ry2)) +
                   6*b*c*(-5*r12 + 33*rx2)*ry*rz + c2*(5*r14 + 99*rx2*rz2 -
                   15*r12*(rx2 + rz2)))));
         break;
      default:
         
#ifdef DEBUG_TAYLOR_SERIES
         MessageInterface::ShowMessage("      Warning:  Last term of Taylor Series expansion used for "
                                       "Doppler type measurement was greater than tolerance %le\n", tolerance);
#endif
         return Delta;
         break;
      }

      Delta += term;

#ifdef DEBUG_TAYLOR_SERIES
      MessageInterface::ShowMessage("      term%d = %.12le\n", termNumber, term);
#endif
      
      if (GmatMathUtil::Abs(term) < tolerance)
         return Delta;
   }
         
#ifdef DEBUG_TAYLOR_SERIES
   MessageInterface::ShowMessage("      Warning:  Last term of Taylor Series expansion used for "
                                 "Doppler type measurement was greater than tolerance %le\n", tolerance);
#endif

   return Delta;
}
