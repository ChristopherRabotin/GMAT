//$Header$
//------------------------------------------------------------------------------
//                                  Planet
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
 * This is the class for planets.
 *
 */
//------------------------------------------------------------------------------


#ifndef Planet_hpp
#define Planet_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Rmatrix.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"

/**
 * Planet class, for all planets in the solar system : ).
 *
 * The Planet class will contain all data and methods for any planet that exists in
 * the solar system.
 */
class GMAT_API Planet : public CelestialBody
{
public:
   // default constructor, with optional name
   Planet(std::string name = SolarSystem::EARTH_NAME);
   // additional constructor
   Planet(std::string name, CelestialBody* cBody);
   // copy constructor
   Planet(const Planet &pl);
   // operator=
   Planet& operator=(const Planet &pl);
   // destructor
   virtual ~Planet();


   //------------------------------------------------------------------------------
   // virtual Planet* Clone(void) const
   //------------------------------------------------------------------------------
   /**
     * Method used to create a copy of the Planet object
     */
   //------------------------------------------------------------------------------
   Planet* Clone(void) const;

   // default values for CelesitalBody data
   static const Gmat::BodyType        BODY_TYPE;
   static const Real                  MASS;
   static const Real                  EQUATORIAL_RADIUS;
   static const Real                  POLAR_RADIUS;
   static const Real                  MU;
   static const Gmat::PosVelSource    POS_VEL_SOURCE;
   static const Gmat::AnalyticMethod  ANALYTIC_METHOD;
   static const Integer               BODY_NUMBER;
   static const Integer               REF_BODY_NUMBER;
   static const Integer               ORDER;
   static const Integer               DEGREE;
   static const Integer               COEFFICIENT_SIZE;
   static const Rmatrix               SIJ;
   static const Rmatrix               CIJ;
   // add other ones as needed

protected:

   // what other star-specifi parameters do I need?

   void InitializePlanet(CelestialBody* cBody);

private:

};
#endif // Planet_hpp

