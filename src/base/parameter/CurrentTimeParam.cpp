//$Header$
//------------------------------------------------------------------------------
//                              CurrentTimeParam
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
 * Defines A1Mjd type operations.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "CurrentTimeParam.hpp"  // for CurrentTimeParam
#include "A1Mjd.hpp"             // for A1Mjd

//---------------------------------
// static data
//---------------------------------
const std::string
CurrentTimeParam::PARAMETER_TEXT[CurrentTimeParamCount] =
{
   "CurrentTime"
};

const Gmat::ParameterType
CurrentTimeParam::PARAMETER_TYPE[CurrentTimeParamCount] =
{
   Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// CurrentTimeParam(const std::string &name, const std::string &desc,
//                  const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
CurrentTimeParam::CurrentTimeParam(const std::string &name,
                                   const std::string &desc,
                                   const A1Mjd &a1Mjd)
   : Parameter(name, "CurrentTimeParam", SYSTEM_PARAM, desc)
{
   // GmatBase data
   parameterCount = CurrentTimeParamCount;

   // CurrentTimeParam data
   mA1Mjd = a1Mjd;
}

//------------------------------------------------------------------------------
// CurrentTimeParam(const std::string &name, const std::string &desc,
//                  const Real val)
//------------------------------------------------------------------------------
CurrentTimeParam::CurrentTimeParam(const std::string &name,
                                   const std::string &desc,
                                   const Real val)
   : Parameter(name, "CurrentTimeParam", SYSTEM_PARAM, desc)
{
   // GmatBase data
   parameterCount = CurrentTimeParamCount;

   // CurrentTimeParam data
   mA1Mjd.Set(val);
}

//------------------------------------------------------------------------------
// CurrentTimeParam::CurrentTimeParam(const CurrentTimeParam &param)
//------------------------------------------------------------------------------
CurrentTimeParam::CurrentTimeParam(const CurrentTimeParam &param)
   : Parameter(param)
{
   // CurrentTimeParam data
   mA1Mjd = param.mA1Mjd;
}

//------------------------------------------------------------------------------
// CurrentTimeParam& CurrentTimeParam::operator=(const CurrentTimeParam &right)
//------------------------------------------------------------------------------
CurrentTimeParam& CurrentTimeParam::operator=(const CurrentTimeParam &right)
{
   if (this != &right)
   {
      Parameter::operator=(right);
      
      // CurrentTimeParam data
      mA1Mjd = right.mA1Mjd;
   }
   return *this;
}

//------------------------------------------------------------------------------
// CurrentTimeParam::~CurrentTimeParam()
//------------------------------------------------------------------------------
CurrentTimeParam::~CurrentTimeParam()
{
}

//------------------------------------------------------------------------------
// A1Mjd GetValue() const
//------------------------------------------------------------------------------
A1Mjd CurrentTimeParam::GetValue() const
{
   return mA1Mjd;
}

//------------------------------------------------------------------------------
// Real CurrentTimeParam::GetRealValue() const
//------------------------------------------------------------------------------
Real CurrentTimeParam::GetRealValue() const
{
   return mA1Mjd.Get();
}

//------------------------------------------------------------------------------
// void SetValue(const CurrentTimeParam &param)
//------------------------------------------------------------------------------
void CurrentTimeParam::SetValue(const CurrentTimeParam &param)
{
   mA1Mjd = param.mA1Mjd;
}

//------------------------------------------------------------------------------
// void CurrentTimeParam::SetValue(const A1Mjd &a1mjd)
//------------------------------------------------------------------------------
void CurrentTimeParam::SetValue(const A1Mjd &a1mjd)
{
   mA1Mjd = a1mjd;
}

//------------------------------------------------------------------------------
// void CurrentTimeParam::SetValue(const Real &val)
//------------------------------------------------------------------------------
void CurrentTimeParam::SetValue(const Real &val)
{
   mA1Mjd.Set(val);
}

//------------------------------------------------------------------------------
// void operator==(const CurrentTimeParam &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type, name and value are the same as this object.
 */
//------------------------------------------------------------------------------
bool CurrentTimeParam::operator==(const CurrentTimeParam &right) const
{
   if (mA1Mjd != right.GetValue())
      return false;

   return (Parameter::operator==(right));
}

//------------------------------------------------------------------------------
// void operator!=(const CurrentTimeParam &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type, name and value are not the same as
 * this object.
 */
//------------------------------------------------------------------------------
bool CurrentTimeParam::operator!=(const CurrentTimeParam &right) const
{
   return !(*this == right);
}

//------------------------------------
// Inherited methods from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType CurrentTimeParam::GetParameterType(const Integer id) const
{
   switch (id)
   {
   case CURRENT_TIME:
      return CurrentTimeParam::PARAMETER_TYPE[id];
   default:
      return GmatBase::GetParameterType(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string CurrentTimeParam::GetParameterTypeString(const Integer id) const
{
   switch (id)
   {
   case CURRENT_TIME:
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   default:
      return GmatBase::GetParameterTypeString(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
std::string CurrentTimeParam::GetParameterText(const Integer id)
{
   switch (id)
   {
   case CURRENT_TIME:
      return CurrentTimeParam::PARAMETER_TEXT[id];
   default:
      return GmatBase::GetParameterText(id);
   }
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
Integer CurrentTimeParam::GetParameterID(const std::string str)
{
   for (int i=0; i<CurrentTimeParamCount; i++)
   {
      if (str == CurrentTimeParam::PARAMETER_TEXT[i])
          return i;
   }
   
   return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id)
//------------------------------------------------------------------------------
Real CurrentTimeParam::GetRealParameter(const Integer id)
{
   switch (id)
   {
   case CURRENT_TIME:
      return mA1Mjd.Get();
   default:
      return GmatBase::GetRealParameter(id);
   }
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real CurrentTimeParam::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case CURRENT_TIME:
      mA1Mjd.Set(value);
      return mA1Mjd.Get();
   default:
      return GmatBase::SetRealParameter(id, value);
   }
}

//------------------------------------
// Inherited methods from Parameter
//------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* CurrentTimeParam::GetParameterList() const
{
   return PARAMETER_TEXT;
}
