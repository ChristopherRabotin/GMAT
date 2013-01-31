//$Id$
//------------------------------------------------------------------------------
//                                  ArrayWrapper
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan
// Created: 2007.04.04
//
/**
 * Implementation of the ArrayWrapper class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ArrayWrapper.hpp"
#include "ParameterException.hpp"
#include "RealUtilities.hpp"
#include "StringUtil.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ARRAY_WRAPPER

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ArrayWrapper();
//---------------------------------------------------------------------------
/**
 * Constructs ArrayWrapper structures
 * (default constructor).
 *
 */
//---------------------------------------------------------------------------
ArrayWrapper::ArrayWrapper() :
   ElementWrapper(),
   array         (NULL),
   arrayName     ("")
{
   wrapperType = Gmat::ARRAY_WT;
}

//---------------------------------------------------------------------------
//  ArrayWrapper(const ArrayWrapper &aw);
//---------------------------------------------------------------------------
/**
 * Constructs base ArrayWrapper structures used in derived classes, 
 * by copying the input instance (copy constructor).
 *
 * @param <aw>  ArrayWrapper instance to copy to create "this" 
 * instance.
 */
//---------------------------------------------------------------------------
ArrayWrapper::ArrayWrapper(const ArrayWrapper &aw) :
   ElementWrapper(aw),
   array         (aw.array),
   arrayName     (aw.arrayName)
{
}

//---------------------------------------------------------------------------
//  ArrayWrapper& operator=(const ArrayWrapper &aw)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ArrayWrapper structures.
 *
 * @param <aw> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ArrayWrapper& ArrayWrapper::operator=(const ArrayWrapper &aw)
{
   if (&aw == this)
      return *this;
   
   ElementWrapper::operator=(aw);
   array        = aw.array;  
   arrayName    = aw.arrayName;

   return *this;
}

//---------------------------------------------------------------------------
//  ~ArrayWrapper()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ArrayWrapper::~ArrayWrapper()
{
}


//------------------------------------------------------------------------------
//  ElementWrapper* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new object that l.ooks like this one.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
ElementWrapper* ArrayWrapper::Clone() const
{
   return new ArrayWrapper(*this);
}


//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return ArrayWrapper value converted to std::string.
 *
 * @exception <GmatBaseException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
std::string ArrayWrapper::ToString()
{
   return array->ToString();
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetDataType() const
//------------------------------------------------------------------------------
/**
 * This method returns the data type for the ArrayWrapper object.
 *
 * @return data type for the object.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ArrayWrapper::GetDataType() const
{
   return Gmat::RMATRIX_TYPE;
}

//------------------------------------------------------------------------------
//  const StringArray&  GetRefObjectNames() 
//------------------------------------------------------------------------------
/**
 * This method returns the list of reference object names for the ElementWrapper 
 * object.
 *
 * @return list of reference object names for the object.
 *
 */
//------------------------------------------------------------------------------
const StringArray& ArrayWrapper::GetRefObjectNames()
{
   refObjectNames.clear();
   // start with the array name ...
   refObjectNames.push_back(arrayName);
   
   #ifdef DEBUG_ARRAY_WRAPPER
      MessageInterface::ShowMessage("ArrayWrapper:: Returning ref object names:\n");
      for (Integer ii = 0; ii < (Integer) refObjectNames.size(); ii++)
         MessageInterface::ShowMessage("      %s\n", refObjectNames[ii].c_str());
   #endif
   
   return refObjectNames;
}

//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * This method retrives a reference object for the wrapper name
 * 
 * @param <name> name of the wrapper
 *
 * @return reference for success; NULL if name not found
 *
 */
//------------------------------------------------------------------------------
GmatBase* ArrayWrapper::GetRefObject(const std::string &name)
{
   return array;
}


//---------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj)
//---------------------------------------------------------------------------
/**
 * Method to set the reference object (ObjectProperty) pointer on the wrapped 
 * object.
 * 
 * @return true if successful; false otherwise.
 *
 * @note The description must be set before setting the ref object because
 *       it's checking for arrayName
 */
//---------------------------------------------------------------------------
bool ArrayWrapper::SetRefObject(GmatBase *obj)
{
   #ifdef DEBUG_ARRAY_WRAPPER
   MessageInterface::ShowMessage
      ("ArrayWrapper::SetRefObject() obj=<%p>, type='%s', name='%s', "
       "arrayName='%s'\n", obj, obj->GetTypeName().c_str(), obj->GetName().c_str(),
       arrayName.c_str());
   #endif
   
   bool isOk   = false;
   if ( (obj->IsOfType("Array")) && (obj->GetName() == arrayName) )
   {
      array = (Array*) obj;
      #ifdef DEBUG_ARRAY_WRAPPER
         MessageInterface::ShowMessage("ArrayWrapper:: Setting array object %s\n",
            arrayName.c_str());
      #endif
      isOk = true;
   }
   
   if (!isOk) return false;
   #ifdef DEBUG_ARRAY_WRAPPER
      MessageInterface::ShowMessage(
         "ArrayWrapper:: Returning true from SetRefObject\n");
   #endif
   
   return true;
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
bool ArrayWrapper::RenameObject(const std::string &oldName, 
                                       const std::string &newName)
{
   ElementWrapper::RenameObject(oldName, newName);
   // now rebuild the description string from the refObjectNames
   // first, rename things in the row and column wrappers
   // then put it all back together
   arrayName   = refObjectNames[0];
   description = arrayName;
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
 * @note This method is not value for Arrays.
 * 
 */
//---------------------------------------------------------------------------
Real ArrayWrapper::EvaluateReal() const
{
   if (array->GetRowCount() == 1 && array->GetColCount() == 1)
      return array->GetRealParameter("SingleValue", 0, 0);
   else
      throw GmatBaseException(
         "ArrayWrapper::EvaluateReal() method not valid for wrapper of Array type.\n");
}

//---------------------------------------------------------------------------
//  bool SetReal(const Real toValue)
//---------------------------------------------------------------------------
/**
 * Method to set the Real value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 * 
 * @note This method is not value for Arrays.
 */
//---------------------------------------------------------------------------
bool ArrayWrapper::SetReal(const Real toValue)
{
   throw GmatBaseException(
      "SetReal() method not valid for wrapper of Array type.\n");
}

//---------------------------------------------------------------------------
//  const Rmatrix& EvaluateArray() const
//---------------------------------------------------------------------------
/**
 * Method to return the Array (Rmatrix) value of the wrapped object.
 *
 * @return Real value of the wrapped Array object.
 * 
 */
//---------------------------------------------------------------------------
const Rmatrix& ArrayWrapper::EvaluateArray() const
{
   #ifdef DEBUG_ARRAY_WRAPPER
      MessageInterface::ShowMessage(
         "ArrayWrapper::EvaluateArray called on array %s\n", 
         arrayName.c_str());
   #endif
   if (array == NULL)
      throw ParameterException(
      "Cannot return value of Array - object pointer is NULL\n");
   return array->EvaluateRmatrix();
}

//---------------------------------------------------------------------------
//  bool SetArray(const Rmatrix &toValue) const
//---------------------------------------------------------------------------
/**
 * Method to return the Array (Rmatrix) value of the wrapped object.
 *
 * @return bool true if successful; otherwise, false.
 * 
 */
//---------------------------------------------------------------------------
bool ArrayWrapper::SetArray(const Rmatrix &toValue)
{
   #ifdef DEBUG_ARRAY_WRAPPER
      MessageInterface::ShowMessage(
         "ArrayWrapper::SetArray called on array %s\n", 
         arrayName.c_str());
   #endif
   if (array == NULL)
      throw ParameterException(
      "Cannot set value of Array - object pointer is NULL\n");
   try
   {
      array->SetRmatrix(toValue);
   }
   catch (BaseException &be)
   {
      std::string errmsg = "Error setting Array value : " + be.GetFullMessage();
      errmsg += "\n";
      throw ParameterException(errmsg);
   }
   return true;
}

//---------------------------------------------------------------------------
//  void SetupWrapper()
//---------------------------------------------------------------------------
/**
 * Method to set up the Array Element Wrapper.
 *
 */
//---------------------------------------------------------------------------
void ArrayWrapper::SetupWrapper()
{
   #ifdef DEBUG_ARRAY_WRAPPER
   MessageInterface::ShowMessage
      ("ArrayWrapper::SetupWrapper() description='%s'\n", description.c_str());
   #endif
   
   arrayName = description;
   // for now, put the array name in the list of reference objects
   refObjectNames.push_back(arrayName);
}
