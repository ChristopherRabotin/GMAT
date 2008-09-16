//$Header$
//------------------------------------------------------------------------------
//                           ExponentialAtmosphere
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/21
//
/**
 * An exponentially modeled atmosphere based on Vallado, pp 532-534 and Wertz,
 * p 820.
 */
//------------------------------------------------------------------------------


#ifndef ExponentialAtmosphere_hpp
#define ExponentialAtmosphere_hpp


#include "AtmosphereModel.hpp"


/**
 * The exponential atmosphere model in Vallado (2001) and Wertz (1978).
 * 
 * This code calculates the atmosphereic density at a given position based on 
 * the altitude of the input state above the spherical Earth.  This model does
 * not include an atmospheric bulge due to solar heating.  The code will need
 * to be refined to use the oblate Earth once oblateness is added to the code.
 * 
 * The density is given by
 * 
 * \f[\rho = \rho_0 e^{-{{h_{ellp} - h_0}\over{H}}}\f]
 * 
 * where \f$\rho_0\f$ is a reference density, specified at a reference altitude
 * \f$h_0\f$, \f$h_{ellp}\f$ is the height of the specified position above the 
 * body's ellipsoid, and \f$H\f$ is a scale height, used to scale the other 
 * variables in the formula.
 * 
 * Developers and other users can build exponential models for bodies other than 
 * the Earth by deriving a class off of this one and overriding the SetConstants
 * method.
 * 
 * @todo Replace the spherical Earth model with an oblate Earth model.
 * @test Check to see if the band disconituities merit smoothing.
 */
class GMAT_API ExponentialAtmosphere : public AtmosphereModel
{
public:
   ExponentialAtmosphere();
   virtual ~ExponentialAtmosphere();
   ExponentialAtmosphere(const ExponentialAtmosphere& atm);
   ExponentialAtmosphere&  operator=(const ExponentialAtmosphere& atm);
    
   virtual bool            Density(Real *position, Real *density, 
                                   Real epoch=21545.0,
                                   Integer count = 1);

protected: 
   /// Table of scale heights, \f$H\f$.
   Real                    *scaleHeight;
   /// Table of Reference heights, \f$h_0\f$.
   Real                    *refHeight;
   /// Table of reference densities, \f$\rho_0\f$.
   Real                    *refDensity;
   /// Number of altitude bands used in the model.
   Integer                 altitudeBands;
   /// Flag indicating if the altitude is "at" a boundary
   bool                    smoothDensity;
    
   virtual void            SetConstants(void);
   Real                    CalculateHeight(Real *loc);
   Integer                 FindBand(Real height);
   Real                    Smooth(Real height, Integer index);

   virtual GmatBase*       Clone() const; // inherited from GmatBase

private:

};


#endif // ExponentialAtmosphere_hpp
