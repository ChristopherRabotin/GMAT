//$Header: /cygdrive/p/dev/cvs/test/TestParam/MyEtParam.cpp,v 1.1 2003/10/24 15:10:04 ljun Exp $
//------------------------------------------------------------------------------
//                              MyEtParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Author: Linda Jun
// Created: 2003/09/22
//
/**
 * Defines my elapsed time parameter operations.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "MyEtParam.hpp"
#include "ElapsedTime.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string MyEtParam::PARAMETER_TEXT[MyEtParamCount] =
{
   "AnotherElapsedDays"
};

const Gmat::ParameterType
MyEtParam::PARAMETER_TYPE[MyEtParamCount] =
{
   Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// MyEtParam(const std::string &name, const std::string &desc,
//                  const ElapsedTime &elapsedTime, const Real anotherTime)
//------------------------------------------------------------------------------
MyEtParam::MyEtParam(const std::string &name, const std::string &desc,
                     const ElapsedTime &elapsedTime, const Real anotherTime)
   : ElapsedTimeParam(name, desc, elapsedTime)
{
   // GmatBase data
   typeName = "MyEtParam";
   parameterCount = MyEtParamCount;
   
   // MyEtParam data
   mAnotherTime = anotherTime;
}

//------------------------------------------------------------------------------
// MyEtParam(const std::string &name, const std::string &desc,
//                  const Real val)
//------------------------------------------------------------------------------
MyEtParam::MyEtParam(const std::string &name, const std::string &desc,
                     const Real val, const Real anotherTime)
   : ElapsedTimeParam(name, desc, val)
{
   // GmatBase data
   typeName = "MyEtParam";
   parameterCount = MyEtParamCount;

   // MyEtParam data
   mAnotherTime = anotherTime;
}

//------------------------------------------------------------------------------
// MyEtParam(const MyEtParam &param)
//------------------------------------------------------------------------------
MyEtParam::MyEtParam(const MyEtParam &param)
   : ElapsedTimeParam(param)
{
   parameterCount = param.parameterCount;
   mAnotherTime = param.mAnotherTime;
}

//------------------------------------------------------------------------------
// const MyEtParam& operator=(const MyEtParam &param)
//------------------------------------------------------------------------------
const MyEtParam& MyEtParam::operator=(const MyEtParam &param)
{
   if (this != &param)
   {
      ElapsedTimeParam::operator=(param);
      mAnotherTime = param.mAnotherTime;
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~MyEtParam()
//------------------------------------------------------------------------------
MyEtParam::~MyEtParam()
{
}

//------------------------------------------------------------------------------
// Real GetAnotherTimeValue() const
//------------------------------------------------------------------------------
Real MyEtParam::GetAnotherTimeValue() const
{
   return mAnotherTime;
}

//------------------------------------
// Inherited methods from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType MyEtParam::GetParameterType(const Integer id) const
{
   switch (id)
   {
   case ANOTHER_ELAPSED_TIME:
      return MyEtParam::PARAMETER_TYPE[id-ElapsedTimeParamCount];
   default:
      return ElapsedTimeParam::GetParameterType(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string MyEtParam::GetParameterTypeString(const Integer id) const
{
   switch (id)
   {
   case ANOTHER_ELAPSED_TIME:
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   default:
      return ElapsedTimeParam::GetParameterTypeString(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
std::string MyEtParam::GetParameterText(const Integer id)
{
   switch (id)
   {
   case ANOTHER_ELAPSED_TIME:
      return MyEtParam::PARAMETER_TEXT[id - ElapsedTimeParamCount];
   default:
      return ElapsedTimeParam::GetParameterText(id);
   }
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
Integer MyEtParam::GetParameterID(const std::string str)
{
   for (int i=0; i<MyEtParamCount; i++)
   {
      if (str == MyEtParam::PARAMETER_TEXT[i])
          return i + ElapsedTimeParamCount;
   }
   
   return ElapsedTimeParam::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id)
//------------------------------------------------------------------------------
Real MyEtParam::GetRealParameter(const Integer id)
{
   switch (id)
   {
   case ANOTHER_ELAPSED_TIME:
      return mAnotherTime;
   default:
      return ElapsedTimeParam::GetRealParameter(id);
   }
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real MyEtParam::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case ANOTHER_ELAPSED_TIME:
      mAnotherTime = value;
      return mAnotherTime;
   default:
      return ElapsedTimeParam::SetRealParameter(id, value);
   }
}

