//$Header$
//------------------------------------------------------------------------------
//                                  RealParameter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/08
//
/**
 * Defines base class of Real parameters.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "RealParameter.hpp"


//---------------------------------
// static data
//---------------------------------
const std::string
RealParameter::PARAMETER_TEXT[RealParameterCount] =
{
    "Param1"
}; 

const Gmat::ParameterType
RealParameter::PARAMETER_TYPE[RealParameterCount] =
{
    Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RealParameter(const std::string &name, const std::string &typeStr,
//               ParameterKey key, GmatBase *obj, const std::string &desc)
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
 *
 * @exception <ParameterException> thrown if parameter name has blank spaces
 */
//------------------------------------------------------------------------------
RealParameter::RealParameter(const std::string &name, const std::string &typeStr,
                             ParameterKey key, GmatBase *obj,
                             const std::string &desc, const std::string &unit)
    : Parameter(name, typeStr, key, obj, desc, unit)
{  
    mValue = REAL_PARAMETER_UNDEFINED;
    // GmatBase data
    parameterCount = RealParameterCount;
}

//------------------------------------------------------------------------------
// RealParameter(const RealParameter &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the object being copied.
 */
//------------------------------------------------------------------------------
RealParameter::RealParameter(const RealParameter &param)
    : Parameter(param)
{
    mValue = param.mValue;
}

//------------------------------------------------------------------------------
// RealParameter& operator= (const RealParameter& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
RealParameter& RealParameter::operator= (const RealParameter& right)
{
    if (this != &right)
    {
        Parameter::operator=(right);
        mValue = right.mValue;
    }

    return *this;
}

//------------------------------------------------------------------------------
// ~RealParameter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RealParameter::~RealParameter()
{
}

//------------------------------------------------------------------------------
// bool operator==(const RealParameter &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool RealParameter::operator==(const RealParameter &right) const
{
    return Parameter::operator==(right);
}

//------------------------------------------------------------------------------
// bool operator!=(const RealParameter &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool RealParameter::operator!=(const RealParameter &right) const
{
    return Parameter::operator!=(right);
}

//------------------------------------------------------------------------------
// Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * Evaluates Real value of parameter. The derived class should implement this
 * method.
 */
//------------------------------------------------------------------------------
Real RealParameter::EvaluateReal()
{
    return REAL_PARAMETER_UNDEFINED;
}


//------------------------------------------------------------------------------
// Real GetReal() const
//------------------------------------------------------------------------------
/**
 * Retrieves Real value of parameter without ev
 */
//------------------------------------------------------------------------------
Real RealParameter::GetReal() const
{
    return mValue;
}

//------------------------------------
// Inherited methods from Parameter
//------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* RealParameter::GetParameterList() const
{
    return PARAMETER_TEXT;
}


//------------------------------------
// Inherited methods from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType RealParameter::GetParameterType(const Integer id) const
{
    switch (id)
    {
    case PARAM_1:
        return RealParameter::PARAMETER_TYPE[id];
    default:
        return GmatBase::GetParameterType(id);
    }
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string RealParameter::GetParameterTypeString(const Integer id) const
{
    switch (id)
    {
    case PARAM_1:
        return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
    default:
        return GmatBase::GetParameterTypeString(id);
    }
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
std::string RealParameter::GetParameterText(const Integer id)
{
    switch (id)
    {
    case PARAM_1:
        return PARAMETER_TEXT[id];
    default:
        return Parameter::GetParameterText(id);
    }
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
Integer RealParameter::GetParameterID(const std::string str)
{
    for (int i=0; i<RealParameterCount; i++)
    {
        if (str == PARAMETER_TEXT[i])
            return i;
    }
   
    return Parameter::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id)
//------------------------------------------------------------------------------
Real RealParameter::GetRealParameter(const Integer id)
{
    switch (id)
    {
    case PARAM_1:
        return mValue;
    default:
        return Parameter::GetRealParameter(id);
    }
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real RealParameter::GetRealParameter(const std::string &label)
{
   if (label == "Param1")
       return mValue;
   else
       return RealParameter::GetRealParameter(label);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real RealParameter::SetRealParameter(const Integer id, const Real value)
{
    switch (id)
    {
    case PARAM_1:
        mValue = value;
        return mValue;
    default:
        return Parameter::SetRealParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std:string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real RealParameter::SetRealParameter(const std::string &label, const Real value)
{
    if (label == "Param1")
    {
        mValue = value;
        return value;
    }
    else
    {
       return Parameter::SetRealParameter(label, value);
    }
}
