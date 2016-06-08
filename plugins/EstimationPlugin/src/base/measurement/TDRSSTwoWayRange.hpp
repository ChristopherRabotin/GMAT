//$Id: TDRSSTwoWayRange.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         TDRSSTwoWayRange
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS Task 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2010/05/10
//
/**
 * The TDRSS 2-way range core measurement model.
 */
//------------------------------------------------------------------------------


#ifndef TDRSSTwoWayRange_hpp
#define TDRSSTwoWayRange_hpp

#include "estimation_defs.hpp"
#include "TwoWayRange.hpp"


/**
 * TDRSS 2-Way Range Measurement Model
 */
class ESTIMATION_API TDRSSTwoWayRange: public TwoWayRange
{
public:
   TDRSSTwoWayRange(const std::string nomme = "");
   virtual ~TDRSSTwoWayRange();
   TDRSSTwoWayRange(const TDRSSTwoWayRange& usn);
   TDRSSTwoWayRange& operator=(const TDRSSTwoWayRange& usn);

   virtual GmatBase* Clone() const;

   bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
         const std::string &name);
   bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
         const std::string &name, const Integer index);

   virtual bool Initialize();

   virtual const std::vector<RealArray>& CalculateMeasurementDerivatives(
         GmatBase *obj, Integer id);

   virtual Event* GetEvent(UnsignedInt whichOne);
   virtual bool SetEventData(Event *locatedEvent);

protected:
   /// Turnaround time at the TDRSS (aka transponder delay) on way to the s/c
   Real                 tdrssUplinkDelay;
   /// Turnaround time at the TDRSS (aka transponder delay) on way to the ground
   Real                 tdrssDownlinkDelay;
   /// Light transit time for the forward link
   Real                 forwardlinkTime;
   /// Light transit time for the return link
   Real                 backlinkTime;

   /// The event used to model the uplink
   LightTimeCorrection  forwardlinkLeg;
   /// The event used to model the downlink
   LightTimeCorrection  backlinkLeg;

   /// The distance covered during the uplink
   Real                 forwardlinkRange;
   /// The distance covered during the downlink
   Real                 backlinkRange;

   virtual bool         Evaluate(bool withEvents = false);

   void                 SetHardwareDelays(bool loadEvents);

   virtual void         InitializeMeasurement();
};

#endif /* TDRSSTwoWayRange_hpp */
