//$Id$
//------------------------------------------------------------------------------
//                              RungeKutta
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// *** File Name : RungeKutta.cpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 1/15/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 2/19/2003 - D. Conway, Thinking Systems, Inc.
//                             Added code to manage min and max allowed
//                             stepsizes
//
//                           : 3/3/2003 - D. Conway, Thinking Systems, Inc.
//                             Fixed bug found with final step while stepping
//                             backwards
//
//                           : 09/24/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/21/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                              Removals:
//                              Additions:
// **************************************************************************

#include "gmatdefs.hpp"
#include "RungeKutta.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PROPAGATOR_FLOW
//#define DEBUG_RAW_STEP_STATE

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// RungeKutta::RungeKutta(Integer st, Integer order)
//------------------------------------------------------------------------------
/**
 * The RK Constructor
 *
 * @param st    Number of stages in the specific algorithm implemented
 * @param order Order of the expansion used for the integrator
 */
//------------------------------------------------------------------------------
RungeKutta::RungeKutta(Integer st, Integer order, const std::string &typeStr,
                                           const std::string &nomme) :
    Integrator      (typeStr, nomme),
    stages          (st),
    ki              (NULL),
    ai              (NULL),
    bij             (NULL),
    cj              (NULL),
    ee              (NULL),
    sigma           (0.9),
    incPower        (1.0/order),
    decPower        (1.0/(order-1)),
    stageState      (NULL),
    candidateState  (NULL)
{
}

//------------------------------------------------------------------------------
// RungeKutta::~RungeKutta(void)
//------------------------------------------------------------------------------
/**
 * The RK destructor
 */
//------------------------------------------------------------------------------
RungeKutta::~RungeKutta()
{
    ClearArrays();
}

//------------------------------------------------------------------------------
// RungeKutta::RungeKutta(const RungeKutta& rk)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
//------------------------------------------------------------------------------
RungeKutta::RungeKutta(const RungeKutta& rk) :
    Integrator      (rk),
    stages          (rk.stages),
    ki              (NULL),
    ai              (NULL),
    bij             (NULL),
    cj              (NULL),
    ee              (NULL),
    sigma           (rk.sigma),
    incPower        (rk.incPower),
    decPower        (rk.decPower),
    stageState      (NULL),
    candidateState  (NULL)
{
}

//------------------------------------------------------------------------------
// RungeKutta& RungeKutta::operator=(const RungeKutta& rk)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
RungeKutta& RungeKutta::operator=(const RungeKutta& rk)
{
    if (this == &rk)
        return *this;

    Integrator::operator=(rk);
    stages = rk.stages;
    sigma = rk.sigma;
    incPower = rk.incPower;
    decPower = rk.decPower;

    ClearArrays();

    ki = NULL;
    ai = NULL;
    bij = NULL;
    cj = NULL;
    ee = NULL;
    stageState = NULL;
    candidateState = NULL;

    isInitialized = false;

    return *this;
}

//------------------------------------------------------------------------------
// void RungeKutta::SetPhysicalModel(PhysicalModel *pPhysicalModel)
//------------------------------------------------------------------------------
/**
 * Sets up the derivative source
 *
 * This method calls the Integrator::SetDerivatives() method.  If that method
 * succeeds and the system reports that it is initialized, the accumulator
 * arrays are also setup.
 */
//------------------------------------------------------------------------------
void RungeKutta::SetPhysicalModel(PhysicalModel *pPhysicalModel)
{
    Integrator::SetPhysicalModel(pPhysicalModel);
    if (isInitialized)
       SetupAccumulator();
}

//------------------------------------------------------------------------------
// bool RungeKutta::Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the internal data structures
 *
 * This method allocates the data arrays and calls the functions that set the
 * RK coefficients and accumulator array (ki) memory allocation.  If any of the
 * allocation fails, the internal initialized flag is set to false.
 *
 * @return true upon successful initialization
 */
//------------------------------------------------------------------------------
bool RungeKutta::Initialize()
{
    Propagator::Initialize();
    if (stages <= 0)
    {
        isInitialized = false;
        return isInitialized;
    }

    ClearArrays();

    if ((ai = new Real [stages]) == NULL)
    {
        isInitialized = false;
        return isInitialized;
    }
    if ((bij = new Real*[stages]) == NULL)
    {
        delete [] ai;
        isInitialized = false;
        return isInitialized;
    }
    if ((cj = new Real [stages]) == NULL)
    {
        delete [] ai;
        delete [] bij;
        isInitialized = false;
        return isInitialized;
    }
    if ((ki = new Real*[stages]) == NULL)
    {
        delete [] ai;
        delete [] bij;
        delete [] cj;
        isInitialized = false;
        return isInitialized;
    }
    if ((ee = new Real [stages]) == NULL)
    {
        delete [] ai;
        delete [] bij;
        delete [] cj;
        delete [] ki;
        isInitialized = false;
        return isInitialized;
    }

    for (int i = 0; i < stages; i++)
    {
        ki[i] = NULL;
        if ((bij[i] = new Real[i+1]) == NULL)
        {
            for (int j = 0; j < i; j++)
                delete [] bij[j];
            delete [] ai;
            delete [] bij;
            delete [] cj;
            delete [] ki;
            isInitialized = false;
            return isInitialized;
        }
    }

    if (derivativeOrder == 1)
    {
       SetCoefficients();
       SetupAccumulator();
    }

    isInitialized = true;
    return true;
}

//------------------------------------------------------------------------------
// bool RungeKutta::Step()
//------------------------------------------------------------------------------
/**
 * The RK propagation method
 *
 * This method implements the heart of the Runge-Kutta integration
 *
 * @return      true if a good step was taken, false if no good step was found or
 *              if another failure was encountered (a bad force model call, for
 *              example)
 */
//------------------------------------------------------------------------------
bool RungeKutta::Step()
{
    #ifdef DEBUG_PROPAGATOR_FLOW
       MessageInterface::ShowMessage(".");
    #endif

    // Force epoch updates at every step (a test, but left in place in case
    // it's needed later -- DJC
    // ((ForceModel*)physicalModel)->UpdateInitialData();

    if (!isInitialized)
    {
       MessageInterface::ShowMessage("RK not initialized\n");
       return false;
    }

    if ((fabs(stepSize) < minimumStep) && !finalStep)
        stepSize = ((stepSize > 0.0) ? minimumStep : -minimumStep);
    if (fabs(stepSize) > maximumStep)
        stepSize = ((stepSize > 0.0) ? maximumStep : -maximumStep);

    bool goodStepTaken = false;
    Real maxerror;

    do
    {
        if (!RawStep())
           return false;

        maxerror = EstimateError();
        stepTaken = stepSize;
        if (maxerror!= 0.0)
        {
           if (AdaptStep(maxerror))
              goodStepTaken = true;
        }
        else  // 0.0 means no error control; in that case leave step alone
        {
           memcpy(outState, candidateState, dimension*sizeof(Real));
           stepAttempts = 0;
           goodStepTaken = true;
        }

        if (stepAttempts >= maxStepAttempts)
        {
           MessageInterface::ShowMessage("%d step attempts taken; max is %d\n",
                 stepAttempts, maxStepAttempts);
           return false;
        }
    } while (!goodStepTaken);

    physicalModel->IncrementTime(stepTaken);
    return true;
}

//------------------------------------------------------------------------------
// bool RungeKutta::RawStep()
//------------------------------------------------------------------------------
/**
 * Take a raw prop step without any error control
 */
//------------------------------------------------------------------------------
bool RungeKutta::RawStep()
{
   #ifdef DEBUG_PROPAGATOR_FLOW
      MessageInterface::ShowMessage("*");
   #endif

   Integer i, j, k;

//   Real *inState = new Real[dimension];

   if (inState != physicalModel->GetState())
      memcpy(inState, physicalModel->GetState(), sizeof(Real) * dimension);

   #ifdef DEBUG_RAW_STEP_STATE
      MessageInterface::ShowMessage("inState: [");
      for (Integer q = 0; q < dimension-1; ++q)
        MessageInterface::ShowMessage("%le, ", inState[q]);
      MessageInterface::ShowMessage("%le]\n", inState[dimension-1]);
   #endif

   // Calculate the stages
   for (i = 0; i < stages; i++)
   {
      memcpy(stageState, inState, sizeof(Real) * dimension);

      #ifdef DEBUG_RAW_STEP_STATE
         MessageInterface::ShowMessage("inState[%d]: [", i);
         for (Integer q = 0; q < dimension-1; ++q)
                MessageInterface::ShowMessage("%le, ", inState[q]);
         MessageInterface::ShowMessage("%le]\n", inState[dimension-1]);
         MessageInterface::ShowMessage("stageState: [");
         for (Integer q = 0; q < dimension-1; ++q)
                MessageInterface::ShowMessage("%le, ", stageState[q]);
         MessageInterface::ShowMessage("%le]\n", stageState[dimension-1]);
      #endif

      // Built the accumulated state if this is not the initial step
      if (i > 0)
      {
         for (j = 0; j < i; j++)
         {
            for (k = 0; k < dimension; k++)
               stageState[k] += bij[i][j] * ki[j][k];
         }
      }

      if (!physicalModel->GetDerivatives(stageState, stepSize * ai[i]))
      {
         return false;
      }

      #ifdef DEBUG_RAW_STEP_STATE
         MessageInterface::ShowMessage("ddt data: [");
         for (Integer q = 0; q < dimension-1; ++q)
                MessageInterface::ShowMessage("%le, ", ddt[q]);
         MessageInterface::ShowMessage("%le]\n", ddt[dimension-1]);
         MessageInterface::ShowMessage("++ After GetDerivatives,  stageState: [");
         for (Integer q = 0; q < dimension-1; ++q)
            MessageInterface::ShowMessage("%le, ", stageState[q]);
         MessageInterface::ShowMessage("%le]\n", stageState[dimension-1]);
      #endif

      for (j = 0; j < dimension; j++)
         ki[i][j] = stepSize * ddt[j];
   }

   // Accumulate the stages for the RK step
   memcpy(candidateState, inState, dimension*sizeof(Real));
   for (i = 0; i < stages; i++)
   {
      for (j = 0; j < dimension; j++)
      {
         candidateState[j] += cj[i] * ki[i][j];
      }
   }

   #ifdef DEBUG_RAW_STEP_STATE
      MessageInterface::ShowMessage("candidateState: [");
      for (Integer q = 0; q < dimension-1; ++q)
         MessageInterface::ShowMessage("%le, ", candidateState[q]);
      MessageInterface::ShowMessage("%le]\n", candidateState[dimension-1]);
   #endif

//   delete [] inState;

   return true;
}

//------------------------------------------------------------------------------
// bool RungeKutta::Step(Real dt)
//------------------------------------------------------------------------------
/**
 * Method used to step a fixed time
 *
 * @param dt    The time interval to step
 */
//------------------------------------------------------------------------------
bool RungeKutta::Step(Real dt)
{
    bool stepFinished = false;
    timeleft = dt;
    Integer attemptsTaken = 0;
    do
    {
        if (attemptsTaken > maxStepAttempts)
        {
           MessageInterface::ShowMessage(
              "    Integrator attempted too many steps! (%d attempts "
              "taken)\n", attemptsTaken);
           return false;
        }
        if (!Propagator::Step(timeleft))
            return false;
        if (fabs(timeleft - stepTaken) <= smallestTime)
            stepFinished = true;
        timeleft -= stepTaken;
        ++attemptsTaken;
    } while (stepFinished == false);

    return true;
}

//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// Real RungeKutta::EstimateError(void)
//------------------------------------------------------------------------------
/**
 * Provides the greatest relative error in the state vector
 *
 * This method takes the state vector and calculates the error in each
 * component.  The error is then divided by the change in the component.  The
 * function returns the largest of the resulting relative errors.
 *
 * Override this method if you want a different error estimate for the stepsize
 * control.  For example, we are using
 *
 * \f[error_i = \left({{\Delta_i(t+\delta t)}\over
 *                  {r_i(t+\delta t) - r_i(t)}}\right)\f]
 *
 * Another popular approach is to divide the estimated error \f$\Delta_i\f$ by
 * the norm of the corresponding 3-vector; for instance, divide the error in x
 * by the magnitude of the displacement in position for the step.
 */
//------------------------------------------------------------------------------
Real RungeKutta::EstimateError()
{
    Integer i, j;

    for (i = 0; i < dimension; i++)
    {
        errorEstimates[i] = 0.0;
        for (j = 0; j < stages; j++)
        {
            errorEstimates[i] += ee[j] * ki[j][i];
        }
    }

    // Find the maximum error
    return physicalModel->EstimateError(errorEstimates, candidateState);
}

//------------------------------------------------------------------------------
// bool RungeKutta::AdaptStep(Real maxerror)
//------------------------------------------------------------------------------
/**
 * Adjust the stepsize to maintain the accuracy and step constraints
 *
 *  Note that there is a kludge in this method to force the RK integrator to
 *  accept bad steps -- steps with too much error -- if the stepsize == the
 *  minimum value.
 *
 *  This is not the correct approach; that means the system can run without error
 *  control, and without reporting the bad steps.  It has been put in place to
 *  allow propagation across the SRP discontinuity experienced by a satellite
 *  crossing into or out of shadow when using a cylindrical shadow model.
 *
 *  This issue needs to be reexamined once the system is functional.
 *
 *  @param maxerror     Maximum error found in the current attempt
 *
 *  @return             true if the step is accepted, false if not
 */
//------------------------------------------------------------------------------
bool RungeKutta::AdaptStep(Real maxerror)
{
    // Adapt the step if necessary
    if (maxerror > tolerance)  // Too much error, so need to try again
    {

        // The following code is in place to omit error control for steps
        // at the minimum stepsize.  See the note above for more
        // information.  Remove this block if the issue gets resolved.
        if (GmatMathUtil::Abs(stepSize) == minimumStep)
        {
           if (stopIfAccuracyViolated)
           {
              throw PropagatorException(typeSource + ": Accuracy settings will "
                    "be violated with current step size values.\n");
           }
           else
           {
              if (!accuracyWarningTriggered) // so only write the warning once per propagation command
              {
                 accuracyWarningTriggered = true;
                 MessageInterface::ShowMessage("**** Warning **** %s: Accuracy "
                       "settings will be violated with current step size "
                       "values.\n", typeSource.c_str());
              }
               // Do this if the step was at the minimum stepSize
              memcpy(outState, candidateState, dimension*sizeof(Real));
              stepAttempts = 0;
              return true;
           }
        }

        stepSize = sigma * stepSize * pow(tolerance/maxerror, decPower);

        if (fabs(stepSize) < minimumStep)
            stepSize = ((stepSize < 0.0) ? -minimumStep : minimumStep);
        ++stepAttempts;

        // Flag this as an unacceptable step
        return false;
    }

    // Increase the stepsize, but keep this step
    stepSize = sigma * stepSize * pow(tolerance/maxerror, incPower);
    memcpy(outState, candidateState, dimension*sizeof(Real));
    stepAttempts = 0;

    // Identify the step as accepted
    return true;
}

//------------------------------------------------------------------------------
// void RungeKutta::ClearArrays(void)
//------------------------------------------------------------------------------
/**
 * Deallocates and NULLs the array data structures
 */
//------------------------------------------------------------------------------
void RungeKutta::ClearArrays()
{
    if (ki != NULL)
    {
        for (int i = 0; i < stages; i++)
        {
            if (ki[i] != NULL)
            {
                delete [] ki[i];
                ki[i] = NULL;
            }
        }
        delete [] ki;
    }

    if (ai != NULL)
        delete [] ai;

    if (bij != NULL)
    {
        for (int i = 0; i < stages; i++)
        {
            if (bij[i] != NULL)
                delete [] bij[i];
        }
        delete [] bij;
    }

    if (cj != NULL)
        delete [] cj;

    if (ee != NULL)
        delete [] ee;

    if (stageState != NULL)
        delete [] stageState;

    if (candidateState != NULL)
    {
        delete [] candidateState;
    }

    ki = bij = NULL;
    ai = cj = ee = stageState = candidateState = NULL;
    //    ai = cj = ee = stageState = candidateState = errorEstimates = NULL;
}

//------------------------------------------------------------------------------
// bool RungeKutta::SetupAccumulator()
//------------------------------------------------------------------------------
/**
 * Sets memory for the stageState and the members of the ki array
 */
//------------------------------------------------------------------------------
bool RungeKutta::SetupAccumulator()
{
    if (physicalModel)
    {
        isInitialized = true;
        dimension = physicalModel->GetDimension();

        if (stageState)
            delete [] stageState;

        if ((stageState = new Real[dimension]) == NULL)
        {
            isInitialized = false;
            return false;
        }

        if (candidateState)
            delete [] candidateState;

        if ((candidateState = new Real[dimension]) == NULL)
        {
            isInitialized = false;
            delete [] stageState;
            stageState = NULL;
            return false;
        }

        ddt = physicalModel->GetDerivativeArray();

        if (errorEstimates)
            delete [] errorEstimates;

        if ((errorEstimates = new Real[dimension]) == NULL)
        {
            isInitialized = false;
            delete [] stageState;
            stageState = NULL;
            delete [] candidateState;
            candidateState = NULL;
            return false;
        }

        if (ki)
        {
            for (int i = 0; i < stages; i++)
            {
                if (ki[i])
                    delete [] ki[i];
                if ((ki[i] = new Real[dimension]) == NULL)
                {
                    isInitialized = false;
                    delete [] stageState;
                    stageState = NULL;
                    delete [] candidateState;
                    candidateState = NULL;
                    delete [] errorEstimates;
                    errorEstimates = NULL;
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}
