//$Header$
//------------------------------------------------------------------------------
//                              ElapsedDaysParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/13
//
/**
 * Implements elapsed time in days parameter class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElapsedDaysParam.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
ElapsedDaysParam::PARAMETER_TEXT[ElapsedDaysParamCount] =
{
    "InitialEpoch"
};

const Gmat::ParameterType
ElapsedDaysParam::PARAMETER_TYPE[ElapsedDaysParamCount] =
{
    Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ElapsedDaysParam(const std::string &name, GmatBase *obj,
//                 const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
ElapsedDaysParam::ElapsedDaysParam(const std::string &name, GmatBase *obj,
                                   const std::string &desc, const std::string &unit)
    : RealParameter(name, "ElapsedDaysParam", SYSTEM_PARAM, obj, desc, unit)
{
    // GmatBase data
    parameterCount = ElapsedDaysParamCount;

    AddObject(obj);
}

//------------------------------------------------------------------------------
// ElapsedDaysParam(const ElapsedDaysParam &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ElapsedDaysParam::ElapsedDaysParam(const ElapsedDaysParam &param)
    : RealParameter(param)
{
}

//------------------------------------------------------------------------------
// const ElapsedDaysParam& operator=(const ElapsedDaysParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ElapsedDaysParam& ElapsedDaysParam::operator=(const ElapsedDaysParam &right)
{
    if (this != &right)
    {
        RealParameter::operator=(right);
    }
    return *this;
}

//------------------------------------------------------------------------------
// ~ElapsedDaysParam()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ElapsedDaysParam::~ElapsedDaysParam()
{
}

//-------------------------------------
// Inherited methods from RealParameter
//-------------------------------------

//------------------------------------------------------------------------------
// Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * Computes elapsed time from epoch and current time.
 *
 * @return the elapsed seconds
 */
//------------------------------------------------------------------------------
Real ElapsedDaysParam::EvaluateReal()
{
    Evaluate();
    return mValue;
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds reference objects.
 */
//------------------------------------------------------------------------------
bool ElapsedDaysParam::AddObject(GmatBase *obj)
{
    if (obj != NULL)
        return AddRefObject(obj);
    else
        return false;
}

//------------------------------------------------------------------------------
// Integer GetNumObjects() const
//------------------------------------------------------------------------------
/**
 * Retrives number of reference objects.
 */
//------------------------------------------------------------------------------
Integer ElapsedDaysParam::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// void Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 */
//------------------------------------------------------------------------------
void ElapsedDaysParam::Evaluate()
{
    mValue = GetElapsedTimeReal("Days");
}

//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
/**
 * Validates reference objects.
 */
//------------------------------------------------------------------------------
bool ElapsedDaysParam::Validate()
{
    return ValidateRefObjects(this);
}

//--------------------------------------
// Inherited methods from GmatBase
//--------------------------------------

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ElapsedDaysParam::GetParameterType(const Integer id) const
{
   switch (id)
   {
   case INITIAL_EPOCH:
      return ElapsedDaysParam::PARAMETER_TYPE[id - RealParameterCount];
   default:
      return RealParameter::GetParameterType(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedDaysParam::GetParameterTypeString(const Integer id) const
{
   switch (id)
   {
   case INITIAL_EPOCH:
      return RealParameter::PARAM_TYPE_STRING[GetParameterType(id)];
   default:
      return RealParameter::GetParameterTypeString(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedDaysParam::GetParameterText(const Integer id)
{
   switch (id)
   {
   case INITIAL_EPOCH:
      return ElapsedDaysParam::PARAMETER_TEXT[id - RealParameterCount];
   default:
      return RealParameter::GetParameterText(id);
   }
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ElapsedDaysParam::GetParameterID(const std::string &str)
{
   for (int i=0; i<ElapsedDaysParamCount; i++)
   {
      if (str == ElapsedDaysParam::PARAMETER_TEXT[i])
          return i + RealParameterCount;
   }
   
   return RealParameter::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDaysParam::GetRealParameter(const Integer id)
{
   switch (id)
   {
   case INITIAL_EPOCH:
       return GetInitialEpoch();
   default:
       return RealParameter::GetRealParameter(id);
   }
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDaysParam::GetRealParameter(const std::string &label)
{
   if (label == "InitialEpoch")
       return GetInitialEpoch();
   else
       return RealParameter::GetRealParameter(label);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDaysParam::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case INITIAL_EPOCH:
       SetInitialEpoch(value);
       return value;
   default:
       return RealParameter::SetRealParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std:string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDaysParam::SetRealParameter(const std::string &label, const Real value)
{
    if (label == "InitialEpoch")
    {
        SetInitialEpoch(value);
        return value;
    }
    else
    {
       return RealParameter::SetRealParameter(label, value);
    }
}
