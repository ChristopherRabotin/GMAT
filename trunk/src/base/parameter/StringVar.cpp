//$Id$
//------------------------------------------------------------------------------
//                                  StringVar
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/1/7
//
/**
 * Implements StringVar class which handles std::string value. The string value
 * is stored in Parameter::mExpr.
 */
//------------------------------------------------------------------------------

#include "StringVar.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_STRINGVAR 1

//---------------------------------
// static data
//---------------------------------

const std::string
StringVar::PARAMETER_TEXT[StringVarParamCount - ParameterParamCount] =
{
   "Value",
};

const Gmat::ParameterType
StringVar::PARAMETER_TYPE[StringVarParamCount - ParameterParamCount] =
{
   Gmat::STRING_TYPE,
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// StringVar(const std::string &name, const std::string &typeStr, 
//         GmatParam::ParameterKey key, GmatBase *obj, const std::string &desc,
//         const std::string &unit, GmatParam::DepObject depObj, Gmat::ObjectType,
//         bool isTimeParam)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> parameter name
 * @param <typeStr>  parameter type string
 * @param <key>  parameter key (SYSTEM_PARAM, USER_PARAM, etc)
 * @param <obj>  reference object pointer
 * @param <desc> parameter description
 * @param <unit> parameter unit
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NONE)
 * @param <ownerType> object type who owns this parameter as property
 * @param <isTimeParam> true if parameter is time related, false otherwise
 */
//------------------------------------------------------------------------------
StringVar::StringVar(const std::string &name, const std::string &typeStr, 
                     GmatParam::ParameterKey key, GmatBase *obj, const std::string &desc,
                     const std::string &unit, GmatParam::DepObject depObj,
                     Gmat::ObjectType ownerType, bool isTimeParam)
   : Parameter(name, typeStr, key, obj, desc, unit, depObj, ownerType, isTimeParam,
               false, false, true)
{  
   objectTypeNames.push_back("String");
   mStringValue = STRING_PARAMETER_UNDEFINED;
   mReturnType = Gmat::STRING_TYPE;
}


//------------------------------------------------------------------------------
// StringVar(const StringVar &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
StringVar::StringVar(const StringVar &copy)
   : Parameter(copy)
{
   mStringValue = copy.mStringValue;   
}


//------------------------------------------------------------------------------
// StringVar& operator= (const StringVar& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
StringVar& StringVar::operator= (const StringVar& right)
{
   if (this != &right)
   {
      // We don't want to change the name when copy
      std::string thisName = instanceName;
      
      Parameter::operator=(right);
      mStringValue = right.mStringValue;
      
      SetName(thisName);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~StringVar()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
StringVar::~StringVar()
{
}


//------------------------------------
// methods inherited from Parameter
//------------------------------------

//------------------------------------------------------------------------------
// bool operator==(const StringVar &right) const
//------------------------------------------------------------------------------
/**
 * Equal operator.
 *
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool StringVar::operator==(const StringVar &right) const
{
   return Parameter::operator==(right);
}


//------------------------------------------------------------------------------
// bool operator!=(const StringVar &right) const
//------------------------------------------------------------------------------
/**
 * Not equal operator.
 *
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool StringVar::operator!=(const StringVar &right) const
{
   return Parameter::operator!=(right);
}


//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * Retrieves string value of parameter.
 *
 * @return string value of parameter.
 */
//------------------------------------------------------------------------------
std::string StringVar::ToString()
{
   return mExpr;
}


//------------------------------------------------------------------------------
// std::string GetString() const
//------------------------------------------------------------------------------
/**
 * Retrieves string value of parameter.
 *
 * @return string value.
 */
//------------------------------------------------------------------------------
std::string StringVar::GetString() const
{
   return mExpr;
}


//------------------------------------------------------------------------------
// std::string EvaluateString()
//------------------------------------------------------------------------------
/**
 * Retrieves string value of parameter.
 *
 * @return string value.
 */
//------------------------------------------------------------------------------
std::string StringVar::EvaluateString()
{
   return mExpr;
}


//------------------------------------
// methods inherited from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object.
 *
 * @return cloned object pointer.
 */
//------------------------------------------------------------------------------
GmatBase* StringVar::Clone() const
{
   return new StringVar(*this);
}


//------------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//------------------------------------------------------------------------------
void StringVar::Copy(const GmatBase* orig)
{
   operator=(*((StringVar *)(orig)));
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer StringVar::GetParameterID(const std::string &str) const
{
   for (int i=ParameterParamCount; i<StringVarParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return Parameter::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string StringVar::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case VALUE:
      return mExpr;
   default:
      return Parameter::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string StringVar::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool StringVar::SetStringParameter(const Integer id, const std::string &value)
{
   #if DEBUG_STRINGVAR
   MessageInterface::ShowMessage("StringVar::SetStringParameter() id=%d, value=%s\n",
                                 id, value.c_str());
   #endif
   
   switch (id)
   {
   case VALUE:
      mExpr = value;
      mStringValue = value;
      return true;
   default:
      return Parameter::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool StringVar::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
   #if DEBUG_STRINGVAR
   MessageInterface::ShowMessage("StringVar::SetStringParameter() label=%s value=%s\n",
                                 label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(...)
//------------------------------------------------------------------------------
/**
 * Produces a string, possibly multi-line, containing the text that produces an
 * object.
 * 
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 * 
 * @return A string containing the text.
 */
//------------------------------------------------------------------------------
const std::string& StringVar::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   // Note:
   // Do not write "Create name" since multiple Strings will be written from
   // the ScriptInterpreter
   
   std::string sval = GetString();

   // Write value if it is not blank
   if (sval != "")
   {
      generatingString = "GMAT " + GetName() + " = " + sval;
      generatingString = generatingString + ";" + inlineComment + "\n";
   }
   
   return generatingString;
}

