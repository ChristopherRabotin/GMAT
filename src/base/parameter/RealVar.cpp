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
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_REALVAR 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RealVar(const std::string &name, const std::string &typeStr, 
//         GmatParam::ParameterKey key, GmatBase *obj, const std::string &desc,
//         const std::string &unit, GmatParam::DepObject depObj, Gmat::ObjectType,
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
 * @param <ownerType> object type who owns this parameter as property
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NONE)
 * @param <isTimeParam> true if parameter is time related, false otherwise
 */
//------------------------------------------------------------------------------
RealVar::RealVar(const std::string &name, const std::string &typeStr, 
                 GmatParam::ParameterKey key, GmatBase *obj, const std::string &desc,
                 const std::string &unit, GmatParam::DepObject depObj,
                 Gmat::ObjectType ownerType, bool isTimeParam)
   : Parameter(name, typeStr, key, obj, desc, unit, depObj, ownerType, isTimeParam)
{  
   mRealValue = REAL_PARAMETER_UNDEFINED;
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

//------------------------------------
// methods inherited from Parameter
//------------------------------------

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
// Real GetReal() const
//------------------------------------------------------------------------------
/**
 * @return Real value of parameter without evaluating.
 */
//------------------------------------------------------------------------------
Real RealVar::GetReal() const
{
   return mRealValue;
}

//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets real value of parameter.
 */
//------------------------------------------------------------------------------
void RealVar::SetReal(Real val)
{
   mRealValue = val;
}

//------------------------------------
// methods inherited from GmatBase
//------------------------------------


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool RealVar::SetStringParameter(const Integer id, const std::string &value)
{
#if DEBUG_REALVAR
   MessageInterface::ShowMessage("RealVar::SetStringParameter() id=%d, value=%s\n",
                                 id, value.c_str());
#endif
   
   switch (id)
   {
   case EXPRESSION:
      {
         // if expression is just a number set value to expression
         double temp = atof(value.c_str());
         mRealValue = temp;
         return Parameter::SetStringParameter(id, value);
      }
   default:
      return Parameter::SetStringParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool RealVar::SetStringParameter(const std::string &label,
                                 const std::string &value)
{
#if DEBUG_REALVAR
   MessageInterface::ShowMessage("RealVar::SetStringParameter() label=%s value=%s\n",
                                 label.c_str(), value.c_str());
#endif
   
   return SetStringParameter(GetParameterID(label), value);
}

