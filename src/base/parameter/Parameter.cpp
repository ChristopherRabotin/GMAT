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
#include "ParameterInfo.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PARAMETER 1

//---------------------------------
// static data
//---------------------------------
const std::string
Parameter::PARAMETER_KEY_STRING[GmatParam::KeyCount] =
{
   "SystemParam",
   "UserParam"
};

const std::string
Parameter::PARAMETER_TEXT[ParameterParamCount] =
{
   "Object",
   "Expression",
   "Description",
   "Unit",
   "DepObject",
   "Color",
};

const Gmat::ParameterType
Parameter::PARAMETER_TYPE[ParameterParamCount] =
{
   Gmat::STRING_TYPE,          //"Object",
   Gmat::STRING_TYPE,          //"Expression",
   Gmat::STRING_TYPE,          //"Description",
   Gmat::STRING_TYPE,          //"Unit",
   Gmat::STRING_TYPE,          //"DepObject",
   Gmat::UNSIGNED_INT_TYPE,    //"Color",
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Parameter(const std::string &name, const std::string &typeStr,
//           GmatParam::ParameterKey key, GmatBase *obj, const std::string &desc,
//           const std::string &unit, GmatParam::DepObject depObj,
//           Gmat::ObjectType ownerType, bool isTimeParam)
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
 * @param <ownerType> object type who owns this parameter as property
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NONE)
 * @param <isTimeParam> true if parameter is time related, false otherwise
 *
 * @exception <ParameterException> thrown if parameter name has blank spaces
 */
//------------------------------------------------------------------------------
Parameter::Parameter(const std::string &name, const std::string &typeStr,
                     GmatParam::ParameterKey key, GmatBase *obj,
                     const std::string &desc, const std::string &unit,
                     GmatParam::DepObject depObj, Gmat::ObjectType ownerType,
                     bool isTimeParam)
   : GmatBase(Gmat::PARAMETER, typeStr, name)
{  
   objectTypes.push_back(Gmat::PARAMETER);
   objectTypeNames.push_back("Parameter");

   mKey = key;
   
   //if ((name != "" && name != " "))
   if (name != "")
   {
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
   mDepObjectName = "";
   mOwnerType = ownerType;
   mColor = 0; // black
   
   mIsTimeParam = isTimeParam;
   mNeedCoordSystem = false;
   mIsCoordSysDependent = false;
   mIsOriginDependent = false;
   
   if (depObj == GmatParam::COORD_SYS)
      mIsCoordSysDependent = true;
   else if (depObj == GmatParam::ORIGIN)
      mIsOriginDependent = true;
   
   mIsPlottable = true;

   // register parameter names with info
   ParameterInfo::Instance()->Add(name, depObj);

   // set parameter count
   parameterCount = ParameterParamCount;

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
   mKey  = copy.mKey;
   mExpr = copy.mExpr;
   mDesc = copy.mDesc;
   mUnit = copy.mUnit;
   mDepObjectName = copy.mDepObjectName;
   mOwnerType = copy.mOwnerType;
   mColor = copy.mColor;
   mIsTimeParam = copy.mIsTimeParam;
   mIsPlottable = copy.mIsPlottable;
   mIsCoordSysDependent = copy.mIsCoordSysDependent;
   mIsOriginDependent = copy.mIsOriginDependent;
   mNeedCoordSystem = copy.mNeedCoordSystem;
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
      mUnit = right.mUnit;
      mDepObjectName = right.mDepObjectName;
      mOwnerType = right.mOwnerType;
      mColor = right.mColor;
      mIsTimeParam = right.mIsTimeParam;
      mIsPlottable = right.mIsPlottable;
      mIsCoordSysDependent = right.mIsCoordSysDependent;
      mIsOriginDependent = right.mIsOriginDependent;
      mNeedCoordSystem = right.mNeedCoordSystem;
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
// GmatParam::ParameterKey GetKey() const
//------------------------------------------------------------------------------
/**
 * @return enumeration value of parameter key.
 */
//------------------------------------------------------------------------------
GmatParam::ParameterKey Parameter::GetKey() const
{
   return mKey;
}

//------------------------------------------------------------------------------
// Gmat::ObjectType GetOwnerType() const
//------------------------------------------------------------------------------
/**
 * @return enumeration value of object type.
 */
//------------------------------------------------------------------------------
Gmat::ObjectType Parameter::GetOwnerType() const
{
   return mOwnerType;
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
// bool IsCoordSysDependent() const
//------------------------------------------------------------------------------
/**
 * @return true if parameter is CoordinateSystem dependent.
 */
//------------------------------------------------------------------------------
bool Parameter::IsCoordSysDependent() const
{
   return mIsCoordSysDependent;
}

//------------------------------------------------------------------------------
// bool IsOriginDependent() const
//------------------------------------------------------------------------------
/**
 * @return true if parameter is origin dependent.
 */
//------------------------------------------------------------------------------
bool Parameter::IsOriginDependent() const
{
   return mIsOriginDependent;
}

//------------------------------------------------------------------------------
// bool NeedCoordSystem() const
//------------------------------------------------------------------------------
/**
 * @return true if parameter is needs CoordinateSystem.
 */
//------------------------------------------------------------------------------
bool Parameter::NeedCoordSystem() const
{
   return mNeedCoordSystem;
}

//------------------------------------------------------------------------------
// void SetKey(const GmatParam::ParameterKey &key)
//------------------------------------------------------------------------------
/**
 * Sets parameter key.
 *
 * @param <key> key of parameter.
 */
//------------------------------------------------------------------------------
void Parameter::SetKey(const GmatParam::ParameterKey &key)
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
// Real GetReal() const
//------------------------------------------------------------------------------
/**
 * @return parameter value without evaluating.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
Real Parameter::GetReal() const
{
   throw ParameterException
      ("Parameter: GetReal(): " + this->GetTypeName() + " has no "
       "implementation of GetReal().\nMay be an invalid call to this "
       "function.\n");
}

//------------------------------------------------------------------------------
// Rvector6 GetRvector6() const
//------------------------------------------------------------------------------
/**
 * @return parameter value without evaluating.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
Rvector6 Parameter::GetRvector6() const
{
   throw ParameterException
      ("Parameter: GetRvector6(): " + this->GetTypeName() + " has no "
       "implementation of GetRvector6().\nMay be an invalid call to this "
       "function.\n");
}

//------------------------------------------------------------------------------
// void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets Real value of parameter.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
void Parameter::SetReal(Real val)
{
   throw ParameterException
      ("Parameter: SetReal(): " + this->GetTypeName() + " has no "
       "implementation of SetReal().\nMay be an invalid call to this "
       "function.\n");
}

//------------------------------------------------------------------------------
// void SetRvector6(const Rvector6 &val)
//------------------------------------------------------------------------------
/**
 * Sets Rvector6 value of parameter.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
void Parameter::SetRvector6(const Rvector6 &val)
{
   throw ParameterException
      ("Parameter: SetRvector6(): " + this->GetTypeName() + " has no "
       "implementation of SetRvector6().\nMay be an invalid call to this "
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
// virtual CoordinateSystem* GetInternalCoordSystem()
//------------------------------------------------------------------------------
CoordinateSystem* Parameter::GetInternalCoordSystem()
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
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
void Parameter::SetInternalCoordSystem(CoordinateSystem *cs)
{
   ; // do nothing here
}

//------------------------------------------------------------------------------
// virtual void Initialize()
//------------------------------------------------------------------------------
bool Parameter::Initialize()
{
   return true; // do nothing here
}

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
bool Parameter::Evaluate()
{
   if (mKey == GmatParam::SYSTEM_PARAM)
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
   if (mKey == GmatParam::SYSTEM_PARAM)
      throw ParameterException("Parameter: AddObject() should be implemented "
                               "for Parameter Type:" + GetTypeName() + "\n");

   return false;
}

//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer Parameter::GetNumRefObjects() const
{
   if (mKey == GmatParam::SYSTEM_PARAM)
      throw ParameterException("Parameter: GetNumRefObjects() should be implemented"
                               "for Parameter Type: " + GetTypeName() + "\n");

   return 0;
}

//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
bool Parameter::Validate()
{
   if (mKey == GmatParam::SYSTEM_PARAM)
      throw ParameterException("Parameter: Validate() should be implemented "
                               "for Parameter Type: " + GetTypeName() + "\n");

   return true; // loj: 9/23/04 There is nothing to validate for USER_PARAM
}


//---------------------------------
// methods inherited from GmatBase
//---------------------------------

// required method for all subclasses that can be copied in a script
void Parameter::Copy(const GmatBase* orig)
{
   operator=(*((Parameter *)(orig)));
}


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

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool Parameter::IsParameterReadOnly(const Integer id) const
{
   if ((id == DESCRIPTION) || (id == UNIT) ||
       (id == DEP_OBJECT) || (id == COLOR))
      return true;

   return GmatBase::IsParameterReadOnly(id);
}


//----- UnsignedInt parameters

//------------------------------------------------------------------------------
// UnsignedInt GetUnsignedIntParameter(const Integer id) const
//------------------------------------------------------------------------------
UnsignedInt Parameter::GetUnsignedIntParameter(const Integer id) const
{
   #ifdef DEBUG_PARAMETER
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
   #ifdef DEBUG_PARAMETER
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
   case OBJECT:
      if (GetNumRefObjects() > 0)
         //return GetRefObjectName(Gmat::SPACECRAFT);
         return GetRefObjectName(mOwnerType);
      else
         // DJC Changed 1/5/05 to make parameter writing work
         return "";
//         return GmatBase::GetStringParameter(id);
   case EXPRESSION:
      return mExpr;
   case DESCRIPTION:
      return mDesc;
   case UNIT:
      return mUnit;
   case DEP_OBJECT: //loj: 12/8/04 added
      return mDepObjectName;
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
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage("Parameter::SetStringParameter() id=%d, value=%s\n",
                                 id, value.c_str());
   #endif
   
   switch (id)
   {
   case OBJECT:
      //return SetRefObjectName(Gmat::SPACECRAFT, value);
      //loj: 12/10/04  Changed to use owner type
      return SetRefObjectName(mOwnerType, value);
   case EXPRESSION:
      mExpr = value;
      return true;
   case DESCRIPTION:
      mDesc = value;
      return true;
   case UNIT:
      mUnit = value;
      return true;
   case DEP_OBJECT: //loj: 12/8/04 added
      mDepObjectName = value;
      //if (mIsCoordSysDependent)
      //   return SetRefObjectName(Gmat::COORDINATE_SYSTEM, value);
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
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage("Parameter::SetStringParameter() label=%s value=%s\n",
                                 label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------
// protected methods
//---------------------------------

