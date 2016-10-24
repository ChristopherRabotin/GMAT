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
 * Implementation of the Spacecraft class.
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

//---------------------------------------------------------------------------
//  Spacecraft(AbsoluteDate *epoch, OrbitState *state)
//---------------------------------------------------------------------------
/**
 * Default constructor for Spacecraft.
 *
 * @param epoch The orbit epoch object
 * @param state The orbit state object
 * 
 */
//---------------------------------------------------------------------------
Spacecraft::Spacecraft(AbsoluteDate *epoch, OrbitState *state) :
   orbitState (state),
   orbitEpoch (epoch),
   numSensors (0)
{
   // sensorList is empty at start
}

//---------------------------------------------------------------------------
//  Spacecraft(const Spacecraft &copy)
//---------------------------------------------------------------------------
/**
 * Copy constructor for Spacecraft.
 *
 * @param copy The spacecraft to copy
 * 
 */
//---------------------------------------------------------------------------
Spacecraft::Spacecraft(const Spacecraft &copy) :
   orbitState (copy.orbitState),   // Clone these?
   orbitEpoch (copy.orbitEpoch),   // Clone these?
   numSensors (copy.numSensors)
{
   if (copy.numSensors > 0)
   {
      sensorList.clear();
      for (Integer ii = 0; ii < copy.numSensors; ii++)
         sensorList.push_back(copy.sensorList.at(ii)); // need Clone here?
   }
}

//---------------------------------------------------------------------------
//  Spacecraft& operator=(const Spacecraft &copy)
//---------------------------------------------------------------------------
/**
 * operator= for Spacecraft.
 *
 * @param copy The spacecraft whose values to copy
 * 
 */
//---------------------------------------------------------------------------
Spacecraft& Spacecraft::operator=(const Spacecraft &copy)
{
   if (&copy == this)
      return *this;
   
   orbitEpoch    = copy.orbitEpoch;  // Clone these?
   orbitState    = copy.orbitState;  // Clone these?
   numSensors    = copy.numSensors;
   
   sensorList.clear();
   for (Integer ii = 0; ii < copy.numSensors; ii++)
      sensorList.push_back(copy.sensorList.at(ii)); // need Clone here?

   return *this;
}

//---------------------------------------------------------------------------
//  ~Spacecraft()
//---------------------------------------------------------------------------
/**
 * destructor for Spacecraft.
 *
 */
//---------------------------------------------------------------------------
Spacecraft::~Spacecraft()
{
}

//---------------------------------------------------------------------------
//  OrbitState* GetOrbitState()
//---------------------------------------------------------------------------
/**
 * Returns a pointer to the Spacecraft's OrbitState object.
 *
 * @return  pointer to the spacecraft's OrbitState
 * 
 */
//---------------------------------------------------------------------------
OrbitState* Spacecraft::GetOrbitState()
{
   return orbitState;
}

//---------------------------------------------------------------------------
//  AbsoluteDate* GetOrbitEpoch()
//---------------------------------------------------------------------------
/**
 * Returns a pointer to the Spacecraft's AbsoluteDate object.
 *
 * @return  pointer to the spacecraft's AbsoluteDate
 * 
 */
//---------------------------------------------------------------------------
AbsoluteDate* Spacecraft::GetOrbitEpoch()
{
   return orbitEpoch;
}

//---------------------------------------------------------------------------
//  Real GetJulianDate()
//---------------------------------------------------------------------------
/**
 * Returns the Spacecraft's Julian Date.
 *
 * @return  Spacecraft's JulianDate
 * 
 */
//---------------------------------------------------------------------------
Real Spacecraft::GetJulianDate()
{
   return orbitEpoch->GetJulianDate();
}

//---------------------------------------------------------------------------
//  Rvector6 GetCartesianState()
//---------------------------------------------------------------------------
/**
 * Returns the Spacecraft's cartesian state.
 *
 * @return  Spacecraft's cartesian state
 * 
 */
//---------------------------------------------------------------------------
Rvector6 Spacecraft::GetCartesianState()
{
   return orbitState->GetCartesianState();
}

//---------------------------------------------------------------------------
//  void AddSensor(ConicalSensor* sensor)
//---------------------------------------------------------------------------
/**
 * Adds the input sensor to the Spacecraft's sensor list.
 *
 * @param  sensor Sensor to add to the list
 * 
 */
//---------------------------------------------------------------------------
void Spacecraft::AddSensor(ConicalSensor* sensor)
{
   // @todo - check for sensor already on list!!
   sensorList.push_back(sensor);
   numSensors++;
}

//---------------------------------------------------------------------------
//  Real GetSensorFOV(Integer forSensor)
//---------------------------------------------------------------------------
/**
 * Returns the field-of-view of the specified sensor.
 *
 * @param  forSensor ID (index) of the spacecraft whose FOV to return
 * 
 */
//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
//  bool HasSensors()
//---------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the spacecraft has sensors.
 *
 * @return  flag indicating whether or not the spacecraft has sensors.
 * 
 */
//---------------------------------------------------------------------------
bool Spacecraft::HasSensors()
{
   return (numSensors > 0);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// None
