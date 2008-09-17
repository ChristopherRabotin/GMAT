//$Id$
//------------------------------------------------------------------------------
//                                  Add
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2006/03/31
//
/**
 * Implements Add class.
 */
//------------------------------------------------------------------------------

#include "Add.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ADD
//#define DEBUG_INPUT_OUTPUT
//#define DEBUG_EVALUATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Add()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Add::Add(const std::string &nomme)
   : MathFunction("Add", nomme)
{
}


//------------------------------------------------------------------------------
// ~Add()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Add::~Add()
{
}


//------------------------------------------------------------------------------
//  Add(const Add &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Add object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Add::Add(const Add &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Add operation.
 *
 * @return clone of the Add operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Add::Clone() const
{
   return (new Add(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Add::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Add::GetOutputInfo() exp=%s, leftNode=<%p><%s>, rightNode=<%p><%s>\n",
       GetName().c_str(), leftNode, leftNode ? leftNode->GetTypeName().c_str() : "NULL",
       rightNode, rightNode ? rightNode->GetTypeName().c_str() : "NULL");
   #endif
   
   if (!leftNode)
      throw MathException("Add::GetOutputInfo() The left node is NULL");

   if (!rightNode)
      throw MathException("Add::GetOutputInfo() The right node is NULL");
   
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   type = type1;
   rowCount = row1;
   colCount = col1;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Add::GetOutputInfo() type1=%d, row1=%d, col1=%d, type2=%d, "
       "row2=%d, col2=%d\n", type1, row1, col1, type2, row2, col2);
   #endif
   
   if ((type1 != type2) || (row1 != row2) || (col1 != col2))
      throw MathException("Dimentions are not the same, can not add.\n");    
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Add::GetOutputInfo() returning type=%d, rowCount=%d, colCount=%d\n",
       type, rowCount, colCount);
   #endif
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Add::ValidateInputs()
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage("Add::ValidateInputs()\n");
   #endif
   
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   if ((type1 == Gmat::REAL_TYPE) && (type2 == Gmat::REAL_TYPE))
      return true;
   else if ((type1 == Gmat::RMATRIX_TYPE) && (type2 == Gmat::RMATRIX_TYPE))
      if ((row1 == row2) && (col1 == col2))
         return true;
      else
         return false; 
   else
      return false;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the sum of left node(real) and right node(real)
 *
 */
//------------------------------------------------------------------------------
Real Add::Evaluate()
{
   return leftNode->Evaluate() + rightNode->Evaluate();
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the sum of left node(matrix) and right node(matrix)
 *
 */
//------------------------------------------------------------------------------
Rmatrix Add::MatrixEvaluate()
{
   #if DEBUG_EVLAUATE
   MessageInterface::ShowMessage
      ("Add::MatrixEvaluate() left=%s, right=%s\n",
       leftNode->MatrixEvaluate().ToString().c_str(),
       rightNode->MatrixEvaluate().ToString().c_str());
   #endif
   
   return leftNode->MatrixEvaluate() + rightNode->MatrixEvaluate();
}

