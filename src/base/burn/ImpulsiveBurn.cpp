//$Header$
//------------------------------------------------------------------------------
//                              ImpulsiveBurn
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the ImpulsiveBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#include "ImpulsiveBurn.hpp"


ImpulsiveBurn::ImpulsiveBurn(std::string nomme) :
    Burn            ("ImpulsiveBurn", nomme)
{
}


ImpulsiveBurn::~ImpulsiveBurn()
{
}
