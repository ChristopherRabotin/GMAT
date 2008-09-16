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
 * The Moon class will contain all data and methods for any moon that exists in
 * the solar system.
 *
 * @note Currently, only contains default data for Luna, Phobos, and Deimos.
 */
//------------------------------------------------------------------------------


#ifndef Moon_hpp
#define Moon_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Rmatrix.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"

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

   virtual Rvector GetBodyCartographicCoordinates(const A1Mjd &forTime) const;
   virtual Real    GetHourAngle(A1Mjd atTime); 
   
   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;


   // default values for CelestialBody data
   static const Gmat::BodyType        BODY_TYPE;
   static const Gmat::PosVelSource    POS_VEL_SOURCE;
   static const Gmat::AnalyticMethod  ANALYTIC_METHOD;
   static const Integer               ORDER;
   static const Integer               DEGREE;

   static const Integer               LUNA_BODY_NUMBER;
   static const Integer               LUNA_REF_BODY_NUMBER;

   static const Real                  LUNA_EQUATORIAL_RADIUS;
   static const Real                  LUNA_FLATTENING;
   static const Real                  LUNA_MU;
   static const Rmatrix               LUNA_SIJ;
   static const Rmatrix               LUNA_CIJ;
   
   static const Real                  ANALYTIC_EPOCH;
   static const Rvector6              ANALYTIC_ELEMENTS;

protected:
   enum
   {
      MoonParamCount = CelestialBodyParamCount,
      
   };
   
   //static const std::string PARAMETER_TEXT[MoonParamCount - CelestialBodyParamCount];
   
   //static const Gmat::ParameterType 
   //                         PARAMETER_TYPE[MoonParamCount - CelestialBodyParamCount];

   void             InitializeMoon(const std::string &cBody);
   

private:

};
#endif // Moon_hpp

