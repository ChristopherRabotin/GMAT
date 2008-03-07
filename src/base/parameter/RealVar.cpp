//$Id$
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

#include "RealVar.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"          // for GmatStringUtil::ToReal()
#include "GmatGlobal.hpp"          // for Global settings
#include "MessageInterface.hpp"
//#include <sstream>

//#define DEBUG_REAL_VAR 1

//---------------------------------
// static data
//---------------------------------
const std::string
RealVar::PARAMETER_TEXT[RealVarParamCount - ParameterParamCount] =
{
   "Value"
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
// RealVar(const std::string &name, const std::string &valStr,
//         const std::string &typeStr, GmatParam::ParameterKey key, GmatBase *obj,
//         const std::string &desc, const std::string &unit, GmatParam::DepObject depObj,
//         Gmat::ObjectType, bool isTimeParam)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> parameter name
 * @param <valStr>  value string of parameter
 * @param <typeStr>  parameter type string
 * @param <key>  parameter key (SYSTEM_PARAM, USER_PARAM, etc)
 * @param <obj>  reference object pointer
 * @param <desc> parameter description
 * @param <unit> parameter unit
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NONE)
 * @param <ownerType> object type who owns this parameter as property
 * @param <isTimeParam> true if parameter is time related, false otherwise
 */
//------------------------------------------------------------------------------
RealVar::RealVar(const std::string &name, const std::string &valStr,
                 const std::string &typeStr, GmatParam::ParameterKey key,
                 GmatBase *obj, const std::string &desc, const std::string &unit,
                 GmatParam::DepObject depObj, Gmat::ObjectType ownerType,
                 bool isTimeParam, bool isSettable)
   : Parameter(name, typeStr, key, obj, desc, unit, depObj, ownerType, isTimeParam,
               isSettable, true, true)
{  
   //mRealValue = REAL_PARAMETER_UNDEFINED;
   mRealValue = 0.0;
   Real rval;
   mIsNumberEquation = true;
   
   if (GmatStringUtil::ToReal(valStr, &rval))
   {
      mRealValue = rval;
      mExpr = valStr;
      mIsNumberEquation = false;
   }
   else
   {
      for (UnsignedInt i=0; i<mExpr.size(); i++)
      {
         if (isalpha(mExpr[i]))
         {
            mIsNumberEquation = false;
            break;
         }
      }
   }
   
   mReturnType = Gmat::REAL_TYPE;
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
   //MessageInterface::ShowMessage("==> RealVar::~RealVar()\n");
}

//------------------------------------------------------------------------------
// bool operator==(const RealVar &right) const
//------------------------------------------------------------------------------
/**
 * Equal operator.
 *
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
 * Not equal operator.
 *
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
// bool Initialize()
//------------------------------------------------------------------------------
bool RealVar::Initialize()
{
   //mRealValue = REAL_PARAMETER_UNDEFINED;
   return true;
}


//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * Converts parameter real value to string.
 *
 * @return parameter value converted to std::string.
 */
//------------------------------------------------------------------------------
std::string RealVar::ToString()
{
   // use default global precision to convert to string (loj: 2008.03.05)
   return GmatStringUtil::ToString(mRealValue, false, false, false, 
                                   GmatGlobal::DATA_PRECISION, 1);
   
   //std::stringstream ss("");
   //ss.precision(10);
   //ss << mRealValue;
   //return std::string(ss.str());
}


//------------------------------------------------------------------------------
// Real GetReal() const
//------------------------------------------------------------------------------
/**
 * Retrives real value of parameter.
 *
 * @return Real value of parameter without evaluating.
 */
//------------------------------------------------------------------------------
Real RealVar::GetReal() const
{
   #if DEBUG_REAL_VAR
   MessageInterface::ShowMessage
      ("RealVar::GetReal() name=%s, returning val=%.18f\n", GetName().c_str(), 
      mRealValue);
   #endif
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
   #if DEBUG_REAL_VAR
   MessageInterface::ShowMessage
      ("RealVar::SetReal(Real val) name=%s, val=%.18f\n", GetName().c_str(), val);
   #endif
   
   mRealValue = val;
   
   // use default global precision to convert to string (loj: 2008.03.05)
   mExpr = GmatStringUtil::ToString(mRealValue, false, false, false,
                                    GmatGlobal::DATA_PRECISION, 1);
   
   //std::stringstream ss("");
   //ss.precision(10);
   //ss << mRealValue;
   //mExpr = ss.str();
   
   #if DEBUG_REAL_VAR
   MessageInterface::ShowMessage
      ("RealVar::SetReal(Real val) mExpr=%s\n", mExpr.c_str());
   #endif
}

//------------------------------------
// methods inherited from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer RealVar::GetParameterID(const std::string &str) const
{
   for (int i=ParameterParamCount; i<RealVarParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return Parameter::GetParameterID(str);
}


//-------------------------------------------------------------------------------
// virtual Real GetRealParameter(const Integer id) const
//-------------------------------------------------------------------------------
Real RealVar::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case VALUE:
      return mRealValue;
   default:
      return Parameter::GetRealParameter(id);
   }
}


//-------------------------------------------------------------------------------
// virtual Real GetRealParameter(const std::string &label) const
//-------------------------------------------------------------------------------
Real RealVar::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//-------------------------------------------------------------------------------
// virtual Real SetRealParameter(const Integer id, const Real value)
//-------------------------------------------------------------------------------
Real RealVar::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case VALUE:
      SetReal(value);
      return value;
   default:
      return Parameter::GetRealParameter(id);
   }
}


//-------------------------------------------------------------------------------
// virtual Real SetRealParameter(const std::string &label, const Real value)
//-------------------------------------------------------------------------------
Real RealVar::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, throw if the parameter is not stored.
 */
//------------------------------------------------------------------------------
bool RealVar::SetStringParameter(const Integer id, const std::string &value)
{
   #if DEBUG_REAL_VAR
   MessageInterface::ShowMessage
      ("RealVar::SetStringParameter() name:%s, id=%d, value=%s\n",
       GetName().c_str(), id, value.c_str());
   #endif
   
   switch (id)
   {
   case EXPRESSION:
      {
         // if value is just a number, convert and set to real value
         mIsNumberEquation = true;
         Real temp;
         if (GmatStringUtil::ToReal(value, &temp))
         {
            mRealValue = temp;
            mIsNumberEquation = false;
            
            #if DEBUG_REAL_VAR
            MessageInterface::ShowMessage("mRealValue set to %f\n", mRealValue);
            #endif
         }
         else
         {
            for (UnsignedInt i=0; i<value.size(); i++)
            {
               if (isalpha(value[i]))
               {
                  mIsNumberEquation = false;
                  break;
               }
            }
         }
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
/**
 * Change the value of a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool RealVar::SetStringParameter(const std::string &label,
                                 const std::string &value)
{
   #if DEBUG_REAL_VAR > 1
   MessageInterface::ShowMessage("RealVar::SetStringParameter() label=%s value=%s\n",
                                 label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}

