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
 * An exponentially modeled atmosphere
 */
//------------------------------------------------------------------------------


#ifndef ExponentialAtmosphere_hpp
#define ExponentialAtmosphere_hpp


#include "AtmosphereModel.hpp"


class ExponentialAtmosphere : public AtmosphereModel
{
public:
	ExponentialAtmosphere();
	virtual ~ExponentialAtmosphere();
    
    bool                    Density(Real *position, Real *density, 
                                    Real epoch=21545.0,
                                    Integer count = 1);
protected: 
    /// The "Scale height"
    Real                    scale[11];      // in km^-1
    /// Reference heights
    Real                    z[11];
    /// Reference density
    Real                    rho0;
    /// Lag angle for the bulge trailing the Sun (in radians)
    Real                    lagAngle;
    
    void                    SetConstants(void);

private:
    ExponentialAtmosphere(const ExponentialAtmosphere& atm);
    ExponentialAtmosphere&  operator=(const ExponentialAtmosphere& atm);
};


#endif // ExponentialAtmosphere_hpp
