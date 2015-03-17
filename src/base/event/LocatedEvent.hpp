//$Id$
//------------------------------------------------------------------------------
//                           LocatedEvent
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Sep 2, 2011
// Heavily Modified: Wendy Shoan/GSFC  2015.01.09
//
/**
 * Definition for the Event base class.
 */
//------------------------------------------------------------------------------

#ifndef LocatedEvent_hpp
#define LocatedEvent_hpp

#include "gmatdefs.hpp"

/**
 * Base LocatedEvent class
 */
class GMAT_API LocatedEvent
{
public:

   LocatedEvent();
   LocatedEvent(Real startEpoch, Real endEpoch);
   virtual ~LocatedEvent();
   LocatedEvent(const LocatedEvent& copy);
   LocatedEvent& operator=(const LocatedEvent& copy);

   /// Return computed quantities
   virtual Real         GetDuration();
   virtual Real         GetStart();
   virtual Real         GetEnd();

   /// Report string - abstract method to return the report string
   /// for the event
   virtual std::string  GetReportString() = 0;

   // Use these when you have used the default constructor and/or need to
   // compute the start and end times elsewhere and then set them here
   virtual void         SetStart(Real epoch1);
   virtual void         SetEnd(Real epoch2);

protected:
   /// Start time (A1Mjd)
   Real         start;
   /// End time   (A1Mjd)
   Real         end;

   /// Computed quantities
   /// Duration
   Real         duration;
};

#endif // LocatedEvent_hpp
