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
const UnsignedInt GmatBase::UNSIGNED_INT_PARAMETER_UNDEFINED = 987654321;
const std::string GmatBase::STRING_PARAMETER_UNDEFINED = "STRING_PARAMETER_UNDEFINED";
const Rvector     GmatBase::RVECTOR_PARAMETER_UNDEFINED = Rvector(1,
                  GmatBase::REAL_PARAMETER_UNDEFINED);
const Rmatrix     GmatBase::RMATRIX_PARAMETER_UNDEFINED = Rmatrix(1,1,
                  GmatBase::REAL_PARAMETER_UNDEFINED);

/// Build the list of type names
const std::string
GmatBase::PARAM_TYPE_STRING[Gmat::TypeCount] =
{
   "Integer",     "UnsignedInt", "UnsignedIntArray", "Real",       "String",
   "StringArray", "Boolean",     "Rvector",          "Rvector3",   "Rvector6",
   "Rmatrix",     "Rmatrix33",   "Cartesian",        "Keplerian",  "A1Mjd",
   "UtcDate",     "Object"
};

/// Build the list of object type names
const std::string
GmatBase::OBJECT_TYPE_STRING[Gmat::UNKNOWN_OBJECT - Gmat::SPACECRAFT+1] =
{
   "Spacecraft",    "Formation",     "SpaceObject",   "GroundStation",
   "Burn",          "Command",       "Propagator",    "ForceModel",
   "PhysicalModel", "Interpolator",  "SolarSystem",   "CelestialBody",
   "Atmosphere",    "Parameter",     "StopCondition", "Solver",
   "Subscriber",    "PropSetup",     "RefFrame",      "Function",
   "FuelTank",      "Thruster",      "Hardware",      "UnknownObject"
};


/// initialize the count of instances
Integer GmatBase::instanceCount = 0; 

//-------------------------------------
// public static methods
//-------------------------------------

//---------------------------------------------------------------------------
// std::string GetObjectTypeString(Gmat::ObjectType type)
//---------------------------------------------------------------------------
/**
 * @param <type> object type
 *
 * @return object type string for given type
 *
 */
//---------------------------------------------------------------------------
std::string GmatBase::GetObjectTypeString(Gmat::ObjectType type)
{
   return OBJECT_TYPE_STRING[type - Gmat::SPACECRAFT];
}

//---------------------------------------------------------------------------
// Gmat::ObjectType GetObjectType(const std::string &typeString)
//---------------------------------------------------------------------------
/**
 * @param <typeString> object type string
 *
 * @return object type for given type string
 *
 */
//---------------------------------------------------------------------------
Gmat::ObjectType GmatBase::GetObjectType(const std::string &typeString)
{
   for (int i=0; i<Gmat::UNKNOWN_OBJECT - Gmat::SPACECRAFT; i++)
      if (OBJECT_TYPE_STRING[i] == typeString)
         return (Gmat::ObjectType)i;

   return Gmat::UNKNOWN_OBJECT;
}

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
   parameterCount  (GmatBaseParamCount),
   typeName        (typeStr),
   instanceName    (nomme),
   type            (typeId),
   ownedObjectCount(0)
{
   // one more instance - add to the instanceCount
   instanceCount++;
}


//---------------------------------------------------------------------------
//  ~GmatBase(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
GmatBase::~GmatBase(void)
{
   // subtract this instance from the instanceCount
   instanceCount--;
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
//    instanceName    ("CopyOf"+a.instanceName),
    instanceName    (a.instanceName),
    type            (a.type),
    ownedObjectCount(a.ownedObjectCount)
{
   // one more instance - add to the instanceCount
   instanceCount++;
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
//  std::string GetRefObjectName(const Gmat::ObjectType type) const
//---------------------------------------------------------------------------
/**
 * Returns the name of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.
 *
 * @return The name of the reference object.
 */
std::string GmatBase::GetRefObjectName(const Gmat::ObjectType type) const
{
   throw GmatBaseException("Reference Object not defined\n");
}

//---------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//---------------------------------------------------------------------------
/**
 * Returns the names of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.
 *
 * @return The names of the reference object.
 */
const StringArray& GmatBase::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   throw GmatBaseException("Reference Object not defined\n");
}

//---------------------------------------------------------------------------
//  bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name) 
//---------------------------------------------------------------------------
/**
 * Sets the name of the reference object.  (Derived classes should implement
 * this as needed.)
 *
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return success of the operation.
 */
bool GmatBase::SetRefObjectName(const Gmat::ObjectType type,
                                const std::string &name)
{
   char errortext[256];
   std::sprintf(errortext, "Reference Object \"%s\" not defined", name.c_str());
   throw GmatBaseException(errortext);
}

//---------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name)
//---------------------------------------------------------------------------
/**
 * Returns the reference object pointer.  (Derived classes should implement
 * this method as needed.)
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return reference object pointer.
 */
GmatBase* GmatBase::GetRefObject(const Gmat::ObjectType type,
                                 const std::string &name)
{
   char errortext[256];
   std::sprintf(errortext, "Reference Object \"%s\" not defined", name.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name,
//                        const Integer index)
//---------------------------------------------------------------------------
/**
 * Returns the reference object pointer.  (Derived classes should implement
 * this method as needed.)
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 * @param index Index into the object array.
 * 
 * @return reference object pointer.
 */
GmatBase* GmatBase::GetRefObject(const Gmat::ObjectType type,
                                 const std::string &name, const Integer index)
{
   char errortext[256];
   std::sprintf(errortext, "Reference Object \"%s\" with index %d not defined", 
           name.c_str(), index);
   throw GmatBaseException(errortext);
}

//---------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name)
//---------------------------------------------------------------------------
/**
 * Sets the reference object.  (Derived classes should implement
 * this method as needed.)
 *
 * @param <obj> reference object pointer.
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return success of the operation.
 */
bool GmatBase::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                            const std::string &name)
{
   char errortext[256];
   std::sprintf(errortext, "Cannot set reference object \"%s\" on object %s" , 
           name.c_str(), instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name, const Integer index)
//---------------------------------------------------------------------------
/**
 * Sets the reference object.  (Derived classes should implement
 * this method as needed.)
 *
 * @param obj reference object pointer.
 * @param type type of the reference object.
 * @param name name of the reference object.
 * @param index Index into the object array.
 *
 * @return success of the operation.
 */
bool GmatBase::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                            const std::string &name, const Integer index)
{
   char errortext[256];
   std::sprintf(errortext, "Cannot set reference object \"%s\" on %s", name.c_str(), 
           instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//---------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers by type.
 * 
 * @param type The type of objects requested
 *
 * @return Reference to the array.  This default method returns an empty vector.
 */
ObjectArray& GmatBase::GetRefObjectArray(const Gmat::ObjectType type)
{
   static ObjectArray oa;
   oa.clear();
   return oa;
}


//---------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//---------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers based on a string (e.g. the typename).
 * 
 * @param typeString The string used to find the objects requested.
 *
 * @return Reference to the array.  This default method returns an empty vector.
 */
ObjectArray& GmatBase::GetRefObjectArray(const std::string& typeString)
{
   return GetRefObjectArray(Gmat::UNKNOWN_OBJECT);
}


//---------------------------------------------------------------------------
//  Integer GetOwnedObjectCount()
//---------------------------------------------------------------------------
/**
 * Find out how many GmatBase objects belong to this instance.
 *
 * This method is used find out how many unnamed objects belong to the current 
 * instance.  The Interpreter subsystem used this method to count these objects,
 * so that it can access them one by one for reading and writing.  
 * 
 * One example of this use is the PropSetup class; each PropSetup contains an 
 * unnamed Integrator.  The Integrator parameters are accessed for reading and
 * writing scripts. This method determines how many objects are available for 
 * access.  The objects are then accessed using GetOwnedObject(Integer).  The 
 * input parameter to GetOwnedObject method is found by calling this method, 
 * and then looping through all of the owned objects, accessing the related 
 * parameters for each.
 * 
 * @return The number of owned objects.
 */
Integer GmatBase::GetOwnedObjectCount()
{
   return ownedObjectCount;
}


//---------------------------------------------------------------------------
//  static Integer GetInstanceCount()
//---------------------------------------------------------------------------
/**
 * Access GmatBase objects belonging to this instance.
 *
 * This method is used to access the unnamed objects owned by a class derived 
 * from GmatBase.  The Interpreter subsystem used this method to access 
 * parameters for these objects.  
 * 
 * One example of this use is the PropSetup class; each PropSetup contains an 
 * unnamed Integrator.  The Integrator parameters are accessed for reading and
 * writing scripts using this method.  
 * 
 * The input parameter to GetOwnedObject an integer that identifies which owned
 * object is requested.
 * 
 * @return Pointer to the owned object.
 */
GmatBase* GmatBase::GetOwnedObject(Integer whichOne)
{
   throw GmatBaseException("No owned objects for this instance\n");
}


//---------------------------------------------------------------------------
//  static Integer GetInstanceCount()
//---------------------------------------------------------------------------
/**
 * Find out how many GmatBase objects have been instantiated.
 *
 * @return The number of instantiated objects.
 */
Integer GmatBase::GetInstanceCount()
{
   return GmatBase::instanceCount;
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
   char errortext[256];
   std::sprintf(errortext, "Parameter id = %d not defined on object %s\n", id, 
           instanceName.c_str());
   throw GmatBaseException(errortext);
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
   char errortext[256];
   std::string desc;
   desc = instanceName;
   if (desc == "")
      desc = typeName;
   sprintf(errortext, "No parameter defined with description \"%s\" on %s\n", 
           str.c_str(), desc.c_str());
   throw GmatBaseException(errortext);
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
   char errortext[256];
   sprintf(errortext, "No real parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
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
   char errortext[256];
   sprintf(errortext, "No real parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id, Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for objecs in arrays.
 *
 * @return The parameter's value.
 */
Real GmatBase::GetRealParameter(const Integer id, const Integer index) const
{
   char errortext[256];
   sprintf(errortext, "No real parameter with ID %d and index %d on %s\n", 
           id, index, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value, Integer index)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or 
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the 
 *         parameter type is not Real.
 */
Real GmatBase::SetRealParameter(const Integer id, const Real value, 
                                const Integer index)
{
   char errortext[256];
   sprintf(errortext, "No real parameter with ID %d and index %d on %s\n", 
           id, index, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  Integer GetIntegerParameter(const Integer id) const
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
   char errortext[256];
   sprintf(errortext, "No integer parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
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
   char errortext[256];
   sprintf(errortext, "No integer parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
}

//---------------------------------------------------------------------------
//  Integer GetIntegerParameter(const Integer id, const Integer index) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Integer parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for objecs in arrays.
 *
 * @return The parameter's value.
 */
Integer GmatBase::GetIntegerParameter(const Integer id, 
                                      const Integer index) const
{
   char errortext[256];
   sprintf(errortext, "No integer parameter with ID %d and index %d on %s\n", 
           id, index, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  Integer SetIntegerParameter(const Integer id, const Integer value, 
//                              const Integer index)
//---------------------------------------------------------------------------
/**
* Set the value for an Integer parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not an Integer.
 */
Integer GmatBase::SetIntegerParameter(const Integer id, const Integer value, 
                                      const Integer index)
{
   char errortext[256];
   sprintf(errortext, "Cannot set integer parameter with ID %d and index %d on %s\n", 
           id, index, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  UnsignedInt GetUnsignedIntParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for an UnsignedInt parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
UnsignedInt GmatBase::GetUnsignedIntParameter(const Integer id) const
{
   char errortext[256];
   sprintf(errortext, "No unsigned integer parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  UnsignedInt SetUnsignedIntParameter(const Integer id,
//                                      const UnsignedInt value)
//---------------------------------------------------------------------------
/**
 * Set the value for an UnsignedInt parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         UNSIGNED_INT_PARAMETER_UNDEFINED if the parameter id is invalid or
 *         the parameter type is not an UnsignedInt.
 */
UnsignedInt GmatBase::SetUnsignedIntParameter(const Integer id,
                                              const UnsignedInt value)
{
   char errortext[256];
   sprintf(errortext, "Cannot set unsigned integer parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  UnsignedInt GetUnsignedIntParameter(const Integer id, const Integer index) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an UnsignedInt parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The parameter's value.
 */
UnsignedInt GmatBase::GetUnsignedIntParameter(const Integer id, 
                                              const Integer index) const
{
   char errortext[256];
   sprintf(errortext, "No unsigned integer parameter with ID %d and index %d on %s\n", 
           id, index, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  UnsignedInt SetUnsignedIntParameter(const Integer id,
//                                      const UnsignedInt value, 
//                                      const Integer index)
//---------------------------------------------------------------------------
/**
* Set the value for an UnsignedInt parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         UNSIGNED_INT_PARAMETER_UNDEFINED if the parameter id is invalid or
 *         the parameter type is not an UnsignedInt.
 */
UnsignedInt GmatBase::SetUnsignedIntParameter(const Integer id,
                                              const UnsignedInt value,
                                              const Integer index)
{
   char errortext[256];
   sprintf(errortext, "Cannot set unsigned integer parameter with ID %d and index %d on %s\n", 
           id, index, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  const UnsignedIntArray& GetUnsignedIntArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of unsigned int data.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested UnsignedIntArray; throws if the parameter is not a 
 *         UnsignedIntArray.
 */
const UnsignedIntArray& GmatBase::GetUnsignedIntArrayParameter(const Integer id) const
{
   char errortext[256];
   sprintf(errortext, "No unsigned int array parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  const Rvector& GetRvectorParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Rvector parameter.
*
* @param <id> The integer ID for the parameter.
*
* @return The parameter's value.
*/
const Rvector& GmatBase::GetRvectorParameter(const Integer id) const
{
   char errortext[256];
   sprintf(errortext, "No RVector parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  const Rvector& SetRvectorParameter(const Integer id, const Rvector& value)
//---------------------------------------------------------------------------
/**
* Set the value for an Rvector parameter.
*
* @param <id> The integer ID for the parameter.
* @param <value> The new parameter value.
*
* @return the parameter value at the end of this call, or 
*         RVECTOR_PARAMETER_UNDEFINED if the parameter id is invalid or the 
*         parameter type is not Rvector.
*/
const Rvector& GmatBase::SetRvectorParameter(const Integer id,
                                             const Rvector& value)
{
   char errortext[256];
   sprintf(errortext, "Cannot set RVector parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
}

//---------------------------------------------------------------------------
//  const Rmatrix& GetRmatrixParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Rmatrix parameter.
*
* @param <id> The integer ID for the parameter.
*
* @return The parameter's value.
*/
const Rmatrix& GmatBase::GetRmatrixParameter(const Integer id) const
{
   char errortext[256];
   sprintf(errortext, "No Rmatrix parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  const Rmatrix& SetRmatrixParameter(const Integer id, const Rmatrix& value)
//---------------------------------------------------------------------------
/**
* Set the value for an Rmatrix parameter.
*
* @param <id> The integer ID for the parameter.
* @param <value> The new parameter value.
*
* @return the parameter value at the end of this call, or 
*         RMATRIX_PARAMETER_UNDEFINED if the parameter id is invalid or the 
*         parameter type is not Rmatrix.
*/
const Rmatrix& GmatBase::SetRmatrixParameter(const Integer id,
                                             const Rmatrix& value)
{
   return RMATRIX_PARAMETER_UNDEFINED;
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
   char errortext[256];
   sprintf(errortext, "No string parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
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
   char errortext[256];
   sprintf(errortext, "Cannot set string parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
std::string GmatBase::GetStringParameter(const Integer id, 
                                         const Integer index) const
{
   char errortext[256];
   sprintf(errortext, "No string parameter with ID %d and index %d on %s\n", 
           id, index, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value, 
//                          const Integer index)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new string for this parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return true if the string is stored, false if not.
 */
bool GmatBase::SetStringParameter(const Integer id, const std::string &value, 
                                  const Integer index)
{
   char errortext[256];
   sprintf(errortext, "Cannot set string parameter with ID %d and index %d on %s\n", 
           id, index, instanceName.c_str());
   throw GmatBaseException(errortext);
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
const StringArray& GmatBase::GetStringArrayParameter(const Integer id) const
{
   char errortext[256];
   sprintf(errortext, "No string array parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id, 
//                                             const Integer index) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 * @param index The index when multiple StringArrays are supported.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
const StringArray& GmatBase::GetStringArrayParameter(const Integer id, 
                                               const Integer index) const
{
   char errortext[256];
   std::string me = instanceName;
   if (instanceName == "")
      me = typeName;
   sprintf(errortext, "No indexed string array parameter with ID %d on %s\n", 
           id, me.c_str());
   throw GmatBaseException(errortext);
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
   char errortext[256];
   sprintf(errortext, "No boolean parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const bool value)
//---------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new value.
 * 
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
bool GmatBase::SetBooleanParameter(const Integer id, const bool value)
{
   char errortext[256];
   sprintf(errortext, "Cannot set boolean parameter with ID %d on %s\n", 
           id, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id, const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 *
 * @todo Setup the GmatBase Get/Set methods to throw exceptions for invalid
 *       parameter accesses.
 */
bool GmatBase::GetBooleanParameter(const Integer id, const Integer index) const
{
   char errortext[256];
   sprintf(errortext, "No boolean parameter with ID %d and index %d on %s\n", 
           id, index, instanceName.c_str());
   throw GmatBaseException(errortext);
}


//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const bool value, 
//                           const Integer index)
//---------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param value The new value for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
bool GmatBase::SetBooleanParameter(const Integer id, const bool value, 
                                   const Integer index)
{
   char errortext[256];
   sprintf(errortext, "Cannot set boolean parameter with ID %d and index %d on %s\n", 
           id, index, instanceName.c_str());
   throw GmatBaseException(errortext);
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
//  Real GetRealParameter(const std::string &label, const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param label The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The parameter's value.
 */
Real GmatBase::GetRealParameter(const std::string &label, 
                                const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetRealParameter(id, index);
}


//---------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value, 
//                        const Integer index)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or 
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the 
 *         parameter type is not Real.
 */
Real GmatBase::SetRealParameter(const std::string &label, const Real value, 
                                const Integer index)
{
   Integer id = GetParameterID(label);
   return SetRealParameter(id, value, index);
}


//---------------------------------------------------------------------------
//  Integer GetIntegerParameter(const std::string &label) const
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
//  Integer GetIntegerParameter(const std::string &label,
//                              const Integer index) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Integer parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The parameter's value.
 */
Integer GmatBase::GetIntegerParameter(const std::string &label,
                                      const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetIntegerParameter(id, index);
}


//---------------------------------------------------------------------------
//  Integer SetIntegerParameter(const std::string &label, const Integer value,
//                              const Integer index)
//---------------------------------------------------------------------------
/**
* Set the value for an Integer parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not an Integer.
 */
Integer GmatBase::SetIntegerParameter(const std::string &label,
                                      const Integer value,
                                      const Integer index)
{
   Integer id = GetParameterID(label);
   return SetIntegerParameter(id, value, index);
}

//---------------------------------------------------------------------------
//  UnsignedInt GetUnsignedIntParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for an UnsignedInt parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The parameter's value.
 */
UnsignedInt GmatBase::GetUnsignedIntParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetUnsignedIntParameter(id);
}


//---------------------------------------------------------------------------
//  UnsignedInt SetUnsignedIntParameter(const std::string &label,
//                                      const UnsignedInt value)
//---------------------------------------------------------------------------
/**
* Set the value for an UnsignedInt parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         UNSIGNED_INT_PARAMETER_UNDEFINED if the parameter id is invalid
 *         or the parameter type is not an UnsignedInt.
 */
UnsignedInt GmatBase::SetUnsignedIntParameter(const std::string &label,
                                              const UnsignedInt value)
{
   Integer id = GetParameterID(label);
   return SetUnsignedIntParameter(id, value);
}

//---------------------------------------------------------------------------
//  UnsignedInt GetUnsignedIntParameter(const std::string &label, 
//                                      const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for an UnsignedInt parameter.
 *
 * @param label The (string) label for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return The parameter's value.
 */
UnsignedInt GmatBase::GetUnsignedIntParameter(const std::string &label, 
                                              const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetUnsignedIntParameter(id, index);
}


//---------------------------------------------------------------------------
//  UnsignedInt SetUnsignedIntParameter(const std::string &label,
//                                      const UnsignedInt value,
//                                      const Integer index)
//---------------------------------------------------------------------------
/**
* Set the value for an UnsignedInt parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         UNSIGNED_INT_PARAMETER_UNDEFINED if the parameter id is invalid
 *         or the parameter type is not an UnsignedInt.
 */
UnsignedInt GmatBase::SetUnsignedIntParameter(const std::string &label,
                                              const UnsignedInt value,
                                              const Integer index)
{
   Integer id = GetParameterID(label);
   return SetUnsignedIntParameter(id, value, index);
}

//---------------------------------------------------------------------------
//  const UnsignedIntArray&
//  GetUnsignedIntArrayParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Access an array of unsigned int data.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The requested UnsignedIntArray; throws if the parameter is not a 
 *         UnsignedIntArray.
 */
const UnsignedIntArray&
GmatBase::GetUnsignedIntArrayParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetUnsignedIntArrayParameter(id);
}

//---------------------------------------------------------------------------
// const  Rvector& GetRvectorParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Rvector parameter.
*
* @param <label> The (string) label for the parameter.
*
* @return The parameter's value.
*/
const Rvector& GmatBase::GetRvectorParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetRvectorParameter(id);
}


//---------------------------------------------------------------------------
//  const Rvector& SetRvectorParameter(const std::string &label,
//                                     const Rvector& value)
//---------------------------------------------------------------------------
/**
* Set the value for an Rvector parameter.
*
* @param <label> The (string) label for the parameter.
* @param <value> The new parameter value.
*
* @return the parameter value at the end of this call, or 
*         RVECTOR_PARAMETER_UNDEFINED if the parameter id is invalid or the 
*         parameter type is not Rvector.
*/
const Rvector& GmatBase::SetRvectorParameter(const std::string &label,
                                             const Rvector& value)
{
   Integer id = GetParameterID(label);
   return SetRvectorParameter(id, value);
}

//---------------------------------------------------------------------------
//  const Rmatrix& GetRmatrixParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Rmatrix parameter.
*
* @param <label> The (string) label for the parameter.
*
* @return The parameter's value.
*/
const Rmatrix& GmatBase::GetRmatrixParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetRmatrixParameter(id);
}


//---------------------------------------------------------------------------
//  const Rmatrix& SetRmatrixParameter(const std::string &label,
//                                     const Rmatrix& value)
//---------------------------------------------------------------------------
/**
* Set the value for an Rmatrix parameter.
*
* @param <label> The (string) label for the parameter.
* @param <value> The new parameter value.
*
* @return the parameter value at the end of this call, or 
*         RMATRIX_PARAMETER_UNDEFINED if the parameter id is invalid or the 
*         parameter type is not Rmatrix.
*/
const Rmatrix& GmatBase::SetRmatrixParameter(const std::string &label,
                                             const Rmatrix& value)
{
   Integer id = GetParameterID(label);
   return SetRmatrixParameter(id, value);
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
//  std::string GetStringParameter(const std::string &label,
//                                 const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
std::string GmatBase::GetStringParameter(const std::string &label,
                                         const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetStringParameter(id, index);
}

//---------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value,
//                          const Integer index)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new string for this parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return true if the string is stored, false if not.
 */
bool GmatBase::SetStringParameter(const std::string &label, 
                                  const std::string &value,
                                  const Integer index)
{
   Integer id = GetParameterID(label);
   return SetStringParameter(id, value, index);
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param label The (string) label for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
const StringArray& GmatBase::GetStringArrayParameter(
                                          const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetStringArrayParameter(id);
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const std::string &label, 
//                                             const Integer index) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param label The (string) label for the parameter.
 * @param index Which string array if more than one is supported.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
const StringArray& GmatBase::GetStringArrayParameter(const std::string &label, 
                                               const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetStringArrayParameter(id, index);
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
//  bool GetBooleanParameter(const std::string &label, const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param index Index for parameters in arrays.  
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 *
 * @todo Setup the GmatBase Get/Set methods to throw exceptions for invalid
 *       parameter accesses.
 */
bool GmatBase::GetBooleanParameter(const std::string &label, 
                                   const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetBooleanParameter(id, index);
}


//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const std::string &label, const bool value)
//---------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
bool GmatBase::SetBooleanParameter(const std::string &label, const bool value, 
                                   const Integer index)
{
   Integer id = GetParameterID(label);
   return SetBooleanParameter(id, value, index);
}


//---------------------------------------------------------------------------
//  bool TakeAction(const std::string &action, const std::string &actionData)
//---------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <action> The string descriptor for the requested action.
 * @param <actionData> Optional data used for the action.
 * 
 * @return true if the action was performed, false if not.
 */
bool GmatBase::TakeAction(const std::string &action, 
                          const std::string &actionData)
{
   return false;
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

