//$Id: Elevation.hpp 1915 2011-11-16 19:09:31Z djconway@NDC $
//------------------------------------------------------------------------------
//                           Elevation
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
// Created: Nov 9, 2011
//
/**
 * Definition of the elevation event function
 */
//------------------------------------------------------------------------------


#ifndef Elevation_hpp
#define Elevation_hpp

#include "EventFunction.hpp"
#include "CoordinateConverter.hpp"


/**
 * The EventFunction used for Elevation events
 */
class Elevation : public EventFunction
{
public:
   Elevation();
   virtual ~Elevation();
   Elevation(const Elevation& c);
   Elevation& operator=(const Elevation& c);

   void SetStation(SpacePoint *s);
   virtual bool Initialize();
   virtual Real* Evaluate(GmatEpoch atEpoch = -1.0, Real* forState = NULL);

protected:
   /// The station
   SpacePoint        *station;
   /// Flag indicating if the station has a mask (currently ignored)
   bool              isMasked;
   /// Station coord. system used to put computations in correct reference frame
   CoordinateSystem  *bfcs;
   /// A MJ2000 coordinate system
   CoordinateSystem  *mj2kcs;
   /// Converter helper
   CoordinateConverter ccvtr;
};

#endif /* Elevation_hpp */
