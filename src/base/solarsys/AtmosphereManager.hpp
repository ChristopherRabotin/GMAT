//$Header$
//------------------------------------------------------------------------------
//                             AtmosphereManager
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
 * Manager for planetary atmosphere models
 */
//------------------------------------------------------------------------------


#ifndef AtmosphereManager_hpp
#define AtmosphereManager_hpp


#include "AtmosphereModel.hpp"


class AtmosphereManager
{
public:
	AtmosphereManager(const std::string& body = "Earth");
	virtual ~AtmosphereManager();
 
    const StringArray&  GetSupportedAtmospheres(const std::string& body = "");
    AtmosphereModel*    GetAtmosphere(const std::string& atmType = "", 
                                      bool isInternal = true);
    void                SetSunVector(Real *sv);
    
protected:
    /// Table of available atmospheres
    StringArray                 atmospheres;
    /// Table of available atmospheres for a specific body
    StringArray                 bodyAtmospheres;
    /// Current atmosphere used
    AtmosphereModel             *currentAtmosphere;
    /// Body that has the atmosphere
    std::string                 atmOwner;
    /// Vector from the owning body to the Sun
    Real                        *sunVector;
    
    void                        BuildBodyAtmospheres(void);

private:
    AtmosphereManager(const AtmosphereManager& am);
    AtmosphereManager&          operator=(const AtmosphereManager& am);
};

#endif // AtmosphereManager_hpp
