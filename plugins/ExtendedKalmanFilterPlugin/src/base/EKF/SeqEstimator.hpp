//$Id: SeqEstimator.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         SeqEstimator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
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
// Created: 2009/09/03
//
/**
 * Base class for sequential estimation
 */
//------------------------------------------------------------------------------


#ifndef SeqEstimator_hpp
#define SeqEstimator_hpp

#include "kalman_defs.hpp"
#include "Estimator.hpp"

struct MeasNoiseType   // EKF Mod 12/16
{
public:
   static const std::string Hardware;
   static const std::string Filter;
};

struct ProcessNoiseType  // EKF Mod 12/16
{
public:
   static const std::string None;
   static const std::string Constant;
   static const std::string BasicTime;
   static const std::string SingerModel;
   static const std::string SNC;
   //static const std::string DMC; // not supported yet...
};

/**
 * Provides core functionality used in sequential estimation.
 *
 * This class provides the data structures and interfaces used when performing
 * sequential estimation.  The class includes an implementation of a finite
 * state machine that represents the typical state transitions encountered when
 * performing sequential estimation in, for example, an extended Kalman filter.
 */
class KALMAN_API SeqEstimator: public Estimator
{
public:
   SeqEstimator(const std::string &type, const std::string &name);
   virtual ~SeqEstimator();
   SeqEstimator(const SeqEstimator &se);
   SeqEstimator&    operator=(const SeqEstimator &se);

   virtual bool         Initialize();
   virtual SolverState  AdvanceState();
   virtual bool         Finalize();

   // methods overridden from GmatBase
// EKF mod 12/16
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);

   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);

   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);

   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);

   virtual const Rvector&
                        GetRvectorParameter(const Integer id) const;
   virtual const Rvector&
                        SetRvectorParameter(const Integer id,
                                            const Rvector &value);

   virtual const Rvector&
                        GetRvectorParameter(const std::string &label) const;
   virtual const Rvector&
                        SetRvectorParameter(const std::string &label,
                                            const Rvector &value);

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

   virtual const StringArray& GetPropertyEnumStrings(const Integer id) const;
// End EKF mod

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

protected:
// EKF mod 12/16
   static const UnsignedInt truthStateSize;
   static const UnsignedInt stdColLen;
   static const UnsignedInt minPartSize;
   static const UnsignedInt strSizeMin;
   static const double      defaultMeasSigma;

   UnsignedInt pcolumnLen;

   Rvector     defaultMeasCovarianceDiag;
   Rmatrix     defaultMeasCovariance;

   /// Time system used to specify the estimation epoch
   std::string             estEpochFormat;
   /// The estimation epoch.  "FromParticipants" means use the spacecraft epoch.
   std::string             estEpoch;
// End EKF mod

   /// Changes in the state vector
   Rvector                 dx;
   /// Array used to track the one-sigma deviation for each measurement point
   RealArray               sigma;

   Rmatrix                 H; // measurement matrix, EKF mod 12/16
   Rmatrix                 I; // identity matrix of size stateSize, EKF mod 12/16

   /// The measurement error covariance
   Covariance              *measCovariance;

// EKF mod 12/16   
/// Size of the measurement
   UnsignedInt             measSize;
   /// meas count
   UnsignedInt             measCount;

   /// Meas noise
   std::string              measNoiseType;
   Rvector                  measNoiseSigma;

   // Process noise
   std::string              processNoiseType;
   Rvector                  processNoiseConstantVector;
   Real                     processPosNoiseTimeRate;
   Real                     processVelNoiseTimeRate;
   Real                     processSingerTimeConst;
   Real                     processSingerSigma;

   /// previous measurement epoch
   GmatEpoch               prevObsEpoch;

   /// Parameter IDs for the BatchEstimators
   enum
   {
      MEAS_NOISE_TYPE = EstimatorParamCount,
      MEAS_NOISE_SIGMA,
      PROCESS_NOISE_TYPE,
      PROCESS_NOISE_CONSTANT_VECTOR,
      PROCESS_POS_NOISE_TIME_RATE,
      PROCESS_VEL_NOISE_TIME_RATE,
      PROCESS_SINGER_TIME_CONST,
      PROCESS_SINGER_SIGMA,
      SeqEstimatorParamCount
   };

   /// Strings describing the BatchEstimator parameters
   static const std::string
                           PARAMETER_TEXT[SeqEstimatorParamCount -
                                              EstimatorParamCount];
   /// Types of the BatchEstimator parameters
   static const Gmat::ParameterType
                           PARAMETER_TYPE[SeqEstimatorParamCount -
                                              EstimatorParamCount];
// End EKF mod

   // Methods used in the finite state machine
   virtual void            CompleteInitialization();
   virtual void            FindTimeStep();
   virtual void            CalculateData();
   virtual void            ProcessEvent();
   virtual void            CheckCompletion();
   virtual void            RunComplete();

   /**
    *  Abstract method that is overridden in derived classes to perform
    *  sequential estimation.
    */
   virtual void            Estimate() = 0;

   virtual void            PrepareForStep();

//   virtual bool            TestForConvergence(std::string &reason);
   virtual void            WriteToTextFile(Solver::SolverState state =
                                                Solver::UNDEFINED_STATE);
   virtual void            UpdateReportText() = 0;
   virtual void            UpdateStateReportText(std::stringstream& sLine) = 0;

   // Additional methods used by the EKF
   virtual void            GetEstimationState(GmatState& outputState);
   bool                    ConvertToPartCoordSys(ListItem* infor, Real epoch,
                                 Real inputStateElement,
                                 Real* outputStateElement);

   std::string             GetElementFullName(ListItem* infor, bool isInternalCS) const;
   Rmatrix                 CovarianceConvertionMatrix(std::map<GmatBase*, Rvector6> stateMap);

   std::map<GmatBase*, Rvector6>
                           CalculateCartesianStateMap(const std::vector<ListItem*> *map, GmatState state);
   std::map<GmatBase*, Rvector6>
                           CalculateKeplerianStateMap(const std::vector<ListItem*> *map, GmatState state);
   Rmatrix66               CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state);

   virtual std::string     GetProgressString();

// EKF mod 12/16
private:
   void                   WriteScript();
   void                   WriteHeader();
   void                   WriteSummary(Solver::SolverState sState);
   void                   WriteConclusion();
// End EKF mod
};

#endif /* SeqEstimator_hpp */
