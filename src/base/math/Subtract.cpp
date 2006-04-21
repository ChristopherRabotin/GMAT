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
   : MathFunction("", nomme)
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
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the difference of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Real Subtract::Evaluate()
{
   return leftNode->Evaluate() - rightNode->Evaluate();
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
   return leftNode->MatrixEvaluate() - rightNode->MatrixEvaluate();
}
