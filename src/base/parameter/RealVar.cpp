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
#include "ParameterException.hpp"
#include <sstream>


//---------------------------------
// static data
//---------------------------------
const std::string
RealVar::PARAMETER_TEXT[RealVarParamCount - ParameterParamCount] =
{
   "Param1"
}; 

const Gmat::ParameterType
RealVar::PARAMETER_TYPE[RealVarParamCount - ParameterParamCount] =
{
   Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RealVar(const std::string &name, const std::string &typeStr, 
//         ParameterKey key, GmatBase *obj, const std::string &desc,
//         bool isTimeParam)
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
   mRealValue = REAL_PARAMETER_UNDEFINED;
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
   mRealValue = copy.mRealValue;
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
      mRealValue = right.mRealValue;
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

//loj: 9/7/04 added
//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return parameter value converted to std::string.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
std::string RealVar::ToString()
{
   std::stringstream ss("");
   ss.precision(10);
   ss << mRealValue;
   return std::string(ss.str());
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
   if (mKey == SYSTEM_PARAM)
   {
      throw ParameterException("Parameter: EvaluateReal() should be implemented "
                               "for Parameter Type:" + GetTypeName());
   }
   else
   {
      //loj: Parse the expresstion, then evaluate
      //loj: Should we use mDesc field instead of creating new exp. field?
      return mRealValue; //loj: temp code
   }

   return REAL_PARAMETER_UNDEFINED;
}


//------------------------------------------------------------------------------
// Real GetReal() const
//------------------------------------------------------------------------------
/**
 * Retrieves Real value of parameter without evaluating.
 */
//------------------------------------------------------------------------------
Real RealVar::GetReal() const
{
   return mRealValue;
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
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string RealVar::GetParameterText(const Integer id) const
{
   if (id >= ParameterParamCount && id < RealVarParamCount)
      return PARAMETER_TEXT[id - ParameterParamCount];
   else
      return Parameter::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str) const
//------------------------------------------------------------------------------
Integer RealVar::GetParameterID(const std::string str) const
{
   for (int i=ParameterParamCount; i<RealVarParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return Parameter::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType RealVar::GetParameterType(const Integer id) const
{
   if (id >= ParameterParamCount && id < RealVarParamCount)
      return PARAMETER_TYPE[id - ParameterParamCount];
   else
      return Parameter::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string RealVar::GetParameterTypeString(const Integer id) const
{
   if (id >= ParameterParamCount && id < RealVarParamCount)
      return PARAM_TYPE_STRING[GetParameterType(id - ParameterParamCount)];
   else
      return Parameter::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
Real RealVar::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case PARAM_1:
      return mRealValue;
   default:
      return Parameter::GetRealParameter(id);
   }
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real RealVar::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real RealVar::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case PARAM_1:
      mRealValue = value;
      return mRealValue;
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
Real RealVar::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}
