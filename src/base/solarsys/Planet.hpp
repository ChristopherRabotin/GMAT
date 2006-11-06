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
 * @note When the Planet is the Earth, the initial epoch and Keplerian elements
 *       used for the Low Fidelity modeling are set, they must be 'forwarded'
 *       on to the Sun.  This is necessary because:
 *       1. the KeplersProbelm code computes the state of a body with respect to
 *          its central body, then returms it with respect to the Earth
 *       2. the Sun has no cental body, so
 *       3. the Sun will have to use the Earth's information to compute and
 *          return its state
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
 * The Planet class will contain all data and methods for any planet that exists 
 * in the solar system.
 */
class GMAT_API Planet : public CelestialBody
{
public:
   // default constructor, with optional name
   Planet(std::string name = SolarSystem::EARTH_NAME);
   // additional constructor
   Planet(std::string name, const std::string &cBody);
   // copy constructor
   Planet(const Planet &pl);
   // operator=
   Planet& operator=(const Planet &pl);
   // destructor
   virtual ~Planet();

   virtual Rvector       GetBodyCartographicCoordinates(
                            const A1Mjd &forTime) const;
   virtual Real          GetHourAngle(A1Mjd atTime); 
   
   // For the Earth, these methods need to update the Sun's information, as the 
   // Sun has no cental body and its initial epoch and keplerian elements need 
   // to be set and stored differently
   virtual bool          SetAnalyticEpoch(const A1Mjd &toTime);
   virtual bool          SetAnalyticElements(const Rvector6 &kepl);
   
   virtual Real          GetUpdateInterval() const;
   virtual bool          SetUpdateInterval(Real val);

   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

   // Parameter access methods - overridden from GmatBase - may need these later??
   virtual std::string     GetParameterText(const Integer id) const;     
   virtual Integer         GetParameterID(const std::string &str) const; 
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;
   virtual bool            IsParameterReadOnly(const Integer id) const;
   virtual Real            GetRealParameter(const Integer id) const;
   virtual Real            SetRealParameter(const Integer id,
                                            const Real value);
   virtual Real            GetRealParameter(const std::string &label) const;
   virtual Real            SetRealParameter(const std::string &label,
                                            const Real value);
                                            
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
      /// @todo  add Sedna later??
   };

   enum
   {
      UPDATE_INTERVAL = CelestialBodyParamCount,
      PlanetParamCount
   };
   
   static const std::string PARAMETER_TEXT[PlanetParamCount - CelestialBodyParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[PlanetParamCount - CelestialBodyParamCount];
   
   // default values for CelestialBody data
   static const Gmat::BodyType        DEFAULT_BODY_TYPE;
   static const Gmat::PosVelSource    DEFAULT_POS_VEL_SOURCE;
   static const Gmat::AnalyticMethod  DEFAULT_ANALYTIC_METHOD;
   static const Integer               DEFAULT_BODY_NUMBER;
   static const Integer               DEFAULT_REF_BODY_NUMBER;

   static const Real                  EQUATORIAL_RADIUS[NumberOfPlanets];
   static const Real                  FLATTENING[NumberOfPlanets];
   static const Real                  MU[NumberOfPlanets];
   static const Integer               ORDER[NumberOfPlanets];
   static const Integer               DEGREE[NumberOfPlanets];
   static const Rmatrix               SIJ[NumberOfPlanets];
   static const Rmatrix               CIJ[NumberOfPlanets];
   
   static const Real                  ANALYTIC_EPOCH[NumberOfPlanets];
   static const Rvector6              ANALYTIC_ELEMENTS[NumberOfPlanets];
                                            
   Real                               updateInterval;
   /// @todo add other ones as needed

   void             InitializePlanet(const std::string &cBody);
                                          

private:

};
#endif // Planet_hpp

