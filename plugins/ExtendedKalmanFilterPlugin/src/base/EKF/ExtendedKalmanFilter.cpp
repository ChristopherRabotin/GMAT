//$Id: ExtendedKalmanFilter.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         ExtendedKalmanFilter
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
// Process Noise added by:
// Author: Jamie J. LaPointe, University of Arizona
// Modifed: 2016/05/09
//
/**
 * A simple extended Kalman filter
 */
//------------------------------------------------------------------------------

#include "ExtendedKalmanFilter.hpp"
#include "EstimatorException.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include <cmath>
#include <limits>


//#define DEBUG_ESTIMATION
//#define DEBUG_JOSEPH

//------------------------------------------------------------------------------
// ExtendedKalmanFilter(const std::string name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the new instance
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilter::ExtendedKalmanFilter(const std::string name) :
   SeqEstimator  ("ExtendedKalmanFilter", name),
   dt(0.0),
   isFirst(true),
   prevState(6,1),
   prevMeas(2),
   debugMeas(2,1),
   dState(6,1),
   dMeas(2),
   calculatedMeas(0),
   currentObs(0),
   ocDiff(0.0)
{
   objectTypeNames.push_back("ExtendedKalmanFilter");
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage(" EKF default constructor: stateSize = %o, "
            "measSize = %o", stateSize, measSize);
   #endif
}

//------------------------------------------------------------------------------
// ~ExtendedKalmanFilter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilter::~ExtendedKalmanFilter()
{
   if (measNoiseType == MeasNoiseType::Filter)
   {
      if (measCovariance)
      {
         delete measCovariance;
         measCovariance = NULL;
      }
   }
}


//------------------------------------------------------------------------------
// ExtendedKalmanFilter(const ExtendedKalmanFilter & ekf) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ekf The instance used to configure this instance
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilter::ExtendedKalmanFilter(const ExtendedKalmanFilter & ekf) :
   SeqEstimator  (ekf),
   dt(0.0),
   isFirst(true),
   prevState(6,1),
   prevMeas(2),
   debugMeas(2,1),
   dState(6,1),
   dMeas(2),
   calculatedMeas(0),
   currentObs(0),
   ocDiff(0.0)
{
}


//------------------------------------------------------------------------------
// ExtendedKalmanFilter& operator=(const ExtendedKalmanFilter &ekf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ekf The instance used to configure this instance
 *
 * @return this instance, configured to match ekf.
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilter& ExtendedKalmanFilter::operator=(const ExtendedKalmanFilter &ekf)
{
   if (this != &ekf)
   {
      SeqEstimator::operator=(ekf);
      isFirst = ekf.isFirst;
      prevState = ekf.prevState;
      prevMeas = ekf.prevMeas;
      debugMeas = ekf.debugMeas;
      dState = ekf.dState;
      dMeas = ekf.dMeas;
      measSize = ekf.measSize;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Object cloner
 *
 * @return A clone of this object
 */
//------------------------------------------------------------------------------
GmatBase* ExtendedKalmanFilter::Clone() const
{
   return new ExtendedKalmanFilter(*this);
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
void  ExtendedKalmanFilter::Copy(const GmatBase* orig)
{
   operator=(*((ExtendedKalmanFilter*)(orig)));
}



//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void CompleteInitialization()
//------------------------------------------------------------------------------
/**
 * Prepares the estimator for a run
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::CompleteInitialization()
{
   SeqEstimator::CompleteInitialization();
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage(" EKF CompleteInitialization: stateSize = %o, \n"
            "measSize = %o", stateSize, measSize);
   #endif

   Integer size = stateCovariance->GetDimension();
   if (size != (Integer)stateSize)
   {
      throw EstimatorException("In ExtendedKalmanFilter::Estimate(), the "
            "covariance matrix is not sized correctly!!!");
   }

   pBar.SetSize(stateSize, stateSize);
   Q.SetSize(stateSize, stateSize);
   H.SetSize(measSize, stateSize);
   yi.SetSize(measSize);
   I = Rmatrix::Identity(stateSize);
   kalman.SetSize(stateSize, measSize);
   defaultMeasCovarianceDiag.SetSize(measSize);
   innovationCov.SetSize(measSize,measSize);
   innovationCovInv.SetSize(measSize,measSize);
   dt = 0.0;

   for (Integer i = 0; i < measSize; ++i)
   {
      defaultMeasCovarianceDiag(i) = defaultMeasSigma*defaultMeasSigma;
   }
   defaultMeasCovariance = Rmatrix::Diagonal(measSize, defaultMeasCovarianceDiag);

   if (measNoiseType == MeasNoiseType::Filter)
   {
      Rmatrix measData(measSize, measSize);
      for (Integer i = 0; i < measSize; ++i)
      {
         measData(i,i) = measNoiseSigma(i)*measNoiseSigma(i);
      }
      measCovariance = new Covariance(NULL);
      measCovariance->SetDimension(measSize);
      measCovariance->FillMatrix(measData, false);
   }

   currentObs =  measManager.GetObsData();
   prevObsEpoch = currentEpoch;
}


//------------------------------------------------------------------------------
// void Estimate()
//------------------------------------------------------------------------------
/**
 * Implements the time update, compute, and orbit updates for the estimator
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::Estimate()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("\n\n--ExtendedKalmanFilter::Estimate----\n");
      MessageInterface::ShowMessage("Current covariance:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12le", stateCovariance->GetCovariance()->GetElement(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");

      MessageInterface::ShowMessage("Current stm:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", (*stm)(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");

      MessageInterface::ShowMessage("Current State: [ ");
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage(" %.12lf ", (*estimationState)[i]);
      MessageInterface::ShowMessage("\n");
   #endif

   // setup the measurement objects for the rest of this frame of data to use
   SetupMeas();

   if (!calculatedMeas->isFeasible && calculatedMeas->unfeasibleReason[0] == 'B')
   {
      AdvanceEpoch();
      return;
   }

   // Update the Process Noise
   UpdateProcessNoise();

   // Perform the time update of the covariances, phi P phi^T, and the state
   TimeUpdate();

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Time updated matrix \\bar P:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12le", pBar(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   // Construct the O-C data and H tilde
   ComputeObs();

   // <debug>
   Rmatrix state(6, 1);
   for (UnsignedInt i = 0; i < 6; ++i)
   {
      state(i, 0) = (*estimationState)[i];
   }
   if (!isFirst)
   {
//MessageInterface::ShowMessage("   Value size %d, prev size %d\n", calculatedMeas->value.size(), prevMeas.size());
      for (UnsignedInt i = 0; i < calculatedMeas->value.size(); ++i)
      {
         dMeas[i] = calculatedMeas->value[i] - prevMeas[i];
      }
      dState = state - prevState;
//      debugMeas = H * dState;
   }

   isFirst = false;
   for (UnsignedInt i = 0; i < 6; ++i)
   {
      prevState(i, 0) = (*estimationState)[i];
   }

   for (UnsignedInt i = 0; i < 2; ++i)
   {
      prevMeas[i] = calculatedMeas->value[i];
   }

   // </debug>

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("hTilde:\n");
      for (UnsignedInt i = 0; i < measSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", hTilde[i][j]);
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   // Then the Kalman gain
   ComputeGain();

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("The Kalman gain is: \n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < measSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", kalman(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   // Finally, update everything
   UpdateElements();

   // Plot residuals if set
   if (showAllResiduals)
   {
      PlotResiduals();
   }

   // Convert current estimation state from GMAT internal coordinate system to participants' coordinate system
   GetEstimationStateForReport(currentSolveForState);
   UpdateReportText();


   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updated covariance:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12le", stateCovariance->GetCovariance()->GetElement(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");

      MessageInterface::ShowMessage("Updated State: [ ");
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage(" %.12lf ", (*estimationState)[i]);
      MessageInterface::ShowMessage("\n\n---------------------\n");
   #endif

   ReportProgress();

   AdvanceEpoch();
}

//------------------------------------------------------------------------------
// void SetupMeas()
//------------------------------------------------------------------------------
/**
 * This sets up the measurement information for others to use later
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::SetupMeas()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Performing measurement setup\n");
   #endif

   modelsToAccess = measManager.GetValidMeasurementList();
   currentObs =  measManager.GetObsData();

   // Currently assuming uniqueness in models to access
   measCount = measManager.Calculate(modelsToAccess[0], true);
   calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);
}

//------------------------------------------------------------------------------
// void UpdateProcessNoise()
//------------------------------------------------------------------------------
/**
 * This updates the process noise matrix, Q
 *
 * Author: Jamie LaPointe
 * Org:    University of Arizona - Department of Aerospace and Mechanical Engineering
 * Date:   9 May 2016
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::UpdateProcessNoise()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Performing process noise update\n");
   #endif

   // TODO implement SNC and DMC models

   static bool isFirst = true;
   dt = (calculatedMeas->epoch - prevObsEpoch)*GmatTimeConstants::SECS_PER_DAY;

   if (processNoiseType == ProcessNoiseType::Constant)
   {
      Q(0,0) = processNoiseConstantVector(0);
      Q(1,1) = processNoiseConstantVector(1);
      Q(2,2) = processNoiseConstantVector(2);
      Q(3,3) = processNoiseConstantVector(3);
      Q(4,4) = processNoiseConstantVector(4);
      Q(5,5) = processNoiseConstantVector(5);
   }
   else if (processNoiseType == ProcessNoiseType::BasicTime)
   {
      Real positionProcessNoiseSigma = processPosNoiseTimeRate * dt;
      Real velocityProcessNoiseSigma = processVelNoiseTimeRate * dt;
      Q(0,0) = Q(1,1) = Q(2,2) = positionProcessNoiseSigma * positionProcessNoiseSigma;
      Q(3,3) = Q(4,4) = Q(5,5) = velocityProcessNoiseSigma * velocityProcessNoiseSigma;
   }
   else if (processNoiseType == ProcessNoiseType::SingerModel)
   {
      // This algorithm is from Design & Analysis of Modern Tracking Systems by
      // Blackman, Samuel and Popoli, Robert. 1999. ISBN 1-58053-006-0
      // pp. 202 & 276
      // target dynamics/perturbations where the unmodeled accelerations are taken to be
      // process white noise
      Real s = 2 * processSingerSigma * processSingerSigma * processSingerTimeConst;
      Real dt2 = (dt * dt)/2.0;
      Real dt3 = (dt2 * dt)/3.0;

      Rmatrix qTemp(2,2, dt3, dt2,
                         dt2, dt);

      // setup the block diagaonal matrix for the process noise matrix, Q
      Q(0,0) = Q(1,1) = Q(2,2) = qTemp(0,0) * s;
      Q(0,3) = Q(1,4) = Q(2,5) = qTemp(0,1) * s;
      Q(3,0) = Q(4,1) = Q(5,2) = qTemp(1,0) * s;
      Q(3,3) = Q(4,4) = Q(5,5) = qTemp(1,1) * s;
   }
   else if (processNoiseType == ProcessNoiseType::SNC)
   {
      // TODO Add SNC
   }
   else // ProcessNoiseType::None:
   {
      // do nothing
   }
}

//------------------------------------------------------------------------------
// void TimeUpdate()
//------------------------------------------------------------------------------
/**
 * Performs the time update of the state error covariance
 *
 * This method applies equation 4.7.1(b), and then symmetrizes the resulting
 * time updated covariance, pBar.
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::TimeUpdate()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Performing time update\n");
   #endif

   // phi * P * phi^T + Q

   pBar = ((*stm) * (*(stateCovariance->GetCovariance())) * (stm->Transpose())) + Q;

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Q = \n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < measSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", Q(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   // make it symmetric!
   Symmetrize(pBar);
}


//------------------------------------------------------------------------------
// void ComputeObs()
//------------------------------------------------------------------------------
/**
 * Computes the measurement residuals and the H-tilde matrix
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::ComputeObs()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Computing obs and hTilde\n");
   #endif
   // Compute the O-C, Htilde, and Kalman gain
   std::vector<RealArray> stateDeriv;
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();
   hTilde.clear();

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("StateMap size is %d\n", stateMap->size());
   #endif

   if (calculatedMeas == NULL || measCount == 0)
   {
      throw EstimatorException("No measurement was calculated!");
   }

   UnsignedInt rowCount;
   RealArray hTrow;
   hTrow.assign(stateSize, 0.0);
   rowCount = calculatedMeas->value.size();
   measSize = currentObs->value.size();

   #ifdef DEBUG_ESTIMATION
      if (rowCount != measSize)
         MessageInterface::ShowMessage("Mismatch between rowCount (%d) and "
               "measSize(%d)\n", rowCount, measSize);
   #endif

   for (UnsignedInt i = 0; i < rowCount; ++i)
   {
      hTilde.push_back(hTrow);
   }

   // Now walk the state vector and get elements of H-tilde for each piece
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      if ((*stateMap)[i]->subelement == 1)
      {
         stateDeriv = measManager.CalculateDerivatives(
               (*stateMap)[i]->object, (*stateMap)[i]->elementID,
               modelsToAccess[0]);

         // Fill in the corresponding elements of hTilde
         for (UnsignedInt j = 0; j < rowCount; ++j)
         {
            for (Integer k = 0; k < (*stateMap)[i]->length; ++k)
            {
               hTilde[j][i+k] = stateDeriv[j][k];
               H(j,i+k) = hTilde[j][i+k];
            }
         }
      }
   }

   for (UnsignedInt k = 0; k < measSize; ++k)
   {
      ocDiff = currentObs->value[k] - calculatedMeas->value[k];
      measurementEpochs.push_back(currentObs->epoch);
      measurementResiduals.push_back(ocDiff);
      measurementResidualID.push_back(calculatedMeas->uniqueID);
      yi(k) = ocDiff;
      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("*** Current O-C = %.12lf\n", ocDiff);
      #endif
   }

   if (measNoiseType == MeasNoiseType::Hardware)
   {
      if (currentObs->noiseCovariance == NULL)
      {
         measCovariance = calculatedMeas->covariance;
      }
      else
      {
         measCovariance = currentObs->noiseCovariance;
      }
   }
}


//------------------------------------------------------------------------------
// void ComputeGain()
//------------------------------------------------------------------------------
/**
 * Computes the Kalman gain
 *
 * The error estimates used for error bars on the residuals plots are calculated
 * as
 *
 *    sigma = sqrt(H P H' + R)
 *
 * Since the argument of the square root is calculated as part of the Kalman
 * gain calculation, this value is also stored in this method
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::ComputeGain()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Computing Kalman Gain\n");
   #endif

   #ifdef DEBUG_ESTIMATION_DETAILS
      MessageInterface::ShowMessage("Calculating P H^T\n");
   #endif

   // Compute the Innovation (residual) covariance matrix
   // S = H*P*H^T + R
   innovationCov = (H * pBar * H.Transpose()) + (*(measCovariance->GetCovariance()));
   innovationCovInv = innovationCov.Inverse(std::numeric_limits<double>::epsilon());

   #ifdef DEBUG_ESTIMATION
	   MessageInterface::ShowMessage("H = \n");
	   for (UnsignedInt i = 0; i < measSize; ++i)
	   {
		  for (UnsignedInt j = 0; j < stateSize; ++j)
		  {
			 MessageInterface::ShowMessage("  %.12le", H(i,j));
		  }
		  MessageInterface::ShowMessage("\n");
	   }
	   MessageInterface::ShowMessage("pBar = \n");
	   for (UnsignedInt i = 0; i < stateSize; ++i)
	   {
		  for (UnsignedInt j = 0; j < stateSize; ++j)
		  {
			 MessageInterface::ShowMessage("  %.12le", pBar(i,j));
		  }
		  MessageInterface::ShowMessage("\n");
	   }
	   MessageInterface::ShowMessage("R = \n");
	   for (UnsignedInt i = 0; i < measSize; ++i)
	   {
		  for (UnsignedInt j = 0; j < measSize; ++j)
		  {
			 MessageInterface::ShowMessage("  %.12le", (*(measCovariance->GetCovariance()))(i,j));
		  }
		  MessageInterface::ShowMessage("\n");
	   }
      MessageInterface::ShowMessage("Gain denominator = \n");
      for (UnsignedInt i = 0; i < measSize; ++i)
      {
         for (UnsignedInt j = 0; j < measSize; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", innovationCov(i,j));
         }
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("innovationCovInv = \n");
      for (UnsignedInt i = 0; i < measSize; ++i)
      {
         for (UnsignedInt j = 0; j < measSize; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", innovationCovInv(i,j));
         }
         MessageInterface::ShowMessage("\n");
      }
   #endif

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Calculating the Kalman gain\n");
   #endif

   // compute the Kalman Gain
   // K = P * H^T * S^{-1}  OR  K = P * H^T * (H * P * H^T + R)^{-1}
   kalman = pBar * H.Transpose() * innovationCovInv;
}


//------------------------------------------------------------------------------
// void UpdateElements()
//------------------------------------------------------------------------------
/**
 * Updates the estimation state and covariance matrix
 *
 * Programmers can select the covariance update method at the end of this
 * method.  The resulting covariance is symmetrized before returning.
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::UpdateElements()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updating elements\n");
   #endif

   dx = kalman * yi;

   // Update the state, covariances, and so forth
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      (*estimationState)[i] += dx[i];
   }

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Calculated state change: [");
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage(" %.12lf ", dx[i]);
      MessageInterface::ShowMessage("\n");
   #endif

   // Select the method used to update the covariance here:
   // UpdateCovarianceSimple();
   UpdateCovarianceJoseph();

   Symmetrize(*stateCovariance);
}

//------------------------------------------------------------------------------
// void Symmetrize(Rmatrix& mat)
//------------------------------------------------------------------------------
/**
 * Symmetrizes a covariance matrix
 *
 * @param mat The covariance matrix that is symmetrized
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::Symmetrize(Covariance& mat)
{
   Integer size = mat.GetDimension();

   for (Integer i = 0; i < size; ++i)
   {
      for (Integer j = i+1; j < size; ++j)
      {
         mat(i,j) = 0.5 * (mat(i,j) + mat(j,i));
         mat(j,i) = mat(i,j);
      }
   }
}


//------------------------------------------------------------------------------
// void Symmetrize(Rmatrix& mat)
//------------------------------------------------------------------------------
/**
 * Symmetrizes a square Rmatrix
 *
 * @param mat The matrix that is symmetrized
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::Symmetrize(Rmatrix& mat)
{
   Integer size = mat.GetNumRows();

   if (size != mat.GetNumColumns())
   {
      throw EstimatorException("Cannot symmetrize non-square matrices");
   }

   for (Integer i = 0; i < size; ++i)
   {
      for (Integer j = i+1; j < size; ++j)
      {
         mat(i,j) = 0.5 * (mat(i,j) + mat(j,i));
         mat(j,i) = mat(i,j);
      }
   }
}


//------------------------------------------------------------------------------
// void UpdateCovarianceSimple()
//------------------------------------------------------------------------------
/**
 * Applies equation (4.7.12) to update the state error covariance matrix
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::UpdateCovarianceSimple()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updating covariance using simple "
            "method\n");
   #endif

   // P = (I - K * H) * Pbar
   (*(stateCovariance->GetCovariance())) = (I - (kalman * H)) * pBar;
}


//------------------------------------------------------------------------------
// void ExtendedKalmanFilter::UpdateCovarianceJoseph()
//------------------------------------------------------------------------------
/**
 * This method updates the state error covariance matrix using the method
 * developed by Bucy and Joseph, as presented in Tapley, Schutz and Born
 * eq (4.7.19)
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::UpdateCovarianceJoseph()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updating covariance using Joseph "
            "method\n");
   #endif

   Rmatrix *r;

   if (measCovariance)
   {
      r = measCovariance->GetCovariance();
   }
   else
   {
      r = &defaultMeasCovariance;
   }

   // P = (I - K * H) * Pbar * (I - K * H)^T + K * R * K^T
   (*(stateCovariance->GetCovariance())) =
         ((I - (kalman * H)) * pBar * (I - (kalman * H)).Transpose()) +
         (kalman * (*r) * kalman.Transpose());

   #ifdef DEBUG_JOSEPH
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("  %.12lf  ", (*covariance)(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");

      throw EstimatorException("Intentional debug break!");
   #endif
}

void ExtendedKalmanFilter::UpdateReportText()
{
   char s[1000];
   std::stringstream msg;

   Real temp;
   std::string times;

   TimeConverterUtil::Convert("A1ModJulian", calculatedMeas->epoch,"","UTCGregorian", temp, times, 2);

   // print the RecNum, UTCGregorian-Epoch, TAIModJulian-Epoch
   if (textFileMode == "Normal")
   {
      sprintf(&s[0],"%8d   %s   ",
                  measManager.GetCurrentRecordNumber(), times.c_str());
   }
   else
   {
      Real timeTAI = TimeConverterUtil::Convert(
            calculatedMeas->epoch,calculatedMeas->epochSystem,
            TimeConverterUtil::TAIMJD);
      sprintf(&s[0],"%8d   %s  %.12lf        ",
             measManager.GetCurrentRecordNumber(), times.c_str(),
             timeTAI);
   }
   msg << s;

   // print the Obs Type
   std::string ss = currentObs->typeName + "                    ";
   msg << ss.substr(0,20) << " ";

   // print the Units
   ss = currentObs->unit + "    ";
   msg << ss.substr(0,4) << " ";

   // print the Participants
   ss = "";
   for(UnsignedInt n = 0; n < currentObs->participantIDs.size(); ++n)
   {
      ss = ss + currentObs->participantIDs[n] +
            (((n+1) == currentObs->participantIDs.size()) ? "" : ",");
   }
   msg << GmatStringUtil::GetAlignmentString(ss,
         GmatMathUtil::Max(pcolumnLen, minPartSize));

   // print the Edit
   // Specify removed reason and count number of removed records
   ss = measManager.GetObsDataObject()->removedReason;         //currentObs->removedReason;
   if (ss.substr(0,1) == "B")
   {
      numRemovedRecords["B"]++;
   }
   else
   {
      numRemovedRecords[ss]++;
   }
   msg << GmatStringUtil::GetAlignmentString(ss, 10, GmatStringUtil::LEFT);

   // print the obs,
   sprintf(&s[0],"%22.6lf   %22.6lf   %22.6lf   %18.6lf    %.12le   ",
         currentObs->value_orig[0],
         currentObs->value[0],
         calculatedMeas->value[0],
         ocDiff,
         calculatedMeas->feasibilityValue);
   msg << s;

   UpdateStateReportText(msg);

   // print out the Radar stuff
   if (textFileMode != "Normal")
   {
      if ((currentObs->typeName == "DSNTwoWayRange")||(currentObs->typeName == "DSNRange"))
      {
         sprintf(&s[0],"            %d   %.15le   "
               "%.15le                     N/A",
               currentObs->uplinkBand, currentObs->uplinkFreqAtRecei,    // currentObs->uplinkFreq is no longer in use. Use currentObs->uplinkFreqAtRecei instead
               currentObs->rangeModulo);
      }
      else if ((currentObs->typeName == "DSNTwoWayDoppler")||
            (currentObs->typeName == "Doppler")||
            (currentObs->typeName == "Doppler_RangeRate"))
      {
         sprintf(&s[0],"            %d                      "
               "N/A                      N/A                 %.4lf",
               currentObs->uplinkBand, currentObs->dopplerCountInterval);
      }
      else
      {
         sprintf(&s[0],"          N/A                      "
               "N/A                      N/A                     N/A");
      }
      msg << s;
   }
   msg << "\n";

   linesBuff = msg.str();
   WriteToTextFile(currentState);
}

void ExtendedKalmanFilter::UpdateStateReportText(std::stringstream& msg)
{
   char s[1000];
   std::string ss;

   // print out the state: xHat(k) = xHat(k)- + K * (z(k) - H * xHat(k)-)
   // or xHat(k) = xHat(k)- + K * residual
   for (UnsignedInt i=0; i < stateSize; ++i)
   {
      sprintf(&s[0],"   %18.12le", (*estimationState)[i]);
      ss.assign(s);
      ss = ss.substr(ss.size()-20,20);
      msg << "     " << ss;
   }

   // print out the state covariance:
   // P(k) = (I - K * H) * P(k)- * (I - K * H)' + K * R * K'
   for (UnsignedInt i=0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
      {
         sprintf(&s[0],"   %18.12le", (*stateCovariance)(i,j));
         ss.assign(s);
         ss = ss.substr(ss.size()-20,20);
         msg << "     " << ss;
      }
   }

   // print out the innovation covariance: S = H * P(k)- * H' + R
   for (UnsignedInt i = 0; i < measSize; ++i)
   {
      for (UnsignedInt j = 0; j < measSize; ++j)
      {
         sprintf(&s[0],"   %18.12le", innovationCov(i,j));
         ss.assign(s);
         ss = ss.substr(ss.size()-20,20);
         msg << "     " << ss;
      }
   }

   // print out the Kalman Gain: K(k) = P(k)- * H' * inv(H * P(k)- * H' + R)
   // or K(k) = P(k)- * H' * S^-1
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < measSize; ++j)
      {
         sprintf(&s[0],"   %18.12le", kalman(i,j));
         ss.assign(s);
         ss = ss.substr(ss.size()-20,20);
         msg << "     " << ss;
      }
   }
}

void ExtendedKalmanFilter::AdvanceEpoch()
{
   // Advance MeasMan to the next measurement and get its epoch
   bool isEndOfTable = measManager.AdvanceObservation();
   if (isEndOfTable)
   {
      currentState = CHECKINGRUN;
   }
   else
   {
      nextMeasurementEpoch = measManager.GetEpoch();
      FindTimeStep();

      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("ExtendedKalmanFilter::AdvanceEpoch CurrentEpoch = %.12lf, next "
               "epoch = %.12lf, timeStep = %.12lf\n", currentEpoch,
               nextMeasurementEpoch, timeStep);
      #endif

      // this magical number is from the Batch Estimator in its accumulating state...
      if (currentEpoch <= (nextMeasurementEpoch+5.0e-12))
      {
         // Reset the STM
         for (UnsignedInt i = 0; i < stateSize; ++i)
         {
            for (UnsignedInt j = 0; j < stateSize; ++j)
            {
               if (i == j)
               {
                  (*stm)(i,j) = 1.0;
               }
               else
               {
                  (*stm)(i,j) = 0.0;
               }
            }
         }
         esm.MapSTMToObjects();
         esm.MapVectorToObjects();
         PropagationStateManager *psm = propagator->GetPropStateManager();
         psm->MapObjectsToVector();

         // Flag that a new current state has been loaded in the objects
         resetState = true;

         currentState = PROPAGATING;
      }
      else
      {
         currentState = CHECKINGRUN;  // Should this just go to FINISHED?
      }
   }

   prevObsEpoch = calculatedMeas->epoch;
}


