//$Header$
//------------------------------------------------------------------------------
//                                  RealVar
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
#include "RealVar.hpp"


//---------------------------------
// static data
//---------------------------------
const std::string
RealVar::PARAMETER_TEXT[RealVarParamCount] =
{
    "Param1"
}; 

const Gmat::ParameterType
RealVar::PARAMETER_TYPE[RealVarParamCount] =
{
    Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RealVar(const std::string &name, const std::string &typeStr,
//          ParameterKey key, GmatBase *obj, const std::string &desc,
//          bool isTimeParam)
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
RealVar::RealVar(const std::string &name, const std::string &typeStr,
                 ParameterKey key, GmatBase *obj, const std::string &desc,
                 const std::string &unit, bool isTimeParam)
    : Parameter(name, typeStr, key, obj, desc, unit, isTimeParam)
{  
    mValue = REAL_PARAMETER_UNDEFINED;
    // GmatBase data
    parameterCount = RealVarParamCount;
}

//------------------------------------------------------------------------------
// RealVar(const RealVar &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
RealVar::RealVar(const RealVar &copy)
    : Parameter(copy)
{
    mValue = copy.mValue;
}

//------------------------------------------------------------------------------
// RealVar& operator= (const RealVar& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
RealVar& RealVar::operator= (const RealVar& right)
{
    if (this != &right)
    {
        Parameter::operator=(right);
        mValue = right.mValue;
    }

    return *this;
}

//------------------------------------------------------------------------------
// ~RealVar()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RealVar::~RealVar()
{
}

//------------------------------------------------------------------------------
// bool operator==(const RealVar &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool RealVar::operator==(const RealVar &right) const
{
    return Parameter::operator==(right);
}

//------------------------------------------------------------------------------
// bool operator!=(const RealVar &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool RealVar::operator!=(const RealVar &right) const
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
Real RealVar::EvaluateReal()
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
Real RealVar::GetReal() const
{
    return mValue;
}

//------------------------------------
// Inherited methods from Parameter
//------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* RealVar::GetParameterList() const
{
    return PARAMETER_TEXT;
}


//------------------------------------
// Inherited methods from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType RealVar::GetParameterType(const Integer id) const
{
    switch (id)
    {
    case PARAM_1:
        return RealVar::PARAMETER_TYPE[id - ParameterParamCount];
    default:
        return Parameter::GetParameterType(id);
    }
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string RealVar::GetParameterTypeString(const Integer id) const
{
    switch (id)
    {
    case PARAM_1:
        return Parameter::PARAM_TYPE_STRING[GetParameterType(id)];
    default:
        return Parameter::GetParameterTypeString(id);
    }
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
std::string RealVar::GetParameterText(const Integer id)
{
    switch (id)
    {
    case PARAM_1:
        return PARAMETER_TEXT[id - ParameterParamCount];
    default:
        return Parameter::GetParameterText(id);
    }
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
Integer RealVar::GetParameterID(const std::string str)
{
    for (int i=0; i<RealVarParamCount; i++)
    {
        if (str == PARAMETER_TEXT[i])
            return i + ParameterParamCount;
    }
   
    return Parameter::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id)
//------------------------------------------------------------------------------
Real RealVar::GetRealParameter(const Integer id)
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
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real RealVar::SetRealParameter(const Integer id, const Real value)
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
// Real GetRealParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real RealVar::GetRealParameter(const std::string &label)
{
   if (label == "Param1")
       return mValue;
   else
       return RealVar::GetRealParameter(label);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std:string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real RealVar::SetRealParameter(const std::string &label, const Real value)
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
