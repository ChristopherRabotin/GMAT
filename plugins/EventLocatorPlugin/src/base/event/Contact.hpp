//$Id: Contact.hpp 1915 2011-11-16 19:09:31Z djconway@NDC $
//------------------------------------------------------------------------------
//                           Contact
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Nov 7, 2011
//
/**
 * Definition of the ...
 */
//------------------------------------------------------------------------------


#ifndef Contact_hpp
#define Contact_hpp

#include "EventLocatorDefs.hpp"
#include "EventFunction.hpp"
#include "Elevation.hpp"
#include "LineOfSight.hpp"


class SpacePoint;
class CelestialBody;

/**
 * Container for monitoring contact between a station and a SpaceObject
 *
 * Contact events consist of a collection of an Elevation event function and
 * zero or more LineOfSight event functions that work together to determine if
 * contact was possible between the principal SpaceObject (Usually a Spacecraft)
 * and a station.
 *
 * The current implementation is designed to handle GroundStation objects as the
 * second object in the computations.
 *
 * @note: Work on this event type is temporarily on hold.  The current state is
 * a set of event functions coded but untested, and a containing ContactLocator
 * which has untested initialization but is missing the reference object
 * methods.
 */
class LOCATOR_API Contact : public EventFunction
{
public:
   Contact();
   virtual ~Contact();
   Contact(const Contact& c);
   Contact& operator=(const Contact& c);

   void SetStation(SpacePoint *s);

   bool HasEvent(EventFunction *ef);
   void SetEvent(EventFunction *ef);

   virtual bool Initialize();
   virtual Real* Evaluate(GmatEpoch atEpoch = -1.0, Real* forState = NULL);
   virtual void ClearEventFunctions();

protected:
   /// The station
   SpacePoint                 *station;

   /// The contained elevation event function
   Elevation                  *elevation;
   /// The contained line of sight event functions
   std::vector<LineOfSight*>  los;
};

#endif /* Contact_hpp */
