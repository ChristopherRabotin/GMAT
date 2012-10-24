//$Id:$
//------------------------------------------------------------------------------
//                            SpecialCelestialPoint
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Wendy C. Shoan
// Created: 2011.07.15
//
/**
 * The SpecialCelestialPoint class will contain all data and methods for a solar system
 * point that can be handled similarly to a Celestialbody, e.g. for a point whose
 * state can be obtained from a planetary ephemeris file.  An example would be the
 * Solar System Barycenter, whose data is contained on DE and SPK planetary ephemeris
 * files, along with state data for celestial bodies.
 *
 * This class must override several of the CelestialBody methods.
 *
 */
//------------------------------------------------------------------------------

#ifndef SpecialCelestialPoint_hpp
#define SpecialCelestialPoint_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Rmatrix.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"

class GMAT_API SpecialCelestialPoint : public CelestialBody
{
public:
   // default constructor, with optional name
   SpecialCelestialPoint(std::string name = "");
   // copy constructor
   SpecialCelestialPoint(const SpecialCelestialPoint &cp);
   // operator=
   SpecialCelestialPoint& operator=(const SpecialCelestialPoint &cp);
   // destructor
   virtual ~SpecialCelestialPoint();

   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   // required method for all subclasses that can be copied in a script
   virtual void      Copy(const GmatBase* orig);
   virtual bool      NeedsOnlyMainSPK();

   virtual bool      IsParameterReadOnly(const Integer id) const;
   virtual bool      SetSource(Gmat::PosVelSource pvSrc);
   virtual bool      SetAllowSpice(const bool allow);
   virtual Rvector6  ComputeTwoBody(const A1Mjd &forTime);

protected:
   enum
   {
      SpecialCelestialPointParamCount = CelestialBodyParamCount,

   };

private:

};
#endif // SpecialCelestialPoint_hpp
