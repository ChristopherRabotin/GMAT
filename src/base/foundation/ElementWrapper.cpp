//$Header$
//------------------------------------------------------------------------------
//                                  ElementWrapper
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan
// Created: 2007.01.17
//
/**
 * Implementation of the ElementWrapper class.  This is the base class for wrappers 
 * for various element types.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElementWrapper.hpp"

//#define DEBUG_RENAME

#ifdef DEBUG_RENAME
   #include "MessageInterface.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
const Real ElementWrapper::UNDEFINED_REAL = -999.99;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ElementWrapper();
//---------------------------------------------------------------------------
/**
 * Constructs base ElementWrapper structures used in derived classes
 * (default constructor).
 *
 */
//---------------------------------------------------------------------------
ElementWrapper::ElementWrapper() :
   description  (""),
   wrapperType  (Gmat::NUMBER)
{
}

//---------------------------------------------------------------------------
//  ElementWrapper(const ElementWrapper &er);
//---------------------------------------------------------------------------
/**
 * Constructs base ElementWrapper structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param <er>  ElementWrapper instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
ElementWrapper::ElementWrapper(const ElementWrapper &er) :
   description    (er.description),
   refObjectNames (er.refObjectNames),
   wrapperType    (er.wrapperType)
{
}

//---------------------------------------------------------------------------
//  ElementWrapper& operator=(const ElementWrapper &er)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ElementWrapper structures.
 *
 * @param <er> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ElementWrapper& ElementWrapper::operator=(const ElementWrapper &er)
{
   if (&er == this)
      return *this;

   description    = er.description;
   refObjectNames = er.refObjectNames;
   wrapperType    = er.wrapperType;

   return *this;
}
//---------------------------------------------------------------------------
//  ~ElementWrapper()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ElementWrapper::~ElementWrapper()
{
   refObjectNames.clear();
}

//------------------------------------------------------------------------------
//  void SetDescription(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Sets the description for the ElementWrapper object.
 */
//------------------------------------------------------------------------------
void ElementWrapper::SetDescription(const std::string &str)
{
   description = str;
   SetupWrapper();
}



//------------------------------------------------------------------------------
//  std::string  GetDescription() const
//------------------------------------------------------------------------------
/**
 * This method returns the description for the ElementWrapper object.
 *
 * @return description string for the object.
 *
 */
//------------------------------------------------------------------------------
std::string ElementWrapper::GetDescription() const
{
   return description;
}

//------------------------------------------------------------------------------
//  Gmat::WrapperDataType  GetWrapperType() const
//------------------------------------------------------------------------------
/**
 * This method returns the wrapper type for the ElementWrapper object.
 *
 * @return wrapper type for the object.
 *
 */
//------------------------------------------------------------------------------
Gmat::WrapperDataType ElementWrapper::GetWrapperType() const
{
   return wrapperType;
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
const StringArray& ElementWrapper::GetRefObjectNames()
{
   return refObjectNames;
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the ElementWrapper 
 * object.
 * 
 * @param <obj> pointer to the object.
 *
 * @return true for success; false for failure.
 *
 */
//------------------------------------------------------------------------------
bool ElementWrapper::SetRefObject(GmatBase *obj)
{
   return false;
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
bool ElementWrapper::RenameObject(const std::string &oldName, 
                                  const std::string &newName)
{
   #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage(
      "Now entering EW::RenameObject with oldName = %s\n and newName = %s\n",
      oldName.c_str(), newName.c_str());
   #endif
   // replace the old name with the new name in the list of ref objects
   Integer sz = refObjectNames.size();
   for (Integer j=0; j < sz; j++)
   {
      if (refObjectNames[j] == oldName)  refObjectNames[j] = newName;
      #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage(
         "--- And now changing the name in the refObjectNames array");
      #endif
      std::string::size_type pos = refObjectNames[j].find(oldName);
      
      if (pos != refObjectNames[j].npos)
      {
         refObjectNames[j].replace(pos, oldName.size(), newName);
      }
   }
   return true;
}

Rmatrix ElementWrapper::EvaluateArray() const
{
   throw GmatBaseException(
      "EvaluateArray() method not valid for wrapper of non-Array type.\n");
}

bool ElementWrapper::SetArray(const Rmatrix &toValue)
{
   throw GmatBaseException(
      "SetArray() method not valid for wrapper of non-Array type.\n");
}
std::string ElementWrapper::EvaluateString() const
{
   throw GmatBaseException(
      "EvaluateString() method not valid for wrapper of non-String type.\n");
}

bool ElementWrapper::SetString(const std::string &toValue)
{
   throw GmatBaseException(
      "SetString() method not valid for wrapper of non-String type.\n");
}
