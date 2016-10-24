//$Id: MeasurementManager.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         MeasurementManager
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
// Created: 2009/06/24
//
/**
 * MeasurementManager implementation used in GMAT's estimators and simulator
 */
//------------------------------------------------------------------------------

#include "MeasurementManager.hpp"
#include "MeasurementException.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include <sstream>            // To build DataStream for a TrackingFileSet

#include "DataFileAdapter.hpp"
#include "PropSetup.hpp"
#include "StatisticAcceptFilter.hpp"
#include "StatisticRejectFilter.hpp"

// Temporary to get Adapters hooked up
#include "GmatObType.hpp"
#include "RampTableType.hpp"

//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION
//#define DEBUG_FILE_WRITE
//#define DEBUG_FLOW
//#define DEBUG_CALCULATE_MEASUREMENTS
//#define DEBUG_GET_ACTIVE_EVENTS
//#define DEBUG_LOAD_OBSERVATIONS
//#define DEBUG_RAMP_TABLE
//#define DEBUG_LOAD_FREQUENCY_RAMP_TABLE
//#define DEBUG_INITIALIZE
//#define DEBUG_MODEL_MAPPING
//#define DEBUG_CALCULATE
//#define DEBUG_ADVANCE_OBSERVATION
//#define DEBUG_EXECUTION
//#define DEBUG_ADAPTERS
//#define DEBUG_AUTO_GENERATE_TRACKINGDATAADAPTERS

// Selects between old datafile classes and the classes in the DataFile plugin
//#define USE_DATAFILE_PLUGINS


//------------------------------------------------------------------------------
// MeasurementManager()
//------------------------------------------------------------------------------
/**
 * Constructor for the measurement manager
 */
//------------------------------------------------------------------------------
MeasurementManager::MeasurementManager() :
   thePropagator     (NULL),
   anchorEpoch       (GmatTimeConstants::MJD_OF_J2000),
   currentEpoch      (GmatTimeConstants::MJD_OF_J2000),
   idBase            (10000),
   largestId         (10000),
   eventCount        (0),
   inSimulationMode  (false)
{
}

//------------------------------------------------------------------------------
// ~MeasurementManager()
//------------------------------------------------------------------------------
/**
 * Destructor for the measurement manager
 */
//------------------------------------------------------------------------------
MeasurementManager::~MeasurementManager()
{
   //MessageInterface::ShowMessage("MeasurementMaganger destruction!!!!!!!!!!!!\n");
}

//------------------------------------------------------------------------------
// MeasurementManager(const MeasurementManager &mm)
//------------------------------------------------------------------------------
/**
 * Copy constructor for the measurement manager
 *
 * @param mm The manager that gets copied
 */
//------------------------------------------------------------------------------
MeasurementManager::MeasurementManager(const MeasurementManager &mm) :
   thePropagator     (mm.thePropagator),
   anchorEpoch       (mm.anchorEpoch),
   currentEpoch      (mm.currentEpoch),
   idBase            (mm.idBase),
   largestId         (mm.largestId),
   eventCount        (mm.eventCount),
   inSimulationMode  (mm.inSimulationMode)
{
   modelNames = mm.modelNames;

   // process MeasurementModel objects
   for (std::vector<MeasurementModel*>::const_iterator i = mm.models.begin();
         i != mm.models.end(); ++i)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Cloning %s MeasurementModel\n",
               (*i)->GetStringParameter("Type").c_str());
      #endif
      models.push_back((MeasurementModel*)((*i)->Clone()));
   }

   for (std::vector<TrackingSystem*>::const_iterator i = mm.systems.begin();
         i != mm.systems.end(); ++i)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Cloning %s TrackingSystem\n",
               (*i)->GetStringParameter("Type").c_str());
      #endif
      systems.push_back((TrackingSystem*)((*i)->Clone()));
   }

   // made changes @ 9/16/2014
   // process TrackingDataAdapter objects
   for (std::vector<TrackingDataAdapter*>::const_iterator i = mm.adapters.begin();
         i != mm.adapters.end(); ++i)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Cloning %s TrackingDataAdapter\n",
               (*i)->GetStringParameter("Type").c_str());
      #endif
      adapters.push_back((TrackingDataAdapter*)((*i)->Clone()));
   }

   for (std::vector<TrackingFileSet*>::const_iterator i = mm.trackingSets.begin();
         i != mm.trackingSets.end(); ++i)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Cloning %s TrackingDataSet\n",
               (*i)->GetName().c_str());
      #endif
      trackingSets.push_back((TrackingFileSet*)((*i)->Clone()));

      // Note: No measurement data associated with TrackingFileSet object, but TrackingDataAdapter has its owe measurement data
      // MeasurementData md;
      // measurements.push_back(md);
   }
}

//------------------------------------------------------------------------------
// MeasurementManager& operator=(const MeasurementManager &mm)
//------------------------------------------------------------------------------
/**
 * Measurement manager assignment operator
 *
 * @param mm The manager that gets copied
 *
 * @return This measurement manager
 */
//------------------------------------------------------------------------------
MeasurementManager& MeasurementManager::operator=(const MeasurementManager &mm)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("MeasurementManager::operator= enter: <%p> assign from <%p>\n", this, &mm);  
#endif
   if (&mm != this)
   {
      thePropagator    = mm.thePropagator;
      anchorEpoch      = mm.anchorEpoch;
      currentEpoch     = mm.currentEpoch;
      modelNames       = mm.modelNames;
      eventCount       = mm.eventCount;
      inSimulationMode = mm.inSimulationMode;

      // Clone the measurements, tracking systems, adapters, and tracking file sets
      // Clean up first
      for (std::vector<MeasurementModel*>::iterator i = models.begin();
            i != models.end(); ++i)
         delete (*i);
      models.clear();

      for (std::vector<TrackingSystem*>::iterator i = systems.begin();
            i != systems.end(); ++i)
         delete (*i);
      systems.clear();

      for (std::vector<TrackingDataAdapter*>::iterator i = adapters.begin();
            i != adapters.end(); ++i)
         delete (*i);
      adapters.clear();

      for (std::vector<TrackingFileSet*>::iterator i = trackingSets.begin();
            i != trackingSets.end(); ++i)
         delete (*i);
      trackingSets.clear();

      measurements.clear();

      // Then clone them
      for (std::vector<MeasurementModel*>::const_iterator i = mm.models.begin();
            i != mm.models.end(); ++i)
         models.push_back((MeasurementModel*)(*i)->Clone());

      for (std::vector<TrackingSystem*>::const_iterator i = mm.systems.begin();
            i != mm.systems.end(); ++i)
         systems.push_back((TrackingSystem*)(*i)->Clone());

      for (std::vector<TrackingDataAdapter*>::const_iterator i = mm.adapters.begin();
            i != mm.adapters.end(); ++i)
         adapters.push_back((TrackingDataAdapter*)(*i)->Clone());

      for (std::vector<TrackingFileSet*>::const_iterator i = mm.trackingSets.begin();
            i != mm.trackingSets.end(); ++i)
         trackingSets.push_back((TrackingFileSet*)((*i)->Clone()));

      // Should measurements be rebuilt here?
   }

#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("MeasurementManager::operator= exit: <%p> assign from <%p>\n", this, &mm);  
#endif
   return *this;
}


//------------------------------------------------------------------------------
// bool SetPropagator(PropSetup* ps)
//------------------------------------------------------------------------------
/**
 * Sets the propagator needed by the tracking data adapters
 *
 * @param ps The propagator
 *
 * @todo The current call supports a single propagator.  Once the estimation
 *       system supports multiple propagators, this should be changed to a
 *       vector of PropSetup objects.
 *
 * @return true if the pointer is set; false if it is NULL
 */
//------------------------------------------------------------------------------
bool MeasurementManager::SetPropagator(PropSetup* ps)
{
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Setting the propagator in the measurement "
            "manager to %p\n", ps);
   #endif
   thePropagator = ps;
   return (thePropagator != NULL);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Verifies that the measurement models are ready to calculate measuremetns,
 * and builds internal data structures needed to manage these calculations.
 *
 * @return true is ready to go, false if not
 */
//------------------------------------------------------------------------------
bool MeasurementManager::Initialize()
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Entered MeasurementManager::Initialize() method\n");
   #endif

   #ifdef DEBUG_INITIALIZE
      MessageInterface::ShowMessage(
            "Entered MeasurementManager::Initialize()\n");
   #endif

   bool retval = true;

   measurements.clear();
   for (UnsignedInt i = 0; i < models.size(); ++i)
   {
      if (models[i]->Initialize() == false)
         return false;
      MeasurementData md;
      measurements.push_back(md);
   }
   
   for (UnsignedInt i = 0; i < trackingSets.size(); ++i)
   {
      // For each tracking set, It needs to do the following steps to process

      // Step 1: Initialize trackingSets[i]
      if (trackingSets[i]->Initialize() == false)
         return false;

      // Step 2: Load data adapters from tracking file sets to measurement manager
      std::vector<TrackingDataAdapter*> *setAdapters =
            trackingSets[i]->GetAdapters();
      StringArray names;
      for (UnsignedInt j = 0; j < setAdapters->size(); ++j)
      {
         AddMeasurement((*setAdapters)[j]);
         MeasurementData md;
         measurements.push_back(md);
         names.push_back((*setAdapters)[j]->GetName());

         // Set retval?
      }
      adapterFromTFSMap[trackingSets[i]] = names;

      // Step 3: Set stream objects and data filters for all observation data files in trackingSet[i]
      // 3.1. Create DataFile object for each file name
      StringArray filenames = trackingSets[i]->GetStringArrayParameter("FileName");
      for (UnsignedInt k = 0; k < filenames.size(); ++k)
      {
         // 3.1.1 Create DataFile object
         DataFile *newStream = new DataFile(filenames[k]);
         newStream->SetStringParameter("Filename", filenames[k]);

         // 3.1.2 Create and set a data stream associated with the DataFile object
         GmatObType *got = new GmatObType();                      // ??? what happen for GMAT_OD and GMAT_ODDoppler???   // In new design, GMATInteral data file contains data records with different measurement type
         newStream->SetStream(got);
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("   Adding %s DataFile %s <%p>\n",
                  (newStream->IsInitialized() ? "initialized" :
                  "not initialized"), newStream->GetName().c_str(), newStream);
         #endif

         SetStreamObject(newStream);

         // Associate the adapters with the stream
         for (UnsignedInt j = 0; j < setAdapters->size(); ++j)
         {
            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("Associating %d with %s\n",
                     (*setAdapters)[j]->GetModelID(),
                     newStream->GetName().c_str());
            #endif
            idToStreamMap[(*setAdapters)[j]->GetModelID()] = newStream;
         }// for j loop
         
         if (inSimulationMode)
         {
            if (newStream->OpenStream(inSimulationMode) == false)
               throw MeasurementException("The stream " + filenames[k] + 
                     " failed to open in simulation mode");
         }
      }// for k loop

      // 3.2. Set data filters to data file (only set data filters in estimation mode)
      if (!inSimulationMode)
         SetStatisticsDataFiltersToDataFiles(i);

      // Step 4: Set stream objects for all ramp tables in trackingSet[i] 
      StringArray rampedTablenames = trackingSets[i]->GetStringArrayParameter("RampTable");
      for (UnsignedInt k1 = 0; k1 < rampedTablenames.size(); ++k1)
      {
//         std::stringstream fn;
//         fn << trackingSets[i]->GetName() << "RampTable" << k1;
//         DataFile *newStream = new DataFile(fn.str());
         DataFile *newStream = new DataFile(rampedTablenames[k1]);
         newStream->SetStringParameter("Filename", rampedTablenames[k1]);

#ifdef DEBUG_RAMP_TABLE
         MessageInterface::ShowMessage("MeasurementManager::Initialize():  Ramp table name = %s\n", rampedTablenames[k1].c_str());
#endif
         RampTableType *rpt = new RampTableType();
         newStream->SetStream(rpt);
         newStream->SetStringParameter("Format", "GMAT_RampTable");

         #ifdef DEBUG_RAMP_TABLE
            MessageInterface::ShowMessage("   Adding %s DataFile %s <%p>\n",
                  (newStream->IsInitialized() ? "initialized" :
                  "not initialized"), newStream->GetName().c_str(), newStream);
         #endif

         SetRampTableDataStreamObject(newStream);

         // Associate the adapters with the stream
         for (UnsignedInt j = 0; j < setAdapters->size(); ++j)
         {
            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("Associating %d with %s\n",
                     (*setAdapters)[j]->GetModelID(),
                     newStream->GetName().c_str());
            #endif
            idToRampTableStreamMap[(*setAdapters)[j]->GetModelID()] = newStream;
         }// for j loop

         if (inSimulationMode)
         {
            if (newStream->OpenStream(inSimulationMode) == false)
               throw MeasurementException("The stream " + rampedTablenames[k1] + 
                     " failed to open in simulation mode");
         }
      }// for k1 loop

   }// for i loop

   // 5. Initialize all data file objects 
   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      if (streamList[i]->IsInitialized() == false)
         streamList[i]->Initialize();
   }

///// TBD: Do we want something more generic here?
   // 6. Initialize all ramp table data objects
   for (UnsignedInt i = 0; i < rampTableDataStreamList.size(); ++i)
   {
      if (rampTableDataStreamList[i]->IsInitialized() == false)
         rampTableDataStreamList[i]->Initialize();
   }


   #ifdef DEBUG_INITIALIZE
      MessageInterface::ShowMessage(
            "Exit MeasurementManager::Initialize()\n");
   #endif
#ifdef DEBUG_FLOW
   MessageInterface::ShowMessage(
         "Exit MeasurementManager::Initialize() method\n");
#endif

   retval = true;
   return retval;
}


//-----------------------------------------------------------------------------------
// bool SetStatisticsDataFiltersToDataFiles(Integer option)
//-----------------------------------------------------------------------------------
/** This function is used to set statistics data filters to approriated data file
* as specified in tracking file set objects.
*
* @param i      TrackingFileSet index
*
*/
//-----------------------------------------------------------------------------------
bool MeasurementManager::SetStatisticsDataFiltersToDataFiles(UnsignedInt i)
{
      // 1. Get tracking configs in ID from TrackingFileSet trackingSet[i]
      StringArray tkconfigs;
      std::stringstream ss;
      std::vector<TrackingDataAdapter*>* adapters = trackingSets[i]->GetAdapters();
      for (UnsignedInt k = 0; k < adapters->size(); ++k)
      {
         MeasureModel* model = adapters->at(k)->GetMeasurementModel();
         std::vector<ObjectArray*> objArrList = model->GetParticipantObjectLists();
         for (UnsignedInt k1 = 0; k1 < objArrList.size(); ++k1)
         {
            ss.str("");
            ss << "{{";
            for (UnsignedInt k2 = 0; k2 < objArrList[k1]->size(); ++k2)
            {
               ss << objArrList[k1]->at(k2)->GetStringParameter("Id");
               if (k2 < objArrList[k1]->size()-1)
                  ss << ",";
            }
            ss << "}" << adapters->at(k)->GetStringParameter("MeasurementType") << "}";
            tkconfigs.push_back(ss.str());
            //MessageInterface::ShowMessage("tkconfig: <%s>\n", ss.str().c_str());
         }
      }

      // 2. Get all data filters defined in tracking file set object trackingSet[i]
      ObjectArray dataFilterObjects = trackingSets[i]->GetRefObjectArray(Gmat::DATA_FILTER);
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(" &&&&   There are %d data filter objects defined in %s.\n", dataFilterObjects.size(), trackingSets[i]->GetName().c_str());
         for(UnsignedInt j = 0; j < dataFilterObjects.size(); ++j)
            MessageInterface::ShowMessage("filter %d: <%s>\n", j, dataFilterObjects[j]->GetName().c_str());
      #endif

      // 3. Set tracking configs to statistics data filters in order to filter data based on them
      for (UnsignedInt k = 0; k < dataFilterObjects.size(); ++k)
      {
         // set tracking configs to statistics accept filter
         if (dataFilterObjects[k]->IsOfType("StatisticsAcceptFilter"))
            ((StatisticAcceptFilter*)dataFilterObjects[k])->SetTrackingConfigs(tkconfigs);

         // set tracking configs to statistics reject filter
         if (dataFilterObjects[k]->IsOfType("StatisticsRejectFilter"))
            ((StatisticRejectFilter*)dataFilterObjects[k])->SetTrackingConfigs(tkconfigs);
      }

      // 4. Get list of all file names defined in trackingSet[i]
      StringArray filenames = trackingSets[i]->GetStringArrayParameter("FileName");
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(" &&&&   There are %d filenames defined in %s.\n", filenames.size(), trackingSets[i]->GetName().c_str());
         for(UnsignedInt j = 0; j < filenames.size(); ++j)
            MessageInterface::ShowMessage("file %d: <%s>\n", j, filenames[j].c_str());
      #endif
      
      // 5. Set data filters to the DataFile objects
      for (UnsignedInt j = 0; j < filenames.size(); ++j)
      {
         std::string fileName = filenames[j];
         // 5.1. Specify DataFile object having filename[j]
         DataFile* fileObj = NULL;
         for (UnsignedInt k = 0; k < streamList.size(); ++k)
         {
            if (streamList[k]->GetName() == fileName)
            {
               fileObj = streamList[k];
               break;
            }
         }
         if (fileObj == NULL)
            throw MeasurementException("Error: DataFile object with name '" + filenames[j] + "' was not set in MeasurementManager.\n");
         
         // 5.2. Set data filters of type "Statistics" filter to DataFile object
         for (UnsignedInt k = 0; k < dataFilterObjects.size(); ++k)
         {
            GmatBase*  datafilter = dataFilterObjects[k];

            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage(" &&&&   Set data filter <%s,%p> to data file <%s,%p>.\n", datafilter->GetName().c_str(), datafilter, fileObj->GetName().c_str(), fileObj);
            #endif
            // Set statistic filter to DataFile object. 
            // Filter will handle the selection of data record based on file names; therefore, the follwoing code is not needed.
            fileObj->SetDataFilter((DataFilter*)datafilter);
         }// for k loop
      }// for j loop

   return true;
}



//------------------------------------------------------------------------------
// bool PrepareForProcessing(bool simulating, PropSetup *propagator)
//------------------------------------------------------------------------------
/**
 * This method sets up measurement models for use in simulation or estimation
 *
 * @param simulating Flag to tell the MM that the system is only simulating, so
 *                   derivative calculations are not necessary.
 *
 * @return true unless the stream could not be opened to receive simulated data;
 *         in that case, returns false.
 */
//------------------------------------------------------------------------------
bool MeasurementManager::PrepareForProcessing(bool simulating)
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Entered MeasurementManager::PrepareForProcessing() method\n");
      MessageInterface::ShowMessage(
            "Working with %d streams\n", streamList.size());
   #endif

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
         "Entered MeasurementManager::PrepareForProcessing(%s)\n",
         (simulating ? "true" : "false"));
   #endif
   
   // Verify no pair of streams having the same file name but different data format:
   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      std::string fname      = streamList[i]->GetStringParameter("Filename");
      std::string dataformat = streamList[i]->GetStringParameter("Format");
      for(UnsignedInt j = 0; j < streamList.size(); ++j)
      {
         if (i == j)
            continue;
         if ((fname == streamList[j]->GetStringParameter("Filename"))&&(dataformat != streamList[j]->GetStringParameter("Format")))
            throw MeasurementException("Error: DataFile objects '"+ streamList[i]->GetName() +"' and '" + streamList[j]->GetName() + "' have the same file name but different data format\n");
      }

      for(UnsignedInt j = 0; j < rampTableDataStreamList.size(); ++j)
      {
         if ((fname == rampTableDataStreamList[j]->GetStringParameter("Filename"))&&(dataformat != rampTableDataStreamList[j]->GetStringParameter("Format")))
            throw MeasurementException("Error: DataFile objects '"+ streamList[i]->GetName() +"' and '" + rampTableDataStreamList[j]->GetName() + "' have the same file name but different data format\n");
      }
   }
   
   /// @todo: Set the propagators based on the spacecraft in each adapter.  This
   ///        piece is not yet part of the GMAT implementation, but when ready,
   ///        needs to be addressed here.
   // Pass the propagator to the tracking data adapters
   for (UnsignedInt i = 0; i < adapters.size(); ++i)
      adapters[i]->SetPropagator(thePropagator);
   
   // Open all streams in streamList
   bool retval = true;
   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Opening the data stream to file %s\n",
               streamList[i]->GetName().c_str());
      #endif

      #ifdef USE_DATAFILE_PLUGINS
         std::string writeMode = (simulating ? "w" : "r");
         streamList[i]->SetStringParameter(
               streamList[i]->GetParameterID("ReadWriteMode"), writeMode);

         if (streamList[i]->OpenFile() == false)
            retval = false;
      #else
         if (streamList[i]->OpenStream(simulating) == false)
         {
            MessageInterface::ShowMessage("Open failed\n");
            retval = false;
         }
      #endif
   }
   
///// TBD: Do we want something more generic here?
   for (UnsignedInt i = 0; i < rampTableDataStreamList.size(); ++i)
   {
      #ifdef USE_DATAFILE_PLUGINS
         std::string writeMode = (simulating ? "w" : "r");
         rampTableDataStreamList[i]->SetStringParameter(
               rampTableDataStreamList[i]->GetParameterID("ReadWriteMode"), writeMode);

         if (rampTableDataStreamList[i]->OpenFile() == false)
            retval = false;
      #else
         if (rampTableDataStreamList[i]->OpenStream(simulating) == false)
            retval = false;
      #endif
   }
   
   inSimulationMode = simulating;

   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Exit MeasurementManager::PrepareForProcessing() method: retval = %s\n", (retval?"true":"false"));
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool MeasurementManager::ProcessingComplete()
//------------------------------------------------------------------------------
/**
 * Closes the measurement streams when processing is finished
 *
 * @return true if the streams closed, false if any fail to close
 */
//------------------------------------------------------------------------------
bool MeasurementManager::ProcessingComplete()
{
   #ifdef DEBUG_FLOW
   MessageInterface::ShowMessage(
         "Entered MeasurementManager::ProcessingComplete() method\n");
   #endif

   bool retval = true;

   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      #ifdef USE_DATAFILE_PLUGINS
         if (streamList[i]->CloseFile() == false)
            retval = false;
      #else
         if (streamList[i]->CloseStream() == false)
            retval = false;
      #endif
   }

///// TBD: Do we want something more generic here?
   for (UnsignedInt i = 0; i < rampTableDataStreamList.size(); ++i)
   {
      #ifdef USE_DATAFILE_PLUGINS
         if (rampTableDataStreamList[i]->CloseFile() == false)
            retval = false;
      #else
         if (rampTableDataStreamList[i]->CloseStream() == false)
            retval = false;
      #endif
   }

   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Exit MeasurementManager::ProcessingComplete() method\n");
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// bool Finalize()
//------------------------------------------------------------------------------
/**
 * This method performs the final clean-up for a MeasurementManager prior to
 * deletion
 *
 * @return true if the object was cleaned up, false is a problem was encountered
 */
//------------------------------------------------------------------------------
bool MeasurementManager::Finalize()
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Entered MeasurementManager::Finalize() method\n");
   #endif

   bool retval = true;
   return retval;
}


//------------------------------------------------------------------------------
// Integer Calculate(const Integer measurementToCalc, bool withEvents)
//------------------------------------------------------------------------------
/**
 * Calculate the selected measurement for the current state
 *
 * @param measurementToCalc The ID of the measurement that gets calculated.  If
 *                          the ID is -1, all measurements are attempted
 * @param withEvents Flag used to perform calculation with event data
 *
 * @return The number of measurements that were calculated
 */
//------------------------------------------------------------------------------
Integer MeasurementManager::Calculate(const Integer measurementToCalc,
      bool withEvents)
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
            "Entered MeasurementManager::Calculate(%d, %s) method\n",
            measurementToCalc, (withEvents?"true":"false"));
   #endif

   Integer successCount = 0;
   eventCount = 0;

   if (measurementToCalc == -1)
   {
      for (UnsignedInt j = 0; j < models.size(); ++j)
      {
         // Specify ramp table associated with measurement model models[j]:
         // Note: Only one ramp table is used for a measurement model
///// TBD: Do we want something more generic here?
         StringArray sr = models[j]->GetStringArrayParameter("RampTables");
         std::vector<RampTableData>* rt = NULL;
         if (sr.size() > 0)
            rt = &(rampTables[sr[0]]);

         // Specify current observation data. If no observation data used, it passes a NULL pointer. 
         ObservationData* od = NULL;
         if (!observations.empty())
            od = &(*currentObs);

         measurements[j] = models[j]->CalculateMeasurement(withEvents, od, rt);

         if (measurements[j].isFeasible)
         {
            ++successCount;
            eventCount += measurements[j].eventCount;
         }
      }

      for (UnsignedInt j = 0; j < adapters.size(); ++j)
      {
         // Specify ramp table associated with measurement model models[j]:
         // Note: Only one ramp table is used for a measurement model
///// TBD: Do we want something more generic here?
         //StringArray sr = models[j]->GetStringArrayParameter("RampTables");
         StringArray sr = adapters[j]->GetStringArrayParameter("RampTables");
         std::vector<RampTableData>* rt = NULL;
         if (sr.size() > 0)
            rt = &(rampTables[sr[0]]);

         // Specify current observation data. If no observation data used, it
         // passes a NULL pointer.
         ObservationData* od = NULL;
         if (!observations.empty())
            od = &(*currentObs);

         measurements[j] = adapters[j]->CalculateMeasurement(withEvents, od, rt);
         MessageInterface::ShowMessage("Calculating adapter based measurement...");

         if (measurements[j].isFeasible)
         {
            MessageInterface::ShowMessage("Calculated feasible measurement\n");
            ++successCount;
            eventCount += measurements[j].eventCount;
         }
         else
            MessageInterface::ShowMessage("NOT feasible\n");

      }
   }
   else
   {
      if ((measurementToCalc < (Integer)models.size()) && measurementToCalc >= 0)
      {
///// TBD: Do we want something more generic here?
         // Specify ramp table associated with measurement model models[j]:
         // Note: Only one ramp table is used for a measurement model
         StringArray sr = models[measurementToCalc]->GetStringArrayParameter("RampTables");
         std::vector<RampTableData>* rt = NULL;
         if (sr.size() > 0)
            rt = &(rampTables[sr[0]]);

         // Specify current observation data. If no observation data used, it passes a NULL pointer. 
         ObservationData* od = NULL;
         if (!observations.empty())
            od = &(*currentObs);

         measurements[measurementToCalc] = 
            models[measurementToCalc]->CalculateMeasurement(withEvents, od, rt);
         
         if (measurements[measurementToCalc].isFeasible)
         {
            successCount = 1;
            eventCount = measurements[measurementToCalc].eventCount;
         }
      }

      if ((measurementToCalc < (Integer)adapters.size()) && (measurementToCalc >= 0))
      {
///// TBD: Do we want something more generic here?
         // Specify ramp table associated with tracking data adapter adapters[j]:
         // Note: Only one ramp table is used for a tracking data adapter
         // StringArray sr = models[measurementToCalc]->GetStringArrayParameter("RampTables");
         StringArray sr = adapters[measurementToCalc]->GetStringArrayParameter("RampTables");
         std::vector<RampTableData>* rt = NULL;
         if (sr.size() > 0)
            rt = &(rampTables[sr[0]]);

         // Specify current observation data. If no observation data used, it passes a NULL pointer.
         ObservationData* od = NULL;
         if (!observations.empty())
            od = &(*currentObs);

         #ifdef DEBUG_CALCULATE
            MessageInterface::ShowMessage("****** adapters[%d]name = '%s'\n",
                  measurementToCalc,
                  adapters[measurementToCalc]->GetName().c_str());
            MessageInterface::ShowMessage("****** observations.size() = %d\n", observations.size());
            if (od == NULL)
               MessageInterface::ShowMessage("****** Observation data is not used in calculation\n");
            else
               MessageInterface::ShowMessage("****** currentObs: epoch = %.12lf, participants: %s  %s,  meas value = %.12lf\n", currentObs->epoch, currentObs->participantIDs[0].c_str(), currentObs->participantIDs[1].c_str(), currentObs->value[0]);
         #endif
         
         measurements[measurementToCalc] =
            adapters[measurementToCalc]->CalculateMeasurement(withEvents, od, rt);
         
         #ifdef DEBUG_CALCULATE
            MessageInterface::ShowMessage("****** measurements[%d] = <%p>,   "
                  ".epoch = %.12lf,   .participants: %s  %s,   "
                  ".value[0] = %.12le\n", measurementToCalc,
                  &measurements[measurementToCalc],
                  measurements[measurementToCalc].epoch,
                  measurements[measurementToCalc].participantIDs[0].c_str(),
                  measurements[measurementToCalc].participantIDs[1].c_str(),
                  measurements[measurementToCalc].value[0]);
         #endif

         if (measurements[measurementToCalc].isFeasible)
         {
            successCount = 1;
            eventCount = measurements[measurementToCalc].eventCount;
         }
      }
   }

   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
            "   Found %d events to process\n", eventCount);
      MessageInterface::ShowMessage(
            "Exit MeasurementManager::Calculate(%d, %s) method\n",
            measurementToCalc, (withEvents?"true":"false"));
   #endif

   return successCount;
}

//------------------------------------------------------------------------------
// MeasurementData* GetMeasurement(const Integer measurementToGet)
//------------------------------------------------------------------------------
/**
 * Retrieves a calculated measurement
 *
 * @param measurementToGet ID of the desired measurement
 *
 * @return Pointer to the measurement, or NULL if the measuremetn was not
 *         available
 */
//------------------------------------------------------------------------------
const MeasurementData* MeasurementManager::GetMeasurement(
         const Integer measurementToGet)
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Entered MeasurementManager::GetMeasurement(measurentToGet = %d) method\n", measurementToGet);
   #endif

   MeasurementData *theMeasurement = NULL;
   if ( (measurementToGet >= 0) &&
        (measurementToGet < (Integer)measurements.size()))
      theMeasurement = &measurements[measurementToGet];

   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Exit MeasurementManager::GetMeasurement() method\n");
   #endif

   return theMeasurement;
}


//------------------------------------------------------------------------------
// MeasurementModel* GetMeasurementObject(const Integer measurementToGet)
//------------------------------------------------------------------------------
/**
 * Accessor for specific measurement model objects
 *
 * @param measurementToGet Index to the requested measurement model
 *
 * @return The model
 */
//------------------------------------------------------------------------------
MeasurementModelBase* MeasurementManager::GetMeasurementObject(
      const Integer measurementToGet)
{
   MeasurementModelBase *retval = NULL;

   if ((measurementToGet >= 0) && (measurementToGet < (Integer)models.size()))
      retval = models[measurementToGet];
   else if ((measurementToGet >= 0) && (measurementToGet < (Integer)adapters.size()))
      retval = adapters[measurementToGet];


   return retval;
}


//-----------------------------------------------------------------------------
// Integer GetEventCount(const Integer forMeasurement)
//-----------------------------------------------------------------------------
/**
 * Returns the number of events associated with a specific measurement
 *
 * @param forMeasurement The index of the measurement
 *
 * @return The number of associated events
 */
//-----------------------------------------------------------------------------
Integer MeasurementManager::GetEventCount(const Integer forMeasurement)
{
   #ifdef DEBUG_EVENTS
      MessageInterface::ShowMessage("MeasurementManager::GetEventCount(%d) "
            "called", forMeasurement);
   #endif
   Integer retval = 0;

   if (forMeasurement == -1)
      retval = eventCount;
   else if ((forMeasurement >= 0) &&
            (forMeasurement < (Integer)measurements.size()))
      retval = measurements[forMeasurement].eventCount;

   #ifdef DEBUG_EVENTS
      MessageInterface::ShowMessage("; Event count is %d\n", retval);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool WriteMeasurement(const Integer measurementToWrite)
//------------------------------------------------------------------------------
/**
 * Sends the selected measurement to a MeasurementStream
 *
 * @param measurementToWrite ID of the calculated measurement that is to be
 *                           written
 *
 * @return true if the measurement was sent to a MeasurementStream; false if
 *         not (either because the stream was not available or because the
 *         measurement is not possible)
 */
//------------------------------------------------------------------------------
bool MeasurementManager::WriteMeasurement(const Integer measurementToWrite)
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
            "Entered MeasurementManager::WriteMeasurement() method\n");
   #endif

   bool wasWritten = false;

   return wasWritten;
}



//-----------------------------------------------------------------------------
// UsignedInt LoadObservations()
//-----------------------------------------------------------------------------
/**
 * Opens the observation data sources and reads in all available observations.
 *
 * return   number of observation data
 *
 * @todo Once multiple observations sources are in use, this method will need to
 * sort the observations into time order.
 */
//-----------------------------------------------------------------------------
UnsignedInt MeasurementManager::LoadObservations()
{
   #ifdef DEBUG_LOAD_OBSERVATIONS
      MessageInterface::ShowMessage(
         "Entered MeasurementManager::LoadObservations() method\n");
   #endif

   #ifdef USE_DATAFILE_PLUGINS
      DataFileAdapter dfa;
   #endif

   // Open all streams in streamList
   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      #ifdef DEBUG_LOAD_OBSERVATIONS
         MessageInterface::ShowMessage("Opening the data stream to file %s\n",
               streamList[i]->GetName().c_str()); 
      #endif

      #ifdef USE_DATAFILE_PLUGINS
         std::string writeMode = (simulating ? "w" : "r");
         streamList[i]->SetStringParameter(
               streamList[i]->GetParameterID("ReadWriteMode"), writeMode);

         if (streamList[i]->OpenFile() == false)
            retval = false;
      #else
         if (streamList[i]->OpenStream(false) == false)
            throw MeasurementException("Error: Cannot open file '" + streamList[i]->GetName() + "'.\n");
      #endif
   }
   
   // Initialize trackingConfigsMap
   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      StringArray sa;
      trackingConfigsMap[i] = sa;
   }


   std::map<std::string, Integer> totalCount;
   totalCount["Invalid measurement value"]        = 0;
   totalCount["Record duplication or time order"] = 0;
   totalCount["Old Syntax's Trackers selection"]  = 0;
   totalCount["Old Syntax's Data thinning"]       = 0;
   totalCount["Old Syntax's Time span"]           = 0;

   observations.clear();

   std::vector<UnsignedInt> numRec;                    // numRec[i] is number of records of data file specified by streamList[i] 
   std::vector<UnsignedInt> count;                     // count[i] is number of all accepted records associated with file specified by streamList[i] after applying statistic filters
   std::vector<ObservationData*> dataBuffer;           // dataBuffer[i] contains the current data record read from streamList[i]  
   ObservationData* odPointer;

   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      odPointer = streamList[i]->ReadObservation();
      dataBuffer.push_back(odPointer);
      
      if (odPointer == NULL)
         numRec.push_back(0);
      else
         numRec.push_back(1);

      count.push_back(0);
   }
   
   ObservationData od;
   while (true)
   {
      // 1. get a data record in data buffer with smallest value of epoch
      UnsignedInt minIndex = dataBuffer.size();     // point to the outside of data buffer
      for (UnsignedInt i = 0; i < dataBuffer.size(); ++i)
      {
         if (dataBuffer[i] == NULL)
            continue;

         if (minIndex == dataBuffer.size())
            minIndex = i;
         else
         {
            if (dataBuffer[i]->epoch < dataBuffer[minIndex]->epoch)
               minIndex = i;
         }
      }
      
      // 2. if dada buffer contains all NULL data record (that means all streams at EOF), then exit while loop
      if (minIndex == dataBuffer.size())
         break;
      
      // 3. Filter the data record do it for all filters
      Integer rejectedReason = 0;
      ObservationData* obsData = dataBuffer[minIndex];
      // This observation data is belong to data file: streamList[minIndex]. Therefore, it needs to use filters defined in that data file object
      ObservationData* selectedData = streamList[minIndex]->FilteringData(obsData, rejectedReason);
      
      // 4. if it passes all filters then add it to observations
      if (selectedData != NULL)
      {
         od = *selectedData;
         observations.push_back(od);
         count[minIndex] = count[minIndex] + 1;

         //@todo: generate tracking config here
         std::stringstream ss;
         ss << "{";
         for (UnsignedInt i = 0; i < od.participantIDs.size(); ++i)
         {
            if (i != 0)
               ss << ",";
            ss << od.participantIDs[i];
         }
         ss << "}," << od.typeName;

         if (trackingConfigsMap[minIndex].empty())
            trackingConfigsMap[minIndex].push_back(ss.str());
         else if (find(trackingConfigsMap[minIndex].begin(), trackingConfigsMap[minIndex].end(), ss.str()) == trackingConfigsMap[minIndex].end())
            trackingConfigsMap[minIndex].push_back(ss.str());
      }
      
      // 5. count throw recods based on rejectedReason
      switch (rejectedReason)
      {
         case 1:
            ++totalCount["Old Syntax's Data thinning"];
            break;
         case 2:
            ++totalCount["Old Syntax's Time span"];
            break;
         case 3:
            ++totalCount["Invalid measurement value"];
            break;
         case 4:
            ++totalCount["Record duplication or time order"];
            break;
         case 5:
            ++totalCount["Old Syntax's Trackers selection"];
            break;
         default:
            {
               if (rejectedReason >=6)
               { 
                  Integer filterIndex = rejectedReason - 6;
                  std::vector<DataFilter*>& filters = streamList[minIndex]->GetFilterList();                  
                  if (filterIndex < filters.size())
                  {
                     std::string filterName = "";
                     if (filters[filterIndex]->IsOfType("StatisticsAcceptFilter"))
                        filterName += "All Statistics Accept Filter";
                     else if  (filters[filterIndex]->IsOfType("StatisticsRejectFilter"))
                     {
                        filterName += "StatisticsRejectFilter ";
                        filterName += filters[filterIndex]->GetName();
                     }
                     ++totalCount[filterName];
                  }
               }
            }
      }
      
      // 6. Read data record from streamList[minIndex] to fill data buffer if the stream is not EOF
      if (dataBuffer[minIndex] != NULL)
      {
         dataBuffer[minIndex] = streamList[minIndex]->ReadObservation();
         if (dataBuffer[minIndex] != NULL)
            numRec[minIndex] = numRec[minIndex] + 1;        // count up number of read records if it really has one record read from file
      }
   }

   // 7. Display all statistic of data records
   Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
   MessageInterface::ShowMessage("Number of thrown records due to:\n");
   for(std::map<std::string,Integer>::iterator i = totalCount.begin(); i != totalCount.end(); ++i)
   {
      //if ((runmode != GmatGlobal::TESTING) && (runmode != GmatGlobal::TESTING_NO_PLOTS))
      if (runmode != GmatGlobal::TESTING)
      {
         if (i->first.substr(0, 3) == "Old")
            continue;
      }
      MessageInterface::ShowMessage("     .%s : %d\n", i->first.c_str(), i->second); 
   }


   for (UnsignedInt i = 0; i < streamList.size(); ++i)
      MessageInterface::ShowMessage("Data file '%s' has %d of %d records used for estimation.\n", streamList[i]->GetStringParameter("Filename").c_str(), count[i], numRec[i]);

   #ifdef DEBUG_LOAD_OBSERVATIONS
      for (UnsignedInt i = 0; i < observations.size(); ++i)
         MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf\n", observations[i].dataFormat.c_str(), observations[i].epoch, observations[i].typeName.c_str(), observations[i].type, observations[i].participantIDs[0].c_str(), observations[i].participantIDs[1].c_str(), observations[i].value[0]);
   #endif

   // Set the current data pointer to the first observation value
   currentObs = observations.begin();
   MessageInterface::ShowMessage("Total number of load records : %d\n\n", observations.size());

   for(UnsignedInt i = 0; i < trackingConfigsMap.size(); ++i)
   {
      MessageInterface::ShowMessage("List of tracking configurations (present in participant ID) for load records from data file '%s':\n", streamList[i]->GetName().c_str());
      if (trackingConfigsMap[i].size() == 0)
         MessageInterface::ShowMessage("   None\n");
      else
      {
         for (UnsignedInt j = 0; j < trackingConfigsMap[i].size(); ++j)
            MessageInterface::ShowMessage("   Config %d: {%s}\n", j, trackingConfigsMap[i].at(j).c_str());
      }
   }
   MessageInterface::ShowMessage("\n");

   #ifdef DEBUG_LOAD_OBSERVATIONS
     MessageInterface::ShowMessage(
         "Exit MeasurementManager::LoadObservations() method\n");
   #endif

   return observations.size(); 
}


//-----------------------------------------------------------------------------
// bool AutoGenerateTrackingDataAdapters()
//-----------------------------------------------------------------------------
/**
* This function is used to generate TrackingDataAdapters automatically based on
* observation data.
*
*/
//-----------------------------------------------------------------------------
bool MeasurementManager::AutoGenerateTrackingDataAdapters()
{
#ifdef DEBUG_AUTO_GENERATE_TRACKINGDATAADAPTERS
   MessageInterface::ShowMessage("Enter MeasurementManager::AutoGenerateTrackingDataAdapters()\n");
#endif

   // Get a list of TrackingFileSet having no tracking configs
   for(UnsignedInt i = 0; i < trackingSets.size(); ++i)
   {
      StringArray list = trackingSets[i]->GetStringArrayParameter("AddTrackingConfig");
      if (list.empty())
      {
         // For each tracking file set object, It generates tracking data adapters for trackingSets[i]
         StringArray createList;

         // 1. Get list data files
         StringArray fileNamesList = trackingSets[i]->GetStringArrayParameter("FileName");
         for (UnsignedInt j = 0; j < fileNamesList.size(); ++j)
         {
            // Get file index for fileNamesList[j]
            UnsignedInt index;
            for (UnsignedInt k = 0; k < streamList.size(); ++k)
            {
               if (GmatStringUtil::ToUpper(fileNamesList[j]) == GmatStringUtil::ToUpper(streamList[k]->GetName()))
               {
                  index = k;
                  break;
               }
            }

            // Get a list of tracking configurations for this data file
            StringArray tclist = trackingConfigsMap[index];
            for (UnsignedInt k = 0; k < tclist.size(); ++k)
            {
               if (createList.empty())
                  createList.push_back(tclist[k]);
               else if (find(createList.begin(), createList.end(), tclist[k]) == createList.end())
                  createList.push_back(tclist[k]);
            }
         }

         // 2. Create tracking data adapters for trackingSets[i] based on createList
         // 2.0. Get a list of stations and spacecrafts
         ObjectMap objectmap = trackingSets[i]->GetConfiguredObjectMap();
         ObjectArray partList;
         for (ObjectMap::iterator j = objectmap.begin(); j != objectmap.end(); ++j)
         {
            if (((*j).second->IsOfType(Gmat::GROUND_STATION))||
                ((*j).second->IsOfType(Gmat::SPACECRAFT)))
               partList.push_back((*j).second);
         }

         // 2.1. Prepare all inputs
         std::vector<StringArray> strands;
         StringArray types;

         for (UnsignedInt j = 0; j < createList.size(); ++j)
         {
            StringArray participants;
            std::string participantID;
            Integer count;
            GmatBase* obj;

            std::string config = createList[j];
            size_t pos = config.find_last_of(',');                     // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
            std::string type = config.substr(pos+1, config.size()-(pos+1));
            std::string strand = config.substr(1, pos-2);
            while (strand != "")
            {
               pos = strand.find_first_of(',');
               if (pos != strand.npos)
               {
                  participantID = strand.substr(0,pos);
                  strand = strand.substr(pos+1);
               }
               else
               {
                  participantID = strand;
                  strand = "";
               }

               // Convert participant ID to participant object
               count = 0;
               obj = NULL;
               for (UnsignedInt k = 0; k < partList.size(); ++k)
               {
                  if (partList[k]->GetStringParameter("Id") == participantID)
                  {
                     ++count;
                     obj = partList[k];
                  }
               }
               if (count == 0)
                  throw MeasurementException("Error: Failed to generate tracking configuration due to neither station nor spacecraft defined in your script has Id = '" + participantID + "'\n" );
               else if (count > 1)
                  throw MeasurementException("Error: Failed to generate tracking configuation due to 2 or more GMAT objects having the same Id = '" + participantID + "'\n");

               // Set name
               participants.push_back(obj->GetName());
               // Set reference object for tracking file set
               try
               {
                  trackingSets[i]->SetRefObject(obj, obj->GetType(), obj->GetName());
               }
               catch(...)
               {
                  // skip when it fail to set referent object
               }
            }

            //participants.push_back(strand);
            strands.push_back(participants);
            types.push_back(type);
         }

         // 2.2. Generate tracking configs
         trackingSets[i]->GenerateTrackingConfigs(strands, types);

         // 3. Set stream objects and data filters for all observation data files in trackingSet[i]
         // 3.1. Get list of all data filters defined in trackingSet[i]
         ObjectArray dataFilterObjects = trackingSets[i]->GetRefObjectArray(Gmat::DATA_FILTER);
         // 3.2. Get list of all file names defined in trackingSet[i]
         StringArray filenames = trackingSets[i]->GetStringArrayParameter("FileName");

         for(UnsignedInt j = 0; j < filenames.size(); ++j)
         {
            // 3.3. Set data filter to data file object
            // 3.3.1. Get data file object with name filenames[j]
            DataFile* fileObj = NULL;
            for (UnsignedInt k = 0; k < streamList.size(); ++k)
            {
               if (streamList[k]->GetName() == filenames[j])
               {
                  fileObj = streamList[k];
                  break;
               }
            }

            // 3.3.2. Set data filters of type "Statistics" filter to DataFile object
            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage(" &&&&   There are %d data filter objects.\n", dataFilterObjects.size());
            #endif
            for (UnsignedInt k = 0; k < dataFilterObjects.size(); ++k)
            {
               if ((dataFilterObjects[k]->IsOfType("StatisticsAcceptFilter"))||(dataFilterObjects[k]->IsOfType("StatisticsRejectFilter")))
               {
                  // data filter is only set to data file if and only if datafilter.Filenames parameter 
                  // is an empty list or contains the name of data file.
                  StringArray nameList = dataFilterObjects[k]->GetStringArrayParameter("FileNames");
               
                  bool setfilter = false;
                  if (nameList.size() == 0)
                     setfilter = true;
                  else
                  {
                     if ((dataFilterObjects[k]->IsOfType("StatisticsAcceptFilter"))&&
                         ((find(nameList.begin(), nameList.end(), "From_AddTrackingConfig") != nameList.end())||
                          (find(nameList.begin(), nameList.end(), "All") != nameList.end())))
                        setfilter = true;
                     else
                     {
                        for (UnsignedInt q = 0; q < nameList.size(); ++q)
                        {
                           if (GmatStringUtil::ToUpper(nameList[q]) == GmatStringUtil::ToUpper(filenames[j]))
                           {
                              setfilter = true;
                              break;
                           }
                        }
                     }
                  }

                  if (setfilter)
                  {
                     #ifdef DEBUG_INITIALIZATION
                        MessageInterface::ShowMessage(" &&&&   Set data filter <%s,%p> to data file <%s,%p>.\n", dataFilterObjects[k]->GetName().c_str(), dataFilterObjects[k], fileObj->GetName().c_str(), fileObj);
                     #endif
                     fileObj->SetDataFilter((DataFilter*)dataFilterObjects[k]);
                  }
               }
            }
         }

         // 4. Load data adapters from tracking file sets to measurement manager
         std::vector<TrackingDataAdapter*> *setAdapters = trackingSets[i]->GetAdapters();
         StringArray names;
         for (UnsignedInt j = 0; j < setAdapters->size(); ++j)
         {
            AddMeasurement((*setAdapters)[j]);
            MeasurementData md;
            measurements.push_back(md);
            names.push_back((*setAdapters)[j]->GetName());
         }
         adapterFromTFSMap[trackingSets[i]] = names;
      }
      else
         MessageInterface::ShowMessage("****   No tracking configuration was "
               "generated because the tracking configuration is defined in the "
               "script.\n");
   }// for i loop


#ifdef DEBUG_AUTO_GENERATE_TRACKINGDATAADAPTERS
   MessageInterface::ShowMessage("Exit MeasurementManager::AutoGenerateTrackingDataAdapters()\n");
#endif
   return true;
}



//UnsignedInt MeasurementManager::LoadObservationsOld()
//{
//   #ifdef DEBUG_LOAD_OBSERVATIONS
//      MessageInterface::ShowMessage(
//         "Entered MeasurementManager::LoadObservations() method\n");
//   #endif
//
//   #ifdef USE_DATAFILE_PLUGINS
//      DataFileAdapter dfa;
//   #endif
//
//   std::vector<ObservationData>     obsTable;
//   std::vector<UnsignedInt>         startIndexes;
//   std::vector<UnsignedInt>         endIndexes;
//   observations.clear();
//   
//   for (UnsignedInt i = 0; i < streamList.size(); ++i)
//   {
//      startIndexes.push_back(obsTable.size());
//
//      ObservationData *od;
/////// TBD: Look at file handlers here once data file pieces are designed
//      ObservationData od_old;
//      od_old.epoch = -1.0;
//
//      std::string streamFormat = streamList[i]->GetStringParameter("Format");
//
/////// TBD: Especially here; this style will cause maintenence issues as more types are added
//      if ((streamFormat == "GMAT_OD") || (streamFormat == "GMAT_ODDoppler") ||
//          (streamFormat == "GMATInternal") || (streamFormat == "TDM"))      // It needs for loading all type of observation data (except ramp table)
//      {
//      #ifdef USE_DATAFILE_PLUGINS
//         if (streamList[i]->GetIsOpen())
//         {
//            ObType *obs;
//
//            // This part needs some design information from Matt
//            ObType *obd = dfa.GetObTypeObject(streamList[i]);
//            if (!streamList[i]->GetData(obd))
//            {
//               throw MeasurementException("Could not load observation\n");
//            }
//            dfa.LoadObservation(*obd, *od);
//
//            while (!streamList[i]->IsEOF())
//            {
//               if (streamList[i]->GetData(obs))
//               {
//                  // Store the data for processing
//               }
//               streamList[i]->AdvanceToNextOb();
//            }
//         }
//      #else
//
//         if (streamList[i]->IsOpen())
//         {
//            UnsignedInt filter0Num;
//            filter0Num = 0;
//            UnsignedInt filter1Num, filter2Num, filter3Num, filter4Num, filter5Num, count, numRec;
//            filter1Num = filter2Num = filter3Num = filter4Num = filter5Num =  count = numRec = 0;
//            Real acc = 1.0;
//
//            Real epoch1 = 0.0;
//            Real epoch2 = 0.0;
//
//            Real thinningRatio = streamList[i]->GetRealParameter("DataThinningRatio"); 
//            StringArray selectedStations = streamList[i]->GetStringArrayParameter("SelectedStationIDs"); 
//            bool EndofFile = false;
//            while (true)
//            {
//               od = streamList[i]->ReadObservation();
//               ++numRec;
//               
//               // End of file
//               if (EndofFile)
//               {
//                  --numRec;
//                  break;
//               }
//
//               if (od == NULL)
//               {
//                  ++filter0Num;
//                  continue;
//               }
//
//               #ifdef DUMP_OBSDATA
//                  // Write out the observation
//                  MessageInterface::ShowMessage("In MeasurementManager: Observation record at %p:\n", od);
//                  MessageInterface::ShowMessage("   typeName:  %s\n", od->typeName.c_str());
//                  MessageInterface::ShowMessage("   type:  %d\n", od->type);
//                  MessageInterface::ShowMessage("   inUsed:  %s\n", (od->inUsed ? "true" : "false"));
//                  MessageInterface::ShowMessage("   removedReason:  %s\n", od->removedReason.c_str());
//                  MessageInterface::ShowMessage("   uniqueID:  %d\n", od->uniqueID);
//                  MessageInterface::ShowMessage("   epochSystem:  %d\n", od->epochSystem);
//                  MessageInterface::ShowMessage("   epoch:  %.12le\n", od->epoch);
//                  MessageInterface::ShowMessage("   epochAtEnd:  %s\n", (od->epochAtEnd ? "true" : "false"));
//                  MessageInterface::ShowMessage("   epochAtIntegrationEnd:  %s\n", (od->epochAtIntegrationEnd ? "true" : "false"));
//                  MessageInterface::ShowMessage("   participantIDs:  %d members\n", od->participantIDs.size());
//                  for (UnsignedInt i = 0; i < od->participantIDs.size(); ++i)
//                     MessageInterface::ShowMessage("      %d: %s\n", i, od->participantIDs[i].c_str());
//                  MessageInterface::ShowMessage("   strands: %d strands in the record\n", od->strands.size());
//                  MessageInterface::ShowMessage("   value:  %d members\n", od->value.size());
//                  MessageInterface::ShowMessage("   dataMap:  %d members\n", od->dataMap.size());
//                  for (UnsignedInt i = 0; i < od->value.size(); ++i)
//                  {
//                     if (od->dataMap.size() > i)
//                        MessageInterface::ShowMessage("      %s --> ", od->dataMap[i].c_str());
//                     else
//                        MessageInterface::ShowMessage("      ");
//                     MessageInterface::ShowMessage("%.12lf\n", od->value[i]);
//                  }
//                  MessageInterface::ShowMessage("   value_orig:  %d members\n", od->value_orig.size());
//                  for (UnsignedInt i = 0; i < od->value_orig.size(); ++i)
//                  {
//                     if (od->dataMap.size() > i)
//                        MessageInterface::ShowMessage("      %s --> ", od->dataMap[i].c_str());
//                     else
//                        MessageInterface::ShowMessage("      ");
//                     MessageInterface::ShowMessage("%.12lf\n", od->value_orig[i]);
//                  }
//                  MessageInterface::ShowMessage("   unit:  %s\n", od->unit.c_str());
//                  MessageInterface::ShowMessage("   noiseCovariance:  <%p>\n", od->noiseCovariance);
//                  MessageInterface::ShowMessage("   extraDataDescriptions:  %d members\n", od->extraDataDescriptions.size());
//                  MessageInterface::ShowMessage("   extraTypes:  %d members\n", od->extraTypes.size());
//                  MessageInterface::ShowMessage("   extraData:  %d members\n", od->extraData.size());
//                  MessageInterface::ShowMessage("   uplinkBand:  %d\n", od->uplinkBand);
//                  MessageInterface::ShowMessage("   uplinkFreq:  %.12le\n", od->uplinkFreq);
//                  MessageInterface::ShowMessage("   rangeModulo: %.12le\n", od->rangeModulo);
//                  MessageInterface::ShowMessage("   dopplerCountInterval:  %.12le\n", od->dopplerCountInterval);
//               #endif
//
//               // Get start epoch and end epoch when od != NULL
//               if (epoch1 == 0.0)
//               {
//                  epoch1 = TimeConverterUtil::Convert(streamList[i]->GetRealParameter("StartEpoch"), TimeConverterUtil::A1MJD, od->epochSystem);
//                  epoch2 = TimeConverterUtil::Convert(streamList[i]->GetRealParameter("EndEpoch"), TimeConverterUtil::A1MJD, od->epochSystem);
//               }
//               
//               // Data thinning filter
//               acc = acc + thinningRatio;
//               if (acc < 1.0)
//               {
//                  #ifdef DEBUG_LOAD_OBSERVATIONS
//                     MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf :Throw away this record due to data thinning\n", streamFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
//                  #endif
//                  ++filter4Num;
//                  continue;
//               }
//               else
//                  acc = acc -1.0;
//               
//               // Time span filter
//               if ((od->epoch < epoch1)||(od->epoch > epoch2))
//               {
//                  #ifdef DEBUG_LOAD_OBSERVATIONS
//                     MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf :Throw away this record due to time span filter\n", streamFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
//                  #endif
//                  ++filter5Num;
//                  continue;
//               }
//               
//               // Invalid measurement value filter
//               if (od->value.size() > 0)
//                  if (od->value[0] == -1.0)      // throw away this observation data if it is invalid
//                  {
//                     #ifdef DEBUG_LOAD_OBSERVATIONS
//                        MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf :Throw away this record due to invalid observation data\n", streamFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
//                     #endif
//                     ++filter1Num;
//                     continue;
//                  }
//
//               
//               // Duplication or time order filter
//               if (od_old.epoch >= (od->epoch + 2.0e-12))
//               {
//                  #ifdef DEBUG_LOAD_OBSERVATIONS
//                     MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf :Throw away this record due to duplication or time order\n", streamFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
//                  #endif
//                  ++filter2Num;
//                  continue;
//               }
//
//               // Selected stations filter
//               bool choose = false;
//               if (selectedStations.size() == 0)
//                  choose = true;
//               else
//               {
//                  for (int j=0; j < selectedStations.size(); ++j)
//                  {
//                     if (selectedStations[j] == od->participantIDs[0])
//                     {
//                        choose = true;
//                         break;
//                     }
//                  }
//               }
//
//               if (choose == false)
//               {
//                  #ifdef DEBUG_LOAD_OBSERVATIONS
//                     MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf :Throw away this record due to station is not in SelectedStationID\n", streamFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
//                  #endif
//                  ++filter3Num;
//                  continue;
//               }
//
//               obsTable.push_back(*od);
//               #ifdef DEBUG_LOAD_OBSERVATIONS
//                  MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf   ", streamFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
//                  if (od->typeName == "DSNTwoWayDoppler")
//                     MessageInterface::ShowMessage(" Band = %d    Doppler count interval = %le\n", od->uplinkBand, od->dopplerCountInterval);
//                  else if (od->typeName == "DSNTwoWayRange")
//                     MessageInterface::ShowMessage(" Band = %d    Frequency = %.15le   Range Modulo = %.15le\n", od->uplinkBand, od->uplinkFreq, od->rangeModulo);
//                  else
//                     MessageInterface::ShowMessage("\n");
//               #endif
//
//               ++count;
//               od_old = *od;
//            }// endwhile(true)   
//
//            MessageInterface::ShowMessage("Number of thrown records in '%s' due to:\n", streamList[i]->GetStringParameter("Filename").c_str());
//            MessageInterface::ShowMessage("      .Filter by Satistic data filters        : %d\n", filter0Num);
//            MessageInterface::ShowMessage("      .Invalid measurement value              : %d\n", filter1Num);
//            MessageInterface::ShowMessage("      .Duplicated record or time order filter : %d\n", filter2Num);
//            MessageInterface::ShowMessage("      .Selected stations filter               : %d\n", filter3Num);
//            MessageInterface::ShowMessage("      .Data thinning filter                   : %d\n", filter4Num);
//            MessageInterface::ShowMessage("      .Time span filter                       : %d\n", filter5Num);
//            MessageInterface::ShowMessage("Total number of records in '%s': %d\n", streamList[i]->GetStringParameter("Filename").c_str(), numRec);
//            MessageInterface::ShowMessage("Number of records in '%s' used for estimation: %d\n\n", streamList[i]->GetStringParameter("Filename").c_str(), count);
//
//         } // endif (streamList[i]->IsOpen())
//       
//      #endif
//      } // endif ((streamFormat == "GMAT_OD")
//
//      // fix bug GMT-4394
//      if (obsTable.size() == 0)
//         throw MeasurementException("Error: No observation data in tracking data file '" + streamList[i]->GetStringParameter("Filename") +"'\n"); 
//      else
//      {
//         if ((Integer)startIndexes[i] <= (obsTable.size()-1))
//            endIndexes.push_back(obsTable.size()-1);
//         else
//            throw MeasurementException("Error: No observation data in tracking data file '" + streamList[i]->GetStringParameter("Filename") +"'\n"); 
//      }
//     
//   }// for i loop
//
//
//   // Sort observation data by epoch due to observations table is required to have an epoch ascending order
//   bool completed = false;
//   while (!completed)
//   {
//      UnsignedInt minIndex = streamList.size();
//      for (UnsignedInt i = 0; i < streamList.size(); ++i)
//      {
//         if (startIndexes[i] > endIndexes[i])
//            continue;
//
//         if (minIndex == streamList.size())
//            minIndex = i;
//         else
//         {
//            if (obsTable[startIndexes[minIndex]].epoch > obsTable[startIndexes[i]].epoch)
//               minIndex = i;
//         }
//     }
//     
//     if (minIndex < streamList.size())
//     {
//        observations.push_back(obsTable[startIndexes[minIndex]]);
//        startIndexes[minIndex]++;
//     }
//     else
//        completed = true;
//
//   }
//   
//   #ifdef DEBUG_LOAD_OBSERVATIONS
//      for (UnsignedInt i = 0; i < observations.size(); ++i)
//         MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf\n", observations[i].dataFormat.c_str(), observations[i].epoch, observations[i].typeName.c_str(), observations[i].type, observations[i].participantIDs[0].c_str(), observations[i].participantIDs[1].c_str(), observations[i].value[0]);
//   #endif
//
//   // Set the current data pointer to the first observation value
//   currentObs = observations.begin();
//   MessageInterface::ShowMessage("Total number of load records : %d\n", observations.size());
//
//   #ifdef DEBUG_LOAD_OBSERVATIONS
//     MessageInterface::ShowMessage(
//         "Exit MeasurementManager::LoadObservations() method\n");
//   #endif
//
//   return observations.size(); 
//}


//-----------------------------------------------------------------------------
// std::vector<ObservationData>* GetObservationDataList()
//-----------------------------------------------------------------------------
/**
* Get a list of all observations
*/
//-----------------------------------------------------------------------------
std::vector<ObservationData>* MeasurementManager::GetObservationDataList()
{
   return &observations;
}


//-----------------------------------------------------------------------------
// void LoadRampTables()
//-----------------------------------------------------------------------------
/**
 * Load all frequency ramp tables.
 */
//-----------------------------------------------------------------------------
void MeasurementManager::LoadRampTables()
{
   #ifdef DEBUG_LOAD_FREQUENCY_RAMP_TABLE
      MessageInterface::ShowMessage("Entered MeasurementManager::LoadRampTables() method\n");
      MessageInterface::ShowMessage("  rampTableDataStreamList.size() = %d\n", rampTableDataStreamList.size());
   #endif

   #ifdef USE_DATAFILE_PLUGINS
      DataFileAdapter dfa;
   #endif

   rampTables.clear();

   for (UnsignedInt i = 0; i < rampTableDataStreamList.size(); ++i)
   {
      RampTableData *rtd;
      RampTableData rtd_old;
      rtd_old.epoch = -1.0;
      if (rampTableDataStreamList[i]->GetStringParameter("Format") == "GMAT_RampTable")
      {
      #ifdef USE_DATAFILE_PLUGINS
         if (rampTableDataStreamList[i]->GetIsOpen())
         {
            ObType *obs;

            // This part needs some design information from Matt
            ObType *obd = dfa.GetObTypeObject(rampTableDataStreamList[i]);
            if (!rampTableDataStreamList[i]->GetData(obd))
            {
               throw MeasurementException("Could not load ramp table data\n");
            }
            dfa.LoadRampTable(*obd, *rtd);

            while (!rampTableDataStreamList[i]->IsEOF())
            {
               if (rampTableDataStreamList[i]->GetData(obs))
               {
                  // Store the data for processing
               }
               rampTableDataStreamList[i]->AdvanceToNextOb();
            }
         }
      #else
         if (rampTableDataStreamList[i]->IsOpen())
         {
            if (rampTableDataStreamList[i]->GetStringParameter("Format") == "GMAT_RampTable")
            {
               std::map<std::string, RampTableData> ramp_table_map;
               std::vector<RampTableData> ramp_table;
               rtd = rampTableDataStreamList[i]->ReadRampTableData();
               while (rtd != NULL)
               {
                  // Data records containing rampType = 0(snap), 6(invalid/unknown) , and 7(left bank in DSN file) will be removed from ramp table  
                  //if ((rtd_old.epoch < rtd->epoch)&&(rtd->rampType >= 1)&&(rtd->rampType <= 5))
                  if ((rtd->rampType >= 1)&&(rtd->rampType <= 5))
                  {
                     // Specify keyindex
                     std::stringstream ss;
                     ss.precision(21);
                     for (UnsignedInt p = 0; p < rtd->participantIDs.size(); p++)
                     {
                        ss << rtd->participantIDs[p];
                        ss << " ";          // adding a blank between partcipants
                     }
                     ss << rtd->epoch;
                     rtd->indexkey = ss.str();

                     // Store data record into ramp table map if it is not in there 
                     if (ramp_table_map.find(rtd->indexkey) == ramp_table_map.end())
                        ramp_table_map[rtd->indexkey] = *rtd;

                     #ifdef DEBUG_LOAD_FREQUENCY_RAMP_TABLE
                     MessageInterface::ShowMessage(" epoch: %.15lf   participants: %s   %s   frequency band = %d    ramp type = %d    ramp frequency = %.12le     ramp rate = : %.12le  indexkey = <%s>\n", rtd->epoch, rtd->participantIDs[0].c_str(), rtd->participantIDs[1].c_str(), rtd->uplinkBand, rtd->rampType, rtd->rampFrequency, rtd->rampRate, rtd->indexkey.c_str());
                     #endif

                     rtd_old = *rtd;
                  }
                  else
                  {
                     #ifdef DEBUG_LOAD_FREQUENCY_RAMP_TABLE
                        //MessageInterface::ShowMessage(" epoch: %.15lf   participants: %s   %s   frequency band = %d    ramp type = %d    ramp frequency = %.12le     ramp rate = : %.12le: throw away this record due to the order of time or rampType = 0, 6, or 7\n", rtd->epoch, rtd->participantIDs[0].c_str(), rtd->participantIDs[1].c_str(), rtd->uplinkBand, rtd->rampType, rtd->rampFrequency, rtd->rampRate);
                        MessageInterface::ShowMessage(" epoch: %.15lf   participants: %s   %s   frequency band = %d    ramp type = %d    ramp frequency = %.12le     ramp rate = : %.12le: throw away this record due to rampType = 0, 6, or 7\n", rtd->epoch, rtd->participantIDs[0].c_str(), rtd->participantIDs[1].c_str(), rtd->uplinkBand, rtd->rampType, rtd->rampFrequency, rtd->rampRate);
                     #endif
                  }

                  rtd = rampTableDataStreamList[i]->ReadRampTableData();
               }

               // Store data from ramp table map to ramp_table
               for (std::map<std::string, RampTableData>::iterator q = ramp_table_map.begin(); q != ramp_table_map.end(); ++q)
               {
                  ramp_table.push_back(q->second);
               }

               // store ramp_table to rampTables
               rampTables[rampTableDataStreamList[i]->GetName()] = ramp_table;
               #ifdef DEBUG_LOAD_FREQUENCY_RAMP_TABLE
                  MessageInterface::ShowMessage("Ramp Table:\n");
                  for (UnsignedInt j = 0; j < ramp_table.size(); ++j)
                  {
                     RampTableData data = ramp_table[j];
                     MessageInterface::ShowMessage("%.12lf  %s  %s  %d  %d  %le  %le\n", data.epoch, data.participantIDs[0].c_str(), data.participantIDs[1].c_str(), data.uplinkBand, data.rampType, data.rampFrequency, data.rampRate);
                  }
                  MessageInterface::ShowMessage("      ^^^^^^ ramp_table[%s] = <%p>\n", rampTableDataStreamList[i]->GetName().c_str(), &rampTables[rampTableDataStreamList[i]->GetName()]);
               #endif

               // Clean up memmory
               ramp_table_map.clear();
               ramp_table.clear();
            }
         }
      #endif
      }
   }

   #ifdef DEBUG_LOAD_FREQUENCY_RAMP_TABLE
      MessageInterface::ShowMessage(
         "Exit MeasurementManager::LoadRampTable() method\n");
   #endif
}


//-----------------------------------------------------------------------------
// GmatEpoch GetEpoch()
//-----------------------------------------------------------------------------
/**
 * Retrieves the epoch of the current observation
 *
 * @return The (a.1 modified Julian) epoch of the observation.
 */
//-----------------------------------------------------------------------------
GmatEpoch MeasurementManager::GetEpoch()
{
   if (currentObs == observations.end())
      return 0.0;
   return currentObs->epoch;
}


//-----------------------------------------------------------------------------
// GmatEpoch GetNextEpoch()
//-----------------------------------------------------------------------------
/**
 * Retrieves the epoch for the next available observation
 *
 * @return The (a.1 modified Julian) epoch of the next observation.
 */
//-----------------------------------------------------------------------------
GmatEpoch MeasurementManager::GetNextEpoch()
{
   std::vector<ObservationData>::iterator nextObs = observations.end();
   if (currentObs != observations.end())
      nextObs = currentObs + 1;

   if (nextObs == observations.end())
      return 0.0;

   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage("Next epoch: %.12lf\n", nextObs->epoch);
   #endif
   return nextObs->epoch;
}


//-----------------------------------------------------------------------------
// const ObservationData *GetObsData(const Integer observationToGet)
//-----------------------------------------------------------------------------
/**
 * Access method for the observation data
 *
 * This method is used to access observations.  If the input parameter,
 * observationToGet, is -1, the current observation is returned.  If it is set
 * to a non-negative number, the observation at that index in the observations
 * vector is returned.  If neither case is valid, a NULL pointer is returned.
 *
 * @param observationToGet Index of the desired observation, or -1 for the
 *                         current observation.
 *
 * @return A pointer to the observation
 */
//-----------------------------------------------------------------------------
const ObservationData *MeasurementManager::GetObsData(
      const Integer observationToGet)
{
   if (observationToGet == -1)
      return &(*currentObs);

   if ((observationToGet < 0) ||
       (observationToGet >= (Integer)observations.size()))
      // Should throw here
      return NULL;

   return &(observations[observationToGet]);
}


//-----------------------------------------------------------------------------
// ObservationData* GetObsDataObject(const Integer observationToGet)
//-----------------------------------------------------------------------------
/**
 * This method is used to get an observation data object.  If the input parameter,
 * observationToGet, is -1, the current observation data object is returned.  If it is set
 * to a non-negative number, the observation data object at that index in the observations
 * vector is returned.  If neither case is valid, a NULL pointer is returned.
 *
 * @param observationToGet Index of the desired observation data object, or -1 for the
 *                         current observation data object.
 *
 * @return A pointer to the observation data object
 */
//-----------------------------------------------------------------------------
ObservationData* MeasurementManager::GetObsDataObject(const Integer observationToGet)
{
   if (observationToGet == -1)
   {
      return &(*currentObs);
   }

   if ((observationToGet < 0) ||
       (observationToGet >= (Integer)observations.size()))
      return NULL;

   return &(observations[observationToGet]);
}



//-----------------------------------------------------------------------------
// bool MeasurementManager::AdvanceObservation()
//-----------------------------------------------------------------------------
/**
 * Advances the current observation pointer to the next observation in the
 * observations vector
 *
 * @return   true when it is at the end of observations table otherwise return false
 */
//-----------------------------------------------------------------------------
bool MeasurementManager::AdvanceObservation()
{
   if (currentObs != observations.end())
   {
      ++currentObs;
   }


#ifdef DEBUG_ADVANCE_OBSERVATION
   if (currentObs == observations.end())
      MessageInterface::ShowMessage("MeasurementManager::AdanceObservation():   currentObs == end\n");
   else
      MessageInterface::ShowMessage("MeasurementManager::AdanceObservation():   currentObs->epoch = %.12lf   correntObs->value.size() = %d\n", currentObs->epoch, currentObs->value.size());
#endif

   if (currentObs == observations.end())
      return true;
   else
     return false;
}

//------------------------------------------------------------------------------
// Integer AddMeasurement(MeasurementModel *meas)
//------------------------------------------------------------------------------
/**
 * Sets a new MeasurementModel on the MeasurementManager
 *
 * @param meas The new MeasurementModel
 *
 * @return The ID for the model during this run
 */
//------------------------------------------------------------------------------
Integer MeasurementManager::AddMeasurement(MeasurementModel *meas)
{
   Integer retval = -1;
   if (meas->IsOfType("TrackingFileSet"))
   {
      retval = AddMeasurement((TrackingFileSet*)meas);
   }
   else
   {
      meas->SetModelID(largestId++);
      models.push_back(meas);
      retval = meas->GetModelID();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
            "Added measurement of type %s with unique ID %d\n",
            meas->GetStringParameter("Type").c_str(),
            meas->GetModelID());
      #endif
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer AddMeasurement(MeasurementModel *meas)
//------------------------------------------------------------------------------
/**
 * Sets a TrackingSystem and associated collection of MeasurementModels on the
 * MeasurementManager
 *
 * @param system The new TrackingSystem
 *
 * @return The ID for the first model in this system
 */
//------------------------------------------------------------------------------
Integer MeasurementManager::AddMeasurement(TrackingSystem *system)
{
//   Integer firstModel = largestId;
//
//   MeasurementModel *meas = NULL;
//
//   return firstModel;
//   meas->SetModelID(largestId++);
   systems.push_back(system);

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
            "Added tracking system named %s of type %s with unique ID %d\n",
            system->GetName().c_str(), system->GetTypeName().c_str(),
            -1 /*meas->GetModelID()*/);
   #endif

   return -1; //meas->GetModelID();
}


//------------------------------------------------------------------------------
// const StringArray& GetParticipantList()
//------------------------------------------------------------------------------
/**
 * Accesses the complete list of measurement participants, avoiding duplicates.
 *
 * @return The list.
 */
//------------------------------------------------------------------------------
const StringArray& MeasurementManager::GetParticipantList()
{
   participants.clear();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("MeasurementManager knows about %d "
         "measurement models\n", models.size());
      MessageInterface::ShowMessage("MeasurementManager knows about %d "
            "TrackingDataSets\n", trackingSets.size());
      MessageInterface::ShowMessage("MeasurementManager knows about %d "
         "measurement model names\n", modelNames.size());
      for (UnsignedInt i = 0; i < modelNames.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", modelNames[i].c_str());
   #endif

   // Walk through the collection of measurement models...
   for (std::vector<MeasurementModel*>::iterator i =  models.begin();
         i !=  models.end(); ++i)
   {
      // Walk through the participant list for the model
      StringArray parts = (*i)->GetStringArrayParameter("Participants");
      for (StringArray::iterator j = parts.begin(); j != parts.end(); ++j)
      {
         // If the participant is not in the list yet, add it
         if (find(participants.begin(), participants.end(), (*j)) ==
               participants.end())
            participants.push_back(*j);
      }
   }

   for (std::vector<TrackingFileSet*>::iterator i =  trackingSets.begin();
         i !=  trackingSets.end(); ++i)
   {
      /// @todo: This part will need changes
      // Walk through the participant list for the model
      StringArray parts = (*i)->GetParticipants();
      for (UnsignedInt j = 0; j < parts.size(); ++j)
      {
         // If the participant is not in the list yet, add it
         if (find(participants.begin(), participants.end(), parts[j]) ==
               participants.end())
            participants.push_back(parts[j]);
      }
   }

   return participants;
}


//------------------------------------------------------------------------------
// std::vector<StringArray> GetSignalPathList()
//------------------------------------------------------------------------------
/**
 * Accesses the complete list of all signal paths.
 *
 * @return The list.
 */
//------------------------------------------------------------------------------
std::vector<StringArray> MeasurementManager::GetSignalPathList()
{
   std::vector<StringArray> signalPathList;
   signalPathList.clear();

   // Walk through the collection of measurement models...
   for (std::vector<MeasurementModel*>::iterator i =  models.begin();
         i !=  models.end(); ++i)
   {
      StringArray parts = (*i)->GetStringArrayParameter("Participants");
      signalPathList.push_back(parts);
   }

   for (std::vector<TrackingDataAdapter*>::iterator i =  adapters.begin();
         i !=  adapters.end(); ++i)
   {
      StringArray parts = (*i)->GetParticipants(0);
      signalPathList.push_back(parts);
   }

   return signalPathList;
}


//-----------------------------------------------------------------------------
// void AddMeasurementName(std::string measName)
//-----------------------------------------------------------------------------
/**
 * Adds a named measurement model to the list of managed models.
 *
 * This method is idempotent: the same model can be added multiple times, and
 * only one copy of the name will appear in the list.
 *
 * @param measName The name of the measurement model
 */
//-----------------------------------------------------------------------------
void MeasurementManager::AddMeasurementName(std::string measName)
{
   if (find(modelNames.begin(), modelNames.end(), measName) == modelNames.end())
      modelNames.push_back(measName);
}

//-----------------------------------------------------------------------------
// const StringArray& GetMeasurementNames() const
//-----------------------------------------------------------------------------
/**
 * Retrieves the list of measurement models registered with this
 * MeasurementManager
 *
 * @return The list of models
 */
//-----------------------------------------------------------------------------
const StringArray& MeasurementManager::GetMeasurementNames() const
{
   return modelNames;
}


//-----------------------------------------------------------------------------
// const std::vector<TrackingDataAdapter*>& GetAllTrackingDataAdapters()
//-----------------------------------------------------------------------------
/**
* Get a list of all TrackingAdapter objects
*/
//-----------------------------------------------------------------------------
const std::vector<TrackingDataAdapter*>& MeasurementManager::GetAllTrackingDataAdapters()
{
   return adapters;
}


//-----------------------------------------------------------------------------
// const Integer GetMeasurementId(const std::string &modelName) const
//-----------------------------------------------------------------------------
/**
 * Retrieves the model ID (or IDs) for a named measurement model or a named tracking file set
 *
 * @param modelName The name of the measurement model or the tracking file set
 *
 * @return The ID (or IDs) for model or tracking file set
 */
//-----------------------------------------------------------------------------
const IntegerArray MeasurementManager::GetMeasurementId(
      const std::string &modelName) const
{
   IntegerArray arrayId;
   Integer foundId = -1;

   for (UnsignedInt i = 0; i < models.size(); ++i)
   {
      if (models[i]->GetName() == modelName)
      {
         foundId = models[i]->GetModelID();
         break;
      }
   }

   if (foundId != -1)
      arrayId.push_back(foundId);
   else
   {
      // Check modelName to be a name of trackingSet 
      UnsignedInt i;
      for (i = 0; i < trackingSets.size(); ++i)
      {
         if (trackingSets[i]->GetName() == modelName)
            break;
      }

      if (i < trackingSets.size())
      {
         // Get a name list of TrackingDataAdapter object associated the TrackingFileSet specified by modelName
         StringArray tfsAdapterList;
         if (adapterFromTFSMap.find(trackingSets[i]) != adapterFromTFSMap.end())
            tfsAdapterList = adapterFromTFSMap.find(trackingSets[i])->second;

         for (UnsignedInt k = 0; k < tfsAdapterList.size(); ++k)
         {
            // Find an adapter having the same name as tfsAdapterList[i]
            foundId = -1;
            for (UnsignedInt j = 0; j < adapters.size(); ++j)
            {
               if (adapters[j]->GetName() == tfsAdapterList[k])
               {
                  foundId = adapters[j]->GetModelID();
                  break;
               }
            }

            // Add founId to ID
            if (foundId != -1)
               arrayId.push_back(foundId);
         }
      }
      else
         arrayId.push_back(-1);
   }

   return arrayId;

   /// @todo: Handle multiple measurements inside of a TrackingFileSet

   //for (UnsignedInt i = 0; i < trackingSets.size(); ++i)
   //{
   //   if (trackingSets[i]->GetName() == modelName)
   //   {
   //      if (adapterFromTFSMap.find(trackingSets[i]) != adapterFromTFSMap.end())
   //      {
   //         /// @todo: Make measurementID accessor work for > 1 adapter in a TFS
   //         StringArray tfsAdapterList =
   //               adapterFromTFSMap.find(trackingSets[i])->second;

   //         std::string modelAdapterName = (tfsAdapterList.size() > 0 ?
   //               tfsAdapterList[0] : "Range");

   //         for (UnsignedInt j = 0; j < adapters.size(); ++j)
   //            if (adapters[j]->GetName() == modelAdapterName)
   //               foundId = adapters[j]->GetModelID();
   //      }
   //   }
   //}

   //return foundId;
}


//-----------------------------------------------------------------------------
// GmatBase* GetClone(GmatBase *obj)
//-----------------------------------------------------------------------------
/**
 * Finds a local clone of an object if one is available
 *
 * @param obj The object whose clone is needed
 *
 * @return A pointer to the clone, if one is available
 */
//-----------------------------------------------------------------------------
GmatBase* MeasurementManager::GetClone(GmatBase *obj)
{
   GmatBase *retval = NULL;
   // Look for an object named same as obj
   std::string objname = obj->GetName();

   if (objname != "")
   {
      // Check the models
      for (UnsignedInt i = 0; i < models.size(); ++i)
         if (models[i]->GetName() == objname)
         {
            retval = models[i];
            break;
         }

      // Check the tracking file sets
      for (UnsignedInt i = 0; i < trackingSets.size(); ++i)
         if (trackingSets[i]->GetName() == objname)
         {
            retval = trackingSets[i];
            break;
         }
   }

   return retval;
}


//--------------------------------------------------------------------------------------
// bool ValidateDouplicationOfGroundStationID()
//--------------------------------------------------------------------------------------
/**
* This function is used to verify 2 different ground stations having the same Id
*
* @return      true if each ground station has a unique ID, false otherwise 
*/
//--------------------------------------------------------------------------------------
bool MeasurementManager::ValidateDuplicationOfGroundStationID(std::string& errorMsg)
{
   std::map<std::string, std::string> gsNameIdMap;
   
   for (Integer i = 0; i < models.size(); ++i)
   {
      ObjectArray oa = models[i]->GetParticipants();

      for (Integer j = 0; j < oa.size(); ++j)
      {
         if (oa[j]->IsOfType(Gmat::GROUND_STATION))
         {
            if (gsNameIdMap.empty())
               gsNameIdMap[oa[j]->GetName()] = oa[j]->GetStringParameter("Id");
            else
            {
               for (std::map<std::string, std::string>::iterator objPtr = gsNameIdMap.begin(); objPtr != gsNameIdMap.end(); ++objPtr)
               {
                  if (objPtr->first == oa[j]->GetName())
                     continue;
                  else
                  {
                     if (objPtr->second != oa[j]->GetStringParameter("Id"))
                     {
                        // Add to gsNameIdMap
                        gsNameIdMap[oa[j]->GetName()] = oa[j]->GetStringParameter("Id");
                        break;
                     }
                     else
                     {
                        errorMsg = "Both ground stations " + objPtr->first + " and " + oa[j]->GetName() + " have the same Id '" + objPtr->second + "'";
                        return false;
                     }
                  }
               }
            }
         }
      }
   }

   for (Integer i = 0; i < adapters.size(); ++i)
   {
      std::vector<ObjectArray*> participantObjLists = adapters[i]->GetMeasurementModel()->GetParticipantObjectLists();
      ObjectArray* oa = participantObjLists[0];

      for (Integer j = 0; j < (*oa).size(); ++j)
      {
         if ((*oa)[j]->IsOfType(Gmat::GROUND_STATION))
         {
            if (gsNameIdMap.size() == 0)
               gsNameIdMap[(*oa)[j]->GetName()] = (*oa)[j]->GetStringParameter("Id");
            else
            {
               for (std::map<std::string, std::string>::iterator objPtr = gsNameIdMap.begin(); objPtr != gsNameIdMap.end(); ++objPtr)
               {
                  if (objPtr->first == (*oa)[j]->GetName())
                     continue;
                  else
                  {
                     if (objPtr->second != (*oa)[j]->GetStringParameter("Id"))
                     {
                        // Add to gsNameIdMap
                        gsNameIdMap[(*oa)[j]->GetName()] = (*oa)[j]->GetStringParameter("Id");
                        break;
                     }
                     else
                     {
                        errorMsg = "Both ground stations '" + objPtr->first + "' and '" + (*oa)[j]->GetName() + "' have the same Id '" + objPtr->second + "'";
                        return false;
                     }
                  }
               }
            }
         }
      }
   }

   gsNameIdMap.clear();

   return true;
}


//------------------------------------------------------------------------------
// bool CalculateMeasurements(bool forSimulation, bool withEvents, bool addNoise)   
//------------------------------------------------------------------------------
/**
 * Fires the calculation method of each owned MeasurementModel
 *
 * @param  forSimulation   true for simulation calculation and false for 
 *                          estimation calculation. Default value is false
 * @param  withEvents       flag to indicate calculation with ro without events
 * @param  addNoise         flag to indicate noise added to measurement
 *
 * @return True if at least one measurement is feasible and calculated; false
 *         otherwise
 */
//------------------------------------------------------------------------------
bool MeasurementManager::CalculateMeasurements(bool forSimulation, bool withEvents, bool addNoise)
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(" Entered bool MeasurementManager::CalculateMeasurements(simulation = %s, withEvents = %s, addNoise = %s)\n", (forSimulation?"true":"false"), (withEvents?"true":"false"), (addNoise?"true":"false"));
   #endif
   
   // Specify observation data for measurement
   ObservationData* od = NULL;
   if (!observations.empty())
      od = &(*currentObs);

   bool retval = false;
   eventCount = 0;

   if (forSimulation)
   {  // This section is used for simulation only:
      for (UnsignedInt j = 0; j < models.size(); ++j)
      {
         #ifdef DEBUG_CALCULATE_MEASUREMENTS
           MessageInterface::ShowMessage(" Measurement models[%d] name = '%s'    measurement type = '%s'\n", j, models[j]->GetName().c_str(), models[j]->GetStringParameter("Type").c_str());
         #endif

///// TBD: Do we want something more generic here?
         // Specify ramp table associated with measurement model models[j]:
         // Note: Only one ramp table is used for a measurement model
         StringArray sr = models[j]->GetStringArrayParameter("RampTables");
         std::vector<RampTableData>* rt = NULL;
         if (sr.size() > 0)
            rt = &(rampTables[sr[0]]);
       
         if (withEvents)
         {
#ifdef DEBUG_CALCULATE_MEASUREMENTS
            MessageInterface::ShowMessage(" Simulation: measurement with events\n");
#endif
            measurements[j] = models[j]->CalculateMeasurement(withEvents, od, rt, addNoise);
         }
         else
         {
#ifdef DEBUG_CALCULATE_MEASUREMENTS
            MessageInterface::ShowMessage(" Simulation: measurement without events\n");
#endif
            // when running without event, no noise is added due to check feasibility
            measurements[j] = models[j]->CalculateMeasurement(withEvents, od, rt);
         }

         // Throw an error message if ramp table is missed when it needs
         if (measurements[j].unfeasibleReason == "R")
         {
               Real a1Time = measurements[j].epoch;
               Real taiTime;
               std::string tais;
               TimeConverterUtil::Convert("A1ModJulian", a1Time, "", "TAIModJulian", taiTime, tais); 
               char s[1000];
               sprintf(&s[0], "Error: In simulation for measurement model %s, epoch %.12lf TAIMdj is out of ramp table.\n Please make sure ramped table cover all simulation epochs.\n", models[j]->GetName().c_str(), taiTime);
               throw MeasurementException(s); 
         }

         if (measurements[j].isFeasible)
         {
            if (!withEvents)
               eventCount += measurements[j].eventCount;
            retval = true;
         }

         #ifdef DEBUG_CALCULATE_MEASUREMENTS
            MessageInterface::ShowMessage(" Measurement is %s. Its value is %lf\n", (measurements[j].isFeasible?"feasible":" not feasible"), measurements[j].value[0]);
         #endif

      } // for j loop

      // Now do the same thing for the TrackingDataAdapters
      #ifdef DEBUG_CALCULATE_MEASUREMENTS
      MessageInterface::ShowMessage("adapters size = %d\n", adapters.size());
      #endif
      for (UnsignedInt i = 0; i < adapters.size(); ++i)
      {
         // Specify ramp table associated with measurement adapters[i]:
         // Note: Only one ramp table is used for a measurement model
         StringArray sr = adapters[i]->GetStringArrayParameter("RampTables");
         std::vector<RampTableData>* rt = NULL;
         if (sr.size() > 0)
            rt = &(rampTables[sr[0]]);

         
         // Set AddNoise to measuement apdater
         adapters[i]->SetBooleanParameter("AddNoise", addNoise);


         // Run CalculateMeasurement() function          
         //if (withEvents)
         //{  
         //   #ifdef DEBUG_CALCULATE_MEASUREMENTS
         //      MessageInterface::ShowMessage(" Simulation: measurement adapter %s with events\n", adapters[i]->GetName().c_str());
         //   #endif
         //   measurements[i] = adapters[i]->CalculateMeasurement(withEvents, od, rt);
         //   if (measurements[i].unfeasibleReason == "R")
         //      throw MeasurementException(adapters[i]->GetErrorMessage());
         //}
         //else
         //{
         //   #ifdef DEBUG_CALCULATE_MEASUREMENTS
         //      MessageInterface::ShowMessage(" Simulation: measurement adapter %s without events\n", adapters[i]->GetName().c_str());
         //   #endif
         //   measurements[i] = adapters[i]->CalculateMeasurement(withEvents, od, rt);
         //   if (measurements[i].unfeasibleReason == "R")
         //      throw MeasurementException(adapters[i]->GetErrorMessage());
         //}

         #ifdef DEBUG_CALCULATE_MEASUREMENTS
            if (withEvents)
               MessageInterface::ShowMessage(" Simulation: measurement adapter %s with events\n", adapters[i]->GetName().c_str());
            else
               MessageInterface::ShowMessage(" Simulation: measurement adapter %s without events\n", adapters[i]->GetName().c_str());
         #endif
         measurements[i] = adapters[i]->CalculateMeasurement(withEvents, od, rt);
         if (measurements[i].unfeasibleReason == "R")
            throw MeasurementException(adapters[i]->GetErrorMessage());

         if (measurements[i].isFeasible)
         {
            if (!withEvents)
               eventCount += measurements[i].eventCount;
            retval = true;
         }
         
         #ifdef DEBUG_CALCULATE_MEASUREMENTS
            MessageInterface::ShowMessage(" Adapter Measurement is %s. Its value is "
                  "%lf\n", (measurements[i].isFeasible?"feasible":
                  " not feasible"), measurements[i].value[0]);
         #endif
      } // for i loop
   }
   else
   {  // This section is used for estimation only:
      if (od == NULL)
      {
         #ifdef DEBUG_CALCULATE_MEASUREMENTS
            MessageInterface::ShowMessage("    observation data is NULL\n");
         #endif
         return retval;               // no calculation measurement value whenever no observation data is available
      }
      #ifdef DEBUG_CALCULATE_MEASUREMENTS
      else
         MessageInterface::ShowMessage("    %s observation data: %.12lf  %s  %d  %s  %s   %.12lf\n", od->dataFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(),od->value[0]);
      #endif

      #ifdef DEBUG_CALCULATE_MEASUREMENTS
         MessageInterface::ShowMessage(" processing for measurement models ...\n");
      #endif
      for (UnsignedInt j = 0; j < models.size(); ++j)
      {
         // Verify observation data belonging to the measurement model jth
         bool isbelong;
         if ((models[j]->GetStringParameter("Type") != od->typeName))
         {
            isbelong = false;
         }
         else
         {
            isbelong = false;
            ObjectArray participants = models[j]->GetParticipants();      // participants in measurement model
            if (participants.size() == od->participantIDs.size())
            {
               int num = participants.size();
               for (int i1 = 0; i1 < num; ++i1)
               {
                  isbelong = false;
                  for (int i2 = 0; i2 < num; ++i2)
                  {
                     if (od->participantIDs[i1] == participants[i2]->GetStringParameter("Id"))
                     {
                        isbelong = true;
                        break;
                     }
                  } // for i2 loop

                  if (isbelong == false)  
                     break;              
               } // for i1 loop
            }
         }
       
         if (isbelong == false)
         {
//          MessageInterface::ShowMessage("This observation data is not belong to model[%d]<%s>: %lf  %s  %s  %s  %lf\n",j, models[j]->GetStringParameter("Type").c_str(), od->epoch, od->typeName.c_str(), od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
            measurements[j].typeName       = models[j]->GetStringParameter("Type");
            measurements[j].epoch          = od->epoch;
            measurements[j].epochSystem        = od->epochSystem;
            measurements[j].isFeasible       = false;
            measurements[j].covariance       = NULL;
            measurements[j].eventCount       = 0;
            measurements[j].feasibilityValue = 0.0;
            measurements[j].unfeasibleReason = "U";
            measurements[j].value.clear();  
         }
         else
         {
///// TBD: Do we want something more generic here?
            // Specify ramp table associated with measurement model models[j]:
            // Note: Only one ramp table is used for a measurement model
            StringArray sr = models[j]->GetStringArrayParameter("RampTables");
            std::vector<RampTableData>* rt = NULL;
            if (sr.size() > 0)
               rt = &(rampTables[sr[0]]);
          
            measurements[j] = models[j]->CalculateMeasurement(withEvents, od, rt);
        
            if (measurements[j].isFeasible)
            {
               if (!withEvents)
                  eventCount += measurements[j].eventCount;
               retval = true;
            }
         }
      }// for j loop

      #ifdef DEBUG_CALCULATE_MEASUREMENTS
         MessageInterface::ShowMessage(" processing for tracking data adapters ...\n");
         MessageInterface::ShowMessage("adapters.size() = %d:\n", adapters.size());
      #endif
      // Now do the tracking data adapters
      for (UnsignedInt j = 0; j < adapters.size(); ++j)
      {
         // Code to verify observation data belonging to the measurement model jth
         bool isbelong = false;
         if ((adapters[j]->GetStringParameter("MeasurementType") == od->typeName))
         {
            std::vector<ObjectArray*> participantObjLists = adapters[j]->GetMeasurementModel()->GetParticipantObjectLists();
            UnsignedInt num = participantObjLists[0]->size();                                       // participants in measurement model
            if (num == od->participantIDs.size())
            {
               isbelong = true;
               for (UnsignedInt i1 = 0; i1 < num; ++i1)
               {
                  if (od->participantIDs[i1] == participantObjLists[0]->at(i1)->GetStringParameter("Id"))
                  {
                     isbelong = false;
                     break;
                  }
               }
            }
         }

         if (isbelong == false)
         {
            // MessageInterface::ShowMessage("This observation data is not belong to model[%d]<%s>: %lf  %s  %s  %s  %lf\n",j, adapters[j]->GetStringParameter("MeasurementType").c_str(), od->epoch, od->typeName.c_str(), od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
            measurements[j].typeName         = adapters[j]->GetStringParameter("MeasurementType");
            measurements[j].epoch            = od->epoch;
            measurements[j].epochSystem      = od->epochSystem;
            measurements[j].isFeasible       = false;
            measurements[j].covariance       = NULL;
            measurements[j].eventCount       = 0;
            measurements[j].feasibilityValue = 0.0;
            measurements[j].unfeasibleReason = "U";
            measurements[j].value.clear();  
         }
         else
         {
///// TBD: Do we want something more generic here?
            // Specify ramp table associated with tracking data adapter adapters[j]:
            // Note: Only one ramp table is used for a tracking data adapter
            StringArray sr = adapters[j]->GetStringArrayParameter("RampTables");
            std::vector<RampTableData>* rt = NULL;
            if (sr.size() > 0)
               rt = &(rampTables[sr[0]]);
            measurements[j] = adapters[j]->CalculateMeasurement(withEvents, od, rt);
            
            if (measurements[j].isFeasible)
            {
               if (!withEvents)
                  eventCount += measurements[j].eventCount;
               retval = true;
            }

            #ifdef DEBUG_ADAPTERS
               MessageInterface::ShowMessage("   Measurement %d computed; first "
                  "value: %lf\n", j, measurements[j].value[0]);
            #endif
         }
      }// for j loop
   }

   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(" Returning %s from bool MeasurementManager"
            "::CalculateMeasurements(%s,%s)\n", (retval ? "true" : "false"),
            (forSimulation?"true":"false"), (withEvents?"true":"false"));
   #endif

   return retval;
}


//-----------------------------------------------------------------------------
// bool MeasurementHasEvents()
//-----------------------------------------------------------------------------
/**
 * Check to see if any registered measurements have events
 *
 * @return true if any of the measurements have events
 */
//-----------------------------------------------------------------------------
bool MeasurementManager::MeasurementHasEvents()
{
   bool retval = false;

   for (UnsignedInt i = 0; i < measurements.size(); ++i)
      if (measurements[i].eventCount > 0)
         retval = true;

   return retval;
}


//-----------------------------------------------------------------------------
// ObjectArray& GetActiveEvents()
//-----------------------------------------------------------------------------
/**
 * Retrieves a vector of all events associated with currently feasible
 * measurements in the MeasurementManager
 *
 * @return The vector of events for the feasible measurements
 */
//-----------------------------------------------------------------------------
ObjectArray& MeasurementManager::GetActiveEvents()
{
#ifdef DEBUG_GET_ACTIVE_EVENTS
   MessageInterface::ShowMessage("MeasurementManager::GetActiveEvents()   entered: \n");
#endif

   activeEvents.clear();
   eventMap.clear();

   for (UnsignedInt j = 0; j < models.size(); ++j)
   {
#ifdef DEBUG_GET_ACTIVE_EVENTS
      MessageInterface::ShowMessage("    measurement model[%d] <%s> is %s.\n", j, models[j]->GetName().c_str(), (measurements[j].isFeasible?"feasible":"not feasible"));
#endif
      if (measurements[j].isFeasible)
      {
         Integer count = models[j]->GetEventCount();
         for (Integer i = 0; i < count; ++i)
         {
            Event *currentEvent = models[j]->GetEvent(i);
            eventMap[currentEvent] = models[j];
            activeEvents.push_back(currentEvent);
#ifdef DEBUG_GET_ACTIVE_EVENTS
     MessageInterface::ShowMessage("    measurement model[%d] <%s> event %s.\n", j, models[j]->GetName().c_str(), currentEvent->GetName().c_str());
#endif
         }
      }
   }

#ifdef DEBUG_GET_ACTIVE_EVENTS
   MessageInterface::ShowMessage("MeasurementManager::GetActiveEvents()   exit: \n");
#endif

   return activeEvents;
}


//-----------------------------------------------------------------------------
// bool ProcessEvent(Event *locatedEvent)
//-----------------------------------------------------------------------------
/**
 * Passes an event to the owning measurement so that measurement can process the
 * result from that event.
 *
 * This method is called after the RootFinder has located an event, so that the
 * measurement that depends on that event can retrieve the result of the event
 * location process for use in the measurement evaluation process.
 *
 * @param locatedEvent Pointer to the event that is to be processed
 *
 * @return true if the event was passed to a measurement model, false if not
 */
//-----------------------------------------------------------------------------
bool MeasurementManager::ProcessEvent(Event *locatedEvent)
{
   bool retval = false;

   if (eventMap.find(locatedEvent) != eventMap.end())
   {
      MeasurementModel *model = eventMap[locatedEvent];
      model->SetEventData(locatedEvent);
      retval = true;
   }

   return retval;
}

//------------------------------------------------------------------------------
// const std::vector<RealArray>& MeasurementManager::CalculateDerivatives(
//                         GmatBase *obj, Integer wrt, Integer forMeasurement)
//------------------------------------------------------------------------------
/**
 * Fires the calculation method of each owned MeasurementModel, and on success
 * (i.e. if the measurement was feasible) then fires the derivative calculator.
 *
 * @param obj The object holding the with-respect-to parameters
 * @param wrt The with-respect-to parameters
 * @param forMeasurement Indix of the measurement that is being differentiated
 *
 * @return The derivative data
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& MeasurementManager::CalculateDerivatives(
                           GmatBase *obj, Integer wrt, Integer forMeasurement)
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered MeasurementManager::"
            "CalculateDerivatives(%s <%p>, %d, %d)\n", obj->GetName().c_str(),
            obj, wrt, forMeasurement);
   #endif

   if ((Integer)models.size() > forMeasurement)
      return models[forMeasurement]->CalculateMeasurementDerivatives(obj, wrt);
   else
      return adapters[forMeasurement]->CalculateMeasurementDerivatives(obj,wrt);
}


//------------------------------------------------------------------------------
// bool MeasurementManager::WriteMeasurements()
//------------------------------------------------------------------------------
/**
 * Writes the calculated data to a file.
 *
 * This method is used during measurement simulation to generate simulated data.
 *
 * @return  true when at least one measurement successed for writing, 
 *          false when all measurement are unfeasible
 */
//------------------------------------------------------------------------------
bool MeasurementManager::WriteMeasurements()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("Entered MeasurementManager::"
            "WriteMeasurements()\n");
   #endif

   bool retval = false;

   for (UnsignedInt i = 0; i < measurements.size(); ++i)
   {
      if (measurements[i].isFeasible)
      {
         #ifdef DEBUG_FILE_WRITE
            MessageInterface::ShowMessage("Feasible\n   id = %d\n",
                  measurements[i].uniqueID);
            MessageInterface::ShowMessage("   Stream at <%p>\n",
                  idToStreamMap[measurements[i].uniqueID]);
         #endif
         if (idToStreamMap[measurements[i].uniqueID])
         {
            idToStreamMap[measurements[i].uniqueID]->
               WriteMeasurement(&(measurements[i]));
         }
         else
            throw MeasurementException("Error: No data file is defined in TrackingFileSet or MeasurementModel\n");

         retval = true;
      }
   }

   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("Exit MeasurementManager::"
            "WriteMeasurements()\n");
   #endif

   return retval;
}


//-----------------------------------------------------------------------------
// const StringArray& GetStreamList()
//-----------------------------------------------------------------------------
/**
 * Retrieves a list of the observation data streams available to this
 * MeasurementManager.
 *
 * The current implementation queries the measurement models for the list of
 * streams in those models.  That dependency may change with later builds, based
 * on changes in the scripting to the location of the stream identifiers.
 *
 * @return The list of streams
 */
//-----------------------------------------------------------------------------
const StringArray& MeasurementManager::GetStreamList()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("MeasurementManager::GetStreamList() "
            "Entered\n   %d models registered\n", models.size());
   #endif

   // Run through the measurements and build the list
   streamNames.clear();
   for (UnsignedInt i = 0; i < models.size(); ++i)
   {
      StringArray names = models[i]->GetStringArrayParameter("ObservationData");
      for (UnsignedInt j = 0; j < names.size(); ++j)
      {
         if(find(streamNames.begin(), streamNames.end(), names[j]) ==
               streamNames.end())
            streamNames.push_back(names[j]);
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("MeasurementManager::GetStreamList() Exit \n");
   #endif

   return streamNames;
}


///// TBD: Do we want something more generic here?
//-----------------------------------------------------------------------------
// const StringArray& GetRampTableDataStreamList()
//-----------------------------------------------------------------------------
/**
 * Retrieves a list of the ramp table data streams available to this
 * MeasurementManager.
 *
 * The current implementation queries the measurement models for the list of
 * streams in those models.  That dependency may change with later builds, based
 * on changes in the scripting to the location of the stream identifiers.
 *
 * @return The list of ramp table data streams
 */
//-----------------------------------------------------------------------------
const StringArray& MeasurementManager::GetRampTableDataStreamList()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("MeasurementManager::GetRampTableDataStreamList() "
            "Entered\n   %d models registered\n", models.size());
   #endif

   // Run through the measurements and build the list
   rampTableDataStreamNames.clear();
   for (UnsignedInt i = 0; i < models.size(); ++i)
   {
      StringArray names = models[i]->GetStringArrayParameter("RampTables");
      for (UnsignedInt j = 0; j < names.size(); ++j)
      {
         if(find(rampTableDataStreamNames.begin(), rampTableDataStreamNames.end(), names[j]) ==
               rampTableDataStreamNames.end())
            rampTableDataStreamNames.push_back(names[j]);
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("MeasurementManager::GetRampTableDataStreamList() Exit\n");
   #endif

   return rampTableDataStreamNames;
}


//-----------------------------------------------------------------------------
// void SetStreamObject(DataFile *newStream)
//-----------------------------------------------------------------------------
/**
 * Passes an observation stream into the MeasurementManager.
 *
 * This method passes an observation stream to the MeasurementManager.  The
 * MeasurementManager, in turn, passes the stream pointer to each
 * MeasurementModel that uses it.
 *
 * This method is idempotent.  Multiple calls with the same observation stream
 * have the same result as passing in the stream once.
 *
 * @param newStream The stream that is being set
 */
//-----------------------------------------------------------------------------
void MeasurementManager::SetStreamObject(DataFile *newStream)
{
   if (find(streamList.begin(), streamList.end(), newStream) ==
         streamList.end())
   {
      streamList.push_back(newStream);
      std::string streamName = newStream->GetName();

      // Walk through the models and set each model's index that refs the new
      // stream to point to it.
      for (UnsignedInt i = 0; i < models.size(); ++i)
      {
         StringArray od = models[i]->GetStringArrayParameter("ObservationData");
         for (UnsignedInt j = 0; j < od.size(); ++j)
         {
            // todo: Each model feeds only one stream with this code
            if (streamName == od[j])
            {
               idToStreamMap[models[i]->GetModelID()] = newStream;
               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("Stream id %d -> %s\n",
                        models[i]->GetModelID(), newStream->GetName().c_str());
               #endif
            }
         }
      }
   }
}


///// TBD: Do we want something more generic here?
//-----------------------------------------------------------------------------
// void SetRampTableDataStreamObject(DataFile *newStream)
//-----------------------------------------------------------------------------
/**
 * Passes a ramp table data stream into the MeasurementManager.
 *
 * This method passes a ramp table data stream to the MeasurementManager.  The
 * MeasurementManager, in turn, passes the stream pointer to each
 * MeasurementModel that uses it.
 *
 * This method is idempotent.  Multiple calls with the same ramp table data stream
 * have the same result as passing in the stream once.
 *
 * @param newStream The stream that is being set
 */
//-----------------------------------------------------------------------------
void MeasurementManager::SetRampTableDataStreamObject(DataFile *newStream)
{
   if (find(rampTableDataStreamList.begin(), rampTableDataStreamList.end(), newStream) ==
         rampTableDataStreamList.end())
   {
      rampTableDataStreamList.push_back(newStream);
      std::string streamName = newStream->GetName();

      // Walk through the models and set each model's index that refs the new
      // stream to point to it.
      for (UnsignedInt i = 0; i < models.size(); ++i)
      {
         StringArray rtNameList = models[i]->GetStringArrayParameter("RampTables");
         for (UnsignedInt j = 0; j < rtNameList.size(); ++j)
         {
            // todo: Each model feeds only one stream with this code
            if (streamName == rtNameList[j])
            {
               idToRampTableStreamMap[models[i]->GetModelID()] = newStream;
               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("Ramp table stream id %d -> %s\n",
                        models[i]->GetModelID(), newStream->GetName().c_str());
               #endif
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
// IntegerArray& GetValidMeasurementList()
//-----------------------------------------------------------------------------
/**
 * Retrieves the indices of the current active measurement models
 *
 * @return A vector of indices for the active measurements
 */
//-----------------------------------------------------------------------------
IntegerArray& MeasurementManager::GetValidMeasurementList()
{
   activeMeasurements.clear();

   FindModelForObservation();

   #ifdef DEBUG_MODEL_MAPPING
      MessageInterface::ShowMessage("Found %d potential models for the current "
            "observation\n", activeMeasurements.size());
   #endif

   return activeMeasurements;
}


//-----------------------------------------------------------------------------
// Integer FindModelForObservation()
//-----------------------------------------------------------------------------
/**
 * Finds the MeasurementModel associated with the current observation
 *
 * @return The index for the measurement model
 */
//-----------------------------------------------------------------------------
Integer MeasurementManager::FindModelForObservation()
{
   #ifdef DEBUG_MODEL_MAPPING
       MessageInterface::ShowMessage("Entered MeasurementManager::"
             "FindModelForObservation()\n");
   #endif
   Integer retval = 0;

   Integer type =  currentObs->type;

   #ifdef DEBUG_MODEL_MAPPING
       MessageInterface::ShowMessage("   Current observation type: %d\n", type);
   #endif

   if (type < 9000)
   {
   for (UnsignedInt i = 0; i < models.size(); ++i)
   {
      MeasurementData theMeas = models[i]->GetMeasurement();

      #ifdef DEBUG_MODEL_MAPPING
          MessageInterface::ShowMessage("   Current model type: %d\n",
                theMeas.type);
      #endif

      if (theMeas.type == type)
      {
         #ifdef DEBUG_MODEL_MAPPING
             MessageInterface::ShowMessage("   Found a model of this type; "
                   "checking participants\n");
         #endif
         StringArray parts = currentObs->participantIDs;
         StringArray measParts = theMeas.participantIDs;
         bool missingParticipant = false;
         for (UnsignedInt j = 0; j < parts.size(); ++j)
            if (find(measParts.begin(), measParts.end(), parts[j]) ==
                  measParts.end())
               missingParticipant = true;

         if (!missingParticipant)
         {
            activeMeasurements.push_back(i);
            ++retval;
         }
      }
   }
   }
   else
   {
      for (UnsignedInt i = 0; i < adapters.size(); ++i)
      {
         MeasurementData theMeas = adapters[i]->GetMeasurement();

         #ifdef DEBUG_MODEL_MAPPING
             MessageInterface::ShowMessage("   Current model type: %d\n",
                   theMeas.type);
         #endif

         if (theMeas.type == type)
         {
            #ifdef DEBUG_MODEL_MAPPING
                MessageInterface::ShowMessage("   Found a model of this type; "
                      "checking participants\n");
            #endif
            StringArray parts = currentObs->participantIDs;
            StringArray measParts = theMeas.participantIDs;
            bool missingParticipant = false;
            for (UnsignedInt j = 0; j < parts.size(); ++j)
               if (find(measParts.begin(), measParts.end(), parts[j]) ==
                     measParts.end())
                  missingParticipant = true;

            if (!missingParticipant)
            {
               activeMeasurements.push_back(i);
               ++retval;
            }
         }
      }
   }

   #ifdef DEBUG_MODEL_MAPPING
       MessageInterface::ShowMessage("Exit MeasurementManager::"
             "FindModelForObservation()\n");
   #endif

   return retval;
}


//-----------------------------------------------------------------------------
// void Reset()
//-----------------------------------------------------------------------------
/**
 * Resets the data pointers in the MeasurementManager.
 *
 * This method is used to set the MeasurementManager to the start of the
 * observation data so that an Estimator can start processing with the first
 * observation available to the MeasurementManager.
 */
//-----------------------------------------------------------------------------
void  MeasurementManager::Reset()
{
   currentObs = observations.begin();
}


//------------------------------------------------------------------------------
// Integer AddMeasurement(TrackingFileSet* tfs)
//------------------------------------------------------------------------------
/**
 * Adds a tracking file set to the manager
 *
 * @param tfs The TrackingFileSet object that is managed
 *
 * @return TBD
 */
//------------------------------------------------------------------------------
Integer MeasurementManager::AddMeasurement(TrackingFileSet* tfs)
{
   Integer retval = -1;

   if (find(trackingSets.begin(),trackingSets.end(), tfs) == trackingSets.end())
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Adding %s TrackingFileSet %s <%p>\n",
               (tfs->IsInitialized() ? "initialized" : "not initialized"),
               tfs->GetName().c_str(), tfs);
      #endif
      trackingSets.push_back(tfs);

//      // Now add the adapters
//      std::vector<TrackingDataAdapter*> *setAdapters = tfs->GetAdapters();
//
//      for (UnsignedInt i = 0; i < setAdapters->size(); ++i)
//         AddMeasurement((*setAdapters)[i]);
//
//      // And the stream objects
//      StringArray filenames = tfs->GetStringArrayParameter("FileName");
//      for (UnsignedInt i = 0; i < filenames.size(); ++i)
//      {
//         std::stringstream fn;
//         fn << tfs->GetName() << "DataFile" << i;
//         DataFile *newStream = new DataFile(fn.str());
//         newStream->SetStringParameter("Filename", filenames[i]);
//         GmatObType *got = new GmatObType();
//         newStream->SetStream(got);
//         newStream->Initialize();
//
//         #ifdef DEBUG_INITIALIZATION
//            MessageInterface::ShowMessage("   Adding %s DataFile %s <%p>\n",
//                  (newStream->IsInitialized() ? "initialized" :
//                  "not initialized"), newStream->GetName().c_str(), newStream);
//         #endif
//
//         SetStreamObject(newStream);
//
//         // Associate the adapters with the stream
//         for (UnsignedInt j = 0; j < setAdapters->size(); ++j)
//         {
//            #ifdef DEBUG_INITIALIZATION
//               MessageInterface::ShowMessage("Associating %d with %s\n",
//                     (*setAdapters)[j]->GetModelID(),
//                     newStream->GetName().c_str());
//            #endif
//            idToStreamMap[(*setAdapters)[j]->GetModelID()] = newStream;
//         }
//      }
//      // Set retval?
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer AddMeasurement(TrackingDataAdapter* adapter)
//------------------------------------------------------------------------------
/**
 * Adds a TrackingDataAdapter to the measurement manager
 *
 * @param adapter The adapter
 *
 * @return TBD
 */
//------------------------------------------------------------------------------
Integer MeasurementManager::AddMeasurement(TrackingDataAdapter* adapter)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("   Adding %s TrackingDataAdapter %s "
            "<%p>\n", (adapter->IsInitialized() ? "initialized" :
            "not initialized"), adapter->GetName().c_str(), adapter);
   #endif
   adapter->SetModelID(largestId++);
   if (thePropagator)
      adapter->SetPropagator(thePropagator);
   adapters.push_back(adapter);
   return -1;
}


//------------------------------------------------------------------------------
// const std::vector<MeasurementModel*>& GetAllMeasurementModels()
//------------------------------------------------------------------------------
/**
* Get a list of all MeasurementModel objects
*/
//------------------------------------------------------------------------------
const std::vector<MeasurementModel*>& MeasurementManager::GetAllMeasurementModels()
{
   return models;
}


//------------------------------------------------------------------------------
// const std::vector<TrackingSystem*>& GetAllTrackingSystems()
//------------------------------------------------------------------------------
/**
* Get a list of all TrackingSystem objects
*/
//------------------------------------------------------------------------------
const std::vector<TrackingSystem*>& MeasurementManager::GetAllTrackingSystems()
{
   return systems;
}


//------------------------------------------------------------------------------
// const std::vector<TrackingFileSet*>& GetAllTrackingFileSets()
//------------------------------------------------------------------------------
/**
* Get a list of all TrackingFileSet objects
*/
//------------------------------------------------------------------------------
const std::vector<TrackingFileSet*>& MeasurementManager::GetAllTrackingFileSets()
{
   return trackingSets;
}


//------------------------------------------------------------------------------
// UnsignedInt GetCurrentRecordNumber()
//------------------------------------------------------------------------------
/**
* Get the current observation record number
*/
//------------------------------------------------------------------------------
UnsignedInt MeasurementManager::GetCurrentRecordNumber()
{
   UnsignedInt i = 0;
   for (std::vector<ObservationData>::iterator j = observations.begin(); j != observations.end(); ++j)
   {
      if (j == currentObs)
       break;
     ++i;
   }

   return i;
}
