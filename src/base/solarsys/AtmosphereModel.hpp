//$Header$
//------------------------------------------------------------------------------
//                              AtmosphereModel
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
 * Base class for the atmosphere models
 */
//------------------------------------------------------------------------------


#ifndef AtmosphereModel_hpp
#define AtmosphereModel_hpp


#include "GmatBase.hpp"
#include "AtmosphereException.hpp"


/**
 * Base class for atmosphere models.
 * 
 * Atmosphere models are used to model drag forces and other aerodynamics.
 */ 
class AtmosphereModel : public GmatBase
{
public:

    AtmosphereModel(const std::string &typeStr);
    virtual ~AtmosphereModel();
    
    /**
     * Calculates the atmospheric density at a specified location.
     * 
     * Density if the core calculation provided by classes derived from this one.
     * The outpur array, density, must contain the density at the requested
     * locations, expressed in kg / m^3.
     * 
     *  @param position  The input vector of spacecraft states
     *  @param density   The array of output densities
     *  @param epoch     The current TAIJulian epoch
     *  @param count     The number of spacecraft contained in position
     */
    virtual bool            Density(Real *position, Real *density, 
                                    Real epoch = 21545.0,
                                    Integer count = 1) = 0;
    void                    SetSunVector(Real *sv);
    void                    SetCentralBodyVector(Real *cv);
    
protected:
    /// Vector from the central body to the sun
    Real                    *sunVector;
    /// Name of the central body
    std::string             centralBody;
    /// Location of the central body
    Real                    *centralBodyLocation;
    /// Central body radius
    Real                    cbRadius;

private:
    AtmosphereModel(const AtmosphereModel& am);
    AtmosphereModel&        operator=(const AtmosphereModel& am);
};

#endif // AtmosphereModel_hpp
