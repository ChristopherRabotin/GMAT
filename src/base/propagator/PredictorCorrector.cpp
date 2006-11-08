//$Header$
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
// static data
//---------------------------------
const std::string
PredictorCorrector::PARAMETER_TEXT[PredictorCorrectorParamCount - IntegratorParamCount] =
{
   "StepCount",
   "MaximumError",
   "LowerError",
   "TargetError",
   "StepSign",
   "StartupComplete",
   "StartupCount",
   "InvOrder"
};

const Gmat::ParameterType
PredictorCorrector::PARAMETER_TYPE[PredictorCorrectorParamCount - IntegratorParamCount] =
{
	Gmat::INTEGER_TYPE,
	Gmat::REAL_TYPE,
	Gmat::REAL_TYPE,
	Gmat::REAL_TYPE,
	Gmat::REAL_TYPE,
	Gmat::BOOLEAN_TYPE,
	Gmat::INTEGER_TYPE,
	Gmat::REAL_TYPE,
};

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
    invOrder                        (1.0/order)
{
    parameterCount = PredictorCorrectorParamCount;
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
    invOrder                        (pc.invOrder)
{
    parameterCount = PredictorCorrectorParamCount;
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
    
    // ag: Added - is this right?
    ddt = NULL;
    history = NULL;
    pweights = NULL;
    cweights = NULL;
    predictorState = NULL;
    correctorState = NULL;
    starter = NULL;

    return *this;
}

//------------------------------------------------------------------------------
// Clone(void) const
//------------------------------------------------------------------------------
/**
 * Used to create a copy of the object.
 * This method returns a copy of the current instance.  The copy has all of the
 * parameters set to the current values, but is not yet initialized becuse the 
 * PhysicalModel pointer is not set, nor are the internal arrays that depend on 
 * the PhysicalModel allocated.
 *
 * @return <Propagator*>
 */
//------------------------------------------------------------------------------
//GmatBase* PredictorCorrector::Clone(void) const
//{
//    return new PredictorCorrector(*this);
//}

//------------------------------------------------------------------------------
// void PredictorCorrector::Initialize(void)
//------------------------------------------------------------------------------
/**
 * Method used to setup the data structures for the algorithm 
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::Initialize()
{
//    Propagator::Initialize();

    initialized = false;
    if (stepCount <= 0)
        return initialized;

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
            return initialized;
        }

        correctorState = new Real[dimension];
        if (!correctorState) {
            delete [] predictorState;
            predictorState = NULL;
            return initialized;
        }

        errorEstimates = new Real[dimension];
        if (!errorEstimates) {
            delete [] predictorState;
            predictorState = NULL;
            delete [] correctorState;
            correctorState = NULL;
            return initialized;
        }

        pweights = new Real[stepCount];
        if (!pweights) {
            delete [] predictorState;
            predictorState = NULL;
            delete [] correctorState;
            correctorState = NULL;
            delete [] errorEstimates;
            errorEstimates = NULL;
            return initialized;
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
            return initialized;
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
                    return initialized;
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
    
    return initialized;
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
//------------------------------------------------------------------------------
// std::string PredictorCorrector::GetParameterText(const int id) const
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
std::string PredictorCorrector::GetParameterText(const Integer id) const
{
    if (id >= STEP_COUNT && id < PredictorCorrectorParamCount)
        return PARAMETER_TEXT[id - IntegratorParamCount];
    else
        return Integrator::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer PredictorCorrector::GetParameterID(const std::string &str) const
{
    for (Integer i = STEP_COUNT; i < PredictorCorrectorParamCount; i++)
    {
        if (str == PARAMETER_TEXT[i - IntegratorParamCount])
            return i;
    }

    return Integrator::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType PredictorCorrector::GetParameterType(const Integer id) const
{
    if (id >= STEP_COUNT && id < PredictorCorrectorParamCount)
        return PARAMETER_TYPE[id - IntegratorParamCount];
    else
        return Integrator::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PredictorCorrector::GetParameterTypeString(const Integer id) const
{
   if (id >= STEP_COUNT && id < PredictorCorrectorParamCount)
      return Integrator::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return Integrator::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Method used to determine is a parameter should be hidden when writing scripts
 * 
 * @param <id> The parameter id.
 * 
 * @return true is the parameter should be hidden, false if it is visible.
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::IsParameterReadOnly(const Integer id) const
{
   if ((id == STEP_COUNT) || (id == MAXIMUM_ERROR) || (id == STEP_SIGN) || 
       (id == STARTUP_COMPLETE) || (id == STARTUP_COUNT) || (id == INV_ORDER))
      return true;
      
   return Integrator::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Method used to determine is a parameter should be hidden when writing scripts
 * 
 * @param <label> The parameter name.
 * 
 * @return true is the parameter should be hidden, false if it is visible.
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real PredictorCorrector::GetRealParameter(const Integer id) const
{
	if (id == MAXIMUM_ERROR)					return maxError;
	else if (id == LOWEVER_ERROR)				return lowerError;
	else if (id == TARGET_ERROR)	return targetError;
	else if (id == STEP_SIGN)	return stepSign;
	else if (id == INV_ORDER)				return invOrder;

   return Integrator::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real PredictorCorrector::GetRealParameter(const std::string &label) const
{
    Integer id = GetParameterID(label);

    return GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Method used to Set parameters for the BS Integrator.
 * This method sets the tolerance and tolerance limit for the Bulirsch-Stoer 
 * integrator, and calls Integrator::SetParameter() for other parameters.  The
 * implementation here prevents setting tolerances of more than 100%.  If the
 * minimum tolerance is set to a value larger than the current tolerance, the
 * current value is raised to the minimum value.
 * 
 * @param <id>  ID for the parameter being set
 * @param <value>  New value for the parameter
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real PredictorCorrector::SetRealParameter(const Integer id, const Real value)
{
	if (id == MAXIMUM_ERROR)
	{
		maxError = value;
		return maxError;
	}
	else if (id == LOWEVER_ERROR)
	{
		lowerError = value;
		return lowerError;
	}
	else if (id == TARGET_ERROR)
	{
		targetError = value;
		return targetError;
	}
	else if (id == STEP_SIGN)
	{
		stepSign = value;
		return stepSign;
	}
	else if (id == INV_ORDER)
	{
		invOrder = value;
		return invOrder;
	}

   return Integrator::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real PredictorCorrector::SetRealParameter(const std::string &label, const Real value)
{
    return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter ID.
 * @see GmatBase
 * @param <id> ID for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer PredictorCorrector::GetIntegerParameter(const Integer id) const
{
   if (id == STEP_COUNT)			     	return stepCount;
	else if (id == STARTUP_COUNT)				return startupCount;

	return Integrator::GetIntegerParameter(id);
}

//------------------------------------------------------------------------------
//  Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * This method sets the Integer parameter value, given the input
 * parameter ID.
 * @see GmatBase
 * @param <id> ID for the requested parameter.
 * @param <value> Integer value for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer PredictorCorrector::SetIntegerParameter(const Integer id,
											   const Integer value) // const?
{
	if (id == STEP_COUNT)
	{
		stepCount = value;
		return stepCount;
	}
	else if (id == STARTUP_COUNT)
	{
		startupCount = value;
		return startupCount;
	}
	return Integrator::SetIntegerParameter(id,value);
}

//------------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the bool parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  bool value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::GetBooleanParameter(const Integer id) const
{
	if (id == STARTUP_COMPLETE)	return startupComplete;

	return Integrator::GetBooleanParameter(id);
}

//------------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
* This method sets the bool parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> bool value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::SetBooleanParameter(const Integer id, const bool value)
{
	if (id ==  STARTUP_COMPLETE)
	{
		startupComplete = value;
		return startupComplete;
	}

	return Integrator::SetBooleanParameter(id,value);
}

////------------------------------------------------------------------------------
//// Real PredictorCorrector::GetParameter(const Integer id) const
////------------------------------------------------------------------------------
///**
// * Helper method used to find the text name for the class parameters.
// * This static method is used to obtain text descriptions for the parameters 
// * defined by the class.  These strings can be used to fill in the descriptors
// * for user interface elements, so that those elements have labels that match 
// * the intent of each of the user selectable parameters.
// * 
// */
////------------------------------------------------------------------------------
//Real PredictorCorrector::GetParameter(const Integer id) const
//{
//    if (id == PREDICTORCORRECTOR_LOWERERROR)
//        return lowerError;
//
//    if (id == PREDICTORCORRECTOR_TARGETERROR)
//        return targetError;
//
//    return Integrator::GetRealParameter(id);
//}
//
////------------------------------------------------------------------------------
//// bool PredictorCorrector::SetParameter(const int id, const Real val)
////------------------------------------------------------------------------------
///**
// * Method used to set predictor-corrector parameters
// *
// * This is the SetParameter method for the Predictor-Corrector implementations.
// * Each Predictor-Corrector has a startup propagator, starter, that may also 
// * have the parameters passed into this method.  The starter is given the 
// * opportunity to accept the parameters that correspond to the Integrator
// * parameters (but not the Predictor-Corrector parameters).  The return value 
// * from the starter setup is not checked for validity.
// *
// * @ param id     ID for the parameter
// * @ param val    New value for the parameter
// *
// * @ return true if the id is valid and the Predictor-Corrector accepts the value
// */
////------------------------------------------------------------------------------
//bool PredictorCorrector::SetParameter(const int id, const Real val)
//{
//    if (id == PREDICTORCORRECTOR_LOWERERROR)
//        lowerError = val;
//    else if (id == PREDICTORCORRECTOR_TARGETERROR)
//        targetError = val;
//    else {
//        if (id == GetParameterID("InitialStepSize"))   // ag: was STEP_SIZE_PARAMETER
//            if (val == 0.0)
//                return false;
//        
//        stepSign = (val < 0.0 ? -1.0 : 1.0);
//        starter->SetRealParameter(id, val);
//
//        return Integrator::SetRealParameter(id, val);
//    }
//    return true;
//}

