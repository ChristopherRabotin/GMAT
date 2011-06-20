//$Id$
//------------------------------------------------------------------------------
//                           HarmonicGravityCof
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
 * This is the class that loads data from a COF type gravity file.
 */
//------------------------------------------------------------------------------
#ifndef HarmonicGravityCof_hpp
#define HarmonicGravityCof_hpp

#include "gmatdefs.hpp"
#include "HarmonicGravity.hpp"

class HarmonicGravityCof : public HarmonicGravity
{
public:
   HarmonicGravityCof(const std::string& filename,
                      const Real& radius, const Real& mukm);
   virtual ~HarmonicGravityCof();

protected:
   void Load ();

private:  // No copying allowed, so thes are declared as private but not implemented
   HarmonicGravityCof(const HarmonicGravityCof& x);
   HarmonicGravityCof& operator=(const HarmonicGravityCof& x);
};

#endif // HarmonicGravityCof_hpp
