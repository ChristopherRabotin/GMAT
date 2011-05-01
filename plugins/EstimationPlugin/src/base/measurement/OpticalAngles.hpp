//$Id$
//------------------------------------------------------------------------------
//                             OpticalAngles
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
