//$Header$
//------------------------------------------------------------------------------
//                                  Array
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/09/20
//
/**
 * Implements Array class which handles one or two dimentional array.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "Array.hpp"
#include "ParameterException.hpp"
#include <sstream>


//---------------------------------
// static data
//---------------------------------
const std::string
Array::PARAMETER_TEXT[ArrayParamCount - ParameterParamCount] =
{
   "NumCol",
   "NumRow",
   "Param1",
}; 

const Gmat::ParameterType
Array::PARAMETER_TYPE[ArrayParamCount - ParameterParamCount] =
{
   Gmat::INTEGER_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::REAL_TYPE,
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Array(const std::string &name = "", const std::string &desc = "",
//       const std::string &unit = "")
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> parameter name
 * @param <desc> parameter description
 * @param <unit> parameter unit
 *
 * @exception <ParameterException> thrown if parameter name has blank spaces
 */
//------------------------------------------------------------------------------
Array::Array(const std::string &name, const std::string &desc,
             const std::string &unit)
   : Parameter(name, "Array", USER_PARAM, NULL, desc, unit, false)
{  
   mRmatValue = Rmatrix();
   // GmatBase data
   parameterCount = ArrayParamCount;
}

//------------------------------------------------------------------------------
// Array(const Array &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
Array::Array(const Array &copy)
   : Parameter(copy)
{
   mRmatValue = copy.mRmatValue;
}

//------------------------------------------------------------------------------
// Array& operator= (const Array& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
Array& Array::operator= (const Array& right)
{
   if (this != &right)
   {
      Parameter::operator=(right);
      mRmatValue = right.mRmatValue;
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~Array()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Array::~Array()
{
}

//------------------------------------------------------------------------------
// bool operator==(const Array &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool Array::operator==(const Array &right) const
{
   return Parameter::operator==(right);
}

//------------------------------------------------------------------------------
// bool operator!=(const Array &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool Array::operator!=(const Array &right) const
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
std::string Array::ToString()
{
   std::stringstream ss("");
   ss.precision(10);
   ss << mRmatValue;
   return std::string(ss.str());
}

//------------------------------------------------------------------------------
// Rmatrix GetMatrix() const
//------------------------------------------------------------------------------
/**
 * Retrieves Rmatrix value of parameter without evaluating.
 */
//------------------------------------------------------------------------------
Rmatrix Array::GetMatrix() const
{
   return mRmatValue;
}

//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* Array::GetParameterList() const
{
   return PARAMETER_TEXT;
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string Array::GetParameterText(const Integer id) const
{
   if (id >= ParameterParamCount && id < ArrayParamCount)
      return PARAMETER_TEXT[id - ParameterParamCount];
   else
      return Parameter::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str) const
//------------------------------------------------------------------------------
Integer Array::GetParameterID(const std::string str) const
{
   for (int i=ParameterParamCount; i<ArrayParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return Parameter::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType Array::GetParameterType(const Integer id) const
{
   if (id >= ParameterParamCount && id < ArrayParamCount)
      return PARAMETER_TYPE[id - ParameterParamCount];
   else
      return Parameter::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string Array::GetParameterTypeString(const Integer id) const
{
   if (id >= ParameterParamCount && id < ArrayParamCount)
      return PARAM_TYPE_STRING[GetParameterType(id - ParameterParamCount)];
   else
      return Parameter::GetParameterTypeString(id);
}

//----- Integer parameter

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
Real Array::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case PARAM_1:
      return mRmatValue;
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
Real Array::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real Array::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case PARAM_1:
      mRmatValue = value;
      return mRmatValue;
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
Real Array::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//----- Real parameter

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
Real Array::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case PARAM_1:
      return mRmatValue.GetElement(0, 0); // return 1st element
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
Real Array::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real Array::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case PARAM_1:
      mRmatValue.SetElement(0,0) = value; // Set 1st element
      return value;
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
Real Array::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//----- Rmatrix parameter

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rmatrix& Array::GetRmatrixParameter(const Integer id) const
{
   switch (id)
   {
   case RMAT_VALUE:
      mRmatValue.SetElement(0,0) = value; // Set 1st element
      return value;
   default:
      return Parameter::SetRealParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& Array::GetRvectorParameter(const std::string &label) const
{
}


//------------------------------------------------------------------------------
// const Rmatrix& SetRmatrixParameter(const Integer id,
//                                    const Rmatrix &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rmatrix& Array::SetRmatrixParameter(const Integer id,
                                          const Rmatrix &value)
{
   switch (id)
   {
   case RMAT_VALUE:
      mRmatValue.SetElement(0,0) = value; // Set 1st element
      return value;
   default:
      return Parameter::SetRealParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// const Rmatrix& SetRmatrixParameter(const std::string &label,
//                                    const Rmatrix &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rmatrix& Array::SetRmatrixParameter(const std::string &label,
                                          const Rmatrix &value)
{
}


//------------------------------------
// methods inherited from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* Array::Clone() const
{
   return new Array(*this);
}

