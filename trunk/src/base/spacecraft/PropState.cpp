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


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// PropState(const Integer dim)
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @param <dim>   Size of the requested state vector.
 */
//------------------------------------------------------------------------------
PropState::PropState(const Integer dim) :
   dimension      (dim),
   epoch          (21545.0)
{
   state = new Real[dimension];
}


//------------------------------------------------------------------------------
// ~PropState()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
PropState::~PropState()
{
   delete [] state;
}


//------------------------------------------------------------------------------
// PropState(const PropState& ps)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <ps>  The PropState that gets copied here.
 */
//------------------------------------------------------------------------------
PropState::PropState(const PropState& ps) :
   dimension      (ps.dimension),
   epoch          (ps.epoch)
{
   state = new Real[dimension];
   for (Integer i = 0; i < dimension; ++i)
      state[i] = ps.state[i];
}


//------------------------------------------------------------------------------
// PropState& operator=(const PropState& ps)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <ps>  The PropState that gets copied here.
 *
 * @return this PropState, with data matching the input state.
 */
//------------------------------------------------------------------------------
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

 
//------------------------------------------------------------------------------
// Real operator[](const Integer el)
//------------------------------------------------------------------------------
/**
 * Element access operator.
 *
 * This operator lets other code read and assign values to the elements of a
 * PropState as if it were a standard C/C++ array.
 *
 * @param <el>  Index of the PropState element that is being manipulated.
 *
 * @return The element's value at the end of the call.
 */
//------------------------------------------------------------------------------
Real& PropState::operator[](const Integer el)
{
   if ((el < 0 ) || (el >= dimension))
      throw SpaceObjectException("PropState array index out of bounds");
   return state[el];
}


//------------------------------------------------------------------------------
// Real operator[](const Integer el) const
//------------------------------------------------------------------------------
/**
 * Element access operator.
 *
 * This operator lets other code read values to the elements of a PropState
 * as if it were a standard C/C++ array from within const methods.
 *
 * @param <el>  Index of the PropState element that is being manipulated.
 *
 * @return The element's value at the end of the call.
 */
//------------------------------------------------------------------------------
Real PropState::operator[](const Integer el) const
{
   if ((el < 0 ) || (el >= dimension))
      throw SpaceObjectException("PropState array index out of bounds");
   return state[el];
}


//------------------------------------------------------------------------------
// void SetSize(const Integer size)
//------------------------------------------------------------------------------
/**
 * Array size manipulator.
 *
 * This method changes the size of the PropState vector.  The elements of the 
 * old vector are copied into the new vector; if the new size is larger than the
 * old vector, only the elements up to the old size are filled.  If the new
 * vector is smaller, only the elements at the start of the old vector are
 * copied into the new one.
 *
 * @param <size> Size of the new PropState (must be greater than 0).
 *
 * @return The element's value at the end of the call.
 */
//------------------------------------------------------------------------------
void PropState::SetSize(const Integer size)
{
   if (size != dimension)
   {
      if (size <= 0)
         throw SpaceObjectException(
            "PropState resize requested for an unphysical state size.");
      Real *newstate = new Real[size];
      Integer copySize = ((size > dimension) ? dimension : size);
      memcpy(newstate, state, copySize * sizeof(Real));
      delete [] state;
      state = newstate;
      dimension = size;
   }
}


// Accessors

//------------------------------------------------------------------------------
// Integer GetDimension() const
//------------------------------------------------------------------------------
/**
 * Finds the size of the PropState vector.
 *
 * @return The size of the vector.
 */
//------------------------------------------------------------------------------
Integer PropState::GetSize() const
{
   return dimension;
}


//------------------------------------------------------------------------------
// Real* GetState()
//------------------------------------------------------------------------------
/**
 * Accesses the state vector.
 *
 * @return The state vector.
 */
//------------------------------------------------------------------------------
Real* PropState::GetState()
{
   return state;
}


//------------------------------------------------------------------------------
// bool PropState::SetState(const Real *data, const Integer size)
//------------------------------------------------------------------------------
/**
 * Sets the state elements to match an input array.
 *
 * This method copies the elements of the input array into the  first size 
 * elements of the PropState vector.
 *
 * @param <data> The data that gets copied into the state vector.
 * @param <size> Number of elements that get copied (must be greater than 0).
 *
 * @return true if the elements were filled successfully.
 */
//------------------------------------------------------------------------------
bool PropState::SetState(const Real *data, const Integer size)
{
   if (size <= dimension) {
      if (size <= 0)
         throw SpaceObjectException(
            "PropState attempting to fill an unphysical number of elements.");
      memcpy(state, data, size*sizeof(Real));
      return true;
   }
   return false;
}


//------------------------------------------------------------------------------
// Real GetEpoch()
//------------------------------------------------------------------------------
/**
 * Accessor for the current epoch of the object, in A.1 Modified Julian format.
 *
 * @return The A.1 epoch.
 *
 * @todo The epoch probably should be TAI throughout GMAT.
 */
//------------------------------------------------------------------------------
Real PropState::GetEpoch() const
{
   return epoch;
}


//------------------------------------------------------------------------------
// Real SetEpoch(const Real ep)
//------------------------------------------------------------------------------
/**
 * Accessor used to set epoch (in A.1 Modified Julian format) of the object.
 *
 * @param <ep> The new A.1 epoch.
 *
 * @return The updated A.1 epoch.
 *
 * @todo The epoch probably should be TAI throughout GMAT.
 */
//------------------------------------------------------------------------------
Real PropState::SetEpoch(const Real ep)
{
   return epoch = ep;
}
