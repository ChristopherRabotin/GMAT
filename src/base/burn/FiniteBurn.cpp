//$Header$
//------------------------------------------------------------------------------
//                              FiniteBurn
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/12/20
//
/**
 * Implements the FiniteBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#include "FiniteBurn.hpp"


FiniteBurn::FiniteBurn(std::string nomme) :
   Burn        ("FiniteBurn", nomme)
{
}


FiniteBurn::~FiniteBurn()
{}


FiniteBurn::FiniteBurn(const FiniteBurn& fb) :
   Burn        (fb)
{
}


FiniteBurn& FiniteBurn::operator=(const FiniteBurn& fb)
{
   if (&fb == this)
      return *this;
      
   return *this;
}

   
bool FiniteBurn::Fire(Real *burnData)
{
   return false;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ImpulsiveBurn.
 *
 * @return clone of the ImpulsiveBurn.
 *
 */
//------------------------------------------------------------------------------
GmatBase* FiniteBurn::Clone(void) const
{
   return (new FiniteBurn(*this));
}

