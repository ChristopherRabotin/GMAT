//------------------------------------------------------------------------------
//                              PredictorCorrector
//------------------------------------------------------------------------------
// *** File Name : predictorcorrector.cpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
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
//                           : 1/7/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 2/18/2003 - D. Conway, Thinking Systems, Inc.
//                             Reworked step control for fixed step mode
//
//                           : 3/3/2003 - D. Conway, Thinking Systems, Inc.
//                             Moved time step granularity to Integrator
//                             class
//
//                           : 6/17/2004 - A. Greene, Missions Applications
//                              Changes:
//                                - Updated style using GMAT cpp style guide
// **************************************************************************


#include "PredictorCorrector.hpp"
#include <fstream>


//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// PredictorCorrector::PredictorCorrector(Integer sc, Integer order, 
//                  const std::string &typeStr, const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * The Predictor-Corrector Constructor 
 *
 * @param sc	Size of the Predictor-Corrector buffer needed to advance
 *                   the state (basically the amount of history that needs to be
 *                   preserved from step to step)
 *
 * @param order	Truncation order of the highest order term kept in the
 *                   series expansion used to construct the algorithm.  This
 *                   value, inverted, is used in the stepsize control algorithm
 *                   to determine new step sizes if the estimated error falls
 *                   outside of the target error bounds.
 */
//------------------------------------------------------------------------------
PredictorCorrector::PredictorCorrector(Integer sc, Integer order, 
                  const std::string &typeStr, const std::string &nomme) :
    Integrator      (typeStr, nomme),
    stepCount                       (sc),       //ag: changed from sc
    ddt                             (NULL),
    history                         (NULL),
    pweights                        (NULL),
    cweights                        (NULL),
    predictorState                  (NULL),
    correctorState                  (NULL),
    maxError                        (0.0),
    lowerError                      (1.0e-13),
    targetError                     (1.0e-11),
    stepSign                        (1.0),
    startupComplete                 (false),
    startupCount                    (0),
    starter                         (NULL),
    invOrder                        (1.0/order),
    PREDICTORCORRECTOR_LOWERERROR   (parameterCount + 1),
    PREDICTORCORRECTOR_TARGETERROR  (parameterCount + 2)
{
    parameterCount = PREDICTORCORRECTOR_TARGETERROR;
    tolerance = 1.0e-10;
}


//------------------------------------------------------------------------------
// PredictorCorrector::~PredictorCorrector(void)
//------------------------------------------------------------------------------
/**
 * The PredictorCorrector destructor
 */
//------------------------------------------------------------------------------
PredictorCorrector::~PredictorCorrector(void)
{
    if (history) {
        for (int i = 0; i < stepCount; i++)
            if (history[i])
                delete [] history[i];
        delete [] history;
    }

    if (pweights)
        delete [] pweights;

    if (cweights)
        delete [] cweights;

    if (predictorState)
        delete [] predictorState;

    if (correctorState)
        delete [] correctorState;

    if (starter)
        delete starter;
}


//------------------------------------------------------------------------------
// PredictorCorrector::PredictorCorrector(const PredictorCorrector& pc) 
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
//------------------------------------------------------------------------------
PredictorCorrector::PredictorCorrector(const PredictorCorrector& pc) :
    Integrator                      (pc),
    stepCount                       (pc.stepCount),
    ddt                             (NULL),
    history                         (NULL),
    pweights                        (NULL),
    cweights                        (NULL),
    predictorState                  (NULL),
    correctorState                  (NULL),
    maxError                        (pc.maxError),
    lowerError                      (pc.lowerError),
    targetError                     (pc.targetError),
    stepSign                        (pc.stepSign),
    startupComplete                 (false),
    startupCount                    (0),
    starter                         (NULL),
    invOrder                        (pc.invOrder),
    PREDICTORCORRECTOR_LOWERERROR   (parameterCount + 1),
    PREDICTORCORRECTOR_TARGETERROR  (parameterCount + 2)
{
    parameterCount = PREDICTORCORRECTOR_TARGETERROR;
    tolerance = pc.tolerance;
    initialized = false;
}


//------------------------------------------------------------------------------
// PredictorCorrector& PredictorCorrector::operator=(const PredictorCorrector& pc)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
PredictorCorrector& PredictorCorrector::operator=(const PredictorCorrector& pc)
{
    if (this == &pc)
        return *this;

    Integrator::operator=(pc);
    stepCount = pc.stepCount;
    maxError = pc.maxError;
    lowerError = pc.lowerError;
    targetError = pc.targetError;
    stepSign = pc.stepSign;
    startupComplete = false;
    startupCount = 0;
    invOrder = pc.invOrder;
    initialized = false;

    return *this;
}

//------------------------------------------------------------------------------
// std::string PredictorCorrector::GetParameterName(const int parm) const
//------------------------------------------------------------------------------
/**
 * Helper method used to find the text name for the class parameters
 *
 * This static method is used to obtain text descriptions for the parameters 
 * defined by the class.  These strings can be used to fill in the descriptors
 * for user interface elements, so that those elements have labels that match 
 * the intent of each of the user selectable parameters.
 *
 * @param parm ID of the parameter that needs the text string
 */
//------------------------------------------------------------------------------
std::string PredictorCorrector::GetParameterName(const int parm) const
{
    if (parm == PREDICTORCORRECTOR_LOWERERROR)
        return "Minimum Allowed Error";

    else if (parm == PREDICTORCORRECTOR_TARGETERROR)
        return "Nominal Allowed Error";

    else return Integrator::GetParameterText(parm);
}

//------------------------------------------------------------------------------
// Real PredictorCorrector::GetParameter(const int id) const
//------------------------------------------------------------------------------
/**
 * Method used to retrieve predictor-corrector parameters 
 */
//------------------------------------------------------------------------------
Real PredictorCorrector::GetParameter(const int id) const
{
    if (id == PREDICTORCORRECTOR_LOWERERROR)
        return lowerError;

    if (id == PREDICTORCORRECTOR_TARGETERROR)
        return targetError;

    return Integrator::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// bool PredictorCorrector::SetParameter(const int id, const Real val)
//------------------------------------------------------------------------------
/**
 * Method used to set predictor-corrector parameters
 *
 * This is the SetParameter method for the Predictor-Corrector implementations.
 * Each Predictor-Corrector has a startup propagator, starter, that may also 
 * have the parameters passed into this method.  The starter is given the 
 * opportunity to accept the parameters that correspond to the Integrator
 * parameters (but not the Predictor-Corrector parameters).  The return value 
 * from the starter setup is not checked for validity.
 *
 * @ param id     ID for the parameter
 * @ param val    New value for the parameter
 *
 * @ return true if the id is valid and the Predictor-Corrector accepts the value
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::SetParameter(const int id, const Real val)
{
    if (id == PREDICTORCORRECTOR_LOWERERROR)
        lowerError = val;
    else if (id == PREDICTORCORRECTOR_TARGETERROR)
        targetError = val;
    else {
        if (id == GetParameterID("StepSize"))   // ag: was STEP_SIZE_PARAMETER
            if (val == 0.0)
                return false;
        
        stepSign = (val < 0.0 ? -1.0 : 1.0);
        starter->SetRealParameter(id, val);

        return Integrator::SetRealParameter(id, val);
    }
    return true;
}

//------------------------------------------------------------------------------
// void PredictorCorrector::Initialize(void)
//------------------------------------------------------------------------------
/**
 * Method used to setup the data structures for the algorithm 
 */
//------------------------------------------------------------------------------
void PredictorCorrector::Initialize(void)
{
//    Propagator::Initialize();

    initialized = false;
    if (stepCount <= 0)
        return;

    if (physicalModel) {
        dimension = physicalModel->GetDimension();

        if (history) {
            for (int i = 0; i < stepCount; i++)
                if (history[i])
                    delete [] history[i];
            delete [] history;
            history = NULL;
        }

        if (pweights) {
            delete [] pweights;
            pweights = NULL;
        }

        if (cweights) {
            delete [] cweights;
            cweights = NULL;
        }

        if (predictorState) {
            delete [] predictorState;
            predictorState = NULL;
        }

        if (correctorState) {
            delete [] correctorState;
            correctorState = NULL;
        }

        if (errorEstimates) {
            delete [] errorEstimates;
            errorEstimates = NULL;
        }

        predictorState = new Real[dimension];
        if (!predictorState) {
            return;
        }

        correctorState = new Real[dimension];
        if (!correctorState) {
            delete [] predictorState;
            predictorState = NULL;
            return;
        }

        errorEstimates = new Real[dimension];
        if (!errorEstimates) {
            delete [] predictorState;
            predictorState = NULL;
            delete [] correctorState;
            correctorState = NULL;
            return;
        }

        pweights = new Real[stepCount];
        if (!pweights) {
            delete [] predictorState;
            predictorState = NULL;
            delete [] correctorState;
            correctorState = NULL;
            delete [] errorEstimates;
            errorEstimates = NULL;
            return;
        }

        cweights = new Real[stepCount];
        if (!cweights) {
            delete [] pweights;
            pweights = NULL;
            delete [] predictorState;
            predictorState = NULL;
            delete [] correctorState;
            correctorState = NULL;
            delete [] errorEstimates;
            errorEstimates = NULL;
            return;
        }

        history = new Real*[stepCount];
        if (history) {
            for (int i = 0; i < stepCount; i++) {
                history[i] = new Real[dimension];
                if (!history[i]) {
                    for (int j = 0; j < i; j++) {
                        delete [] history[j];
                    }
                    delete [] history;
                    history = NULL;
                    delete [] pweights;
                    pweights = NULL;
                    delete [] cweights;
                    cweights = NULL;
                    delete [] predictorState;
                    predictorState = NULL;
                    delete [] correctorState;
                    correctorState = NULL;
                    delete [] errorEstimates;
                    errorEstimates = NULL;
                    return;
                }
                if (SetWeights()) {
                    ddt = physicalModel->GetDerivativeArray();
                    initialized = true;
                }
            }
        }
        else {
            delete [] pweights;
            pweights = NULL;
            delete [] cweights;
            cweights = NULL;
            delete [] predictorState;
            predictorState = NULL;
            delete [] correctorState;
            correctorState = NULL;
            delete [] errorEstimates;
            errorEstimates = NULL;
        }

        // Setup the starter
        if (starter == NULL)
            starter = new RungeKutta89;

        starter->SetPhysicalModel(physicalModel);
        starter->Initialize();

        inState  = physicalModel->GetState();
        outState = physicalModel->GetState();
    }
}

//------------------------------------------------------------------------------
// bool PredictorCorrector::Step(Real dt)
//------------------------------------------------------------------------------
/**
 * Method to advance the system by a fixed interval
 *
 * @ param dt The interval
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::Step(Real dt)
{
//    bool stepFinished = false;
    timeleft = dt;

    if (fabs(dt) != fabs(stepSize)) {
        if (fabs(dt) < fabs(stepSize)) {
            stepSize = dt;
            Reset();
        }
        else {
            Real ss = fabs(dt / stepSize);
            if (fabs(ss - int(ss)) > smallestTime) {
                stepSize = dt / ((int)(ss+1));
                Reset();
            }
        }
    }

    bool fixed = fixedStep;
    fixedStep = true;
    Real stepsign = ((stepSize > 0.0) ? 1.0 : -1.0);
    do {
        if (!Step())
            return false;
    } while (timeleft * stepsign > smallestTime);
    fixedStep = fixed;

    return true;
}

//------------------------------------------------------------------------------
// bool PredictorCorrector::Step(void)
//------------------------------------------------------------------------------
/**
 * Method used to advance the Predictor-Corrector one step
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::Step(void)
{
    if (!initialized)
        return false;

    if (physicalModel->StateChanged())
        Reset();

    do {
        if (!startupComplete) {
            if (ddt == NULL) {
                ddt = physicalModel->GetDerivativeArray();
                if (ddt == NULL)
                    return false;
            }
            // First load up the derivatives for the current state
            physicalModel->GetDerivatives(inState);
            memcpy(history[startupCount+1], ddt,
                dimension * sizeof(Real));
            bool retval = FireStartupStep();
            if (retval)
                timeleft -= stepTaken;
            else
                return false;

            // Omit any error from the starter code -- we assume it is good
            // enough for startup purposes
            maxError = 0.0;
        }
        else {
            if (!Predict())
                return false;
            if (!Correct())
                return false;
            if (EstimateError() < 0.0)
                return false;
            if (maxError <= tolerance) {
                memcpy(outState, correctorState, dimension * sizeof(Real));
                physicalModel->IncrementTime(stepSize);
                stepTaken = stepSize;
                timeleft -= stepTaken;
            }
            if ((maxError > tolerance) || (maxError < lowerError))
                if (maxError != 0.0)
                    if (!AdaptStep(maxError))
                        return false;
        }

        if (stepAttempts >= maxStepAttempts)
            return false;
    } while (maxError > tolerance);

    return true;
}


//------------------------------------------------------------------------------
// bool PredictorCorrector::RawStep(void)
//------------------------------------------------------------------------------
/**
 * For the P-C integrators, this method just returns false.
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::RawStep(void)
{
    return false;
}

//------------------------------------------------------------------------------
// bool PredictorCorrector::AdaptStep(Real maxError)
//------------------------------------------------------------------------------
/**
 * Method used to change the stepsize
 *
 * The default implementation halves the stepsize, and then resets the algorithm
 * to try again.
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::AdaptStep(Real maxError)
{
    Real newStep = stepSize * pow(targetError / maxError, invOrder);

    // Make sure new step is in the accepted range
    if (fabs(newStep) < minimumStep)
        newStep = minimumStep * stepSign;
    if (fabs(newStep) > maximumStep)
        newStep = maximumStep * stepSign;

    if (!fixedStep) {
        // Variable step mode
        if (maxError > tolerance) {
            // Tried and failed at the minimum stepsize
            if (fabs(stepSize) == minimumStep)
                return false;
            stepSize = newStep;
            ++stepAttempts;
        }
        else {
            // Step can be "safely" increased -- but only up to twice old value
            if (newStep >= 2.0 * stepSize)
                stepSize *= 2.0;
            else
                stepSize = newStep;

            stepAttempts = 0;
        }
    }
    else {
        // Fixed step mode: Adjust newStep to be a power of 2 different from
        // the current step
        Real twoStep = stepSize;
        if (fabs(newStep) < fabs(stepSize)) {
            do {
                twoStep /= 2.0;
            } while (fabs(twoStep) > fabs(newStep));
            stepSize = twoStep;
        }
        else if (fabs(newStep) >= fabs(2.0 * stepSize)) {
            // Check to see if the step can safely be Reald, given the
            // remaining interval
            Real stepsToGo = timeleft / (2.0 * stepSize);
            if (stepsToGo == int(stepsToGo))
                stepSize *= 2.0;
            else 
                return true;
        }
        else        // Cannot increase this stepsize in fixed step mode
            return true;
        while (fabs(stepSize) < minimumStep) 
            stepSize *= 2.0;
    }
    return Reset();
}

