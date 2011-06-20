//$Id$
//------------------------------------------------------------------------------
//                           HarmonicGravityBody
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: John P. Downing/GSFC/595
// Created: 2010.10.28
// (modified for style, etc. by Wendy Shoan/GSFC/583 2011.05.31)
//
/**
 * This class returns handles data obtained form the celestial body.
 */
//------------------------------------------------------------------------------
#ifndef HarmonicGravityBody_hpp
#define HarmonicGravityBody_hpp

#include "HarmonicGravity.hpp"
#include "CelestialBody.hpp"

class HarmonicGravityBody : public HarmonicGravity
{
public:
   HarmonicGravityBody(CelestialBody* body);
   virtual ~HarmonicGravityBody();

protected:
   CelestialBody* Body;
   virtual void Load();

private:  // No copying allowed, so these are declared private, but not implemented
   HarmonicGravityBody(const HarmonicGravityBody& x);
   HarmonicGravityBody& operator=(const HarmonicGravityBody& x);
};

#endif // HarmonicGravityBody_hpp
