//$Header$
//------------------------------------------------------------------------------
//                           SphericalAZFPA
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
 * Definition for the SphericalAZFPA class including Azimuth and Flight Path 
 * Angle with first four spherical elements.
 * 
 * @note:   This code is revived from the original argosy and swingby
 */
//------------------------------------------------------------------------------
#ifndef SphericalAZFPA_hpp
#define SphericalAZFPA_hpp

#include "Spherical.hpp"
#include "Anomaly.hpp"
#include "CoordUtil.hpp"

class SphericalAZFPA : public Spherical
{
public:
    SphericalAZFPA();
    SphericalAZFPA(const Rvector6& state);
    SphericalAZFPA(Real rMag, Real ra, Real dec, Real vmag, Real az, Real fPA);
    SphericalAZFPA(const SphericalAZFPA &spherical);
    SphericalAZFPA& operator=(const SphericalAZFPA &spherical);
    virtual ~SphericalAZFPA();

    //  Friend functions
    friend std::ostream& operator<<(std::ostream& output, SphericalAZFPA& s);
    friend std::istream& operator>>(std::istream& input, SphericalAZFPA& s);

    friend Rvector6 CartesianToSphericalAZFPA(const Rvector6& cartVector);
    friend Rvector6 SphericalAZFPAToCartesian(const Rvector6& sphVector);

    friend Rvector6 KeplerianToSphericalAZFPA(const Rvector6& keplerian, 
                                              const Real mu, Anomaly anomaly);
    friend Rvector6 SphericalAZFPAToKeplerian(const Rvector6& spherical, 
                                              const Real mu, Anomaly &anomaly);

    // public methods
    Rvector6 GetState();
    void SetState(const Rvector6& state);

    Real GetAzimuth() const;
    void SetAzimuth(const Real az);

    Real GetFlightPathAngle() const;
    void SetFlightPathAngle(const Real fPA);

    Rvector6  GetCartesian(); 

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
#endif // SphericalAZFPA_hpp
