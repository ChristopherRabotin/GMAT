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
// Created: 2003/09/16
//
/**
 * Implements base class of parameters.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PARAMETER 1

//---------------------------------
// static data
//---------------------------------
const std::string
Parameter::PARAMETER_KEY_STRING[KeyCount] =
{
   "SystemParam",
   "UserParam"
};

const std::string
Parameter::PARAMETER_TEXT[ParameterParamCount] =
{
   "Object",       //loj: 4/23/04 this will be removed later (need for current script)
   "Spacecraft",
   "Expression",
   "Description",
   "Unit",
   "Color",
};

const Gmat::ParameterType
Parameter::PARAMETER_TYPE[ParameterParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::UNSIGNED_INT_TYPE,
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Parameter(const std::string &name, const std::string &typeStr,
//           ParameterKey key, GmatBase *obj, const std::string &desc,
//           const std::string &unit, bool isTimeParam)
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
 * @param <isTimeParam> true if parameter is time related, false otherwise
 *
 * @exception <ParameterException> thrown if parameter name has blank spaces
 */
//------------------------------------------------------------------------------
Parameter::Parameter(const std::string &name, const std::string &typeStr,
                     ParameterKey key, GmatBase *obj, const std::string &desc,
                     const std::string &unit, bool isTimeParam)
   : GmatBase(Gmat::PARAMETER, typeStr, name)
{  
   mKey = key;
   
   //if ((name != "" && name != " "))
   if (name != "")
   {
      //loj: 4/26/04
      //if constructor throws an exception, it isn't caught in the caller code.
      //so replace blank space with underscore "_"
      std::string tempName = name;
      std::string replaceStr = "_";
      for (unsigned int i=0; i<tempName.size(); i++)
         if (tempName[i] == ' ')
            tempName.replace(i, 1, replaceStr);

      instanceName = tempName;
      
      //if (name.find(' ') < name.npos)
      //     throw ParameterException
      //         ("Parameter: parameter name cannot have blank space: " + name);
   }
   
   if (desc == "")
      mDesc = instanceName;
   else
      mDesc = desc;

   mExpr = "";
   mUnit = unit;
   mColor = 0; // black
   
   mIsTimeParam = isTimeParam;
   mIsPlottable = true;
}

//------------------------------------------------------------------------------
// Parameter(const Parameter &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
Parameter::Parameter(const Parameter &copy)
   : GmatBase(copy)
{
   mKey = copy.mKey;
   mExpr = copy.mExpr;
   mDesc = copy.mDesc;
   mUnit = copy.mUnit;
   mIsTimeParam = copy.mIsTimeParam;
   mIsPlottable = copy.mIsPlottable;
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
      mExpr = right.mExpr;
      mDesc = right.mDesc;
      mIsTimeParam = right.mIsTimeParam;
      mIsPlottable = right.mIsPlottable;
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
// ParameterKey GetKey() const
//------------------------------------------------------------------------------
/**
 * @return enumeration value of parameter key.
 */
//------------------------------------------------------------------------------
Parameter::ParameterKey Parameter::GetKey() const
{
   return mKey;
}

//------------------------------------------------------------------------------
// bool IsTimeParameter() const
//------------------------------------------------------------------------------
/**
 * @return true if parameter is time related.
 */
//------------------------------------------------------------------------------
bool Parameter::IsTimeParameter() const
{
   return mIsTimeParam;
}

//------------------------------------------------------------------------------
// bool IsPlottable() const
//------------------------------------------------------------------------------
/**
 * @return true if parameter is plottble.
 */
//------------------------------------------------------------------------------
bool Parameter::IsPlottable() const
{
   return mIsPlottable;
}

//------------------------------------------------------------------------------
// void SetKey(const ParameterKey &key)
//------------------------------------------------------------------------------
/**
 * Sets parameter key.
 *
 * @param <key> key of parameter.
 */
//------------------------------------------------------------------------------
void Parameter::SetKey(const ParameterKey &key)
{
   mKey = key;
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

//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return parameter value converted to std::string.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
std::string Parameter::ToString()
{
   throw ParameterException
      ("Parameter: ToString(): " + this->GetTypeName() + " has no "
       "implementation of ToString().\n");
}

//------------------------------------------------------------------------------
// Real GetReal()
//------------------------------------------------------------------------------
/**
 * @return parameter value without evaluating.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
Real Parameter::GetReal()
{
   throw ParameterException
      ("Parameter: GetReal(): " + this->GetTypeName() + " has no "
       "implementation of GetReal().\nMay be an invalid call to this "
       "function.\n");
}

//------------------------------------------------------------------------------
// Rvector6 GetRvector6()
//------------------------------------------------------------------------------
/**
 * @return parameter value without evaluating.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
Rvector6 Parameter::GetRvector6()
{
   throw ParameterException
      ("Parameter: GetRvector6(): " + this->GetTypeName() + " has no "
       "implementation of GetRvector6().\nMay be an invalid call to this "
       "function.\n");
}

//------------------------------------------------------------------------------
// Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated parameter value.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
Real Parameter::EvaluateReal()
{
   throw ParameterException
      ("Parameter: EvaluateReal(): " + this->GetTypeName() + " has no "
       "implementation of EvaluateReal().\nMay be an invalid call to this "
       "function.\n");
}

//------------------------------------------------------------------------------
// Rvector6 EvaluateRvector6()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated parameter value.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
Rvector6 Parameter::EvaluateRvector6()
{
   throw ParameterException
      ("Parameter: EvaluateRvector6(): " + this->GetTypeName() + " has no "
       "implementation of EvaluateRvector6().\nMay be an invalid call to this "
       "function.\n");
}

//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* Parameter::GetParameterList() const
{
   return NULL;
}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void Parameter::SetSolarSystem(SolarSystem *ss)
{
   ; // do nothing here
}

//------------------------------------------------------------------------------
// virtual void Initialize()
//------------------------------------------------------------------------------
void Parameter::Initialize()
{
   //if (mKey == SYSTEM_PARAM)
   //   throw ParameterException("Parameter: Initialize() should be implemented "
   //                            "for Parameter Type: " + GetTypeName());
}

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
bool Parameter::Evaluate()
{
   if (mKey == SYSTEM_PARAM)
      throw ParameterException("Parameter: Evaluate() should be implemented "
                               "for Parameter Type: " + GetTypeName() + "\n");

   return false;
}

//------------------------------------------
// methods All SYSTEM_PARAM should implement
//------------------------------------------

//------------------------------------------------------------------------------
// virtual bool AddRefObject(GmatBase *object)
//------------------------------------------------------------------------------
bool Parameter::AddRefObject(GmatBase *object)
{
   if (mKey == SYSTEM_PARAM)
      throw ParameterException("Parameter: AddObject() should be implemented "
                               "for Parameter Type:" + GetTypeName() + "\n");

   return false;
}

//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer Parameter::GetNumRefObjects() const
{
   if (mKey == SYSTEM_PARAM)
      throw ParameterException("Parameter: GetNumRefObjects() should be implemented"
                               "for Parameter Type: " + GetTypeName() + "\n");

   return 0;
}

//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
bool Parameter::Validate()
{
   if (mKey == SYSTEM_PARAM)
      throw ParameterException("Parameter: Validate() should be implemented "
                               "for Parameter Type: " + GetTypeName() + "\n");

   return false;
}


//---------------------------------
// methods inherited from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string Parameter::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ParameterParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Parameter::GetParameterID(const std::string &str) const
{
   for (int i=GmatBaseParamCount; i<ParameterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType Parameter::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ParameterParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string Parameter::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ParameterParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - GmatBaseParamCount)];
   else
      return GmatBase::GetParameterTypeString(id);
    
}

//----- UnsignedInt parameters

//------------------------------------------------------------------------------
// UnsignedInt GetUnsignedIntParameter(const Integer id) const
//------------------------------------------------------------------------------
UnsignedInt Parameter::GetUnsignedIntParameter(const Integer id) const
{
#if DEBUG_PARAMETER
   MessageInterface::ShowMessage("Parameter::GetUnsignedIntParameter() "
                                 "id=%d\n", id);
#endif
   
   switch (id)
   {
   case COLOR:
      return mColor;
   default:
      return GmatBase::GetUnsignedIntParameter(id);
   }
}

//------------------------------------------------------------------------------
// UnsignedInt GetUnsignedIntParameter(const std::string &label) const
//------------------------------------------------------------------------------
UnsignedInt Parameter::GetUnsignedIntParameter(const std::string &label) const
{
   return GetUnsignedIntParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// UnsignedInt SetUnsignedIntParameter(const Integer id, const UnsignedInt value)
//------------------------------------------------------------------------------
UnsignedInt Parameter::SetUnsignedIntParameter(const Integer id,
                                               const UnsignedInt value)
{
#if DEBUG_PARAMETER
   MessageInterface::ShowMessage("Parameter::SetUnsignedIntParameter() "
                                 "id=%d value=%d\n", id, value);
#endif
   switch (id)
   {
   case COLOR: 
      mColor = value;
      return mColor;
   default:
      return GmatBase::SetUnsignedIntParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// UnsignedInt SetUnsignedIntParameter(const std::string &label,
//                                     const UnsignedInt &value)
//------------------------------------------------------------------------------
UnsignedInt Parameter::SetUnsignedIntParameter(const std::string &label,
                                               const UnsignedInt value)
{
   return SetUnsignedIntParameter(GetParameterID(label), value);
}

//----- std::string parameters

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string Parameter::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case OBJECT: //loj: 9/13/04 return first object name of spacecraft
      if (GetNumRefObjects() > 0)
         return GetRefObjectName(Gmat::SPACECRAFT);
      else
         return GmatBase::GetStringParameter(id);
   case SPACECRAFT: //loj: 9/13/04 dded
      return GetRefObjectName(Gmat::SPACECRAFT);
   case EXPRESSION:
      return mExpr;
   case DESCRIPTION:
      return mDesc;
   case UNIT:
      return mUnit;
   default:
      return GmatBase::GetStringParameter(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string Parameter::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool Parameter::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case OBJECT: //loj: 9/13/04 only spacecraft is allowed for now
      return SetRefObjectName(Gmat::SPACECRAFT, value);
   case SPACECRAFT: //loj: 9/13/04 added
      return SetRefObjectName(Gmat::SPACECRAFT, value);
   case EXPRESSION:
      mExpr = value;
      return true;
   case DESCRIPTION:
      mDesc = value;
      return true;
   case UNIT:
      mUnit = value;
      return true;
   default:
      return GmatBase::SetStringParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool Parameter::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
#if DEBUG_PARAMETER
   MessageInterface::ShowMessage("Parameter::SetStringParameter() entered: "
                                 "label = " + label + ", value = " + value + "\n");
#endif
   
   return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------
// protected methods
//---------------------------------

