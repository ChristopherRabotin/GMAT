//$Id: TwoWayRange.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         TwoWayRange
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/18
//
/**
 * The 2-way range core measurement model base class.
 */
//------------------------------------------------------------------------------


#ifndef TwoWayRange_hpp
#define TwoWayRange_hpp

#include "estimation_defs.hpp"
#include "PhysicalMeasurement.hpp"
#include "LightTimeCorrection.hpp"
#include "Rvector3.hpp"


/**
 * Base class for 2-way range real world measurement models
 */
class ESTIMATION_API TwoWayRange: public PhysicalMeasurement
{
public:
   TwoWayRange(const std::string &type, const std::string &nomme = "");
   virtual ~TwoWayRange();
   TwoWayRange(const TwoWayRange& twr);
   TwoWayRange& operator=(const TwoWayRange& twr);

   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);

   virtual bool Initialize();
   virtual Event* GetEvent(UnsignedInt whichOne);
   virtual bool SetEventData(Event *locatedEvent = NULL);

protected:
   /// Epoch when the measurement was received.  For now, the anchor epoch.
   GmatEpoch            tR;
   /// Epoch when the measurement was transmitted.
   GmatEpoch            tT;
   /// Epoch
   GmatEpoch            tV;

   /// Delay time in the electronics of the transmitter, in seconds
   Real                 transmitDelay;
   /// Turnaround time at the target (aka transponder delay)
   Real                 targetDelay;
   /// Delay time in the electronics of the receiver, in seconds
   Real                 receiveDelay;
   /// Light transit time for the uplink
   Real                 uplinkTime;
   /// Light transit time for the downlink
   Real                 downlinkTime;

   /// The event used to model the uplink
   LightTimeCorrection  uplinkLeg;
   /// The event used to model the downlink
   LightTimeCorrection  downlinkLeg;

   /// The distance covered during the uplink
   Real                 uplinkRange;
   /// The distance covered during the downlink
   Real                 downlinkRange;

   virtual void         InitializeMeasurement();
   virtual void         SetHardwareDelays(bool loadEvents = true);
};

#endif /* TwoWayRange_hpp */
