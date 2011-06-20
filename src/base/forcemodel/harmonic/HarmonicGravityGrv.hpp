//$Id$
//------------------------------------------------------------------------------
//                           HarmonicGravityGrv
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: John P. Downing/GSFC/595
// Created: 2010.10.28
// (modified for style, etc. by Wendy Shoan/GSFC/583 2011.06.04)
//
/**
 * This is the class that loads data from a GRV type gravity file.
 */
//------------------------------------------------------------------------------
#ifndef HarmonicGravityGrv_hpp
#define HarmonicGravityGrv_hpp

#include "HarmonicGravity.hpp"

class HarmonicGravityGrv : public HarmonicGravity
{
public:
   HarmonicGravityGrv(const std::string& filename, const Real& radius, const Real& mukm);
   virtual ~HarmonicGravityGrv();

protected:
   void Load();

private:  // No copying allowed, so these are declared private, but not implemented
   HarmonicGravityGrv(const HarmonicGravityGrv& x);
   HarmonicGravityGrv& operator=(const HarmonicGravityGrv& x);

};

#endif  // HarmonicGravityGrv_hpp
