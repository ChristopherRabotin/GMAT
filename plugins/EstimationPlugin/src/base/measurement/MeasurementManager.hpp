//$Id: MeasurementManager.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
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
 * MeasurementManager declaration used in GMAT's estimators and simulator
 *
 * This class acts as a mediator between the estimators and simulator and the
 * measurement models.
 */
//------------------------------------------------------------------------------

#ifndef MeasurementManager_hpp
#define MeasurementManager_hpp

#include "estimation_defs.hpp"
#include "gmatdefs.hpp"
#include "EstimationDefs.hpp"
#include "MeasurementData.hpp"
#include "ObservationData.hpp"
#include "Rmatrix.hpp"
#include "MeasurementModel.hpp"
#include "TrackingSystem.hpp"

#include "CoreMeasurement.hpp"
#include "DataFile.hpp"


class ESTIMATION_API MeasurementManager
{
public:
   MeasurementManager();
   virtual ~MeasurementManager();
   MeasurementManager(const MeasurementManager &mm);
   MeasurementManager& operator=(const MeasurementManager &mm);

   bool                    Initialize();
   bool                    PrepareForProcessing(bool simulating = false);
   bool                    ProcessingComplete();
   bool                    Finalize();

   bool                    CalculateMeasurements(bool withEvents = false);
   const std::vector<RealArray>&
                           CalculateDerivatives(GmatBase *obj, Integer wrt,
                                                Integer forMeasurement);
   bool                    MeasurementHasEvents();
   ObjectArray&            GetActiveEvents();
   bool                    ProcessEvent(Event *locatedEvent);
   bool                    WriteMeasurements();

   Integer                 AddMeasurement(MeasurementModel *meas);
   Integer                 AddMeasurement(TrackingSystem *system);
   void                    AddMeasurementName(std::string measName);
   GmatBase*               GetClone(GmatBase *obj);
   const StringArray&      GetMeasurementNames() const;
   const Integer           GetMeasurementId(const std::string &modelName) const;
   const StringArray&      GetParticipantList();
   Integer                 Calculate(const Integer measurementToCalc,
                                     bool withEvents = false);
   const MeasurementData*  GetMeasurement(const Integer measurementToGet);
   MeasurementModel*       GetMeasurementObject(const Integer measurementToGet);
   Integer                 GetEventCount(const Integer forMeasurement = -1);
   const StringArray&      GetStreamList();
   void                    SetStreamObject(DataFile *newStream);
   bool                    WriteMeasurement(const Integer measurementToWrite);

   IntegerArray&           GetValidMeasurementList();

   // Observation reader methods needed for estimation
   void                    LoadObservations();
   GmatEpoch               GetEpoch();
   GmatEpoch               GetNextEpoch();
   const ObservationData * GetObsData(const Integer observationToGet = -1);
   void                    AdvanceObservation();
   void                    Reset();

protected:
   /// List of the managed measurement models
   StringArray                      modelNames;
   /// List of all participants referenced in the measurement models
   StringArray                      participants;
   /// Pointers to the measurements
   std::vector<MeasurementModel*>   models;
   /// Pointers to the tracking systems
   std::vector<TrackingSystem*>     systems;
   /// Current measurement epoch, ignoring event searching
   GmatEpoch                        anchorEpoch;
   /// Current measurement epoch, including event searching
   GmatEpoch                        currentEpoch;
   /// Measurement calculations
   std::vector<MeasurementData>     measurements;

   /// Observation data from all of the input observation data files
   std::vector<ObservationData>     observations;
   /// The current observation from the vector of observations
   std::vector<ObservationData>::iterator
                                    currentObs;

   /// Measurement derivatives
   std::vector<Rmatrix>             derivatives;
   /// Measurement stream objects
   StringArray                      streamNames;
   /// Measurement stream objects
   std::vector<DataFile*>           streamList;
   /// Temporary element used to manage events that are ready for processing
   ObjectArray                      activeEvents;
   /// Association between Events and the MeasurementModels that provided them
   std::map<Event*,MeasurementModel*>
                                    eventMap;

   /// Starting ID for measurement models; used to provide unique IDs in a run
   Integer                          idBase;
   /// One more than the largest assigned ID
   Integer                          largestId;

   /// Maping between unique IDs and the associated data files
   std::map<Integer,DataFile*>      idToStreamMap;

   /// Indices of measurement models that match observations at current epoch
   IntegerArray                     activeMeasurements;
   /// Total number of events that must be evaluated
   Integer                          eventCount;

   Integer                          FindModelForObservation();
};

#endif /*MeasurementManager_hpp*/
