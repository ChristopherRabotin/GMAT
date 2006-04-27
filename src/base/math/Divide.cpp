//------------------------------------------------------------------------------
//                                  Divide
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
 * Implements Divide class.
 */
//------------------------------------------------------------------------------

#include "Divide.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Divide()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Divide::Divide(const std::string &nomme)
   : MathFunction("Divide", nomme)
{
}


//------------------------------------------------------------------------------
// ~Divide()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Divide::~Divide()
{
}


//------------------------------------------------------------------------------
//  Divide(const Divide &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Divide object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Divide::Divide(const Divide &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Divide operation.
 *
 * @return clone of the Divide operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Divide::Clone() const
{
   return (new Divide(*this));
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the product of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Real Divide::Evaluate()
{
   return leftNode->Evaluate() / rightNode->Evaluate();
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the division of left node by the right node
 *
 */
//------------------------------------------------------------------------------
Rmatrix Divide::MatrixEvaluate()
{
   Integer type1, row1, col1; // Left node matrix
   Integer type2, row2, col2; // Right node matrix
   Rmatrix div;
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   // Divide matrix by matrix
   if( type1 == Gmat::RMATRIX_TYPE && type2 == Gmat::RMATRIX_TYPE)
      div = leftNode->MatrixEvaluate() / rightNode->MatrixEvaluate();
   // Divide scalar by matrix
//   else if( type1 == Gmat::REAL_TYPE && type2 == Gmat::RMATRIX_TYPE)
//      div = leftNode->Evaluate() / rightNode->MatrixEvaluate();
   // Divide matrix by scalar
   else if( type1 == Gmat::RMATRIX_TYPE && type2 == Gmat::REAL_TYPE)
      div = leftNode->MatrixEvaluate() / rightNode->Evaluate();

   return div;
}

