//$Id: BatchEstimatorSVD.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                      BatchEstimatorSVD
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
// Created: 2009/08/28
//
/**
 * Batch least squares estimator using singular value decomposition.
 *
 * Note: SVD code is not yet implemented
 */
//------------------------------------------------------------------------------


#include "BatchEstimatorSVD.hpp"
#include "MessageInterface.hpp"
#include "EstimatorException.hpp"



//------------------------------------------------------------------------------
// BatchEstimatorSVD(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name for the constructed instance
 */
//------------------------------------------------------------------------------
BatchEstimatorSVD::BatchEstimatorSVD(const std::string &name):
   BatchEstimator       ("BatchEstimatorSVD", name)
{
   objectTypeNames.push_back("BatchEstimatorSVD");
}


//------------------------------------------------------------------------------
// BatchEstimatorSVD::~BatchEstimatorSVD()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
BatchEstimatorSVD::~BatchEstimatorSVD()
{
}


//------------------------------------------------------------------------------
// BatchEstimatorSVD(const BatchEstimatorSVD& est)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param est The estimator that is copied into this one
 */
//------------------------------------------------------------------------------
BatchEstimatorSVD::BatchEstimatorSVD(const BatchEstimatorSVD& est) :
   BatchEstimator       (est)
{

}


//------------------------------------------------------------------------------
// BatchEstimatorSVD& operator=(const BatchEstimatorSVD& est)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param est The estimator that supplies the configuration data for this one
 *
 * @return This estimator, configured to match est
 */
//------------------------------------------------------------------------------
BatchEstimatorSVD& BatchEstimatorSVD::operator=(const BatchEstimatorSVD& est)
{
   if (this != &est)
   {
      BatchEstimator::operator=(est);
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* BatchEstimatorSVD::Clone() const
//------------------------------------------------------------------------------
/**
 * Cloning method used to replicate this estimator.
 *
 * @return A new BatchEstimatorSVD, configured to match this one.
 */
//------------------------------------------------------------------------------
GmatBase* BatchEstimatorSVD::Clone() const
{
   return new BatchEstimatorSVD(*this);
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
void  BatchEstimatorSVD::Copy(const GmatBase* orig)
{
   operator=(*((BatchEstimatorSVD*)(orig)));
}



// Temporary -- rework or remove when math is coded
void BatchEstimatorSVD::CompleteInitialization()
{
   throw EstimatorException("The Batch SVD estimator is not implemented yet.");
}


//------------------------------------------------------------------------------
//  void Accumulate()
//------------------------------------------------------------------------------
/**
 * This method collects the data needed for estimation.
 */
//------------------------------------------------------------------------------
void BatchEstimatorSVD::Accumulate()
{
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("BatchEstimator state is ACCUMULATING\n");
   #endif

   const MeasurementData *calculatedMeas;
   std::vector<RealArray> stateDeriv;
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();

   hTilde.clear();

   #ifdef DEBUG_ACCUMULATION
      MessageInterface::ShowMessage("StateMap size is %d\n", stateMap->size());
   #endif
   modelsToAccess = measManager.GetValidMeasurementList();

   // todo: Determoine if modelsToAccess ever contains more than 1 value.
   // Currently assuming uniqueness
   if (measManager.Calculate(modelsToAccess[0]) >= 1)
   {
      calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);
      RealArray hTrow;
      hTrow.assign(stateSize, 0.0);
      UnsignedInt rowCount = calculatedMeas->value.size();
      for (UnsignedInt i = 0; i < rowCount; ++i)
         hTilde.push_back(hTrow);

      // Now walk the state vector and get elements of H-tilde for each piece
      for (UnsignedInt i = 0; i < stateMap->size(); ++i)
      {
         if ((*stateMap)[i]->subelement == 1)
         {
            #ifdef DEBUG_ACCUMULATION
               MessageInterface::ShowMessage(
                     "   Calculating ddstate(%d) for %s, subelement %d of %d, "
                     "id = %d\n",
                     i, (*stateMap)[i]->elementName.c_str(),
                     (*stateMap)[i]->subelement, (*stateMap)[i]->length,
                     (*stateMap)[i]->elementID);
            #endif
            stateDeriv = measManager.CalculateDerivatives(
                  (*stateMap)[i]->object, (*stateMap)[i]->elementID,
                  modelsToAccess[0]);

            // Fill in the corresponding elements of hTilde
            for (UnsignedInt j = 0; j < rowCount; ++j)
               for (Integer k = 0; k < (*stateMap)[i]->length; ++k)
                  hTilde[j][i+k] = stateDeriv[j][k];

            #ifdef DEBUG_ACCUMULATION
               MessageInterface::ShowMessage("      Result:\n         ");
               for (UnsignedInt l = 0; l < stateDeriv.size(); ++l)
               {
                  for (Integer m = 0; m < (*stateMap)[i]->length; ++m)
                     MessageInterface::ShowMessage("%.12lf   ",
                           stateDeriv[l][m]);
                  MessageInterface::ShowMessage("\n         ");
               }
               MessageInterface::ShowMessage("\n");
            #endif
         }
      }

      // Apply the STM
      RealArray hRow;
      Real entry;
      for (UnsignedInt i = 0; i < hTilde.size(); ++i)
      {
         hRow.assign(stateMap->size(), 0.0);
         for (UnsignedInt j = 0; j < stateMap->size(); ++j)
         {
            entry = 0.0;
            for (UnsignedInt k = 0; k < stateMap->size(); ++k)
               entry += hTilde[i][k] * (*stm)(k, j);
            hRow[j] = entry;
         }

         hAccum.push_back(hRow);

         #ifdef DEBUG_ACCUMULATION_RESULTS
            MessageInterface::ShowMessage("   Htilde  = [");
            for (UnsignedInt l = 0; l < stateMap->size(); ++l)
               MessageInterface::ShowMessage(  " %.12lf ", hTilde[i][l]);
            MessageInterface::ShowMessage(  "]\n");

            MessageInterface::ShowMessage("   H accum = [");
            for (UnsignedInt l = 0; l < stateMap->size(); ++l)
               MessageInterface::ShowMessage(  " %.12lf ", hRow[l]);
            MessageInterface::ShowMessage(  "]\n");

            MessageInterface::ShowMessage("   H has %d rows\n",
                  hAccum.size());
         #endif
      }

      // Accumulate the observed - calculated difference
      const ObservationData *currentObs =  measManager.GetObsData();
      Real ocDiff;
      for (UnsignedInt k = 0; k < currentObs->value.size(); ++k)
      {
         ocDiff = currentObs->value[k] - calculatedMeas->value[k];
         measurementResiduals.push_back(ocDiff);

         Real weight = 1.0;

         for (UnsignedInt i = 0; i < stateSize; ++i)
         {
            for (UnsignedInt j = 0; j < stateSize; ++j)
            {
               // todo: Add in the weights!!!
               // information(i,j) += hRow[i] * weights(i,j) * hRow[j];
               information(i,j) += hRow[i] * weight * hRow[j];
            }
            // todo: Add in the weights!!!
            residuals[i] += hRow[i] * weight * ocDiff;
         }
      }

      #ifdef DEBUG_ACCUMULATION_RESULTS
         MessageInterface::ShowMessage("Observed measurement value:\n");
         for (UnsignedInt k = 0; k < currentObs->value.size(); ++k)
            MessageInterface::ShowMessage("   %.12lf", currentObs->value[k]);
         MessageInterface::ShowMessage("\n");

         MessageInterface::ShowMessage("Calculated measurement value:\n");
         for (UnsignedInt k = 0; k < calculatedMeas->value.size(); ++k)
            MessageInterface::ShowMessage("   %.12lf",calculatedMeas->value[k]);
         MessageInterface::ShowMessage("\n");

         MessageInterface::ShowMessage("   O - C = ");
         for (UnsignedInt k = 0; k < calculatedMeas->value.size(); ++k)
            MessageInterface::ShowMessage("   %.12lf",
                  currentObs->value[k] - calculatedMeas->value[k]);
         MessageInterface::ShowMessage("\n");

         MessageInterface::ShowMessage("Derivatives w.r.t. state (H-tilde):\n");
         for (UnsignedInt k = 0; k < hTilde.size(); ++k)
         {
            for (UnsignedInt l = 0; l < hTilde[k].size(); ++l)
               MessageInterface::ShowMessage("   %.12lf",hTilde[k][l]);
            MessageInterface::ShowMessage("\n");
         }
         MessageInterface::ShowMessage("   Information Matrix = \n");
         for (Integer i = 0; i < estimationState->GetSize(); ++i)
         {
            MessageInterface::ShowMessage("      [");
            for (Integer j = 0; j < estimationState->GetSize(); ++j)
            {
               MessageInterface::ShowMessage(" %.12lf ", information(i, j));
            }
            MessageInterface::ShowMessage("]\n");
         }
         MessageInterface::ShowMessage("   Residuals = [");
         for (Integer i = 0; i < residuals.GetSize(); ++i)
            MessageInterface::ShowMessage(" %.12lf ", residuals[i]);
         MessageInterface::ShowMessage("]\n");
      #endif
   }

   // Accumulate the processed data
   #ifdef RUN_SINGLE_PASS
      #ifdef SHOW_STATE_TRANSITIONS
         MessageInterface::ShowMessage("BatchEstimator state is ESTIMATING at "
               "epoch %.12lf\n", currentEpoch);
      #endif
   #endif

   // Advance to MeasMan the next measurement and get its epoch
   measManager.AdvanceObservation();
   nextMeasurementEpoch = measManager.GetEpoch();
   FindTimeStep();

   if (currentEpoch < nextMeasurementEpoch)
      currentState = PROPAGATING;
   else
      currentState = ESTIMATING;
}


//------------------------------------------------------------------------------
// void Estimate()
//------------------------------------------------------------------------------
/**
 * This method solves the normal equations
 */
//------------------------------------------------------------------------------
void BatchEstimatorSVD::Estimate()
{
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("BatchEstimator state is ESTIMATING\n");
   #endif

   MessageInterface::ShowMessage("Accumulation complete; now solving the "
         "normal equations!\n");

   MessageInterface::ShowMessage("\nEstimating changes for iteration %d\n\n",
         iterationsTaken+1);

   MessageInterface::ShowMessage("   Presolution estimation state:\n      "
         "epoch = %.12lf\n      [", estimationState->GetEpoch());
   for (UnsignedInt i = 0; i < stateSize; ++i)
      MessageInterface::ShowMessage("  %.12lf  ", (*estimationState)[i]);
   MessageInterface::ShowMessage("]\n");

   Rmatrix cov = information.Inverse();
   dx.clear();
   Real delta;
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      delta = 0.0;
      for (UnsignedInt j = 0; j < stateSize; ++j)
         delta += cov(i,j) * residuals(j);
      dx.push_back(delta);
      (*estimationState)[i] += delta;
   }

   delta = 0.0;
   for (UnsignedInt i = 0; i < measurementResiduals.size(); ++i)
      delta += measurementResiduals[i] * measurementResiduals[i];
   oldResidualRMS = newResidualRMS;
   newResidualRMS = GmatMathUtil::Sqrt(delta / measurementResiduals.size());

   MessageInterface::ShowMessage("   State vector change (dx):\n      [");
   for (UnsignedInt i = 0; i < stateSize; ++i)
      MessageInterface::ShowMessage("  %.12lf  ", dx[i]);
   MessageInterface::ShowMessage("]\n");

   MessageInterface::ShowMessage("   New estimation state:\n      "
         "epoch = %.12lf\n      [", estimationState->GetEpoch());
   for (UnsignedInt i = 0; i < stateSize; ++i)
      MessageInterface::ShowMessage("  %.12lf  ", (*estimationState)[i]);
   MessageInterface::ShowMessage("]\n");

   MessageInterface::ShowMessage("   RMS measurement residuals = %.12lf\n",
         newResidualRMS);

   currentState = CHECKINGRUN;
}

