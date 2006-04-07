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
   : MathFunction("", nomme)
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
   Integer type, row, col;
   Rmatrix prod;
   
   // Get the type of the left node (Real or Matrix)
   leftNode->ReportOutputs(type, row, col);
   
   if( type == Gmat::REAL_TYPE)           // Multipling scalar time matrix
      prod = leftNode->Evaluate() * rightNode->MatrixEvaluate();
   else if( type == Gmat::RMATRIX_TYPE)   // Multipling matrix time matrix
      prod = leftNode->MatrixEvaluate() * rightNode->MatrixEvaluate();
   else    								  // Multipling matrix time scalar
      prod = leftNode->MatrixEvaluate() * rightNode->Evaluate();

   return prod;
}


