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
//#define DEBUG_ARRAY_SET

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
   "InitialValue",
}; 

const Gmat::ParameterType
Array::PARAMETER_TYPE[ArrayParamCount - ParameterParamCount] =
{
   Gmat::INTEGER_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::RMATRIX_TYPE,
   Gmat::REAL_ELEMENT_TYPE,  // was Gmat::REAL_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::STRING_TYPE,
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
               GmatParam::NO_DEP, Gmat::UNKNOWN_OBJECT, false, false, false, true)
{
   mNumRows = 0;
   mNumCols = 0;
   mSizeSet = false;
   
   // GmatBase data
   objectTypeNames.push_back("Array");
   mReturnType = Gmat::RMATRIX_TYPE;
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
   // We don't want to change the name when copy
   std::string thisName = GetName();
   
   mNumRows = copy.mNumRows;
   mNumCols = copy.mNumCols;
   mSizeSet = copy.mSizeSet;
   mRmatValue = copy.mRmatValue;
   
   SetName(thisName);
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
      // We don't want to change the name when copy
      std::string thisName = GetName();
      
      Parameter::operator=(right);
      mNumRows = right.mNumRows;
      mNumCols = right.mNumCols;
      mSizeSet = right.mSizeSet;
      mRmatValue = right.mRmatValue;
      
      SetName(thisName);
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


//------------------------------------------------------------------------------
// bool SetSize(const Integer row, const Integer col)
//------------------------------------------------------------------------------
bool Array::SetSize(const Integer row, const Integer col)
{
   #if DEBUG_ARRAY
   MessageInterface::ShowMessage
      ("Array::SetSize() row=%d, col=%d\n", row, col);
   #endif
   
   mNumRows = row;
   mNumCols = col;
   mRmatValue.SetSize(mNumRows, mNumCols);
   mSizeSet = true;
   return true;
}


//------------------------------------
// methods inherited from Parameter
//------------------------------------

//------------------------------------------------------------------------------
// void SetRmatrix(const Rmatrix &mat)
//------------------------------------------------------------------------------
/**
 * Sets Rmatrix value of parameter.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
void Array::SetRmatrix(const Rmatrix &mat)
{
   if (mSizeSet)
   {
      mRmatValue = mat;
   }
   else
   {
      throw ParameterException("The size has not been set for " + GetName());
   }
}


//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return parameter value converted to std::string.
 */
//------------------------------------------------------------------------------
std::string Array::ToString()
{
   // use default global precision to convert to string (loj: 2008.03.05)
   return mRmatValue.ToString(false, false, false, GmatGlobal::DATA_PRECISION, 1,
                              true, 1, "", false);
   
   //std::stringstream ss("");
   //ss.precision(10);
   //ss << mRmatValue;
   //return std::string(ss.str());
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
// void Copy(const GmatBase* orig)
//------------------------------------------------------------------------------
void Array::Copy(const GmatBase* orig)
{
   operator=(*((Array *)(orig)));
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
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Array::GetParameterID(const std::string &str) const
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

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool Array::IsParameterReadOnly(const Integer id) const
{
   //loj: 11/22/06 read only parameters were wrong
   //if ((id == NUM_ROWS) || (id == NUM_COLS) || (id == SINGLE_VALUE))
   //   return true;
   
   if ((id == NUM_ROWS) || (id == NUM_COLS) || (id == RMAT_VALUE) ||
       (id == ROW_VALUE) || (id == COL_VALUE) || id == INITIAL_VALUE)
      return true;
   
   return Parameter::IsParameterReadOnly(id);
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
   switch (id)
   {
   case RMAT_VALUE:
      SetRmatrix(value);
      return value;
   default:
      return Parameter::SetRmatrixParameter(id, value);
   }
   
//    if (mSizeSet)
//    {
//       switch (id)
//       {
//       case RMAT_VALUE:
//          mRmatValue = value;
//          return value;
//       default:
//          return Parameter::SetRmatrixParameter(id, value);
//       }
//    }
//    else
//    {
//       throw ParameterException("The size has not been set for " + GetName());
//    }
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


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index)
//------------------------------------------------------------------------------
Real Array::GetRealParameter(const Integer id, const Integer index) const
{
   switch (id)
   {
   case SINGLE_VALUE:
      return mRmatValue.GetElement(0, index);
   default:
      throw ParameterException
         ("Array::GetRealParameter() Unknown Parameter Name" + PARAMETER_TEXT[id]);
   }
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Array::GetRealParameter(const std::string &label, const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
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
   case SINGLE_VALUE: //loj: 12/30/04 Changed from ROW_VALUE
      #ifdef DEBUG_ARRAY_SET
         MessageInterface::ShowMessage(
         "In Array::SetRealParameter, row = %d, col = %d, value = %.12f\n",
         row, col, value);
      #endif
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


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/*
 * Override this method to return array declaration string.
 * Such as Arr1[2]
 */
//------------------------------------------------------------------------------
std::string Array::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case DESCRIPTION:
      return GetArrayDefString();
   case INITIAL_VALUE:
      return GetInitialValueString();
   default:
      return Parameter::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string Array::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// StringArray GetGeneratingString(Gmat::WriteMode mode,
//                const std::string &prefix, const std::string &useName)
//------------------------------------------------------------------------------
/**
 * Produces a string, possibly multi-line, containing the text that produces an
 * object.
 * 
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 * 
 * @return A string containing the text.
 */
//------------------------------------------------------------------------------
const std::string& Array::GetGeneratingString(Gmat::WriteMode mode,
                                              const std::string &prefix,
                                              const std::string &useName)
{
   std::stringstream data;
   //std::stringstream value;
   
   // Crank up data precision so we don't lose anything
   //data.precision(18);
   data.precision(GetDataPrecision()); 
   std::string preface = "", nomme;
   
   //value.str("");
   //value.precision(18);
   
   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::OWNED_OBJECT) ||
       (mode == Gmat::SHOW_SCRIPT))
      inMatlabMode = false;
   if (mode == Gmat::MATLAB_STRUCT)
      inMatlabMode = true;
   
   if (useName != "")
      nomme = useName;
   else
      nomme = instanceName;
   
   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::SHOW_SCRIPT))
   {
      std::string tname = typeName;

      // Add comment line (loj: 03/27/07)
      if (GetCommentLine() != "")
         data << GetCommentLine();
      
      data << "Create " << tname << " " << nomme 
           << "[" << mNumRows << "," << mNumCols << "];\n";
      
      preface = "GMAT ";
   }
   
   nomme += ".";
   
   if (mode == Gmat::OWNED_OBJECT) {
      preface = prefix;
      nomme = "";
   }
   
   preface += nomme;
   
   WriteParameters(mode, preface, data);
   generatingString = data.str();   
   
   //MessageInterface::ShowMessage
   //   ("===> Array::GetGeneratingString() return\n%s\n", generatingString.c_str());
   
   return generatingString;
}


//------------------------------------------------------------------------------
// std::string GetArrayDefString() const
//------------------------------------------------------------------------------
/*
 * Returns array declaration string.
 * Such as Arr1[2]
 */
//------------------------------------------------------------------------------
std::string Array::GetArrayDefString() const
{
   std::stringstream data;
   data << instanceName << "[" << mNumRows << "," << mNumCols << "]";
   return data.str();
}


//------------------------------------------------------------------------------
// std::string GetInitialValueString()
//------------------------------------------------------------------------------
/*
 * Returns array initial value string including inline comments.
 *
 * Such as GMAT Arr1[1,1] = 13.34; %% initialize
 */
//------------------------------------------------------------------------------
std::string Array::GetInitialValueString() const
{
   std::stringstream data;
   
   Integer row = GetIntegerParameter("NumRows");
   Integer col = GetIntegerParameter("NumCols");

   for (Integer i = 0; i < row; ++i)
   {
      //loj: Do not write if value is zero since default is zero(03/27/07)
      for (Integer j = 0; j < col; ++j)
      {
         if (GetRealParameter(SINGLE_VALUE, i, j) != 0.0)
         {
            data << "GMAT " << instanceName << "(" << i+1 << ", " << j+1 <<
               ") = " << GetRealParameter(SINGLE_VALUE, i, j) << ";";
            data << GetInlineComment() + "\n";
         }
      }
   }
   
   return data.str();
}

