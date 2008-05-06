//$Id$
//------------------------------------------------------------------------------
//                                  FunctionRunner
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
// Author: Wendy Shoan
// Created: 2008.04.21
//
/**
 * Implements FunctionRunner class.
 */
//------------------------------------------------------------------------------

#include "FunctionRunner.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// FunctionRunner()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
FunctionRunner::FunctionRunner(const std::string &nomme)
   : MathFunction("FunctionRunner", nomme)
{
   objectTypeNames.push_back("FunctionRunner");   
}


//------------------------------------------------------------------------------
// ~FunctionRunner()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
FunctionRunner::~FunctionRunner()
{
}


//------------------------------------------------------------------------------
//  FunctionRunner(const FunctionRunner &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the FunctionRunner object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
FunctionRunner::FunctionRunner(const FunctionRunner &copy) :
   MathFunction(copy)
{
}


//------------------------------------------------------------------------------
// void SetFunctionName(const std::string &fname)
//------------------------------------------------------------------------------
/*
 * Sets function name to the FunctionManager.
 *
 * @param  fname  The function name to set
 */
//------------------------------------------------------------------------------
void FunctionRunner::SetFunctionName(const std::string &fname)
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("FunctionRunner::SetFunctionName() passingArgs='%s', fname='%s'\n",
       GetName().c_str(), fname.c_str());
   #endif
   
   theFunctionManager.SetFunctionName(fname);
}


//------------------------------------------------------------------------------
// void SetFunction(Function *function)
//------------------------------------------------------------------------------
/*
 * Sets function pointer to the FunctionManager.
 *
 * @param  function  The function pointer to set
 */
//------------------------------------------------------------------------------
void FunctionRunner::SetFunction(Function *function)
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("FunctionRunner::SetFunction() passingArgs='%s', function=%p, name='%s'\n",
       GetName().c_str(), function, function->GetName().c_str());
   #endif
   
   if (theFunctionManager.GetFunctionName() == function->GetName())
      theFunctionManager.SetFunction(function);
}


//------------------------------------------------------------------------------
// void AddFunctionInput(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Adds function input argument name to the input names.
 *
 * @param  name  The name to add the list
 */
//------------------------------------------------------------------------------
void FunctionRunner::AddFunctionInput(const std::string &name)
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("FunctionRunner::AddFunctionInput() passingArgs='%s', adding name='%s'\n",
       GetName().c_str(), name.c_str());
   #endif
   
   theInputNames.push_back(name);
}


//------------------------------------------------------------------------------
// void AddFunctionOutput(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Adds function output argument name to the output names.
 *
 * @param  name  The name to add the list
 */
//------------------------------------------------------------------------------
void FunctionRunner::AddFunctionOutput(const std::string &name)
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("FunctionRunner::AddFunctionOutput() passingArgs='%s', adding name='%s'\n",
       GetName().c_str(), name.c_str());
   #endif
   
   theOutputNames.push_back(name);
}


//------------------------------------------------------------------------------
// void SetFunctionInputs()
//------------------------------------------------------------------------------
/*
 * Sets function input list to the FunctionManager.
 */
//------------------------------------------------------------------------------
void FunctionRunner::SetFunctionInputs()
{
   theFunctionManager.SetInputs(theInputNames);
}


//------------------------------------------------------------------------------
// void SetFunctionOutputs()
//------------------------------------------------------------------------------
/*
 * Sets function output list to the FunctionManager.
 */
//------------------------------------------------------------------------------
void FunctionRunner::SetFunctionOutputs()
{
   theFunctionManager.SetOutputs(theOutputNames);
}


//------------------------------------------------------------------------------
// const StringArray& GetInputs()
//------------------------------------------------------------------------------
/*
 * @return the input list
 */
//------------------------------------------------------------------------------
const StringArray& FunctionRunner::GetInputs()
{
   return theInputNames;
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void FunctionRunner::GetOutputInfo(Integer &type,
                                   Integer &rowCount, Integer &colCount)
{
   Function *function = theFunctionManager.GetFunction();
   if (function == NULL)
      throw MathException("FunctionRunner::GetOutputInfo() function is NULL\n");

   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("FunctionRunner::GetOutputInfo() entered, this=<%p><%s>, function=<%s><%p>\n",
       this, GetName().c_str(), function->GetName().c_str(), function);
   #endif
   
   // check for function output type
   IntegerArray rowCounts, colCounts;
   WrapperTypeArray outputTypes = function->GetOutputTypes(rowCounts, colCounts);
   std::string errMsg;
   
   if (outputTypes.size() == 0)
   {
      errMsg = "The function \"" + function->GetName() + "\" does not return"
         " any value";
   }
   else if (outputTypes.size() > 1)
   {
      errMsg = "The function \"" + function->GetName() + "\" returns more than "
         " one value";
   }
   else
   {
      if (outputTypes[0] == Gmat::VARIABLE)
      {
         type = Gmat::REAL_TYPE;
         rowCount = 1;
         colCount = 1;
      }
      else if (outputTypes[0] == Gmat::ARRAY)
      {
         type = Gmat::RMATRIX_TYPE;
         rowCount = rowCounts[0];
         colCount = colCounts[0];
         matrix.SetSize(rowCount, colCount);
      }
   }
   
   elementType = type;
   
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("FunctionRunner::GetOutputInfo() type=%d, rowCount=%d, colCount=%d\n",
       type, rowCount, colCount);
   #endif
   
   if (errMsg != "")
   {
      throw MathException("FunctionRunner::GetOutputInfo() " + errMsg);
   }
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool FunctionRunner::ValidateInputs()
{
   Function *function = theFunctionManager.GetFunction();
   if (function == NULL)
      throw MathException("FunctionRunner::ValidateInputs() function is NULL\n");
   
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("FunctionRunner::ValidateInputs() entered, this=<%p><%s>, function=<%s><%p>\n",
       this, GetName().c_str(), function->GetName().c_str(), function);
   #endif
   
   // How can we validate input here? Just return true for now.
   return true;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the FunctionRunner of left node
 *
 */
//------------------------------------------------------------------------------
Real FunctionRunner::Evaluate()
{
   Function *function = theFunctionManager.GetFunction();
   if (function == NULL)
      throw MathException("FunctionRunner::Evaluate() function is NULL\n");
   
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("FunctionRunner::Evaluate() entered, this=<%p><%s>, function=<%s><%p>\n",
       this, GetName().c_str(), function->GetName().c_str(), function);
   #endif
   
   if (elementType == Gmat::RMATRIX_TYPE)
      throw MathException
         ("The function \"" + function->GetName() + "\" returns matrix value");
   
   // Tell the FunctionManager to build the Function Object Store
   //theFunctionManager.BuildFunctionObjectStore();
   
   return theFunctionManager.Evaluate();   
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the FunctionRunner of left node
 *
 */
//------------------------------------------------------------------------------
Rmatrix FunctionRunner::MatrixEvaluate()
{
   Function *function = theFunctionManager.GetFunction();
   if (function == NULL)
      throw MathException("FunctionRunner::Evaluate() function is NULL\n");
   
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("FunctionRunner::MatrixEvaluate() entered, this=<%p><%s>, function=<%s><%p>\n",
       this, GetName().c_str(), function->GetName().c_str(), function);
   #endif
   
   if (elementType == Gmat::REAL_TYPE)
      throw MathException
         ("The function \"" + function->GetName() + "\" returns Real value");
   
   // Tell the FunctionManager to build the Function Object Store
   //theFunctionManager.BuildFunctionObjectStore();
   
   return theFunctionManager.MatrixEvaluate();
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the FunctionRunner operation.
 *
 * @return clone of the FunctionRunner operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* FunctionRunner::Clone() const
{
   return (new FunctionRunner(*this));
}


