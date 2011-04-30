//$Id$
//------------------------------------------------------------------------------
//                              PlanetaryEphemException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2004/02/17
//
/**
 * This class provides an exception class for the PlanetaryEphem class
 */
//------------------------------------------------------------------------------
#ifndef PlanetaryEphemException_hpp
#define PlanetaryEphemException_hpp

#include "gmatdefs.hpp"
#include "BaseException.hpp"

class GMAT_API PlanetaryEphemException : public BaseException
{
public:

   PlanetaryEphemException(const std::string &details = "");

protected:

private:
};
#endif // PlanetaryEphemException_hpp
