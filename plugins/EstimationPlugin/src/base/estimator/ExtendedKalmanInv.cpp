//$Id: ExtendedKalmanInv.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         ExtendedKalmanInv
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
// Created: 2009/09/03
//
/**
 * A simple extended Kalman filter
 */
//------------------------------------------------------------------------------


#include "ExtendedKalmanInv.hpp"
#include "EstimatorException.hpp"
#include "MessageInterface.hpp"
#include <cmath>


//#define DEBUG_ESTIMATION
//#define DEBUG_JOSEPH


#define DEFAULT_MEASUREMENT_COVARIANCE 0.1

//------------------------------------------------------------------------------
// ExtendedKalmanInv(const std::string name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the new instance
 */
//------------------------------------------------------------------------------
ExtendedKalmanInv::ExtendedKalmanInv(const std::string name) :
   SequentialEstimator  ("ExtendedKalmanInv", name),
   measSize             (0)
{
   objectTypeNames.push_back("ExtendedKalmanInv");
}

//------------------------------------------------------------------------------
// ~ExtendedKalmanInv()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ExtendedKalmanInv::~ExtendedKalmanInv()
{
}


//------------------------------------------------------------------------------
// ExtendedKalmanInv(const ExtendedKalmanInv & ekf) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ekf The instance used to configure this instance
 */
//------------------------------------------------------------------------------
ExtendedKalmanInv::ExtendedKalmanInv(const ExtendedKalmanInv & ekf) :
   SequentialEstimator  (ekf),
   measSize             (ekf.measSize)
{
}


//------------------------------------------------------------------------------
// ExtendedKalmanInv& operator=(const ExtendedKalmanInv &ekf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ekf The instance used to configure this instance
 *
 * @return this instance, configured to match ekf.
 */
//------------------------------------------------------------------------------
ExtendedKalmanInv& ExtendedKalmanInv::operator=(const ExtendedKalmanInv &ekf)
{
   if (this != &ekf)
   {
      SequentialEstimator::operator=(ekf);
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
GmatBase* ExtendedKalmanInv::Clone() const
{
   return new ExtendedKalmanInv(*this);
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
void  ExtendedKalmanInv::Copy(const GmatBase* orig)
{
   operator=(*((ExtendedKalmanInv*)(orig)));
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
void ExtendedKalmanInv::CompleteInitialization()
{
   SequentialEstimator::CompleteInitialization();

   Integer size = stateCovariance->GetDimension();
   if (size != (Integer)stateSize)
      throw EstimatorException("In ExtendedKalmanInv::Estimate(), the "
            "covariance matrix is not sized correctly!!!");

   pBar.SetSize(stateSize, stateSize);
}


//------------------------------------------------------------------------------
// void Estimate()
//------------------------------------------------------------------------------
/**
 * Implements the time update, compute, and orbit updates for the estimator
 */
//------------------------------------------------------------------------------
void ExtendedKalmanInv::Estimate()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("\n\n---------------------\n");
      MessageInterface::ShowMessage("Current covariance:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", (*covariance)(i,j));
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

   // Perform the time update of the covariances, phi P phi^T, and the state
   TimeUpdate();

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Time updated matrix \\bar P:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", pBar(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   // Construct the O-C data and H tilde
   ComputeObs();

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
      PlotResiduals();


   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updated covariance:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", (*covariance)(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");

      MessageInterface::ShowMessage("Updated State: [ ");
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage(" %.12lf ", (*estimationState)[i]);
      MessageInterface::ShowMessage("\n\n---------------------\n");
   #endif

   // ReportProgress();

   // Advance MeasMan to the next measurement and get its epoch
   measManager.AdvanceObservation();
   nextMeasurementEpoch = measManager.GetEpoch();
   FindTimeStep();

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("CurrentEpoch = %.12lf, next "
            "epoch = %.12lf, timeStep = %.12lf\n", currentEpoch,
            nextMeasurementEpoch, timeStep);
   #endif

   if (currentEpoch < nextMeasurementEpoch)
   {
      // Reset the STM
      for (UnsignedInt i = 0; i < stateSize; ++i)
         for (UnsignedInt j = 0; j < stateSize; ++j)
            if (i == j)
               (*stm)(i,j) = 1.0;
            else
               (*stm)(i,j) = 0.0;
      esm.MapSTMToObjects();
      esm.MapVectorToObjects();
      PropagationStateManager *psm = propagator->GetPropStateManager();
      psm->MapObjectsToVector();

      // Flag that a new current state has been loaded in the objects
      resetState = true;

      currentState = PROPAGATING;
   }
   else
      currentState = CHECKINGRUN;  // Should this just go to FINISHED?
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
void ExtendedKalmanInv::TimeUpdate()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Performing time update\n");
   #endif

   Rmatrix cov(stateSize, stateSize), stmt(stateSize, stateSize);

   stmt = stm->Transpose();

   // P phi^T:
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
      {
         cov(i,j) = 0.0;
         for (UnsignedInt k = 0; k < stateSize; ++k)
         {
            cov(i,j) += (*stateCovariance)(i,k) * stmt(k,j);
         }
      }
   }
   // phi * (P phi^T):
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
      {
         pBar(i,j) = 0.0;
         for (UnsignedInt k = 0; k < stateSize; ++k)
            pBar(i,j) += (*stm)(i,k) * cov(k,j);
      }
   }

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
void ExtendedKalmanInv::ComputeObs()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Computing obs and hTilde\n");
   #endif
   // Compute the O-C, Htilde, and Kalman gain
   const MeasurementData *calculatedMeas = NULL;
   std::vector<RealArray> stateDeriv;
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();
   const ObservationData *currentObs =  measManager.GetObsData();
   hTilde.clear();

   #ifdef DEBUG_ESTIMATION_DETAILS
      MessageInterface::ShowMessage("StateMap size is %d\n", stateMap->size());
   #endif

   modelsToAccess = measManager.GetValidMeasurementList();

   UnsignedInt rowCount;
   // Currently assuming uniqueness in models to access
   if (measManager.Calculate(modelsToAccess[0], true) >= 1)
   {
      calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);
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
         hTilde.push_back(hTrow);

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
               for (Integer k = 0; k < (*stateMap)[i]->length; ++k)
                  hTilde[j][i+k] = stateDeriv[j][k];
         }
      }
   }

   if (calculatedMeas == NULL)
      throw EstimatorException("No measurement was calculated!");

   Real ocDiff;
   yi.clear();
   for (UnsignedInt k = 0; k < measSize; ++k)
   {
      ocDiff = currentObs->value[k] - calculatedMeas->value[k];
      measurementEpochs.push_back(currentEpoch);
      measurementResiduals.push_back(ocDiff);
      measurementResidualID.push_back(calculatedMeas->uniqueID);
      yi.push_back(ocDiff);
      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("*** Current O-C = %.12lf\n", ocDiff);
      #endif
   }

   if (currentObs->noiseCovariance == NULL)
      measCovariance = calculatedMeas->covariance;
   else
      measCovariance = currentObs->noiseCovariance;
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
void ExtendedKalmanInv::ComputeGain()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Computing Kalman Gain\n");
   #endif

   // Finally, the Kalman gain
   kalman.SetSize(stateSize, measSize);

   #ifdef DEBUG_ESTIMATION_DETAILS
      MessageInterface::ShowMessage("Calculating P H^T\n");
   #endif
   Rmatrix ph(stateSize, measSize), hph(measSize, measSize);

   // P H^T
   for (UnsignedInt i = 0; i < stateSize; ++i)
      for (UnsignedInt j = 0; j < measSize; ++j)
      {
         ph(i,j) = 0.0;
         for (UnsignedInt k = 0; k < stateSize; ++k)
            ph(i,j) += pBar(i,k) * hTilde[j][k];
      }

   // H P H^T
   #ifdef DEBUG_ESTIMATION_DETAILS
      MessageInterface::ShowMessage("Calculating H P H^T\n");
   #endif
   for (UnsignedInt i = 0; i < measSize; ++i)
      for (UnsignedInt j = 0; j < measSize; ++j)
      {
         hph(i,j) = 0.0;
         for (UnsignedInt k = 0; k < stateSize; ++k)
            hph(i,j) += hTilde[i][k] * ph(k,j);
      }

   // H P H^T + R; hard coded for now, since the obs covariances aren't available
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Adding R -- Constant 0.1 diag for now\n");
   #endif
   for (UnsignedInt i = 0; i < measSize; ++i)
   {
      for (UnsignedInt j = 0; j < measSize; ++j)
      {
         if (measCovariance == NULL)
         {
            if (i == j)
            {
               #ifdef DEBUG_WEIGHTS
                  MessageInterface::ShowMessage("Measurement covariance is "
                        "%le\n", (*(measCovariance))(i,j));
               #endif
               hph(i,j) += DEFAULT_MEASUREMENT_COVARIANCE;

            }
         }
         else
         {
            #ifdef DEBUG_WEIGHTS
               MessageInterface::ShowMessage("Measurement covariance (%d, %d) "
                     "is %le\n", i, j, (*(measCovariance))(i,j));
            #endif
            hph(i,j) += (*measCovariance)(i,j);
         }

         // Save the std deviation data
         sigma.push_back(sqrt(hph(i,j)));
      }
   }

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Gain denominator = \n");
      for (UnsignedInt i = 0; i < measSize; ++i)
      {
         for (UnsignedInt j = 0; j < measSize; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", hph(i,j));
         }
         MessageInterface::ShowMessage("\n");
      }
   #endif

   // Finally, Kalman = P H^T (H P H^T + R)^{-1}
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Calculating the Kalman gain\n");
   #endif
   Rmatrix inv = hph.Inverse();
   for (UnsignedInt i = 0; i < stateSize; ++i)
      for (UnsignedInt j = 0; j < measSize; ++j)
      {
         kalman(i,j) = 0.0;
         for (UnsignedInt k = 0; k < measSize; ++k)
            kalman(i,j) += ph(i,k) * inv(k,j);
      }
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
void ExtendedKalmanInv::UpdateElements()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updating elements\n");
   #endif

   // Update the state, covariances, and so forth
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      dx[i] = 0.0;
      for (UnsignedInt j = 0; j < measSize; ++j)
      {
         dx[i] += kalman(i,j) * yi[j];
         // Update the state vector
      }
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
void ExtendedKalmanInv::Symmetrize(Covariance& mat)
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
void ExtendedKalmanInv::Symmetrize(Rmatrix& mat)
{
   Integer size = mat.GetNumRows();

   if (size != mat.GetNumColumns())
      throw EstimatorException("Cannot symmetrize non-square matrices");

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
void ExtendedKalmanInv::UpdateCovarianceSimple()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updating covariance using simple "
            "method\n");
   #endif

   // Update the covariance
   Real khSum;
   Rmatrix temp(stateSize, stateSize);

   // First calc  (I - K H)
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
      {
         if (i == j)
            temp(i,j) = 1.0;
         else
            temp(i,j) = 0.0;
         khSum = 0.0;
         for (UnsignedInt k = 0; k < measSize; ++k)
            khSum += kalman(i,k) * hTilde[k][j];
         temp(i,j) -= khSum;
      }
   }

   // Now build (I - K H) (\bar P) (I - K H)
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
      {
         (*stateCovariance)(i,j) = 0.0;
         for (UnsignedInt k = 0; k < stateSize; ++k)
            (*stateCovariance)(i,j) += temp(i,k) * pBar(k,j);
      }
   }
}


//------------------------------------------------------------------------------
// void ExtendedKalmanInv::UpdateCovarianceJoseph()
//------------------------------------------------------------------------------
/**
 * This method updates the state error covariance matrix using the method
 * developed by Bucy and Joseph, as presented in Tapley, Schutz and Born
 * eq (4.7.19)
 */
//------------------------------------------------------------------------------
void ExtendedKalmanInv::UpdateCovarianceJoseph()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updating covariance using Joseph "
            "method\n");
   #endif

   // Update the covariance
   Real khSum;
   Rmatrix temp(stateSize, stateSize), krk(stateSize, stateSize);
   Rmatrix pikh(stateSize, stateSize), r(measSize, measSize);
   Rmatrix ikh(stateSize, stateSize), rk(measSize, stateSize);

   // First calc  (I - K H)
   #ifdef DEBUG_JOSEPH
      MessageInterface::ShowMessage("Calcing I - K H\n K:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < measSize; ++j)
            MessageInterface::ShowMessage("  %.12lf  ", kalman(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n H:\n");

      for (UnsignedInt i = 0; i < measSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("  %.12lf  ", hTilde[i][j]);
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
      {
         if (i == j)
            ikh(i,j) = 1.0;
         else
            ikh(i,j) = 0.0;
         khSum = 0.0;
         for (UnsignedInt k = 0; k < measSize; ++k)
            khSum += kalman(i,k) * hTilde[k][j];
         ikh(i,j) -= khSum;
      }
   }

   #ifdef DEBUG_JOSEPH
      MessageInterface::ShowMessage(" I - K H:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("  %.12lf  ", ikh(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n pBar:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("  %.12lf  ", pBar(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n (I - K H) \bar P (I - K H)^T:\n");
   #endif

   // Build (I - K H) \bar P (I - K H)^T
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
      {
         pikh(i,j) = 0.0;
         for (UnsignedInt k = 0; k < stateSize; ++k)
            pikh(i,j) += pBar(i,k) * ikh(j,k);
      }
   }
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
      {
         temp(i,j) = 0.0;
         for (UnsignedInt k = 0; k < stateSize; ++k)
            temp(i,j) += ikh(i,k) * pikh(k,j);
      }
   }

   #ifdef DEBUG_JOSEPH
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("  %.12lf  ", temp(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   #ifdef DEBUG_JOSEPH
      MessageInterface::ShowMessage("Prepping to fake up R\n R:\n");
   #endif
   // Fake up the measurement covariance if needed
   if (measCovariance)
      r = *(measCovariance->GetCovariance());
   else
      for (UnsignedInt i = 0; i < measSize; ++i)
         for (UnsignedInt j = 0; j < measSize; ++j)
            if (i == j)
               r(i,j) = DEFAULT_MEASUREMENT_COVARIANCE;
            else
               r(i,j) = 0.0;

   // Now K R K^T
   #ifdef DEBUG_JOSEPH
      for (UnsignedInt i = 0; i < measSize; ++i)
      {
         for (UnsignedInt j = 0; j < measSize; ++j)
            MessageInterface::ShowMessage("  %.12lf  ", r(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");

      MessageInterface::ShowMessage("Calcing RK:\n");
   #endif
   for (UnsignedInt i = 0; i < measSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
      {
         rk(i,j) = 0.0;
         for (UnsignedInt k = 0; k < measSize; ++k)
            rk(i,j) += r(i,k) * kalman(j,k);
      }
   }
   #ifdef DEBUG_JOSEPH
      for (UnsignedInt i = 0; i < measSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("  %.12lf  ", rk(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");

      MessageInterface::ShowMessage("Calcing KRK:\n");
   #endif
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
      {
         krk(i,j) = 0.0;
         for (UnsignedInt k = 0; k < measSize; ++k)
            krk(i,j) += kalman(i,k) * rk(k,j);
      }
   }
   #ifdef DEBUG_JOSEPH
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("  %.12lf  ", krk(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   // Add 'em up
   #ifdef DEBUG_JOSEPH
      MessageInterface::ShowMessage("Summing covariance\n");
   #endif
   for (UnsignedInt i = 0; i < stateSize; ++i)
      for (UnsignedInt j = 0; j < stateSize; ++j)
         (*stateCovariance)(i,j) = temp(i,j) + krk(i,j);

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
