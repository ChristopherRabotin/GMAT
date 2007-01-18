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

//#include "MessageInterface.hpp"

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ObjectPropertyWrapper(const std::string &desc);
//---------------------------------------------------------------------------
/**
 * Constructs ObjectPropertyWrapper structures
 * (default constructor).
 *
 * @param <desc> Optional description for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
ObjectPropertyWrapper::ObjectPropertyWrapper(const std::string &desc) :
   ElementWrapper(desc),
   object        (NULL),
   parameterId   (-1)
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
   parameterId   (opw.parameterId)
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
   parameterId = opw.parameterId;

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
//  bool SetObjectAndId(ObjectProperty *toVar)
//---------------------------------------------------------------------------
/**
 * Method to set the ObjectProperty pointer to the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetObjectAndId(GmatBase *toObj, Integer andId)
{
   object      = toObj;
   parameterId = andId;
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
      itsValue = object->GetRealParameter(parameterId);
   }
   catch (BaseException &be)
   {
      std::string errmsg = "Cannot return Real value for id " + parameterId; 
      errmsg += " for object " + object->GetName() + " - exception thrown: " +
      be.GetMessage();
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
      object->SetRealParameter(parameterId, toValue);
   }
   catch (BaseException &be)
   {
      std::string errmsg = "Cannot set Real value for id " + parameterId; 
      errmsg += " for object " + object->GetName() + " - exception thrown: " +
      be.GetMessage();
      throw ParameterException(errmsg);
   }
         
   return true;
}
