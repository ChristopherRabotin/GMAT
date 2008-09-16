//$Id$
//------------------------------------------------------------------------------
//                                  Subtract
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
 * Implements Subtract class.
 */
//------------------------------------------------------------------------------

#include "Subtract.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SUBTRACT 1
//#define DEBUG_MATH_VALIDATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Subtract()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Subtract::Subtract(const std::string &nomme)
   : MathFunction("Subtract", nomme)
{
}


//------------------------------------------------------------------------------
// ~Subtract()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Subtract::~Subtract()
{
}


//------------------------------------------------------------------------------
//  Subtract(const Subtract &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Subtract object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Subtract::Subtract(const Subtract &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Subtract operation.
 *
 * @return clone of the Subtract operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Subtract::Clone() const
{
   return (new Subtract(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Subtract::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   #if DEBUG_SUBTRACT
   MessageInterface::ShowMessage
      ("Divide::GetOutputInfo() this=<%p><%s><%s>\n", this, GetTypeName().c_str(),
       GetName().c_str());
   #endif
   
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   #if DEBUG_SUBTRACT
   MessageInterface::ShowMessage
      ("Subtract::GetOutputInfo() type1=%d, row1=%d, col1=%d, type2=%d, "
       "row2=%d, col2=%d\n", type1, row1, col1, type2, row2, col2);
   #endif
   
   if ((type1 != type2) || (row1 != row2) || (col1 != col2))
      throw MathException("Matrixes are not the same can not add.\n");    
   else
   {
      type = type1;
      rowCount = row1;
      colCount = col1;
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
bool Subtract::ValidateInputs()
{
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   bool retval = false;
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   #ifdef DEBUG_MATH_VALIDATE
   MessageInterface::ShowMessage
      ("Subtract::ValidateInputs() type1=%d, row1=%d, col1=%d, "
       "type2=%d, row2=%d, col2=%d\n", type1, row1, col1, type2, row2, col2);
   #endif
   
   if (type1 == Gmat::REAL_TYPE && type2 == Gmat::REAL_TYPE)
      retval = true;
   else if (type1 == Gmat::RMATRIX_TYPE && type2 == Gmat::RMATRIX_TYPE)
      if (row1 == row2 && col1 == col2)
         retval = true;
   
   #ifdef DEBUG_MATH_VALIDATE
   MessageInterface::ShowMessage
      ("Subtract::ValidateInputs() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the difference of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Real Subtract::Evaluate()
{
   if (ValidateInputs())
      return leftNode->Evaluate() - rightNode->Evaluate();
   else
      throw MathException("Subtract::Both operands should be of type Real.\n");    
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the difference of left node(matrix) and right node(matrix)
 *
 */
//------------------------------------------------------------------------------
Rmatrix Subtract::MatrixEvaluate()
{
   if (ValidateInputs())
      return leftNode->MatrixEvaluate() - rightNode->MatrixEvaluate();
   else
      throw MathException("Subtract::Operands are not of the same type or same "
                          "dimension.\n");    
}
