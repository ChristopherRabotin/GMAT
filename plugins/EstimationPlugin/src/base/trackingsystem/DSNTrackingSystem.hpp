//$Id: DSNTrackingSystem.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                             DSNTrackingSystem
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
// Created: 2010/02/22 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the DSNTrackingSystem class
 */
//------------------------------------------------------------------------------


#ifndef DSNTrackingSystem_hpp
#define DSNTrackingSystem_hpp

#include "estimation_defs.hpp"
#include "TrackingSystem.hpp"

/**
 * The TrackingSystem model used for the deep space network (DSN).
 */
class ESTIMATION_API DSNTrackingSystem : public TrackingSystem
{
public:
   DSNTrackingSystem(const std::string &name);
   virtual ~DSNTrackingSystem();
   DSNTrackingSystem(const DSNTrackingSystem& dts);
   DSNTrackingSystem& operator=(const DSNTrackingSystem& dts);

   virtual GmatBase* Clone() const;
};

#endif /* DSNTrackingSystem_hpp */
