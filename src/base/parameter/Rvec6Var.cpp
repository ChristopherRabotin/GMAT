//$Header$
//------------------------------------------------------------------------------
//                                  Rvec6Var
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/09
//
/**
 * Defines base class of Rvector6 parameters.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "Rvec6Var.hpp"
#include "ParameterException.hpp"


//---------------------------------
// static data
//---------------------------------
const std::string
Rvec6Var::PARAMETER_TEXT[Rvec6VarParamCount - ParameterParamCount] =
{
    "Param1",
    "Param2",
    "Param3",
    "Param4",
    "Param5",
    "Param6"
}; 

const Gmat::ParameterType
Rvec6Var::PARAMETER_TYPE[Rvec6VarParamCount - ParameterParamCount] =
{
    Gmat::REAL_TYPE,
    Gmat::REAL_TYPE,
    Gmat::REAL_TYPE,
    Gmat::REAL_TYPE,
    Gmat::REAL_TYPE,
    Gmat::REAL_TYPE,
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Rvec6Var(const std::string &name, const std::string &typeStr,
//          ParameterKey key, GmatBase *obj, const std::string &desc,
//          const std::string &unit, bool isTimeParam)
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
Rvec6Var::Rvec6Var(const std::string &name,
                   const std::string &typeStr,
                   ParameterKey key, GmatBase *obj,
                   const std::string &desc,
                   const std::string &unit,
                   bool isTimeParam)
    : Parameter(name, typeStr, key, obj, desc, unit, isTimeParam)
{  
    mRvec6Value = Rvector6::RVECTOR6_UNDEFINED;
    // GmatBase data
    parameterCount = Rvec6VarParamCount;
}

//------------------------------------------------------------------------------
// Rvec6Var(const Rvec6Var &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
Rvec6Var::Rvec6Var(const Rvec6Var &copy)
    : Parameter(copy)
{
    mRvec6Value = copy.mRvec6Value;
}

//------------------------------------------------------------------------------
// Rvec6Var& operator= (const Rvec6Var& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
Rvec6Var& Rvec6Var::operator= (const Rvec6Var& right)
{
    if (this != &right)
    {
        Parameter::operator=(right);
        mRvec6Value = right.mRvec6Value;
    }

    return *this;
}

//------------------------------------------------------------------------------
// ~Rvec6Var()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Rvec6Var::~Rvec6Var()
{
}

//------------------------------------------------------------------------------
// bool operator==(const Rvec6Var &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool Rvec6Var::operator==(const Rvec6Var &right) const
{
    return Parameter::operator==(right);
}

//------------------------------------------------------------------------------
// bool operator!=(const Rvec6Var &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool Rvec6Var::operator!=(const Rvec6Var &right) const
{
    return Parameter::operator!=(right);
}

//------------------------------------------------------------------------------
// Rvector6 EvaluateRvector6()
//------------------------------------------------------------------------------
/**
 * Evaluates Real value of parameter. The derived class should implement this
 * method.
 */
//------------------------------------------------------------------------------
Rvector6 Rvec6Var::EvaluateRvector6()
{
    if (mKey == SYSTEM_PARAM)
    {
        throw ParameterException("Parameter: EvaluateReal() should be implemented "
                                 "for Parameter Type:" + GetTypeName());
    }
    else
    {
        //loj: Parse the expresstion, then evaluate
        //loj: Should we use mDesc field instead of creating new exp. field?
        return mRvec6Value; //loj: temp code
    }

    return Rvector6(REAL_PARAMETER_UNDEFINED, REAL_PARAMETER_UNDEFINED,
                    REAL_PARAMETER_UNDEFINED, REAL_PARAMETER_UNDEFINED,
                    REAL_PARAMETER_UNDEFINED, REAL_PARAMETER_UNDEFINED);
}


//------------------------------------------------------------------------------
// Rvector6 GetRvector6() const
//------------------------------------------------------------------------------
/**
 * Retrieves Rvector6 value of parameter without evaluating.
 */
//------------------------------------------------------------------------------
Rvector6 Rvec6Var::GetRvector6() const
{
    return mRvec6Value;
}

//------------------------------------
// Inherited methods from Parameter
//------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* Rvec6Var::GetParameterList() const
{
    return PARAMETER_TEXT;
}


//------------------------------------
// Inherited methods from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType Rvec6Var::GetParameterType(const Integer id) const
{
    if (id >= ParameterParamCount && id <= Rvec6VarParamCount)
        return PARAMETER_TYPE[id - ParameterParamCount];
    else
        return Parameter::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string Rvec6Var::GetParameterTypeString(const Integer id) const
{
    if (id >= ParameterParamCount && id <= Rvec6VarParamCount)
        return PARAM_TYPE_STRING[GetParameterType(id - ParameterParamCount)];
    else
        return Parameter::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string Rvec6Var::GetParameterText(const Integer id) const
{
    if (id >= ParameterParamCount && id <= Rvec6VarParamCount)
        return PARAMETER_TEXT[id - ParameterParamCount];
    else
        return Parameter::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str) const
//------------------------------------------------------------------------------
Integer Rvec6Var::GetParameterID(const std::string str) const
{
    for (int i=ParameterParamCount; i<Rvec6VarParamCount; i++)
    {
        if (str == PARAMETER_TEXT[i - ParameterParamCount])
            return i;
    }
   
    return Parameter::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
Real Rvec6Var::GetRealParameter(const Integer id) const
{
    if (id >= ParameterParamCount && id <= Rvec6VarParamCount)
        return mRvec6Value(id - ParameterParamCount);
    else
        return Parameter::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real Rvec6Var::SetRealParameter(const Integer id, const Real value)
{
    if (id >= ParameterParamCount && id <= Rvec6VarParamCount)
    {
        mRvec6Value(id - ParameterParamCount) = value;
        return value;
    }
    else
    {
        return Parameter::SetRealParameter(id, value);
    }
}
