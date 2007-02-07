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
   if (obj->GetName() != refObjectNames[0])
   {
      std::string errmsg = "Referenced object \"";
      errmsg += object->GetName();
      errmsg += "\" was passed into the variable wrapper \"";
      errmsg += description;
      errmsg += "\", but an object named \"";
      errmsg += refObjectNames[0];
      errmsg += "\" was expected.\n";
      throw ParameterException(errmsg);
   }
   object = obj;
   propID = object->GetParameterID(propIDNames[0]);
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
   }
   catch (BaseException &be)
   {
      std::string errmsg = "Cannot return Real value for id \"" + propID; 
      errmsg += "\" for object \"" + object->GetName();
      errmsg += "\" - exception thrown: " + be.GetMessage();
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
   }
   catch (BaseException &be)
   {
      std::string errmsg = "Cannot set Real value for id \"" + propID; 
      errmsg += "\" for object \"" + object->GetName();
      errmsg += "\" - exception thrown: " + be.GetMessage();
      throw ParameterException(errmsg);
   }
         
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
}
