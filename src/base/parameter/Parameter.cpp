//$Header$
//------------------------------------------------------------------------------
//                                  Parameter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/16 Linda Jun
//
/**
 * Defines base class of parameters.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "ParameterException.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
Parameter::PARAMETER_KEY_STRING[KeyCount] =
{
   "SystemParam",
   "UserParam"
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Parameter(const std::string &name, const std::string &typeStr,
//           ParameterKey key, const std::string &desc)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> parameter name
 * @param <key>  parameter key (SYSTEM_PARAM, USER_PARAM, etc)
 * @param <desc> parameter description
 *
 * @exception <ParameterException> thrown if parameter name is blank or has
 *    blank spaces
 */
//------------------------------------------------------------------------------
Parameter::Parameter(const std::string &name, const std::string &typeStr,
                     ParameterKey key,const std::string &desc)
   : GmatBase(Gmat::PARAMETER, typeStr, name)
{  
   mKey = key;

   //if name is blank or has blank spaces
   if ((name == "") || (name.find(' ') < name.npos))
      throw ParameterException
         ("Parameter: parameter name is blank or has blank space:" + name);
   else
      instanceName = name;

   if (desc == "")
      mDesc = std::string(name);
   else
      mDesc = desc;
}

//------------------------------------------------------------------------------
// Parameter(const Parameter &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the object being copied.
 */
//------------------------------------------------------------------------------
Parameter::Parameter(const Parameter &param)
   : GmatBase(param)
{
   mKey = param.mKey;
   mDesc = param.mDesc;
}

//------------------------------------------------------------------------------
// Parameter& operator= (const Parameter& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
Parameter& Parameter::operator= (const Parameter& right)
{
   if (this != &right)
   {
      GmatBase::operator=(right);
      mKey = right.mKey;
      mDesc = right.mDesc;
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~Parameter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Parameter::~Parameter()
{
}

//------------------------------------------------------------------------------
// ParameterKey GetParameterKey() const
//------------------------------------------------------------------------------
/**
 * Retrieves parameter key.
 *
 * @return enumeration value of parameter key.
 */
//------------------------------------------------------------------------------
Parameter::ParameterKey Parameter::GetParameterKey() const
{
   return mKey;
}

//------------------------------------------------------------------------------
// std::string GetParameterDesc() const
//------------------------------------------------------------------------------
/**
 * Retrieves parameter description.
 *
 * @return parameter description.
 */
//------------------------------------------------------------------------------
std::string Parameter::GetParameterDesc() const
{
   return mDesc;
}

//------------------------------------------------------------------------------
// void SetDesc(cosnt std::string &desc)
//------------------------------------------------------------------------------
/**
 * Sets parameter description.
 *
 * @param <desc> description of parameter.
 */
//------------------------------------------------------------------------------
void Parameter::SetDesc(const std::string &desc)
{
   mDesc = desc;
}

//------------------------------------------------------------------------------
// bool operator==(const Parameter &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool Parameter::operator==(const Parameter &right) const
{
   if (typeName != right.typeName)
      return false;

   if (instanceName.compare(right.instanceName) != 0)
      return false;

   return true;
}

//------------------------------------------------------------------------------
// bool operator!=(const Parameter &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool Parameter::operator!=(const Parameter &right) const
{
   return !(*this == right);
}

//loj: added since review ready

//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* Parameter::GetParameterList() const
{
   return NULL;
}
