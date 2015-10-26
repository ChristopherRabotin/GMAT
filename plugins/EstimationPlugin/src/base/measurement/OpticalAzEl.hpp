//$Id: OpticalAzEl.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                             OpticalAzEl
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
 * Implementation for the OpticalAzEl class
 */
//------------------------------------------------------------------------------


#ifndef OpticalAzEl_hpp
#define OpticalAzEl_hpp


#include "estimation_defs.hpp"
#include "OpticalAngles.hpp"

/**
 * Optical Azimuth/Elevation angle measurements
 *
 * OpticalAzEl provides the measurement model for Azimuth and Elevation angle
 * pairs that together constitute a single optical measurement.  The observation
 * includes calculations for the light travel time from the target object to the
 * sensor.  The current code does not take into account
 */
class ESTIMATION_API OpticalAzEl : public OpticalAngles
{
public:
   OpticalAzEl(const std::string &name = "");
   virtual ~OpticalAzEl();
   OpticalAzEl(const OpticalAzEl& oae);
   OpticalAzEl& operator=(const OpticalAzEl& oae);

   virtual bool Initialize();
   virtual GmatBase* Clone() const;
   virtual const std::vector<RealArray>& CalculateMeasurementDerivatives(
            GmatBase *obj, Integer id);

protected:
   /// Electronics delay at the receiver
   Real receiveDelay;
   /// The range vector in the observer's coordinate frame
   Rvector3 obsRange;

   /// Derived classes override this method to implement measurement
   /// calculations
   virtual bool                    Evaluate(bool withEvents = false);
};

#endif /* OpticalAzEl_hpp */
