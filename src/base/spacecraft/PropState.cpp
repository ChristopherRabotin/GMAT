#include "PropState.hpp"
#include "SpaceObjectException.hpp"


PropState::PropState(Integer dim) :
   dimension      (dim)
{
   state = new Real[dimension];
}


PropState::~PropState()
{
   delete [] state;
}


PropState::PropState(const PropState& ps) :
   dimension      (ps.dimension)
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


Real* PropState::GetState()
{
   return state;
}
