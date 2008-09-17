//$Id$
//------------------------------------------------------------------------------
//                                Variable
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/09/15
//
/**
 * Implements Variable class which provides methods for general variable evaluation.
 *    Such as: 100 * Sat1.X; where Sat1.X is already defined
 *             Sat1.X + Sat1.X;
 *             A * B; where A and B are already defined
 *    Only operator allowed are: +*-/^
 */
//------------------------------------------------------------------------------

#include "Variable.hpp"
#include "ParameterException.hpp"
#include "ExpressionParser.hpp"
#include "MessageInterface.hpp"
#include "Linear.hpp"             // for ToString()
#include "StringUtil.hpp"         // for Replace()
#include <sstream>

//#define DEBUG_VARIABLE_SET 1
//#define DEBUG_VARIABLE_EVAL 1
//#define DEBUG_RENAME 1
//#define DEBUG_GEN_STRING 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Variable(const std::string &name, const std::string &valStr = "",
//          const std::string &desc = "", const std::string &unit = "")
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
Variable::Variable(const std::string &name, const std::string &valStr,
                   const std::string &desc, const std::string &unit)
   : RealVar(name, valStr, "Variable", GmatParam::USER_PARAM, NULL, desc, unit,
             GmatParam::NO_DEP, Gmat::UNKNOWN_OBJECT, false)
{
   objectTypeNames.push_back("Variable");
   
   mParamDb = new ParameterDatabase();
   mExpParser = new ExpressionParser();
   // Set parameter database to be used
   mExpParser->SetParameterDatabase(mParamDb);
   
   // Initialize real value and expression
   mIsNumber = true;
   mRealValue = 0.0;
   mExpr = "0";
   
   #if DEBUG_VARIABLE
   MessageInterface::ShowMessage("Variable::Variable() constructor\n");
   MessageInterface::ShowMessage
      ("   numDBParams = %d\n", mParamDb->GetNumParameters());
   #endif
}


//------------------------------------------------------------------------------
// Variable(const Variable &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
Variable::Variable(const Variable &copy)
   : RealVar(copy)
{
   mParamDb = new ParameterDatabase(*copy.mParamDb);
   mExpParser = new ExpressionParser();
   mExpParser->SetParameterDatabase(mParamDb);
   
   #if DEBUG_VARIABLE
   MessageInterface::ShowMessage("Variable::Variable() copy constructor\n");
   MessageInterface::ShowMessage
      ("   copy.numDBParams = %d\n", copy.mParamDb->GetNumParameters());
   MessageInterface::ShowMessage
      ("   numDBParams = %d\n", mParamDb->GetNumParameters());
   #endif
}


//------------------------------------------------------------------------------
// Variable& operator=(const Variable &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
Variable& Variable::operator=(const Variable &right)
{
   if (this != &right)
   {
      // We don't want to change the name when copy
      std::string thisName = GetName();
      
      RealVar::operator=(right);
      mParamDb = new ParameterDatabase(*right.mParamDb);
      mExpParser = new ExpressionParser();
      mExpParser->SetParameterDatabase(mParamDb);
      // Set expression to name of right side since expression is used for
      // writnig in GetGeneratingString() (loj: 2008.08.13)
      // For example:
      // var1 = 123.45;
      // var2 = var1;
      // We want to write "var2 = var1" instead of "var2 = 123.45"
      mExpr = right.GetName();
      SetName(thisName);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~Variable()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Variable::~Variable()
{
   delete mParamDb;
   delete mExpParser;
}

//-------------------------------------
// Inherited methods from RealVar
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Real GetReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Real Variable::GetReal()
{
   return mRealValue;
}

//------------------------------------------------------------------------------
// virtual Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Real Variable::EvaluateReal()
{
   #if DEBUG_VARIABLE_EVAL
   MessageInterface::ShowMessage
      ("Variable::EvaluateReal() this=<%p>'%s', mExpr=%s, mIsNumber=%d\n",
       this, GetName().c_str(), mExpr.c_str(), mIsNumber);
   #endif
   
   if (mIsNumber)
   {      
      #if DEBUG_VARIABLE_EVAL
      MessageInterface::ShowMessage
         ("Variable::EvaluateReal() Returning just a number: mRealValue=%f\n",
          mRealValue);
      #endif
      
      return mRealValue;
   }
   else
   {
      try
      {
         // Evaluate the expression
         mRealValue = mExpParser->EvalExp(mExpr.c_str());
         
         #if DEBUG_VARIABLE_EVAL
         MessageInterface::ShowMessage
            ("Variable::EvaluateReal() Returning expression evaluation: "
             "mRealValue=%f\n", mRealValue);
         #endif
         
         return mRealValue;
      }
      catch (BaseException &e)
      {
         throw ParameterException
            (e.GetFullMessage() + " for the Variable \"" + GetName() + "\"");
      }
   }
}


//-------------------------------------
// Methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* Variable::Clone() const
{
   return new Variable(*this);
}


//------------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//------------------------------------------------------------------------------
void Variable::Copy(const GmatBase* orig)
{
   operator=(*((Variable *)(orig)));
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
bool Variable::SetStringParameter(const Integer id, const std::string &value)
{
   #if DEBUG_VARIABLE_SET
   MessageInterface::ShowMessage
      ("Variable::SetStringParameter() this=<%p>'%s', id=%d, value='%s'\n", this,
       GetName().c_str(), id, value.c_str());
   #endif
   
   switch (id)
   {
   case EXPRESSION:
      {         
         // if value is blank or number, call Parent class
         Real rval;
         if (value == "" || GmatStringUtil::ToReal(value, &rval))
         {
            return RealVar::SetStringParameter(id, value);
         }
         else
         {
            mValueSet = true;
            mIsNumber = false;
            mExpr = value;  
            
            #if DEBUG_VARIABLE_SET
            MessageInterface::ShowMessage
               ("   Variable expression set to '%s'\n", value.c_str());
            #endif
            return true;
         }
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
bool Variable::SetStringParameter(const std::string &label,
                                 const std::string &value)
{
   #if DEBUG_VARIABLE_SET
   MessageInterface::ShowMessage
      ("Variable::SetStringParameter() label=%s value=%s\n",
       label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Variable::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Variable::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::PARAMETER && type != Gmat::COORDINATE_SYSTEM &&
       type != Gmat::SPACECRAFT && type != Gmat::CALCULATED_POINT)
      return true;
   
   // change expression if it has object name and .
   std::string tmpOldName = oldName + ".";
   std::string::size_type pos = mExpr.find(tmpOldName);
   if (pos != mExpr.npos)
      mExpr = GmatStringUtil::Replace(mExpr, oldName, newName);
   
   mParamDb->RenameParameter(oldName, newName);
   RealVar::RenameRefObject(type, oldName, newName);
   return true;
}


//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string Variable::GetRefObjectName(const Gmat::ObjectType type) const
{
   if (type != Gmat::PARAMETER)
   {
      throw ParameterException
         ("Variable::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return mParamDb->GetFirstParameterName();
}


//------------------------------------------------------------------------------
// virtual bool SetRefObjectName(const Gmat::ObjectType type,
//                               const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets reference object name to given object type.
 *
 * @param <type> object type
 * @param <name> object name
 *
 */
//------------------------------------------------------------------------------
bool Variable::SetRefObjectName(const Gmat::ObjectType type,
                                const std::string &name)
{
   #if DEBUG_VARIABLE_SET
   MessageInterface::ShowMessage
      ("Variable::SetRefObjectName() this=<%p>'%s', type=%d, name=%s\n", this,
       GetName().c_str(), type, name.c_str());
   #endif
   
   if (type != Gmat::PARAMETER)
   {
      throw ParameterException
         ("Variable::SetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   #if DEBUG_VARIABLE_SET
   MessageInterface::ShowMessage
      ("   Adding '%s' to '%s' parameter database\n", name.c_str(), GetName().c_str());
   #endif
   
   mParamDb->Add(name);
   return true;
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* Variable::GetRefObject(const Gmat::ObjectType type,
                                 const std::string &name)
{
   if (type != Gmat::PARAMETER)
   {
      throw ParameterException
         ("Variable::GetRefObject() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return mParamDb->GetParameter(name);
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool Variable::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                            const std::string &name)
{
   if (type != Gmat::PARAMETER)
   {
      throw ParameterException
         ("Variable::SetRefObject() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return mParamDb->SetParameter(name, (Parameter*)obj);
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& Variable::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   if (mParamDb == NULL)
      throw ParameterException
         ("Variable::GetRefObjectNameArray() mParamDb is NULL\n");
   
   #if DEBUG_VARIABLE
   MessageInterface::ShowMessage
      ("Variable::GetRefObjectNameArray() type=%d\n", type);
   StringArray paramNames = mParamDb->GetNamesOfParameters();
   MessageInterface::ShowMessage
      ("Variable::GetStringArrayParameter() mParamDb->GetNamesOfParameters() "
       "size=%d\n", paramNames.size());
   for (unsigned int i=0; i<paramNames.size(); i++)
      MessageInterface::ShowMessage
         ("Variable::GetStringArrayParameter() "
          "paramNames[%d]=%s\n", i, paramNames[i].c_str());
   #endif
   
   return mParamDb->GetNamesOfParameters();
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName
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
const std::string& Variable::GetGeneratingString(Gmat::WriteMode mode,
                                                 const std::string &prefix,
                                                 const std::string &useName)
{
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("Variable::GetGeneratingString() this=<%s>'%s', mode=%d, prefix='%s', "
       "useName='%s'\n   mExpr='%s', mRealValue=%f, mIsNumber=%d, mValueSet=%d\n",
       this, GetName().c_str(), mode, prefix.c_str(), useName.c_str(),
       mExpr.c_str(), mRealValue, mIsNumber, mValueSet);
   #endif
   
   bool generateStr = false;
   
   if (mode == Gmat::SHOW_SCRIPT)
   {
      #ifdef DEBUG_GEN_STRING
      MessageInterface::ShowMessage("   mode is SHOW_SCRIPT, so generating string\n");
      #endif
      generateStr = true;
   }
   else
   {
      if (mIsNumber)
      {
         Real rval = GetReal();
         
         // Write value if it is set from user or non-zero
         if (mValueSet || rval != 0.0)
         {
            #ifdef DEBUG_GEN_STRING
            MessageInterface::ShowMessage
               ("   '%s' is a non-zero number or set by user, so generating string\n",
                mExpr.c_str());
            #endif
            generateStr = true;
         }
      }
      else
      {
         #ifdef DEBUG_GEN_STRING
         MessageInterface::ShowMessage
            ("   '%s' is not a number, so generating string\n", mExpr.c_str());
         #endif
         generateStr = true;
      }
   }
   
   if (generateStr)
   {
      generatingString = "GMAT " + GetName() + " = " + mExpr + ";";
      generatingString = generatingString + inlineComment + "\n";
   }
   
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("Variable::GetGeneratingString() returning\n<%s>\n", generatingString.c_str());
   #endif
   
   return generatingString;
}

