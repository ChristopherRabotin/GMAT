//$Header$
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

//#define DEBUG_DIVIDE 1

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
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Divide::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   #if DEBUG_DIVIDE
   MessageInterface::ShowMessage
      ("Divide::GetOutputInfo() this=<%p><%s><%s>\n", this, GetTypeName().c_str(),
       GetName().c_str());
   #endif
   
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   if (leftNode)
      leftNode->GetOutputInfo(type1, row1, col1);
   else
      throw MathException("Left node is NULL in " + GetTypeName() +
                          "::GetOutputInfo()\n");
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   if (rightNode)
      rightNode->GetOutputInfo(type2, row2, col2);
   else
      throw MathException("Right node is NULL in " + GetTypeName() +
                          "::GetOutputInfo()\n");
   
   #if DEBUG_DIVIDE
   MessageInterface::ShowMessage
      ("Divide::GetOutputInfo() type1=%d, row1=%d, col1=%d, type2=%d, "
       "row2=%d, col2=%d\n", type1, row1, col1, type2, row2, col2);
   #endif
   
   if (type1 == Gmat::REAL_TYPE && type2 == Gmat::RMATRIX_TYPE)
      if (col1 != col2 && row2 != row2)
         throw MathException("Left and Right dimentions not same, can not divide.\n");
   
   if (type1 == Gmat::REAL_TYPE &&  type2 == Gmat::RMATRIX_TYPE)
      throw MathException("Left and Right dimentions not same, can not divide.\n");
   
   type = type1;
   rowCount = row1;
   colCount = col1;
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Divide::ValidateInputs()
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
      if ((row1 == row2) && (col1 == col2))
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
   else if( type1 == Gmat::RMATRIX_TYPE && type2 == Gmat::REAL_TYPE)
      div = leftNode->MatrixEvaluate() / rightNode->Evaluate();

   return div;
}

