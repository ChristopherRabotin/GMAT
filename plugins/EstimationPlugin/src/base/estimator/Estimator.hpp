//$Id: Estimator.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         Estimator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
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
// Created: 2009/08/03
//
/**
 * Definition of the Estimator base class
 */
//------------------------------------------------------------------------------


#ifndef Estimator_hpp
#define Estimator_hpp

#include "Solver.hpp"
#include "MeasurementManager.hpp"
#include "EstimationStateManager.hpp"
#include "PropSetup.hpp"

/**
 * The base class used for GMAT's estimators
 */

//#define ESTTIME_ROUNDOFF 1e-10
#define ESTTIME_ROUNDOFF 1e-9      //1e-10 causes infinitive loop in propagation steps

class ESTIMATION_API Estimator : public Solver
{
public:
   Estimator(const std::string &type, const std::string &name);
   virtual ~Estimator() = 0;
   Estimator(const Estimator& est);
   Estimator& operator=(const Estimator& est);

   virtual void         SetSolarSystem(SolarSystem *ss);

   virtual bool         Initialize();
   bool                 Reinitialize();
   virtual void         CompleteInitialization();
   virtual bool         Finalize();

   // methods overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
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

   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id,
                                         const std::string &value);
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label,
                                          const std::string &value);

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);

   virtual UnsignedInt
                        GetPropertyObjectType(const Integer id) const;

   // Access methods for the reference objects
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObjectName(const UnsignedInt type,
                                         const std::string &name);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);

   Real                 GetTimeStep();

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

   virtual void         UpdateCurrentEpoch(GmatTime newEpoch);
   virtual GmatTime     GetCurrentEpoch();

   PropSetup*           GetPropagator();
   MeasurementManager*  GetMeasurementManager();
   EstimationStateManager*
                        GetEstimationStateManager();

   bool                 ResetState();

   virtual bool HasLocalClones();
   virtual void UpdateClonedObject(GmatBase *obj);
   virtual void UpdateClonedObjectParameter(GmatBase *obj,
         Integer updatedParameterId);

   void                SetDelayInitialization(bool delay); 
   
protected:
///// TBD: Do we need to separate TS and MM like this going forward?
   /// Names of measurements and tracking systems used in the estimation
   StringArray             measurementNames;
   /// Names of the measurement models used in the estimation
   StringArray             modelNames;
   /// The solve for parameters in the estimation problem
   StringArray             solveForStrings;
   /// The consider parameters in the estimation problem
   StringArray             considerStrings;
   /// Tolerance measure applied to RMS state change to test for convergence
   Real                    absoluteTolerance;
   /// Tolerance measure applied to RMS residual change to test for convergence
   Real                    relativeTolerance;
   /// Name of the PropSetup used to evolve the system
   std::string             propagatorName;
   /// Flag used to check for a state load from the objects prior to propagation
   bool                    resetState;


   //---------------------------------------------------------------------------
   // Internal Data
   //---------------------------------------------------------------------------

   /**
    *  The timestep that gets returned for the next propagation
    *
    *  timeStep will be the same as estimationStep when the state machine is in
    *  the PROPAGATING state.  When in the LOCATING state, timeStep is the time
    *  from the base epoch to the next attempt at finding the event that is
    *  being located.
    */
   Real                    timeStep;
   /// Local storage element for ref object names
   StringArray             refObjectList;

   /// The estimator's measurement manager
   MeasurementManager      measManager;
   /// The estimator's state manager
   EstimationStateManager  esm;
   /// The propagator configured for simulation
   PropSetup               *propagator;

   /// Epoch of the current estimation state
   GmatTime                estimationEpochGT;
   /// Currrrent simulation epoch
   GmatTime                currentEpochGT;
   /// The next epoch desired from propagation
   GmatTime                nextMeasurementEpochGT;

   /// The measurement derivatives at the measurement epoch, \tilde H
   std::vector<RealArray>  hTilde;
   /// The accumulated measurement data (transformed to the estimation epoch)
   std::vector<RealArray>  hAccum;

   /// Weight, Observation data, calculated data
   RealArray Weight;
   //RealArray OData;        // correction value of observation data
   //RealArray CData;
   
   /// The indices for the MeasurementModels with observations at current epoch
   IntegerArray            modelsToAccess;

   /// The estimation state transition matrix
   Rmatrix                 *stm;
   /// The estimation state covariance matrix
   Covariance              *stateCovariance;
   /// The estimated state in GMAT internal coordinate system (in Cartesian coordinate system)
   GmatState               *estimationState;
   /// The estimated state in solve-for state type (solve-for state could be CartesianState or KeplerianState)
   GmatState               estimationStateS;


   /// The previous estimated state in GMAT internal coordinate system (in Cartesian coordinate system)
   GmatState               oldEstimationState;
   /// The previous estimated state in solve-for state type (solve-for could be CartesianState or KeplerianState)
   GmatState               oldEstimationStateS;

   /// Apriori estimation state presenting in MJ2000Eq cooridnate system and solve-for state type
   GmatState aprioriMJ2000EqSolveForState;

   /// Apriori estimation state presenting in participants' cooridnate systems and state type specified by spacecraft's DisplayStateType
   GmatState aprioriSolveForState;
   /// Previous estimation state presenting in participants' coordinate systems and state type specified by spacecraft's DisplayStateType
   GmatState previousSolveForState;
   /// Current estimation state presenting in participants' coordinate systems and state type specified by spacecraft's DisplayStateType
   GmatState currentSolveForState;
   /// Apriori estimation state presenting in participants' cooridnate systems and state type specified by spacecraft's DisplayStateType
   /// Its anomaly is in form of MA 
   GmatState aprioriSolveForStateMA;

   /// Apriori estimation state presenting in participants' Cartesian coordinate system
   GmatState aprioriSolveForStateC;
   /// Previous estimation state presenting in participants' Cartesian coordinate system
   GmatState previousSolveForStateC;
   /// Current estimation state presenting in participants' coordinate systems
   GmatState currentSolveForStateC;


   /// Size of the estimation state vector
   UnsignedInt             stateSize;
   /// The estimated state in Cartesian coordinate system
   GmatState               initialEstimationState;
   /// The estimated state in solve-for state type (solve-for state could be CartesianState or KeplerianState)
   GmatState               initialEstimationStateS;

   /// Estimation status
   Integer                 estimationStatus;


   /// The information matrix, $\Lambda$
   Rmatrix                 information;

   /// The residual vector, N in Tapley, Schutz and Born
   Rvector                 residuals;
   /// The a priori state, \bar x_0 in Tapley, Schutz and Born
   Rvector                 x0bar;
   /// The O-C value for each observation
   RealArray               measurementResiduals;
   /// The observation epochs
   std::vector<GmatTime>   measurementEpochs;
   /// The observation ID for the residual
   IntegerArray            measurementResidualID;

   std::vector<GmatTime>   measurementTimes;
   // Index of an element in BatchEstimator::stationAndType array. 
   // It is used to specified ground station and data type associated with measurementResiduals. 
   IntegerArray            KeyIndex;


   IntegerArray            observationID;
   /// List of residual vector (each element is a residual vector = O_vector - C_vector)
   std::vector<RealArray>  measurementResVectors;


   /// Flag set to show all residuals during processing
   bool                    showAllResiduals;
   /// Flag used to only display a specific set of residuals
   bool                    showSpecificResiduals;
   /// Toggle for displayign error bars
   bool                    showErrorBars;
   /// List of the residuals plot names
   StringArray             addedPlots;

   /// The owned residuals plots used by this estimator
   std::vector<OwnedPlot*> residualPlots;
   /// The data used to generate error bars on the residuals plots
   std::vector<RealArray*> hiLowData;

   /// Vector of the current active Event objects
   ObjectArray             activeEvents;
   /// Flag indicating is an Event is currently being located
   bool                    locatingEvent;


   /// Parameters for data sigma editting
   Real maxResidualMult;
   Real constMult;
   Real additiveConst;
   /// Flag to indicate to use weightedRMS or predictedRMS for OLSE
   bool chooseRMSP;

   /// Flag indicating to reset best RMS when estimation is diverged
   bool resetBestRMSFlag;

   /// Number of removed observation data records
   std::map<std::string, UnsignedInt> numRemovedRecords;

   /// A string as a line/lines buffer to store a line/lines for writing to report file
   std::string linesBuff;

   std::string linesDataBuff;

   /// Solar system      // It needs to display information of central bodies in estimation report file
   SolarSystem *solarSystem;

   // EOP time range. It needs to check measurement epoch to be inside that range.
   Real      eopTimeMin;        // EOP time min
   Real      eopTimeMax;        // EOP time max
   Integer   warningCount;      // count number of warning message when check measurement epoch to be inside or outside EOP time range

   // Media correction QA
   StringArray ionoWarningList;   // list contains all measurement passes with ionosphere correction to be outside acceptable range [0m, 20m]
   StringArray tropoWarningList;  // list contains all measurement passes with troposphere correction to be outside acceptable range [0m, 60m]

   /// List of estimation data filters
   StringArray dataFilterStrings;
   ObjectArray dataFilterObjs;
   IntegerArray editedRecords;             // flag indicating an observation data record used for calculating estimation

   /// Parameters associated with the Estimators
   enum
   {
      MEASUREMENTS  = SolverParamCount,
      SOLVEFORS,
      ABSOLUTETOLERANCE,
      RELATIVETOLERANCE,
      PROPAGATOR,
      SHOW_RESIDUALS,
      ADD_RESIDUAL_PLOT,
      MAX_RESIDUAL_MULTIPLIER,
      CONSTANT_MULTIPLIER,
      ADDITIVE_CONSTANT,
      USE_RMSP,
      RESET_BEST_RMS,
      DATA_FILTERS,
      CONVERGENT_STATUS,
      EstimatorParamCount
   };

   /// String identifiers for the parameters
   static const std::string    PARAMETER_TEXT[EstimatorParamCount -
                                              SolverParamCount];
   /// Types of the parameters
   static const Gmat::ParameterType
                               PARAMETER_TYPE[EstimatorParamCount -
                                              SolverParamCount];

   virtual Integer         TestForConvergence(std::string &reason);

   GmatTime                ConvertToGmatTimeEpoch(const std::string &theEpoch, 
                                                  const std::string &theFormat);

   virtual void            BuildResidualPlot(const std::string &plotName,
                              const StringArray &measurementNames);
   virtual void            PlotResiduals();
   virtual void            EnhancePlot();


   virtual Integer         SetSolverResults(Real*, const std::string&,
                                            const std::string&);
   virtual void            SetResultValue(Integer, Real, const std::string&);

   virtual ObservationData*
                           FilteringData(ObservationData* obsData, Integer obDataId, Integer& filterIndex);

   virtual Integer         SumAcceptedRecords(Integer key); 
   virtual Real            SumAllResidual(Integer key);
   virtual Real            SumAllResidualDeviationSqr(Integer key, Real ave);
   virtual Real            SumAllWeightedResidualSqr(Integer key);

   /// Estimation status contains all status of an estimation
   enum EstimationStatus
   {
      UNKNOWN,
      ABSOLUTETOL_CONVERGED,
      RELATIVETOL_CONVERGED,
      ABS_AND_REL_TOL_CONVERGED,
      MAX_CONSECUTIVE_DIVERGED,
      MAX_ITERATIONS_DIVERGED,
      CONVERGING,
      DIVERGING,
   };

private:
   bool      delayInitialization;
};

#endif /* Estimator_hpp */
