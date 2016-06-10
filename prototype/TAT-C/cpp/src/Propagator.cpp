//------------------------------------------------------------------------------
//                           Propagator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan, NASA/GSFC
// Created: 2016.05.06
//
/**
 * Implementation of the propagator class
 */
//------------------------------------------------------------------------------

#include <cmath>            // for INFINITY
#include "gmatdefs.hpp"
#include "Propagator.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "Rmatrix33.hpp"
#include "TATCException.hpp"
#include "StateConversionUtil.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  Propagator(Spacecraft *sat)
//---------------------------------------------------------------------------
/**
 * Default constructor for Propagator.
 *
 * @param sat The spacecraft object
 * 
 */
//---------------------------------------------------------------------------
Propagator::Propagator(Spacecraft *sat) :
   sc                     (sat),
   J2                     (1.0826269e-003),
   mu                     (3.986004415e+5),
   eqRadius               (6.3781363e+003),
   refJd                  (GmatTimeConstants::JD_OF_J2000),
   SMA                    (0.0),
   ECC                    (0.0),
   INC                    (0.0),
   RAAN                   (0.0),
   AOP                    (0.0),
   TA                     (0.0),
   MA                     (0.0),
   meanMotionRate         (0.0),
   argPeriapsisRate       (0.0),
   rightAscensionNodeRate (0.0),
   semiLatusRectum        (0.0),
   meanMotion             (0.0)
{
   OrbitState *orbSt = sc->GetOrbitState();
   SetOrbitState(orbSt);
   refJd = sc->GetOrbitEpoch()->GetJulianDate();
   ComputeOrbitRates();
}

//---------------------------------------------------------------------------
//  Propagator(const Propagator &copy)
//---------------------------------------------------------------------------
/**
 * Copy constructor for Propagator.
 *
 * @param copy The propagator to copy
 * 
 */
//---------------------------------------------------------------------------
Propagator::Propagator(const Propagator &copy) :
   sc                     (copy.sc),   // correct?
   J2                     (copy.J2),
   mu                     (copy.mu),
   eqRadius               (copy.eqRadius),
   refJd                  (copy.refJd),
   propEpoch              (copy.propEpoch),
   SMA                    (copy.SMA),
   ECC                    (copy.ECC),
   INC                    (copy.INC),
   RAAN                   (copy.RAAN),
   AOP                    (copy.AOP),
   TA                     (copy.TA),
   MA                     (copy.MA),
   meanMotionRate         (copy.meanMotionRate),
   argPeriapsisRate       (copy.argPeriapsisRate),
   rightAscensionNodeRate (copy.rightAscensionNodeRate),
   semiLatusRectum        (copy.semiLatusRectum),
   meanMotion             (copy.meanMotion)
{
}

//---------------------------------------------------------------------------
//  Propagator& operator=(const Propagator &copy)
//---------------------------------------------------------------------------
/**
 * operator= for Propagator.
 *
 * @param copy The propagator to copy
 * 
 */
//---------------------------------------------------------------------------
Propagator& Propagator::operator=(const Propagator &copy)
{
   if (&copy == this)
      return *this;
   
   sc                     = copy.sc; // correct?
   J2                     = copy.J2;
   mu                     = copy.mu;
   eqRadius               = copy.eqRadius;
   refJd                  = copy.refJd;
   propEpoch              = copy.propEpoch;
   SMA                    = copy.SMA;
   ECC                    = copy.ECC;
   INC                    = copy.INC;
   RAAN                   = copy.RAAN;
   AOP                    = copy.AOP;
   TA                     = copy.TA;
   MA                     = copy.MA;
   meanMotionRate         = copy.meanMotionRate;
   argPeriapsisRate       = copy.argPeriapsisRate;
   rightAscensionNodeRate = copy.rightAscensionNodeRate;
   semiLatusRectum        = copy.semiLatusRectum;
   meanMotion             = copy.meanMotion;
   
   return *this;
}

//---------------------------------------------------------------------------
//  ~Propagator()
//---------------------------------------------------------------------------
/**
 * destructor for Propagator.
 *
 */
//---------------------------------------------------------------------------
Propagator::~Propagator()
{
}

//------------------------------------------------------------------------------
// void SetPhysicalConstants(Real bodyMu, Real bodyJ2,
//                           Real bodyRadius)
//------------------------------------------------------------------------------
/**
 * Sets physical constant values for the Propagator.
 *
 * @param bodyMu     gravitational parameter to use
 * @param bodyJ2 J2  term to use
 * @param bodyRadius radius of the body
 * 
 */
//---------------------------------------------------------------------------
void Propagator::SetPhysicalConstants(Real bodyMu, Real bodyJ2,
                                      Real bodyRadius)
{
   mu       = bodyMu;
   J2       = bodyJ2;
   eqRadius = bodyRadius;
}

//------------------------------------------------------------------------------
// Rvector6 Propagate(const AbsoluteDate &toDate)
//------------------------------------------------------------------------------
/**
 * Propagates to the input time.
 *
 * @param toDate     date to which to propagate
 * 
 */
//---------------------------------------------------------------------------
Rvector6 Propagator::Propagate(const AbsoluteDate &toDate)
{
   // Propgate and return cartesian state given AbsoluteDate
   Real     propDuration = (toDate.GetJulianDate() -
                           refJd)*GmatTimeConstants::SECS_PER_DAY;
   Rvector6 orbElem      = PropagateOrbitalElements(propDuration);
   // Get the pointer to the orbit state
   OrbitState *ptrOrbitState = sc->GetOrbitState();
   ptrOrbitState->SetKeplerianVectorState(orbElem);
   Rvector6 cartState = ptrOrbitState->GetCartesianState();
   return cartState;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void SetOrbitState(OrbitState &orbState)
//------------------------------------------------------------------------------
/**
 * Sets the orbit state on the Propagator.
 *
 * @param orbState     orbit state
 * 
 */
//---------------------------------------------------------------------------
void Propagator::SetOrbitState(OrbitState *orbState)
{
   // Set orbit state given OrbitState object.
   
   // Done at intialization for performance reasons.
   Rvector6 kepElements = orbState->GetKeplerianState();
   SMA  = kepElements(0);
   ECC  = kepElements(1);
   INC  = kepElements(2);
   RAAN = kepElements(3);
   AOP  = kepElements(4);
   TA   = kepElements(5);
   MA   = StateConversionUtil::TrueToMeanAnomaly(TA,ECC);
}


//------------------------------------------------------------------------------
// Rvector6 PropagateOrbitalElements(Real propDuration)
//------------------------------------------------------------------------------
/**
 * Propagates the orbital elements for the specificed duration.
 *
 * @param propDuration     duration over which to propagate
 * 
 */
//---------------------------------------------------------------------------
Rvector6 Propagator::PropagateOrbitalElements(Real propDuration)
{
   // Propagate and return orbital elements
   Rvector6 orbElements; // all zeros by default
   orbElements(0) = SMA;
   orbElements(1) = ECC;
   orbElements(2) = INC;
   orbElements(3) = GmatMathUtil::Mod(RAAN +
                                      rightAscensionNodeRate*propDuration,
                                      GmatMathConstants::TWO_PI);
   orbElements(4) = GmatMathUtil::Mod(AOP + argPeriapsisRate*propDuration,
                                      GmatMathConstants::TWO_PI);
   Real newMA = GmatMathUtil::Mod(MA + meanMotionRate*propDuration,
                                  GmatMathConstants::TWO_PI);
   orbElements(5) = StateConversionUtil::MeanToTrueAnomaly(newMA,ECC);
   return orbElements;
}

//------------------------------------------------------------------------------
// Real MeanMotion()
//------------------------------------------------------------------------------
/**
 * Computes the mean motion.
 *
 * @return Mean Motion
 * 
 */
//---------------------------------------------------------------------------
Real Propagator::MeanMotion()
{
  // Computes the orbital mean motion
   Real meanMotion = GmatMathUtil::Sqrt(mu/(SMA*SMA*SMA));
   return meanMotion;
}

//------------------------------------------------------------------------------
// Real SemiParameter()
//------------------------------------------------------------------------------
/**
 * Computes the semi parameter.
 *
 * @return SemiParameter
 * 
 */
//---------------------------------------------------------------------------
Real Propagator::SemiParameter()
{
   // Computes the orbital semi parameter
   Real semiParameter = SMA * (1-ECC*ECC);
   return semiParameter;
}

//------------------------------------------------------------------------------
// void ComputeOrbitRates()
//------------------------------------------------------------------------------
/**
 * Computes the orbit rates.
 *
 */
//---------------------------------------------------------------------------
void Propagator::ComputeOrbitRates()
{
   // Compute orbit element rates
   ComputeMeanMotionRate();
   ComputeArgumentOfPeriapsisRate();
   ComputeRightAscensionNodeRate();
}

//------------------------------------------------------------------------------
// void ComputeMeanMotionRate()
//------------------------------------------------------------------------------
/**
 * Computes the mean motion rate.
 * 
 */
//---------------------------------------------------------------------------
void Propagator::ComputeMeanMotionRate()
{
   // Computes the orbital mean motion Rate
   // Vallado, 3rd. Ed.  Eq9-41
   Real n      = MeanMotion();
   Real tmpJ2  = J2;
   Real e      = ECC;
   Real p      = SemiParameter();
   Real sinInc = GmatMathUtil::Sin(INC);
   meanMotionRate = n - 0.75*n*tmpJ2*(eqRadius/p)*(eqRadius/p) *
                    GmatMathUtil::Sqrt(1.0-e*e)*(3.0*sinInc*sinInc - 2.0);
}

//------------------------------------------------------------------------------
// void ComputeArgumentOfPeriapsisRate()
//------------------------------------------------------------------------------
/**
 * Computes the argument of periapsis rate.
 * 
 */
//---------------------------------------------------------------------------
void Propagator::ComputeArgumentOfPeriapsisRate()
{
   // Computes the argument of periapsis rate
   // Vallado, 3rd. Ed.  Eq9-39
   Real n = MeanMotion();
   Real s = SemiParameter();
   Real sinInc = GmatMathUtil::Sin(INC);
   argPeriapsisRate = 3.0*n*(eqRadius*eqRadius)*J2/
                      4.0/(s*s)*(4.0 - 5.0*(sinInc*sinInc));
}

//------------------------------------------------------------------------------
// void ComputeRightAscensionNodeRate()
//------------------------------------------------------------------------------
/**
 * Computes the right ascension node rate.
 * 
 */
//---------------------------------------------------------------------------
void Propagator::ComputeRightAscensionNodeRate()
{
   // Computes rate right ascension of node rate
   // Vallado, 3rd. Ed.  Eq9-37
   Real n = MeanMotion();
   Real s = SemiParameter();
   rightAscensionNodeRate = -3.0*n*(eqRadius*eqRadius)*J2/
                             2.0/(s*s)*GmatMathUtil::Cos(INC);
}

// These should go away
//Real       MeanToTrueAnomaly(Real meanAnomaly, Real e);
//Real       TrueToEccentricAnomaly(Real trueAnomaly, Real e);
//Real       EccentricTiMeanAnomaly(eccentricAnomaly, Real e);
//Real       TrueToMeanAnomaly(trueAnomaly, Real e);


