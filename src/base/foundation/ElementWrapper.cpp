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

//#include "MessageInterface.hpp"

//---------------------------------
// static data
//---------------------------------
const Real ElementWrapper::UNDEFINED_REAL = -999.99;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ElementWrapper(const std::string &desc);
//---------------------------------------------------------------------------
/**
 * Constructs base ElementWrapper structures used in derived classes
 * (default constructor).
 *
 * @param <desc> Optional description for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
ElementWrapper::ElementWrapper(const std::string &desc) :
   description  (desc)
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
   description (er.description)
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

   description = er.description;

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

