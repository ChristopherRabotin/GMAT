//$Id: RootFinder.hpp,v 1.2 2010/01/19 00:00:07 djc Exp $
//------------------------------------------------------------------------------
//                         RootFinder
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/09
//
/**
 * The root finder used to locate the epoch for events
 */
//------------------------------------------------------------------------------


#ifndef RootFinder_hpp
#define RootFinder_hpp


#include "gmatdefs.hpp"
#include "PropSetup.hpp"
#include "Propagator.hpp"
#include "ODEModel.hpp"
#include "GmatState.hpp"
#include "Event.hpp"

#include "Spacecraft.hpp"
#include "Formation.hpp"


/**
 * Locates roots in Event objects
 */
class ESTIMATION_API RootFinder
{
public:
   RootFinder();
   ~RootFinder();
   RootFinder(const RootFinder& rl);
   RootFinder& operator=(const RootFinder& rl);

   void SetPropSetup(PropSetup* ps);
   void FixState(Event *thisOne);
   Real Locate(ObjectArray &whichOnes);

protected:
   /// The propagator used to locate the root
   PropSetup *propagator;
   /// The maximum number of location attempts before the search aborts
   Integer maxAttempts;
   /// The starting state of the root location search
   GmatState startState;

   /// Buffer of Spacecraft used to restore the initial data
   std::vector<Spacecraft *>    satBuffer;
   /// Buffer of Formations used to restore the initial data
   std::vector<Formation *>     formBuffer;
   /// The current set of events that the RootFinder is using
   std::vector<Event*> *events;

   Real FindRoot(Integer whichOne);
   void BufferSatelliteStates(bool fillingBuffer);
};

#endif // RootFinder_hpp
