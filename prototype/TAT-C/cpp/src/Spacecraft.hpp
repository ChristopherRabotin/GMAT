//------------------------------------------------------------------------------
//                           Spacecraft
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
// Created: 2016.05.02
//
/**
 * Definition of the Spacecraft class.  This class contains data and
 * methods for a simple Spacecraft.
 */
//------------------------------------------------------------------------------
#ifndef Spacecraft_hpp
#define Spacecraft_hpp

#include "gmatdefs.hpp"
#include "OrbitState.hpp"
#include "AbsoluteDate.hpp"
#include "ConicalSensor.hpp"

class Spacecraft
{
public:
   
   // class methods
   Spacecraft(AbsoluteDate *epoch, OrbitState *state);
   Spacecraft( const Spacecraft &copy);
   Spacecraft& operator=(const Spacecraft &copy);
   
   virtual ~Spacecraft();
   
   virtual OrbitState*    GetOrbitState();
   virtual AbsoluteDate*  GetOrbitEpoch();
   virtual Real           GetJulianDate();
   virtual Rvector6       GetCartesianState();
   virtual void           AddSensor(ConicalSensor* sensor);
   virtual Real           GetSensorFOV(Integer forSensor);
   virtual bool           HasSensors();
   
protected:
   
   /// Orbit State
   OrbitState                  *orbitState;
   /// Orbit Epoch
   AbsoluteDate                *orbitEpoch;
   /// Number of attached sensors
   Integer                     numSensors;
   /// Vector of attached sensor objects
   std::vector<ConicalSensor*> sensorList;
};
#endif // Spacecraft_hpp