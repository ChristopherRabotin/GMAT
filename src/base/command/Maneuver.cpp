//$Header$
//------------------------------------------------------------------------------
//                                 Maneuver
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
* Definition for the Maneuver command class
 */
//------------------------------------------------------------------------------


#include "Maneuver.hpp"


Maneuver::Maneuver(void) :
    Command     ("Maneuver")
{}


Maneuver::~Maneuver(void)
{}

    
Maneuver::Maneuver(const Maneuver& m) :
    Command     (m)
{}


Maneuver& Maneuver::operator=(const Maneuver& m)
{
    if (this == &m)
        return *this;
    
    return *this;
}

bool Maneuver::SetObject(const std::string &name, const Gmat::ObjectType type,
                         const std::string &associate,
                         const Gmat::ObjectType associateType)
{
    return Command::SetObject(name, type, associate, associateType);
}

bool Maneuver::SetObject(GmatBase *obj, const Gmat::ObjectType type)
{
    return Command::SetObject(obj, type);
}

std::string Maneuver::GetParameterText(const Integer id) const
{
    return Command::GetParameterText(id);
}

Integer Maneuver::GetParameterID(const std::string &str) const
{
    return Command::GetParameterID(str);
}

Gmat::ParameterType Maneuver::GetParameterType(const Integer id) const
{
    return Command::GetParameterType(id);
}

std::string Maneuver::GetParameterTypeString(const Integer id) const
{
    return Command::GetParameterTypeString(id);
}

Integer Maneuver::GetIntegerParameter(const Integer id) const
{
    return Command::GetIntegerParameter(id);
}

Integer Maneuver::SetIntegerParameter(const Integer id, const Integer value)
{
    return Command::SetIntegerParameter(id, value);
}

bool Maneuver::GetBooleanParameter(const Integer id) const
{
    return Command::GetBooleanParameter(id);
}

bool Maneuver::SetBooleanParameter(const Integer id, const bool value)
{
    return Command::SetBooleanParameter(id, value);
}

void Maneuver::InterpretAction(void)
{
}

bool Maneuver::Initialize(void)
{
    return false;
}

bool Maneuver::Execute(void)
{
    return false;
}
