//$Id: LineOfSight.hpp 1915 2011-11-16 19:09:31Z djconway@NDC $
//------------------------------------------------------------------------------
//                           LineOfSight
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
 * Definition of the line of sight event function
 */
//------------------------------------------------------------------------------


#ifndef LineOfSight_hpp
#define LineOfSight_hpp

#include "EventFunction.hpp"

class CelestialBody;

/**
 * Event function used to find line of sight obstruction boundaries
 */
class LineOfSight : public EventFunction
{
public:
   LineOfSight();
   virtual ~LineOfSight();
   LineOfSight(const LineOfSight& los);
   LineOfSight& operator=(const LineOfSight& los);

   void SetSecondary(SpacePoint *s);
   void SetBody(CelestialBody *cb);
   virtual bool Initialize();
   virtual Real* Evaluate(GmatEpoch atEpoch = -1.0, Real* forState = NULL);

protected:
   /// The other endpoint
   SpacePoint        *secondary;
   /// The body that might intervene
   CelestialBody     *body;
   /// The radius of the occluding body.
   Real bodyRadius;

   /// ID for equatorial radius of the sun and body
   Integer eqRadiusID;
};

#endif /* LineOfSight_hpp */
