#include "PropState.hpp"
#include "SpaceObjectException.hpp"


PropState::PropState(Integer dim) :
   dimension      (dim)
{
   state = new Real[dim];
}


PropState::~PropState()
{
   delete [] state;
}


PropState::PropState(const PropState& ps)
{
}


PropState& PropState::operator=(const PropState& ps)
{
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
