//$Id: TDRSSTwoWayRange.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         TDRSSTwoWayRange
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
};

#endif /* TDRSSTwoWayRange_hpp */
