//$Header$
//------------------------------------------------------------------------------
//                        AdamsBashforthMoulton
//------------------------------------------------------------------------------
// *** File Name : AdamsBashforthMoulton.hpp
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


#ifndef AdamsBashforthMoulton_H
#define AdamsBashforthMoulton_H

#include "gmatdefs.hpp"
#include "PredictorCorrector.hpp"
#include "RungeKutta.hpp"

/**
 * Implementation of the Adams-Bashford-Moulton Predictor-Corrector
 *
 * This code implements a fourth-order Adams-Bashford predictor / Adams-Moulton
 * corrector pair to integrate a set of first order differential equations.  The
 * algorithm is found at 
 * http://chemical.caeds.eng.uml.edu/onlinec/white/math/s1/s1num/s1num.html
 * or in Bate, Mueller and White, pp. 415-417.
 *
 * The predictor step extrapolates the next state \f$r_{i+1}\f$ of the variables
 * using the the derivative information \f$(f)\f$ at the current state and three 
 * previous states of the variables, by applying the equation
 *
 * \f[ r_{i+1}^{*j} = r_i^j + {{h}\over{24}}\left[55 f_n^j - 59 f_{n-1}^j 
 *                 + 37 f_{n-2}^j - 9 f_{n-3}^j \right] \f]
 *
 * The corrector uses derivative information evaluated for this state, along 
 * with the derivative information at the original state and two preceding 
 * states, to tune this state, giving the final, corrected state:
 *
 * \f[ r_{i+1}^{j} = r_i^j + {{h}\over{24}}\left[9 f_{n+1}^{*j} + 19 f_{n}^j 
 *                 - 5 f_{n-1}^j + 1 f_{n-2}^j \right] \f]
 *
 * Bate, Mueller and White give the estimated accuracy of this solution to be
 *
 * \f[ee = {{19}\over{270}} \left|r_{i+1}^{*j} - r_{i+1}^{j}\right|\f]
 */
 
class GMAT_API AdamsBashforthMoulton :
    public PredictorCorrector
{
public:
//    AdamsBashforthMoulton(void);
    AdamsBashforthMoulton(const std::string &nomme);

    virtual ~AdamsBashforthMoulton(void);

    AdamsBashforthMoulton(const AdamsBashforthMoulton& abm);
    AdamsBashforthMoulton    operator=(const AdamsBashforthMoulton& abm);

    virtual std::string     GetType(void) const;
    virtual Propagator*     Clone(void) const;

protected:
    /// The error estimate prefactor
    Real                    eeFactor;

    bool                    SetWeights(void);
    bool                    FireStartupStep(void);
    bool                    Predict(void);
    bool                    Correct(void);
    Real                    EstimateError(void);
    bool                    Reset(void);
};

#endif  //AdamsBashformMoulton_hpp
