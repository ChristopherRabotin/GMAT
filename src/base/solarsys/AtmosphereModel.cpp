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


#include "AtmosphereModel.hpp"

AtmosphereModel::AtmosphereModel(const std::string &typeStr) :
    GmatBase            (Gmat::ATMOSPHERE, typeStr),
    sunVector           (NULL),
    centralBody         ("Earth"),
    centralBodyLocation (NULL),
    cbRadius            (6378.14)
{
}


AtmosphereModel::~AtmosphereModel()
{
}


void AtmosphereModel::SetSunVector(Real *sv)
{
    sunVector = sv;
}


void AtmosphereModel::SetCentralBodyVector(Real *cv)
{
    centralBodyLocation = cv;
}

