//$Id: BatchEstimator.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         BatchEstimator
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
 * Definition of the BatchEstimator class
 */
//------------------------------------------------------------------------------


#ifndef BatchEstimator_hpp
#define BatchEstimator_hpp

#include "Estimator.hpp"
#include "PropSetup.hpp"
#include "MeasurementManager.hpp"

#include "OwnedPlot.hpp"

/**
 * Implementation of a standard batch estimation state machine
 *
 * This class provides a batch estimation state machine that follows a typical
 * batch estimation process.  Derived classes override specific methods to
 * implement the math required for their algorithm.  Every derived estimator
 * must implement the Accumulate and Estimate methods.  The other methods called
 * in the finite state machine provide default implementation that can be
 * overridden as  needed.
 *
 * BatchEstimator is an abstract class; a derived class is needed to instantiate
 * a BatchEstimator
 */
class ESTIMATION_API BatchEstimator : public Estimator
{
public:
   BatchEstimator(const std::string &type, const std::string &name);
   virtual ~BatchEstimator();
   BatchEstimator(const BatchEstimator& est);
   BatchEstimator& operator=(const BatchEstimator& est);

   virtual bool         Initialize();
   virtual SolverState  AdvanceState();
   virtual bool         Finalize();

   // methods overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id,
                                         const std::string &value);
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label,
                                          const std::string &value);

   virtual const StringArray& GetPropertyEnumStrings(const Integer id) const;

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

protected:
   /// Time system used to specify the estimation epoch
   std::string             estEpochFormat;
   /// The estimation epoch.  "FromParticipants" means use the spacecraft epoch.
   std::string             estEpoch;

   /// RMS residual value from the previous pass through the data
   Real                    oldResidualRMS;
   /// RMS residual value from the current pass through the data
   Real                    newResidualRMS;
   /// The best RMS residual
   Real                    bestResidualRMS;
   /// Predicted RMS residual
   Real predictedRMS;
   /// Number consecutive iterations diverging
   Integer numDivIterations;
   /// Flag to indicate weightedRMS or predictedRMS
   bool chooseRMSP;

   /// Flag set when an a priori estimate is available
   bool                    useApriori;
   /// The most recently computed state vector changes
   RealArray               dx;
   /// The weighting matrix used when accumulating data
   Rmatrix                 weights;
   /// Flag used to indicate propagation to estimation epoch is executing
   bool                    advanceToEstimationEpoch;

//   /// Estimation status
//   Integer                 estimationStatus;         // This variable is moved to Estimator class

   // String to show reason of convergence
   std::string convergenceReason;
   /// Buffer of the participants for the outer batch loop
   ObjectArray             outerLoopBuffer;
   /// Inversion algorithm used 
   std::string             inversionType;

   /// Maximum consecutive divergences
   Integer               maxConsDivergences;

   /// particicpants column lenght. It is used for writing report file               // made changes by TUAN NGUYEN
   Integer                pcolumnLen;                                                // made changes by TUAN NGUYEN

   /// Variables used for statistics calculation
   std::map<std::string, std::map<std::string, Real> > statisticsTable;       // this table is for groundstation and measurement type
   std::map<std::string, std::map<std::string, Real> > statisticsTable1;      // this table is for measurement type only

   // Statistics information for all combination of station and measurement type
   StringArray  stationAndType;          // combination of station and type
   StringArray  stationsList;            // station
   StringArray  measTypesList;           // measurement type
   IntegerArray sumAllRecords;           // total number of records
   IntegerArray sumAcceptRecords;        // total all accepted records
   RealArray    sumResidual;             // sum of all O-C of accepted records
   RealArray    sumResidualSquare;       // sum of all (O-C)^2 of accepted records
   RealArray    sumWeightResidualSquare; // sum of all [W*(O-C)]^2 of accepted records

   // Statisrics information for sigma edited records
   IntegerArray sumSERecords;               // total all sigma edited records
   RealArray    sumSEResidual;              // sum of all O-C of all sigma edited records
   RealArray    sumSEResidualSquare;        // sum of all (O-C)^2 of  all sigma edited records
   RealArray    sumSEWeightResidualSquare;  // sum of all [W*(O-C)]^2 of all sigma edited records

   std::stringstream textFile0;
   std::stringstream textFile1;
   std::stringstream textFile2;
   std::stringstream textFile3;
   std::stringstream textFile4;

   /// Parameter IDs for the BatchEstimators
   enum
   {
      ESTIMATION_EPOCH_FORMAT = EstimatorParamCount,
      ESTIMATION_EPOCH,
//     USE_PRIORI_ESTIMATE,
      INVERSION_ALGORITHM,
      MAX_CONSECUTIVE_DIVERGENCES,
      BatchEstimatorParamCount,
   };

   /// Strings describing the BatchEstimator parameters
   static const std::string
                           PARAMETER_TEXT[BatchEstimatorParamCount -
                                              EstimatorParamCount];
   /// Types of the BatchEstimator parameters
   static const Gmat::ParameterType
                           PARAMETER_TYPE[BatchEstimatorParamCount -
                                              EstimatorParamCount];

   virtual void            CompleteInitialization();
   virtual void            FindTimeStep();
   virtual void            CalculateData();
   virtual void            ProcessEvent();
   virtual void            CheckCompletion();
   virtual void            RunComplete();

   // Abstract methods defined in derived classes
   /// Abstract method that implements accumulation in derived classes
   virtual void            Accumulate() = 0;
   /// Abstract method that performs the estimation in derived classes
   virtual void            Estimate() = 0;

   virtual Integer         TestForConvergence(std::string &reason);
   virtual void            WriteToTextFile(Solver::SolverState state =
                                              Solver::UNDEFINED_STATE);

   // progress string for reporting
   virtual std::string    GetProgressString();

   Integer SchurInvert(Real *SUM1, Integer array_size);
   Integer CholeskyInvert(Real *SUM1, Integer array_size);

   virtual bool            DataFilter();

private:
//   bool                    IsReuseableType(const std::string& value);
   
   void                   WriteScript();                                                                            // made changes by TUAN NGUYEN
   void                   WriteHeader();
   void                   WriteSummary(Solver::SolverState sState);
   void                   WriteConclusion();

   void                   WriteReportFileHeaderPart1();                               // made changes by TUAN NGUYEN
   void                   WriteReportFileHeaderPart2();                               // made changes by TUAN NGUYEN
   void                   WriteReportFileHeaderPart3();                               // made changes by TUAN NGUYEN
   void                   WriteReportFileHeaderPart4_1();                             // made changes by TUAN NGUYEN
   void                   WriteReportFileHeaderPart4_2();                             // made changes by TUAN NGUYEN
   void                   WriteReportFileHeaderPart4_3();                             // made changes by TUAN NGUYEN
   void                   WriteReportFileHeaderPart4();                               // made changes by TUAN NGUYEN
   void                   WriteReportFileHeaderPart5();                               // made changes by TUAN NGUYEN
   void                   WriteReportFileHeaderPart6();                               // made changes by TUAN NGUYEN
   void                   WriteReportFileHeader();                                    // made changes by TUAN NGUYEN

   void                   WriteIterationHeader();                                     // made changes by TUAN NGUYEN
   void                   WritePageHeader();                                          // made changes by TUAN NGUYEN
   
   void                   WriteIterationSummaryPart1(Solver::SolverState sState);     // made changes by TUAN NGUYEN
   void                   WriteIterationSummaryPart2(Solver::SolverState sState);     // made changes by TUAN NGUYEN
   void                   WriteIterationSummaryPart3(Solver::SolverState sState);     // made changes by TUAN NGUYEN
   void                   WriteIterationSummaryPart4(Solver::SolverState sState);     // made changes by TUAN NGUYEN
   void                   WriteReportFileSummary(Solver::SolverState sState);         // made changes by TUAN NGUYEN
   
   std::string            GetElementFullName(ListItem* infor, bool isInternalCS) const;
   std::string            GetElementUnit(ListItem* infor) const;                          // made changes by TUAN NGUYEN
   std::string            GetUnit(std::string type);                                      // made changes by TUAN NGUYEN
   Integer                GetElementPrecision(std::string unit) const;                    // made changes by TUAN NGUYEN
   Rmatrix                CovarianceConvertionMatrix(std::map<GmatBase*, Rvector6> stateMap);                       // made changes by TUAN NGUYEN

   std::map<GmatBase*, Rvector6> 
                          CalculateCartesianStateMap(const std::vector<ListItem*> *map, GmatState state);           // made changes by TUAN NGUYEN
   std::map<GmatBase*, Rvector6> 
                          CalculateKeplerianStateMap(const std::vector<ListItem*> *map, GmatState state);           // made changes by TUAN NGUYEN
   Rmatrix66              CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state);      // made changes by TUAN NGUYEN


   std::string            GetFileCreateTime(std::string fileName);
   std::string            CTime(const time_t* time);
   std::string            GetOperatingSystemName();
   std::string            GetOperatingSystemVersion();
   std::string            GetHostName();
   std::string            GetUserID();
};

#endif /* BatchEstimator_hpp */
