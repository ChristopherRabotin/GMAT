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

//#define DEBUG_RENAME 1

Maneuver::Maneuver(void) :
    GmatCommand     ("Maneuver"),
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
    GmatCommand     (m),
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
    return GmatCommand::SetObject(name, type, associate, associateType);
}

bool Maneuver::SetObject(GmatBase *obj, const Gmat::ObjectType type)
{
    return GmatCommand::SetObject(obj, type);
}



//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Maneuver.
 *
 * @return clone of the Maneuver.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Maneuver::Clone(void) const
{
   return (new Maneuver(*this));
}

//loj: 11/16/04 added
//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Maneuver::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
#if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Maneuver::RenameConfiguredItem() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
#endif
   
   if (type != Gmat::SPACECRAFT && type != Gmat::BURN)
      return true;
   
   if (type == Gmat::SPACECRAFT)
   {
      if (satName == oldName)
      {
         satName = newName;
         return true;
      }
   }
   else if (type == Gmat::BURN)
   {
      if (burnName == oldName)
      {
         burnName = newName;
         return true;
      }
   }
   
   return false;
}

std::string Maneuver::GetParameterText(const Integer id) const
{
    if (id == burnNameID) {
        return "Burn";
    }
    
    else if (id == satNameID) {
        return "Spacecraft";
    }
    
    return GmatCommand::GetParameterText(id);
}


Integer Maneuver::GetParameterID(const std::string &str) const
{
    if (str == "Burn") {
        return burnNameID;
    }
    
    else if (str == "Spacecraft") {
        return satNameID;
    }

    return GmatCommand::GetParameterID(str);
}


Gmat::ParameterType Maneuver::GetParameterType(const Integer id) const
{
    if (id == burnNameID) {
        return Gmat::STRING_TYPE;
    }
    
    if (id == satNameID) {
        return Gmat::STRING_TYPE;
    }
    
    return GmatCommand::GetParameterType(id);
}


std::string Maneuver::GetParameterTypeString(const Integer id) const
{
    if (id == burnNameID)
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
    
    if (id == satNameID)
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
    
    return GmatCommand::GetParameterTypeString(id);
}


std::string Maneuver::GetStringParameter(const Integer id) const
{
    if (id == burnNameID)
        return burnName;
    
    if (id == satNameID)
        return satName;
    
    return GmatCommand::GetStringParameter(id);
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
    
    return GmatCommand::SetStringParameter(id, value);
}


bool Maneuver::InterpretAction(void)
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
    
    return true;
}


bool Maneuver::Initialize(void)
{
    GmatCommand::Initialize();

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
