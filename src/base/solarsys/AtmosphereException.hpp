//$Header$
//------------------------------------------------------------------------------
//                              PlanetaryEphemException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2004/02/29
//
/**
 * This class provides an exception class for the AtmosphereModel classes
 */
//------------------------------------------------------------------------------
#ifndef AtmosphereException_hpp
#define AtmosphereException_hpp

#include "gmatdefs.hpp"
#include "BaseException.hpp"

class GMAT_API AtmosphereException : public BaseException
{
public:

   AtmosphereException(std::string details);

protected:

private:
};
#endif // AtmosphereException_hpp
