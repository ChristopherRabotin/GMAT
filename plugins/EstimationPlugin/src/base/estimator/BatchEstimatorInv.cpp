//$Id: BatchEstimatorInv.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         BatchEstimatorInv
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/08/04
//
/**
 * Batch least squares estimator using direct inversion
 */
//------------------------------------------------------------------------------

#include "BatchEstimatorInv.hpp"
#include "MessageInterface.hpp"
#include "EstimatorException.hpp"
#include <sstream>
#include "StringUtil.hpp"


//#define DEBUG_ACCUMULATION
//#define DEBUG_ACCUMULATION_RESULTS
//#define WALK_STATE_MACHINE
//#define DEBUG_VERBOSE
//#define DEBUG_WEIGHTS
//#define DEBUG_O_MINUS_C
//#define DEBUG_SCHUR
//#define DEBUG_INVERSION
//#define DEBUG_STM

#define NEW_REPORTFILE_FORMAT

//------------------------------------------------------------------------------
// BatchEstimatorInv(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name Name of the instance being constructed
 */
//------------------------------------------------------------------------------
BatchEstimatorInv::BatchEstimatorInv(const std::string &name):
   BatchEstimator       ("BatchEstimatorInv", name)
{
   objectTypeNames.push_back("BatchEstimatorInv");
}


//------------------------------------------------------------------------------
// ~BatchEstimatorInv()
//------------------------------------------------------------------------------
/**
 * Class destructor
 */
//------------------------------------------------------------------------------
BatchEstimatorInv::~BatchEstimatorInv()
{
}


//------------------------------------------------------------------------------
// BatchEstimatorInv(const BatchEstimatorInv& est)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param est The object that is being copied
 */
//------------------------------------------------------------------------------
BatchEstimatorInv::BatchEstimatorInv(const BatchEstimatorInv& est) :
   BatchEstimator       (est)
{

}


//------------------------------------------------------------------------------
// BatchEstimatorInv& operator=(const BatchEstimatorInv& est)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param est The object providing configuration data for this object
 *
 * @return This object, configured to match est
 */
//------------------------------------------------------------------------------
BatchEstimatorInv& BatchEstimatorInv::operator=(const BatchEstimatorInv& est)
{
   if (this != &est)
   {
      BatchEstimator::operator=(est);
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Object cloner
 *
 * @return Pointer to a new BatchEstimatorInv configured to match this one.
 */
//------------------------------------------------------------------------------
GmatBase* BatchEstimatorInv::Clone() const
{
   return new BatchEstimatorInv(*this);
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void  BatchEstimatorInv::Copy(const GmatBase* orig)
{
   operator=(*((BatchEstimatorInv*)(orig)));
}


//------------------------------------------------------------------------------
//  void Accumulate()
//------------------------------------------------------------------------------
/**
 * This method collects the data needed for estimation.
 */
//------------------------------------------------------------------------------
void BatchEstimatorInv::Accumulate()
{
#ifdef WALK_STATE_MACHINE
   MessageInterface::ShowMessage("BatchEstimator state is ACCUMULATING\n");
#endif

#ifdef DEBUG_ACCUMULATION
   MessageInterface::ShowMessage("Entered BatchEstimatorInv::Accumulate()\n");
#endif

   // Measurements are possible!
   const MeasurementData *calculatedMeas;
   std::vector<RealArray> stateDeriv;

   for (UnsignedInt i = 0; i < hTilde.size(); ++i)
      hTilde[i].clear();
   hTilde.clear();

   // Get state map, measurement models, and measurement data
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();
   modelsToAccess = measManager.GetValidMeasurementList();            // Get valid measurement models
   const ObservationData *currentObs = measManager.GetObsData();      // Get current observation data


   // Get ground station and measurement type from current observation data
   std::string gsName = currentObs->participantIDs[0];
   std::string typeName = currentObs->typeName;
   std::string keyword = gsName + " " + typeName;

   bool found = false;
   UnsignedInt indexKey = 0;
   for (; indexKey < stationAndType.size(); ++indexKey)
   {
      if (stationAndType[indexKey] == keyword)
      {
         sumAllRecords[indexKey] += 1;            // count for the total records
         found = true;
         break;
      }
   }
   if (!found)
   {
      // create a statistics record for a new combination of station and measuement type 
      stationAndType.push_back(keyword);
      stationsList.push_back(gsName);
      measTypesList.push_back(typeName);
      sumAllRecords.push_back(1);                // set total records to 1 at the first time
      sumAcceptRecords.push_back(0);
      sumResidual.push_back(0.0);
      sumResidualSquare.push_back(0.0);
      sumWeightResidualSquare.push_back(0.0);

      sumSERecords.push_back(0);
      sumSEResidual.push_back(0.0);
      sumSEResidualSquare.push_back(0.0);
      sumSEWeightResidualSquare.push_back(0.0);
   }


   // Set initial value for 2 statistics tables
   if (statisticsTable["TOTAL NUM RECORDS"].find(keyword) == statisticsTable["TOTAL NUM RECORDS"].end())
      statisticsTable["TOTAL NUM RECORDS"][keyword] = 0.0;
   if (statisticsTable["ACCEPTED RECORDS"].find(keyword) == statisticsTable["ACCEPTED RECORDS"].end())
      statisticsTable["ACCEPTED RECORDS"][keyword] = 0.0;
   if (statisticsTable["WEIGHTED RMS"].find(keyword) == statisticsTable["WEIGHTED RMS"].end())
      statisticsTable["WEIGHTED RMS"][keyword] = 0.0;
   if (statisticsTable["MEAN RESIDUAL"].find(keyword) == statisticsTable["MEAN RESIDUAL"].end())
      statisticsTable["MEAN RESIDUAL"][keyword] = 0.0;
   if (statisticsTable["STANDARD DEVIATION"].find(keyword) == statisticsTable["STANDARD DEVIATION"].end())
      statisticsTable["STANDARD DEVIATION"][keyword] = 0.0;

   if (statisticsTable1["TOTAL NUM RECORDS"].find(typeName) == statisticsTable1["TOTAL NUM RECORDS"].end())
      statisticsTable1["TOTAL NUM RECORDS"][typeName] = 0.0;
   if (statisticsTable1["ACCEPTED RECORDS"].find(typeName) == statisticsTable1["ACCEPTED RECORDS"].end())
      statisticsTable1["ACCEPTED RECORDS"][typeName] = 0.0;
   if (statisticsTable1["WEIGHTED RMS"].find(typeName) == statisticsTable1["WEIGHTED RMS"].end())
      statisticsTable1["WEIGHTED RMS"][typeName] = 0.0;
   if (statisticsTable1["MEAN RESIDUAL"].find(typeName) == statisticsTable1["MEAN RESIDUAL"].end())
      statisticsTable1["MEAN RESIDUAL"][typeName] = 0.0;
   if (statisticsTable1["STANDARD DEVIATION"].find(typeName) == statisticsTable1["STANDARD DEVIATION"].end())
      statisticsTable1["STANDARD DEVIATION"][typeName] = 0.0;

   // count total number of observation data for a pair of groundstation and measurement type
   statisticsTable["TOTAL NUM RECORDS"][keyword] += 1;
   statisticsTable1["TOTAL NUM RECORDS"][typeName] += 1;

#ifdef DEBUG_ACCUMULATION
   MessageInterface::ShowMessage("StateMap size is %d\n", stateMap->size());
   MessageInterface::ShowMessage("Found %d models\n", modelsToAccess.size());

   MessageInterface::ShowMessage("Observation data O: epoch = %.12lf   type = <%s>", currentObs->epoch, currentObs->typeName.c_str());
   for(UnsignedInt i=0; i < currentObs->participantIDs.size(); ++i)
      MessageInterface::ShowMessage("   %s", currentObs->participantIDs[i].c_str());

   MessageInterface::ShowMessage("   O = %.12lf", currentObs->value[0]);

   if ((currentObs->typeName == "DSNRange")||(currentObs->typeName == "DSNTwoWayRange"))
      MessageInterface::ShowMessage("   range modulo = %lf\n", currentObs->rangeModulo);
   MessageInterface::ShowMessage("\n");
#endif

   std::stringstream sLine;
   char s[1000];
   std::string times;
   Real temp;

   // Write to report file iteration number, record number, and time:
   TimeConverterUtil::Convert("A1ModJulian", currentObs->epoch, "", "UTCGregorian", temp, times, 1);
   if (textFileMode == "Normal")
   {
#ifdef NEW_REPORTFILE_FORMAT
      sprintf(&s[0], "%4d %6d  %s  ", iterationsTaken, measManager.GetCurrentRecordNumber(), times.c_str());
#else
      sprintf(&s[0], "%4d  %8d   %s   ", iterationsTaken, measManager.GetCurrentRecordNumber(), times.c_str());
#endif

   }
   else
   {
      Real timeTAI = TimeConverterUtil::Convert(currentObs->epoch, currentObs->epochSystem, TimeConverterUtil::TAIMJD);
#ifdef NEW_REPORTFILE_FORMAT
      sprintf(&s[0], "%4d   %6d  %s  %.12lf ", iterationsTaken, measManager.GetCurrentRecordNumber(), times.c_str(), timeTAI);
#else
      sprintf(&s[0],"%4d  %8d   %s  %.12lf        ", iterationsTaken, measManager.GetCurrentRecordNumber(), times.c_str(), timeTAI);
#endif
   }
   sLine << s;


#ifdef NEW_REPORTFILE_FORMAT
   std::string ss;
   if (textFileMode == "Normal")
   {
      // Write to report file measurement type name:
      sLine << GmatStringUtil::GetAlignmentString(currentObs->typeName, 19) + " ";

      // Write to report file all participants in signal path:
      ss = "";
      for (UnsignedInt n = 0; n < currentObs->participantIDs.size(); ++n)
         ss = ss + currentObs->participantIDs[n] + (((n + 1) == currentObs->participantIDs.size()) ? "" : ",");
      sLine << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(ss), pcolumnLen) + " ";                                          // made changes by TUAN NGUYEN
   }
   else
   {
      // Write to report file measurement type name and its unit:
      ss = currentObs->typeName + "                    ";
      sLine << ss.substr(0, 20) << " ";

      ss = currentObs->unit + "    ";
      sLine << ss.substr(0, 4) << " ";

      // Write to report file all participants in signal path:
      ss = "";
      for (UnsignedInt n = 0; n < currentObs->participantIDs.size(); ++n)
         ss = ss + currentObs->participantIDs[n] + (((n + 1) == currentObs->participantIDs.size()) ? "" : ",");
      sLine << GmatStringUtil::GetAlignmentString(ss, pcolumnLen) + " ";                                          // made changes by TUAN NGUYEN
   }
#else
   // Write to report file measurement type name and its unit:
   std::string ss = currentObs->typeName + "                    ";
   sLine << ss.substr(0,20) << " ";

   ss = currentObs->unit + "    ";
   sLine << ss.substr(0,4) << " ";


   // Write to report file all participants in signal path:
   ss = "";
   for (UnsignedInt n = 0; n < currentObs->participantIDs.size(); ++n)
      ss = ss + currentObs->participantIDs[n] + (((n + 1) == currentObs->participantIDs.size()) ? "" : ",");
   sLine << GmatStringUtil::GetAlignmentString(ss, pcolumnLen) + " ";                                          // made changes by TUAN NGUYEN
#endif


   if (modelsToAccess.size() == 0)
   {
      // Count number of records removed by measurement model unmatched 
      numRemovedRecords["U"]++;
      measManager.GetObsDataObject()->inUsed = false;
      measManager.GetObsDataObject()->removedReason = "U";

#ifdef NEW_REPORTFILE_FORMAT
      if (textFileMode == "Normal")
      {
         // Write to report file edit status:
         sLine << GmatStringUtil::GetAlignmentString(measManager.GetObsDataObject()->removedReason, 4) + " ";

         // Write to report file O-value, C-value, O-C, and elevation angle 
         sprintf(&s[0], "%22.6lf                     N/A                  N/A    N/A", currentObs->value[0]);
         sLine << s;
         sLine << "\n";
      }
      else
      {
         // Write to report file edit status:
         sLine << GmatStringUtil::GetAlignmentString(measManager.GetObsDataObject()->removedReason, 10, GmatStringUtil::LEFT);

         // This line crashes on Linux, so comment out there
         sprintf(&s[0],"%22.6lf   %22.6lf                      N/A                  N/A                   N/A                   N/A                   N/A                  N/A", currentObs->value_orig[0], currentObs->value[0]);
         sLine << s;

         // fill out N/A for partial derivative
         for (int i = 0; i < stateMap->size(); ++i)
            sLine << "                      N/A";

         if ((currentObs->typeName == "DSNTwoWayRange")||(currentObs->typeName == "DSNRange"))
            sprintf(&s[0],"            %d   %.15le   %.15le                     N/A", currentObs->uplinkBand, currentObs->uplinkFreq, currentObs->rangeModulo);
         else if ((currentObs->typeName == "DSNTwoWayDoppler")||(currentObs->typeName == "Doppler")||(currentObs->typeName == "Doppler_RangeRate"))                        // made changes by TUAN NGUYEN
            sprintf(&s[0],"            %d                      N/A                      N/A                 %.4lf", currentObs->uplinkBand, currentObs->dopplerCountInterval);
         else
            sprintf(&s[0],"          N/A                      N/A                      N/A                     N/A");
         sLine << s;
         sLine << "\n";
      }
#else
      // Write to report file edit status:
      sLine << GmatStringUtil::GetAlignmentString(measManager.GetObsDataObject()->removedReason, 10, GmatStringUtil::LEFT);

	  // This line crashes on Linux, so comment out there
      sprintf(&s[0],"%22.6lf   %22.6lf                      N/A                  N/A                   N/A                   N/A                   N/A                  N/A", currentObs->value_orig[0], currentObs->value[0]);
      sLine << s;

      if (textFileMode != "Normal")
      {
         // fill out N/A for partial derivative
         for (int i = 0; i < stateMap->size(); ++i)
            sLine << "                      N/A";

         if ((currentObs->typeName == "DSNTwoWayRange")||(currentObs->typeName == "DSNRange"))
            sprintf(&s[0],"            %d   %.15le   %.15le                     N/A", currentObs->uplinkBand, currentObs->uplinkFreq, currentObs->rangeModulo);
         else if ((currentObs->typeName == "DSNTwoWayDoppler") || (currentObs->typeName == "Doppler") || (currentObs->typeName == "Doppler_RangeRate"))                        // made changes by TUAN NGUYEN
            sprintf(&s[0], "            %d                      N/A                      N/A                 %.4lf", currentObs->uplinkBand, currentObs->dopplerCountInterval);
         else
            sprintf(&s[0], "          N/A                      N/A                      N/A                     N/A");
         sLine << s;
      }
      sLine << "\n";
#endif

   }
   else
   {
      int count = measManager.Calculate(modelsToAccess[0], true);
      calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);
      
      if (count == 0)
      {
         std::string ss = measManager.GetObsDataObject()->removedReason = calculatedMeas->unfeasibleReason;
         if (ss.substr(0,1) == "B")
            numRemovedRecords["B"]++;
         else
            numRemovedRecords[ss]++;

#ifdef NEW_REPORTFILE_FORMAT
         if (textFileMode == "Normal")
         {
            // Write to report file edit status:
            sLine << GmatStringUtil::GetAlignmentString(ss, 4, GmatStringUtil::LEFT) + " ";

            // Write to report file O-value, C-value, O-C, unit, and elevation angle
            sprintf(&s[0], "%22.6lf                     N/A                  N/A ", currentObs->value[0]);
            sLine << s;
            sprintf(&s[0], "%6.2lf", calculatedMeas->feasibilityValue);
            sLine << s;
            sLine << "\n";
         }
         else
         {
            // Write to report file edit status:
            sLine << GmatStringUtil::GetAlignmentString(ss, 10, GmatStringUtil::LEFT);

            sprintf(&s[0], "%22.6lf   %22.6lf                      N/A                  N/A                   N/A                   N/A                   N/A   %18.12lf", currentObs->value_orig[0], currentObs->value[0], calculatedMeas->feasibilityValue);
            sLine << s;

            // fill out N/A for partial derivative
            for (int i = 0; i < stateMap->size(); ++i)
               sLine << "                      N/A";

            if ((currentObs->typeName == "DSNTwoWayRange")||(currentObs->typeName == "DSNRange"))
               sprintf(&s[0],"            %d   %.15le   %.15le                     N/A", currentObs->uplinkBand, currentObs->uplinkFreq, currentObs->rangeModulo);
            else if ((currentObs->typeName == "DSNTwoWayDoppler")||(currentObs->typeName == "Doppler")||(currentObs->typeName == "Doppler_RangeRate"))
               sprintf(&s[0],"            %d                      N/A                      N/A                 %.4lf", currentObs->uplinkBand, currentObs->dopplerCountInterval);
            else
               sprintf(&s[0],"          N/A                      N/A                      N/A                     N/A");
            sLine << s;
            sLine << "\n";
         }
#else
         // Write to report file edit status:
         sLine << GmatStringUtil::GetAlignmentString(ss, 10, GmatStringUtil::LEFT);

         sprintf(&s[0], "%22.6lf   %22.6lf                      N/A                  N/A                   N/A                   N/A                   N/A   %18.12lf", currentObs->value_orig[0], currentObs->value[0], calculatedMeas->feasibilityValue);
         sLine << s;

         if (textFileMode != "Normal")
         {
            // fill out N/A for partial derivative
            for (int i = 0; i < stateMap->size(); ++i)
               sLine << "                      N/A";

            if ((currentObs->typeName == "DSNTwoWayRange")||(currentObs->typeName == "DSNRange"))
               sprintf(&s[0], "            %d   %.15le   %.15le                     N/A", currentObs->uplinkBand, currentObs->uplinkFreq, currentObs->rangeModulo);
            else if ((currentObs->typeName == "DSNTwoWayDoppler") || (currentObs->typeName == "Doppler") || (currentObs->typeName == "Doppler_RangeRate"))
               sprintf(&s[0], "            %d                      N/A                      N/A                 %.4lf", currentObs->uplinkBand, currentObs->dopplerCountInterval);
            else
               sprintf(&s[0], "          N/A                      N/A                      N/A                     N/A");
            sLine << s;
         }
         sLine << "\n";
#endif
      }
      else // (count >= 1)
      {
         // Currently assuming uniqueness; modify if more than 1 possible here
         // Case: ((modelsToAccess.size() > 0) && (measManager.Calculate(modelsToAccess[0], true) >= 1))
         
         // It has to make correction for observation value before running data filter
         if ((iterationsTaken == 0)&&((currentObs->typeName == "DSNTwoWayRange")||(currentObs->typeName == "DSNRange")))
         {
            // value correction is only applied for DSNTwoWayRange and it is only performed at the first time
            for (Integer index = 0; index < currentObs->value.size(); ++index)
               measManager.GetObsDataObject()->value[index] = ObservationDataCorrection(calculatedMeas->value[index], currentObs->value[index], currentObs->rangeModulo);
         }
         
         bool isReUsed = DataFilter();
         
         #ifdef DEBUG_ACCUMULATION
         MessageInterface::ShowMessage("iterationsTaken = %d    inUsed = %s\n", iterationsTaken, (measManager.GetObsDataObject()->inUsed ? "true" : "false"));
         #endif
         
         if (measManager.GetObsDataObject()->inUsed == false)
         {
            // Specify reamoved reason and count number of removed records
            std::string ss = measManager.GetObsDataObject()->removedReason;         //currentObs->removedReason;
            if (ss.substr(0,1) == "B")
               numRemovedRecords["B"]++;
            else
               numRemovedRecords[ss]++;

            Real ocDiff = currentObs->value[0]-calculatedMeas->value[0];
            Real weight;
            if ((*(calculatedMeas->covariance))(0,0) != 0.0)
               weight = 1.0 / (*(calculatedMeas->covariance))(0,0);
            else
               weight = 1.0;

#ifdef NEW_REPORTFILE_FORMAT
            if (textFileMode == "Normal")
            {
               // Write to report file edit status:
               sLine << GmatStringUtil::GetAlignmentString(ss, 4, GmatStringUtil::LEFT) + " ";

               // Write O-value, C-value, and O-C
               sprintf(&s[0], "%22.6lf  %22.6lf   %18.6lf ", currentObs->value[0], calculatedMeas->value[0], ocDiff);
               sLine << s;
               // Write to report file elevation angle:
               sprintf(&s[0], "%6.2lf", calculatedMeas->feasibilityValue);
               sLine << s;
               sLine << "\n";
            }
            else
            {
               // Write to report file edit status:
               sLine << GmatStringUtil::GetAlignmentString(ss, 10, GmatStringUtil::LEFT);

               sprintf(&s[0], "%22.6lf   %22.6lf   %22.6lf   %18.6lf    %.12le   %.12le   %.12le   %18.12lf", currentObs->value_orig[0], currentObs->value[0], calculatedMeas->value[0], ocDiff, weight, ocDiff*ocDiff*weight, sqrt(weight)*abs(ocDiff), calculatedMeas->feasibilityValue);
               sLine << s;

               // fill out N/A for partial derivative
               for (int i = 0; i < stateMap->size(); ++i)
                  sLine << "                      N/A";

               if ((currentObs->typeName == "DSNTwoWayRange")||(currentObs->typeName == "DSNRange"))
                  sprintf(&s[0],"            %d   %.15le   %.15le                     N/A", currentObs->uplinkBand, currentObs->uplinkFreq, currentObs->rangeModulo);
               else if ((currentObs->typeName == "DSNTwoWayDoppler")||(currentObs->typeName == "Doppler")||(currentObs->typeName == "Doppler_RangeRate"))
                  sprintf(&s[0],"            %d                      N/A                      N/A                 %.4lf", currentObs->uplinkBand, currentObs->dopplerCountInterval);
               else
                  sprintf(&s[0],"          N/A                      N/A                      N/A                     N/A");
               sLine << s;
               sLine << "\n";
            }
#else
            // Write to report file edit status:
            sLine << GmatStringUtil::GetAlignmentString(ss, 10, GmatStringUtil::LEFT);

            sprintf(&s[0], "%22.6lf   %22.6lf   %22.6lf   %18.6lf    %.12le   %.12le   %.12le   %18.12lf", currentObs->value_orig[0], currentObs->value[0], calculatedMeas->value[0], ocDiff, weight, ocDiff*ocDiff*weight, sqrt(weight)*abs(ocDiff), calculatedMeas->feasibilityValue);
            sLine << s;

            if (textFileMode != "Normal")
            {
               // fill out N/A for partial derivative
               for (int i = 0; i < stateMap->size(); ++i)
                  sLine << "                      N/A";

               if ((currentObs->typeName == "DSNTwoWayRange")||(currentObs->typeName == "DSNRange"))
                  sprintf(&s[0],"            %d   %.15le   %.15le                     N/A", currentObs->uplinkBand, currentObs->uplinkFreq, currentObs->rangeModulo);
               else if ((currentObs->typeName == "DSNTwoWayDoppler") || (currentObs->typeName == "Doppler") || (currentObs->typeName == "Doppler_RangeRate"))
                  sprintf(&s[0], "            %d                      N/A                      N/A                 %.4lf", currentObs->uplinkBand, currentObs->dopplerCountInterval);
               else
                  sprintf(&s[0], "          N/A                      N/A                      N/A                     N/A");
               sLine << s;
            }
            sLine << "\n";
#endif


            // Reset value for removed reason for all reuseable data records
            if (isReUsed)
            {
               measManager.GetObsDataObject()->inUsed = true;
               measManager.GetObsDataObject()->removedReason = "N";
            }
         }
         else
         {
            RealArray hTrow;
            hTrow.assign(stateSize, 0.0);
            UnsignedInt rowCount = calculatedMeas->value.size();

            for (UnsignedInt i = 0; i < rowCount; ++i)
               hTilde.push_back(hTrow);
            
            // Now walk the state vector and get elements of H-tilde for each piece
            for (UnsignedInt i = 0; i < stateMap->size(); ++i)
            {
               if ((*stateMap)[i]->subelement == 1)
               {
                  #ifdef DEBUG_ACCUMULATION
                     MessageInterface::ShowMessage(
                        "   Calculating ddstate(%d) for %s, subelement %d of %d, "
                        "id = %d\n",
                        i, (*stateMap)[i]->elementName.c_str(),
                        (*stateMap)[i]->subelement, (*stateMap)[i]->length,
                        (*stateMap)[i]->elementID);
                     MessageInterface::ShowMessage("object = <%p '%s'>\n", (*stateMap)[i]->object, (*stateMap)[i]->object->GetName().c_str());
                  #endif
                  // Partial derivatives at measurement time tm
                  stateDeriv = measManager.CalculateDerivatives(
                     (*stateMap)[i]->object, (*stateMap)[i]->elementID,
                     modelsToAccess[0]);
                  
                  // Fill in the corresponding elements of hTilde
                  for (UnsignedInt j = 0; j < rowCount; ++j)
                     for (UnsignedInt k = 0; k < (*stateMap)[i]->length; ++k)
                        hTilde[j][i+k] = stateDeriv[j][k];                                          // hTilde is partial derivates at measurement time tm (not at aprioi time t0)

                  #ifdef DEBUG_ACCUMULATION
                     MessageInterface::ShowMessage("      Result:\n         ");
                     for (UnsignedInt l = 0; l < stateDeriv.size(); ++l)
                     {
                        for (UnsignedInt m = 0; m < (*stateMap)[i]->length; ++m)
                           MessageInterface::ShowMessage("%.12lf   ",
                              stateDeriv[l][m]);
                        MessageInterface::ShowMessage("\n         ");
                     }
                     MessageInterface::ShowMessage("\n");
                  #endif
               }
            }
         
     
            // Apply the STM
            #ifdef DEBUG_ACCUMULATION
               MessageInterface::ShowMessage("Applying the STM\n");
            #endif
            RealArray hRow;

            // Temporary buffer for non-scalar measurements; hMeas is used to build
            // the information matrix below.
            std::vector<RealArray> hMeas;

            #ifdef DEBUG_STM
               MessageInterface::ShowMessage("STM:\n");
               for (UnsignedInt j = 0; j < stateMap->size(); ++j)
               {
                  MessageInterface::ShowMessage("      [");
                  for (UnsignedInt k = 0; k < stateMap->size(); ++k)
                  {
                     MessageInterface::ShowMessage("  %le  ", (*stm)(j,k));
                  }
                  MessageInterface::ShowMessage("]\n");
               }
            #endif

            Real entry;
            for (UnsignedInt i = 0; i < hTilde.size(); ++i)
            {
               hRow.assign(stateMap->size(), 0.0);

               // hRow is partial derivaties at apriori time t0
               for (UnsignedInt j = 0; j < stateMap->size(); ++j)
               {
                  entry = 0.0;
                  for (UnsignedInt k = 0; k < stateMap->size(); ++k)
                  {
                     entry += hTilde[i][k] * (*stm)(k, j);
                  }
                  hRow[j] = entry;
               }

               hAccum.push_back(hRow);                   // each element of hAccum is a vector partial derivative
               hMeas.push_back(hRow);

               #ifdef DEBUG_ACCUMULATION_RESULTS
               MessageInterface::ShowMessage("   Htilde  = [");
               for (UnsignedInt l = 0; l < stateMap->size(); ++l)
                  MessageInterface::ShowMessage(  " %.12lf ", hTilde[i][l]);
               MessageInterface::ShowMessage(  "]\n");

               MessageInterface::ShowMessage("   H accum = [");
               for (UnsignedInt l = 0; l < stateMap->size(); ++l)
                  MessageInterface::ShowMessage(  " %.12lf ", hRow[l]);
               MessageInterface::ShowMessage(  "]\n");

               MessageInterface::ShowMessage("   H has %d rows\n",
                  hAccum.size());
               #endif
            }

            Real ocDiff;
            Real weight;
            #ifdef DEBUG_ACCUMULATION
               MessageInterface::ShowMessage("Accumulating the O-C differences\n");
               MessageInterface::ShowMessage("   Obs size = %d, calc size = %d\n",
                  currentObs->value.size(), calculatedMeas->value.size());
            #endif
            for (UnsignedInt k = 0; k < currentObs->value.size(); ++k)
            {
               // Calculate residual O-C
               ocDiff = currentObs->value[k] - calculatedMeas->value[k];
               #ifdef DEBUG_O_MINUS_C
                  Real OD_Epoch = TimeConverterUtil::Convert(currentObs->epoch,
                                      TimeConverterUtil::A1MJD, TimeConverterUtil::TAIMJD,
                                      GmatTimeConstants::JD_JAN_5_1941);
                  Real C_Epoch = TimeConverterUtil::Convert(calculatedMeas->epoch,
                                      TimeConverterUtil::A1MJD, TimeConverterUtil::TAIMJD,
                                      GmatTimeConstants::JD_JAN_5_1941);
                  MessageInterface::ShowMessage("Observation data: %s  %s  %s  Epoch = %.12lf  O = %.12le;         Calculated measurement: %s  %s  Epoch = %.12lf  C = %.12le;       O-C = %.12le     frequency = %.12le\n",
                     currentObs->typeName.c_str(), currentObs->participantIDs[0].c_str(), currentObs->participantIDs[1].c_str(), OD_Epoch, currentObs->value[k], 
                     calculatedMeas->participantIDs[0].c_str(), calculatedMeas->participantIDs[1].c_str(), C_Epoch, calculatedMeas->value[k], 
                     ocDiff, calculatedMeas->uplinkFreq);
               #endif

               measurementEpochs.push_back(currentEpoch);
               OData.push_back(currentObs->value[k]);
               CData.push_back(calculatedMeas->value[k]);
               measurementResiduals.push_back(ocDiff);
               measurementResidualID.push_back(calculatedMeas->uniqueID);
            
               // Calculate weight
               weight = 1.0;
               if (currentObs->noiseCovariance == NULL)
               {
                  #ifdef DEBUG_WEIGHTS
                  MessageInterface::ShowMessage("Measurement covariance(%d %d) "
                     "is %le\n", k, k, (*(calculatedMeas->covariance))(k,k));
                  #endif

                  // todo: Figure out why the covariances can be 0.0!?!

                  if ((*(calculatedMeas->covariance))(k,k) != 0.0)
                  // Weight is diag(1 / sigma^2), per Montebruck & Gill, eq. 8.33
                  // Covariance diagonal is ~diag(sigma^2)
                  // If no off diagonal terms, inverse in 1/element along diagonal
                     weight = 1.0 / (*(calculatedMeas->covariance))(k,k);
                  else
                     weight = 1.0;
               }
               else
               {
                   weight = 1.0 / (*(currentObs->noiseCovariance))(k,k);
                   #ifdef DEBUG_WEIGHTS
                   MessageInterface::ShowMessage("Noise covariance(%d %d) "
                     "is %le\n", k, k, (*(currentObs->noiseCovariance))(k,k));
                   #endif
               }
               Weight.push_back(weight);
            
               for (UnsignedInt i = 0; i < stateSize; ++i)
               {
                  for (UnsignedInt j = 0; j < stateSize; ++j)
                     //information(i,j) += hRow[i] * weight * hRow[j];
                     information(i,j) += hMeas[k][i] * weight * hMeas[k][j];

                  //residuals[i] += hRow[i] * weight * ocDiff;
                  residuals[i] += hMeas[k][i] * weight * ocDiff;
               }

            
               // Write report for this observation data for case data record is removed
               std::string ss = currentObs->removedReason;

#ifdef NEW_REPORTFILE_FORMAT
               if (textFileMode == "Normal")
               {
                  // Write to report file edit status:
                  sLine << GmatStringUtil::GetAlignmentString(ss, 4, GmatStringUtil::LEFT) + " ";

                  // Write to report file O-value, C-value, O-C, 
                  sprintf(&s[0], "%22.6lf  %22.6lf   %18.6lf ", currentObs->value[k], calculatedMeas->value[k], ocDiff);
                  sLine << s;
                  // Write to report file elevation angle:
                  sprintf(&s[0], "%6.2lf", calculatedMeas->feasibilityValue);
                  sLine << s;
               }
               else
               {
                  // Write to report file edit status:
                  sLine << GmatStringUtil::GetAlignmentString(ss, 10, GmatStringUtil::LEFT);

                  sprintf(&s[0], "%22.6lf   %22.6lf   %22.6lf   %18.6lf    %.12le   %.12le   %.12le   %18.12lf", currentObs->value_orig[k], currentObs->value[k], calculatedMeas->value[k], ocDiff, weight, ocDiff*ocDiff*weight, sqrt(weight)*abs(ocDiff), calculatedMeas->feasibilityValue);
                  sLine << s;
               }
#else
               // Write to report file edit status:
               sLine << GmatStringUtil::GetAlignmentString(ss, 10, GmatStringUtil::LEFT);

               sprintf(&s[0], "%22.6lf   %22.6lf   %22.6lf   %18.6lf    %.12le   %.12le   %.12le   %18.12lf", currentObs->value_orig[k], currentObs->value[k], calculatedMeas->value[k], ocDiff, weight, ocDiff*ocDiff*weight, sqrt(weight)*abs(ocDiff), calculatedMeas->feasibilityValue);
               sLine << s;
#endif
            
               if (textFileMode != "Normal")
               {
                  // fill out N/A for partial derivative
                  for (UnsignedInt p = 0; p < hAccum[hAccum.size()-1].size(); ++p)
                  {
                     Real derivative = hAccum[hAccum.size()-1][p];
                     if ((*stateMap)[p]->elementName == "Cr_Epsilon")
                     {
                        Real Cr = (*stateMap)[p]->object->GetRealParameter("Cr") / (1 + (*stateMap)[p]->object->GetRealParameter("Cr_Epsilon"));
                        derivative = derivative/ Cr;
                     }
                     else if ((*stateMap)[p]->elementName == "Cd_Epsilon")
                     {
                        Real Cd = (*stateMap)[p]->object->GetRealParameter("Cd") / (1 + (*stateMap)[p]->object->GetRealParameter("Cd_Epsilon"));
                        derivative = derivative/Cd;
                     }

                     // sprintf(&s[0],"   %18.12le", hAccum[hAccum.size()-1][p]);
                     sprintf(&s[0],"   %18.12le", derivative);
                     ss.assign(s); 
                     ss = ss.substr(ss.size()-20,20); 
                     sLine << "     " << ss;
                  }

                  if ((currentObs->typeName == "DSNTwoWayRange")||(currentObs->typeName == "DSNRange"))
                     sprintf(&s[0],"            %d   %.15le   %.15le                     N/A", currentObs->uplinkBand, currentObs->uplinkFreq, currentObs->rangeModulo);
                  else if ((currentObs->typeName == "DSNTwoWayDoppler")||(currentObs->typeName == "Doppler")||(currentObs->typeName == "Doppler_RangeRate"))
                     sprintf(&s[0],"            %d                      N/A                      N/A                 %.4lf", currentObs->uplinkBand, currentObs->dopplerCountInterval);
                  else
                     sprintf(&s[0],"          N/A                      N/A                      N/A                     N/A");
                  sLine << s;
               }
               sLine << "\n";

            }

            // Count number of observation data accepted for estimation calculation
            statisticsTable["ACCEPTED RECORDS"][keyword] += 1;
            statisticsTable["WEIGHTED RMS"][keyword] += weight*ocDiff*ocDiff;                    // sum of weighted residual square
            statisticsTable["MEAN RESIDUAL"][keyword] += ocDiff;                                 // sum of residual
            statisticsTable["STANDARD DEVIATION"][keyword] += ocDiff*ocDiff;                     // sum of residual square
            
            statisticsTable1["ACCEPTED RECORDS"][typeName] += 1;
            statisticsTable1["WEIGHTED RMS"][typeName] += weight*ocDiff*ocDiff;                    // sum of weighted residual square
            statisticsTable1["MEAN RESIDUAL"][typeName] += ocDiff;                                 // sum of residual
            statisticsTable1["STANDARD DEVIATION"][typeName] += ocDiff*ocDiff;                     // sum of residual square

            
            sumAcceptRecords[indexKey] += 1;                             // sum of all accepted records
            sumResidual[indexKey] += ocDiff;                             // sum of residual
            Real ocDiff2 = ocDiff*ocDiff;
            sumResidualSquare[indexKey] += ocDiff2;                      // sum of residual square
            sumWeightResidualSquare[indexKey] += weight*ocDiff2;         // sum of weigth residual square


            #ifdef DEBUG_ACCUMULATION_RESULTS
               MessageInterface::ShowMessage("Observed measurement value:\n");
               for (UnsignedInt k = 0; k < currentObs->value.size(); ++k)
                  MessageInterface::ShowMessage("   %.12lf", currentObs->value[k]);
               MessageInterface::ShowMessage("\n");

               MessageInterface::ShowMessage("Calculated measurement value:\n");
               for (UnsignedInt k = 0; k < calculatedMeas->value.size(); ++k)
                  MessageInterface::ShowMessage("   %.12lf",calculatedMeas->value[k]);
               MessageInterface::ShowMessage("\n");

               MessageInterface::ShowMessage("   O - C = ");
               for (UnsignedInt k = 0; k < calculatedMeas->value.size(); ++k)
                  MessageInterface::ShowMessage("   %.12lf",
                     currentObs->value[k] - calculatedMeas->value[k]);
               MessageInterface::ShowMessage("\n");

               MessageInterface::ShowMessage("Derivatives w.r.t. state (H-tilde):\n");
               for (UnsignedInt k = 0; k < hTilde.size(); ++k)
               {
                  for (UnsignedInt l = 0; l < hTilde[k].size(); ++l)
                     MessageInterface::ShowMessage("   %.12lf",hTilde[k][l]);
                  MessageInterface::ShowMessage("\n");
               }
               MessageInterface::ShowMessage("   Information Matrix = \n");
               for (Integer i = 0; i < estimationState->GetSize(); ++i)
               {
                  MessageInterface::ShowMessage("      [");
                  for (Integer j = 0; j < estimationState->GetSize(); ++j)
                  {
                     MessageInterface::ShowMessage(" %.12lf ", information(i, j));
                  }
                  MessageInterface::ShowMessage("]\n");
               }
               MessageInterface::ShowMessage("   Residuals = [");
               for (Integer i = 0; i < residuals.GetSize(); ++i)
                  MessageInterface::ShowMessage(" %.12lf ", residuals[i]);
               MessageInterface::ShowMessage("]\n");
            #endif

         } // end of if (measManager.GetObsDataObject()->inUsed)

      } // end of if (count >= 1)

   }  // end of if (modelsToAccess.size() == 0)

   
   linesBuff = sLine.str();
   WriteToTextFile(currentState);

   // Accumulate the processed data
   #ifdef RUN_SINGLE_PASS
      #ifdef SHOW_STATE_TRANSITIONS
         MessageInterface::ShowMessage("BatchEstimator state is ESTIMATING at "
               "epoch %.12lf\n", currentEpoch);
      #endif
   #endif

   #ifdef DEBUG_ACCUMULATION
      MessageInterface::ShowMessage("Advancing to the next measurement\n");
   #endif

   // Advance to the next measurement and get its epoch
   bool isEndOfTable = measManager.AdvanceObservation();
   if (isEndOfTable)
      currentState = ESTIMATING;
   else
   {
      nextMeasurementEpoch = measManager.GetEpoch();
      FindTimeStep();

      if (currentEpoch <= (nextMeasurementEpoch + 5.0e-12))       // It needs to add 5.0e-12 in order to avoid accuracy limit of double
         currentState = PROPAGATING;
      else
         currentState = ESTIMATING;
   }

   #ifdef DEBUG_ACCUMULATION
      MessageInterface::ShowMessage("Exit BatchEstimatorInv::Accumulate()\n");
   #endif

}




//------------------------------------------------------------------------------
// void Estimate()
//------------------------------------------------------------------------------
/**
 * This method solves the normal equations using direct inversion
 */
//------------------------------------------------------------------------------
void BatchEstimatorInv::Estimate()
{
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("BatchEstimator state is ESTIMATING\n");
   #endif

   // Plot all residuals
   if (showAllResiduals)
      PlotResiduals();

   // Display number of removed records for each type of filters
   if (!numRemovedRecords.empty())
   {
      MessageInterface::ShowMessage("Number of Records Removed Due To:\n");
      MessageInterface::ShowMessage("   . No Computed Value Configuration Available : %d\n", numRemovedRecords["U"]);
      MessageInterface::ShowMessage("   . Out of Ramped Table Range : %d\n", numRemovedRecords["R"]);
      MessageInterface::ShowMessage("   . Signal Blocked : %d\n", numRemovedRecords["B"]);
      MessageInterface::ShowMessage("   . Initial RMS Sigma Filter  : %d\n", numRemovedRecords["IRMS"]);
      MessageInterface::ShowMessage("   . Outer-Loop Sigma Editor : %d\n", numRemovedRecords["OLSE"]);
   }
   MessageInterface::ShowMessage("Number of records used for estimation: %d\n", measurementResiduals.size());
   
   if (measurementResiduals.size() < esm.GetStateMap()->size())
   {
      std::stringstream ss;
      ss << "Error: For Batch estimator " << GetName() 
         << ", there are " << esm.GetStateMap()->size() 
         << " solver-for parameters, and only " << measurementResiduals.size() 
         << " valid observable records remaining after editing. Please modify data editing criteria or provide a better a-priori estimate.\n";
      throw EstimatorException(ss.str());
   }

   // Apriori state (initial state for 0th iteration) and initial state for current iteration: 
   if (iterationsTaken == 0)
      initialEstimationState = (*estimationState);
   oldEstimationState = (*estimationState);
   // Convert previous state from GMAT internal coordinate system to participants' coordinate system
   GetEstimationStateForReport(previousSolveForState);


   // Specify previous, current, and the best weighted RMS:
   // Calculate RMSOLD:
   if (iterationsTaken > 0)
      oldResidualRMS = newResidualRMS;                       // old value is only valid from 1st iteration
   // Calculate RMS:
   newResidualRMS = 0.0;
   for (int i = 0; i < measurementResiduals.size(); ++i)
      newResidualRMS += measurementResiduals[i] * measurementResiduals[i]*Weight[i];
   newResidualRMS = GmatMathUtil::Sqrt(newResidualRMS / measurementResiduals.size());
   // Calculate RMSB:
   if (iterationsTaken == 0)
      bestResidualRMS = newResidualRMS;
   else
   {
      // Reset best RMS as needed                                            // made changes by TUAN NGUYEN
      if (resetBestRMSFlag)                                                  // made changes by TUAN NGUYEN
      {                                                                      // made changes by TUAN NGUYEN
         if (estimationStatus == DIVERGING)                                  // made changes by TUAN NGUYEN
            bestResidualRMS = oldResidualRMS;                                // made changes by TUAN NGUYEN
      }                                                                      // made changes by TUAN NGUYEN

      bestResidualRMS = GmatMathUtil::Min(bestResidualRMS, newResidualRMS);
   }

   // Solve normal equation
   #ifdef DEBUG_VERBOSE
      MessageInterface::ShowMessage("Accumulation complete; now solving the "
            "normal equations!\n");

      MessageInterface::ShowMessage("\nEstimating changes for iteration %d\n\n",
            iterationsTaken+1);

      MessageInterface::ShowMessage("   Presolution estimation state:\n      "
            "epoch = %.12lf\n      [", estimationState->GetEpoch());
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage("  %.12lf  ", (*estimationState)[i]);
      MessageInterface::ShowMessage("]\n");

      MessageInterface::ShowMessage("   Information matrix:\n");
      for (UnsignedInt i = 0; i < information.GetNumRows(); ++i)
      {
         MessageInterface::ShowMessage("      [");
         for (UnsignedInt j = 0; j < information.GetNumColumns(); ++j)
         {
            MessageInterface::ShowMessage(" %.12lf ", information(i,j));
         }
         MessageInterface::ShowMessage("]\n");
      }
   #endif

   Rmatrix cov(information.GetNumRows(), information.GetNumColumns());
   if (inversionType == "Schur")
   {
      Real *sum1;
      Integer arraysize;

      Integer iSize = information.GetNumColumns();
      if (iSize != information.GetNumRows())
         throw SolverException("Schur inversion requires a square information "
                        "matrix");

      arraysize = iSize * (iSize + 1) / 2;
      sum1 = new Real[arraysize];

      // Fill sum1 with the upper triangle
      Integer index = 0;
      for (Integer i = 0; i < information.GetNumRows(); ++i)
         for (Integer j = i; j < information.GetNumColumns(); ++j)
         {
            sum1[index] = information(i,j);
            ++index;
         }

      #ifdef DEBUG_SCHUR
         MessageInterface::ShowMessage("Calling Schur with array size = %d\n"
               "   Input vector:  [", arraysize);
         for (Integer i = 0; i < arraysize; ++i)
         {
            if (i > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", sum1[i]);
         }
         MessageInterface::ShowMessage("]\n");
      #endif
      
      Integer schurRet = SchurInvert(sum1, arraysize);

      #ifdef DEBUG_SCHUR
         MessageInterface::ShowMessage("   Output vector: [", arraysize);
         for (Integer i = 0; i < arraysize; ++i)
         {
            if (i > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", sum1[i]);
         }
         MessageInterface::ShowMessage("]\n");
      #endif

      if (schurRet != 0)
         throw SolverException("Schur inversion failed");

      // Now fill in cov
      // Fill sum1 with the upper triangle
      index = 0;
      for (Integer i = 0; i < information.GetNumRows(); ++i)
         for (Integer j = i; j < information.GetNumColumns(); ++j)
         {
            cov(i,j) = sum1[index];
            ++index;
            if (i != j)
               cov(j,i) = cov(i,j);
         }
      delete [] sum1;
   }
   else if (inversionType == "Cholesky")
   {
      Real *sum1;
      Integer arraysize;

      Integer iSize = information.GetNumColumns();
      if (iSize != information.GetNumRows())
         throw SolverException("Cholesky inversion requires a symmetric positive definite "
                  "information matrix");

      arraysize = iSize * (iSize + 1) / 2;
      sum1 = new Real[arraysize];
      // Fill sum1 with the upper triangle
      Integer index = 0;
      for (Integer i = 0; i < information.GetNumRows(); ++i)
         for (Integer j = i; j < information.GetNumColumns(); ++j)
         {
            sum1[index] = information(i,j);
            ++index;
         }

      if (CholeskyInvert(sum1, arraysize) != 0)
         throw SolverException("Cholesky inversion failed");

      // Now fill in cov
      // Fill sum1 with the upper triangle
      index = 0;
      for (Integer i = 0; i < information.GetNumRows(); ++i)
         for (Integer j = i; j < information.GetNumColumns(); ++j)
         {
            cov(i,j) = sum1[index];
            if (i != j)
               cov(j,i) = cov(i,j);
            ++index;
         }
      delete [] sum1;
   }
   else
   {
      try
      {
         cov = information.Inverse();
      } 
      catch (...)
      {
         #ifdef DEBUG_INVERSION
            MessageInterface::ShowMessage("Information matrix:\n");
            for (UnsignedInt i = 0; i < cov.GetNumRows(); ++i)
            {
               MessageInterface::ShowMessage("      [");
               for (UnsignedInt j = 0; j < information.GetNumColumns(); ++j)
               {
                  MessageInterface::ShowMessage(" %.12lf ", information(i,j));
               }
               MessageInterface::ShowMessage("]\n");
            }
         #endif
         throw EstimatorException("Error: Normal matrix is singular\n");
      }
   }

   #ifdef DEBUG_VERBOSE
      MessageInterface::ShowMessage(" residuals: [");
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage("  %.12lf  ", residuals(i));
      MessageInterface::ShowMessage("]\n");

      MessageInterface::ShowMessage("   covariance matrix:\n");
      for (UnsignedInt i = 0; i < cov.GetNumRows(); ++i)
      {
         MessageInterface::ShowMessage("      [");
         for (UnsignedInt j = 0; j < cov.GetNumColumns(); ++j)
         {
            MessageInterface::ShowMessage(" %.12lf ", cov(i,j));
         }
         MessageInterface::ShowMessage("]\n");
      }
   #endif

   // Calculate state change dx
   dx.clear();
   Real delta;
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      delta = 0.0;
      for (UnsignedInt j = 0; j < stateSize; ++j)
         delta += cov(i,j) * residuals(j);
      dx.push_back(delta);
      (*estimationState)[i] += delta;
   }
   esm.RestoreObjects(&outerLoopBuffer);                           // Restore solver-object initial state           // made changes by TUAN NGUYEN
   esm.MapVectorToObjects();                                       // update objects state to current state         // made changes by TUAN NGUYEN

   // Convert current estimation state from GMAT internal coordinate system to participants' coordinate system
   GetEstimationStateForReport(currentSolveForState);

   #ifdef DEBUG_VERBOSE
      MessageInterface::ShowMessage("   State vector change (dx):\n      [");
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage("  %.12lf  ", dx[i]);
      MessageInterface::ShowMessage("]\n");

      MessageInterface::ShowMessage("   New estimation state:\n      "
            "epoch = %.12lf\n      [", estimationState->GetEpoch());
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage("  %.12lf  ", (*estimationState)[i]);
      MessageInterface::ShowMessage("]\n");
   #endif
   
   // Specify RMSP:
   predictedRMS = 0;
   if (useApriori)
   {
      GmatState currentEstimationState = (*estimationState);
      Rmatrix Pdx0_inv = stateCovariance->GetCovariance()->Inverse();
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            predictedRMS += (currentEstimationState[i] - initialEstimationState[i])*Pdx0_inv(i,j)*(currentEstimationState[j] - initialEstimationState[j]);
      }
   }
   
   for (UnsignedInt j = 0; j < hAccum.size(); ++j)      // j presents for the index of the measurement jth
   {
      Real temp = 0;
      for(UnsignedInt i = 0; i < hAccum[j].size(); ++i)
      {
         temp += hAccum[j][i]*dx[i];
      }
      predictedRMS += (measurementResiduals[j] - temp)*(measurementResiduals[j] - temp)*Weight[j];
   }
   predictedRMS = sqrt(predictedRMS/measurementResiduals.size());


   // Write to report initial state for current iteration
   WriteToTextFile(currentState);


   // Clear O, C, and W lists
   Weight.clear();
   OData.clear();
   CData.clear();

   currentState = CHECKINGRUN;
}


Real BatchEstimatorInv::ObservationDataCorrection(Real cValue, Real oValue, Real moduloConstant)
{
   Real delta = cValue - oValue;
   int N = (int)(delta/moduloConstant + 0.5);

   return (oValue + N*moduloConstant);
}
