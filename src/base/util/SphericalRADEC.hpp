//$Header$
//------------------------------------------------------------------------------
//                           SphericalRADEC
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
 * Definition for the SphericalRADEC class used for spherical elements including
 * Right Ascension of Velocity and Declination of Velocity.
 *
 * @note:   This code is revived from the original argosy and swingby
 */
//------------------------------------------------------------------------------
#ifndef SphericalRADEC_hpp
#define SphericalRADEC_hpp

#include <iostream>
#include "Spherical.hpp"
#include "SphericalAZFPA.hpp"
#include "CoordUtil.hpp"

class SphericalRADEC : public Spherical
{
public:
   SphericalRADEC();
   SphericalRADEC(const Rvector6& state);
   SphericalRADEC(Real rMag, Real ra, Real dec, Real vmag, Real vRA, Real vDec);
   SphericalRADEC(const SphericalRADEC &spherical);
   SphericalRADEC& operator=(const SphericalRADEC &spherical);
   virtual ~SphericalRADEC();

   // public methods
   Rvector6 GetState();
   void SetState(const Rvector6& state);

   Real GetVelocityRA() const;
   void SetVelocityRA(const Real vRA);

   Real GetVelocityDeclination() const;
   void SetVelocityDeclination(const Real vDeclincation);

   Rvector6 GetCartesian();

   Integer GetNumData() const;
   const std::string* GetDataDescriptions() const;
   std::string* ToValueStrings();
   std::string ToString(); //loj: 6/16/04 added

   //  Friend functions
    friend Rvector6 CartesianToSphericalRADEC(const Rvector6& cartesian);
    friend Rvector6 SphericalRADECToCartesian(const Rvector6& spherical);

    friend Rvector6 KeplerianToSphericalRADEC(const Rvector6& keplerian,
                                              const Real mu, Anomaly anomaly);
    friend Rvector6 SphericalRADECToKeplerian(const Rvector6& spherical,
                                              const Real mu, Anomaly &anomaly);

    friend Rvector6 AZFPA_To_RADECV(const Rvector6& spherical);
    friend Rvector6 RADECV_To_AZFPA(const Rvector6& spherical);

   friend std::ostream& operator<<(std::ostream& output, SphericalRADEC& s);
   friend std::istream& operator>>(std::istream& input, SphericalRADEC& s);

protected:

private:
   Real     raVelocity;            //  Right Ascension of Velocity 
   Real     decVelocity;           //  Declination of Velocity 

   static const Integer NUM_DATA = 6;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
   std::string stringValues[NUM_DATA];

};
#endif // SphericalRADEC_hpp
