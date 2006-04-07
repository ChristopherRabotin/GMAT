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
   : MathFunction("", nomme)
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
   return leftNode->Evaluate() * rightNode->Evaluate();
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the divide of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Rmatrix Divide::MatrixEvaluate()
{
   Integer type, row, col;
   Rmatrix div;
   
   // Get the type of the left node (Real or Matrix)
   rightNode->ReportOutputs(type, row, col);
   
   if( type == Gmat::REAL_TYPE)           // Divide matrix by scalar
      div = leftNode->MatrixEvaluate() / rightNode->Evaluate();
   else if( type == Gmat::RMATRIX_TYPE)   // Divide matrix by matrix
      div = leftNode->MatrixEvaluate() / rightNode->MatrixEvaluate();

   return div;
}

