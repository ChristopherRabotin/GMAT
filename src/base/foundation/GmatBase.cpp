//$Header$
//------------------------------------------------------------------------------
//                                  GmatBase
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/25
//
/**
 * Implementation for the base class for all GMAT extensible objects
 *
 * The following class hierarchy trees use this class as their basis:
 *
 *     Asset (hence Spacecraft and GroundStation)
 *     CelestialBody
 *     Propagator
 *     PhysicalModel (hence Force and ForceModel)
 *     PropConfig
 *     Parameter
 *     Command
 *
 * Every class that users can use to derive new classes, or that need to be
 * accessed through generic pointers, should be derived from this class to
 * ensure consistent interfaces accessed by the GMAT control systems (i.e. the
 * Moderator, FactoryManager, Configuration, Interpreter, and Sandbox, along
 * with the GUIInterpreter).
 */
//------------------------------------------------------------------------------


#include "GmatBase.hpp"


/// Set the static "undefined" parameters
const Real        GmatBase::REAL_PARAMETER_UNDEFINED = -987654321.0123e-45;
const Integer     GmatBase::INTEGER_PARAMETER_UNDEFINED = -987654321;

/// Build the list of type names
const std::string GmatBase::PARAM_TYPE_STRING[Gmat::TypeCount] =
      {
        "Integer", "Real", "String", "Boolean",
        "Rvector3", "Rvector6", "Rmatrix33", "Cartesian", "Keplerian",
        "A1Mjd", "UtcDate", "Object"
      }; 


//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  GmatBase(Gmat::ObjectTypes typeId, std::string &typeStr, std::string &nomme)
//---------------------------------------------------------------------------
/**
 * Constructs base GmatBase structures used in GMAT extensible classes.
 *
 * @param <typeId> Gmat::ObjectTypes enumeration for the object.
 * @param <typeStr> GMAT script string associated with this type of object.
 * @param <nomme> Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for GmatBase.  Derived classes
 *       must pass in the typeId and typeStr parameters.
 */
GmatBase::GmatBase(const Gmat::ObjectType typeId, const std::string &typeStr, 
                   const std::string &nomme) :
    parameterCount  (0),
    typeName        (typeStr),
    instanceName    (nomme),
    type            (typeId)
{
}


//---------------------------------------------------------------------------
//  ~GmatBase(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
GmatBase::~GmatBase(void)
{
}


//---------------------------------------------------------------------------
//  GmatBase(const GmatBase &a)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base GmatBase structures.
 *
 * @param <a> The original that is being copied.
 */
GmatBase::GmatBase(const GmatBase &a) :
    parameterCount  (a.parameterCount),
    typeName        (a.typeName),
    instanceName    ("CopyOf"+a.instanceName),
    type            (a.type)
{
}


//---------------------------------------------------------------------------
//  GmatBase& operator=(const GmatBase &a)
//---------------------------------------------------------------------------
/**
 * Assignment operator for GmatBase structures.
 *
 * @param <a> The original that is being copied.
 *
 * @return Reference to this object
 */
GmatBase& GmatBase::operator=(const GmatBase &a)
{
    // Don't do anything if copying self
    if (&a == this)
        return *this;
        
    // Currently nothing to do from the base class; this may change in a later 
    // build

    return *this;
}


//---------------------------------------------------------------------------
//  Gmat::ObjectTypes GetType(void) const
//---------------------------------------------------------------------------
/**
 * Retrieve the enumerated base type for the object
 *
 * @return Enumeration value for this object
 */
Gmat::ObjectType GmatBase::GetType(void) const
{
    return type;
}


//---------------------------------------------------------------------------
//  std::string GetTypeName(void) const
//---------------------------------------------------------------------------
/**
 * Retrieve the script string used for this class.
 *
 * @return The string used in the scripting for this type of object.
 */
std::string GmatBase::GetTypeName(void) const
{
    return typeName;
}


//---------------------------------------------------------------------------
//  Integer GetParameterCount(void) const
//---------------------------------------------------------------------------
/**
 * Find out how many parameters are accessible for this object.
 *
 * @return The number of parameters.
 */
Integer GmatBase::GetParameterCount(void) const
{
    return parameterCount;
}


//---------------------------------------------------------------------------
//  std::string GetName(void) const
//---------------------------------------------------------------------------
/**
 * Retrieve the name of the instance.
 *
 * @return This object's name, or the empty string ("").
 *
 * @note Some classes are unnamed.
 */
std::string GmatBase::GetName(void) const
{
    return instanceName;
}


//---------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the enumerated type of the object.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The enumeration for the type of the parameter, or
 *         UNKNOWN_PARAMETER_TYPE.
 */
Gmat::ParameterType GmatBase::GetParameterType(const Integer id) const
{
    return Gmat::UNKNOWN_PARAMETER_TYPE;
}


//---------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the string associated with a parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return Text description for the type of the parameter, or the empty
 *         string ("").
 */
std::string GmatBase::GetParameterTypeString(const Integer id) const
{
    std::string retval = "";
    Gmat::ParameterType t = GetParameterType(id);
    if (t != Gmat::UNKNOWN_PARAMETER_TYPE)
        retval = PARAM_TYPE_STRING[t];
    return retval;
}


//---------------------------------------------------------------------------
//  bool SetName(std::string &who)
//---------------------------------------------------------------------------
/**
 * Set the name for this instance.
 *
 * @param <who> the object's name.
 *
 * @return true if the name was changed, false if an error was encountered.
 *
 * @note Some classes are unnamed.
 */
bool GmatBase::SetName(const std::string &who)
{
    instanceName = who;
    return true;
}


//---------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the description for the parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return String description for the requested parameter.
 *
 * @note The parameter strings should not include any white space
 */
std::string GmatBase::GetParameterText(const Integer id) const
{
    return "";
}


//---------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//---------------------------------------------------------------------------
/**
 * Retrieve the ID for the parameter given its description.
 *
 * @param <str> Description for the parameter.
 *
 * @return the parameter ID, or -1 if there is no associated ID.
 */
Integer GmatBase::GetParameterID(const std::string &str) const
{
    return -1;
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
Real GmatBase::GetRealParameter(const Integer id) const
{
    return REAL_PARAMETER_UNDEFINED;
}


//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or 
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the 
 *         parameter type is not Real.
 */
Real GmatBase::SetRealParameter(const Integer id, const Real value)
{
    return REAL_PARAMETER_UNDEFINED;
}


//---------------------------------------------------------------------------
//  Real GetIntegerParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
Integer GmatBase::GetIntegerParameter(const Integer id) const
{
    return INTEGER_PARAMETER_UNDEFINED;
}


//---------------------------------------------------------------------------
//  Integer SetIntegerParameter(const Integer id, const Integer value)
//---------------------------------------------------------------------------
/**
 * Set the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or 
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the 
 *         parameter type is not an Integer.
 */
Integer GmatBase::SetIntegerParameter(const Integer id, const Integer value)
{
    return INTEGER_PARAMETER_UNDEFINED;
}

//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
std::string GmatBase::GetStringParameter(const Integer id) const
{
    return "STRING_PARAMETER_UNDEFINED";
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
bool GmatBase::SetStringParameter(const Integer id, const std::string &value)
{
    return false;
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
StringArray& GmatBase::GetStringArrayParameter(const Integer id) const
{
    std::string errorText = "No StringArrays defined for the ID ";
    errorText += id;
    throw GmatBaseException(errorText);
}


//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 *
 * @todo Setup the GmatBase Get/Set methods to throw exceptions for invalid
 *       parameter accesses.
 */
bool GmatBase::GetBooleanParameter(const Integer id) const
{
    std::string errorText = "No Boolean data defined for the ID ";
    errorText += id;
    throw GmatBaseException(errorText);
}


//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const bool value)
//---------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
bool GmatBase::SetBooleanParameter(const Integer id, const bool value)
{
    return false;
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The parameter's value.
 */
Real GmatBase::GetRealParameter(const std::string &label) const
{
    Integer id = GetParameterID(label);
    return GetRealParameter(id);
}


//---------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or 
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the 
 *         parameter type is not Real.
 */
Real GmatBase::SetRealParameter(const std::string &label, const Real value)
{
    Integer id = GetParameterID(label);
    return SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
//  Real GetIntegerParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for an Integer parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The parameter's value.
 */
Integer GmatBase::GetIntegerParameter(const std::string &label) const
{
    Integer id = GetParameterID(label);
    return GetIntegerParameter(id);
}


//---------------------------------------------------------------------------
//  Integer SetIntegerParameter(const std::string &label, const Integer value)
//---------------------------------------------------------------------------
/**
 * Set the value for an Integer parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or 
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the 
 *         parameter type is not an Integer.
 */
Integer GmatBase::SetIntegerParameter(const std::string &label, 
                                      const Integer value)
{
    Integer id = GetParameterID(label);
    return SetIntegerParameter(id, value);
}

//---------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
std::string GmatBase::GetStringParameter(const std::string &label) const
{
    Integer id = GetParameterID(label);
    return GetStringParameter(id);
}

//---------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
bool GmatBase::SetStringParameter(const std::string &label, 
                                  const std::string &value)
{
    Integer id = GetParameterID(label);
    return SetStringParameter(id, value);
}



//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
StringArray& GmatBase::GetStringArrayParameter(const std::string &label) const
{
    Integer id = GetParameterID(label);
    return GetStringArrayParameter(id);
}


//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 *
 * @todo Setup the GmatBase Get/Set methods to throw exceptions for invalid
 *       parameter accesses.
 */
bool GmatBase::GetBooleanParameter(const std::string &label) const
{
    Integer id = GetParameterID(label);
    return GetBooleanParameter(id);
}


//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const std::string &label, const bool value)
//---------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
bool GmatBase::SetBooleanParameter(const std::string &label, const bool value)
{
    Integer id = GetParameterID(label);
    return SetBooleanParameter(id, value);
}


//---------------------------------------------------------------------------
//  void CopyParameters(const GmatBase &a)
//---------------------------------------------------------------------------
/**
 * Copies the parameters from one object into this object.
 *
 * @param <a> The object that is being copied.
 */
void GmatBase::CopyParameters(const GmatBase &a)
{
    Integer i, iVal;
    Real rVal;
    std::string sVal;
    bool bVal;
    Gmat::ParameterType parmType;
    
    for (i = 0; i < parameterCount; ++i) {
        parmType = a.GetParameterType(i);
        
        if (parmType == Gmat::REAL_TYPE) 
        {
            rVal = a.GetRealParameter(i);
            SetRealParameter(i, rVal);
        }
        
        if (parmType == Gmat::INTEGER_TYPE) 
        {
            iVal = a.GetIntegerParameter(i);
            SetIntegerParameter(i, iVal);
        }
        
        if (parmType == Gmat::STRING_TYPE)
        {
            sVal = a.GetStringParameter(i);
            SetStringParameter(i, sVal);
        }

        if (parmType == Gmat::BOOLEAN_TYPE)
        {
            bVal = a.GetBooleanParameter(i);
            SetBooleanParameter(i, bVal);
        }
    }    
}

