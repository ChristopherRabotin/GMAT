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
{
}


FiniteThrust::~FiniteThrust()
{
}


FiniteThrust::FiniteThrust(const FiniteThrust& ft) :
   PhysicalModel        (ft)
{
}


FiniteThrust& FiniteThrust::operator=(const FiniteThrust& ft)
{
   if (this == &ft)
      return *this;
      
   mySpacecraft = ft.mySpacecraft;
   spacecraft.clear();
      
   return *this;
}


GmatBase* FiniteThrust::Clone() const
{
   return new FiniteThrust(*this);
}


//------------------------------------------------------------------------------
// void Clear(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Clears the arrays of elements that get set by the Propagate commands
 * 
 * @param type The type of element that gets cleared.  Set to 
 *             Gmat::UNKNOWN_OBJECT to clear all of the configrable arrays.
 */
//------------------------------------------------------------------------------
void FiniteThrust::Clear(const Gmat::ObjectType type)
{
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::SPACECRAFT)) {
      mySpacecraft.clear();
      spacecraft.clear();
   }
   
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::THRUSTER)) {
      thrusterNames.clear();
   }
}


bool FiniteThrust::SetRefObjectName(const Gmat::ObjectType type,
                                  const std::string &name)
{
   if (type == Gmat::SPACECRAFT) {
      if (find(mySpacecraft.begin(), mySpacecraft.end(), name) == mySpacecraft.end())
         mySpacecraft.push_back(name);
      return true;
   }
   
   return PhysicalModel::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Set up data structures to manage finite burns
 * 
 * @return              true if the call succeeds, false on failure.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::Initialize(void)
{
   // set up the indices into the state vector that match spacecraft with active 
   // thrusters
   
   // Look up the thrust scale factors for the finite burns
   
   // Set up the arrays used to set the burn direction
}


//------------------------------------------------------------------------------
// bool GetDerivatives(Real * state, Real dt, Integer order)
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
bool FiniteThrust::GetDerivatives(Real * state, Real dt, Integer order)
{
   // Start with zero thrust
   
   // Accumulate thrust and mass flow for each active thruster
   
   // Divide through by masses to get accelerations
   
   // Apply the burns to the state vector
   
   return false;
}
