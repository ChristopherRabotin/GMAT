//$Header$
//------------------------------------------------------------------------------
//                                  ArrayElementWrapper
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan
// Created: 2007.01.18
//
/**
 * Implementation of the ArrayElementWrapper class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ArrayElementWrapper.hpp"
#include "ParameterException.hpp"
#include "RealUtilities.hpp"

//#include "MessageInterface.hpp"

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ArrayElementWrapper(const std::string &desc);
//---------------------------------------------------------------------------
/**
 * Constructs ArrayElementWrapper structures
 * (default constructor).
 *
 * @param <desc> Optional description for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
ArrayElementWrapper::ArrayElementWrapper(const std::string &desc) :
   ElementWrapper(desc),
   array         (NULL),
   row           (NULL),
   column        (NULL)
{
}

//---------------------------------------------------------------------------
//  ArrayElementWrapper(const ArrayElementWrapper &aew);
//---------------------------------------------------------------------------
/**
 * Constructs base ArrayElementWrapper structures used in derived classes, 
 * by copying the input instance (copy constructor).
 *
 * @param <aew>  ArrayElementWrapper instance to copy to create "this" 
 * instance.
 */
//---------------------------------------------------------------------------
ArrayElementWrapper::ArrayElementWrapper(const ArrayElementWrapper &aew) :
   ElementWrapper(aew),
   row           (aew.row),
   column        (aew.column)
{
}

//---------------------------------------------------------------------------
//  ArrayElementWrapper& operator=(const ArrayElementWrapper &aew)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ArrayElementWrapper structures.
 *
 * @param <aew> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ArrayElementWrapper& ArrayElementWrapper::operator=(
                             const ArrayElementWrapper &aew)
{
   if (&aew == this)
      return *this;

   ElementWrapper::operator=(aew);
   row      = aew.row;
   column   = aew.column;

   return *this;
}
//---------------------------------------------------------------------------
//  ~ArrayElementWrapper()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ArrayElementWrapper::~ArrayElementWrapper()
{
}

//---------------------------------------------------------------------------
//  bool SetArray(Array *toArray)
//---------------------------------------------------------------------------
/**
 * Method to set the ArrayElement pointer to the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ArrayElementWrapper::SetArray(Array *toArray)
{
   array      = toArray;
   return true;
}

//---------------------------------------------------------------------------
//  bool SetRowWrapper(ElementWrapper* toRow)
//---------------------------------------------------------------------------
/**
 * Method to set the ElementWrapper for the row.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ArrayElementWrapper::SetRowWrapper(ElementWrapper* toRow)
{
   row      = toRow;
   return true;
}

//---------------------------------------------------------------------------
//  bool SetColumnWrapper(ElementWrapper* toColumn)
//---------------------------------------------------------------------------
/**
 * Method to set the ElementWrapper for the column.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ArrayElementWrapper::SetColumnWrapper(ElementWrapper* toColumn)
{
   column   = toColumn;
   return true;
}

//---------------------------------------------------------------------------
//  Real EvaluateReal() const
//---------------------------------------------------------------------------
/**
 * Method to return the Real value of the wrapped object.
 *
 * @return Real value of the wrapped number object.
 * 
 */
//---------------------------------------------------------------------------
Real ArrayElementWrapper::EvaluateReal() const
{
   if (array == NULL)
      throw ParameterException(
      "Cannot return value of ArrayElement - object pointer is NULL\n");
   if ((row == NULL) || (column == NULL))
      throw ParameterException(
      "Cannot return value of ArrayElement - row or column element is NULL\n");
   Real itsValue;
   Integer rowInt    = -99;
   Integer columnInt = -99;
   try
   {
      // get the row value
      Real rowVal        = row->EvaluateReal();
      Real rowNearestInt = GmatMathUtil::NearestInt(rowVal);
      if ((GmatMathUtil::Mod(rowVal, rowNearestInt) != 0.0))
      {
         std::string errmsg =  "Cannot evaluate ArrayElement - ";
         errmsg += "row Element evaluates to a non-Integer value\n";
         throw ParameterException(errmsg);
      }
      rowInt = (Integer) rowNearestInt;
      // get the column value
      Real colVal        = column->EvaluateReal();
      Real colNearestInt = GmatMathUtil::NearestInt(colVal);
      if ((GmatMathUtil::Mod(colVal, colNearestInt) != 0.0))
      {
         std::string errmsg = "Cannot evaluate ArrayElement - ";
         errmsg += "column Element evaluates to a non-Integer value\n";
         throw ParameterException(errmsg);
      }
      columnInt = (Integer) colNearestInt;
      
      itsValue = array->GetRealParameter("SingleValue", rowInt, columnInt);
   }
   catch (BaseException &be)
   {
      std::string errmsg = "Cannot return Real value for array " + 
                            array->GetName(); 
      errmsg += " with row " + rowInt;
      errmsg += " and column " + columnInt;
      errmsg += " - exception thrown: " + be.GetMessage();
      throw ParameterException(errmsg);
   }
         
   return itsValue;
}
   
//---------------------------------------------------------------------------
//  bool SetReal(const Real toValue)
//---------------------------------------------------------------------------
/**
 * Method to set the Real value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ArrayElementWrapper::SetReal(const Real toValue)
{
   if (array == NULL)
      throw ParameterException(
      "Cannot set value of ArrayElement - object pointer is NULL\n");
   if ((row == NULL) || (column == NULL))
      throw ParameterException(
      "Cannot return value of ArrayElement - row or column element is NULL\n");
   Integer rowInt    = -99;
   Integer columnInt = -99;

   try
   {
      // get the row value
      Real rowVal        = row->EvaluateReal();
      Real rowNearestInt = GmatMathUtil::NearestInt(rowVal);
      if ((GmatMathUtil::Mod(rowVal, rowNearestInt) != 0.0))
      {
         std::string errmsg = "Cannot evaluate ArrayElement - ";
         errmsg += "row Element evaluates to a non-Integer value\n";
         throw ParameterException(errmsg);
      }
      rowInt = (Integer) rowNearestInt;
      // get the column value
      Real colVal        = column->EvaluateReal();
      Real colNearestInt = GmatMathUtil::NearestInt(colVal);
      if ((GmatMathUtil::Mod(colVal, colNearestInt) != 0.0))
      {
         std::string errmsg = "Cannot evaluate ArrayElement - ";
         errmsg += "column Element evaluates to a non-Integer value\n";
         throw ParameterException(errmsg);
      }
      columnInt = (Integer) colNearestInt;

      array->SetRealParameter("SingleValue", toValue, rowInt, columnInt);
   }
   catch (BaseException &be)
   {
      std::string errmsg = "Cannot set Real value for array " + 
                            array->GetName(); 
      errmsg += " with row " + rowInt;
      errmsg += " and column " + columnInt;
      errmsg += " - exception thrown: " + be.GetMessage();
      throw ParameterException(errmsg);
   }
         
   return true;
}
