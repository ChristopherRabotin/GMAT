//$Header$
//------------------------------------------------------------------------------
//                              FiniteThrust
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
 * Implements the FiniteThrust class used to model the acceleration during a 
 * finite burn. 
 */
//------------------------------------------------------------------------------


#include "FiniteThrust.hpp"

FiniteThrust::FiniteThrust(const std::string &name) :
   PhysicalModel        (Gmat::PHYSICAL_MODEL, "FiniteThrust", name)
{}

FiniteThrust::~FiniteThrust()
{}

FiniteThrust::FiniteThrust(const FiniteThrust& ft) :
   PhysicalModel        (ft)
{
}


FiniteThrust& FiniteThrust::operator=(const FiniteThrust& ft)
{
   if (this == &ft)
      return *this;
      
   return *this;
}


//------------------------------------------------------------------------------
// bool PhysicalModel::GetDerivatives(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Method invoked to calculate derivatives
 * 
 * This method is invoked to fill the deriv array with derivative information 
 * for the system that is being integrated.  It uses the state and elapsedTime 
 * parameters, along with the time interval dt passed in as a parameter, to 
 * calculate the derivative information at time \f$t=t_0+t_{elapsed}+dt\f$.
 *
 * @param dt            Additional time increment for the derivatitive 
 *                      calculation; defaults to 0.
 * @param state         Pointer to the current state data.  This can differ
 *                      from the PhysicalModel state if the subscribing
 *                      integrator samples other state values during 
 *                      propagation.  (For example, the Runge-Kutta integrators 
 *                      do this during the stage calculations.)
 * @param order         The order of the derivative to be taken (first 
 *                      derivative, second derivative, etc)
 *
 * @return              true if the call succeeds, false on failure.
 */
//------------------------------------------------------------------------------
bool PhysicalModel::GetDerivatives(Real * state, Real dt, Integer order)
{
   return false;
}


