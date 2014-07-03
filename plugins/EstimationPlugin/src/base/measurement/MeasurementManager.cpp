//$Id: MeasurementManager.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         MeasurementManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include <sstream>            // To build DataStream for a TrackingFileSet

#include "DataFileAdapter.hpp"
#include "PropSetup.hpp"

// Temporary to get Adapters hooked up
#include "GmatObType.hpp"


//#define DEBUG_INITIALIZATION
//#define DEBUG_FILE_WRITE
//#define DEBUG_FLOW
//#define DEBUG_CALCULATE_MEASUREMENTS
//#define DEBUG_GET_ACTIVE_EVENTS
//#define DEBUG_LOAD_OBSERVATIONS
//#define DEBUG_LOAD_FREQUENCY_RAMP_TABLE
//#define DEBUG_MODEL_MAPPING
//#define DEBUG_CALCULATE
//#define DEBUG_ADVANCE_OBSERVATION							// made changes by TUAN NGUYEN
//#define DEBUG_EXECUTION
//#define DEBUG_ADAPTERS

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

   for (std::vector<MeasurementModel*>::const_iterator i = mm.models.begin();
         i != mm.models.end(); ++i)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Cloning %s MeasurementModel\n",
               (*i)->GetStringParameter("Type").c_str());
      #endif
      models.push_back((MeasurementModel*)((*i)->Clone()));
      MeasurementData md;
      measurements.push_back(md);
   }

   for (std::vector<TrackingFileSet*>::const_iterator i = mm.trackingSets.begin();
         i != mm.trackingSets.end(); ++i)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Cloning %s TrackingDataSet\n",
               (*i)->GetName().c_str());
      #endif
      trackingSets.push_back((TrackingFileSet*)((*i)->Clone()));
      MeasurementData md;
      measurements.push_back(md);
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
   if (&mm != this)
   {
      thePropagator     = mm.thePropagator;
      anchorEpoch  = mm.anchorEpoch;
      currentEpoch = mm.currentEpoch;
      modelNames   = mm.modelNames;
      eventCount   = mm.eventCount;
      inSimulationMode  = mm.inSimulationMode;
      // Clone the measurements and tracking file sets
      for (std::vector<MeasurementModel*>::iterator i = models.begin();
            i != models.end(); ++i)
         delete (*i);

      models.clear();
      measurements.clear();

      for (std::vector<MeasurementModel*>::const_iterator i = mm.models.begin();
            i != mm.models.end(); ++i)
         models.push_back((MeasurementModel*)(*i)->Clone());

      for (std::vector<TrackingFileSet*>::iterator i = trackingSets.begin();
            i != trackingSets.end(); ++i)
         delete (*i);

      trackingSets.clear();
      for (std::vector<TrackingFileSet*>::const_iterator i = mm.trackingSets.begin();
            i != mm.trackingSets.end(); ++i)
         trackingSets.push_back((TrackingFileSet*)((*i)->Clone()));

      // Should measurements be rebuilt here?
   }

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

   #ifdef DEBUG_INITIALIZATION
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
      if (trackingSets[i]->Initialize() == false)
         return false;

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

      // And the stream objects
      StringArray filenames = trackingSets[i]->GetStringArrayParameter("Filename");
      for (UnsignedInt i = 0; i < filenames.size(); ++i)
      {
         std::stringstream fn;
         fn << trackingSets[i]->GetName() << "DataFile" << i;
         DataFile *newStream = new DataFile(fn.str());
         newStream->SetStringParameter("Filename", filenames[i]);
         GmatObType *got = new GmatObType();
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
         }

         if (inSimulationMode)
         {
            if (newStream->OpenStream(inSimulationMode) == false)
               throw MeasurementException("The stream " + filenames[i] +
                     " failed to open in simulation mode");
         }
      }
   }

   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      if (streamList[i]->IsInitialized() == false)						// made changes by TUAN NGUYEN
		 streamList[i]->Initialize();									// made changes by TUAN NGUYEN
   }

///// TBD: Do we want something more generic here?
   for (UnsignedInt i = 0; i < rampTableDataStreamList.size(); ++i)		// made changes by TUAN NGUYEN
   {																	// made changes by TUAN NGUYEN
      if (rampTableDataStreamList[i]->IsInitialized() == false)			// made changes by TUAN NGUYEN
		 rampTableDataStreamList[i]->Initialize();						// made changes by TUAN NGUYEN
   }																	// made changes by TUAN NGUYEN

#ifdef DEBUG_FLOW
   MessageInterface::ShowMessage(
         "Exit MeasurementManager::Initialize() method\n");
#endif

   return retval;
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
//	  MessageInterface::ShowMessage("* streamList[i = %d] Filename = '%s'   Format = '%s'\n", i, fname.c_str(), dataformat.c_str());
	  for(UnsignedInt j = 0; j < streamList.size(); ++j)
      {
		 if (i == j)
		    continue;
//		 MessageInterface::ShowMessage("   + streamList[j = %d] Filename = '%s'   Format = '%s'\n", j, streamList[j]->GetStringParameter("Filename").c_str(), streamList[j]->GetStringParameter("Format").c_str());
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
   for (UnsignedInt i = 0; i < rampTableDataStreamList.size(); ++i)							// made changes by TUAN NGUYEN
   {																						// made changes by TUAN NGUYEN
      #ifdef USE_DATAFILE_PLUGINS															// made changes by TUAN NGUYEN
         std::string writeMode = (simulating ? "w" : "r");									// made changes by TUAN NGUYEN
         rampTableDataStreamList[i]->SetStringParameter(									// made changes by TUAN NGUYEN
               rampTableDataStreamList[i]->GetParameterID("ReadWriteMode"), writeMode);		// made changes by TUAN NGUYEN

         if (rampTableDataStreamList[i]->OpenFile() == false)								// made changes by TUAN NGUYEN
            retval = false;																	// made changes by TUAN NGUYEN
      #else																					// made changes by TUAN NGUYEN
         if (rampTableDataStreamList[i]->OpenStream(simulating) == false)					// made changes by TUAN NGUYEN
            retval = false;																	// made changes by TUAN NGUYEN
      #endif																				// made changes by TUAN NGUYEN
   }																						// made changes by TUAN NGUYEN

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
   for (UnsignedInt i = 0; i < rampTableDataStreamList.size(); ++i)		// made changes by TUAN NGUYEN
   {																	// made changes by TUAN NGUYEN
      #ifdef USE_DATAFILE_PLUGINS										// made changes by TUAN NGUYEN
         if (rampTableDataStreamList[i]->CloseFile() == false)			// made changes by TUAN NGUYEN
            retval = false;												// made changes by TUAN NGUYEN
      #else																// made changes by TUAN NGUYEN
         if (rampTableDataStreamList[i]->CloseStream() == false)		// made changes by TUAN NGUYEN
            retval = false;												// made changes by TUAN NGUYEN
      #endif															// made changes by TUAN NGUYEN
   }																	// made changes by TUAN NGUYEN

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
         // Specify ramp table associated with measurement model models[j]:						// made changes by TUAN NGUYEN
         // Note: Only one ramp table is used for a measurement model							// made changes by TUAN NGUYEN
///// TBD: Do we want something more generic here?
		 StringArray sr = models[j]->GetStringArrayParameter("RampTables");						// made changes by TUAN NGUYEN
		 std::vector<RampTableData>* rt = NULL;													// made changes by TUAN NGUYEN
		 if (sr.size() > 0)																		// made changes by TUAN NGUYEN
		    rt = &(rampTables[sr[0]]);															// made changes by TUAN NGUYEN

		 // Specify current observation data. If no observation data used, it passes a NULL pointer. 
		 ObservationData* od = NULL;															// made changes by TUAN NGUYEN
		 if (!observations.empty())																// made changes by TUAN NGUYEN
            od = &(*currentObs);																// made changes by TUAN NGUYEN

		 measurements[j] = models[j]->CalculateMeasurement(withEvents, od, rt);					// made changes by TUAN NGUYEN

         if (measurements[j].isFeasible)
         {
            ++successCount;
            eventCount += measurements[j].eventCount;
         }
      }

      for (UnsignedInt j = 0; j < adapters.size(); ++j)
      {
         // Specify current observation data. If no observation data used, it
         // passes a NULL pointer.
         ObservationData* od = NULL;
         if (!observations.empty())
            od = &(*currentObs);

         measurements[j] = adapters[j]->CalculateMeasurement(withEvents, od);
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
         // Specify ramp table associated with measurement model models[j]:						// made changes by TUAN NGUYEN
         // Note: Only one ramp table is used for a measurement model							// made changes by TUAN NGUYEN
		 StringArray sr = models[measurementToCalc]->GetStringArrayParameter("RampTables");		// made changes by TUAN NGUYEN
		 std::vector<RampTableData>* rt = NULL;													// made changes by TUAN NGUYEN
		 if (sr.size() > 0)																		// made changes by TUAN NGUYEN
		    rt = &(rampTables[sr[0]]);															// made changes by TUAN NGUYEN

		 // Specify current observation data. If no observation data used, it passes a NULL pointer. 
		 ObservationData* od = NULL;															// made changes by TUAN NGUYEN
		 if (!observations.empty())																// made changes by TUAN NGUYEN
            od = &(*currentObs);																// made changes by TUAN NGUYEN

         measurements[measurementToCalc] =														// made changes by TUAN NGUYEN
			 models[measurementToCalc]->CalculateMeasurement(withEvents, od, rt);				// made changes by TUAN NGUYEN
         
		 if (measurements[measurementToCalc].isFeasible)
         {
            successCount = 1;
            eventCount = measurements[measurementToCalc].eventCount;
         }
      }

      if ((measurementToCalc < (Integer)adapters.size()) && (measurementToCalc >= 0))
      {
         // Specify current observation data. If no observation data used, it passes a NULL pointer.
         ObservationData* od = NULL;
         if (!observations.empty())
            od = &(*currentObs);

         #ifdef DEBUG_CALCULATE
            MessageInterface::ShowMessage("****** models[%d]name = '%s'\n",
                  measurementToCalc,
                  models[measurementToCalc]->GetName().c_str());
            MessageInterface::ShowMessage("****** observations.size() = %d\n", observations.size());
            if (od == NULL)
               MessageInterface::ShowMessage("****** Observation data is not used in calculation\n");
            else
              MessageInterface::ShowMessage("****** currentObs: epoch = %.12lf, participants: %s  %s,  meas value = %.12lf\n", currentObs->epoch, currentObs->participantIDs[0].c_str(), currentObs->participantIDs[1].c_str(), currentObs->value[0]);
         #endif

         measurements[measurementToCalc] =
            adapters[measurementToCalc]->CalculateMeasurement(withEvents, od);

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

   std::vector<ObservationData>     obsTable;				// made changes by TUAN NGUYEN
   std::vector<UnsignedInt>         startIndexes;			// made changes by TUAN NGUYEN
   std::vector<UnsignedInt>         endIndexes;				// made changes by TUAN NGUYEN
   observations.clear();
   
   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      startIndexes.push_back(obsTable.size());				// made changes by TUAN NGUYEN

      ObservationData *od;
///// TBD: Look at file handlers here once data file pieces are designed
	  ObservationData od_old;
	  od_old.epoch = -1.0;

	  std::string streamFormat = streamList[i]->GetStringParameter("Format");
///// TBD: Especially here; this style will cause maintenence issues as more types are added
	  if ((streamFormat == "GMAT_OD")||(streamFormat == "GMAT_ODDoppler")||(streamFormat == "GMATInternal"))		// made changes by TUAN NGUYEN. It needs for loading all type of observation data (except ramp table)
	  {
      #ifdef USE_DATAFILE_PLUGINS
         if (streamList[i]->GetIsOpen())
         {
            ObType *obs;

            // This part needs some design information from Matt
            ObType *obd = dfa.GetObTypeObject(streamList[i]);
            if (!streamList[i]->GetData(obd))
            {
               throw MeasurementException("Could not load observation\n");
            }
            dfa.LoadObservation(*obd, *od);

            while (!streamList[i]->IsEOF())
            {
               if (streamList[i]->GetData(obs))
               {
                  // Store the data for processing
               }
               streamList[i]->AdvanceToNextOb();
            }
         }
      #else
		 
         if (streamList[i]->IsOpen())
         {
			UnsignedInt filter1Num, filter2Num, filter3Num, filter4Num, filter5Num, count, numRec;
			filter1Num = filter2Num = filter3Num = filter4Num = filter5Num =  count = numRec = 0;
			Real acc = 1.0;

			Real epoch1 = 0.0;
            Real epoch2 = 0.0;

			Real thinningRatio = streamList[i]->GetRealParameter("DataThinningRatio"); 
			StringArray selectedStations = streamList[i]->GetStringArrayParameter("SelectedStationIDs"); 
			while (true)
			{
			   od = streamList[i]->ReadObservation();
			   ++numRec;

			   // End of file
			   if (od == NULL)
			   {
				  --numRec;
                  break;
			   }

			   // Get start epoch and end epoch when od != NULL
			   if (epoch1 == 0.0)
			   {
			      epoch1 = TimeConverterUtil::Convert(streamList[i]->GetRealParameter("StartEpoch"), TimeConverterUtil::A1MJD, od->epochSystem);
			      epoch2 = TimeConverterUtil::Convert(streamList[i]->GetRealParameter("EndEpoch"), TimeConverterUtil::A1MJD, od->epochSystem);
			   }

			   // Data thinning filter
			   acc = acc + thinningRatio;
			   if (acc < 1.0)
               {
                  #ifdef DEBUG_LOAD_OBSERVATIONS
				  MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf :Throw away this record due to data thinning\n", streamFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
                  #endif
//				  MessageInterface::ShowMessage("acc = %lf\n", acc);
				  ++filter4Num;
				  continue;
			   }
			   else
			      acc = acc -1.0;

			   // Time span filter
               if ((od->epoch < epoch1)||(od->epoch > epoch2))
               {
                  #ifdef DEBUG_LOAD_OBSERVATIONS
		          MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf :Throw away this record due to time span filter\n", streamFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
                  #endif
                  ++filter5Num;
				  continue;
               }

			   // Invalid measurement value filter
            if (od->value.size() > 0)
			   if (od->value[0] == -1.0)		// throw away this observation data if it is invalid	// made changes by TUAN NGUYEN
			   {
                  #ifdef DEBUG_LOAD_OBSERVATIONS
				  MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf :Throw away this record due to invalid observation data\n", streamFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
                  #endif
				  ++filter1Num;
				  continue;
			   }

			   // Duplication or time order filter
			   if (od_old.epoch >= od->epoch)
			   {
                  #ifdef DEBUG_LOAD_OBSERVATIONS
		          MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf :Throw away this record due to duplication or time order\n", streamFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
                  #endif
				  ++filter2Num;
				  continue;
			   }

			   // Selected stations filter
               bool choose = false;
               if (selectedStations.size() == 0)
				  choose = true;
			   else
			   {
				  for (int j=0; j < selectedStations.size(); ++j)
				  {
				     if (selectedStations[j] == od->participantIDs[0])
					 {
					    choose = true;
						break;
					 }
				  }
               }

               if (choose == false)
			   {
                  #ifdef DEBUG_LOAD_OBSERVATIONS
				  MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf :Throw away this record due to station is not in SelectedStationID\n", streamFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
                  #endif
				  ++filter3Num;
				  continue;
			   }

			   obsTable.push_back(*od);
               #ifdef DEBUG_LOAD_OBSERVATIONS
	           MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf   ", streamFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
			   if (od->typeName == "DSNTwoWayDoppler")
				   MessageInterface::ShowMessage(" Band = %d    Doppler count interval = %le\n", od->uplinkBand, od->dopplerCountInterval);
			   else if (od->typeName == "DSNTwoWayRange")
				   MessageInterface::ShowMessage(" Band = %d    Frequency = %.15le   Range Modulo = %.15le\n", od->uplinkBand, od->uplinkFreq, od->rangeModulo);
			   else
				   MessageInterface::ShowMessage("\n");
               #endif

			   ++count;
               od_old = *od;
			}// endwhile(true)	

			MessageInterface::ShowMessage("Number of thown records in '%s' due to:\n", streamList[i]->GetStringParameter("Filename").c_str());
            MessageInterface::ShowMessage("      .Invalid measurement value              : %d\n", filter1Num);
			MessageInterface::ShowMessage("      .Duplication record or time order filter: %d\n", filter2Num);
			MessageInterface::ShowMessage("      .Selected stations filter               : %d\n", filter3Num);
			MessageInterface::ShowMessage("      .Data thinning filter                   : %d\n", filter4Num);
			MessageInterface::ShowMessage("      .Time span filter                       : %d\n", filter5Num);
			MessageInterface::ShowMessage("Total number of records in '%s': %d\n", streamList[i]->GetStringParameter("Filename").c_str(), numRec);
            MessageInterface::ShowMessage("Number of records in '%s' used for estimation: %d\n\n", streamList[i]->GetStringParameter("Filename").c_str(), count);

         } // endif (streamList[i]->IsOpen())
		 
      #endif
	  } // endif ((streamFormat == "GMAT_OD")

	  // made changes by TUAN NGUYEN : fix bug GMT-4394
	  if (obsTable.size() == 0)
		 throw MeasurementException("Error: No observation data in tracking data file '" + streamList[i]->GetStringParameter("Filename") +"'\n"); 
	  else
	  {
         if ((Integer)startIndexes[i] <= (obsTable.size()-1))
		    endIndexes.push_back(obsTable.size()-1);
	     else
		    throw MeasurementException("Error: No observation data in tracking data file '" + streamList[i]->GetStringParameter("Filename") +"'\n"); 
	  }
	  
   }


   // Sort observation data by epoch due to observations table is required to have an epoch ascending order
   bool completed = false;
   while (!completed)
   {
      UnsignedInt minIndex = streamList.size();
      for (UnsignedInt i = 0; i < streamList.size(); ++i)
      {
		 if (startIndexes[i] > endIndexes[i])
            continue;

		 if (minIndex == streamList.size())
            minIndex = i;
		 else
		 {
		    if (obsTable[startIndexes[minIndex]].epoch > obsTable[startIndexes[i]].epoch)
               minIndex = i;
		 }
	  }
	  
	  if (minIndex < streamList.size())
	  {
	     observations.push_back(obsTable[startIndexes[minIndex]]);
		 startIndexes[minIndex]++;
	  }
	  else
		 completed = true;

   }
   
   #ifdef DEBUG_LOAD_OBSERVATIONS
   for (UnsignedInt i = 0; i < observations.size(); ++i)
	   MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf\n", observations[i].dataFormat.c_str(), observations[i].epoch, observations[i].typeName.c_str(), observations[i].type, observations[i].participantIDs[0].c_str(), observations[i].participantIDs[1].c_str(), observations[i].value[0]);
   #endif

   // Set the current data pointer to the first observation value
   currentObs = observations.begin();
   MessageInterface::ShowMessage("Total number of load records : %d\n", observations.size());

#ifdef DEBUG_LOAD_OBSERVATIONS
   MessageInterface::ShowMessage(
         "Exit MeasurementManager::LoadObservations() method\n");
#endif

   return observations.size(); 
}



std::vector<ObservationData>* MeasurementManager::GetObservationDataList()
{
   return &observations;
}


//-----------------------------------------------------------------------------
// void LoadRampTables()												// this function was added by TUAN NGUYEN
//-----------------------------------------------------------------------------
/**
 * Load all frequency ramp tables.
 *
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
			   std::vector<RampTableData> ramp_table;
               rtd = rampTableDataStreamList[i]->ReadRampTableData();
               while (rtd != NULL)
               {
				  // Data records containing rampType = 0(snap), 6(invalid/unknown) , and 7(left bank in DSN file) will be removed from ramp table  
				  if ((rtd_old.epoch < rtd->epoch)&&(rtd->rampType >= 1)&&(rtd->rampType <= 5))
			      {
                     ramp_table.push_back(*rtd);

                     #ifdef DEBUG_LOAD_FREQUENCY_RAMP_TABLE
					    MessageInterface::ShowMessage(" epoch: %.15lf   participants: %s   %s   frequency band = %d    ramp type = %d    ramp frequency = %.12le     ramp rate = : %.12le\n", rtd->epoch, rtd->participantIDs[0].c_str(), rtd->participantIDs[1].c_str(), rtd->uplinkBand, rtd->rampType, rtd->rampFrequency, rtd->rampRate);
                     #endif

					 rtd_old = *rtd;
			      }
			      else
		          {
                     #ifdef DEBUG_LOAD_FREQUENCY_RAMP_TABLE
					    MessageInterface::ShowMessage(" epoch: %.15lf   participants: %s   %s   frequency band = %d    ramp type = %d    ramp frequency = %.12le     ramp rate = : %.12le: throw away this record due to the order of time or rampType = 0, 6, or 7\n", rtd->epoch, rtd->participantIDs[0].c_str(), rtd->participantIDs[1].c_str(), rtd->uplinkBand, rtd->rampType, rtd->rampFrequency, rtd->rampRate);
                     #endif
			      }
                  rtd = rampTableDataStreamList[i]->ReadRampTableData();
               }

			   rampTables[rampTableDataStreamList[i]->GetName()] = ramp_table;
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


// Made changes by TUAN NGUYEN
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
 * @return	true when it is at the end of observations table otherwise return false
 */
//-----------------------------------------------------------------------------
bool MeasurementManager::AdvanceObservation()
{
   if (currentObs != observations.end())
   {
      ++currentObs;

	  // This change intends to use all observation data no matter it is either in used or not. 
	  // When they come into BatchEstimatorInv::Accumulate() function, all unused data will be 
	  // writen to report file as unused data.  
      //while ((currentObs != observations.end())&&		// made changes by TUAN NGUYEN
		    // (!currentObs->inUsed))					// made changes by TUAN NGUYEN
      //{												// made changes by TUAN NGUYEN
      //      ++currentObs;								// made changes by TUAN NGUYEN
      //}												// made changes by TUAN NGUYEN
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
      StringArray parts = (*i)->GetStringArrayParameter("AddTrackingConfig");
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
// const Integer GetMeasurementId(const std::string &modelName) const
//-----------------------------------------------------------------------------
/**
 * Retrieves the model ID for a named measurement model
 *
 * @param modelName The name of the measurement model
 *
 * @return The ID for model
 */
//-----------------------------------------------------------------------------
const Integer MeasurementManager::GetMeasurementId(
		const std::string &modelName) const
{
   Integer foundId = -1;

   for (UnsignedInt i = 0; i < models.size(); ++i)
   {
      if (models[i]->GetName() == modelName)
      {
         foundId = models[i]->GetModelID();
         break;
      }
   }

   /// @todo: Handle multiple measurements inside of a TrackingFileSet
   for (UnsignedInt i = 0; i < trackingSets.size(); ++i)
   {
      if (trackingSets[i]->GetName() == modelName)
      {
         if (adapterFromTFSMap.find(trackingSets[i]) != adapterFromTFSMap.end())
         {
            /// @todo: Make measurementID accessor work for > 1 adapter in a TFS
            StringArray tfsAdapterList =
                  adapterFromTFSMap.find(trackingSets[i])->second;

            std::string modelAdapterName = (tfsAdapterList.size() > 0 ?
                  tfsAdapterList[0] : "Range");

            for (UnsignedInt j = 0; j < adapters.size(); ++j)
               if (adapters[j]->GetName() == modelAdapterName)
                  foundId = adapters[j]->GetModelID();
         }
      }
   }

   return foundId;
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


//------------------------------------------------------------------------------
// bool CalculateMeasurements(bool forSimulation, bool withEvents, bool addNoise)	
//																					This method was modified by TUAN NGUYEN
//------------------------------------------------------------------------------
/**
 * Fires the calculation method of each owned MeasurementModel
 *
 * @param  forSimulation	true for simulation calculation and false for 
 *                          estimation calculation. Default value is false
 * @param  withEvents       flag to indicate calculation with ro without events
 * @param  addNoise			flag to indicate noise added to measurement
 *
 * @return True if at least one measurement is feasible and calculated; false
 *         otherwise
 */
//------------------------------------------------------------------------------
bool MeasurementManager::CalculateMeasurements(bool forSimulation, bool withEvents, bool addNoise)
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(" Entered bool MeasurementManager::CalculateMeasurements(%s,%s)\n", (forSimulation?"true":"false"), (withEvents?"true":"false"));
   #endif
   
   // Specify observation data for measurement
   ObservationData* od = NULL;																// made changes by TUAN NGUYEN
   if (!observations.empty())																// made changes by TUAN NGUYEN
     od = &(*currentObs);																	// made changes by TUAN NGUYEN

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
         // Specify ramp table associated with measurement model models[j]:					// made changes by TUAN NGUYEN
	     // Note: Only one ramp table is used for a measurement model						// made changes by TUAN NGUYEN
         StringArray sr = models[j]->GetStringArrayParameter("RampTables");					// made changes by TUAN NGUYEN
	     std::vector<RampTableData>* rt = NULL;												// made changes by TUAN NGUYEN
	     if (sr.size() > 0)																	// made changes by TUAN NGUYEN
	        rt = &(rampTables[sr[0]]);														// made changes by TUAN NGUYEN
		 
		 if (withEvents)
		 {  
         #ifdef DEBUG_CALCULATE_MEASUREMENTS
	        MessageInterface::ShowMessage(" Simulation: measurement with events\n");
         #endif
			if (measurements[j].isFeasible)
			{
	           measurements[j] = models[j]->CalculateMeasurement(withEvents, od, rt, addNoise);		// made changes by TUAN NGUYEN
			   if (measurements[j].unfeasibleReason == "R")											// made changes by TUAN NGUYEN
			   {
				  Real a1Time = measurements[j].epoch;
				  Real taiTime;
				  std::string tais;
				  TimeConverterUtil::Convert("A1ModJulian", a1Time, "", "TAIModJulian", taiTime, tais); 
				  char s[1000];
				  sprintf(&s[0], "Error: In simulation for measurement model %s, epoch %.12lf TAIMdj is out of ramped table.\n Please make sure ramped table cover all simulation epoches.\n", models[j]->GetName().c_str(), taiTime);
			      throw MeasurementException(s); 
			   }
			}
		 }
		 else
		 {
         #ifdef DEBUG_CALCULATE_MEASUREMENTS
	        MessageInterface::ShowMessage(" Simulation: measurement without events\n");
         #endif
			measurements[j] = models[j]->CalculateMeasurement(withEvents, od, rt);			// made changes by TUAN NGUYEN
		 }

         if (measurements[j].isFeasible)
         {
            if (!withEvents)
               eventCount += measurements[j].eventCount;
            retval = true;
         }

         #ifdef DEBUG_FLOW
		    MessageInterface::ShowMessage(" Measurement is %s. Its value is %lf\n", (measurements[j].isFeasible?"feasible":" not feasible"), measurements[j].value[0]);
         #endif

      }
      // Now do the same thing for the TrackingDataAdapters
      for (UnsignedInt i = 0; i < adapters.size(); ++i)
      {
         std::vector<RampTableData>* rt = NULL;
         measurements[i] = adapters[i]->CalculateMeasurement(withEvents, od, rt);
         retval = measurements[i].isFeasible;
   }
   }
   else
   {  // This section is used for estimation only:
      if (od == NULL)
	  {
         #ifdef DEBUG_CALCULATE_MEASUREMENTS
	        MessageInterface::ShowMessage("    observation data is NULL\n");
         #endif
         return retval;					// no calculation measurement value whenever no observation data is available
	  }
      #ifdef DEBUG_CALCULATE_MEASUREMENTS
	  else
		  MessageInterface::ShowMessage("    %s observation data: %.12lf  %s  %d  %s  %s   %.12lf\n", od->dataFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(),od->value[0]);
      #endif

      for (UnsignedInt j = 0; j < models.size(); ++j)
      {
         // Verify observation data belonging to the measurement model jth
		 bool isbelong;
		 std::string reason;
         if ((models[j]->GetStringParameter("Type") != od->typeName))
		 {
	        isbelong = false;
		 }
		 else
		 {
			isbelong = false;
		    ObjectArray participants = models[j]->GetParticipants();		// participants in measurement model
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
			      }

				  if (isbelong == false)  
					 break;				  
			   }

			}
		 }
		 
		 if (isbelong == false)
         {
//			 MessageInterface::ShowMessage("This observation data is not belong to model[%d]<%s>: %lf  %s  %s  %s  %lf\n",j, models[j]->GetStringParameter("Type").c_str(), od->epoch, od->typeName.c_str(), od->participantIDs[0].c_str(), od->participantIDs[1].c_str(), od->value[0]);
		    measurements[j].typeName		 = models[j]->GetStringParameter("Type");
	        measurements[j].epoch			 = od->epoch;
	        measurements[j].epochSystem	     = od->epochSystem;
		    measurements[j].isFeasible		 = false;
	        measurements[j].covariance		 = NULL;
	        measurements[j].eventCount		 = 0;
	        measurements[j].feasibilityValue = 0.0;
			measurements[j].unfeasibleReason = "U";
	        measurements[j].value.clear();  
         }
		 else
		 {
		    
///// TBD: Do we want something more generic here?
            // Specify ramp table associated with measurement model models[j]:					// made changes by TUAN NGUYEN
	        // Note: Only one ramp table is used for a measurement model						// made changes by TUAN NGUYEN
            StringArray sr = models[j]->GetStringArrayParameter("RampTables");					// made changes by TUAN NGUYEN
	        std::vector<RampTableData>* rt = NULL;												// made changes by TUAN NGUYEN
	        if (sr.size() > 0)																	// made changes by TUAN NGUYEN
	           rt = &(rampTables[sr[0]]);														// made changes by TUAN NGUYEN
		    
	        measurements[j] = models[j]->CalculateMeasurement(withEvents, od, rt);				// made changes by TUAN NGUYEN
	     
            if (measurements[j].isFeasible)
            {
               if (!withEvents)
                  eventCount += measurements[j].eventCount;
               retval = true;
            }
		 }
      }

      // Now do the tracking data adapters
      for (UnsignedInt j = 0; j < adapters.size(); ++j)
      {
         measurements[j] = adapters[j]->CalculateMeasurement(withEvents, od);
         if (measurements[j].isFeasible)
         {
//            if (!withEvents)
//               eventCount += measurements[j].eventCount;
            retval = true;
   }
         #ifdef DEBUG_ADAPTERS
            MessageInterface::ShowMessage("   Measurement %d computed; first "
                  "value: %lf\n", j, measurements[j].value[0]);
         #endif
      }
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
 * @return true on success, false on failure
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
         idToStreamMap[measurements[i].uniqueID]->
               WriteMeasurement(&(measurements[i]));
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
// const StringArray& GetRampTableDataStreamList()				// this function was added by TUAN NGUYEN for processing ramp tables
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
//      StringArray filenames = tfs->GetStringArrayParameter("Filename");
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


const std::vector<MeasurementModel*>& MeasurementManager::GetAllMeasurementModels()
{
	return models;
}

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
