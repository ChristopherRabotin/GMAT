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
   if ( (obj->GetName() != refObjectNames[0]) ||
        (!obj->IsOfType("Parameter")) )
   {
      std::string errmsg = "Referenced object \"";
      errmsg += obj->GetName();
      errmsg += "\" was passed into the variable wrapper \"";
      errmsg += description;
      errmsg += "\", but an object named \"";
      errmsg += refObjectNames[0];
      errmsg += "\" was expected.\n";
      throw ParameterException(errmsg);
   }
   param = (Parameter*) obj;
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
Real ParameterWrapper::EvaluateReal() const
{
   if (param == NULL)
      throw ParameterException(
      "Cannot return value of Parameter - pointer is NULL\n");
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
   // now rebuild the description string from the refObjectNames
   Integer pos = description.find(".");
   if (pos != (Integer) std::string::npos)
   {
      description.replace(0,pos,refObjectNames[0]);
   }
   else
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
   // look for a '.' - if it's there only save the object name 
   Integer pos = description.find(".");
   if (pos != (Integer) std::string::npos)
   {
      std::string sub = description.substr(0,pos-1);
      refObjectNames.push_back(sub);
   }
   else   // otherwise, save the whole thing
      refObjectNames.push_back(description);
}

