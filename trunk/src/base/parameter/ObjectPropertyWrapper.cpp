//$Header$
//------------------------------------------------------------------------------
//                                  ObjectPropertyWrapper
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan
// Created: 2007.01.18
//
/**
 * Implementation of the ObjectPropertyWrapper class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ObjectPropertyWrapper.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_RENAME_OBJ_PROP
//#define DEBUG_OBJ_PROP

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ObjectPropertyWrapper();
//---------------------------------------------------------------------------
/**
 * Constructs ObjectPropertyWrapper structures
 * (default constructor).
 *
 */
//---------------------------------------------------------------------------
ObjectPropertyWrapper::ObjectPropertyWrapper() :
   ElementWrapper(),
   object        (NULL),
   propID        (-1)
{
   wrapperType = Gmat::OBJECT_PROPERTY;
}

//---------------------------------------------------------------------------
//  ObjectPropertyWrapper(const ObjectPropertyWrapper &opw);
//---------------------------------------------------------------------------
/**
 * Constructs base ObjectPropertyWrapper structures used in derived classes, 
 * by copying the input instance (copy constructor).
 *
 * @param <opw>  ObjectPropertyWrapper instance to copy to create "this" 
 * instance.
 */
//---------------------------------------------------------------------------
ObjectPropertyWrapper::ObjectPropertyWrapper(const ObjectPropertyWrapper &opw) :
   ElementWrapper(opw),
   object        (opw.object),
   propIDNames   (opw.propIDNames),
   propID        (opw.propID)
{
}

//---------------------------------------------------------------------------
//  ObjectPropertyWrapper& operator=(const ObjectPropertyWrapper &opw)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ObjectPropertyWrapper structures.
 *
 * @param <opw> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ObjectPropertyWrapper& ObjectPropertyWrapper::operator=(
                             const ObjectPropertyWrapper &opw)
{
   if (&opw == this)
      return *this;

   ElementWrapper::operator=(opw);
   object      = opw.object;
   propIDNames = opw.propIDNames;
   propID      = opw.propID;

   return *this;
}

//---------------------------------------------------------------------------
//  ~ObjectPropertyWrapper()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ObjectPropertyWrapper::~ObjectPropertyWrapper()
{
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetDataType() const
//------------------------------------------------------------------------------
/**
 * This method returns the data type for the object property.
 *
 * @return data type for the object property.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ObjectPropertyWrapper::GetDataType() const
{
   if (object == NULL)
      throw ParameterException("The object is NULL, so cannot get data type");

   Gmat::ParameterType propType = object->GetParameterType(propID);
   
   #ifdef DEBUG_OBJ_PROP
   MessageInterface::ShowMessage
      ("ObjectPropertyWrapper::GetDataType() returning %d\n", propType);
   #endif
   
   return propType;
}

//---------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj)
//---------------------------------------------------------------------------
/**
 * Method to set the reference object (ObjectProperty) pointer on the wrapped 
 * object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetRefObject(GmatBase *obj)
{
//   if (obj->GetName() != refObjectNames[0])
//   {
//
//      errmsg += object->GetName();
//      errmsg += "\" was passed into the object property wrapper \"";
//      errmsg += description;
//      errmsg += "\", but an object named \"";
//      errmsg += refObjectNames[0];
//      errmsg += "\" was expected.\n";
//      throw ParameterException(errmsg);
//   }

   if (obj->GetName() == refObjectNames[0])
   {
      object = obj;
      propID = object->GetParameterID(propIDNames[0]);
      #ifdef DEBUG_OBJ_PROP
         MessageInterface::ShowMessage(
         "In ObjPropWrapper::SetRefObject, setting to object %s\n",
         obj->GetName().c_str());
         MessageInterface::ShowMessage("      and propID = %d\n", propID);
      #endif
      return true;
   }
   else return false;
}

//---------------------------------------------------------------------------
//  bool RenameObject(const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Method to rename a reference object for the wrapper.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::RenameObject(const std::string &oldName, 
                                         const std::string &newName)
{
   ElementWrapper::RenameObject(oldName, newName);
   // now rebuild the description string from the refObjectNames
   Integer pos = description.find(".");
   if (description.find(".") != std::string::npos)
   {
      #ifdef DEBUG_RENAME_OBJ_PROP
         MessageInterface::ShowMessage(
         "Found a dot at position %d in the description string %s\n",
         (Integer) pos, description.c_str());
      #endif
      description.replace(0,pos,refObjectNames[0]);
      #ifdef DEBUG_RENAME_OBJ_PROP
         MessageInterface::ShowMessage(
         "--- replacing with %s\n", (refObjectNames[0]).c_str());
         MessageInterface::ShowMessage(
         "--- and now description is %s\n", description.c_str());
      #endif
   }
   else // this shouldn't happen, but ...
   {
      std::string errmsg = 
         "Expecting a \".\" in the description for object property \"";
      errmsg += description + " \"";
      throw ParameterException(errmsg);
   }
   return true;
}

//---------------------------------------------------------------------------
//  Real EvaluateReal() const
//---------------------------------------------------------------------------
/**
 * Method to return the Real value of the wrapped object.
 *
 * @return Real value of the wrapped number object.
 * 
 */
//---------------------------------------------------------------------------
Real ObjectPropertyWrapper::EvaluateReal() const
{
   if (object == NULL)
      throw ParameterException(
      "Cannot return value of ObjectProperty - object pointer is NULL\n");
   Real itsValue;
   try
   {
      itsValue = object->GetRealParameter(propID);
      #ifdef DEBUG_OBJ_PROP
         MessageInterface::ShowMessage(
         "In ObjPropWrapper::EvaluateReal, value = %.12f\n", itsValue);
      #endif
   }
   catch (BaseException &be)
   {
      std::string errmsg = "Cannot return Real value for id \"" + propID; 
      errmsg += "\" for object \"" + object->GetName();
      errmsg += "\" - exception thrown: " + be.GetFullMessage();
      throw ParameterException(errmsg);
   }
   
   return itsValue;
}
   
//---------------------------------------------------------------------------
//  bool SetReal(const Real toValue)
//---------------------------------------------------------------------------
/**
 * Method to set the Real value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetReal(const Real toValue)
{
   if (object == NULL)
      throw ParameterException(
      "Cannot set value of ObjectProperty - object pointer is NULL\n");

   try
   {
      object->SetRealParameter(propID, toValue);
      #ifdef DEBUG_OBJ_PROP
         MessageInterface::ShowMessage(
         "In ObjPropWrapper::SetReal, value has been set to %.12f\n", toValue);
      #endif
   }
   catch (BaseException &be)
   {
      std::string errmsg = "Cannot set Real value for id \"" + propID; 
      errmsg += "\" for object \"" + object->GetName();
      errmsg += "\" - exception thrown: " + be.GetFullMessage();
      throw ParameterException(errmsg);
   }
   
   return true;
}

//---------------------------------------------------------------------------
// std::string EvaluateString() const
//---------------------------------------------------------------------------
std::string ObjectPropertyWrapper::EvaluateString() const
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::STRING_TYPE || propType == Gmat::ON_OFF_TYPE)
      return object->GetStringParameter(propID);
   else
      throw GmatBaseException
         ("EvaluateString() method not valid for wrapper of non-String type.\n");
}

//---------------------------------------------------------------------------
// bool SetString(const std::string &toValue)
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetString(const std::string &toValue)
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::STRING_TYPE ||
       propType == Gmat::ENUMERATION_TYPE ||
       propType == Gmat::STRINGARRAY_TYPE)
      return object->SetStringParameter(propID, toValue);
   else
      throw GmatBaseException
         ("SetString() method not valid for wrapper of non-String type.\n");
}

//---------------------------------------------------------------------------
// std::string EvaluateOnOff() const
//---------------------------------------------------------------------------
std::string ObjectPropertyWrapper::EvaluateOnOff() const
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::ON_OFF_TYPE)
      return object->GetOnOffParameter(propID);
   else
      throw GmatBaseException
         ("EvaluateOnOff() method not valid for wrapper of non-OnOff type.\n");
}

//---------------------------------------------------------------------------
// bool SetOnOff(const std::string &toValue)
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetOnOff(const std::string &toValue)
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::ON_OFF_TYPE)
      return object->SetOnOffParameter(propID, toValue);
   else
      throw GmatBaseException
         ("SetOnOff() method not valid for wrapper of non-OnOff type.\n");
}

//---------------------------------------------------------------------------
// bool EvaluateBoolean() const
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::EvaluateBoolean() const
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::BOOLEAN_TYPE)
      return object->GetBooleanParameter(propID);
   else
      throw GmatBaseException
         ("EvaluateBoolean() method not valid for wrapper of non-Boolean type.\n");
}

//---------------------------------------------------------------------------
// bool SetBoolean(const bool toValue)
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetBoolean(const bool toValue)
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::BOOLEAN_TYPE)
      return object->SetBooleanParameter(propID, toValue);
   else
      throw GmatBaseException
         ("SetBoolean() method not valid for wrapper of non-Boolean type.\n");
}

//---------------------------------------------------------------------------
// Integer EvaluateInteger() const
//---------------------------------------------------------------------------
Integer ObjectPropertyWrapper::EvaluateInteger() const
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::INTEGER_TYPE)
      return object->GetIntegerParameter(propID);
   else
      throw GmatBaseException
         ("EvaluateInteger() method not valid for wrapper of non-Integer type.\n");
}

//---------------------------------------------------------------------------
// bool SetInteger(const Integer toValue)
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetInteger(const Integer toValue)
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::INTEGER_TYPE)
      return object->SetIntegerParameter(propID, toValue);
   else
      throw GmatBaseException
         ("SetInteger() method not valid for wrapper of non-Integer type.\n");
}


//------------------------------------------------------------------------------
// bool SetObject(const GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Method to set the object of the wrapped object.
 *
 * @param <obj> The object pointer to set
 * @return true if successful; false otherwise.
 */
//------------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetObject(const GmatBase *obj)
{
   if (obj == NULL)
   {
      throw ParameterException
         ("Cannot set undefined object to object property \"" +
          GetDescription() +  "\"");         
   }
   
   #ifdef DEBUG_OBJ_PROP
   MessageInterface::ShowMessage
      ("ObjectPropertyWrapper::SetObject() obj=<%s>\n", obj->GetName().c_str());
   #endif
   
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::OBJECT_TYPE || propType == Gmat::OBJECTARRAY_TYPE)
      return object->SetStringParameter(propID, obj->GetName());
   else
      throw GmatBaseException
         ("SetObject() method not valid for wrapper of non-Object type.\n");
   
   return true;
}


//---------------------------------------------------------------------------
//  void SetupWrapper()
//---------------------------------------------------------------------------
/**
 * Method to set up the Object Property Wrapper.
 *
 */
//---------------------------------------------------------------------------
void ObjectPropertyWrapper::SetupWrapper()
{
   std::string type, owner, depobj;
   GmatStringUtil::ParseParameter(description, type, owner, depobj);
   //if (depobj != "")
   //{
   //   throw ParameterException(
   //   "Dependent objects not yet supported for the object property wrapper \"" +
   //   description + "\".\n");
   //   /// @todo Handle object properties that use dependent objects here
   //}
   if (owner == "")
   {
      throw ParameterException(
      "Owner object name is empty for the object property wrapper \"" +
      description + "\".\n");
   }
   if (type == "")
   {
      throw ParameterException(
      "Property ID string is empty for the object property wrapper \"" +
      description + "\".\n");
   }
   refObjectNames.push_back(owner);
   propIDNames.push_back(type);
   #ifdef DEBUG_OBJ_PROP
      MessageInterface::ShowMessage(
      "In ObjPropWrapper::SetupWrapper, owner = %s, type = %s\n", 
      owner.c_str(), type.c_str());
   #endif
}
