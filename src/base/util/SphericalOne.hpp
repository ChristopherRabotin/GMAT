//$Header$
//------------------------------------------------------------------------------
//                           SphericalOne
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
 * Definition for the SphericalOne class including Azimuth and Flight Path 
 * Angle with first four spherical elements.
 * 
 * @note:   This code is revived from the original argosy and swingby
 */
//------------------------------------------------------------------------------
#ifndef SphericalOne_hpp
#define SphericalOne_hpp

#include "Spherical.hpp"
#include "Cartesian.hpp"

class SphericalOne : public Spherical
{
public:
    SphericalOne();
    SphericalOne(Real rMag, Real ra, Real dec, Real vmag, Real az, Real fPA);
    SphericalOne(const SphericalOne &spherical);
    SphericalOne& operator=(const SphericalOne &spherical);
    virtual ~SphericalOne();

    //  Friend functions
    friend std::ostream& operator<<(std::ostream& output, SphericalOne& s);
    friend std::istream& operator>>(std::istream& input, SphericalOne& s);

    // public methods
    Real GetAzimuth() const;
    void SetAzimuth(const Real az);

    Real GetFlightPathAngle() const;
    void SetFlightPathAngle(const Real fPA);

    bool      ToSphericalOne(const Cartesian &c);
    Cartesian GetCartesian(); 

    Integer GetNumData() const;
    const std::string* GetDataDescriptions() const;
    std::string* ToValueStrings();

protected:

private:
    // private method
    void    CalculateLocalPlaneCoord(const Rvector3 position,
                   Rvector3 &x_hat_lp,Rvector3 &y_hat_lp,Rvector3 &z_hat_lp);

    // private data method
    Real     azimuth;              //  Azimuth
    Real     flightPathAngle;      //  Flight path angle measured from axis to
                                   //  the vecolity vector  

   static const Integer NUM_DATA = 6;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
   std::string stringValues[NUM_DATA];

};
#endif // SphericalOne_hpp
