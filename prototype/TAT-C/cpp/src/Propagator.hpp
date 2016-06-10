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
// Created: 2016.05.05
//
/**
 * Definition of the the propagator class.
 */
//------------------------------------------------------------------------------
#ifndef Propagator_hpp
#define Propagator_hpp

#include "gmatdefs.hpp"
#include "AbsoluteDate.hpp"
#include "Spacecraft.hpp"
#include "OrbitState.hpp"
#include "Rvector6.hpp"

class Propagator
{
public:
   
   // class methods
   Propagator(Spacecraft *sat);
   Propagator( const Propagator &copy);
   Propagator& operator=(const Propagator &copy);
   
   virtual ~Propagator();
   
   /// Set the body physical constants on teh propagator
   virtual void      SetPhysicalConstants(Real bodyMu, Real bodyJ2,
                                          Real bodyRadius);
   /// Propagate the spacecraft
   virtual Rvector6  Propagate(const AbsoluteDate &toDate);
   
protected:
   
   /// The spacecraft to be propagated
   Spacecraft   *sc;
   /// J2 term for Earth
   Real         J2;
   /// Gravitational parameter of the Earth
   Real         mu;
   /// Equatorial radius of the Earth
   Real         eqRadius;
   
   /// Julian date of the reference orbital elements
   Real         refJd;
   /// The epoch at which the new state is requested
   AbsoluteDate propEpoch;
   
   /// Orbital semi-major axis
   Real         SMA;
   /// Orbital eccentricity
   Real         ECC;
   /// Orbital inclination
   Real         INC;
   /// Orbital right ascention of the ascending node
   Real         RAAN;
   /// Orbital sargument of periapsis
   Real         AOP;
   /// Orbital true anomaly
   Real         TA;
   /// Orbital true anomaly
   Real         MA;
   
   /// Te drift in mean motion caused by J2
   Real         meanMotionRate;
   /// The drift in argument of periapsis caused by J2
   Real         argPeriapsisRate;
   /// The drift in right ascention of the ascending node caused by J2
   Real         rightAscensionNodeRate;
   /// The orbital semi-latus rectum
   Real         semiLatusRectum;
   /// The orbital mean motion
   Real         meanMotion;
   
   void       SetOrbitState(OrbitState *orbState);
//   Rvector6   PropagateCartesianState(Real propDuration);
   Rvector6   PropagateOrbitalElements(Real propDuration);
   Real       MeanMotion();
   Real       SemiParameter();
   // The next few should not be needed, as we can call StateConversionUtil
//   Real       MeanToTrueAnomaly(Real meanAnomaly, Real e);
//   Real       TrueToEccentricAnomaly(Real trueAnomaly, Real e);
//   Real       EccentricTiMeanAnomaly(eccentricAnomaly, Real e);
//   Real       TrueToMeanAnomaly(trueAnomaly, Real e);
   void       ComputeOrbitRates();
   void       ComputeMeanMotionRate();
   void       ComputeArgumentOfPeriapsisRate();
   void       ComputeRightAscensionNodeRate();
   
};
#endif // Propagator_hpp