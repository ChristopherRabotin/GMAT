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
 * @note Currently, this code assumes that it can set the parameter default
 *       values, based on the input name of the planet (e.g., if the planet's
 *       name is Mercury, the default values for Mercury will be used)
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


protected:

   enum
   {
      MERCURY = 0,
      VENUS,
      EARTH,
      MARS,
      JUPITER,
      SATURN,
      URANUS,
      NEPTUNE,
      PLUTO,
      NumberOfPlanets
      // add Sedna later??
   };

   // default values for CelestialBody data
   static const Gmat::BodyType        DEFAULT_BODY_TYPE;
   static const Gmat::PosVelSource    DEFAULT_POS_VEL_SOURCE;
   static const Gmat::AnalyticMethod  DEFAULT_ANALYTIC_METHOD;
   static const Integer               DEFAULT_BODY_NUMBER;
   static const Integer               DEFAULT_REF_BODY_NUMBER;

   static const Real                  MASS[NumberOfPlanets];
   static const Real                  EQUATORIAL_RADIUS[NumberOfPlanets];
   static const Real                  POLAR_RADIUS[NumberOfPlanets];
   static const Real                  MU[NumberOfPlanets];
   static const Integer               ORDER[NumberOfPlanets];
   static const Integer               DEGREE[NumberOfPlanets];
   static const Integer               COEFFICIENT_SIZE[NumberOfPlanets];
   static const Rmatrix               SIJ[NumberOfPlanets];
   static const Rmatrix               CIJ[NumberOfPlanets];
   // add other ones as needed

   
   // what other planet-specific parameters do I need?

   void InitializePlanet(CelestialBody* cBody);

private:

};
#endif // Planet_hpp

