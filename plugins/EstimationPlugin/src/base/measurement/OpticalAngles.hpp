//$Id$
//------------------------------------------------------------------------------
//                             OpticalAngles
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
 * Implementation for the OpticalAngles class
 */
//------------------------------------------------------------------------------


#ifndef OpticalAngles_hpp
#define OpticalAngles_hpp

#include "estimation_defs.hpp"
#include "LightTimeCorrection.hpp"


/**
 * OpticalAngles ...
 */
#include "PhysicalMeasurement.hpp"

class ESTIMATION_API OpticalAngles : public PhysicalMeasurement
{
public:
   OpticalAngles(const std::string &type, const std::string &nomme);
   virtual ~OpticalAngles();
   OpticalAngles(const OpticalAngles& oa);
   OpticalAngles& operator=(const OpticalAngles& oa);

   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
            const std::string &name = "");
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
            const std::string &name, const Integer index);

   virtual bool Initialize();
   virtual Event* GetEvent(UnsignedInt whichOne);
   virtual bool SetEventData(Event *locatedEvent = NULL);

protected:
   /// Epoch when the measurement was received.
   GmatEpoch            tR;
   /// Epoch the signal left the target
   GmatEpoch            tT;
   /// Delay time in the electronics of the receiver, in seconds
   Real                 receiveDelay;
   /// Light transit time for the signal
   Real                 transitTime;

   /// Light path to the detector
   LightTimeCorrection  lightPath;

   /// Derived classes override this method to implement measurement
   /// calculations
   virtual bool         Evaluate(bool withEvents = false) = 0;
   virtual void         InitializeMeasurement();
};

#endif /* OpticalAngles_hpp */
