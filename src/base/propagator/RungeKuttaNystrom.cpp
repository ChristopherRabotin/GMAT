//$Header$
//------------------------------------------------------------------------------
//                              RungeKuttaNystrom
// *** File Name : rungekuttanystrom.cpp
// *** Created   : January 21, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-67521-G                             ***
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
// Modification History      : 1/21/03 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//                                    Changes
//                           : 02/21/2004 - L. Ruley, Missions Applications Branch
//                                            - Updated style using GMAT cpp style guide
//                                            - All Real types to Real types
//                            - All int types to Integer types
// **************************************************************************

#include "gmatdefs.hpp"
#include "RungeKuttaNystrom.hpp"

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// RungeKuttaNystrom::RungeKuttaNystrom(void)
//------------------------------------------------------------------------------
/**
 * The default constructor
 */
//------------------------------------------------------------------------------
//RungeKuttaNystrom::RungeKuttaNystrom(void) :
//RungeKutta (16, 9, NULL, NULL, NULL)
//{
//}

//------------------------------------------------------------------------------
// RungeKuttaNystrom::RungeKuttaNystrom(Gmat::ObjectType id, const std::string &typeStr,
//                            const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * The constructor
 */
//------------------------------------------------------------------------------
//RungeKuttaNystrom::RungeKuttaNystrom(const std::string &nomme) :
//    RungeKutta (9, 8, "RungeKuttaNystrom", nomme)
RungeKuttaNystrom::RungeKuttaNystrom(Integer st, Integer order, const std::string &typeStr,
                                           const std::string &nomme) :
    RungeKutta          (9, 8, typeStr, nomme),
    cdotj               (NULL),
    derivativeMap       (NULL),
    inverseMap          (NULL),
    derivativeError     (false),
    eeDeriv             (NULL)
{
}

//------------------------------------------------------------------------------
// RungeKuttaNystrom::~RungeKuttaNystrom(void)
//------------------------------------------------------------------------------
/**
 * The destructor
 */
//------------------------------------------------------------------------------
RungeKuttaNystrom::~RungeKuttaNystrom(void)
{
    if (cdotj)
        delete [] cdotj;
    if (derivativeMap)
        delete [] derivativeMap;
    if (inverseMap)
        delete [] inverseMap;
    if (eeDeriv)
        delete [] eeDeriv;
}

//------------------------------------------------------------------------------
// RungeKuttaNystrom::RungeKuttaNystrom(const RungeKuttaNystrom& rk)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
//------------------------------------------------------------------------------
RungeKuttaNystrom::RungeKuttaNystrom(const RungeKuttaNystrom& rk) :
    RungeKutta (rk),
    cdotj               (NULL),
    derivativeMap       (NULL),
    inverseMap          (NULL),
    derivativeError     (rk.derivativeError),
    eeDeriv             (NULL)
{
    initialized = false;
}

//------------------------------------------------------------------------------
// RungeKuttaNystrom & RungeKuttaNystrom::operator=(const RungeKuttaNystrom& rk)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
RungeKuttaNystrom & RungeKuttaNystrom::operator=(const RungeKuttaNystrom& rk)
{
    if (this == &rk)
        return *this;

    initialized = false;
    derivativeError = rk.derivativeError;

    return *this;
}

//------------------------------------------------------------------------------
// char * RungeKuttaNystrom::GetType(void) const
//------------------------------------------------------------------------------
/**
 * Method used to return the name of the object
 */
//------------------------------------------------------------------------------
char * RungeKuttaNystrom::GetType(void) const
{
    return "Runge-Kutta-Nystrom 6(8)";
}

//------------------------------------------------------------------------------
// Propagator* RungeKuttaNystrom::Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
//Propagator* RungeKuttaNystrom::Clone(void) const
//{
//    return new RungeKuttaNystrom(*this);
//}

//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// void RungeKuttaNystrom::Initialize(void)
//------------------------------------------------------------------------------
/**
 * This method allocates the data arrays for the Runge-Kutta-Nystrom integrators
 * by calling the Runge-Kutta initialization method (RungeKutta::Initialize()) 
 * and then initializing the Nystrom specific data.
 */
//------------------------------------------------------------------------------
void RungeKuttaNystrom::Initialize(void)
{
    RungeKutta::Initialize();
    if (initialized == false)
        return;

    /// \todo: Make this consistent with SetupAccumulator in the RK code
    if (physicalModel == NULL) {
        initialized = false;
        return;
    }

    if (cdotj) {
        delete [] cdotj;
        cdotj = NULL;
    }

    if (derivativeMap) {
        delete [] derivativeMap;
        derivativeMap = NULL;
    }

    if (inverseMap) {
        delete [] inverseMap;
        inverseMap = NULL;
    }

    if (eeDeriv) {
        delete [] eeDeriv;
        eeDeriv = NULL;
    }

    // Set the Nystrom-specific structures
    if ((cdotj = new double [stages]) == NULL) {
        RungeKutta::ClearArrays();
        initialized = false;
        return;
    }

    if ((derivativeMap = new int [dimension]) == NULL) {
        RungeKutta::ClearArrays();
        delete [] cdotj;
        cdotj = NULL;
        initialized = false;
        return;
    }

    if ((inverseMap = new int [dimension]) == NULL) {
        RungeKutta::ClearArrays();
        delete [] cdotj;
        cdotj = NULL;
        delete [] derivativeMap;
        derivativeMap = NULL;
        initialized = false;
        return;
    }

    if (!physicalModel->GetComponentMap(derivativeMap)) {
        RungeKutta::ClearArrays();
        delete [] cdotj;
        cdotj = NULL;
        delete [] derivativeMap;
        derivativeMap = NULL;
        initialized = false;
        return;
    }

    if (derivativeError) {
        if ((eeDeriv = new double[stages]) == NULL) {
            RungeKutta::ClearArrays();
            delete [] cdotj;
            cdotj = NULL;
            delete [] derivativeMap;
            derivativeMap = NULL;
            delete [] inverseMap;
            inverseMap = NULL;
            initialized = false;
            return;
        }
    }

    // Fill the inverse map from the derivative map
    for (int i = 0; i < dimension; ++i)
        inverseMap[i] = -1;

    for (int i = 0; i < dimension; ++i)
        if (derivativeMap[i] != -1)
            inverseMap[derivativeMap[i]] = i;

    SetCoefficients();
    SetupAccumulator();
}

bool RungeKuttaNystrom::Step(void)
{
    if (!initialized)
        return false;

    bool goodStepTaken = false;
    double maxerror;
    
    do {
        if (!RawStep())
            return false;

        maxerror = EstimateError();
        stepTaken = stepSize;
        if (AdaptStep(maxerror))
            goodStepTaken = true;

        if (stepAttempts >= maxStepAttempts)
            return false;
    } while (!goodStepTaken);

    physicalModel->IncrementTime(stepTaken);
    return true;
}

bool RungeKuttaNystrom::Step(double dt)
{
    bool stepFinished = false;
    timeleft = dt;
    do {
        if (!Propagator::Step(timeleft))
            return false;
        if (timeleft == stepTaken)
            stepFinished = true;
        timeleft -= stepTaken;
    } while (stepFinished == false);

    return true;
}


bool RungeKuttaNystrom::RawStep(void)
{
    int i, j, k;
    double accum, h2 = stepSize * stepSize;

    // Calculate the stages
    for (i = 0; i < stages; i++) {
        // Built the accumulated state if this is not the initial step
        if (i > 0) {
            for (k = 0; k < dimension; ++k) {
                // Only do the dependents, not their derivatives
                if (derivativeMap[k] >= 0) {
                    accum = 0.0;
                    for (j = 0; j < i; ++j) {
                        accum += bij[i][j] * ki[j][k];
                    }
                    stageState[k] = inState[k] + ai[i] * stepSize * 
                        inState[derivativeMap[k]] + h2 * accum;
                }
            }
        }
        else {
            memcpy(stageState, inState, dimension * sizeof(double));
        }

        if (!physicalModel->GetDerivatives(stageState, stepSize * ai[i], 2)) {
            return false;
        }
        for (j = 0; j < dimension; j++)
            // Only the dependents...
            if (derivativeMap[j] >= 0)
                ki[i][j] = ddt[j];
            else {
                if (derivativeError) {
                    if (inverseMap[j] >= 0)
                        ki[i][j] = ddt[inverseMap[j]];
                    else
                        ki[i][j] = 0.0; 
                }
                else
                    ki[i][j] = 0.0; 
            }
    }

    // Accumulate the stages for the RK step
    memcpy(candidateState, inState, dimension*sizeof(double));
    for (i = 0; i < dimension; i++) {
        if (derivativeMap[i] >= 0) {
            candidateState[i] += stepSize * inState[derivativeMap[i]];
            for (j = 0; j < stages; j++) {
                if (cj[j] != 0.0)
                    candidateState[i] += h2 * cj[j] * ki[j][i];
            }
        }
        else {
            for (j = 0; j < stages; j++) {
                if (cdotj[j] != 0.0)
                    candidateState[i] += stepSize * cdotj[j] * ki[j][inverseMap[i]];
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// void RungeKuttaNystrom::EstimateError(void)
//------------------------------------------------------------------------------
/**
 * This method takes the state vector and calculates the error in each 
 * component.  The error is then divided by the change in the component.  The
 * function returns the largest of the resulting relative errors.

 * The Nystrom implementation of this code is similar to the implementation for
 * the first order Runge-Kutta, except that the derivative terms use special
 * coefficients when error estimates are available for those terms.
 */
//------------------------------------------------------------------------------
Real RungeKuttaNystrom::EstimateError(void)
{
    int i, j;

    double h2 = stepSize * stepSize;

    for (i = 0; i < dimension; i++) {
        errorEstimates[i] = 0.0;
        for (j = 0; j < stages; j++) {
            if (derivativeMap[i] >= 0) {
                errorEstimates[i] += h2 * ee[j] * ki[j][i];
            }
            else if (derivativeError && (inverseMap[i] >= 0)) {
                errorEstimates[i] += stepSize * eeDeriv[j] * ki[j][i];
            }
        }
    }

    // Find the maximum error
    return physicalModel->EstimateError(errorEstimates, candidateState);
}

Integer RungeKuttaNystrom::GetPropagatorOrder(void) const
{
    return 2;
}



