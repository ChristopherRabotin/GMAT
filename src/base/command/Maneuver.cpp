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

//#define DEBUG_MANEUVER 1

//------------------------------------------------------------------------------
// Maneuver()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
Maneuver::Maneuver() :
   GmatCommand ("Maneuver"),
   burnName    (""),
   burn        (NULL),
   satName     (""),
   sat         (NULL),
   // Parameter IDs
   burnNameID  (parameterCount),
   satNameID   (parameterCount+1)
{
   parameterCount += 2;
}


//------------------------------------------------------------------------------
// ~Maneuver()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Maneuver::~Maneuver(void)
{
}


//------------------------------------------------------------------------------
// Maneuver(const Maneuver& m)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param m The maneuver that is copied to this one.
 */
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
// Maneuver& operator=(const Maneuver& m)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Maneuver command.
 *
 * @param m The maneuver that is copied to this one.
 *
 * @return this instance, with the internal data set to match the original's.
 */
//------------------------------------------------------------------------------
Maneuver& Maneuver::operator=(const Maneuver& m)
{
   if (this == &m)
      return *this;

   GmatCommand::operator=(m);
   burnName = m.burnName;
   burn     = NULL;
   satName  = m.satName;
   sat      = NULL;

   return *this;
}

//------------------------------------------------------------------------------
//  bool SetObject(const std::string &name, const Gmat::ObjectType type,
//                 const std::string &associate,
//                 const Gmat::ObjectType associateType)
//------------------------------------------------------------------------------
/**
 * Sets an object used by the Maneuver command.
 *
 * @param name The name of the object.
 * @param type Type of object.
 * @param associate Name of an associated object, if needed.
 * @param associateType Type of associated object.
 *
 * @return true if the object was set.
 */
//------------------------------------------------------------------------------
bool Maneuver::SetObject(const std::string &name, const Gmat::ObjectType type,
                         const std::string &associate,
                         const Gmat::ObjectType associateType)
{
   return GmatCommand::SetObject(name, type, associate, associateType);
}


//------------------------------------------------------------------------------
//  bool SetObject(GmatBase *obj, const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Sets an object used by the Maneuver command.
 *
 * @param obj The object.
 * @param type Type of object.
 *
 * @return true if the object was set.
 */
//------------------------------------------------------------------------------
bool Maneuver::SetObject(GmatBase *obj, const Gmat::ObjectType type)
{
   return GmatCommand::SetObject(obj, type);
}



//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Maneuver.
 *
 * @return clone of the Maneuver.
 */
//------------------------------------------------------------------------------
GmatBase* Maneuver::Clone() const
{
   return (new Maneuver(*this));
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param <mode>    Specifies the type of serialization requested.
 * @param <prefix>  Optional prefix appended to the object's name. (Used for
 *                  indentation)
 * @param <useName> Name that replaces the object's name (Not yet used
 *                  in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& Maneuver::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   generatingString = prefix + "Maneuver ";
   generatingString += burnName + "(" + satName + ");";

   return generatingString;
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * This method renames an object used by the maneuver command.
 *
 * @param type The type of the object.
 * @param oldName The current name for the object.
 * @param newName The new name for the object.
 *
 * @return clone of the Maneuver.
 */
//------------------------------------------------------------------------------
bool Maneuver::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   #if DEBUG_MANEUVER
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
      }
   }
   
   return true;
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Maneuver::GetParameterTypeString(const Integer id) const
{
   if (id == burnNameID)
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
    
   if (id == satNameID)
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
    
   return GmatCommand::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Maneuver::GetStringParameter(const Integer id) const
{
   if (id == burnNameID)
      return burnName;
    
   if (id == satNameID)
      return satName;
    
   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id    ID for the requested parameter.
 * @param value string value for the requested parameter.
 *
 * @return  success flag.
 */
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
//  bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Maneuver command has the following syntax:
 *
 *     Maneuver burn1(sat1);
 *
 * where burn1 is an ImpulsiveBurn used to perform the maneuver, and sat1 is the
 * name of the spacecraft that is maneuvered.  This method breaks the script
 * line into the corresponding pieces, and stores the name of the ImpulsiveBurn
 * and the Spacecraft so they can be set to point to the correct objects during
 * initialization.
 */
//------------------------------------------------------------------------------
bool Maneuver::InterpretAction()
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


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Maneuver.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
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
    
   return true;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Fires an impulsive maneuver.
 *
 * This method calls the Fire method on an ImpulsiveBurn object.
 *
 * @return true if the Maneuver is executed, false if not.
 */
//------------------------------------------------------------------------------
bool Maneuver::Execute()
{
   burn->SetSpacecraftToManeuver(sat);
   return burn->Fire();
}
