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

//#define DEBUG_ARRAY 1 //loj: 11/23/04

//---------------------------------
// static data
//---------------------------------
const std::string
Array::PARAMETER_TEXT[ArrayParamCount - ParameterParamCount] =
{
   "NumRows",
   "NumCols",
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
 */
//------------------------------------------------------------------------------
Array::Array(const std::string &name, const std::string &desc,
             const std::string &unit)
   : Parameter(name, "Array", GmatParam::USER_PARAM, NULL, desc, unit,
               GmatParam::NO_DEP, Gmat::UNKNOWN_OBJECT, false)
{
   mNumRows = 0;
   mNumCols = 0;
   mSizeSet = false;
   
   // Parameter data
   mIsPlottable = false;
   
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
   mNumRows = copy.mNumRows;
   mNumCols = copy.mNumCols;
   mSizeSet = copy.mSizeSet;
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
      mNumRows = right.mNumRows;
      mNumCols = right.mNumCols;
      mSizeSet = right.mSizeSet;
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
   switch (id)
   {
   case ROW_VALUE:
      {
         Rvector rvec(mNumCols);
      
         for (int i=0; i<mNumCols; i++)
            rvec.SetElement(i, mRmatValue.GetElement(index, i));

         return rvec;
      }
   case COL_VALUE:
      {
         Rvector rvec(mNumRows);
      
         for (int i=0; i<mNumRows; i++)
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
   
#if DEBUG_ARRAY
   MessageInterface::ShowMessage
      ("Array::SetRvectorParameter() index=%d, mNumRows=%d, mNumCols=%d\n",
       index, mNumRows, mNumCols);
#endif
   
   switch (id)
   {
   case ROW_VALUE:
      for (int i=0; i<mNumCols; i++)
         mRmatValue.SetElement(index, i, value(i));
      
      return value;
   case COL_VALUE:
      for (int i=0; i<mNumRows; i++)
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
 *
 * @exception <ParameterException> thrown if matrix size has not been set.
 */
//------------------------------------------------------------------------------
const Rmatrix& Array::GetMatrix() const
{
   if (mSizeSet)
      return mRmatValue;
   else
      throw ParameterException("The size has not been set for " + GetName());      
}

//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* Array::GetParameterList() const
{
   return PARAMETER_TEXT;
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
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer Array::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case NUM_ROWS:
      return mNumRows;
   case NUM_COLS:
      return mNumCols;
   default:
      return Parameter::GetIntegerParameter(id);
   }
}

//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Array::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 *
 * @exception <ParameterException> thrown if Row or Column already has been set.
 */
//------------------------------------------------------------------------------
Integer Array::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
   case NUM_ROWS:
      if (mNumRows == 0)
         mNumRows = value;
      else
         throw ParameterException("Row alrealy has been set for " + GetName());

      if (mNumCols > 0 && !mSizeSet)
      {
         mRmatValue.SetSize(mNumRows, mNumCols);
         mSizeSet = true;
      }
      return value;
   case NUM_COLS:
      if (mNumCols == 0)
         mNumCols = value;
      else
         throw ParameterException("Column alrealy has been set for " + GetName());

      if (mNumRows > 0 && !mSizeSet)
      {
         mRmatValue.SetSize(mNumRows, mNumCols);
         mSizeSet = true;
      }
      return value;
   default:
      return Parameter::SetIntegerParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Array::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}

//----- Rmatrix parameter

//------------------------------------------------------------------------------
// const Rmatrix& GetRmatrixParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 *
 * @exception <ParameterException> thrown if matrix size has not been set.
 */
//------------------------------------------------------------------------------
const Rmatrix& Array::GetRmatrixParameter(const Integer id) const
{
   if (mSizeSet)
   {
      switch (id)
      {
      case RMAT_VALUE:
         return mRmatValue;
      default:
         return Parameter::GetRmatrixParameter(id);
      }
   }
   else
   {
      throw ParameterException("The size has not been set for " + GetName());
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
 *
 * @exception <ParameterException> thrown if matrix size has not been set.
 */
//------------------------------------------------------------------------------
const Rmatrix& Array::SetRmatrixParameter(const Integer id,
                                          const Rmatrix &value)
{
   if (mSizeSet)
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
   else
   {
      throw ParameterException("The size has not been set for " + GetName());
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

