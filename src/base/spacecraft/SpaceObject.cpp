#include "SpaceObject.hpp"

SpaceObject::SpaceObject(Gmat::ObjectType typeId, const std::string &typeStr, 
                         const std::string &nomme) :
   GmatBase(typeId, typeStr, nomme)
{}

SpaceObject::~SpaceObject()
{}


SpaceObject::SpaceObject(const SpaceObject& so) :
   GmatBase(so)
{}

SpaceObject& SpaceObject::operator=(const SpaceObject& so)
{
   if (this == &so)
      return *this;
      
   return *this;
}
