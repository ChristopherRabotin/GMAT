//$Header$
//------------------------------------------------------------------------------
//                              SolarSystemException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2004/03/02
//
/**
 * This class provides an exception class for the SolarSystem class
 */
//------------------------------------------------------------------------------
#ifndef SolarSystemException_hpp
#define SolarSystemException_hpp

#include "gmatdefs.hpp"
#include "BaseException.hpp"

class GMAT_API SolarSystemException : public BaseException
{
public:

   SolarSystemException(std::string details = "");

protected:

private:
};
#endif // SolarSystemException_hpp
