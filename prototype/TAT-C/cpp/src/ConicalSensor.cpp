//------------------------------------------------------------------------------
//                           ConicalSensor
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
#include "ConicalSensor.hpp"

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
ConicalSensor::ConicalSensor(Real fov) :
   fieldOfView (0.0)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ConicalSensor::ConicalSensor( const ConicalSensor &copy) :
   fieldOfView (copy.fieldOfView)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ConicalSensor& ConicalSensor::operator=(const ConicalSensor &copy)
{
   if (&copy == this)
      return *this;
   
   fieldOfView    = copy.fieldOfView;
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------

ConicalSensor::~ConicalSensor()
{
}

//------------------------------------------------------------------------------
//  void SetFieldOfView(Real fov)
//------------------------------------------------------------------------------
void ConicalSensor::SetFieldOfView(Real fov)
{
   fieldOfView =fov;
}

//------------------------------------------------------------------------------
//  Real GetFieldOfView()
//------------------------------------------------------------------------------
Real ConicalSensor::GetFieldOfView()
{
   return fieldOfView;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// None
