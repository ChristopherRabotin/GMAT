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
 * This is the class for stars.
 *
 */
//------------------------------------------------------------------------------


#ifndef Star_hpp
#define Star_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Rmatrix.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"

/**
 * Star class, for all stars in the solar system : ).
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

   Real GetRadiantPower() const;       // W / m^2
   Real GetReferenceDistance() const;  // km

   bool SetRadiantPower(Real radPower, Real refDistance);

   virtual Rvector       GetBodyCartographicCoordinates(
                            const A1Mjd &forTime) const;
   virtual Real          GetHourAngle(A1Mjd atTime); 

   // overridden access methods from CelestialBody
   virtual std::string    GetParameterText(const Integer id) const;
   virtual Integer        GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
      GetParameterType(const Integer id) const;
   virtual std::string    GetParameterTypeString(const Integer id) const;

   virtual Real           GetRealParameter(const Integer id) const;
   virtual Real           SetRealParameter(const Integer id,
                                           const Real value);
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
   static const Integer               BODY_NUMBER;
   static const Integer               REF_BODY_NUMBER;
   //static const Integer               COEFFICIENT_SIZE;
   static const Integer               ORDER;
   static const Integer               DEGREE;
   static const Rmatrix               SIJ;
   static const Rmatrix               CIJ;
   
   static const Real                  ANALYTIC_EPOCH;
   static const Rvector6              ANALYTIC_ELEMENTS;

   static const Real                  STAR_RADIANT_POWER;       // W / m^2
   static const Real                  STAR_REFERENCE_DISTANCE;  // km
   static const Real                  STAR_PHOTOSPHERE_RADIUS;  // m
   // add other ones as needed

protected:

   enum
   {
      RADIANT_POWER = CelestialBodyParamCount,
      REFERENCE_DISTANCE,
      PHOTOSPHERE_RADIUS,
      StarParamCount
   };


   static const std::string PARAMETER_TEXT[
      StarParamCount - CelestialBodyParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[
      StarParamCount - CelestialBodyParamCount];

   // constants for cartographic coordinates
   static const Real alpha;// = 286.13;      // deg
   static const Real delta;// = 63.87;       // deg
   static const Real w1;//    = 84.10;       // deg
   static const Real w2;//    = 14.1844000;  //

      // radiant power and reference distance
   Real      radiantPower;
   Real      referenceDistance;
   Real      photosphereRadius;  // meters

   void             InitializeStar();
   virtual Rvector6 ComputeLowFidelity(const A1Mjd &forTime);

private:

};
#endif // Star_hpp

