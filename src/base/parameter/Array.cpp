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
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_ARRAY 1

//---------------------------------
// static data
//---------------------------------
const std::string
Array::PARAMETER_TEXT[ArrayParamCount - ParameterParamCount] =
{
   "Size",
   "NumCol",
   "NumRow",
   "RmatValue",
   "SingleValue",
   "RowValue",
   "ColValue",
}; 

const Gmat::ParameterType
Array::PARAMETER_TYPE[ArrayParamCount - ParameterParamCount] =
{
   Gmat::INTEGER_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::RMATRIX_TYPE,
   Gmat::REAL_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
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

//------------------------------------------------
//----- Real parameter need be added to GmatBase
//------------------------------------------------

//----- Integer parameter

//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer row,
//                             const Integer col)
//------------------------------------------------------------------------------
Integer Array::SetIntegerParameter(const Integer id, const Integer row,
                                   const Integer col)
{
   switch (id)
   {
   case SIZE:
      MessageInterface::ShowMessage
         ("Array::SetIntegerParameter() SetSize to %d,%d\n", row, col);
      mRmatValue.SetSize(row, col);
      return row;
   default:
      throw ParameterException
         ("Array::SetIntegerParameter() Unknown Parameter Name" +
          PARAMETER_TEXT[id]);
      //return Parameter::SetIntegerParameter(id, row, col);
   }
}

//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const std:string &label, const Integer row,
//                             const Integer col)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Array::SetIntegerParameter(const std::string &label, const Integer row,
                                   const Integer col)
{
   return SetIntegerParameter(GetParameterID(label), row, col);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer row,
//                       const Integer col) const
//------------------------------------------------------------------------------
Real Array::GetRealParameter(const Integer id, const Integer row,
                             const Integer col) const
{
   switch (id)
   {
   case SINGLE_VALUE:
      return mRmatValue.GetElement(row, col);
   default:
      throw ParameterException
         ("Array::GetRealParameter() Unknown Parameter Name" + PARAMETER_TEXT[id]);
      //return Parameter::GetRealParameter(id, row, col);
   }
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer row,
//                       const Integer col) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Array::GetRealParameter(const std::string &label, const Integer row,
                             const Integer col) const
{
   return GetRealParameter(GetParameterID(label), row, col);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer row,
//                       const Integer col)
//------------------------------------------------------------------------------
Real Array::SetRealParameter(const Integer id, const Real value,
                             const Integer row, const Integer col)
{
   switch (id)
   {
   case ROW_VALUE:
      mRmatValue.SetElement(row, col, value);
      return value;
   default:
      throw ParameterException
         ("Array::SetRealParameter() Unknown Parameter Name" + PARAMETER_TEXT[id]);
      //return Parameter::SetRealParameter(id, value, row, col);
   }
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std:string &label, const Real value,
//                       const Integer row, const Integer col)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Array::SetRealParameter(const std::string &label, const Real value,
                             const Integer row, const Integer col)
{
   return SetRealParameter(GetParameterID(label), value, row, col);
}

//----- Rvector parameter

//------------------------------------------------------------------------------
// Rvector GetRvectorParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
Rvector Array::GetRvectorParameter(const Integer id, const Integer index) const
{
   Integer row, col;
   mRmatValue.GetSize(row, col);
   
   switch (id)
   {
   case ROW_VALUE:
      {
         Rvector rvec(col);
      
         for (int i=0; i<col; i++)
            rvec.SetElement(i, mRmatValue.GetElement(index, i));

         return rvec;
      }
   case COL_VALUE:
      {
         Rvector rvec(row);
      
         for (int i=0; i<row; i++)
            rvec.SetElement(i, mRmatValue.GetElement(i, index));

         return rvec;
      }
   default:
      throw ParameterException
         ("Array::GetRvectorParameter() Unknown Parameter Name" + PARAMETER_TEXT[id]);
      //return Parameter::GetRvectorParameter(id, index);
   }
}

//------------------------------------------------------------------------------
// Rvector GetRvectorParameter(const std::string &label,
//                                    const Integer index) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Rvector Array::GetRvectorParameter(const std::string &label,
                                   const Integer index) const
{
   return GetRvectorParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// const Rvector& SetRvectorParameter(const Integer id, const Rvector &value,
//                                    const Integer index)
//------------------------------------------------------------------------------
const Rvector& Array::SetRvectorParameter(const Integer id, const Rvector &value,
                                          const Integer index)
{
   Integer row, col;
   mRmatValue.GetSize(row, col);
   
#if DEBUG_ARRAY
   MessageInterface::ShowMessage
      ("Array::SetRvectorParameter() index=%d, row=%d, col=%d\n", index, row, col);
#endif
   
   switch (id)
   {
   case ROW_VALUE:
      for (int i=0; i<col; i++)
         mRmatValue.SetElement(index, i, value(i));
      
      return value;
   case COL_VALUE:
      for (int i=0; i<row; i++)
         mRmatValue.SetElement(i, index, value(i));
      
      return value;
   default:
      throw ParameterException
         ("Array::GetRvectorParameter() Unknown Parameter Name" + PARAMETER_TEXT[id]);
      //return Parameter::SetRvectorParameter(id, value, index);
   }
}

//------------------------------------------------------------------------------
// const Rvector& SetRvectorParameter(const std:string &label,
//                                    const Rvector &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& Array::SetRvectorParameter(const std::string &label,
                                          const Rvector &value,
                                          const Integer index)
{
   return SetRvectorParameter(GetParameterID(label), value, index);
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
// const Rmatrix& GetMatrix() const
//------------------------------------------------------------------------------
/**
 * Retrieves Rmatrix value of parameter without evaluating.
 */
//------------------------------------------------------------------------------
const Rmatrix& Array::GetMatrix() const
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
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer Array::GetIntegerParameter(const Integer id) const
{
   Integer row, col;
   mRmatValue.GetSize(row, col);

   switch (id)
   {
   case NUM_ROWS:
      return row;
   case NUM_COLS:
      return col;
   default:
      return Parameter::GetIntegerParameter(id);
   }
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Array::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

//----- Rmatrix parameter

//------------------------------------------------------------------------------
// const Rmatrix& GetRmatrixParameter(const Integer id) const
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
      return mRmatValue;
   default:
      return Parameter::GetRmatrixParameter(id);
   }
}


//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rmatrix& Array::GetRmatrixParameter(const std::string &label) const
{
   return GetRmatrixParameter(GetParameterID(label));
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
      mRmatValue = value;
      return value;
   default:
      return Parameter::SetRmatrixParameter(id, value);
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
   return SetRmatrixParameter(GetParameterID(label), value);
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

