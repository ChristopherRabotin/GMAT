//$Id: Simulator.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         Simulator
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
// Author: Darrel J. Conway, Thinking Systems, Inc. & Wendy Shoan/GSFC/GSSB
// Created: 2009/06/30
//
/**
 * Definition for the class used to generate simulated measurement data
 */
//------------------------------------------------------------------------------


#ifndef Simulator_hpp
#define Simulator_hpp

#include "estimation_defs.hpp"
#include "Solver.hpp"
#include "GmatState.hpp"
#include "MeasurementManager.hpp"
#include "PropSetup.hpp"


// todo: Make this a propagator parameter
#define SIMTIME_ROUNDOFF 1e-6

class ESTIMATION_API Simulator : public Solver
{
public:
   Simulator(const std::string &name);
   Simulator(const Simulator& sim);
   Simulator& operator=(const Simulator& sim);
   virtual ~Simulator();

   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   virtual Integer      SetSolverResults(Real*, const std::string&,
                                         const std::string&); // noop
   virtual void         SetResultValue(Integer eventState, Real val = 0.0,
                                       const std::string& eventName = "");
   virtual void         WriteToTextFile(SolverState stateToUse = UNDEFINED_STATE);

   virtual bool         Initialize();
   virtual SolverState  AdvanceState();
   virtual bool         Finalize();

   Real                 GetTimeStep(GmatEpoch fromEpoch);
   PropSetup*           GetPropagator();
   MeasurementManager*  GetMeasurementManager();

   // methods overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                       GetPropertyEnumStrings(const Integer id) const;
   virtual Gmat::ObjectType
                        GetPropertyObjectType(const Integer id) const;

   // Access methods for the reference objects
   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         SetRefObjectName(const Gmat::ObjectType type,
                                         const std::string &name);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);


   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

   virtual void         UpdateCurrentEpoch(GmatEpoch newEpoch);

   virtual bool         HasLocalClones();
   virtual void         UpdateClonedObject(GmatBase *obj);

protected:
   /// Enumeration for the scripted parameters on the Simulator
   enum
   {
      MEASUREMENTS = SolverParamCount,
      PROPAGATOR,
      INITIAL_EPOCH_FORMAT,
      INITIAL_EPOCH,
      FINAL_EPOCH_FORMAT,
      FINAL_EPOCH,
      MEASUREMENT_TIME_STEP,
      SimulatorParamCount
   };
   /// Script strings associated with the parameters
   static const std::string    PARAMETER_TEXT[SimulatorParamCount -
                                              SolverParamCount];
   /// Types of the scripted parameters
   static const Gmat::ParameterType
                               PARAMETER_TYPE[SimulatorParamCount -
                                              SolverParamCount];
   /// The propagator configured for simulation
   PropSetup           *propagator;
   /// Name of the propagator
   std::string         propagatorName;

   /// The state vector, used to buffer state information during event location
   GmatState           *simState;   // (This piece is still in flux -- do we do
                                    // it like this, or differently?)
   /// The initial epoch for the simulation
   GmatEpoch           simulationStart;
   /// The target epoch for the end of the simulation
   GmatEpoch           simulationEnd;
   /// The epoch of the next measurement calculation
   GmatEpoch           nextSimulationEpoch;
   /// Counter used to advance the simulation epoch
   UnsignedInt         simEpochCounter;
   /// The current epoch (typically as determined via propagation)
   GmatEpoch           currentEpoch;

   /// Format of the scripted simulation start time
   std::string         initialEpochFormat;
   /// The string specifying the simulation start time
   std::string         initialEpoch;      // ??? is this simulationStart
   /// Format of the scripted simulation end time
   std::string         finalEpochFormat;
   /// The string specifying the simulation end time
   std::string         finalEpoch;      // ??? is this simulationEnd

   /// Time step from current measurement epoch to the epoch of the next measurement
   Real                simulationStep;

   /// Vector of the current active Event objects
   ObjectArray         activeEvents;
   /// Flag indicating is an Event is currently being located
   bool                locatingEvent;

   /**
    *  The time step that gets returned for the next propagation
    *
    *  timeStep will be the same as simulationStep when the state machine is in
    *  the PROPAGATING state.  When in the LOCATING state, timeStep is the time
    *  from the base epoch to the next attempt at finding the event that is
    *  being located.
    */
   Real                timeStep;

   /// The simulator's measurement manager
   MeasurementManager  measManager;
   /// The list of measurement models that the measurement manager is managing
   StringArray         measList;   // temporary - may get list from MeasManager;

   /// Local storage element for ref object names
   StringArray         refObjectList;

   // State machine methods
   void                   CompleteInitialization();
   void                   FindTimeStep();
   void                   CalculateData();
   void                   ProcessEvent();
   void                   SimulateData();
   void                   RunComplete();

   // Helper methods
   Real                   ConvertToRealEpoch(const std::string &theEpoch,
                                              const std::string &theFormat);
   void                   FindNextSimulationEpoch();
   // progress string for reporting
   virtual std::string    GetProgressString();
};

#endif /* Simulator_hpp */
