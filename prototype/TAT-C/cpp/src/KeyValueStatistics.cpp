//------------------------------------------------------------------------------
//                           KeyValueStatistics
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
#include "KeyValueStatistics.hpp"

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
KeyValueStatistics::KeyValueStatistics(Real minVal, Real maxVal, Real avgVal) :
   minValue (minVal),
   maxValue (maxVal),
   avgValue (avgVal)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
KeyValueStatistics::KeyValueStatistics( const KeyValueStatistics &copy) :
   minValue   (copy.minValue),
   maxValue   (copy.maxValue),
   avgValue   (copy.avgValue)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
KeyValueStatistics& KeyValueStatistics::operator=(const KeyValueStatistics &copy)
{
   if (&copy == this)
      return *this;
   
   minValue    = copy.minValue;
   maxValue    = copy.maxValue;
   avgValue    = copy.avgValue;
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------

KeyValueStatistics::~KeyValueStatistics()
{
}

//------------------------------------------------------------------------------
//  Real GetMinValue()
//------------------------------------------------------------------------------
Real KeyValueStatistics::GetMinValue()
{
   return minValue;
}

//------------------------------------------------------------------------------
//  Real GetMaxValue()
//------------------------------------------------------------------------------
Real KeyValueStatistics::GetMaxValue()
{
   return maxValue;
}

//------------------------------------------------------------------------------
//  Real GetAvgValue()
//------------------------------------------------------------------------------
Real KeyValueStatistics::GetAvgValue()
{
   return avgValue;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// None
