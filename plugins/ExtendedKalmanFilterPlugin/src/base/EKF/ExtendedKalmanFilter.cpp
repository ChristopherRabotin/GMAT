//$Id: ExtendedKalmanFilter.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         ExtendedKalmanFilter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
#include "UtilityException.hpp"
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
   cf(),
   qr(false),
   calculatedMeas(0),
   currentObs(0)
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
   cf(),
   qr(false),
   calculatedMeas(0),
   currentObs(0)
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
      measSize = ekf.measSize;

      cf = ekf.cf;
      qr = ekf.qr;
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

   I = Rmatrix::Identity(stateSize);

   sqrtP_T.SetSize(stateSize, stateSize);
   sqrtPupdate_T.SetSize(stateSize, stateSize);
   cf.Factor(*(stateCovariance->GetCovariance()), sqrtP_T);

   currentObs =  measManager.GetObsData();
   prevUpdateEpochGT = currentEpochGT;
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

   UpdateInfoType updateStat;

   // fill sigmas
   updateStat.processNoise.SetSize(Q.GetNumColumns(), Q.GetNumRows());
   updateStat.processNoise = Q;

   // fill STM
   updateStat.stm.SetSize(stm->GetNumRows(), stm->GetNumColumns());
   updateStat.stm = *stm;

   // setup the measurement objects for the rest of this frame of data to use
   SetupMeas();

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
   ComputeObs(updateStat);

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
   ComputeGain(updateStat);

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
   UpdateElements(updateStat);

   // Plot residuals if set
   if (showAllResiduals)
   {
      PlotResiduals();
   }

   // Convert current estimation state from GMAT internal coordinate system to participants' coordinate system
   currentSolveForState = esm.GetEstimationStateForReport();

   for (UnsignedInt ii = 0; ii < stateSize; ii++)
      updateStat.state.push_back(currentSolveForState[ii]);

   // Add state offset if not rectified
   if (esm.HasStateOffset())
   {
      GmatState xOffset = *esm.GetStateOffset();
      for (UnsignedInt ii = 0; ii < stateSize; ii++)
      {
         Real conv = GetEpsilonConversion(ii);
         updateStat.state[ii] += xOffset[ii] * conv;
      }
   }

   updateStat.cov.SetSize(informationInverse.GetNumRows(), informationInverse.GetNumColumns());
   updateStat.cov = informationInverse;
   updateStat.sigmaVNB = GetCovarianceVNB(informationInverse);

   updateStats.push_back(updateStat);
   BuildMeasurementLine(updateStat.measStat);
   WriteDataFile();
   AddMatlabData(updateStat.measStat);
   AddMatlabFilterData(updateStat);


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

   if (textFileMode == "Verbose")
      ReportProgress();

   AdvanceEpoch();
}

//------------------------------------------------------------------------------
// void TimeUpdate()
//------------------------------------------------------------------------------
/**
 * Performs the time update of the state error covariance
 *
 * This method uses Cholesky factorization for covariance
 * This is based on section 5.7 of Brown and Hwang 4e
 */
 //------------------------------------------------------------------------------
void ExtendedKalmanFilter::TimeUpdate()
{
#ifdef DEBUG_ESTIMATION
   MessageInterface::ShowMessage("Performing time update\n");
#endif

#ifdef DEBUG_ESTIMATION
   MessageInterface::ShowMessage("Q = \n");
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
         MessageInterface::ShowMessage("   %.12lf", Q(i, j));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("\n");
#endif

   /// Calculate conversion derivative matrixes
   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   cart2SolvMatrix = esm.CartToSolveForStateConversionDerivativeMatrix();
   // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
   solv2KeplMatrix = esm.SolveForStateToKeplConversionDerivativeMatrix();

   Rmatrix dX_dS = cart2SolvMatrixPrev;
   Rmatrix dS_dX = cart2SolvMatrix.Inverse();

   Rmatrix Q_S = dS_dX * Q * dS_dX.Transpose();
   Rmatrix stm_S = dS_dX * (*stm) * dX_dS;

   // Update offset from reference trajectory
   if (esm.HasStateOffset())
   {
      GmatState *offsetState = esm.GetStateOffset();
      Rvector xOffset(stateSize);
      xOffset.Set(offsetState->GetState(), stateSize);

      xOffset = (*stm) * xOffset;

      for (UnsignedInt i = 0; i < stateSize; ++i)
         (*offsetState)[i] = xOffset[i];
   }

   // Form C matrix and perform QR decomposition to calculate pBar

   // C = [sqrt(P)^T * Phi^T;
   //      sqrt(Q)^T];
   Rmatrix C(2 * stateSize, stateSize);
   Rmatrix C1 = sqrtP_T * stm_S.Transpose();

   Rmatrix sqrtQ_T(stateSize, stateSize);

   bool hasZeroDiag = false;
   for (UnsignedInt ii = 0U; ii < stateSize; ii++)
   {
      if (Q_S(ii, ii) == 0U)
      {
         hasZeroDiag = true;
         break;
      }
   }

   if (!hasZeroDiag)
   {
      try
      {
         cf.Factor(Q_S, sqrtQ_T);
      }
      catch (UtilityException e)
      {
         throw EstimatorException("The process noise matrix is not positive definite!");
      }
   }
   else
   {
      // Remove all zero rows/columns first
      IntegerArray removedIndexes;
      IntegerArray auxVector;
      Integer numRemoved;
      Rmatrix reducedQ_S = MatrixFactorization::CompressNormalMatrix(Q_S,
         removedIndexes, auxVector, numRemoved);

      Rmatrix reducedSqrtQ_T(stateSize - numRemoved, stateSize - numRemoved);
      try
      {
         cf.Factor(reducedQ_S, reducedSqrtQ_T);
      }
      catch (UtilityException e)
      {
         throw EstimatorException("The process noise matrix is not positive definite!");
      }

      sqrtQ_T = MatrixFactorization::ExpandNormalMatrixInverse(reducedSqrtQ_T,
         auxVector, numRemoved);
   }

   for (UnsignedInt ii = 0U; ii < stateSize; ii++)
   {
      for (UnsignedInt jj = 0U; jj < stateSize; jj++)
      {
         C(ii, jj) = C1(ii, jj);
         C(ii + stateSize, jj) = sqrtQ_T(ii, jj);
      }
   }

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("C = \n");
      for (UnsignedInt i = 0; i < 2 * stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", C(i,j));
         }
         MessageInterface::ShowMessage("\n");
      }
   #endif

   Rmatrix Tc(2 * stateSize, 2 * stateSize);
   Rmatrix Uc(2 * stateSize, stateSize);

   // Perform QR factorization
   qr.Factor(C, Uc, Tc);

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Uc = \n");
      for (UnsignedInt i = 0; i < 2 * stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", Uc(i,j));
         }
         MessageInterface::ShowMessage("\n");
      }
   #endif

   // Get sqrt(PBar)^T from top haplf of Uc
   for (UnsignedInt ii = 0U; ii < stateSize; ii++)
      for (UnsignedInt jj = 0U; jj < stateSize; jj++)
         sqrtP_T(ii, jj) = Uc(ii, jj);

   // Warn if covariance is not positive definite
   for (UnsignedInt ii = 0U; ii < stateSize; ii++)
   {
      if (GmatMathUtil::Abs(sqrtP_T(ii, ii)) < 1e-16)
      {
         MessageInterface::ShowMessage("WARNING The covariance is no longer positive definite! Epoch = %s\n", currentEpochGT.ToString().c_str());
         break;
      }
   }

   pBar = sqrtP_T.Transpose() * sqrtP_T;

   // make it symmetric!
   Symmetrize(pBar);
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

   if (modelsToAccess.size() > 0U)
   {
      measCount = measManager.CountFeasibleMeasurements(modelsToAccess[0]);
      calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);

      // verify media correction to be in acceptable range. It is [0m, 60m] for troposphere correction and [0m, 20m] for ionosphere correction
      ValidateMediaCorrection(calculatedMeas);

      // Make correction for observation value before running data filter
      if ((iterationsTaken == 0) && (currentObs->typeName == "DSN_SeqRange"))
      {
         // value correction is only applied for DSN_SeqRange and it is only performed at the first time
         for (Integer index = 0; index < currentObs->value.size(); ++index)
            measManager.GetObsDataObject()->value[index] = ObservationDataCorrection(calculatedMeas->value[index], currentObs->value[index], currentObs->rangeModulo);
      }

      // Size the measurement matricies
      measSize = currentObs->value.size();

      H.SetSize(measSize, stateSize);
      yi.SetSize(measSize);
      kalman.SetSize(stateSize, measSize);
   }

   /// Calculate conversion derivative matrixes
   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   cart2SolvMatrix = esm.CartToSolveForStateConversionDerivativeMatrix();
   // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
   solv2KeplMatrix = esm.SolveForStateToKeplConversionDerivativeMatrix();
}


//------------------------------------------------------------------------------
// void ComputeObs(UpdateInfoType &updateStat)
//------------------------------------------------------------------------------
/**
 * Computes the measurement residuals and the H-tilde matrix
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::ComputeObs(UpdateInfoType &updateStat)
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Computing obs and hTilde\n");
   #endif
   // Compute the O-C, Htilde, and Kalman gain

   // Populate measurement statistics
   FilterMeasurementInfoType measStat;
   CalculateResiduals(measStat);

   // Populate H and y
   if (modelsToAccess.size() > 0)
   {
      // Adjust computed and residual based on value of xOffset
      Rvector xOffset(stateSize);
      xOffset.Set(esm.GetEstimationStateOffset().GetState(), stateSize);

      Rvector H_x = H * xOffset;
      for (UnsignedInt k = 0; k < measStat.residual.size(); ++k)
      {
         measStat.measValue[k] += H_x(k);
         measStat.residual[k] -= H_x(k);
      }

      if (measStat.isCalculated)
      {
         for (UnsignedInt k = 0; k < measStat.residual.size(); ++k)
            yi(k) = measStat.residual[k];
      }

      // get scaled residuals
      Rmatrix R = *(GetMeasurementCovariance()->GetCovariance());

      // Keep this line for when we implement the scaled residual for the entire measurement
      // instead of for each element of the measurement:
      // measStat.scaledResid = GmatMathUtil::Sqrt(yi * (H * pBar * H.Transpose() + R).Inverse() * yi);

      // The element-by-element scaled residual calculation:
      for (UnsignedInt k = 0; k < measStat.residual.size(); ++k)
      {
         Rmatrix Rbar = H * pBar * H.Transpose() + R;
         Real sigmaVal = GmatMathUtil::Sqrt(Rbar(k, k));
         Real scaledResid = measStat.residual[k] / sigmaVal;
         measStat.scaledResid.push_back(scaledResid);
      }

   }  // end of if (modelsToAccess.size() > 0)

   currentSolveForState = esm.GetEstimationStateForReport();
   for (UnsignedInt ii = 0; ii < stateSize; ii++)
      measStat.state.push_back(currentSolveForState[ii]);

   // Add state offset if not rectified
   if (esm.HasStateOffset())
   {
      GmatState xOffset = *esm.GetStateOffset();
      for (UnsignedInt ii = 0; ii < stateSize; ii++)
      {
         Real conv = GetEpsilonConversion(ii);
         measStat.state[ii] += xOffset[ii] * conv;
      }
   }

   measStat.cov.SetSize(pBar.GetNumColumns(), pBar.GetNumRows());
   measStat.cov = pBar;
   measStat.sigmaVNB = GetCovarianceVNB(pBar);

   measStats.push_back(measStat);

   updateStat.epoch = currentEpochGT;
   updateStat.isObs = true;
   updateStat.measStat = measStat;

   BuildMeasurementLine(measStat);
   WriteToTextFile();
}


//------------------------------------------------------------------------------
// void ComputeGain(UpdateInfoType &updateStat)
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
void ExtendedKalmanFilter::ComputeGain(UpdateInfoType &updateStat)
{
   if (updateStat.measStat.isCalculated)
   {
      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("Computing Kalman Gain\n");
      #endif

      // Form A matrix and perform QR decomposition to calculate K and P
      Rmatrix R = *(GetMeasurementCovariance()->GetCovariance());
      Integer measSize = R.GetNumRows();

      // A = [sqrt(R)^T,        0;
      //      sqrt(PBar)^T*H^T, sqrt(PBar)^T];
      Rmatrix A(measSize + stateSize, measSize + stateSize);

      Rmatrix sqrtR_T(measSize, measSize);
      cf.Factor(R, sqrtR_T);

      Rmatrix A21 = sqrtP_T * H.Transpose();

      // Populate A
      // Top block
      for (UnsignedInt ii = 0U; ii < measSize; ii++)
         for (UnsignedInt jj = 0U; jj < measSize; jj++)
            A(ii, jj) = sqrtR_T(ii, jj);

      // Bottom block
      for (UnsignedInt ii = 0U; ii < stateSize; ii++)
      {
         // Left block
         for (UnsignedInt jj = 0U; jj < measSize; jj++)
            A(measSize + ii, jj) = A21(ii, jj);

         // Right block
         for (UnsignedInt jj = 0U; jj < stateSize; jj++)
            A(measSize + ii, measSize + jj) = sqrtP_T(ii, jj);
      }

      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("A = \n");
         for (UnsignedInt i = 0; i < measSize + stateSize; ++i)
         {
           for (UnsignedInt j = 0; j < measSize + stateSize; ++j)
           {
             MessageInterface::ShowMessage("  %.12le", A(i,j));
           }
           MessageInterface::ShowMessage("\n");
         }
      #endif

      Rmatrix Ta(measSize + stateSize, measSize + stateSize);
      Rmatrix Ua(measSize + stateSize, measSize + stateSize);

      // Perform QR factorization
      qr.Factor(A, Ua, Ta);

      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("Ua = \n");
         for (UnsignedInt i = 0; i < measSize + stateSize; ++i)
         {
           for (UnsignedInt j = 0; j < measSize + stateSize; ++j)
           {
             MessageInterface::ShowMessage("  %.12le", Ua(i,j));
           }
           MessageInterface::ShowMessage("\n");
         }
      #endif

      Rmatrix sqrtB_T(measSize, measSize);
      Rmatrix W_T(measSize, stateSize);

      // Populate result matricies
      for (UnsignedInt ii = 0U; ii < measSize; ii++)
         for (UnsignedInt jj = 0U; jj < measSize; jj++)
            sqrtB_T(ii, jj) = Ua(ii, jj);

      for (UnsignedInt ii = 0U; ii < measSize; ii++)
         for (UnsignedInt jj = 0U; jj < stateSize; jj++)
            W_T(ii, jj) = Ua(ii, measSize + jj);

      for (UnsignedInt ii = 0U; ii < stateSize; ii++)
         for (UnsignedInt jj = 0U; jj < stateSize; jj++)
            sqrtPupdate_T(ii, jj) = Ua(measSize + ii, measSize + jj);

      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("Calculating the Kalman gain\n");
      #endif

      kalman = W_T.Transpose() * sqrtB_T.Transpose().Inverse();
      updateStat.measStat.kalmanGain.SetSize(kalman.GetNumRows(), kalman.GetNumColumns());
      updateStat.measStat.kalmanGain = kalman;
   }
}


//------------------------------------------------------------------------------
// void UpdateElements(UpdateInfoType &updateStat)
//------------------------------------------------------------------------------
/**
 * Updates the estimation state and covariance matrix
 *
 * Programmers can select the covariance update method at the end of this
 * method.  The resulting covariance is symmetrized before returning.
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::UpdateElements(UpdateInfoType &updateStat)
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updating elements\n");
   #endif

   if (updateStat.measStat.editFlag == NORMAL_FLAG)
   {
      dx = kalman * yi;

      if (esm.HasStateOffset())
      {
         GmatState offsetState = esm.GetEstimationStateOffset();
         for (UnsignedInt i = 0; i < stateSize; ++i)
         {
            offsetState[i] += dx[i];
         }
         esm.SetEstimationStateOffset(offsetState);

      }
      else
      {
         // Update the state, covariances, and so forth
         estimationStateS = esm.GetEstimationState();
         for (UnsignedInt i = 0; i < stateSize; ++i)
         {
            estimationStateS[i] += dx[i];
         }

         // Convert estimation state from Keplerian to Cartesian
         esm.SetEstimationState(estimationStateS);                       // update the value of estimation state
         esm.MapVectorToObjects();
      }

      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("Calculated state change: [");
         for (UnsignedInt i = 0; i < stateSize; ++i)
            MessageInterface::ShowMessage(" %.12lf ", dx[i]);
         MessageInterface::ShowMessage("\n");
      #endif

      // Select the method used to update the covariance here:
      // UpdateCovarianceSimple();
      // UpdateCovarianceJoseph();

      Rmatrix P2 = sqrtPupdate_T.Transpose() * sqrtPupdate_T;
      sqrtP_T = sqrtPupdate_T;

      // Warn if covariance is not positive definite
      for (UnsignedInt ii = 0U; ii < stateSize; ii++)
      {
         if (GmatMathUtil::Abs(sqrtP_T(ii, ii)) < 1e-16)
         {
            MessageInterface::ShowMessage("WARNING The covariance is no longer positive definite! Epoch = %s\n", currentEpochGT.ToString().c_str());
            break;
         }
      }

      (*(stateCovariance->GetCovariance())) = P2;
   }
   else
      (*(stateCovariance->GetCovariance())) = sqrtP_T.Transpose() * sqrtP_T;

   Symmetrize(*stateCovariance);
   informationInverse = (*(stateCovariance->GetCovariance()));
   information = informationInverse.Inverse(COV_INV_TOL);
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

   Rmatrix *r = GetMeasurementCovariance()->GetCovariance();

   // P = (I - K * H) * Pbar * (I - K * H)^T + K * R * K^T
   (*(stateCovariance->GetCovariance())) =
         ((I - (kalman * H)) * pBar * (I - (kalman * H)).Transpose()) +
         (kalman * (*r) * kalman.Transpose());

   #ifdef DEBUG_JOSEPH
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("  %.12lf  ", (*(stateCovariance->GetCovariance()))(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");

      throw EstimatorException("Intentional debug break!");
   #endif
}

void ExtendedKalmanFilter::AdvanceEpoch()
{
   // Reset the STM
   PrepareForStep();
   esm.MapVectorToObjects();
   PropagationStateManager *psm = propagators[0]->GetPropStateManager();
   psm->MapObjectsToVector();

   // Flag that a new current state has been loaded in the objects
   resetState = true;

   // Advance MeasMan to the next measurement and get its epoch
   bool isEndOfTable = measManager.AdvanceObservation();
   if (isEndOfTable)
   {
      currentState = CHECKINGRUN;
   }
   else
   {
      nextMeasurementEpochGT = measManager.GetEpochGT();

      // Check if rectification should begin here
      if (esm.HasStateOffset())
      {
         // Check if next measurement is after the delayed rectification span
         Real elapsedTime = (nextMeasurementEpochGT - estimationEpochGT).GetTimeInSec();

         if (GmatMathUtil::Abs(elapsedTime) > delayRectifySpan)
         {
            // Update the state with the state offset
            GmatState offsetStateS = esm.GetEstimationStateOffset();
            estimationStateS = esm.GetEstimationState();
            for (UnsignedInt i = 0; i < stateSize; ++i)
            {
               estimationStateS[i] += offsetStateS[i];
            }

            // Convert estimation state from Keplerian to Cartesian
            esm.SetEstimationState(estimationStateS);                       // update the value of estimation state
            esm.MapVectorToObjects();

            // Zero out the state offset
            GmatState *offsetState = esm.GetStateOffset();
            for (UnsignedInt i = 0; i < stateSize; ++i)
               (*offsetState)[i] = 0.0;

            esm.SetHasStateOffset(false);
            WriteDataFile();
         }
      }

      // Reset nextNoiseUpdateGT if it is in the wrong direction
      Real dtNoise = (nextNoiseUpdateGT - currentEpochGT).GetTimeInSec();
      Real dtMeasurement = (nextMeasurementEpochGT - currentEpochGT).GetTimeInSec();

      if (dtNoise*dtMeasurement < 0) // If each dt has opposite sign
      {
         if (nextMeasurementEpochGT > currentEpochGT)
            nextNoiseUpdateGT.AddSeconds(processNoiseStep);
         else if (nextMeasurementEpochGT < currentEpochGT)
            nextNoiseUpdateGT.SubtractSeconds(processNoiseStep);
      }

      // Reset nextNoiseUpdateGT if it is near the current epoch
      if (GmatMathUtil::IsEqual(currentEpochGT, nextNoiseUpdateGT, ESTTIME_ROUNDOFF))
      {
         nextNoiseUpdateGT = currentEpochGT;
         if (nextMeasurementEpochGT > currentEpochGT)
            nextNoiseUpdateGT.AddSeconds(processNoiseStep);
         else if (nextMeasurementEpochGT < currentEpochGT)
            nextNoiseUpdateGT.SubtractSeconds(processNoiseStep);
      }

      FindTimeStep();

      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("ExtendedKalmanFilter::AdvanceEpoch CurrentEpoch = %.12lf, next "
               "epoch = %.12lf, timeStep = %.12lf\n", currentEpochGT,
               nextMeasurementEpochGT, timeStep);
      #endif

      // this magical number is from the Batch Estimator in its accumulating state...
      //if (currentEpoch <= (nextMeasurementEpoch+5.0e-12))
      if (nextMeasurementEpochGT >= 5.0e-12)
      {
         currentState = PROPAGATING;
      }
      else
      {
         currentState = CHECKINGRUN;  // Should this just go to FINISHED?
      }
   }
}


