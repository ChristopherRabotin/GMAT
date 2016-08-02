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
#include "DataWriter.hpp"
#include "WriterData.hpp"
#include "DataBucket.hpp"

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

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);

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
   Real                    resetBestResidualRMS;
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

   /// The .mat DataWriter object used to write data for MATLAB
   DataWriter              *matWriter;
   /// Flag indicating is the .mat file should be written
   bool                    writeMatFile;
   /// .mat data file name
   std::string             matFileName;
   /// Data container used during accumulation
   DataBucket              matData;

   // Indexing for the .mat data elements
   /// Iteration number index
   Integer                 matIterationIndex;
   /// Index of the participants list in the .mat data
   Integer                 matPartIndex;
   /// Index of the participants list in the .mat data
   Integer                 matTypeIndex;
   /// Index of the TAI Mod Julian epoch data in the .mat data
   Integer                 matEpochIndex;
   /// Index of the observation data list in the .mat data
   Integer                 matObsIndex;
   /// Index of the calculated data in the .mat data
   Integer                 matCalcIndex;
   /// Index of the O-C data in the .mat data
   Integer                 matOmcIndex;
   /// Index of the elevation for the obs
   Integer                 matElevationIndex;
   /// TAI Gregorian epoch index
   Integer                 matGregorianIndex;
   ///  Observation edit flag index
   Integer                 matObsEditFlagIndex;

   // Extra entries
   Integer                 matFrequencyIndex;
   Integer                 matFreqBandIndex;
   Integer                 matDoppCountIndex;

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

   /// particicpants column lenght. It is used for writing report file
   Integer                pcolumnLen;

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

   // Statistics information for sigma edited records
   IntegerArray sumSERecords;               // total all sigma edited records
   RealArray    sumSEResidual;              // sum of all O-C of all sigma edited records
   RealArray    sumSEResidualSquare;        // sum of all (O-C)^2 of  all sigma edited records
   RealArray    sumSEWeightResidualSquare;  // sum of all [W*(O-C)]^2 of all sigma edited records

   std::stringstream textFile0;
   std::stringstream textFile1;
   std::stringstream textFile1_1;
   std::stringstream textFile2;
   std::stringstream textFile3;
   std::stringstream textFile4;

   /// Parameter IDs for the BatchEstimators
   enum
   {
      ESTIMATION_EPOCH_FORMAT = EstimatorParamCount,
      ESTIMATION_EPOCH,
//      USE_PRIORI_ESTIMATE,
      USE_INITIAL_COVARIANCE,
      INVERSION_ALGORITHM,
      MAX_CONSECUTIVE_DIVERGENCES,
      MATLAB_OUTPUT_FILENAME,
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

   bool                    WriteMatData();

private:

   void                   WriteReportFileHeaderPart1();
   void                   WriteReportFileHeaderPart2();
   void                   WriteReportFileHeaderPart2b();
   void                   WriteReportFileHeaderPart3();
   void                   WriteReportFileHeaderPart4_1();
   void                   WriteReportFileHeaderPart4_2();
   void                   WriteReportFileHeaderPart4_3();
   void                   WriteReportFileHeaderPart4();
   void                   WriteReportFileHeaderPart5();
   void                   WriteReportFileHeaderPart6();
   void                   WriteReportFileHeader();

   void                   WriteIterationHeader();
   void                   WritePageHeader();
   
   void                   WriteIterationSummaryPart1(Solver::SolverState sState);
   void                   WriteIterationSummaryPart2(Solver::SolverState sState);
   void                   WriteIterationSummaryPart3(Solver::SolverState sState);
   void                   WriteIterationSummaryPart4(Solver::SolverState sState);
   void                   WriteReportFileSummary(Solver::SolverState sState);
   
   std::string            GetElementFullName(ListItem* infor, bool isInternalCS) const;
   std::string            GetElementUnit(ListItem* infor) const;
   std::string            GetUnit(std::string type);
   Integer                GetElementPrecision(std::string unit) const;
   Rmatrix                CovarianceConvertionMatrix(std::map<GmatBase*, Rvector6> stateMap);

   std::map<GmatBase*, Rvector6> 
                          CalculateCartesianStateMap(const std::vector<ListItem*> *map, GmatState state);
   std::map<GmatBase*, Rvector6> 
                          CalculateKeplerianStateMap(const std::vector<ListItem*> *map, GmatState state);
   Rmatrix66              CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state);
   std::map<GmatBase*, RealArray>
                          CalculateAncillaryElements(const std::vector<ListItem*> *map, GmatState state);


   std::string            GetFileCreateTime(std::string fileName);
   std::string            CTime(const time_t* time);
   std::string            GetGMATBuildDate();
   std::string            GetDayOfWeek(Integer day, Integer month, Integer year);
   std::string            GetOperatingSystemName();
   std::string            GetOperatingSystemVersion();
   std::string            GetHostName();
   std::string            GetUserID();
};

#endif /* BatchEstimator_hpp */
