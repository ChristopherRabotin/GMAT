#include "GroundstationInterface.hpp"


GroundstationInterface::GroundstationInterface(const std::string &itsTypeName, 
      const std::string &itsName) :
   BodyFixedPoint    (itsTypeName, itsName, Gmat::GROUND_STATION)
{
   objectTypes.push_back(Gmat::GROUND_STATION);
}


GroundstationInterface::~GroundstationInterface()
{
}


GroundstationInterface::GroundstationInterface(const GroundstationInterface& gsi) :
   BodyFixedPoint          (gsi)
{
}

GroundstationInterface& GroundstationInterface::operator=(const GroundstationInterface& gsi)
{
   if (this != &gsi)
   {
      BodyFixedPoint::operator=(gsi);
   }
   return *this;
}
