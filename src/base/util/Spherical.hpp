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
// Arthor:  Joey Gurganus 
// Created: 2003/12/04 
//
/**
 * Definition for the Spherical Base class to support the spherical elements
 * containing Position Magnitude, Right Ascension, Declination, and Velocity
 * Magnitude.
 *
 * @note:   This code is revived from the original argosy and swingby
 */
//------------------------------------------------------------------------------
#ifndef Spherical_hpp
#define Spherical_hpp

#include <iostream>
#include <sstream>
#include "gmatdefs.hpp"
#include "Linear.hpp"
#include "RealUtilities.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"

class Spherical 
{
public:
    Spherical();
    Spherical(Real rMag, Real ra, Real dec, Real vMag); 
    Spherical(const Spherical &spherical);
    Spherical& operator=(const Spherical &spherical);
    virtual ~Spherical();

    // Friend function
    friend std::ostream& operator<<(std::ostream& output, Spherical& s);
    friend std::istream& operator>>(std::istream& input, Spherical& s);

    // public method 
    Real GetPositionMagnitude() const;
    void SetPositionMagnitude(const Real rMag);

    Real GetRightAscension() const;
    void SetRightAscension(const Real ra);

    Real GetDeclination() const;
    void SetDeclination(const Real dec);

    Real GetVelocityMagnitude() const;
    void SetVelocityMagnitude(const Real vMag);

	// should this be a member function?
	Real GetVerticalFlightPathAngle(const Rvector3& pos, const Rvector3& vel);

    Integer GetNumData() const;
    const std::string* GetDataDescriptions() const;
    std::string* ToValueStrings();
 

protected:
    // Constant value for orbit tolerance
    static const Real ORBIT_TOLERANCE;// = 1.0E-10;

    Real     positionMagnitude;    //  Position vector magnitude
    Real     rightAscension;       //  RA measured of vernal equinox 
    Real     declination;          //  Declination measured north from equator
    Real     velocityMagnitude;    //  Velocity vector magnitude

    // protected method
    bool CartesianToSpherical(const Rvector6& cartVector, 
                              const bool isAZFPA);
                     
    Rvector3 GetPosition();
    Real GetDegree(const Real angle, const Real minAngle, const Real maxAngle);
 
private:
   static const Integer NUM_DATA = 4;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
   std::string stringValues[NUM_DATA];

};
#endif // Spherical_hpp
