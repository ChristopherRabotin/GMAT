//$Header$
//------------------------------------------------------------------------------
//                              Msise90Atmosphere
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
 * The MSISE90 atmosphere
 */
//------------------------------------------------------------------------------


#ifndef Msise90Atmosphere_hpp
#define Msise90Atmosphere_hpp

#include "AtmosphereModel.hpp"
#include "msise90.hpp"


class Msise90Atmosphere : public AtmosphereModel
{
public:
    Msise90Atmosphere();
    virtual ~Msise90Atmosphere();
    
    bool                    Density(Real *position, Real *density, 
                                    Integer count = 1);
                                    
protected:
    MSISE90                 msise90;

private:
    Msise90Atmosphere(const Msise90Atmosphere& msise);
    Msise90Atmosphere&      operator=(const Msise90Atmosphere& msise);
};


#endif // Msise90Atmosphere_hpp
