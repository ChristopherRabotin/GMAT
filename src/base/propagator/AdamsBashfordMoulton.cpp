//$Header$
//------------------------------------------------------------------------------
//                        AdamsBashfordMoulton
//------------------------------------------------------------------------------
// *** File Name : AdamsBashfordMoulton.cpp
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
//                           : 6/17/2004 - A. Greene, Mission Applications, GSFC
//                             - Updated style using GMAT cpp style guide
//                             - All double types to Real types, int to Integer
// **************************************************************************


#include "AdamsBashfordMoulton.hpp"

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// AdamsBashfordMoulton::AdamsBashfordMoulton(const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * The constructor for the ABM integrator
 */
//------------------------------------------------------------------------------
//AdamsBashfordMoulton::AdamsBashfordMoulton(void) :
//    PredictorCorrector      (4, 4),
AdamsBashfordMoulton::AdamsBashfordMoulton(const std::string &nomme) :
    PredictorCorrector      (4, 4, "AdamsBashfordMoulton", nomme),
    eeFactor                (19.0/270.0)
{
    starter = new RungeKutta89;
}

//------------------------------------------------------------------------------
// AdamsBashfordMoulton::~AdamsBashfordMoulton(void)
//------------------------------------------------------------------------------
/**
 * The destructor
 */
//------------------------------------------------------------------------------
AdamsBashfordMoulton::~AdamsBashfordMoulton(void)
{
}

//------------------------------------------------------------------------------
// AdamsBashfordMoulton::AdamsBashfordMoulton(const AdamsBashfordMoulton& abm)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
//------------------------------------------------------------------------------
AdamsBashfordMoulton::AdamsBashfordMoulton(const AdamsBashfordMoulton& abm) :
    PredictorCorrector      (abm),
    eeFactor                (abm.eeFactor)
{
//    starter = new RungeKutta89;
    starter = abm.starter;
}

//------------------------------------------------------------------------------
// AdamsBashfordMoulton 
//            AdamsBashfordMoulton::operator=(const AdamsBashfordMoulton& abm)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
AdamsBashfordMoulton 
            AdamsBashfordMoulton::operator=(const AdamsBashfordMoulton& abm)
{
    if (this == &abm)
        return *this;

    PredictorCorrector::operator=(abm);
    eeFactor = abm.eeFactor;
    starter = abm.starter;

    return *this;
}


//------------------------------------------------------------------------------
// std::string AdamsBashfordMoulton::GetType(void) const
//------------------------------------------------------------------------------
/**
 * Method used to return the name of the object
 */
//------------------------------------------------------------------------------
std::string AdamsBashfordMoulton::GetType(void) const
{
//    return "Adams-Bashford-Moulton 4";
   return "Adams-Bashford-Moulton 4";
}


//------------------------------------------------------------------------------
// Propagator* AdamsBashfordMoulton::Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
Propagator* AdamsBashfordMoulton::Clone(void) const
{
    return new AdamsBashfordMoulton(*this);
}
  
//------------------------------------------------------------------------------
// bool AdamsBashfordMoulton::SetWeights(void)
//------------------------------------------------------------------------------
/**
 * Method used to set the ABM weights
 */
//------------------------------------------------------------------------------
bool AdamsBashfordMoulton::SetWeights(void)
{
    if ((pweights == NULL) || (cweights == NULL))
        return false;

    pweights[3] =  55.0 / 24.0;
    pweights[2] = -59.0 / 24.0;
    pweights[1] =  37.0 / 24.0;
    pweights[0] = - 9.0 / 24.0;

    cweights[3] =   9.0 / 24.0;
    cweights[2] =  19.0 / 24.0;
    cweights[1] = - 5.0 / 24.0;
    cweights[0] =   1.0 / 24.0;

    return true;
}

//------------------------------------------------------------------------------
// bool AdamsBashfordMoulton::FireStartupStep(void)
//------------------------------------------------------------------------------
/**
 * Method used to start the integrator by filling initial states
 */
//------------------------------------------------------------------------------
bool AdamsBashfordMoulton::FireStartupStep(void)
{
    if (starter == NULL)
        return false;

    bool retval = starter->Step(stepSize);
    if (retval) {
        ++startupCount;
        if (startupCount == 3) {
            startupComplete = true;
        }
        stepTaken = stepSize;
    }
    return retval;
}

//------------------------------------------------------------------------------
// bool AdamsBashfordMoulton::Predict(void)
//------------------------------------------------------------------------------
/**
 * Method used to fire the step extrapolation (the predictor phase)
 */
//------------------------------------------------------------------------------
bool AdamsBashfordMoulton::Predict(void)
{
    int i, j;

    physicalModel->GetDerivatives(inState);
    for (i = 0; i < stepCount - 1; i++)
        memcpy(history[i], history[i+1], dimension * sizeof(double));
    memcpy(history[stepCount - 1], ddt, dimension * sizeof(double));


    for (j = 0; j < dimension; j++) {
        predictorState[j] = inState[j];
        for (i = 0; i < stepCount; i++) {
            predictorState[j] += stepSize * pweights[i] * history[i][j];
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// bool AdamsBashfordMoulton::Correct(void)
//------------------------------------------------------------------------------
/**
 * Method used to fire the step refinement (the corrector phase)
 */
//------------------------------------------------------------------------------
bool AdamsBashfordMoulton::Correct(void)
{
    int i, j;

    physicalModel->GetDerivatives(predictorState, stepSize);
    for (j = 0; j < dimension; j++) {
        correctorState[j] = inState[j] + stepSize * cweights[stepCount-1] * ddt[j];
        for (i = 1; i < stepCount; i++) {
            correctorState[j] += stepSize * cweights[i-1] * history[i][j];
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Propagator* PrinceDormand78::Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to determine the error in the step
 *
 * The error estimate for the Adams-Bashford-Moulton integrator is calculated by
 * finding the difference between the corrector and predictor states, and 
 * multiplying this difference by the difference in the truncated Taylor series 
 * used to derive the algorithm.  For this 4th order implementation, the error 
 * in the \f$i^{th}\f$ component is given by
 *
 * \f[EE_i = {{19}\over{270}}\left|r_{i}^{(C)}\left(t+\delta t\right) - 
 *                          r_{i}^{(P)}\left(t+\delta t\right)\right|\f]
 *
 * The resulting error components are passed to PhysicalModel->EstimateError()
 * and processed to determine the maximum error in the system.
 */
//------------------------------------------------------------------------------
Real AdamsBashfordMoulton::EstimateError(void)
{
    int i;

    for (i = 0; i < dimension; i++) 
        errorEstimates[i] = fabs(eeFactor*(correctorState[i]-predictorState[i])); 
    
    maxError = physicalModel->EstimateError(errorEstimates, correctorState);

    return maxError;
}

//------------------------------------------------------------------------------
// Propagator* PrinceDormand78::Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to restart the integrator, when appropriate
 *
 * This method is called when the ABM integrator changes the stepsize or when 
 * the state data in the PhysicalModel is changed discontinuously, so that the
 * internal buffers can be filled with the data for the new stepsize.
 */
//------------------------------------------------------------------------------
bool AdamsBashfordMoulton::Reset(void)
{
    startupCount = 0;
    startupComplete = false;
    return true;
}
