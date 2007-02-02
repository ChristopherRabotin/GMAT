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

#include "MessageInterface.hpp"

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  VariableWrapper();
//---------------------------------------------------------------------------
/**
 * Constructs VariableWrapper structures
 * (default constructor).
 *
 */
//---------------------------------------------------------------------------
VariableWrapper::VariableWrapper() :
   ElementWrapper(),
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
//  bool SetRefObject(GmatBase *obj)
//---------------------------------------------------------------------------
/**
 * Method to set the reference object (Variable) pointer on the wrapped 
 * object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool VariableWrapper::SetRefObject(GmatBase *obj)
{
   if ( (obj->GetName() != refObjectNames[0]) ||
        (obj->GetTypeName() != "Variable") )
   {
      std::string errmsg = "Referenced variable \"";
      errmsg += obj->GetName();
      errmsg += ("\" of type \"" + obj->GetTypeName());
      errmsg += "\" was passed into the variable wrapper \"";
      errmsg += description;
      errmsg += "\", but an object named \"";
      errmsg += refObjectNames[0];
      errmsg += "\" was expected.\n";
      throw ParameterException(errmsg);
   }

   var = (Variable*) obj;
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

//---------------------------------------------------------------------------
//  void SetupWrapper()
//---------------------------------------------------------------------------
/**
 * Method to set up the Variable Wrapper.
 *
 */
//---------------------------------------------------------------------------
void VariableWrapper::SetupWrapper()
{
   refObjectNames.push_back(description);
}
