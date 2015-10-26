//$Id$
//------------------------------------------------------------------------------
//                             OpticalTrackingSystem
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Darrel J. Conway
// Created: Mar 23, 2010 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the OpticalTrackingSystem class
 */
//------------------------------------------------------------------------------


#include "OpticalTrackingSystem.hpp"


//------------------------------------------------------------------------------
// OpticalTrackingSystem(const std::string & name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The tracking system name
 */
//------------------------------------------------------------------------------
OpticalTrackingSystem::OpticalTrackingSystem(const std::string & name) :
   TrackingSystem("OpticalTrackingSystem", name)
{
   objectTypeNames.push_back("OpticalTrackingSystem");

   // Eventually may want to do this, but for now leave this TS open to all
//   for (Integer i = Gmat::GENERAL_FIRST_MEASUREMENT;
//         i < Gmat::GENERAL_MAX_MEASUREMENT; ++i)
//   {
//      allowedMeasurements.push_back(i);
//   }
}


//------------------------------------------------------------------------------
// ~OpticalTrackingSystem()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
OpticalTrackingSystem::~OpticalTrackingSystem()
{
}


//------------------------------------------------------------------------------
// OpticalTrackingSystem(const OpticalTrackingSystem &ots)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ots The instance providing configuration data for the new one
 */
//------------------------------------------------------------------------------
OpticalTrackingSystem::OpticalTrackingSystem(const OpticalTrackingSystem &ots) :
   TrackingSystem          (ots)
{
}


//------------------------------------------------------------------------------
// OpticalTrackingSystem& operator=(const OpticalTrackingSystem& ots)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ots The instance providing configuration data for this one
 *
 * @return This instance, configured to match ots
 */
//------------------------------------------------------------------------------
OpticalTrackingSystem& OpticalTrackingSystem::operator=(
      const OpticalTrackingSystem& ots)
{
   if (this != &ots)
   {
      TrackingSystem::operator=(ots);
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Replicates this object through a call made on a GmatBase pointer
 *
 * @return A new instance generated through the copy constructor for this object
 */
//------------------------------------------------------------------------------
GmatBase *OpticalTrackingSystem::Clone() const
{
   return new OpticalTrackingSystem(*this);
}
