//$Header$
//------------------------------------------------------------------------------
//                                  Star
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
 * This is the base class for stars.
 *
 */
//------------------------------------------------------------------------------


#ifndef Star_hpp
#define Star_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"

/**
* Star base class, for all stars in the solar system : ).
 *
 * The Star class will contain all data and methods for any star that exists in
 * the solar system.
 */
class GMAT_API Star : public CelestialBody
{
public:
   // default constructor, with optional name
   Star(std::string name = SolarSystem::SUN_NAME);
   // copy constructor
   Star(const Star &st);
   // operator=
   Star& operator=(const Star &st);
   // destructor
   virtual ~Star();

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
   // virtual Star* Clone(void) const
   //------------------------------------------------------------------------------
   /**
     * Method used to create a copy of the Star object
     */
   //------------------------------------------------------------------------------
   Star* Clone(void) const;

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

   void InitializeStar();

private:

};
#endif // Star_hpp

