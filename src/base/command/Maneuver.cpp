//$Header$
//------------------------------------------------------------------------------
//                                 Maneuver
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/20
//
/**
 * Definition for the Maneuver command class
 */
//------------------------------------------------------------------------------


#include "Maneuver.hpp"


Maneuver::Maneuver(void) :
    Command     ("Maneuver"),
    burnName    (""),
    burn        (NULL),
    satName     (""),
    sat         (NULL),
    // Parameter IDs 
    burnNameID  (parameterCount),
    satNameID   (parameterCount+1)
{
    parameterCount += 1;
}


Maneuver::~Maneuver(void)
{
    // Delete the burn if it is not managed in the ConfigManager
    if (burn)
        if (burn->GetName() == "")
            delete burn;
}


Maneuver::Maneuver(const Maneuver& m) :
    Command     (m),
    burnName    (m.burnName),
    burn        (NULL),
    satName     (m.satName),
    sat         (NULL),
    // Parameter IDs 
    burnNameID  (m.burnNameID),
    satNameID   (m.satNameID)
{
    parameterCount = m.parameterCount;
}


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
    if (id == burnNameID) {
        return "Burn";
    }
    
    else if (id == satNameID) {
        return "Spacecraft";
    }
    
    return Command::GetParameterText(id);
}


Integer Maneuver::GetParameterID(const std::string &str) const
{
    if (str == "Burn") {
        return burnNameID;
    }
    
    else if (str == "Spacecraft") {
        return satNameID;
    }

    return Command::GetParameterID(str);
}


Gmat::ParameterType Maneuver::GetParameterType(const Integer id) const
{
    if (id == burnNameID) {
        return Gmat::STRING_TYPE;
    }
    
    if (id == satNameID) {
        return Gmat::STRING_TYPE;
    }
    
    return Command::GetParameterType(id);
}


std::string Maneuver::GetParameterTypeString(const Integer id) const
{
    if (id == burnNameID)
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
    
    if (id == satNameID)
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
    
    return Command::GetParameterTypeString(id);
}


std::string Maneuver::GetStringParameter(const Integer id) const
{
    if (id == burnNameID)
        return burnName;
    
    if (id == satNameID)
        return satName;
    
    return Command::GetStringParameter(id);
}


bool Maneuver::SetStringParameter(const Integer id, const std::string &value)
{
    if (id == burnNameID) {
        burnName = value;
        return true;
    }
    
    if (id == satNameID) {
        satName = value;
        return true;
    }
    
    return Command::SetStringParameter(id, value);
}


void Maneuver::InterpretAction(void)
{
    /// @todo: Clean up this hack for the Maneuver::InterpretAction method
    // Sample string:  "Maneuver prop(Sat1);"
    Integer loc = generatingString.find("Maneuver", 0) + 8, end;
    const char *str = generatingString.c_str();
    while (str[loc] == ' ')
        ++loc;
    
    end = generatingString.find("(", loc);
    if (end == (Integer)std::string::npos)
        throw CommandException("Maneuver string does not identify burn");
    
    std::string component = generatingString.substr(loc, end-loc);
    if (component == "")
        throw CommandException("Maneuver string does not identify burn");
    SetStringParameter(burnNameID, component);
    
    loc = end + 1;
    end = generatingString.find(")", loc);
    if (end == (Integer)std::string::npos)
        throw CommandException("Maneuver string does not identify spacecraft");
    
    component = generatingString.substr(loc, end-loc);
    if (component == "")
        throw CommandException("Maneuver string does not identify spacecraft");
    SetStringParameter(satNameID, component);
}


bool Maneuver::Initialize(void)
{
    Command::Initialize();

    if (objectMap->find(burnName) == objectMap->end())
        throw CommandException("Maneuver command cannot find Burn");
    burn = (Burn *)((*objectMap)[burnName]);
    if (!burn)
        return false;
    
    if (objectMap->find(satName) == objectMap->end())
        throw CommandException("Maneuver command cannot find Spacecraft");
    sat = (Spacecraft *)((*objectMap)[satName]);
    if (!sat)
        return false;
    
    burn->SetSpacecraftToManeuver(sat);
    
    return true;
}


bool Maneuver::Execute(void)
{
    return burn->Fire();
}
