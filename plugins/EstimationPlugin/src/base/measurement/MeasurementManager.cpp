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

#include "DataFileAdapter.hpp"


//#define DEBUG_INITIALIZATION
//#define DEBUG_FILE_WRITE
//#define DEBUG_FLOW
//#define DEBUG_MODEL_MAPPING

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
   anchorEpoch       (GmatTimeConstants::MJD_OF_J2000),
   currentEpoch      (GmatTimeConstants::MJD_OF_J2000),
   idBase            (10000),
   largestId         (10000),
   eventCount        (0)
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
   anchorEpoch       (mm.anchorEpoch),
   currentEpoch      (mm.currentEpoch),
   idBase            (mm.idBase),
   largestId         (mm.largestId),
   eventCount        (mm.eventCount)
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
      anchorEpoch  = mm.anchorEpoch;
      currentEpoch = mm.currentEpoch;
      modelNames   = mm.modelNames;
      eventCount   = mm.eventCount;

      // Clone the measurements
      for (std::vector<MeasurementModel*>::iterator i = models.begin();
            i != models.end(); ++i)
         delete (*i);

      models.clear();
      measurements.clear();

      for (std::vector<MeasurementModel*>::const_iterator i = mm.models.begin();
            i != mm.models.end(); ++i)
         models.push_back((MeasurementModel*)(*i)->Clone());
   }

   return *this;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Verifies that the measuremetn models are ready to calculate measuremetns,
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

   return retval;
}


//------------------------------------------------------------------------------
// bool MeasurementManager::PrepareForProcessing(bool simulating)
//------------------------------------------------------------------------------
/**
 * This method...
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
#endif

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage(
         "Entered MeasurementManager::PrepareForProcessing(%s)\n",
         (simulating ? "true" : "false"));
#endif

   bool retval = true;

   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      #ifdef USE_DATAFILE_PLUGINS
         std::string writeMode = (simulating ? "w" : "r");
         streamList[i]->SetStringParameter(
               streamList[i]->GetParameterID("ReadWriteMode"), writeMode);

         if (streamList[i]->OpenFile() == false)
            retval = false;
      #else
         if (streamList[i]->OpenStream(simulating) == false)
            retval = false;
      #endif
   }

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
            "Entered MeasurementManager::Calculate(%d) method\n",
            measurementToCalc);
   #endif

   Integer successCount = 0;
   eventCount = 0;

   if (measurementToCalc == -1)
   {
      for (UnsignedInt j = 0; j < models.size(); ++j)
      {
         measurements[j] = models[j]->CalculateMeasurement(withEvents);
         if (measurements[j].isFeasible)
         {
            ++successCount;
            eventCount += measurements[j].eventCount;
         }
      }
   }
   else
   {
      if ((measurementToCalc < (Integer)models.size()) && measurementToCalc >= 0)
      {
         measurements[measurementToCalc] =
               models[measurementToCalc]->CalculateMeasurement(withEvents);
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
         "Entered MeasurementManager::GetMeasurement() method\n");
#endif

   MeasurementData *theMeasurement = NULL;
   if ( (measurementToGet >= 0) &&
        (measurementToGet < (Integer)measurements.size()))
      theMeasurement = &measurements[measurementToGet];

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
MeasurementModel* MeasurementManager::GetMeasurementObject(
      const Integer measurementToGet)
{
   MeasurementModel *retval = NULL;

   if ((measurementToGet >= 0) && (measurementToGet < (Integer)models.size()))
      retval = models[measurementToGet];

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
// void LoadObservations()
//-----------------------------------------------------------------------------
/**
 * Opens the observation data sources and reads in all available observations.
 *
 * @todo Once multiple observations sources are in use, this method will need to
 * sort the observations into time order.
 */
//-----------------------------------------------------------------------------
void MeasurementManager::LoadObservations()
{
#ifdef DEBUG_FLOW
   MessageInterface::ShowMessage(
         "Entered MeasurementManager::LoadObservations() method\n");
#endif

   #ifdef USE_DATAFILE_PLUGINS
      DataFileAdapter dfa;
   #endif

   observations.clear();

   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      ObservationData *od;
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
            od = streamList[i]->ReadObservation();

            while (od != NULL)
            {
               observations.push_back(*od);
               od = streamList[i]->ReadObservation();
            }
         }
      #endif
   }

   // Set the current data pointer to the first observation value
   currentObs = observations.begin();
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
// void MeasurementManager::AdvanceObservation()
//-----------------------------------------------------------------------------
/**
 * Advances the current observation pointer to the next observation in the
 * observations vector
 */
//-----------------------------------------------------------------------------
void MeasurementManager::AdvanceObservation()
{
   if (currentObs != observations.end())
      ++currentObs;
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
   meas->SetModelID(largestId++);
   models.push_back(meas);

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
            "Added measurement of type %s with unique ID %d\n",
            meas->GetStringParameter("Type").c_str(),
            meas->GetModelID());
   #endif

   return meas->GetModelID();
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
         "measurement model names\n", modelNames.size());
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
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool CalculateMeasurements()
//------------------------------------------------------------------------------
/**
 * Fires the calculation method of each owned MeasurementModel
 *
 * @return True if at least one measurement is feasible and calculated; false
 *         otherwise
 */
//------------------------------------------------------------------------------
bool MeasurementManager::CalculateMeasurements(bool withEvents)
{
   bool retval = false;

   eventCount = 0;

   for (UnsignedInt j = 0; j < models.size(); ++j)
   {
      measurements[j] = models[j]->CalculateMeasurement(withEvents);
      if (measurements[j].isFeasible)
      {
         if (!withEvents)
            eventCount += measurements[j].eventCount;
         retval = true;
      }
   }

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
   activeEvents.clear();
   eventMap.clear();

   for (UnsignedInt j = 0; j < models.size(); ++j)
   {
      if (measurements[j].isFeasible)
      {
         Integer count = models[j]->GetEventCount();
         for (Integer i = 0; i < count; ++i)
         {
            Event *currentEvent = models[j]->GetEvent(i);
            eventMap[currentEvent] = models[j];
            activeEvents.push_back(currentEvent);
         }
      }
   }

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
   return models[forMeasurement]->CalculateMeasurementDerivatives(obj, wrt);
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

   return streamNames;
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

   Gmat::MeasurementType type =  currentObs->type;

   #ifdef DEBUG_MODEL_MAPPING
       MessageInterface::ShowMessage("   Current observation type: %d\n", type);
   #endif

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
