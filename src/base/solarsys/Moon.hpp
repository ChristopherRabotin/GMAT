//$Header$
//------------------------------------------------------------------------------
//                                  Moon
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy C. Shoan
// Created: 2004/01/29
//
/**
 * This is the class for moons.
 *
 */
//------------------------------------------------------------------------------


#ifndef Moon_hpp
#define Moon_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Rmatrix.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"

/**
 * Moon class, for all moons in the solar system : ).
 *
 * The Moon class will contain all data and methods for any moon that exists in
 * the solar system.
 */
class GMAT_API Moon : public CelestialBody
{
public:
   // default constructor, with optional name
   Moon(std::string name = SolarSystem::MOON_NAME);
   // additional constructor
   Moon(std::string name, const std::string &cBody);
   // copy constructor
   Moon(const Moon &m);
   // operator=
   Moon& operator=(const Moon &m);
   // destructor
   virtual ~Moon();


   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

   // default values for CelesitalBody data
   static const Gmat::BodyType        BODY_TYPE;
   //static const Real                  MASS;
   static const Real                  EQUATORIAL_RADIUS;
   static const Real                  FLATTENING;
   //static const Real                  POLAR_RADIUS;
   static const Real                  MU;
   static const Gmat::PosVelSource    POS_VEL_SOURCE;
   static const Gmat::AnalyticMethod  ANALYTIC_METHOD;
   static const CelestialBody*        CENTRAL_BODY;
   static const Integer               BODY_NUMBER;
   static const Integer               REF_BODY_NUMBER;
   static const Integer               ORDER;
   static const Integer               DEGREE;
   static const Rmatrix               SIJ;
   static const Rmatrix               CIJ;
   //static const Integer               COEFFICIENT_SIZE;

protected:

   // what other star-specifi parameters do I need?

   void InitializeMoon(const std::string &cBody);

private:

};
#endif // Moon_hpp

