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


class AtmosphereModel : public GmatBase
{
public:

    AtmosphereModel(const std::string &typeStr);
    virtual ~AtmosphereModel();
    
    virtual bool            Density(Real *position, Real *density, 
                                    Integer count = 1) = 0;
    void                    SetSunVector(Real *sv);
    
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
