//$Id: OpticalAzEl.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             OpticalAzEl
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: Mar 23, 2010 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the OpticalAzEl class
 */
//------------------------------------------------------------------------------


#include "OpticalAzEl.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"


//#define DEBUG_OPTICAL_AZEL_CALC
//#define DEBUG_DERIVATIVES
//#define DEBUG_DERIVATIVE_DETAILS
//#define VIEW_PARTICIPANT_STATES

//------------------------------------------------------------------------------
// OpticalAzEl(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the constructed instance
 */
//------------------------------------------------------------------------------
OpticalAzEl::OpticalAzEl(const std::string &name) :
   OpticalAngles      ("OpticalAzEl", name)
{
   objectTypeNames.push_back("OpticalAzEl");

   // Prep value array in measurement
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "OpticalAzEl";
   currentMeasurement.type = Gmat::OPTICAL_AZEL;

   receiveDelay  = 0.0;    // Electronics delay to process the image

   covariance.SetDimension(2);
   covariance(0,0) = covariance(1,1) = 1.0;
}


//------------------------------------------------------------------------------
// ~OpticalAzEl()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
OpticalAzEl::~OpticalAzEl()
{
}


//------------------------------------------------------------------------------
// OpticalAzEl(const OpticalAzEl & oae)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param oae The instance copied to this new one
 */
//------------------------------------------------------------------------------
OpticalAzEl::OpticalAzEl(const OpticalAzEl & oae) :
   OpticalAngles     (oae)
{
   currentMeasurement = oae.currentMeasurement;
   covariance = oae.covariance;
}


//------------------------------------------------------------------------------
// OpticalAzEl& operator=(const OpticalAzEl& oae)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param oae The instance copied to this one
 *
 * @return This instance, set to match oae
 */
//------------------------------------------------------------------------------
OpticalAzEl& OpticalAzEl::operator=(const OpticalAzEl& oae)
{
   if (this != &oae)
   {
      OpticalAngles::operator=(oae);

      currentMeasurement = oae.currentMeasurement;
      covariance = oae.covariance;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Methos used to replicate this object from a GmatBase pointer to it
 *
 * @return A new OpticalAzEl instance built using this one's copy constructor
 */
//------------------------------------------------------------------------------
GmatBase *OpticalAzEl::Clone() const
{
   return new OpticalAzEl(*this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares this instance for use in an estimation process
 *
 * @return true on successful initialization, false if initialization fails
 */
//------------------------------------------------------------------------------
bool OpticalAzEl::Initialize()
{
   #ifdef DEBUG_OPTICAL_AZEL_CALC
      MessageInterface::ShowMessage("Entered OpticalAzEl::Initialize(); "
            "this = %p\n", this);
   #endif

   bool retval = false;

   if (OpticalAngles::Initialize())
      retval = true;

   #ifdef DEBUG_OPTICAL_AZEL_CALC
      MessageInterface::ShowMessage("   Initialization %s with %d "
            "participants\n", (retval ? "succeeded" : "failed"),
            participants.size());
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// const std::vector<RealArray> & CalculateMeasurementDerivatives(
//          GmatBase *obj, Integer id)
//------------------------------------------------------------------------------
/**
 * Calculates the derivative of this measurement with respect to a parameter
 * on the input object
 *
 * @param obj The object supplying the "with respect to" parameter
 * @param id The ID of the "with respect to" parameter
 *
 * @return The derivative information packaged for use in the estimators
 */
//------------------------------------------------------------------------------
const std::vector<RealArray> & OpticalAzEl::CalculateMeasurementDerivatives(
         GmatBase *obj, Integer id)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("OpticalAzEl::CalculateMeasurement"
            "Derivatives(%s, %d) called\n", obj->GetName().c_str(), id);
   #endif

   if (!initialized)
      InitializeMeasurement();

   GmatBase *objPtr = NULL;

   Integer size = obj->GetEstimationParameterSize(id);
   Integer objNumber = -1;

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   ParameterSize = %d\n", size);
   #endif

   if (size <= 0)
      throw MeasurementException("The derivative parameter on derivative "
            "object " + obj->GetName() + "is not recognized");

   // Check to see if obj is a participant
   for (UnsignedInt i = 0; i < this->participants.size(); ++i)
   {
      if (participants[i] == obj)
      {
         objPtr = participants[i];
         objNumber = i + 1;
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Participant %s found\n",
                  objPtr->GetName().c_str());
         #endif
         break;
      }
   }

   // Or if it is the measurement model for this object
   if (obj->IsOfType(Gmat::MEASUREMENT_MODEL))
   if (obj->GetRefObject(Gmat::CORE_MEASUREMENT, "") == this)
   {
      objPtr = obj;
      objNumber = 0;
      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   The measurement is the object\n",
               objPtr->GetName().c_str());
      #endif
   }

   if (objNumber == -1)
      throw MeasurementException(
            "USNTwoWayRange error - object is neither participant nor "
            "measurement model.");

   RealArray oneRow;
   oneRow.assign(size, 0.0);
   currentDerivatives.clear();
   currentDerivatives.push_back(oneRow);  // Az
   currentDerivatives.push_back(oneRow);  // El

   Integer parameterID = GetParmIdFromEstID(id, obj);

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   Looking up id %d\n", parameterID);
   #endif

   if (objPtr != NULL)
   {
      if (objNumber == 1) // participant number 1, either a GroundStation or a Spacecraft
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of Participant"
                     " 1\n", objPtr->GetParameterText(parameterID).c_str());
         #endif
         if (objPtr->GetParameterText(parameterID) == "Position")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() + " Position is not yet implemented");
         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() + " Velocity is not yet implemented");
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() + " CartesianState is not yet implemented");
         }
         else if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] = 1.0;
         }
         else
         {
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("   Deriv is w.r.t. something "
                        "independent, so zero\n");
            #endif
            for (UnsignedInt i = 0; i < 3; ++i)
               currentDerivatives[0][i] = 0.0;
         }
      }
      else if (objNumber == 2) // participant 2, always a Spacecraft
      {
         if (objPtr->GetParameterText(parameterID) == "Position")
         {
            #ifdef DEBUG_DERIVATIVE_DETAILS
               MessageInterface::ShowMessage("Calculating derivative for az = "
                     "%.12lf, el = %.12lf\n", currentMeasurement.value[0],
                     currentMeasurement.value[1]);
            #endif

            // Get the inverse of the orbit STM for the object supporting the
            // solve-for parameter (i.e. for obj) at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);

            Real cosAz = GmatMathUtil::Cos(currentMeasurement.value[0] *
                  GmatMathConstants::RAD_PER_DEG);
            // This is 1/sec^2(Az) = cos^2(Az):
            Real cos2Az = cosAz*cosAz;

            // This is 1/cos(El)
            Real secEl = 1.0 / GmatMathUtil::Cos(currentMeasurement.value[1] *
                  GmatMathConstants::RAD_PER_DEG);

            #ifdef DEBUG_DERIVATIVE_DETAILS
                MessageInterface::ShowMessage("   1/sec^2(az) = %.12lf\n",
                      cos2Az);
                MessageInterface::ShowMessage("   1/cos(el) = %.12lf\n", secEl);
            #endif

            Rvector3 xUnit(1.0, 0.0, 0.0), yUnit(0.0, 1.0, 0.0),
                     zUnit(0.0, 0.0, 1.0);

            Rmatrix33 identity(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

            // Range vector in the observer's coordinate system
            Real range = obsRange.GetMagnitude();
            Rvector3 unitRange = obsRange / range;
            Rmatrix33 rangeRangeT, rangeXT, azPart, elPart;
            Real xTRange = unitRange[0];

            for (Integer i = 0; i < 3; ++i)
               for (Integer j = 0; j < 3; ++j)
               {
                  rangeRangeT(i,j) = unitRange[i] * unitRange[j];
                  rangeXT(i,j) = unitRange[i] * xUnit[j] / xTRange;
               }

            #ifdef DEBUG_DERIVATIVE_DETAILS
               MessageInterface::ShowMessage("   range = %.12lf\n", range);
               MessageInterface::ShowMessage("   unit range vector = %s\n",
                     unitRange.ToString().c_str());
               MessageInterface::ShowMessage("   x^T unitRange = %.12lf\n",
                     xTRange);
               MessageInterface::ShowMessage("   range range^T = %s\n",
                     rangeRangeT.ToString().c_str());
               MessageInterface::ShowMessage("   range x^T = %s\n",
                     rangeXT.ToString().c_str());
            #endif

            for (Integer i = 0; i < 3; ++i)
               for (Integer j = 0; j < 3; ++j)
               {
                  azPart(i,j) = rangeXT(i,j) - identity(i,j);
                  elPart(i,j) = identity(i,j) - rangeRangeT(i,j);
               }

            Rmatrix33 dRho, dRhoObs;
            GetRangeVectorDerivative(lightPath, stmInv, dRho, true, 1, 0,
                  true, false);

            // Build the range  derivative times the rotation matrix
            dRhoObs  = -R_o_j2k * dRho;

            Real azPrefactor = cos2Az / (range * xTRange);
            Real elPrefactor = secEl/range;

            Rmatrix33 azMat  = azPart * dRhoObs;
            Rmatrix33 elMat  = elPart * dRhoObs;

            for (Integer i = 0; i < 3; ++i)
            {
               // Azimuth derivative
               currentDerivatives[0][i] = azPrefactor * azMat(1, i) *
                     GmatMathConstants::DEG_PER_RAD;

               // Elevation derivative
               currentDerivatives[1][i] = elPrefactor * elMat(2, i) *
                     GmatMathConstants::DEG_PER_RAD;
            }

            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("Position az Derivative: [%.12lf "
                     "%.12lf %.12lf]\n",
                     currentDerivatives[0][0], currentDerivatives[0][1],
                     currentDerivatives[0][2]);
               MessageInterface::ShowMessage("Position el Derivative: [%.12lf "
                     "%.12lf %.12lf]\n", currentDerivatives[1][0],
                     currentDerivatives[1][1], currentDerivatives[1][2]);
            #endif
         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            // Get the inverse of the orbit STM for the object supporting the
            // solve-for parameter (i.e. for obj) at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);

            Real cosAz = GmatMathUtil::Cos(currentMeasurement.value[0] *
                  GmatMathConstants::RAD_PER_DEG);
            // This is 1/sec^2(Az) = cos^2(Az):
            Real cos2Az = cosAz*cosAz;

            // This is 1/cos(El)
            Real secEl = 1.0 / GmatMathUtil::Cos(currentMeasurement.value[1] *
                  GmatMathConstants::RAD_PER_DEG);

            #ifdef DEBUG_DERIVATIVE_DETAILS
                MessageInterface::ShowMessage("   1/sec^2(az) = %.12lf\n",
                      cos2Az);
                MessageInterface::ShowMessage("   1/cos(el) = %.12lf\n", secEl);
            #endif

            Rvector3 xUnit(1.0, 0.0, 0.0), yUnit(0.0, 1.0, 0.0),
                     zUnit(0.0, 0.0, 1.0);

            Rmatrix33 identity(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

            // Range vector in the observer's coordinate system
            Real range = obsRange.GetMagnitude();
            Rvector3 unitRange = obsRange / range;
            Rmatrix33 rangeRangeT, rangeXT, azPart, elPart;
            Real xTRange = unitRange[0];

            for (Integer i = 0; i < 3; ++i)
               for (Integer j = 0; j < 3; ++j)
               {
                  rangeRangeT(i,j) = unitRange[i] * unitRange[j];
                  rangeXT(i,j) = unitRange[i] * xUnit[j] / xTRange;
               }

            #ifdef DEBUG_DERIVATIVE_DETAILS
               MessageInterface::ShowMessage("   range = %.12lf\n", range);
               MessageInterface::ShowMessage("   unit range vector = %s\n",
                     unitRange.ToString().c_str());
               MessageInterface::ShowMessage("   x^T unitRange = %.12lf\n",
                     xTRange);
               MessageInterface::ShowMessage("   range range^T = %s\n",
                     rangeRangeT.ToString().c_str());
               MessageInterface::ShowMessage("   range x^T = %s\n",
                     rangeXT.ToString().c_str());
            #endif

            for (Integer i = 0; i < 3; ++i)
               for (Integer j = 0; j < 3; ++j)
               {
                  azPart(i,j) = rangeXT(i,j) - identity(i,j);
                  elPart(i,j) = identity(i,j) - rangeRangeT(i,j);
               }

            Rmatrix33 dRhoV, dRhoVObs;
            GetRangeVectorDerivative(lightPath, stmInv, dRhoV, true, 1, 0,
                  false, true);

            // Build the range  derivative times the rotation matrix
            dRhoVObs = -R_o_j2k * dRhoV;

            Real azPrefactor = cos2Az / (range * xTRange);
            Real elPrefactor = secEl/range;

            Rmatrix33 azMatV = azPart * dRhoVObs;
            Rmatrix33 elMatV = elPart * dRhoVObs;

            #ifdef DEBUG_DERIVATIVE_DETAILS
               MessageInterface::ShowMessage("   azPart = %s\n",
                     azPart.ToString().c_str());
               MessageInterface::ShowMessage("   elPart = %s\n",
                     elPart.ToString().c_str());
               MessageInterface::ShowMessage("   R_o_j2k =\n");
               for (Integer i = 0; i < 3; ++i)
               {
                  MessageInterface::ShowMessage("     ");
                  for (Integer j = 0; j < 3; ++j)
                     MessageInterface::ShowMessage(" %.12lf ", R_o_j2k(i,j));
                  MessageInterface::ShowMessage("\n");
               }
               MessageInterface::ShowMessage("\n");
               Rmatrix33 eventR = lightPath.GetEventData(participants[0]).
                     rInertial2obj;
               MessageInterface::ShowMessage("   eventR = %s\n",
                     eventR.ToString().c_str());
               MessageInterface::ShowMessage("   dRho/dr^I = %s\n",
                     dRho.ToString().c_str());
               MessageInterface::ShowMessage("   R dRho/dr^I = %s\n",
                     dRhoObs.ToString().c_str());

               MessageInterface::ShowMessage("   azPrefactor = %.12lf\n",
                     azPrefactor);
               MessageInterface::ShowMessage("   elPrefactor = %.12lf\n",
                     elPrefactor);

               MessageInterface::ShowMessage("   azMat = %s\n",
                     azMat.ToString().c_str());
               MessageInterface::ShowMessage("   elMat = %s\n",
                     elMat.ToString().c_str());
            #endif

            for (Integer i = 0; i < 3; ++i)
            {
               currentDerivatives[0][i] = azPrefactor * azMatV(1, i) * 1.0;
               currentDerivatives[1][i] = elPrefactor * elMatV(2, i) * 1.0;
            }

            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("Velocity az Derivative: [%.12lf "
                     "%.12lf %.12lf]\n", currentDerivatives[0][0],
                     currentDerivatives[0][1], currentDerivatives[0][2]);
               MessageInterface::ShowMessage("Velocity el Derivative: [%.12lf "
                     "%.12lf %.12lf]\n", currentDerivatives[1][0],
                     currentDerivatives[1][1], currentDerivatives[1][2]);
            #endif
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            #ifdef DEBUG_DERIVATIVE_DETAILS
               MessageInterface::ShowMessage("Calculating derivative for az = "
                     "%.12lf, el = %.12lf\n", currentMeasurement.value[0],
                     currentMeasurement.value[1]);
            #endif

            // Get the inverse of the orbit STM for the object supporting the
            // solve-for parameter (i.e. for obj) at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);

            Real cosAz = GmatMathUtil::Cos(currentMeasurement.value[0] *
                  GmatMathConstants::RAD_PER_DEG);
            // This is 1/sec^2(Az) = cos^2(Az):
            Real cos2Az = cosAz*cosAz;

            // This is 1/cos(El)
            Real secEl = 1.0 / GmatMathUtil::Cos(currentMeasurement.value[1] *
                  GmatMathConstants::RAD_PER_DEG);

            #ifdef DEBUG_DERIVATIVE_DETAILS
                MessageInterface::ShowMessage("   1/sec^2(az) = %.12lf\n",
                      cos2Az);
                MessageInterface::ShowMessage("   1/cos(el) = %.12lf\n", secEl);
            #endif

            Rvector3 xUnit(1.0, 0.0, 0.0), yUnit(0.0, 1.0, 0.0),
                     zUnit(0.0, 0.0, 1.0);

            Rmatrix33 identity(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

            // Range vector in the observer's coordinate system
            Real range = obsRange.GetMagnitude();
            Rvector3 unitRange = obsRange / range;
            Rmatrix33 rangeRangeT, rangeXT, azPart, elPart;
            Real xTRange = unitRange[0];

            for (Integer i = 0; i < 3; ++i)
               for (Integer j = 0; j < 3; ++j)
               {
                  rangeRangeT(i,j) = unitRange[i] * unitRange[j];
                  rangeXT(i,j) = unitRange[i] * xUnit[j] / xTRange;
               }

            #ifdef DEBUG_DERIVATIVE_DETAILS
               MessageInterface::ShowMessage("   range = %.12lf\n", range);
               MessageInterface::ShowMessage("   unit range vector = %s\n",
                     unitRange.ToString().c_str());
               MessageInterface::ShowMessage("   x^T unitRange = %.12lf\n",
                     xTRange);
               MessageInterface::ShowMessage("   range range^T = %s\n",
                     rangeRangeT.ToString().c_str());
               MessageInterface::ShowMessage("   range x^T = %s\n",
                     rangeXT.ToString().c_str());
            #endif

            for (Integer i = 0; i < 3; ++i)
               for (Integer j = 0; j < 3; ++j)
               {
                  azPart(i,j) = rangeXT(i,j) - identity(i,j);
                  elPart(i,j) = identity(i,j) - rangeRangeT(i,j);
               }

            Rmatrix33 dRho, dRhoObs, dRhoV, dRhoVObs;
            GetRangeVectorDerivative(lightPath, stmInv, dRho, true, 1, 0,
                  true, false);
            GetRangeVectorDerivative(lightPath, stmInv, dRhoV, true, 1, 0,
                  false, true);

            // Build the range  derivative times the rotation matrix
            dRhoObs  = -R_o_j2k * dRho;
            dRhoVObs = -R_o_j2k * dRhoV;

            Real azPrefactor = cos2Az / (range * xTRange);
            Real elPrefactor = secEl/range;

            Rmatrix33 azMat  = azPart * dRhoObs;
            Rmatrix33 elMat  = elPart * dRhoObs;
            Rmatrix33 azMatV = azPart * dRhoVObs;
            Rmatrix33 elMatV = elPart * dRhoVObs;

            #ifdef DEBUG_DERIVATIVE_DETAILS
               MessageInterface::ShowMessage("   azPart = %s\n",
                     azPart.ToString().c_str());
               MessageInterface::ShowMessage("   elPart = %s\n",
                     elPart.ToString().c_str());
               MessageInterface::ShowMessage("   R_o_j2k =\n");
               for (Integer i = 0; i < 3; ++i)
               {
                  MessageInterface::ShowMessage("     ");
                  for (Integer j = 0; j < 3; ++j)
                     MessageInterface::ShowMessage(" %.12lf ", R_o_j2k(i,j));
                  MessageInterface::ShowMessage("\n");
               }
               MessageInterface::ShowMessage("\n");
               Rmatrix33 eventR = lightPath.GetEventData(participants[0]).
                     rInertial2obj;
               MessageInterface::ShowMessage("   eventR = %s\n",
                     eventR.ToString().c_str());
               MessageInterface::ShowMessage("   dRho/dr^I = %s\n",
                     dRho.ToString().c_str());
               MessageInterface::ShowMessage("   R dRho/dr^I = %s\n",
                     dRhoObs.ToString().c_str());

               MessageInterface::ShowMessage("   azPrefactor = %.12lf\n",
                     azPrefactor);
               MessageInterface::ShowMessage("   elPrefactor = %.12lf\n",
                     elPrefactor);

               MessageInterface::ShowMessage("   azMat = %s\n",
                     azMat.ToString().c_str());
               MessageInterface::ShowMessage("   elMat = %s\n",
                     elMat.ToString().c_str());
            #endif

            for (Integer i = 0; i < 3; ++i)
            {
               // Azimuth derivative
               currentDerivatives[0][i] = azPrefactor * azMat(1, i) *
                     GmatMathConstants::DEG_PER_RAD;

               // Elevation derivative
               currentDerivatives[1][i] = elPrefactor * elMat(2, i) *
                     GmatMathConstants::DEG_PER_RAD;

               // Velocity derivatives are zero
               currentDerivatives[0][i+3] = azPrefactor * azMatV(1, i) * 1.0;
               currentDerivatives[1][i+3] = elPrefactor * elMatV(2, i) * 1.0;
            }
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("CartState az Derivative: [%.12lf "
                     "%.12lf %.12lf %.12lf %.12lf %.12lf]\n",
                     currentDerivatives[0][0], currentDerivatives[0][1],
                     currentDerivatives[0][2], currentDerivatives[0][3],
                     currentDerivatives[0][4], currentDerivatives[0][5]);
               MessageInterface::ShowMessage("CartState el Derivative: [%.12lf "
                     "%.12lf %.12lf %.12lf %.12lf %.12lf]\n",
                     currentDerivatives[1][0], currentDerivatives[1][1],
                     currentDerivatives[1][2], currentDerivatives[1][3],
                     currentDerivatives[1][4], currentDerivatives[1][5]);
            #endif
         }
         else if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
            {
               currentDerivatives[0][i] = 1.0;
               currentDerivatives[1][i] = 1.0;
            }
         }
         else
         {
            for (UnsignedInt i = 0; i < 3; ++i)
            {
               currentDerivatives[0][i] = 0.0;
               currentDerivatives[1][i] = 0.0;
            }
         }

      }
      else if (objNumber == 0) // measurement model
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of the "
                     "measurement model\n",
                     objPtr->GetParameterText(parameterID).c_str());
         #endif
         if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
            {
               currentDerivatives[0][i] = 1.0;
               currentDerivatives[1][i] = 1.0;
            }
         }
      }
      else
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of a non-"
                     "Participant\n",
                     objPtr->GetParameterText(parameterID).c_str());
         #endif
         for (Integer i = 0; i < size; ++i)
         {
            currentDerivatives[0][i] = 0.0;
            currentDerivatives[1][i] = 0.0;
         }
      }

      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   Deriv =\n   ");

         for (Integer i = 0; i < size; ++i)
            MessageInterface::ShowMessage("   %.12le",currentDerivatives[0][i]);
         MessageInterface::ShowMessage("\n");
         for (Integer i = 0; i < size; ++i)
            MessageInterface::ShowMessage("      %.12le",currentDerivatives[1][i]);
         MessageInterface::ShowMessage("\n\n");
      #endif
   }
   return currentDerivatives;
}


//------------------------------------------------------------------------------
// bool Evaluate(bool withEvents)
//------------------------------------------------------------------------------
/**
 * Calculates the measurement
 *
 * @param withEvents true to include light time iteration, false to omit it
 *
 * @return true if a feasible measurement was calculated, false if not
 */
//------------------------------------------------------------------------------
bool OpticalAzEl::Evaluate(bool withEvents)
{
   bool retval = false;

   if (!initialized)
      InitializeMeasurement();

   #ifdef DEBUG_OPTICAL_AZEL_CALC
      MessageInterface::ShowMessage("Entered OpticalAzEl::Evaluate()\n");
      MessageInterface::ShowMessage("  ParticipantCount: %d\n",
            participants.size());
   #endif

   if (withEvents == false)
   {
      #ifdef DEBUG_OPTICAL_AZEL_CALC
         MessageInterface::ShowMessage("Optical Azimuth-Elevation Calculation without "
               "events\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES
         DumpParticipantStates("++++++++++++++++++++++++++++++++++++++++++++\n"
               "Evaluating Optical Az-El without events");
      #endif

      CalculateRangeVectorInertial();

      // Set feasibility off of topocentric horizon, set by the Z value in topo
      // coords
      std::string updateAll = "All";
      UpdateRotationMatrix(currentMeasurement.epoch, updateAll);
      obsRange = R_o_j2k * rangeVecInertial;
      currentMeasurement.feasibilityValue = obsRange[2];

      if (currentMeasurement.feasibilityValue > 0.0)
      {
         #ifdef DEBUG_OPTICAL_AZEL_CALC
            MessageInterface::ShowMessage("   Optical Azimuth-Elevation using "
                     "topocentric range vector:\n      %.12lf  %.12lf  "
                     "%.12lf\n", obsRange[0], obsRange[1], obsRange[2]);
         #endif
         currentMeasurement.isFeasible = true;
         // Azimuth
         currentMeasurement.value[0] = atan2(obsRange[1], -obsRange[0]) *
                  GmatMathConstants::DEG_PER_RAD;
         // Range check and fix the angle
//         if (currentMeasurement.value[0] < 0.0)
//            currentMeasurement.value[0] += 360.0;
         if (currentMeasurement.value[0] > GmatMathConstants::PI_DEG)
            currentMeasurement.value[0] -= GmatMathConstants::TWO_PI_DEG;
         // Elevation
         currentMeasurement.value[1] = asin(obsRange[2] /
               obsRange.GetMagnitude()) * GmatMathConstants::DEG_PER_RAD;
         currentMeasurement.eventCount = 1;

         #ifdef DEBUG_OPTICAL_AZEL_CALC
            MessageInterface::ShowMessage("      Azimuth = %.12lf, "
                     "Elevation = %.12lf\n", currentMeasurement.value[0],
                     currentMeasurement.value[1]);
         #endif

         retval = true;
      }
      else
      {
         currentMeasurement.isFeasible = false;
         currentMeasurement.value[0] = 0.0;
         currentMeasurement.value[1] = 0.0;
         currentMeasurement.eventCount = 0;
      }

      #ifdef DEBUG_OPTICAL_AZEL_CALC
         MessageInterface::ShowMessage("Calculating Optical Azimuth-Elevation "
               "at epoch %.12lf\n", currentMeasurement.epoch);
         MessageInterface::ShowMessage("   Location of %s, id = '%s':  %s",
               participants[0]->GetName().c_str(),
               currentMeasurement.participantIDs[0].c_str(),
               p1Loc.ToString().c_str());
         MessageInterface::ShowMessage("   Location of %s, id = '%s':  %s",
               participants[1]->GetName().c_str(),
               currentMeasurement.participantIDs[1].c_str(),
               p2Loc.ToString().c_str());
         MessageInterface::ShowMessage("   Obs Range Vector:  %s\n",
               obsRange.ToString().c_str());
         MessageInterface::ShowMessage("   Feasibility =  %lf\n",
               currentMeasurement.feasibilityValue);
         MessageInterface::ShowMessage("   Feasibility:  %s\n",
               (currentMeasurement.isFeasible ? "true" : "false"));
         MessageInterface::ShowMessage("   Azimuth = %.12lf\n",
               currentMeasurement.value[0]);
         MessageInterface::ShowMessage("   Elevation = %.12lf\n",
               currentMeasurement.value[1]);
         MessageInterface::ShowMessage("   EventCount is %d\n",
               currentMeasurement.eventCount);
      #endif

      #ifdef SHOW_AZEL_CALC
         MessageInterface::ShowMessage("Az/El at epoch %.12lf is ",
                  currentMeasurement.epoch);
         if (currentMeasurement.isFeasible)
            MessageInterface::ShowMessage("feasible, value = [%.12lf %12lf]\n",
                  currentMeasurement.value[0], currentMeasurement.value[1]);
         else
            MessageInterface::ShowMessage("not feasible\n");
      #endif
   }
   else
   {
      // Calculate the corrected Az/El measurement
      #ifdef DEBUG_OPTICAL_AZEL_CALC
         MessageInterface::ShowMessage("Optical Azimuth/Elevation "
               "Calculation:\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES
           // This gives misleading data -- it is not light corrected when
           // generated from DumpParticipantStates.
//         DumpParticipantStates("********************************************\n"
//               "Evaluating Optical Azimuth/Elevation with located events");
      #endif

      // Get the range from the downlink
      Rvector3 r1, r2;
      r1 = lightPath.GetPosition(participants[0]);
      r2 = lightPath.GetPosition(participants[1]);
      Rvector3 rVector = r2 - r1;

      // Set feasibility off of topocentric horizon, set by the Z value in topo
      // coords
      std::string updateAll = "All";
      UpdateRotationMatrix(currentMeasurement.epoch, updateAll);
      obsRange = R_o_j2k * rVector;

      #ifdef DEBUG_OPTICAL_AZEL_CALC
         MessageInterface::ShowMessage("   Optical Topocentric Vector:  "
               "%.12lf  %.12lf  %.12lf\n", obsRange[0], obsRange[1],
               obsRange[2]);
      #endif


      currentMeasurement.value[0] = atan2(obsRange[1], -obsRange[0]) *
               GmatMathConstants::DEG_PER_RAD;
      // Range check and fix the angle
//      if (currentMeasurement.value[0] < 0.0)
//         currentMeasurement.value[0] += 360.0;
      if (currentMeasurement.value[0] > GmatMathConstants::PI_DEG)
         currentMeasurement.value[0] -= GmatMathConstants::TWO_PI_DEG;
      // Elevation
      currentMeasurement.value[1] = asin(obsRange[2] /
            obsRange.GetMagnitude()) * GmatMathConstants::DEG_PER_RAD;

      #ifdef DEBUG_OPTICAL_AZEL_CALC
         MessageInterface::ShowMessage("   Light corrected P1 Position:  %s",
               r1.ToString().c_str());
         MessageInterface::ShowMessage("   Light corrected P2 Position:  %s\n",
               r2.ToString().c_str());
         MessageInterface::ShowMessage("   Calculated Az/El:  %.12lf %.12lf\n",
               currentMeasurement.value[0], currentMeasurement.value[1]);
      #endif

      currentMeasurement.isFeasible = true;
      retval = true;
   }

   return retval;
}
