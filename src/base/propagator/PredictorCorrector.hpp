//$Header:
//------------------------------------------------------------------------------
//                              PredictorCorrector
//------------------------------------------------------------------------------
// *** File Name : PredictorCorrector.hpp
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
//                           : 3/3/2003 - D. Conway, Thinking Systems, Inc.
//                             Moved time step interval to Integrator class 
//                           : 6/17/2004 - A. Greene, Missions Applications
//                              Changes:
//                                - Updated style using GMAT cpp style guide
// **************************************************************************
/** 
 * Base class for Predictor-Corrector Integrators

 * The predictor-corrector integration method is a multistep method that uses 
 * data from the current state of the system, along with a number of previous 
 * states, to predict the next state of the system.  Once this prediction has
 * been made, the algorithm uses the predicted state to calculate the 
 * corresponding derivative information, and then corrects the state using this
 * additional information to provide the final result.

 * This class implements that mechanism by providing a base class for the 
 * implementation of predictor-corrector integrators.  Users of the class need 
 * to implement methods that initialize the system (the SetWeights() method), 
 * run the startup sequence used to initialize the buffer of history data
 * (FireStartupStep()), calculate the extrapolated state (Predict()), update
 * this state using the derivative information at the extrapolated state 
 * (Correct()), and reset the algorithm if the error is outside of the accepted
 * range of errors for the implementation (Reset()).  
  
 * The estimated error for these algorithms depends on the characteristics of 
 * the derivation of that algorithm; for that reason, users will also need to 
 * implement a method that estimates the error in the resulting state
 * (EstimateError()).  Typical implementations estimate the error by finding the 
 * difference between the predicted and corrected states, and then applying an 
 * implementation specific multiplier to this difference to obtain the error
 * estimate.

 * This class provides a default mechanism to reset the step if the error 
 * estimate is out of line with the desired value.
 */

#ifndef PredictorCorrector_H
#define PredictorCorrector_H

#include "gmatdefs.hpp"
#include "Integrator.hpp"
//#include "TwoBodyProp.hpp"
#include "RungeKutta89.hpp"

#ifdef DEBUG_STEP_CONTROL
#include <fstream>
#endif
 
class GMAT_API PredictorCorrector :
    public Integrator
{
public:
    PredictorCorrector(Integer sc, Integer order, const std::string &typeStr,
			   const std::string &nomme = "");
    virtual ~PredictorCorrector(void);

    PredictorCorrector(const PredictorCorrector&);
    PredictorCorrector& operator=(const PredictorCorrector&);
    
    virtual std::string GetParameterName(const Integer parm) const;
    virtual Real GetParameter(const int id) const;
    virtual bool SetParameter(const int id, const Real val);

    virtual void Initialize(void);
    virtual bool  Step(void);
    virtual bool  Step(Real dt);
    virtual bool  RawStep(void);

protected:
    /// Number of steps (including current state) needed for the algorithm
    Integer stepCount;
    /// Array used to capture the derivative information
    const Real * ddt;
    /// Array containing the previous step derivatives (stepcount x dimension)
    Real ** history;
    /// Predictor weights for each of the history steps
    Real * pweights;
    /// Corrector weights for each of the history steps
    Real * cweights;
    /// State returned from the predictor
    Real * predictorState;
    /// State returned from the corrector
    Real * correctorState;
    /// Largest relative error found
    Real maxError;
    /// Smallest allowed error
    Real lowerError;
    /// Desired error estimate (used to adapt stepsize if maxError out of bounds)
    Real  targetError;
    /// Sign of the step, to preserve direction of integration
    Real  stepSign;
    /// Status of the startup procedure: true if the startup is complete
    bool  startupComplete;
    /// Count of startup steps that have fired
    Integer startupCount;
    /// Propagator used during the startup sequence
    Propagator * starter;
    /// Order of the Integration method, used for stepsize control
    Real invOrder;

    virtual bool AdaptStep(Real maxError);

    //------------------------------------------------------------------------------
    // virtual bool SetWeights(void)
    //------------------------------------------------------------------------------
    /**
     * The function is used to set the weights for the preceding steps
     */
    //------------------------------------------------------------------------------
    virtual bool SetWeights(void) = 0;

    //------------------------------------------------------------------------------
    // virtual bool FireStartupStep(void)
    //------------------------------------------------------------------------------
    /**
     * The function is used to take the steps through the starup routine 
     */
    //------------------------------------------------------------------------------
    virtual bool FireStartupStep(void) = 0;

    //------------------------------------------------------------------------------
    // virtual bool Predict(void)
    //------------------------------------------------------------------------------
    /**
     * The function is used to perform the extrapolation step (the Predictor)
     */
    //------------------------------------------------------------------------------
    virtual bool Predict(void) = 0;

    //------------------------------------------------------------------------------
    // virtual bool Correct(void)
    //------------------------------------------------------------------------------
    /**
     * The function is used to perform the refinement step (the Corrector) 
     */
    //------------------------------------------------------------------------------
    virtual bool Correct(void) = 0;

    //------------------------------------------------------------------------------
    // virtual bool Reset(void)
    //------------------------------------------------------------------------------
    /**
     * The function is used to reset the algorithm 
     *
     * This method is called after the stepsize changes.  You should also call
     * it after there is a change in the force model for the system -- for 
     * instance, if a thruster starts or finishes firing on a spacecraft.
     */
    //------------------------------------------------------------------------------
    virtual bool Reset(void) = 0;

private:
    // Parameter IDs 
    /// Least error expected for a predictor-corrector
    const Integer PREDICTORCORRECTOR_LOWERERROR;
    /// Target error for a predictor-corrector
    const Integer PREDICTORCORRECTOR_TARGETERROR;
};

#endif //PredictorCorrector_hpp
