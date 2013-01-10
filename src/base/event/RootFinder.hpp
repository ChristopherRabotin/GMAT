//$Id$
//------------------------------------------------------------------------------
//                           RootFinder
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
// Created: Sep 19, 2011
//
/**
 * Definition of the ...
 */
//------------------------------------------------------------------------------


#ifndef RootFinder_hpp
#define RootFinder_hpp

#include "gmatdefs.hpp"
#include "FormationInterface.hpp"

class Spacecraft;
class PropSetup;
class Event;


class RootFinder
{
public:
   RootFinder(std::string finderType);
   virtual ~RootFinder();
   RootFinder(const RootFinder& rf);
   RootFinder& operator=(const RootFinder& rf);

//   void SetEventVector(std::vector<Event*> *evs);
//   virtual void SetPropSetup(PropSetup* ps);
//   virtual void FixState(Event *thisOne);
//   virtual std::vector<GmatEpoch> LocateEvents(const IntegerArray &whichOnes);
   virtual bool Initialize(GmatEpoch t0, Real f0, GmatEpoch t1, Real f1);
   virtual bool SetValue(GmatEpoch forEpoch, Real withValue);
   virtual Real GetStep(const GmatEpoch currentEpoch = -1.0);
   virtual Real GetStepMeasure();
   virtual void GetBrackets(GmatEpoch &start, GmatEpoch &end);

protected:
   /// Type of the root finding algorithm
   std::string typeName;
   /// Tolerance for the root finding algorithm
   Real tolerance;
   /// Data size for the buffers; this part is algorithm dependent
   Integer bufferSize;
   /// Buffer of the data collected while searching
   Real *buffer;
   /// Buffer of the epoch data associated with the data buffer
   GmatEpoch *epochBuffer;

   /// The propagator used to locate the root
   PropSetup *propagator;
   /// The maximum number of location attempts before the search aborts
   Integer maxAttempts;

   /// Buffer of Spacecraft used to restore the initial data
   std::vector<Spacecraft *>    satBuffer;
   /// Buffer of Formations used to restore the initial data
   std::vector<FormationInterface *>     formBuffer;
   /// The current set of events that the EstimationRootFinder is using
   std::vector<Event*> *events;

//   virtual bool FindRoot(Integer whichOne, GmatEpoch roots[2]);
   virtual Real FindStep(const GmatEpoch currentEpoch) = 0;
//   virtual void BufferSatelliteStates(bool fillingBuffer);

   void Swap(Integer i1, Integer i2);
};

#endif /* RootFinder_hpp */
