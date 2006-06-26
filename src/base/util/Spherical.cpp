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
 * Position Magnitude, Right Ascension, Declination, and Velocity Magnitude.
 *
 * @note:   This code is revived from the original argosy and swingby 
 */
//------------------------------------------------------------------------------

#include "Spherical.hpp"

//---------------------------------
//  static data
//---------------------------------
const Real Spherical::ORBIT_TOLERANCE = 1.0E-10;

const std::string Spherical::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Position Magnitude",
   "Right Ascension",
   "Declination",
   "Velocity Magnitude"
};


Real Spherical::GetVerticalFlightPathAngle(const Rvector3& pos, const Rvector3& vel)
{
	Real fpa = GmatMathUtil::ACos( (pos*vel)/(pos.GetMagnitude()*vel.GetMagnitude()) );
	return fpa;
}

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
    positionMagnitude  (0.0),
    rightAscension    (0.0),
    declination       (0.0),
    velocityMagnitude (0.0)
{
}

//------------------------------------------------------------------------------
//  Spherical::Spherical(Real rMag,  Real ra, Real dec, Real vMag)
//------------------------------------------------------------------------------
Spherical::Spherical(Real rMag,  Real ra, Real dec, Real vMag) :
    positionMagnitude  (rMag),
    rightAscension    (ra),
    declination       (dec),
    velocityMagnitude (vMag)
{
}

//------------------------------------------------------------------------------
//   Spherical::Spherical(const Spherical &spherical)
//------------------------------------------------------------------------------
Spherical::Spherical(const Spherical &spherical) :
    positionMagnitude  (spherical.positionMagnitude),
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
      SetPositionMagnitude( spherical.GetPositionMagnitude() );
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
    Rvector v(4, s.positionMagnitude, s.rightAscension, s.declination,
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
    input >> s.positionMagnitude >> s.rightAscension 
          >> s.declination >> s.velocityMagnitude;

    return input;
}

//------------------------------------------------------------------------------
// Real Spherical::GetPositionMagnitude() const
//------------------------------------------------------------------------------
Real Spherical::GetPositionMagnitude() const
{
        return positionMagnitude;
}

//------------------------------------------------------------------------------
// void Spherical::SetPositionMagnitude(const Real rMag) 
//------------------------------------------------------------------------------
void Spherical::SetPositionMagnitude(const Real rMag)
{
        positionMagnitude = rMag;
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
// bool Spherical::CartesianToSpherical(const Rvector6& cartesian, 
//                                      const bool isAZFPA)
//------------------------------------------------------------------------------
bool Spherical::CartesianToSpherical(const Rvector6& cartesian, 
                                     const bool isAZFPA)
{
    Rvector3 position = cartesian.GetR();
    Rvector3 velocity = cartesian.GetV();

    // Get position and velocity vector magnitudes
    Real r_mag = position.GetMagnitude();

    // Check if input of position magnitude is not valid then return false
    //if (fabs(r_mag) <= ORBIT_TOLERANCE)
    //loj: 4/1/05 Changed to use GmatMathUtil::Abs()
    if (GmatMathUtil::Abs(r_mag) <= ORBIT_TOLERANCE)
    {
       return false;
    }

    // Set the position magnitude
    SetPositionMagnitude(r_mag); 

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

    if (isAZFPA)
       mState = GetDegree(mState,0.-GmatMathUtil::PI,GmatMathUtil::PI);
    else
       mState = GmatMathUtil::Deg(mState);

    SetDeclination(mState); 

    // Get and set velocity vector magnitude
    SetVelocityMagnitude(velocity.GetMagnitude()); 

    return true;
}

//------------------------------------------------------------------------------
// Rvector Spherical::GetPosition()
//------------------------------------------------------------------------------
Rvector3 Spherical::GetPosition()
{
    
    // Check if input of position magnitude is not valid then return false
    //if (fabs(GetPositionMagnitude()) <= ORBIT_TOLERANCE)
    //loj: Changed to use GmatMathUtil::Abs()
    if (GmatMathUtil::Abs(GetPositionMagnitude()) <= ORBIT_TOLERANCE)
    {
       return Rvector3(0,0,0);
    }

    // Convert right ascension and declination from degrees to radians
    Real ra  = GmatMathUtil::Rad( GetRightAscension() ); 
    Real dec = GmatMathUtil::Rad( GetDeclination() );

    // Calculate X, Y, and Z 
    Real x = GetPositionMagnitude() * 
               GmatMathUtil::Cos(dec) * GmatMathUtil::Cos(ra);
    Real y = x * GmatMathUtil::Tan(ra);
    Real z = GetPositionMagnitude() * GmatMathUtil::Sin(dec);
 
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
   //loj: 4/1/05 Changed to use GmatMathUtil::Mod()
   //Real angleInRange = fmod(angle,GmatMathUtil::TWO_PI);
   Real angleInRange = GmatMathUtil::Mod(angle,GmatMathUtil::TWO_PI);
   
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
   
   return stringValues;
}

