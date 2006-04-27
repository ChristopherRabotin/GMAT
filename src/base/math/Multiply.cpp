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
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the product of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Real Multiply::Evaluate()
{
   return leftNode->Evaluate() * rightNode->Evaluate();
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
      prod = leftNode->MatrixEvaluate() * rightNode->Evaluate();

   return prod;
}


