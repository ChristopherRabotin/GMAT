#ifndef PROPSTATE_H
#define PROPSTATE_H

#include "GmatBase.hpp"

class PropState // : public GmatBase
{
public:

	PropState(Integer dim = 6);
	virtual ~PropState();
   PropState(const PropState& ps);
   PropState&        operator=(const PropState& ps);
 
   Real&             operator[](const Integer el);
   Real              operator[](const Integer el) const;
//   Real              operator-(const Real el)
   
   Real*             GetState();
   
 
protected:
   /// Array used for the state data
   Real              *state;
   /// Size of the state vector
   Integer           dimension;
   
};

#endif // PROPSTATE_H
