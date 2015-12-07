//$Id: OpticalTrackingSystem.hpp 1398 2011-04-21 20:39:37Z  $
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


#ifndef OpticalTrackingSystem_hpp
#define OpticalTrackingSystem_hpp

/**
 * Base class for tracking systems that use optical (typically angle)
 * measurements that include light time iteration.
 */
#include "estimation_defs.hpp"
#include "TrackingSystem.hpp"

class ESTIMATION_API OpticalTrackingSystem : public TrackingSystem
{
public:
   OpticalTrackingSystem(const std::string &name);
   virtual ~OpticalTrackingSystem();
   OpticalTrackingSystem(const OpticalTrackingSystem& usn);
   OpticalTrackingSystem& operator=(const OpticalTrackingSystem& usn);

   virtual GmatBase* Clone() const;
};

#endif /* OpticalTrackingSystem_hpp */
