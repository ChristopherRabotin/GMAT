//$Header$ 
//------------------------------------------------------------------------------
//                             Spherical
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
 * Implements Spherical Base class to contains the spherical elements including
 * Radical Magnitude, Right Ascension, Declination, and Velocity Magnitude.
 *
 * @note:   This code is revived from the original argosy and swingby 
 */
//------------------------------------------------------------------------------

#include "Spherical.hpp"

using namespace GmatRealUtil;

//---------------------------------
//  static data
//---------------------------------
const std::string Spherical::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Radical Magnitude",
   "Right Ascension",
   "Declination",
   "Velocity Magnitude"
};

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Spherical::Spherical() 
//------------------------------------------------------------------------------
/**
 * Constructs base Spherical structures 
 */
Spherical::Spherical() :
    radicalMagnitude  (0.0),
    rightAscension    (0.0),
    declination       (0.0),
    velocityMagnitude (0.0)
{
}

//------------------------------------------------------------------------------
//  Spherical::Spherical(Real rMag,  Real ra, Real dec, Real vMag)
//------------------------------------------------------------------------------
Spherical::Spherical(Real rMag,  Real ra, Real dec, Real vMag) :
    radicalMagnitude  (rMag),
    rightAscension    (ra),
    declination       (dec),
    velocityMagnitude (vMag)
{
}

//------------------------------------------------------------------------------
//   Spherical::Spherical(const Spherical &spherical)
//------------------------------------------------------------------------------
Spherical::Spherical(const Spherical &spherical) :
    radicalMagnitude  (spherical.radicalMagnitude),
    rightAscension    (spherical.rightAscension),
    declination       (spherical.declination),
    velocityMagnitude (spherical.velocityMagnitude)
{
}

//------------------------------------------------------------------------------
//  Spherical& Spherical::operator=(const Spherical &spherical)
//------------------------------------------------------------------------------
Spherical& Spherical::operator=(const Spherical &spherical)
{
   if (this != &spherical)
   {
      SetRadicalMagnitude( spherical.GetRadicalMagnitude() );
      SetRightAscension( spherical.GetRightAscension() );
      SetDeclination( spherical.GetDeclination() );
      SetVelocityMagnitude( spherical.GetVelocityMagnitude() );
   }
   return *this;
}

//------------------------------------------------------------------------------
//  Spherical::~Spherical() 
//------------------------------------------------------------------------------
Spherical::~Spherical() 
{
}

//------------------------------------------------------------------------------
//  std::ostream& operator<<(std::ostream& output, Spherical &s)
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& output, Spherical& s)
{
    Rvector v(4, s.radicalMagnitude, s.rightAscension, s.declination,
              s.velocityMagnitude);

    output << v << std::endl;

    return output;
}

//------------------------------------------------------------------------------
//  <friend>
//  std::istream& operator>>(std::istream& input, Spherical& s)
//------------------------------------------------------------------------------
std::istream& operator>>(std::istream& input, Spherical& s)
{
    input >> s.radicalMagnitude >> s.rightAscension 
          >> s.declination >> s.velocityMagnitude;

    return input;
}

//------------------------------------------------------------------------------
// Real Spherical::GetRadicalMagnitude() const
//------------------------------------------------------------------------------
Real Spherical::GetRadicalMagnitude() const
{
	return radicalMagnitude;
}

//------------------------------------------------------------------------------
// void Spherical::SetRadicalMagnitude(const Real rMag) 
//------------------------------------------------------------------------------
void Spherical::SetRadicalMagnitude(const Real rMag)
{
	radicalMagnitude = rMag;
}

//------------------------------------------------------------------------------
// Real Spherical::GetRightAscension() const
//------------------------------------------------------------------------------
Real Spherical::GetRightAscension() const
{
	return rightAscension;
}

//------------------------------------------------------------------------------
// void Spherical::SetRightAscension(const Real ra) 
//------------------------------------------------------------------------------
void Spherical::SetRightAscension(const Real ra) 
{
	rightAscension = ra;
}

//------------------------------------------------------------------------------
// Real Spherical::GetDeclination() const
//------------------------------------------------------------------------------
Real Spherical::GetDeclination() const
{
	return declination;
}

//------------------------------------------------------------------------------
// void Spherical::SetDeclination(const Real dec) 
//------------------------------------------------------------------------------
void Spherical::SetDeclination(const Real dec) 
{
	declination = dec;
}

//------------------------------------------------------------------------------
// Real Spherical::GetVelocityMagnitude() const
//------------------------------------------------------------------------------
Real Spherical::GetVelocityMagnitude() const
{
	return velocityMagnitude;
}

//------------------------------------------------------------------------------
// void Spherical::SetVelocityMagnitude(const Real vMag) 
//------------------------------------------------------------------------------
void Spherical::SetVelocityMagnitude(const Real vMag) 
{
	velocityMagnitude = vMag;
}

//------------------------------------------------------------------------------
// bool Spherical::ToSpherical(const Cartesian &cartesian, const bool isPartOne)
//------------------------------------------------------------------------------
bool Spherical::ToSpherical(const Cartesian &cartesian, const bool isPartOne)
{
    Rvector3 position = cartesian.GetPosition();
    Rvector3 velocity = cartesian.GetVelocity();

    // Get radical and velocity vector magnitudes
    Real r_mag = position.GetMagnitude();

    // Check if input of radical magnitude is not valid then return false
    if (fabs(r_mag) <= ORBIT_TOLERANCE)
    {
       return false;
    }

    // Set the radical magnitude
    SetRadicalMagnitude(r_mag); 

    // Get position (X, Y, Z)
    Real posX = position.Get(0); 
    Real posY = position.Get(1);
    Real posZ = position.Get(2);
  
    // Get right ascension measured east of vernal equinox 
    Real mState = GmatMathUtil::ATan(posY,posX); 
    mState = GetDegree(mState,0.0,GmatMathUtil::TWO_PI); 
    SetRightAscension(mState); 

    // Get declination measured north from the equator
    mState = GmatMathUtil::ATan(posZ, 
                                GmatMathUtil::Sqrt(posX*posX + posY*posY));

    if (isPartOne)
       mState = GetDegree(mState,0.-GmatMathUtil::PI,GmatMathUtil::PI);
    else
       mState = GmatMathUtil::Deg(mState);

    SetDeclination(mState); 

    // Get and set velocity vector magnitude
    SetVelocityMagnitude(cartesian.GetVelocity().GetMagnitude()); 

    return true;
}

//------------------------------------------------------------------------------
// Rvector Spherical::GetPosition()
//------------------------------------------------------------------------------
Rvector3 Spherical::GetPosition()
{
    
    // Check if input of radical magnitude is not valid then return false
    if (fabs(GetRadicalMagnitude()) <= ORBIT_TOLERANCE)
    {
       return Rvector3(0,0,0);
    }

    // Convert right ascension and declination from degrees to radians
    Real ra  = GmatMathUtil::Rad( GetRightAscension() ); 
    Real dec = GmatMathUtil::Rad( GetDeclination() );

    // Calculate X, Y, and Z 
    Real x = GetRadicalMagnitude() * 
               GmatMathUtil::Cos(dec) * GmatMathUtil::Cos(ra);
    Real y = x * GmatMathUtil::Tan(ra);
    Real z = GetRadicalMagnitude() * GmatMathUtil::Sin(dec);
 
    // Return new position
    return Rvector3(x,y,z);
}

//------------------------------------------------------------------------------
// Real Spherical::GetDegree(const Real angle, const Real minAngle, 
//                           const Real maxAngle) 
//------------------------------------------------------------------------------
Real Spherical::GetDegree(const Real angle, const Real minAngle, 
                          const Real maxAngle) 
{
   Real angleInRange = fmod(angle,GmatMathUtil::TWO_PI);
   
   if (angleInRange < minAngle)
      angleInRange += GmatMathUtil::TWO_PI;

   else if (angleInRange > maxAngle)
      angleInRange -= GmatMathUtil::TWO_PI;

   return GmatMathUtil::Deg(angleInRange);
}


//------------------------------------------------------------------------------
// Integer Spherical::GetNumData() const
//------------------------------------------------------------------------------
Integer Spherical::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* Spherical::GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* Spherical::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}

//------------------------------------------------------------------------------
//  std::string* Spherical::ToValueStrings(void)
//------------------------------------------------------------------------------
std::string* Spherical::ToValueStrings(void)
{
   std::stringstream ss("");

   ss << GetRadicalMagnitude();
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
   
   return stringValues;
}

