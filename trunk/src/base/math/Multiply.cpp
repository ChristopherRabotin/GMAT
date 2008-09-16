//$Header$
//------------------------------------------------------------------------------
//                                  Multiply
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2006/04/04
//
/**
 * Implements Multiply class.
 */
//------------------------------------------------------------------------------

#include "Multiply.hpp"
#include "MessageInterface.hpp"

//#if DEBUG_MULTIPLY 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Multiply()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Multiply::Multiply(const std::string &nomme)
   : MathFunction("Multiply", nomme)
{
}


//------------------------------------------------------------------------------
// ~Multiply()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Multiply::~Multiply()
{
}


//------------------------------------------------------------------------------
//  Multiply(const Multiply &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Multiply object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Multiply::Multiply(const Multiply &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Multiply operation.
 *
 * @return clone of the Multiply operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Multiply::Clone() const
{
   return (new Multiply(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Multiply::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);

   #if DEBUG_MULTIPLY
   MessageInterface::ShowMessage
      ("Multiply::GetOutputInfo() type1=%d, row1=%d, col1=%d, type2=%d, "
       "row2=%d, col2=%d\n", type1, row1, col1, type2, row2, col2);
   #endif
   
   type = type1;
   rowCount = row1;
   colCount = col1;
   
   if ((type1 == Gmat::RMATRIX_TYPE) && (type2 == Gmat::RMATRIX_TYPE))
   {
      if (col1 == row2)
      {
         rowCount = row1;
         colCount = col2;
         if (rowCount == 1 && colCount == 1)
            type = Gmat::REAL_TYPE;
      }
      else
      {
         throw MathException
            (GetName() + ":Inner matrix dimensions must agree to multiply.\n");
      }
   }
   else if (type2 == Gmat::RMATRIX_TYPE)
   {
      type = type2;
      rowCount = row2;
      colCount = col2;
   }

   #if DEBUG_MULTIPLY
   MessageInterface::ShowMessage
      ("Multiply::GetOutputInfo() type=%d, rowCount=%d, colCount=%d\n",
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
bool Multiply::ValidateInputs()
{
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   if ((type1 == Gmat::REAL_TYPE) && (type2 == Gmat::REAL_TYPE))
      return true;
   else if ((type1 == Gmat::RMATRIX_TYPE) && (type2 == Gmat::RMATRIX_TYPE))
      if (col1 == row2)
         return true;
      else
         return false; 
   else
      return true;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the product of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Real Multiply::Evaluate()
{
   Integer type1, row1, col1;
   Integer type2, row2, col2;
   leftNode->GetOutputInfo(type1, row1, col1);
   rightNode->GetOutputInfo(type2, row2, col2);
   
   if (type1 == Gmat::REAL_TYPE && type2 == Gmat::REAL_TYPE)
   {
      return (leftNode->Evaluate() * rightNode->Evaluate());
   }

   
   // Handle column vector * row vector resulting scalar
   if (row1 == col2)
   {
      Rmatrix mat = leftNode->MatrixEvaluate() * rightNode->MatrixEvaluate();
      return mat.GetElement(0,0);
   }
   else
   {
      throw MathException
         ("Multiply::Evaluate() row:%d * col:%d does not produce scalar\n");
   }
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the product of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Rmatrix Multiply::MatrixEvaluate()
{
   Integer type1, row1, col1; // Left node matrix
   Integer type2, row2, col2; // Right node matrix
   Rmatrix prod;
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   // Multiply matrix by matrix
   if( type1 == Gmat::RMATRIX_TYPE && type2 == Gmat::RMATRIX_TYPE)
      prod = leftNode->MatrixEvaluate() * rightNode->MatrixEvaluate();
   
   // Multiply scalar by matrix
   else if( type1 == Gmat::REAL_TYPE && type2 == Gmat::RMATRIX_TYPE)
      prod = leftNode->Evaluate() * rightNode->MatrixEvaluate();
   
   // Multiply matrix by scalar
   else if( type1 == Gmat::RMATRIX_TYPE && type2 == Gmat::REAL_TYPE)
   {
      prod = leftNode->MatrixEvaluate() * rightNode->Evaluate();
   }
   
   return prod;
}

