//$Header$
//------------------------------------------------------------------------------
//                              ElapsedTimeParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/22
//
/**
 * Defines elapsed time parameter operations.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElapsedTimeParam.hpp"
#include "ElapsedTime.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
ElapsedTimeParam::PARAMETER_TEXT[ElapsedTimeParamCount] =
{
   "ElapsedTimeInSeconds"
};

const Gmat::ParameterType
ElapsedTimeParam::PARAMETER_TYPE[ElapsedTimeParamCount] =
{
   Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ElapsedTimeParam(const std::string &name, const std::string &desc,
//                  const ElapsedTime &elapsedTime)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <desc> description of the parameter
 * @param <elapsedTime> elapsed time
 */
//------------------------------------------------------------------------------
ElapsedTimeParam::ElapsedTimeParam(const std::string &name,
                                   const std::string &desc,
                                   const ElapsedTime &elapsedTime)
   : Parameter(name, "ElapsedTimeParam", SYSTEM_PARAM, desc)
{
   // GmatBase data
   parameterCount = ElapsedTimeParamCount;

   // ElapsedTimeParam data
   mElapsedTime = elapsedTime;
}

//------------------------------------------------------------------------------
// ElapsedTimeParam(const std::string &name, const std::string &desc,
//                  const Real val)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <desc> description of the parameter
 * @param <val>  Real value of ElapsedTime
 */
//------------------------------------------------------------------------------
ElapsedTimeParam::ElapsedTimeParam(const std::string &name,
                                   const std::string &desc,
                                   const Real val)
   : Parameter(name, "ElapsedTimeParam", SYSTEM_PARAM, desc)
{
   // GmatBase data
   parameterCount = ElapsedTimeParamCount;

   // ElapsedTimeParam data
   mElapsedTime.Set(val);
}

//------------------------------------------------------------------------------
// ElapsedTimeParam(const ElapsedTimeParam &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ElapsedTimeParam::ElapsedTimeParam(const ElapsedTimeParam &param)
   : Parameter(param)
{
   // ElapsedTimeParam data
   mElapsedTime = param.mElapsedTime;
}

//------------------------------------------------------------------------------
// const ElapsedTimeParam& operator=(const ElapsedTimeParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ElapsedTimeParam& ElapsedTimeParam::operator=(const ElapsedTimeParam &right)
{
   if (this != &right)
   {
      Parameter::operator=(right);
           
      // ElapsedTimeParam data
      mElapsedTime = right.mElapsedTime;
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~ElapsedTimeParam()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ElapsedTimeParam::~ElapsedTimeParam()
{
}

//------------------------------------------------------------------------------
// ElapsedTime GetValue() const
//------------------------------------------------------------------------------
/**
 * @return the ElapsedTime object.
 */
//------------------------------------------------------------------------------
ElapsedTime ElapsedTimeParam::GetValue() const
{
   return mElapsedTime;
}

//------------------------------------------------------------------------------
// Real GetRealValue() const
//------------------------------------------------------------------------------
/**
 * @return the Real value ElapsedTime object.
 */
//------------------------------------------------------------------------------
Real ElapsedTimeParam::GetRealValue() const
{
   return mElapsedTime.Get();
}

//------------------------------------------------------------------------------
// void SetValue(const ElapsedTimeParam &param)
//------------------------------------------------------------------------------
/**
 * Sets value of param to ElapsedTime object.
 *
 * @param <param> the parameter to set ElapsedTime object
 */
//------------------------------------------------------------------------------
void ElapsedTimeParam::SetValue(const ElapsedTimeParam &param)
{
   mElapsedTime = param.mElapsedTime;
}

//------------------------------------------------------------------------------
// void SetValue(const ElapsedTime &elapsedTime)
//------------------------------------------------------------------------------
/**
 * Sets elapsedTime to ElapsedTime object.
 *
 * @param <elapsedTime> the ElapsedTime to set ElapsedTime object
 */
//------------------------------------------------------------------------------
void ElapsedTimeParam::SetValue(const ElapsedTime &elapsedTime)
{
   mElapsedTime = elapsedTime;
}

//------------------------------------------------------------------------------
// void SetValue(const A1Mjd &epoch, const A1Mjd &currentTime)
//------------------------------------------------------------------------------
/**
 * Sets ElapsedTime object from epoch and current time.
 *
 * @param <epoch> the epoch
 * @param <currentTime> the current time
 */
//------------------------------------------------------------------------------
void ElapsedTimeParam::SetValue(const A1Mjd &epoch, const A1Mjd &currentTime)
{
   mElapsedTime = currentTime.Subtract(epoch);
}

//------------------------------------------------------------------------------
// void SetValue(const Real &val)
//------------------------------------------------------------------------------
/**
 * Sets ElapsedTime object from Real value.
 *
 * @param <val> the Real value to set ElapsedTime object
 */
//------------------------------------------------------------------------------
void ElapsedTimeParam::SetValue(const Real &val)
{
   mElapsedTime.Set(val);
}

//------------------------------------------------------------------------------
// ElapsedTime Evaluate(const A1Mjd &epoch, const A1Mjd &currentTime)
//------------------------------------------------------------------------------
/**
 * Computes elapsed time from epoch and current and returns ElapsedTime object.
 *
 * @param <epoch> the epoch
 * @param <currentTime> the current time
 *
 * @return the ElapsedTime object
 */
//------------------------------------------------------------------------------
ElapsedTime ElapsedTimeParam::Evaluate(const A1Mjd &epoch, const A1Mjd &currentTime)
{
   mElapsedTime = currentTime.Subtract(epoch);
   return mElapsedTime;
}

//------------------------------------------------------------------------------
// Real Evaluate(const Real &epoch, const Real &currentTime)
//------------------------------------------------------------------------------
/**
 * Computes elapsed time from epoch and current and returns Real value of
 * ElapsedTime object.
 *
 * @param <epoch> the epoch
 * @param <currentTime> the current time
 *
 * @return the Real value of ElapsedTime object
 */
//------------------------------------------------------------------------------
Real ElapsedTimeParam::Evaluate(const Real &epoch, const Real &currentTime)
{
   mElapsedTime.Set(currentTime - epoch);
   return mElapsedTime.Get();
}

//------------------------------------
// Inherited methods from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ElapsedTimeParam::GetParameterType(const Integer id) const
{
   switch (id)
   {
   case ELAPSED_TIME:
      return ElapsedTimeParam::PARAMETER_TYPE[id];
   default:
      return GmatBase::GetParameterType(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedTimeParam::GetParameterTypeString(const Integer id) const
{
   switch (id)
   {
   case ELAPSED_TIME:
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   default:
      return GmatBase::GetParameterTypeString(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedTimeParam::GetParameterText(const Integer id)
{
   switch (id)
   {
   case ELAPSED_TIME:
      return ElapsedTimeParam::PARAMETER_TEXT[id];
   default:
      return GmatBase::GetParameterText(id);
   }
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ElapsedTimeParam::GetParameterID(const std::string str)
{
   for (int i=0; i<ElapsedTimeParamCount; i++)
   {
      if (str == ElapsedTimeParam::PARAMETER_TEXT[i])
          return i;
   }
   
   return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedTimeParam::GetRealParameter(const Integer id)
{
   switch (id)
   {
   case ELAPSED_TIME:
      return mElapsedTime.Get();
   default:
      return GmatBase::GetRealParameter(id);
   }
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedTimeParam::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case ELAPSED_TIME:
      mElapsedTime.Set(value);
      return mElapsedTime.Get();
   default:
      return GmatBase::SetRealParameter(id, value);
   }
}

//------------------------------------
// Inherited methods from Parameter
//------------------------------------

//loj: added since review ready
//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* ElapsedTimeParam::GetParameterList() const
{
   return PARAMETER_TEXT;
}
