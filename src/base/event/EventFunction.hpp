/*
 * EventFunction.hpp
 *
 *  Created on: Aug 16, 2011
 *      Author: djc
 */

#ifndef EVENTFUNCTION_HPP_
#define EVENTFUNCTION_HPP_

#include "SpaceObject.hpp"

class EventFunction
{
public:
   EventFunction(const std::string &type, const std::string &name);
   virtual ~EventFunction();
   EventFunction(const EventFunction& ef);
   EventFunction& operator=(const EventFunction& ef);

   virtual Real* Evaluate() = 0;

protected:
   /// The value of the event function when last evaluated
   Real              value;
   /// The value of the event function derivative when last evaluated
   Real              derivative;
   /// The SpaceObject that plays the role of the “target” object
   SpaceObject       *primary;
};

#endif /* EVENTFUNCTION_HPP_ */
