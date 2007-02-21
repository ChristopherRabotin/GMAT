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
#include "StringUtil.hpp"
#include "UtilityException.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_AE_WRAPPER

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ArrayElementWrapper();
//---------------------------------------------------------------------------
/**
 * Constructs ArrayElementWrapper structures
 * (default constructor).
 *
 */
//---------------------------------------------------------------------------
ArrayElementWrapper::ArrayElementWrapper() :
   ElementWrapper(),
   array         (NULL),
   row           (NULL),
   column        (NULL),
   arrayName     (""),
   rowName       (""),
   columnName    ("")
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
   array         (NULL),
   row           (NULL),
   column        (NULL),
   arrayName     (aew.arrayName),
   rowName       (aew.rowName),
   columnName    (aew.columnName)
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
   array        = NULL;  //
   row          = NULL;  // or do I want copies or clones here?
   column       = NULL;  //
   arrayName    = aew.arrayName;
   rowName      = aew.rowName;
   columnName   = aew.columnName;

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
   if (row)    delete row;
   if (column) delete column;
}

//------------------------------------------------------------------------------
//  const StringArray&  GetRefObjectNames() 
//------------------------------------------------------------------------------
/**
 * This method returns the list of reference object names for the ElementWrapper 
 * object.
 *
 * @return list of reference object names for the object.
 *
 */
//------------------------------------------------------------------------------
const StringArray& ArrayElementWrapper::GetRefObjectNames()
{
   refObjectNames.clear();
   // start with the array name ...
   refObjectNames.push_back(arrayName);
   
   // ... then add the reference object names from the row
   // (don't worry about duplicates for now)
   StringArray rowRefNames = row->GetRefObjectNames();
   for (StringArray::const_iterator i = rowRefNames.begin(); 
        i != rowRefNames.end(); ++i)
          refObjectNames.push_back(*i);
          
   // ... then add the reference object names from the column
   // (don't worry about duplicates for now)
   StringArray colRefNames = column->GetRefObjectNames();
   for (StringArray::const_iterator j = colRefNames.begin(); 
        j != colRefNames.end(); ++j)
          refObjectNames.push_back(*j);
          
   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage("AEWrapper:: Returning ref object names:\n");
      for (Integer ii = 0; ii < (Integer) refObjectNames.size(); ii++)
         MessageInterface::ShowMessage("      %s\n", refObjectNames[ii].c_str());
   #endif
   
   return refObjectNames;
}

//---------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj)
//---------------------------------------------------------------------------
/**
 * Method to set the reference object (ObjectProperty) pointer on the wrapped 
 * object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ArrayElementWrapper::SetRefObject(GmatBase *obj)
{
   bool isOk   = false;
   bool setRow = false;
   bool setCol = false;
   if ( (obj->IsOfType("Array")) && (obj->GetName() == arrayName) )
   {
      array = (Array*) obj;
      #ifdef DEBUG_AE_WRAPPER
         MessageInterface::ShowMessage("AEWrapper:: Setting array object %s\n",
            arrayName.c_str());
      #endif
      isOk = true;
   }
   if (row)    setRow = row->SetRefObject(obj);
   if (column) setCol = column->SetRefObject(obj);
   
   if (!isOk && !setRow && !setCol) return false;
   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage(
         "AEWrapper:: Returning true from SetRefObject\n");
   #endif
   
   return true;
}

//---------------------------------------------------------------------------
//  bool RenameObject(const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Method to rename a reference object for the wrapper.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ArrayElementWrapper::RenameObject(const std::string &oldName, 
                                       const std::string &newName)
{
   ElementWrapper::RenameObject(oldName, newName);
   // now rebuild the description string from the refObjectNames
   // first, rename things in the row and column wrappers
   row->RenameObject(oldName, newName);
   column->RenameObject(oldName, newName);
   rowName     = row->GetDescription();
   columnName  = column->GetDescription();
   // then put it all back together
   arrayName   = refObjectNames[0];
   description = arrayName + "(";
   description += rowName + ",";
   description += columnName + ");";
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
      #ifdef DEBUG_AE_WRAPPER
         MessageInterface::ShowMessage(
            "AEWrapper::EvalReal - row evaluates to %d\n", (Integer) rowNearestInt);
      #endif
      if ((GmatMathUtil::Mod(rowVal, rowNearestInt) != 0.0))
      {
         std::string errmsg =  "Cannot evaluate ArrayElement - ";
         errmsg += "row Element evaluates to a non-Integer value\n";
         throw ParameterException(errmsg);
      }
      rowInt = (Integer) rowNearestInt - 1;
      // get the column value
      Real colVal        = column->EvaluateReal();
      Real colNearestInt = GmatMathUtil::NearestInt(colVal);
      #ifdef DEBUG_AE_WRAPPER
         MessageInterface::ShowMessage(
            "AEWrapper::EvalReal - column evaluates to %d\n", (Integer) colNearestInt);
      #endif
      if ((GmatMathUtil::Mod(colVal, colNearestInt) != 0.0))
      {
         std::string errmsg = "Cannot evaluate ArrayElement - ";
         errmsg += "column Element evaluates to a non-Integer value\n";
         throw ParameterException(errmsg);
      }
      columnInt = (Integer) colNearestInt - 1;
      
      itsValue = array->GetRealParameter("SingleValue", rowInt, columnInt);
      #ifdef DEBUG_AE_WRAPPER
         MessageInterface::ShowMessage(
            "AEWrapper::EvalReal - itsValue evaluates to %.12f\n", 
            itsValue);
      #endif
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
   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage(
         "AEWrapper::SetReal called with input %.12f\n", 
         toValue);
   #endif
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

std::string ArrayElementWrapper::GetRowName()
{
   return rowName;
}

std::string ArrayElementWrapper::GetColumnName()
{
   return columnName;
}

   
bool ArrayElementWrapper::SetRow(ElementWrapper* toWrapper)
{
   row = toWrapper;
   return true;
}

bool ArrayElementWrapper::SetColumn(ElementWrapper* toWrapper)
{
   column = toWrapper;
   return true;
}

//---------------------------------------------------------------------------
//  void SetupWrapper()
//---------------------------------------------------------------------------
/**
 * Method to set up the Array Element Wrapper.
 *
 */
//---------------------------------------------------------------------------
void ArrayElementWrapper::SetupWrapper()
{
   try
   {
      GmatStringUtil::GetArrayIndexVar(description, rowName, columnName,
                                       arrayName);
   }
   catch (UtilityException &ue)
   {
      std::string errmsg = "Unable to set up ArrayElementWrapper \"" +
                           description;
      errmsg += "\" - does not parse correctly as an array.\n";
      throw ParameterException(errmsg);
   }
   #ifdef DEBUG_AE_WRAPPER
      MessageInterface::ShowMessage(
         "AEWrapper::SetupWrapper - \n");
      MessageInterface::ShowMessage("   description = %s\n", description.c_str());
      MessageInterface::ShowMessage("   rowName     = %s\n", rowName.c_str());
      MessageInterface::ShowMessage("   columnName  = %s\n", columnName.c_str());
      MessageInterface::ShowMessage("   arrayName   = %s\n", arrayName.c_str());
   #endif
   // for now, put the array name in the list of reference objects - add
   // all the other stuff when GetRefObjectNames is called
   refObjectNames.push_back(arrayName);
}
