//$Header$ 
//------------------------------------------------------------------------------
//                             SphericalTwo
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus 
// Created: 2003/12/16 Joey Gurganus
//
/**
 * Implements the SphericalTwo class for spherical elements including
 * Right Ascension of Velocity and Declination of Velocity.
 *
 * @note:   This code is revived from the original argosy and swingby 
 */
//------------------------------------------------------------------------------

#include "SphericalTwo.hpp"
#include "UtilityException.hpp"

using namespace GmatMathUtil;

//---------------------------------
//  static data
//---------------------------------
const std::string SphericalTwo::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Radial Magnitude",
   "Right Ascension",
   "Declination",
   "Velocity Magnitude",
   "Right Ascension of Velocity",
   "Declination of Velocity"
};

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  SphericalTwo::SphericalTwo() 
//------------------------------------------------------------------------------
/**
 * Constructs base Spherical structures 
 */
SphericalTwo::SphericalTwo() :
   Spherical       (),
   raVelocity      (0.0),
   decVelocity     (0.0)
{
}

//------------------------------------------------------------------------------
//  SphericalTwo::SphericalTwo(Real rMag,  Real ra, Real dec, Real vMag, 
//                             Real vRA, Real vDec)
//------------------------------------------------------------------------------
SphericalTwo::SphericalTwo(Real rMag,  Real ra, Real dec, Real vMag,
                           Real vRA, Real vDec) :
   Spherical        (rMag, ra, dec, vMag),
   raVelocity       (vRA),
   decVelocity      (vDec)
{
}

//------------------------------------------------------------------------------
//   SphericalTwo::SphericalTwo(const SphericalTwo &spherical)
//------------------------------------------------------------------------------
SphericalTwo::SphericalTwo(const SphericalTwo &spherical) :
   Spherical        (spherical.positionMagnitude, spherical.rightAscension,
                     spherical.declination, spherical.velocityMagnitude),
   raVelocity       (spherical.raVelocity),
   decVelocity      (spherical.decVelocity)
{
}

//------------------------------------------------------------------------------
//  SphericalTwo& SphericalTwo::operator=(const SphericalTwo &spherical)
//------------------------------------------------------------------------------
SphericalTwo& SphericalTwo::operator=(const SphericalTwo &spherical)
{
   if (this != &spherical)
   {
      SetPositionMagnitude( spherical.GetPositionMagnitude() );
      SetRightAscension( spherical.GetRightAscension() );
      SetDeclination( spherical.GetDeclination() );
      SetVelocityMagnitude( spherical.GetVelocityMagnitude() );
      SetVelocityRA( spherical.GetVelocityRA() );
      SetVelocityDeclination( spherical.GetVelocityDeclination() );
   }
   return *this;
}

//------------------------------------------------------------------------------
//  SphericalTwo::~SphericalTwo() 
//------------------------------------------------------------------------------
SphericalTwo::~SphericalTwo() 
{
}

//------------------------------------------------------------------------------
// Real SphericalTwo::GetVelocityRA() const
//------------------------------------------------------------------------------
Real SphericalTwo::GetVelocityRA() const
{
   return raVelocity;
}

//------------------------------------------------------------------------------
// void SphericalTwo::SetVelocityRA(const Real vRA) 
//------------------------------------------------------------------------------
void SphericalTwo::SetVelocityRA(const Real vRA)
{
   raVelocity = vRA;
}

//------------------------------------------------------------------------------
// Real SphericalTwo::GetVelocityDeclination() const
//------------------------------------------------------------------------------
Real SphericalTwo::GetVelocityDeclination() const
{
   return decVelocity;
}

//------------------------------------------------------------------------------
// void SphericalTwo::SetVelocityDeclination(const Real vDec) 
//------------------------------------------------------------------------------
void SphericalTwo::SetVelocityDeclination(const Real vDec)
{
   decVelocity = vDec;
}

//------------------------------------------------------------------------------
// bool SphericalTwo::ToSphericalTwo(const Cartesian &cartesian)
//------------------------------------------------------------------------------
bool SphericalTwo::ToSphericalTwo(const Cartesian &cartesian)
{
   if (!Spherical::ToSpherical(cartesian,false))
   {
      return false;
   }

   // Get velocity vectors from Cartesian
   Real  vX = cartesian.GetVelocity(0);
   Real  vY = cartesian.GetVelocity(1);
   Real  vZ = cartesian.GetVelocity(2);

   // Calculate right ascension of velocity which is measured east of vernal
   // equinox using atan2() which returns an angle between 0.0 and TWO_PI 
   SetVelocityRA( 
                 Spherical::GetDegree(GmatMathUtil::ATan(vY,vX), 0.0, 
                                      GmatMathUtil::TWO_PI ));
  
   // Calculate declination of Velocity which is measured north from the 
   // equator using atan2() which will return an angle between -PI/2..PI/2 
   // radians since the second argument will be greater than or equal to zero.
   SetVelocityDeclination(GmatMathUtil::Deg
                          (GmatMathUtil::ATan(vZ, 
                                              GmatMathUtil::Sqrt(vX*vX + vY*vY))) );
   
   return true;
}

//------------------------------------------------------------------------------
// Cartesian SphericalTwo::GetCartesian()
//------------------------------------------------------------------------------
Cartesian SphericalTwo::GetCartesian()
{
   // Check if input of radial magnitude is not valid then return false
   if (GmatMathUtil::Abs(GetPositionMagnitude()) <= ORBIT_TOLERANCE)
      return Cartesian(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    // Get the position after converting to part of cartesian 
   Rvector3 position = GetPosition();

   // Convert right ascension of velocity and declination of velocity from
   // units of degrees to units of radians
   Real raV  = GmatMathUtil::Rad( GetVelocityRA() );
   Real decV = GmatMathUtil::Rad( GetVelocityDeclination() );
    
   // start to calculate the conversion to cartesian's velocity
   Real vX = GetVelocityMagnitude() * GmatMathUtil::Cos(decV) * 
      GmatMathUtil::Cos(raV);

   Real vY = vX * GmatMathUtil::Tan(raV);

   Real vZ = GetVelocityMagnitude() * GmatMathUtil::Sin(decV);

    // Set up the Cartesian's new velocity
   Rvector3 velocity; 
   velocity.Set(vX,vY,vZ);

    // Return the cartesian conversion
   return Cartesian(position,velocity);
}

//------------------------------------------------------------------------------
// Integer SphericalTwo::GetNumData() const
//------------------------------------------------------------------------------
Integer SphericalTwo::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* SphericalTwo::GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* SphericalTwo::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}

//------------------------------------------------------------------------------
//  std::string* SphericalTwo::ToValueStrings(void)
//------------------------------------------------------------------------------
std::string* SphericalTwo::ToValueStrings(void)
{
   std::stringstream ss("");

   ss << GetPositionMagnitude();
   stringValues[0] = ss.str();
   
   ss.str("");
   ss << GetRightAscension();
   stringValues[1] = ss.str();
   
   ss.str("");
   ss << GetDeclination();
   stringValues[2] = ss.str();
   
   ss.str("");
   ss << GetVelocityMagnitude();
   stringValues[3] = ss.str();

   ss.str("");
   ss << GetVelocityRA();
   stringValues[4] = ss.str();

   ss.str("");
   ss << GetVelocityDeclination();
   stringValues[5] = ss.str();
   
   return stringValues;
}

//------------------------------------------------------------------------------
//  std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return data value string
 */
//------------------------------------------------------------------------------
std::string SphericalTwo::ToString()
{
   std::string s;
   std::string *val;

   val = ToValueStrings();
   
   for (int i=0; i<NUM_DATA; i++)
      s = s + val[i] + " ";
   
   return s;
}

//---------------------------------
// friend functions
//---------------------------------

//------------------------------------------------------------------------------
// friend SphericalTwo ToSphericalTwo(const Cartesian &c); //loj: 4/19/04 added
//------------------------------------------------------------------------------
SphericalTwo ToSphericalTwo(const Cartesian &c)
{
   SphericalTwo sph2;

   if (sph2.ToSphericalTwo(c))
      return sph2;
   else
      throw UtilityException("ToSphericalTwo(): R magnitude is less than orbit tolerance");
   
}

//------------------------------------------------------------------------------
//  friend std::ostream& operator<<(std::ostream& output, SphericalTwo &s)
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& output, SphericalTwo &s)
{
   Rvector v(6, s.positionMagnitude, s.rightAscension, s.declination,
             s.velocityMagnitude, s.raVelocity, s.decVelocity);

   output << v << std::endl;

   return output;
}

//------------------------------------------------------------------------------
//  friend std::istream& operator>>(std::istream& input, SphericalTwo &s)
//------------------------------------------------------------------------------
std::istream& operator>>(std::istream& input, SphericalTwo &s )
{
   input >> s.positionMagnitude >> s.rightAscension
         >> s.declination >> s.velocityMagnitude
         >> s.raVelocity >> s.decVelocity;

   return input;
}

