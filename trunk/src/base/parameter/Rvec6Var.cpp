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
#include <sstream>


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Rvec6Var::Rvec6Var(const std::string &name, const std::string &typeStr,
//                    GmatParam::ParameterKey key, GmatBase *obj,
//                    const std::string &desc, const std::string &unit,
//                    GmatParam::DepObject depObj, Gmat::ObjectType ownerType)
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
Rvec6Var::Rvec6Var(const std::string &name, const std::string &typeStr,
                   GmatParam::ParameterKey key, GmatBase *obj,
                   const std::string &desc, const std::string &unit,
                   GmatParam::DepObject depObj, Gmat::ObjectType ownerType)
   : Parameter(name, typeStr, key, obj, desc, unit, depObj, ownerType, false,
               false, false, false)
{
   mRvec6Value = Rvector6::RVECTOR6_UNDEFINED;
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
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return parameter value converted to std::string.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
std::string Rvec6Var::ToString()
{
   return mRvec6Value.ToString();
}

//------------------------------------------------------------------------------
// Rvector6 GetRvector6() const
//------------------------------------------------------------------------------
/**
 * @return Rvector6 value of parameter without evaluating.
 */
//------------------------------------------------------------------------------
Rvector6 Rvec6Var::GetRvector6() const
{
   return mRvec6Value;
}

//------------------------------------------------------------------------------
// virtual void SetRvector6(const Rvector6 &val)
//------------------------------------------------------------------------------
void Rvec6Var::SetRvector6(const Rvector6 &val)
{
   mRvec6Value = val;
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
   if (mKey == GmatParam::SYSTEM_PARAM)
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

