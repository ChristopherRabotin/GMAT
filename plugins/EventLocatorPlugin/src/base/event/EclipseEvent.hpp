//$Id$
//------------------------------------------------------------------------------
//                               EclipseEvent
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2015.01.08
//
/**
 * Definition for the EclipseEvent class.
 */
//------------------------------------------------------------------------------

#ifndef EclipseEvent_hpp
#define EclipseEvent_hpp

#include "gmatdefs.hpp"
#include "EventLocatorDefs.hpp"
#include "LocatedEvent.hpp"

/**
 * EclipseEvent class
 */
class LOCATOR_API EclipseEvent : public LocatedEvent
{
public:

   EclipseEvent();
   EclipseEvent(Real startEpoch, Real endEpoch, const std::string itsType, const std::string theBody);
   virtual ~EclipseEvent();
   EclipseEvent(const EclipseEvent& copy);
   EclipseEvent& operator=(const EclipseEvent& copy);

   /// Return computed quantities

   // Get the report string
   virtual std::string  GetReportString();

protected:
   /// The eclipse type
   std::string  eclipseType;
   ///The name of the occulting body (or bodies)
   std::string  occultingBody;
};

#endif // EclipseEvent_hpp
