//------------------------------------------------------------------------------
//                           OrbitState
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
// Created: 2016.05.04
//
/**
 * Definition of the OrbitState class.  This class computes and converts
 * Cartesian and Keplerian states.
 */
//------------------------------------------------------------------------------
#ifndef OrbitState_hpp
#define OrbitState_hpp

#include "gmatdefs.hpp"
#include "Rvector6.hpp"

class OrbitState
{
public:
   
   // class methods
   OrbitState();
   OrbitState( const OrbitState &copy);
   OrbitState& operator=(const OrbitState &copy);
   
   virtual ~OrbitState();
   
   virtual void     SetKeplerianState(Real SMA, Real ECC,
                                      Real INC, Real RAAN,
                                      Real AOP, Real TA);
   virtual void     SetKeplerianVectorState(const Rvector6 &kepl);
   virtual void     SetCartesianState(const Rvector6 &cart);
   virtual void     SetGravityParameter(Real toGrav);
   virtual Rvector6 GetKeplerianState();
   virtual Rvector6 GetCartesianState();
   
protected:
   
   /// Current stat in cartsian format
   Rvector6  currentState;
   /// Gravitational parameter for the central body
   Real      mu;       // units??
   
   Rvector6         ConvertKeplerianToCartesian(Real a,  Real e,
                                                Real i,  Real Om,
                                                Real om, Real nu);
   Rvector6         ConvertCartesianToKeplerian(const Rvector6 &cart);
};
#endif // OrbitState_hpp