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

#if !defined __UNIT_TEST__
#include "Moderator.hpp"
#endif

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
    "Object",
    "Color",
};

const Gmat::ParameterType
Parameter::PARAMETER_TYPE[ParameterParamCount] =
{
    Gmat::STRING_TYPE,
    Gmat::STRING_TYPE,
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Parameter(const std::string &name, const std::string &typeStr,
//           ParameterKey key, GmatBase *obj, const std::string &desc
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

    //loj: should "" or " " be allowed?
    if ((name != "" && name != " "))
    {
        //if name has blank spaces
        if (name.find(' ') < name.npos)
            throw ParameterException
                ("Parameter: parameter name cannot have blank space: " + name);
    }
   
    if (desc == "")
        mDesc = std::string(name);
    else
        mDesc = desc;

    mUnit = unit;
    mIsTimeParam = isTimeParam;
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
    mDesc = copy.mDesc;
    mUnit = copy.mUnit;
    mIsTimeParam = copy.mIsTimeParam;
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
        mIsTimeParam = right.mIsTimeParam;
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
 * Retrieves parameter key.
 *
 * @return enumeration value of parameter key.
 */
//------------------------------------------------------------------------------
Parameter::ParameterKey Parameter::GetKey() const
{
    return mKey;
}

//------------------------------------------------------------------------------
// std::string GetDesc() const
//------------------------------------------------------------------------------
/**
 * Retrieves parameter description.
 *
 * @return parameter description.
 */
//------------------------------------------------------------------------------
std::string Parameter::GetDesc() const
{
    return mDesc;
}

//------------------------------------------------------------------------------
// std::string GetUnit() const
//------------------------------------------------------------------------------
/**
 * Retrieves parameter unit.
 *
 * @return parameter description.
 */
//------------------------------------------------------------------------------
std::string Parameter::GetUnit() const
{
    return mUnit;
}

//------------------------------------------------------------------------------
// bool IsTimeParameter() const
//------------------------------------------------------------------------------
/**
 * Retrieves parameter unit.
 *
 * @return parameter description.
 */
//------------------------------------------------------------------------------
bool Parameter::IsTimeParameter() const
{
    return mIsTimeParam;
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
// void SetUnit(cosnt std::string &unit)
//------------------------------------------------------------------------------
/**
 * Sets parameter unit.
 *
 * @param <unit> unit of parameter.
 */
//------------------------------------------------------------------------------
void Parameter::SetUnit(const std::string &unit)
{
    mUnit = unit;
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
// bool EvaluateBoolean()
//------------------------------------------------------------------------------
/**
 * Evaluates parameter value.
 *
 * @return false if derived class doesn't implement this
 */
//------------------------------------------------------------------------------
bool Parameter::EvaluateBoolean()
{
    return false;
}

//------------------------------------------------------------------------------
// Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * Evaluates parameter value.
 *
 * @return REAL_PARAMETER_UNDEFINED if derived class doesn't implement this
 */
//------------------------------------------------------------------------------
Real Parameter::EvaluateReal()
{
    return REAL_PARAMETER_UNDEFINED;
}

//------------------------------------------------------------------------------
// Rvector6 EvaluateRvector6()
//------------------------------------------------------------------------------
/**
 * Evaluates parameter value.
 *
 * @return RVECTOR6_PARAMETER_UNDEFINED if derived class doesn't implement this
 */
//------------------------------------------------------------------------------
Rvector6 Parameter::EvaluateRvector6()
{
    return Rvector6::RVECTOR6_UNDEFINED;
}

//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* Parameter::GetParameterList() const
{
    return NULL;
}

//------------------------------------------------------------------------------
// StringArray& GetObjectTypeNames()
//------------------------------------------------------------------------------
StringArray& Parameter::GetObjectTypeNames()
{
    return mObjectTypeNames;
}

//------------------------------------------------------------------------------
// StringArray& GetObjectNames()
//------------------------------------------------------------------------------
StringArray& Parameter::GetObjectNames()
{
    return mObjectNames;
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* Parameter::GetObject(const std::string &objTypeName)
{
    return NULL;
}

//------------------------------------------------------------------------------
// bool SetObject(Gmat::ObjectType objType,
//                const std::string &objName, GmatBase *obj)
//------------------------------------------------------------------------------
bool Parameter::SetObject(Gmat::ObjectType objType,
                          const std::string &objName, GmatBase *obj)
{
    return false;
}

//------------------------------------------------------------------------------
// bool AddObject(const std::string &name)
//------------------------------------------------------------------------------
bool Parameter::AddObject(const std::string &name)
{
    bool status = false;

#if !defined __UNIT_TEST__
    
    Moderator *theModerator = Moderator::Instance();
    
    //MessageInterface::ShowMessage("Parameter::AddObject entered: "
    //                              "name = %s typename = %s\n", name.c_str());
                                    
    if (name != "")
    {
        //loj: should check name first to see if it's already added - do this later
        
        GmatBase *obj = theModerator->GetConfiguredItem(name);
        if (obj != NULL)
        {
            //mObjectNames.push_back(name);
            //mObjectTypeNames.push_back(obj->GetTypeName());
            //mNumObjects = mObjectNames.size();
            AddObject(obj);
            status = true;
        }
    }
#endif
    return status;
}

//------------------------------------------
// methods All SYSTEM_PARAM should implement
//------------------------------------------

//------------------------------------------------------------------------------
// virtual bool AddObject(GmatBase *object)
//------------------------------------------------------------------------------
bool Parameter::AddObject(GmatBase *object)
{
    if (mKey == SYSTEM_PARAM)
        throw ParameterException("Parameter: AddObject() should be implemented "
                                 "for Parameter Type:" + GetTypeName());

    return false;
}

//------------------------------------------------------------------------------
// virtual Integer GetNumObjects() const
//------------------------------------------------------------------------------
Integer Parameter::GetNumObjects() const
{
    if (mKey == SYSTEM_PARAM)
        throw ParameterException("Parameter: GetNumObjects() should be implemented"
                                 "for Parameter Type: " + GetTypeName());

    return 0;
}

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
bool Parameter::Evaluate()
{
    if (mKey == SYSTEM_PARAM)
        throw ParameterException("Parameter: Evaluate() should be implemented "
                                 "for Parameter Type: " + GetTypeName());

    return false;
}

//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
bool Parameter::Validate()
{
    if (mKey == SYSTEM_PARAM)
        throw ParameterException("Parameter: Validate() should be implemented "
                                 "for Parameter Type: " + GetTypeName());

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
    if (id >= OBJECT && id < ParameterParamCount)
        return PARAMETER_TEXT[id];
    else
        return GmatBase::GetParameterText(id);
    
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Parameter::GetParameterID(const std::string &str) const
{
   for (int i=0; i<ParameterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i])
          return i;
   }
   
   return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType Parameter::GetParameterType(const Integer id) const
{
    if (id >= OBJECT && id < ParameterParamCount)
        return PARAMETER_TYPE[id];
    else
        return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string Parameter::GetParameterTypeString(const Integer id) const
{
    if (id >= OBJECT && id < ParameterParamCount)
        return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
    else
       return GmatBase::GetParameterTypeString(id);
    
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string Parameter::GetStringParameter(const Integer id) const
{
    switch (id)
    {
    case OBJECT: //loj: return first object name for now
        if (mNumObjects > 0)
            return mObjectNames[0];
        else
            return GmatBase::GetStringParameter(id);
    case COLOR:
        return mColorName;
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
    case OBJECT:
        return AddObject(value);
    case COLOR:
        mColorName = value;
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
//      MessageInterface::ShowMessage("Parameter::SetStringParameter() entered: "
//                                    "label = " + label + ", value = " + value + "\n");
    return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void Parameter::ManageObject(GmatBase *obj)
//------------------------------------------------------------------------------
void Parameter::ManageObject(GmatBase *obj)
{
    mObjectNames.push_back(obj->GetName());
    mObjectTypeNames.push_back(obj->GetTypeName());
    mNumObjects = mObjectNames.size();
    //MessageInterface::ShowMessage("Parameter::ManageObject() param name = %s mNumObjects = %d\n",
    //                              obj->GetName().c_str(), mNumObjects);
}

