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
 * Implementation of the the visibility report base class
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "Spacecraft.hpp"
#include "TATCException.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
Spacecraft::Spacecraft(const AbsoluteDate &epoch, const OrbitState &state) :
   orbitState (state),
   orbitEpoch (epoch),
   numSensors (0)
{
   // sensorList is empty at start
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
Spacecraft::Spacecraft( const Spacecraft &copy) :
   orbitState (copy.orbitState),
   orbitEpoch (copy.orbitEpoch),
   numSensors (copy.numSensors)
{
   if (copy.numSensors > 0)
   {
      sensorList.clear();
      for (Integer ii = 0; ii < copy.numSensors; ii++)
         sensorList.push_back(copy.sensorList.at(ii)); // need Clone here?
   }
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
Spacecraft& Spacecraft::operator=(const Spacecraft &copy)
{
   if (&copy == this)
      return *this;
   
   orbitEpoch    = copy.orbitEpoch;
   orbitState    = copy.orbitState;
   numSensors    = copy.numSensors;
   
   sensorList.clear();
   for (Integer ii = 0; ii < copy.numSensors; ii++)
      sensorList.push_back(copy.sensorList.at(ii)); // need Clone here?

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------

Spacecraft::~Spacecraft()
{
}

//------------------------------------------------------------------------------
//  OrbitState GetOrbitState()
//------------------------------------------------------------------------------
OrbitState Spacecraft::GetOrbitState()
{
   return orbitState;
}

//------------------------------------------------------------------------------
//  AbsoluteDate GetOrbitEpoch()
//------------------------------------------------------------------------------
AbsoluteDate Spacecraft::GetOrbitEpoch()
{
   return orbitEpoch;
}

//------------------------------------------------------------------------------
//  Real GetJulianDate()
//------------------------------------------------------------------------------
Real Spacecraft::GetJulianDate()
{
   return orbitEpoch.GetJulianDate();
}

//------------------------------------------------------------------------------
//  const Rvector6& GetCartesianState()
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetCartesianState()
{
   return orbitState.GetCartesianState();
}

//------------------------------------------------------------------------------
//  void AddSensor(ConicalSensor* sensor)
//------------------------------------------------------------------------------
void Spacecraft::AddSensor(ConicalSensor* sensor)
{
   // @todo - check for sensor already on list!!
   sensorList.push_back(sensor);
   numSensors++;
}

//------------------------------------------------------------------------------
//  Real GetSensorFOV(Integer forSensor)
//------------------------------------------------------------------------------
Real Spacecraft::GetSensorFOV(Integer forSensor)
{
   if (numSensors == 0)
   {
      throw TATCException("ERROR - Spacecraft has no sensors\n");
   }

   if ((forSensor < 0) || (forSensor >= numSensors))
      throw TATCException("ERROR - sensor number out-of-bounds in Spacecraft\n");

   return sensorList.at(forSensor)->GetFieldOfView();
}

//------------------------------------------------------------------------------
//  bool HasSensors()
//------------------------------------------------------------------------------
bool Spacecraft::HasSensors()
{
   return (numSensors > 0);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// None
