/*
 * EventFunction.cpp
 *
 *  Created on: Aug 16, 2011
 *      Author: djc
 */

#include "gmatdefs.hpp"
#include "EventFunction.hpp"

EventFunction::EventFunction(const std::string &type, const std::string &name) :
   value             (99999.99999),
   derivative        (99999.99999),
   primary           (NULL)
{
   // TODO Auto-generated constructor stub

}

EventFunction::~EventFunction()
{
   // TODO Auto-generated destructor stub
}

EventFunction::EventFunction(const EventFunction& ef)
{

}

EventFunction& EventFunction::operator=(const EventFunction& ef)
{
   if (&ef != this)
   {

   }

   return *this;
}
