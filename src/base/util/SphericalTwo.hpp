//$Header$
//------------------------------------------------------------------------------
//                           SphericalTwo
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Arthor:  Joey Gurganus 
// Created: 2004/01/08 
//
/**
 * Definition for the SphericalTwo class used for spherical elements including
 * Right Ascension of Velocity and Declination of Velocity.
 *
 * @note:   This code is revived from the original argosy and swingby
 */
//------------------------------------------------------------------------------
#ifndef SphericalTwo_hpp
#define SphericalTwo_hpp

#include <iostream>
#include "gmatdefs.hpp"
#include "Linear.hpp"
#include "PhysicalConstants.hpp"    // for mu
#include "Rvector3.hpp"
#include "Spherical.hpp"

// Forward declaration
class Cartesian;       
class Keplerian;       

class SphericalTwo : public Spherical
{
public:
    SphericalTwo();
    SphericalTwo(Real rMag, Real ra, Real dec, Real vmag, Real vRA, Real vDec);
    SphericalTwo(const SphericalTwo &spherical);
    SphericalTwo& operator=(const SphericalTwo &spherical);
    virtual ~SphericalTwo();

    //  Friend functions
    friend std::ostream& operator<<(std::ostream& output, SphericalTwo& s);
    friend std::istream& operator>>(std::istream& input, SphericalTwo& s);

    // public methods
    Real GetVelocityRA() const;
    void SetVelocityRA(const Real vRA);

    Real GetVelocityDeclination() const;
    void SetVelocityDeclination(const Real vDeclincation);

    bool ToSphericalTwo(const Cartesian &c);
    Cartesian GetCartesian();

    Integer GetNumData() const;
    const std::string* GetDataDescriptions() const;
    std::string* ToValueStrings();

protected:

private:
    Real     raVelocity;            //  Right Ascension of Velocity 
    Real     decVelocity;           //  Declination of Velocity 

   static const Integer NUM_DATA = 6;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
   std::string stringValues[NUM_DATA];

};
#endif // SphericalTwo_hpp
