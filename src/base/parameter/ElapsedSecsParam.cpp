//$Header$
//------------------------------------------------------------------------------
//                              ElapsedSecsParam
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
 * Implements elapsed time in seconds parameter class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElapsedSecsParam.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
ElapsedSecsParam::PARAMETER_TEXT[ElapsedSecsParamCount] =
{
    "InitialEpoch"
};

const Gmat::ParameterType
ElapsedSecsParam::PARAMETER_TYPE[ElapsedSecsParamCount] =
{
    Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ElapsedSecsParam(const std::string &name, GmatBase *obj,
//                  const std::string &desc, const std::string &unit)
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
ElapsedSecsParam::ElapsedSecsParam(const std::string &name, GmatBase *obj,
                                   const std::string &desc, const std::string &unit)
    : RealParameter(name, "ElapsedSecsParam", SYSTEM_PARAM, obj, desc, unit, true)
{
    // GmatBase data
    parameterCount = ElapsedSecsParamCount;

    AddObject(obj);
}

//------------------------------------------------------------------------------
// ElapsedSecsParam(const ElapsedSecsParam &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ElapsedSecsParam::ElapsedSecsParam(const ElapsedSecsParam &copy)
    : RealParameter(copy)
{
}

//------------------------------------------------------------------------------
// const ElapsedSecsParam& operator=(const ElapsedSecsParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ElapsedSecsParam& ElapsedSecsParam::operator=(const ElapsedSecsParam &right)
{
    if (this != &right)
    {
        RealParameter::operator=(right);
    }
    return *this;
}

//------------------------------------------------------------------------------
// ~ElapsedSecsParam()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ElapsedSecsParam::~ElapsedSecsParam()
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
Real ElapsedSecsParam::EvaluateReal()
{
    Evaluate();
    return mValue;
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// Integer GetNumObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer ElapsedSecsParam::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* ElapsedSecsParam::GetObject(const std::string &objTypeName)
{
    return GetRefObject(objTypeName);
}

//------------------------------------------------------------------------------
// virtual bool SetObject(Gmat::ObjectType objType, const std::string &objName,
//                        GmatBase *obj
//------------------------------------------------------------------------------
/**
 * Sets reference object.
 *
 * @return true if the object has been set.
 */
//------------------------------------------------------------------------------
bool ElapsedSecsParam::SetObject(Gmat::ObjectType objType,
                                 const std::string &objName,
                                 GmatBase *obj)
{
    if (obj != NULL)
        return SetRefObject(objType, objName, obj);
    else
        return false;
}

//------------------------------------------------------------------------------
// virtual bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds reference objects.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool ElapsedSecsParam::AddObject(GmatBase *obj)
{
    if (obj != NULL)
    {
        if (AddRefObject(obj))
            ManageObject(obj);
        
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
/**
 * Validates reference objects.
 *
 * @return true if all objects are set; false otherwise
 */
//------------------------------------------------------------------------------
bool ElapsedSecsParam::Validate()
{
    return ValidateRefObjects(this);
}

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool ElapsedSecsParam::Evaluate()
{
    mValue = GetElapsedTimeReal("Secs");
    
    if (mValue == TIME_REAL_UNDEFINED)
        return false;
    else
        return true;
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
Gmat::ParameterType ElapsedSecsParam::GetParameterType(const Integer id) const
{
   switch (id)
   {
   case INITIAL_EPOCH:
      return ElapsedSecsParam::PARAMETER_TYPE[id - RealParameterCount];
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
std::string ElapsedSecsParam::GetParameterTypeString(const Integer id) const
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
std::string ElapsedSecsParam::GetParameterText(const Integer id)
{
   switch (id)
   {
   case INITIAL_EPOCH:
      return ElapsedSecsParam::PARAMETER_TEXT[id - RealParameterCount];
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
Integer ElapsedSecsParam::GetParameterID(const std::string &str)
{
   for (int i=0; i<ElapsedSecsParamCount; i++)
   {
      if (str == ElapsedSecsParam::PARAMETER_TEXT[i])
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
Real ElapsedSecsParam::GetRealParameter(const Integer id)
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
Real ElapsedSecsParam::GetRealParameter(const std::string &label)
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
Real ElapsedSecsParam::SetRealParameter(const Integer id, const Real value)
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
Real ElapsedSecsParam::SetRealParameter(const std::string &label, const Real value)
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
