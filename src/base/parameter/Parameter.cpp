//$Id$
//------------------------------------------------------------------------------
//                                  Parameter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

//#define DEBUG_PARAMETER
//#define DEBUG_RENAME
//#define DEBUG_COMMENT

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
Parameter::PARAMETER_TEXT[ParameterParamCount - GmatBaseParamCount] =
{
   "Object",
   "InitialValue",
   "Expression",
   "Description",
   "Unit",
   "DepObject",
   "Color",
};

const Gmat::ParameterType
Parameter::PARAMETER_TYPE[ParameterParamCount - GmatBaseParamCount] =
{
   Gmat::OBJECT_TYPE,          //"Object",
   Gmat::STRING_TYPE,          //"InitialValue"
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
//           Gmat::ObjectType ownerType, bool isTimeParam, bool isSettable,
//           bool isPlottable, bool isReportable)
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
 * @param <isSettable> true if parameter is settable, false otherwise
 * @param <isPlottable> true if parameter is plottable (Real), false otherwise
 * @param <isReportable> true if parameter is reportable (Real, String), false otherwise
 *
 * @exception <ParameterException> thrown if parameter name has blank spaces
 */
//------------------------------------------------------------------------------
Parameter::Parameter(const std::string &name, const std::string &typeStr,
                     GmatParam::ParameterKey key, GmatBase *obj,
                     const std::string &desc, const std::string &unit,
                     GmatParam::DepObject depObj, Gmat::ObjectType ownerType,
                     bool isTimeParam, bool isSettable, bool isPlottable,
                     bool isReportable)
   : GmatBase(Gmat::PARAMETER, typeStr, name)
{  
   objectTypes.push_back(Gmat::PARAMETER);
   objectTypeNames.push_back("Parameter");
   
   if (key == GmatParam::SYSTEM_PARAM)
      objectTypeNames.push_back("SystemParameter");
   else
      objectTypeNames.push_back("UserParameter");
   
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
   mOwnerName = "";
   mDepObjectName = "";
   mCommentLine2 = "";
   mInitialValue = "";
   mIsCommentFromCreate = true;
   mOwnerType = ownerType;
   mDepObj = depObj;
   mCycleType = GmatParam::NOT_CYCLIC;
   mColor = 0; // black
   mNeedCoordSystem = false;
   mIsCoordSysDependent = false;
   mIsOriginDependent = false;
   
   if (depObj == GmatParam::COORD_SYS)
      mIsCoordSysDependent = true;
   else if (depObj == GmatParam::ORIGIN)
      mIsOriginDependent = true;
   
   mIsAngleParam = false;
   mIsTimeParam = isTimeParam;
   mIsSettable = isSettable;
   mIsPlottable = isPlottable;
   mIsReportable = isReportable;
   
   // register parameter names with info
   ParameterInfo::Instance()->Add(typeName, mOwnerType, instanceName, mDepObj,
                                  isPlottable, isReportable, isSettable);
   
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
   mOwnerName = copy.mOwnerName;
   mDepObjectName = copy.mDepObjectName;
   mCommentLine2 = copy.mCommentLine2;
   mInitialValue = copy.mInitialValue;
   mIsCommentFromCreate = copy.mIsCommentFromCreate;
   mOwnerType = copy.mOwnerType;
   mReturnType = copy.mReturnType;
   mDepObj = copy.mDepObj;
   mCycleType = copy.mCycleType;
   mColor = copy.mColor;
   mIsAngleParam = copy.mIsAngleParam;
   mIsTimeParam = copy.mIsTimeParam;
   mIsSettable = copy.mIsSettable;
   mIsPlottable = copy.mIsPlottable;
   mIsReportable = copy.mIsReportable;
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
   if (this == &right)
      return *this;

   GmatBase::operator=(right);
   
   mKey = right.mKey;
   mExpr = right.mExpr;
   mDesc = right.mDesc;
   mUnit = right.mUnit;
   mDepObjectName = right.mDepObjectName;
   mCommentLine2 = right.mCommentLine2;
   mInitialValue = right.mInitialValue;
   mIsCommentFromCreate = right.mIsCommentFromCreate;
   mOwnerType = right.mOwnerType;
   mReturnType = right.mReturnType;
   mDepObj = right.mDepObj;
   mCycleType = right.mCycleType;
   mColor = right.mColor;
   mIsAngleParam = right.mIsAngleParam;
   mIsTimeParam = right.mIsTimeParam;
   mIsSettable = right.mIsSettable;
   mIsPlottable = right.mIsPlottable;
   mIsReportable = right.mIsReportable;
   mIsCoordSysDependent = right.mIsCoordSysDependent;
   mIsOriginDependent = right.mIsOriginDependent;
   mNeedCoordSystem = right.mNeedCoordSystem;

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
// Gmat::ParameterType GetReturnType() const
//------------------------------------------------------------------------------
/**
 * @return enumeration value of return parameter type.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Parameter::GetReturnType() const
{
   return mReturnType;
}


//------------------------------------------------------------------------------
// GmatParam::CycleType GetCycleType() const
//------------------------------------------------------------------------------
/**
 * @return enumeration value of return parameter type.
 */
//------------------------------------------------------------------------------
GmatParam::CycleType  Parameter::GetCycleType() const
{
   return mCycleType;
}


//------------------------------------------------------------------------------
// bool IsAngleParameter() const
//------------------------------------------------------------------------------
/**
 * @return true if parameter outputs angle value
 */
//------------------------------------------------------------------------------
bool Parameter::IsAngleParameter() const
{
   return mIsAngleParam;
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
// bool IsReportable() const
//------------------------------------------------------------------------------
/**
 * @return true if parameter is plottble.
 */
//------------------------------------------------------------------------------
bool Parameter::IsReportable() const
{
   return mIsReportable;
}


//------------------------------------------------------------------------------
// bool IsSettable() const
//------------------------------------------------------------------------------
/**
 * @return true if parameter is plottble.
 */
//------------------------------------------------------------------------------
bool Parameter::IsSettable() const
{
   return mIsSettable;
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
// const Rvector6& GetRvector6() const
//------------------------------------------------------------------------------
/**
 * @return parameter value without evaluating.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
const Rvector6& Parameter::GetRvector6() const
{
   throw ParameterException
      ("Parameter: GetRvector6(): " + this->GetTypeName() + " has no "
       "implementation of GetRvector6().\nMay be an invalid call to this "
       "function.\n");
}


//------------------------------------------------------------------------------
// const Rmatrix66& GetRmatrix66() const
//------------------------------------------------------------------------------
/**
 * @return parameter value without evaluating.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
const Rmatrix66& Parameter::GetRmatrix66() const
{
   throw ParameterException
      ("Parameter: GetRmatrix66(): " + this->GetTypeName() + " has no "
       "implementation of GetRmatrix66().\nMay be an invalid call to this "
       "function.\n");
}


//------------------------------------------------------------------------------
// const Rmatrix33& GetRmatrix33() const
//------------------------------------------------------------------------------
/**
 * @return parameter value without evaluating.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
const Rmatrix33& Parameter::GetRmatrix33() const
{
   throw ParameterException
      ("Parameter: GetRmatrix33(): " + this->GetTypeName() + " has no "
       "implementation of GetRmatrix33().\nMay be an invalid call to this "
       "function.\n");
}


//------------------------------------------------------------------------------
// const Rmatrix& GetRmatrix() const
//------------------------------------------------------------------------------
/**
 * @return parameter value without evaluating.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
const Rmatrix& Parameter::GetRmatrix() const
{
   throw ParameterException
      ("Parameter: GetRmatrix(): " + this->GetTypeName() + " has no "
       "implementation of GetRmatrix().\nMay be an invalid call to this "
       "function.\n");
}


//------------------------------------------------------------------------------
// const std::string& GetString() const
//------------------------------------------------------------------------------
/**
 * @return parameter value without evaluating.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
const std::string& Parameter::GetString() const
{
   throw ParameterException
      ("Parameter: GetString(): " + this->GetTypeName() + " has no "
       "implementation of GetString().\nMay be an invalid call to this "
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
// void SetRmatrix66(const Rmatrix66 &mat)
//------------------------------------------------------------------------------
/**
 * Sets Rmatrix66 value of parameter.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
void Parameter::SetRmatrix66(const Rmatrix66 &mat)
{
   throw ParameterException
      ("Parameter: SetRmatrix66(): " + this->GetTypeName() + " has no "
       "implementation of SetRmatrix66().\nMay be an invalid call to this "
       "function.\n");
}


//------------------------------------------------------------------------------
// void SetRmatrix33(const Rmatrix33 &mat)
//------------------------------------------------------------------------------
/**
 * Sets Rmatrix33 value of parameter.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
void Parameter::SetRmatrix33(const Rmatrix33 &mat)
{
   throw ParameterException
      ("Parameter: SetRmatrix33(): " + this->GetTypeName() + " has no "
       "implementation of SetRmatrix33().\nMay be an invalid call to this "
       "function.\n");
}


//------------------------------------------------------------------------------
// void SetRmatrix(const Rmatrix &mat)
//------------------------------------------------------------------------------
/**
 * Sets Rmatrix value of parameter.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
void Parameter::SetRmatrix(const Rmatrix &mat)
{
   throw ParameterException
      ("Parameter: SetRmatrix(): " + this->GetTypeName() + " has no "
       "implementation of SetRmatrix().\nMay be an invalid call to this "
       "function.\n");
}


//------------------------------------------------------------------------------
// void SetString(const std::string &val)
//------------------------------------------------------------------------------
/**
 * Sets string value of parameter.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
void Parameter::SetString(const std::string &val)
{
   throw ParameterException
      ("Parameter: SetString(): " + this->GetTypeName() + " has no "
       "implementation of SetString().\nMay be an invalid call to this "
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
// const Rvector6& EvaluateRvector6()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated parameter value.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
const Rvector6& Parameter::EvaluateRvector6()
{
   throw ParameterException
      ("Parameter: EvaluateRvector6(): " + this->GetTypeName() + " has no "
       "implementation of EvaluateRvector6().\nMay be an invalid call to this "
       "function.\n");
}


//------------------------------------------------------------------------------
// const Rmatrix66& EvaluateRmatrix66()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated parameter value.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
const Rmatrix66& Parameter::EvaluateRmatrix66()
{
   throw ParameterException
      ("Parameter: EvaluateRmatrix66(): " + this->GetTypeName() + " has no "
       "implementation of EvaluateRmatrix66().\nMay be an invalid call to this "
       "function.\n");
}


//------------------------------------------------------------------------------
// const Rmatrix33& EvaluateRmatrix33()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated parameter value.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
const Rmatrix33& Parameter::EvaluateRmatrix33()
{
   throw ParameterException
      ("Parameter: EvaluateRmatrix33(): " + this->GetTypeName() + " has no "
       "implementation of EvaluateRmatrix33().\nMay be an invalid call to this "
       "function.\n");
}


//------------------------------------------------------------------------------
// const Rmatrix& EvaluateRmatrix()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated parameter value.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
const Rmatrix& Parameter::EvaluateRmatrix()
{
   throw ParameterException
      ("Parameter: EvaluateRmatrix(): " + this->GetTypeName() + " has no "
       "implementation of EvaluateRmatrix().\nMay be an invalid call to this "
       "function.\n");
}


//------------------------------------------------------------------------------
// const std::string& EvaluateString()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated parameter value.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
const std::string& Parameter::EvaluateString()
{
   throw ParameterException
      ("Parameter: EvaluateString(): " + this->GetTypeName() + " has no "
       "implementation of EvaluateString().\nMay be an invalid call to this "
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
// virtual bool AddRefObject(GmatBase *object, bool replaceName = false)
//------------------------------------------------------------------------------
bool Parameter::AddRefObject(GmatBase *object, bool replaceName)
{
   if (mKey == GmatParam::SYSTEM_PARAM)
      throw ParameterException("Parameter: AddRefObject() should be implemented "
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
//------------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//------------------------------------------------------------------------------
void Parameter::Copy(const GmatBase* orig)
{
   operator=(*((Parameter *)(orig)));
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Parameter::RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Parameter::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif

   // loj: Should I add SPACE_POINT?
   if (type != Gmat::SPACECRAFT && type != Gmat::COORDINATE_SYSTEM)
      return true;
   
   std::string oldExpr = mExpr;
   std::string::size_type pos;
   
   pos = mExpr.find(oldName);
   
   // change expression, if oldName found
   if (pos != mExpr.npos)
   {
      mExpr.replace(pos, oldName.size(), newName);
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("oldExpr=%s, mExpr=%s\n", oldExpr.c_str(), mExpr.c_str());
   #endif
   
   return true;
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
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
   if ((id == DESCRIPTION) || (id == UNIT) || (id == DEP_OBJECT) ||
       (id == COLOR) || (id == EXPRESSION) || id == INITIAL_VALUE)
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
         return GetRefObjectName(mOwnerType);
      else
         return "";
   case INITIAL_VALUE:
      return mInitialValue;
   case EXPRESSION:
      return mExpr;
   case DESCRIPTION:
      return mDesc;
   case UNIT:
      return mUnit;
   case DEP_OBJECT:
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
   #ifdef DEBUG_SET_STRING
   MessageInterface::ShowMessage("Parameter::SetStringParameter() id=%d, value=%s\n",
                                 id, value.c_str());
   #endif
   
   switch (id)
   {
   case OBJECT:
      return SetRefObjectName(mOwnerType, value);
   case INITIAL_VALUE:
      #ifdef DEBUG_SET_STRING
      MessageInterface::ShowMessage("   InitialValue = '%s'\n", value.c_str());
      #endif
      mInitialValue = value;
      return true;
   case EXPRESSION:
      mExpr = value;
      return true;
   case DESCRIPTION:
      mDesc = value;
      return true;
   case UNIT:
      mUnit = value;
      return true;
   case DEP_OBJECT:
      mDepObjectName = value;
      if (mIsCoordSysDependent)
         return SetRefObjectName(Gmat::COORDINATE_SYSTEM, value);
      else if (mIsOriginDependent)
         return SetRefObjectName(Gmat::SPACE_POINT, value);
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


//---------------------------------------------------------------------------
//  const std::string GetCommentLine(Integer which)
//---------------------------------------------------------------------------
/*
 * This method retrives preface comments for Variable and Array.
 *
 * @param which Indicates which comment should be returned
 *              1 = Preface comment from Create line
 *              2 = Preface comment from Initialization line
 * @return Preface comments
 */
//---------------------------------------------------------------------------
const std::string Parameter::GetCommentLine(Integer which)
{
   #ifdef DEBUG_COMMENT
   MessageInterface::ShowMessage
      ("Parameter::GetCommentLine() <%s> which=%d\ncommentLine=<%s>\n"
       "mCommentLine2=<%s>\n", GetName().c_str(), which, commentLine.c_str(),
       mCommentLine2.c_str());
   #endif
   
   if (which == 2)
      return mCommentLine2;
   else
      return commentLine;
}


//---------------------------------------------------------------------------
//  void SetCommentLine(const std::string &comment, Integer which = 0)
//---------------------------------------------------------------------------
/*
 * This method sets preface comments for Variable, Array, and String.
 * Since preface comments from initialization line overrides comments from
 * Create line, there are two comments used in the Parameter class.
 *
 * @param which Indicates which comment should be saved
 *              0 = When this method is called first time,
 *                     it assumes comment is set for Create line.
 *              1 = Preface comment from Create line
 *              2 = Preface comment from Initialization line
 *
 */
//---------------------------------------------------------------------------
void Parameter::SetCommentLine(const std::string &comment, Integer which)
{
   if (which == 0)
   {
      // When this method is called first time, it assumes comment from Create line.
      if (mIsCommentFromCreate)
      {
         #ifdef DEBUG_COMMENT
         MessageInterface::ShowMessage
            ("Parameter::SetCommentLine() <%s> commentLine is set to <%s>\n",
             GetName().c_str(), comment.c_str());
         #endif
         
         commentLine = comment;
         mIsCommentFromCreate = false;
      }
      else
      {
         mCommentLine2 = comment;
         
         #ifdef DEBUG_COMMENT
         MessageInterface::ShowMessage
            ("Parameter::SetCommentLine() <%s> commentLine2 is set to <%s>\n",
             GetName().c_str(), comment.c_str());
         #endif
      }
   }
   else if (which == 1)
   {
      commentLine = comment;
   }
   else if (which == 2)
   {
      mCommentLine2 = comment;
   }
}

