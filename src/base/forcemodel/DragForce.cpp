//$Header$
//------------------------------------------------------------------------------
//                                DragForce
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/29
//
/**
 * Drag force modeling
 */
//------------------------------------------------------------------------------


#include "DragForce.hpp"
#include "ForceModelException.hpp"

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
{
}


DragForce& DragForce::operator=(const DragForce& df)
{
    if (this == &df)
        return *this;
        
    return *this;
}

 
bool DragForce::GetComponentMap(Integer * map, Integer order) const
{
    Integer i6;

    if (order != 1)
        throw ForceModelException("Drag supports 1st order equations of motion only");

    // Calculate how many spacecraft are in the model
    Integer satCount = (Integer)(dimension / 6);
    for (Integer i = 0; i < satCount; i++) 
    {
        i6 = i * 6;

        map[ i6 ] = i6 + 3;
        map[i6+1] = i6 + 4;
        map[i6+2] = i6 + 5;
        map[i6+3] = -1;
        map[i6+4] = -1;
        map[i6+5] = -1;
    }

    return true;
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


