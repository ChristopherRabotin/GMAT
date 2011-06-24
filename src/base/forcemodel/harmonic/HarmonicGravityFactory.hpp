//$Id$
//------------------------------------------------------------------------------
//                           HarmonicGravityFactory
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
 * This is the class that creates and returns a pointer to the
 * appropriate HarmonicGravity sub-class.
 * NOTE: this class currently can only be used in the GravityField class.
 */
//------------------------------------------------------------------------------
#ifndef HarmonicGravityFactory_hpp
#define HarmonicGravityFactory_hpp

#include <vector>
#include "HarmonicGravity.hpp"
#include "GravityFile.hpp"

class CelestialBody; // forward reference

class HarmonicGravityFactory 
{
public:
   HarmonicGravityFactory();
   virtual ~HarmonicGravityFactory();

   static HarmonicGravity* Create(const std::string& filename,
                                  const Real& radius, const Real& mukm);
   static HarmonicGravity* Create(CelestialBody* body);

private:
   static std::vector<HarmonicGravity*> cache;
   static GravityFile *gravFile;

   HarmonicGravityFactory(const HarmonicGravityFactory& x);
   HarmonicGravityFactory& operator=(const HarmonicGravityFactory& x);

   static void             EmptyCache();
};

#endif // HarmonicGravityFactory_hpp
