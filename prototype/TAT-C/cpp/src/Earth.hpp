//------------------------------------------------------------------------------
//                           Earth
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
// Created: 2016.05.03
//
/**
 * Definition of the Earth class.  This class is a simple model of the Earth.
 */
//------------------------------------------------------------------------------
#ifndef Earth_hpp
#define Earth_hpp

#include "gmatdefs.hpp"
#include "OrbitState.hpp"
#include "AbsoluteDate.hpp"
#include "Rmatrix33.hpp"
#include "Rvector3.hpp"

class Earth
{
public:
   
   // class methods
   Earth();
   Earth( const Earth &copy);
   Earth& operator=(const Earth &copy);
   
   virtual ~Earth();
   
   virtual Rmatrix33        GetInertialToFixedRotation(Real jd);
   virtual Real             ComputeGMT(Real jd);
   virtual Rvector3         GetBodyFixedState(Rvector3 inertialState,
                                              Real      jd);
   virtual Rmatrix33        FixedToTopo(Real gdLat, Real gdLon);
   virtual Real             GeocentricToGeodeticLat(Real gcLat);
   virtual void             GetEarthSunDistRaDec(Real jd,    Rvector3 &rSun,
                                                 Real rtAsc, Real decl);
   
protected:
   
   /// J2 term for Earth
   Real      J2;      // units??
   /// Gravitational parameter of the Earth
   Real      mu;       // units??
   /// Equatorial radius of the Earth
   Real      radius;  // km
   /// Flattening of the Earth
   Real      flattening;
};
#endif // Earth_hpp