//$Header$
//------------------------------------------------------------------------------
//                                  VariableWrapper
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
 * Implementation of the VariableWrapper class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "VariableWrapper.hpp"
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
//  VariableWrapper(const std::string &desc);
//---------------------------------------------------------------------------
/**
 * Constructs VariableWrapper structures
 * (default constructor).
 *
 * @param <desc> Optional description for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
VariableWrapper::VariableWrapper(const std::string &desc) :
   ElementWrapper(desc),
   var           (NULL)
{
}

//---------------------------------------------------------------------------
//  VariableWrapper(const VariableWrapper &vw);
//---------------------------------------------------------------------------
/**
 * Constructs base VariableWrapper structures used in derived classes, by 
 * copying the input instance (copy constructor).
 *
 * @param <vw>  VariableWrapper instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
VariableWrapper::VariableWrapper(const VariableWrapper &vw) :
   ElementWrapper(vw),
   var           (vw.var)
{
}

//---------------------------------------------------------------------------
//  VariableWrapper& operator=(const VariableWrapper &vw)
//---------------------------------------------------------------------------
/**
 * Assignment operator for VariableWrapper structures.
 *
 * @param <vw> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const VariableWrapper& VariableWrapper::operator=(const VariableWrapper &vw)
{
   if (&vw == this)
      return *this;

   ElementWrapper::operator=(vw);
   var = vw.var;

   return *this;
}
//---------------------------------------------------------------------------
//  ~VariableWrapper()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
VariableWrapper::~VariableWrapper()
{
}

//---------------------------------------------------------------------------
//  bool SetVariable(Variable *toVar)
//---------------------------------------------------------------------------
/**
 * Method to set the Variable pointer to the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool VariableWrapper::SetVariable(Variable *toVar)
{
   var = toVar;
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
Real VariableWrapper::EvaluateReal() const
{
   if (var == NULL)
      throw ParameterException(
      "Cannot return value of Variable - pointer is NULL\n");
   return var->EvaluateReal();
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
bool VariableWrapper::SetReal(const Real toValue)
{
   if (var == NULL)
      throw ParameterException(
      "Cannot set value of Variable - pointer is NULL\n");
   var->SetReal(toValue);
   return true;
}
