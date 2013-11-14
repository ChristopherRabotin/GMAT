//$Id: BatchEstimatorInv.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         BatchEstimatorInv
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
// Created: 2009/08/04
//
/**
 * Batch least squares estimator using direct inversion
 */
//------------------------------------------------------------------------------


#include "BatchEstimatorInv.hpp"
#include "MessageInterface.hpp"
#include "EstimatorException.hpp"


//#define DEBUG_ACCUMULATION
//#define DEBUG_ACCUMULATION_RESULTS
//#define WALK_STATE_MACHINE
//#define DEBUG_VERBOSE
//#define DEBUG_WEIGHTS
//#define DEBUG_O_MINUS_C

//------------------------------------------------------------------------------
// BatchEstimatorInv(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name Name of the instance being constructed
 */
//------------------------------------------------------------------------------
BatchEstimatorInv::BatchEstimatorInv(const std::string &name):
   BatchEstimator       ("BatchEstimatorInv", name)
{
   objectTypeNames.push_back("BatchEstimatorInv");
}


//------------------------------------------------------------------------------
// ~BatchEstimatorInv()
//------------------------------------------------------------------------------
/**
 * Class destructor
 */
//------------------------------------------------------------------------------
BatchEstimatorInv::~BatchEstimatorInv()
{
}


//------------------------------------------------------------------------------
// BatchEstimatorInv(const BatchEstimatorInv& est)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param est The object that is being copied
 */
//------------------------------------------------------------------------------
BatchEstimatorInv::BatchEstimatorInv(const BatchEstimatorInv& est) :
   BatchEstimator       (est)
{

}


//------------------------------------------------------------------------------
// BatchEstimatorInv& operator=(const BatchEstimatorInv& est)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param est The object providing configuration data for this object
 *
 * @return This object, configured to match est
 */
//------------------------------------------------------------------------------
BatchEstimatorInv& BatchEstimatorInv::operator=(const BatchEstimatorInv& est)
{
   if (this != &est)
   {
      BatchEstimator::operator=(est);
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Object cloner
 *
 * @return Pointer to a new BatchEstimatorInv configured to match this one.
 */
//------------------------------------------------------------------------------
GmatBase* BatchEstimatorInv::Clone() const
{
   return new BatchEstimatorInv(*this);
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
void  BatchEstimatorInv::Copy(const GmatBase* orig)
{
   operator=(*((BatchEstimatorInv*)(orig)));
}


//------------------------------------------------------------------------------
//  void Accumulate()
//------------------------------------------------------------------------------
/**
 * This method collects the data needed for estimation.
 */
//------------------------------------------------------------------------------
void BatchEstimatorInv::Accumulate()
{
   #ifdef DEBUG_ACCUMULATION
      MessageInterface::ShowMessage("Entered BatchEstimatorInv::Accumulate()\n");
   #endif

   // Measurements are possible!
   const MeasurementData *calculatedMeas;
   std::vector<RealArray> stateDeriv;
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();

   hTilde.clear();

   #ifdef DEBUG_ACCUMULATION
      MessageInterface::ShowMessage("StateMap size is %d\n", stateMap->size());
   #endif
   modelsToAccess = measManager.GetValidMeasurementList();
   #ifdef DEBUG_ACCUMULATION
      MessageInterface::ShowMessage("Found %d models\n", modelsToAccess.size());
   #endif

   
   MeasurementModel* measModel = measManager.GetMeasurementObject(modelsToAccess[0]);
   Real maxLimit = measModel->GetRealParameter("ResidualMax");

   // Get the current observation data
   const ObservationData *currentObs =  measManager.GetObsData();
   if ((modelsToAccess.size() > 0)&&(measManager.Calculate(modelsToAccess[0], true) >= 1))
   {
	  calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);
	  if (iterationsTaken == 0)
	  {
	     for (Integer i=0; i < currentObs->value.size(); ++i)
	     {
			// Data filtered based on residual limits:
		    if (abs(currentObs->value[i] - calculatedMeas->value[i]) > maxLimit)	// if (abs(O-C) > max limit) then throw away this data record
		    {
		       measManager.GetObsDataObject()->inUsed = false;
			   break;
		    }

			// Data filtered based on time range:
			Real epoch1 = TimeConverterUtil::Convert(estimationStart, TimeConverterUtil::A1MJD, currentObs->epochSystem);
			Real epoch2 = TimeConverterUtil::Convert(estimationEnd, TimeConverterUtil::A1MJD, currentObs->epochSystem);
			if ((currentObs->epoch < epoch1)||(currentObs->epoch > epoch2))
			{
		       measManager.GetObsDataObject()->inUsed = false;
			   break;
			}
	     }
	  }
   }
   #ifdef DEBUG_ACCUMULATION
      MessageInterface::ShowMessage("iterationsTaken = %d    inUsed = %s\n", iterationsTaken, (measManager.GetObsDataObject()->inUsed ? "true" : "false"));
   #endif


   // Currently assuming uniqueness; modify if more than 1 possible here
//   if ((modelsToAccess.size() > 0) &&
//       (measManager.Calculate(modelsToAccess[0], true) >= 1))
   if (measManager.GetObsDataObject()->inUsed)
   {
//      calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);
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
			   MessageInterface::ShowMessage("object = <%p '%s'>\n", (*stateMap)[i]->object, (*stateMap)[i]->object->GetName().c_str());
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
      #ifdef DEBUG_ACCUMULATION
         MessageInterface::ShowMessage("Applying the STM\n");
      #endif
      RealArray hRow;

      // Temporary buffer for non-scalar measurements; hMeas is used to build
      // the information matrix below.
      std::vector<RealArray> hMeas;

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
         hMeas.push_back(hRow);

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
//      const ObservationData *currentObs =  measManager.GetObsData();
      Real ocDiff;
      #ifdef DEBUG_ACCUMULATION
         MessageInterface::ShowMessage("Accumulating the O-C differences\n");
         MessageInterface::ShowMessage("   Obs size = %d, calc size = %d\n",
               currentObs->value.size(), calculatedMeas->value.size());
      #endif
      for (UnsignedInt k = 0; k < currentObs->value.size(); ++k)
      {
         ocDiff = currentObs->value[k] - calculatedMeas->value[k];

         #ifdef DEBUG_O_MINUS_C
//		    if ((calculatedMeas->value[k] < 0)||(calculatedMeas->value[k] > currentObs->rangeModulo))
//				MessageInterface::ShowMessage("!!!!! Error: Get incorrect calculated measurement value:   C =  %.12le\n", calculatedMeas->value[k]);
//			MessageInterface::ShowMessage("Calculate O-C = %.12le:  O = %.12le    C = %.12le   k = %d  calculatedMeas = <%p>\n\n", ocDiff, currentObs->value[k], calculatedMeas->value[k], k, calculatedMeas);
		    Real OD_Epoch = TimeConverterUtil::Convert(currentObs->epoch,
                                      TimeConverterUtil::A1MJD, TimeConverterUtil::TAIMJD,
                                      GmatTimeConstants::JD_JAN_5_1941);
		    Real C_Epoch = TimeConverterUtil::Convert(calculatedMeas->epoch,
                                      TimeConverterUtil::A1MJD, TimeConverterUtil::TAIMJD,
                                      GmatTimeConstants::JD_JAN_5_1941);
		    MessageInterface::ShowMessage("Observation data: %s  %s  Epoch = %.12lf  O = %.12le;         Calculated measurement: %s  %s  Epoch = %.12lf  C = %.12le;       O-C = %.12le     M = %.12le  frequency = %.12le\n",
				currentObs->participantIDs[0].c_str(), currentObs->participantIDs[1].c_str(), OD_Epoch, currentObs->value[k], 
				calculatedMeas->participantIDs[0].c_str(), calculatedMeas->participantIDs[1].c_str(), C_Epoch, calculatedMeas->value[k], 
				ocDiff, currentObs->rangeModulo, currentObs->uplinkFreq);
         #endif
//		    Real C_Epoch = TimeConverterUtil::Convert(calculatedMeas->epoch,
//                                      TimeConverterUtil::A1MJD, TimeConverterUtil::TAIMJD,
//                                      GmatTimeConstants::JD_JAN_5_1941);
//		    MessageInterface::ShowMessage("%s    %.12lf   %.12le\n", currentObs->participantIDs[0].c_str(), C_Epoch,	ocDiff);

         measurementEpochs.push_back(currentEpoch);
         measurementResiduals.push_back(ocDiff);
         measurementResidualID.push_back(calculatedMeas->uniqueID);

         Real weight = 1.0;
         if (currentObs->noiseCovariance == NULL)
         {
            #ifdef DEBUG_WEIGHTS
               MessageInterface::ShowMessage("Measurement covariance(%d %d) "
                     "is %le\n", k, k, (*(calculatedMeas->covariance))(k,k));
            #endif

            // todo: Figure out why the covariances can be 0.0!?!

            if ((*(calculatedMeas->covariance))(k,k) != 0.0)
               // Weight is diag(1 / sigma^2), per Montebruck & Gill, eq. 8.33
               // Covariance diagonal is ~diag(sigma^2)
               // If no off diagonal terms, inverse in 1/element along diagonal
               weight = 1.0 / (*(calculatedMeas->covariance))(k,k);
            else
               weight = 1.0;
         }
         else
         {
            weight = 1.0 / (*(currentObs->noiseCovariance))(k,k);
            #ifdef DEBUG_WEIGHTS
               MessageInterface::ShowMessage("Noise covariance(%d %d) "
                     "is %le\n", k, k, (*(currentObs->noiseCovariance))(k,k));
            #endif
         }

         for (UnsignedInt i = 0; i < stateSize; ++i)
         {
            for (UnsignedInt j = 0; j < stateSize; ++j)
               //information(i,j) += hRow[i] * weight * hRow[j];
               information(i,j) += hMeas[k][i] * weight * hMeas[k][j];

            //residuals[i] += hRow[i] * weight * ocDiff;
            residuals[i] += hMeas[k][i] * weight * ocDiff;
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

   #ifdef DEBUG_ACCUMULATION
      MessageInterface::ShowMessage("Advancing to the next measurement\n");
   #endif

   // Advance to MeasMan the next measurement and get its epoch
   measManager.AdvanceObservation();
   nextMeasurementEpoch = measManager.GetEpoch();
   FindTimeStep();

   if (currentEpoch < nextMeasurementEpoch)
      currentState = PROPAGATING;
   else
      currentState = ESTIMATING;

   #ifdef DEBUG_ACCUMULATION
      MessageInterface::ShowMessage("Exit BatchEstimatorInv::Accumulate()\n");
   #endif

}




//------------------------------------------------------------------------------
// void Estimate()
//------------------------------------------------------------------------------
/**
 * This method solves the normal equations using direct inversion
 */
//------------------------------------------------------------------------------
void BatchEstimatorInv::Estimate()
{
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("BatchEstimator state is ESTIMATING\n");
   #endif

   if (measurementResiduals.size() == 0)
      throw EstimatorException("Unable to estimate: No measurements were "
            "feasible");

   #ifdef DEBUG_VERBOSE
      MessageInterface::ShowMessage("Accumulation complete; now solving the "
            "normal equations!\n");

      MessageInterface::ShowMessage("\nEstimating changes for iteration %d\n\n",
            iterationsTaken+1);

      MessageInterface::ShowMessage("   Presolution estimation state:\n      "
            "epoch = %.12lf\n      [", estimationState->GetEpoch());
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage("  %.12lf  ", (*estimationState)[i]);
      MessageInterface::ShowMessage("]\n");

      MessageInterface::ShowMessage("   Information matrix:\n");
      for (UnsignedInt i = 0; i < information.GetNumRows(); ++i)
      {
         MessageInterface::ShowMessage("      [");
         for (UnsignedInt j = 0; j < information.GetNumColumns(); ++j)
         {
            MessageInterface::ShowMessage(" %.12lf ", information(i,j));
         }
         MessageInterface::ShowMessage("]\n");
      }
   #endif

   Rmatrix cov = information.Inverse();
   
   #ifdef DEBUG_VERBOSE
      MessageInterface::ShowMessage(" residuals: [\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage("  %.12lf  ", residuals(i));
      MessageInterface::ShowMessage("]\n");

      MessageInterface::ShowMessage("   covarian matrix:\n");
      for (UnsignedInt i = 0; i < cov.GetNumRows(); ++i)
      {
         MessageInterface::ShowMessage("      [");
         for (UnsignedInt j = 0; j < cov.GetNumColumns(); ++j)
         {
            MessageInterface::ShowMessage(" %.12lf ", cov(i,j));
         }
         MessageInterface::ShowMessage("]\n");
      }
   #endif

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

   #ifdef DEBUG_VERBOSE
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
   #endif

   currentState = CHECKINGRUN;
}
