//$Header$
//------------------------------------------------------------------------------
//                              MeanJ2000Equatorial
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/15
//
/**
 * Defines Mean of Reference J2000 Equator frame.
 */
//------------------------------------------------------------------------------
#include "MeanJ2000Equatorial.hpp"
#include "CelestialBody.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// MeanJ2000Equatorial(const CelestialBody &centralBody)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <centralBody> central body
 * @param <refDate> reference date
 */
//------------------------------------------------------------------------------
MeanJ2000Equatorial::MeanJ2000Equatorial(const CelestialBody &centralBody)
   : RefFrame(centralBody, A1Mjd::J2000, "MeanJ2000Equatorial")
{
}

//------------------------------------------------------------------------------
// MeanJ2000Equatorial(const MeanJ2000Equatorial &mj2000eq)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
MeanJ2000Equatorial::MeanJ2000Equatorial(const MeanJ2000Equatorial &mj2000eq)
   : RefFrame(mj2000eq)
{
}

//------------------------------------------------------------------------------
// MeanJ2000Equatorial& operator= (const MeanJ2000Equatorial &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 */
//------------------------------------------------------------------------------
MeanJ2000Equatorial& MeanJ2000Equatorial::operator= (const MeanJ2000Equatorial &right)
{
   if (this != &right)
   {
      RefFrame::operator=(right);
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~MeanJ2000Equatorial()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanJ2000Equatorial::~MeanJ2000Equatorial()
{
}
