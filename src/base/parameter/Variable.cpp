//$Header$
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
 */
//------------------------------------------------------------------------------

#include "Variable.hpp"
#include "ParameterException.hpp"
#include "ExpressionParser.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_VARIABLE 1

//---------------------------------
// static data
//---------------------------------
const std::string
Variable::PARAMETER_TEXT[VariableParamCount - RealVarParamCount] =
{
   "RefParams"
}; 

const Gmat::ParameterType
Variable::PARAMETER_TYPE[VariableParamCount - RealVarParamCount] =
{
   Gmat::STRINGARRAY_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Variable(const std::string &name, const std::string &desc = "",
//          const std::string &unit = "")
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
Variable::Variable(const std::string &name, const std::string &desc,
                   const std::string &unit)
   : RealVar(name, "Variable", GmatParam::USER_PARAM, NULL, desc, unit,
             GmatParam::NO_DEP, Gmat::UNKNOWN_OBJECT, false)
{
   mParamDb = new ParameterDatabase();
   mExpParser = new ExpressionParser();
   // Set parameter database to be used
   mExpParser->SetParameterDatabase(mParamDb);
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
   mParamDb = new ParameterDatabase();
   *mParamDb = *copy.mParamDb;
   mExpParser = new ExpressionParser();
   mExpParser->SetParameterDatabase(mParamDb);   
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
      RealVar::operator=(right);
      mParamDb = new ParameterDatabase();
      *mParamDb = *right.mParamDb;
      mExpParser = new ExpressionParser();
      mExpParser->SetParameterDatabase(mParamDb);   
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
   // if expression is variable itself or empty or first char is digit
   //loj: 11/4/04 added isdigit
   if (this->GetName() == mExpr || mExpr == "" || isdigit(mExpr[0]))
      return mRealValue;
      
#if DEBUG_VARIABLE
   StringArray paramNames = mParamDb->GetNamesOfParameters();
   for (int i = 0; i<mParamDb->GetNumParameters(); i++)
      MessageInterface::ShowMessage
         ("Variable::EvaluateReal() In mParamDb :%s\n", paramNames[i].c_str());
#endif

   // Evaluate the expression
   mRealValue = mExpParser->EvalExp(mExpr.c_str());
   return mRealValue;
}

//-------------------------------------
// Methods inherited from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* Variable::GetParameterList() const
{
   return PARAMETER_TEXT;
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

//loj: 11/16/04 added
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

   if (type == Gmat::PARAMETER)
      return mParamDb->RenameParameter(oldName, newName);
   
   return false;
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string Variable::GetParameterText(const Integer id) const
{
   if (id >= RealVarParamCount && id < VariableParamCount)
      return PARAMETER_TEXT[id - RealVarParamCount];
   else
      return RealVar::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str) const
//------------------------------------------------------------------------------
Integer Variable::GetParameterID(const std::string str) const
{
   for (int i=RealVarParamCount; i<VariableParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - RealVarParamCount])
         return i;
   }
   
   return RealVar::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType Variable::GetParameterType(const Integer id) const
{
   if (id >= RealVarParamCount && id < VariableParamCount)
      return PARAMETER_TYPE[id - RealVarParamCount];
   else
      return RealVar::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string Variable::GetParameterTypeString(const Integer id) const
{
   if (id >= RealVarParamCount && id < VariableParamCount)
      return PARAM_TYPE_STRING[GetParameterType(id - RealVarParamCount)];
   else
      return RealVar::GetParameterTypeString(id);
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
#if DEBUG_VARIABLE
   MessageInterface::ShowMessage
      ("Variable::SetRefObjectName() type=%d, name=%s\n", type, name.c_str());
#endif
   if (type != Gmat::PARAMETER)
   {
      throw ParameterException
         ("Variable::SetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
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
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& Variable::GetStringArrayParameter(const Integer id) const
{
#if DEBUG_VARIABLE
   MessageInterface::ShowMessage
      ("Variable::GetStringArrayParameter() id=%d\n", id);
#endif
   
   switch (id)
   {
   case REF_PARAMS:
      {
#if DEBUG_VARIABLE
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
   default:
      return RealVar::GetStringArrayParameter(id);
   }
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& Variable::GetStringArrayParameter(const std::string &label) const
{
#if DEBUG_VARIABLE
   MessageInterface::ShowMessage
      ("Variable::GetStringArrayParameter() label=%s\n", label.c_str());
#endif
   return (GetStringArrayParameter(GetParameterID(label)));
}
