//$Header$
//------------------------------------------------------------------------------
//                                DragForce
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/29
//
/**
 * Drag force modeling
 */
//------------------------------------------------------------------------------


#ifndef DRAGFORCE_H
#define DRAGFORCE_H

#include "PhysicalModel.hpp"
#include "AtmosphereModel.hpp"
#include <vector>

class DragForce : public PhysicalModel
{
public:
	DragForce();
	virtual ~DragForce();
 
    DragForce(const DragForce& df); 
    DragForce&          operator=(const DragForce& df); 
 
    virtual bool        GetComponentMap(Integer * map, Integer order = 1) const;
    void                SetSatelliteParameter(const Integer i, 
                                              const std::string parmName, 
                                              const Real parm);
    void                SetSatelliteParameter(const Integer i, 
                                              const std::string parmName, 
                                              const std::string parm);
    bool                Initialize(void);
    virtual bool        GetDerivatives(Real * state, Real dt = 0.0, 
                                       Integer order = 1);

    // Parameter accessor methods -- overridden from GmatBase
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer     GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;
    
    virtual Integer     GetIntegerParameter(const Integer id) const;
    virtual Integer     SetIntegerParameter(const Integer id, const Integer value);    
    virtual Real        GetRealParameter(const Integer id) const;
    virtual Real        SetRealParameter(const Integer id, const Real value);    

protected:
    /// Position of the Sun
    Real                *sunPosition;
    /// Position of the body with the atmosphere
    Real                *cbPosition;
    /// Angular velocity of the central body
    Real                *angVel;
    /// Flag to indicate if the atmosphere model is externally owned or internal
    bool                useExternalAtmosphere;
    /// Pointer to the atmosphere model used
    AtmosphereModel     *atmos;
    /// Solar system pointer for sun and body locations
    SolarSystem         *ss;
    /// Array of densities
    Real                *density;
    /// Array of products of spacecraft properties
    Real                *prefactor;
    /// Number of spacecraft in the state vector
    Integer             satCount;
    /// Central bodies used for atmosphere source
    std::vector<std::string>
                        dragBody;
    /// Spacecraft drag areas
    std::vector <Real>  area;
    /// Spacecraft masses
    std::vector <Real>  mass;
    /// Spacecraft coefficients of drag
    std::vector <Real>  dragCoeff;
    
    void                BuildPrefactors(void);
    void                GetDensity(Real *state);
};

#endif // DRAGFORCE_H
