//$Header$
//------------------------------------------------------------------------------
//                              PropState
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/7/24
//
/**
 * Implements the state used in propagation. 
 */
//------------------------------------------------------------------------------


#include "PropState.hpp"
#include "SpaceObjectException.hpp"


PropState::PropState(const Integer dim) :
   dimension      (dim),
   epoch          (21545.0)
{
   state = new Real[dimension];
}


PropState::~PropState()
{
   delete [] state;
}


PropState::PropState(const PropState& ps) :
   dimension      (ps.dimension),
   epoch          (ps.epoch)
{
   state = new Real[dimension];
   for (Integer i = 0; i < dimension; ++i)
      state[i] = ps.state[i];
}


PropState& PropState::operator=(const PropState& ps)
{
   if (this == &ps)
      return *this;
      
   if (state)
      delete [] state;
   dimension = ps.dimension;
   state = new Real[dimension];
   for (Integer i = 0; i < dimension; ++i)
      state[i] = ps.state[i];
   epoch = ps.epoch;
   
   return *this;
}

 
Real& PropState::operator[](const Integer el)
{
   if ((el < 0 ) || (el >= dimension))
      throw SpaceObjectException("PropState array index out of bounds");
   return state[el];
}


Real PropState::operator[](const Integer el) const
{
   if ((el < 0 ) || (el >= dimension))
      throw SpaceObjectException("PropState array index out of bounds");
   return state[el];
}


// Size manipulations
void PropState::Grow(const Integer size)
{
   if (size > dimension) {
      Real *newstate = new Real[size];
      memcpy(newstate, state, dimension * sizeof(Real));
      delete [] state;
      state = newstate;
      dimension = size;
   }
}


void PropState::Shrink(const Integer size)
{
   if (size < dimension) {
      Real *newstate = new Real[size];
      memcpy(newstate, state, size * sizeof(Real));
      delete [] state;
      state = newstate;
      dimension = size;
   }
}


// Accessors
Integer PropState::GetDimension() const
{
   return dimension;
}


Real* PropState::GetState()
{
   return state;
}


bool PropState::SetState(const Real *data, const Integer size)
{
   if (size <= dimension) {
      memcpy(state, data, size*sizeof(Real));
      return true;
   }
   return false;
}


Real PropState::GetEpoch() const
{
   return epoch;
}


Real PropState::SetEpoch(const Real ep)
{
   return epoch = ep;
}
