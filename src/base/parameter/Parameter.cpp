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
