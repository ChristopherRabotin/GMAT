#include "DragForce.hpp"
#include "GmatBaseException.hpp"

DragForce::DragForce() :
    PhysicalModel           (Gmat::PHYSICAL_MODEL, "DragForce"),
    sunPosition             (NULL),
    cbPosition              (NULL),
    angVel                  (NULL),
    useExternalAtmosphere   (true),
    atmos                   (NULL)
{
    // wCrossR[0];
}


DragForce::~DragForce()
{
    if (!useExternalAtmosphere && atmos)
        delete atmos;
}


DragForce::DragForce(const DragForce& df) :
    PhysicalModel           (df)
{}


DragForce& DragForce::operator=(const DragForce& df)
{
    if (this == &df)
        return *this;
        
    return *this;
}

 
bool DragForce::GetComponentMap(Integer * map, Integer order) const
{
    if (order != 1)
        throw GmatBaseException("Drag supports 1st order equations of motion only");
    return false;
}


bool DragForce::Initialize(void)
{
    return false;
}


bool DragForce::GetDerivatives(Real * state, Real dt, Integer order)
{
    return false;
}


std::string DragForce::GetParameterText(const Integer id) const
{
    return PhysicalModel::GetParameterText(id);
}


Integer DragForce::GetParameterID(const std::string &str) const
{
    return PhysicalModel::GetParameterID(str);
}


Gmat::ParameterType DragForce::GetParameterType(const Integer id) const
{
    return PhysicalModel::GetParameterType(id);
}


std::string DragForce::GetParameterTypeString(const Integer id) const
{
    return PhysicalModel::GetParameterTypeString(id);
}


Real DragForce::GetRealParameter(const Integer id) const
{
    return PhysicalModel::GetRealParameter(id);
}


Real DragForce::SetRealParameter(const Integer id, const Real value)
{
    return PhysicalModel::SetRealParameter(id, value);
}



Real DragForce::GetDensity(Real *position)
{
//    if (!atmos)
        return 1.0e-12;
        
//    return atmos->Density(position, density, Integer count);
}


