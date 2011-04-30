//$Id$
//------------------------------------------------------------------------------
//                              JacchiaRobertsAtmosphere
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Waka A. Waktola
// Created: 2004/05/11
//
/**
 * The Jacchia-Roberts atmosphere.
 */
//------------------------------------------------------------------------------

#ifndef JacchiaRobertsAtmosphere_hpp
#define JacchiaRobertsAtmosphere_hpp

#include "AtmosphereModel.hpp"
#include "CelestialBody.hpp"
#include "A1Mjd.hpp"
#include "TimeTypes.hpp"

/**
 * The Jacchia-Roberts Atmosphere Model
 *
 * This is the JR model, ported from the Swingby/Windows source.
 */
class GMAT_API JacchiaRobertsAtmosphere : public AtmosphereModel
{
public:
   JacchiaRobertsAtmosphere(const std::string &name = "");
   virtual ~JacchiaRobertsAtmosphere();
   JacchiaRobertsAtmosphere(const JacchiaRobertsAtmosphere& jr);
   JacchiaRobertsAtmosphere& operator=(const JacchiaRobertsAtmosphere& jr);
    
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
    
   bool Density(Real *position, Real *density, Real epoch = GmatTimeConstants::MJD_OF_J2000,
                Integer count = 1);
   virtual void SetCentralBody(CelestialBody *cb);

private:
   ///  Auxiliary temperature related quantities
   Real root1;
   Real root2;
   Real x_root;
   Real y_root;

   ///  Some intermediate temperature results used by density computations
   Real t_infinity;
   Real tx;
   Real sum;
   Real cbPolarRadius;
   Real cbPolarSquared;

   /// low altitude density in g/cm**2
   const Real rho_zero;
   /// Temperature in degrees kelvin at height of 90km
   const Real tzero;
   /// earth gravitational constant m/sec**2
   const Real g_zero;
   /// gas constant (joules/(degK-mole))
   const Real gas_con;
   /// Avogadro's number
   const Real avogadro;

   // Tables of constants from the model (formerly defined as globals)
   /// Constants for series expansion
   Real con_c[5];
   /// Constants for series expansion
   Real con_l[5];
   /// Constants required between 90 km  and 100 km
   Real mzero;
   /// Constants for series expansion
   Real m_con[7];
   /// Constants for series expansion
   Real s_con[6];
   /// Constants for series expansion
   Real s_beta[6];
   /// Constants required for attitudes between 100 km and 125 km
   Real omega;
   /// Constants for series expansion
   Real zeta_con[7];
   /// Molecular masses of atmospheric  constituents in grams/mole
   Real mol_mass[6];
   /// Number density divided by Avogadro's number of atmospheric constituents
   Real num_dens[5];
   // Constants required for altitude greater than 125 km
   /// Polynomial coefficients for constituent densities of each atmospheric gas
   Real con_den[5][7];


   Real JacchiaRoberts(Real height, Real space_craft[3], Real sun[3],
                      Real a1_time, bool new_file);
   Real exotherm(Real space_craft[3], Real sun[3], GEOPARMS *geo,
                       Real height, Real sun_dec, Real geo_lat);
   Real rho_100(Real height, Real temperature);
   Real rho_125(Real height, Real temperature);
   Real rho_cor(Real height, Real a1_time, Real geo_lat,
                      GEOPARMS *geo);
   Real rho_high(Real height, Real temperature, Real t_500,
                       Real sun_dec, Real geo_lat);
   void roots(Real a[], Integer na, Real croots[][2], Integer irl);
   void deflate_polynomial(Real c[], Integer n, Real root, Real c_new[]);
   Real length_of(Real v[3]);
   Real dot_product(Real a[3] , Real b[3]);
   void LoadConstants();
};

#endif // JacchiaRobertsAtmosphere_hpp
