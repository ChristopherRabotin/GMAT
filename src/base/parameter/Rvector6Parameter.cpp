//$Header$
//------------------------------------------------------------------------------
//                                  Rvector6Parameter
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
#include "Rvector6Parameter.hpp"


//---------------------------------
// static data
//---------------------------------
const std::string
Rvector6Parameter::PARAMETER_TEXT[Rvector6ParameterCount] =
{
    "Param1",
    "Param2",
    "Param3",
    "Param4",
    "Param5",
    "Param6"
}; 

const Gmat::ParameterType
Rvector6Parameter::PARAMETER_TYPE[Rvector6ParameterCount] =
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
// Rvector6Parameter(const std::string &name, const std::string &typeStr,
//                   ParameterKey key, GmatBase *obj, const std::string &desc,
//                   const std::string &unit, bool isTimeParam)
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
Rvector6Parameter::Rvector6Parameter(const std::string &name,
                                     const std::string &typeStr,
                                     ParameterKey key, GmatBase *obj,
                                     const std::string &desc,
                                     const std::string &unit,
                                     bool isTimeParam)
    : Parameter(name, typeStr, key, obj, desc, unit, isTimeParam)
{  
    mValue = Rvector6::RVECTOR6_UNDEFINED;
    // GmatBase data
    parameterCount = Rvector6ParameterCount;
}

//------------------------------------------------------------------------------
// Rvector6Parameter(const Rvector6Parameter &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
Rvector6Parameter::Rvector6Parameter(const Rvector6Parameter &copy)
    : Parameter(copy)
{
    mValue = copy.mValue;
}

//------------------------------------------------------------------------------
// Rvector6Parameter& operator= (const Rvector6Parameter& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
Rvector6Parameter& Rvector6Parameter::operator= (const Rvector6Parameter& right)
{
    if (this != &right)
    {
        Parameter::operator=(right);
        mValue = right.mValue;
    }

    return *this;
}

//------------------------------------------------------------------------------
// ~Rvector6Parameter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Rvector6Parameter::~Rvector6Parameter()
{
}

//------------------------------------------------------------------------------
// bool operator==(const Rvector6Parameter &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool Rvector6Parameter::operator==(const Rvector6Parameter &right) const
{
    return Parameter::operator==(right);
}

//------------------------------------------------------------------------------
// bool operator!=(const Rvector6Parameter &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool Rvector6Parameter::operator!=(const Rvector6Parameter &right) const
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
Rvector6 Rvector6Parameter::EvaluateRvector6()
{
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
Rvector6 Rvector6Parameter::GetRvector6() const
{
    return mValue;
}

//------------------------------------
// Inherited methods from Parameter
//------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* Rvector6Parameter::GetParameterList() const
{
    return PARAMETER_TEXT;
}


//------------------------------------
// Inherited methods from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType Rvector6Parameter::GetParameterType(const Integer id) const
{
    if (id >= PARAM_1 && id <= PARAM_6)
        return PARAMETER_TYPE[id];
    else
        return Parameter::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string Rvector6Parameter::GetParameterTypeString(const Integer id) const
{
    if (id >= PARAM_1 && id <= PARAM_6)
        return PARAM_TYPE_STRING[GetParameterType(id)];
    else
        return Parameter::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
std::string Rvector6Parameter::GetParameterText(const Integer id)
{
    if (id >= PARAM_1 && id <= PARAM_6)
        return PARAMETER_TEXT[id];
    else
        return Parameter::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
Integer Rvector6Parameter::GetParameterID(const std::string str)
{
    for (int i=0; i<Rvector6ParameterCount; i++)
    {
        if (str == PARAMETER_TEXT[i])
            return i;
    }
   
    return Parameter::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id)
//------------------------------------------------------------------------------
Real Rvector6Parameter::GetRealParameter(const Integer id)
{
    if (id >= PARAM_1 && id <= PARAM_6)
        return mValue(id);
    else
        return Parameter::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real Rvector6Parameter::SetRealParameter(const Integer id, const Real value)
{
    if (id >= PARAM_1 && id <= PARAM_6)
        mValue(id) = value;
    else
        return Parameter::SetRealParameter(id, value);
}
