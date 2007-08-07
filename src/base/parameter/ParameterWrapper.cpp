//$Header$
//------------------------------------------------------------------------------
//                                  ParameterWrapper
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
 * Implementation of the ParameterWrapper class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ParameterWrapper.hpp"
#include "ParameterException.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_PW

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ParameterWrapper();
//---------------------------------------------------------------------------
/**
 * Constructs ParameterWrapper structures
 * (default constructor).
 *
 */
//---------------------------------------------------------------------------
ParameterWrapper::ParameterWrapper() :
   ElementWrapper(),
   param         (NULL)
{
   wrapperType = Gmat::PARAMETER_OBJECT;
}

//---------------------------------------------------------------------------
//  ParameterWrapper(const ParameterWrapper &pw);
//---------------------------------------------------------------------------
/**
 * Constructs base ParameterWrapper structures used in derived classes, by 
 * copying the input instance (copy constructor).
 *
 * @param <pw>  ParameterWrapper instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
ParameterWrapper::ParameterWrapper(const ParameterWrapper &pw) :
   ElementWrapper(pw),
   param         (pw.param)
{
}

//---------------------------------------------------------------------------
//  ParameterWrapper& operator=(const ParameterWrapper &pw)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ParameterWrapper structures.
 *
 * @param <pw> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ParameterWrapper& ParameterWrapper::operator=(const ParameterWrapper &pw)
{
   if (&pw == this)
      return *this;

   ElementWrapper::operator=(pw);
   param = pw.param;

   return *this;
}
//---------------------------------------------------------------------------
//  ~ParameterWrapper()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ParameterWrapper::~ParameterWrapper()
{
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType GetDataType() const
//------------------------------------------------------------------------------
/**
 * This method returns the data type for the ParameterWrapper object.
 *
 * @return data type for the object.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ParameterWrapper::GetDataType() const
{
   return Gmat::REAL_TYPE;
}

//---------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj)
//---------------------------------------------------------------------------
/**
 * Method to set the reference object (Parameter) pointer on the wrapped 
 * object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ParameterWrapper::SetRefObject(GmatBase *obj)
{
//   if ( (obj->GetName() != refObjectNames[0]) ||
//        (!obj->IsOfType("Parameter")) )
//   {
//      std::string errmsg = "Referenced object \"";
//      errmsg += obj->GetName();
//      errmsg += "\" was passed into the parameter wrapper \"";
//      errmsg += description;
//      errmsg += "\", but an object named \"";
//      errmsg += refObjectNames[0];
//      errmsg += "\" was expected.\n";
//      throw ParameterException(errmsg);
//   }
   if ( (obj->GetName() == refObjectNames[0]) ||
        (obj->IsOfType("Parameter")) )
   {
      param = (Parameter*) obj;
      return true;
   }
   else return false;
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
Real ParameterWrapper::EvaluateReal() const
{
   if (param == NULL)
      throw ParameterException(
      "Cannot return value of Parameter - pointer is NULL\n");
   #ifdef DEBUG_PW
      MessageInterface::ShowMessage(
      "In ParameterWrapper::EvalReal, value is %.12f\n", param->EvaluateReal());
   #endif
   return param->EvaluateReal();
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
bool ParameterWrapper::SetReal(const Real toValue)
{
   if (param == NULL)
      throw ParameterException(
      "Cannot set value of Parameter - pointer is NULL\n");
   param->SetReal(toValue);
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
bool ParameterWrapper::RenameObject(const std::string &oldName, 
                                    const std::string &newName)
{
   ElementWrapper::RenameObject(oldName, newName);
   description = refObjectNames[0];  
   return true;
}
                                       

//---------------------------------------------------------------------------
//  void SetupWrapper()
//---------------------------------------------------------------------------
/**
 * Method to set up the Parameter Wrapper.
 *
 */
//---------------------------------------------------------------------------
void ParameterWrapper::SetupWrapper()
{
   refObjectNames.push_back(description);
}

