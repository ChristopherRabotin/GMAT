#include "AtmosphereModel.hpp"

AtmosphereModel::AtmosphereModel(const std::string &typeStr) :
    GmatBase    (Gmat::ATMOSPHERE, typeStr)
{
}


AtmosphereModel::~AtmosphereModel()
{
}


// This code will go away when the derived classes are implemented
bool AtmosphereModel::Density(Real *position, Real *density, Integer count)
{
}
