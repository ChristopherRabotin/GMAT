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
 * This is the base class for planets.
 *
 */
//------------------------------------------------------------------------------


#ifndef Planet_hpp
#define Planet_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"

/**
 * Planet base class, for all planets in the solar system : ).
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

   // method to return the state (position and velocity) of the body at
   // the specified time, using the specified method
   RealArray            GetState(A1Mjd atTime);
   
   // Parameter access methods - overridden from GmatBase - do I need any?
   //std::string    GetParameterText(const Integer id) const;     
   //Integer        GetParameterID(const std::string &str) const; 
   //Gmat::ParameterType GetParameterType(const Integer id) const;
   //std::string    GetParameterTypeString(const Integer id) const;

   //Real           GetRealParameter(const Integer id) const;
   //Real           SetRealParameter(const Integer id,
   //                                        const Real value);
   //Integer        GetIntegerParameter(const Integer id) const; 
   //Integer        SetIntegerParameter(const Integer id,
   //                                           const Integer value); 
   //std::string    GetStringParameter(const Integer id) const; 
   //bool           SetStringParameter(const Integer id, 
   //                                          const std::string &value); 
   //bool           GetBooleanParameter(const Integer id) const; 
   //bool           SetBooleanParameter(const Integer id,
   //                                           const bool value); 

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
   static const CelestialBody*        CENTRAL_BODY;
   static const Integer               BODY_NUMBER;
   static const Integer               REF_BODY_NUMBER;
   // add other ones as needed

protected:

   // what other star-specifi parameters do I need?

   void InitializePlanet(CelestialBody* cBody);

private:

};
#endif // Planet_hpp

