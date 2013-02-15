//$Id$
//------------------------------------------------------------------------------
//                                  Multiply
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

//#define DEBUG_INPUT_OUTPUT 1
//#define DEBUG_EVALUATE

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
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Multiply::GetOutputInfo() '%s' entered\n", GetName().c_str());
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
   
   #ifdef DEBUG_INPUT_OUTPUT
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
      }
      else
      {
         // Check for 1x1
         if (row1 == 1 && col1 == 1)
         {
            rowCount = row2;
            colCount = col2;
         }
         else if (row2 == 1 && col2 == 1)
         {
            rowCount = row1;
            colCount = col1;            
         }
         else
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
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Multiply::GetOutputInfo() '%s' leaving, type=%d, rowCount=%d, colCount=%d\n",
       GetName().c_str(), type, rowCount, colCount);
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
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("\nMultiply::ValidateInputs() '%s' entered\n", GetName().c_str());
   #endif
   
   if (leftNode == NULL)
      throw MathException("Multiply() - Missing input arguments");
   
   if (rightNode == NULL)
      throw MathException("Multiply() - Not enough input arguments");
   
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   bool retval = false;
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Multiply::ValidateInputs() type1=%d, row1=%d, col1=%d, "
       "type2=%d, row2=%d, col2=%d\n", type1, row1, col1, type2, row2, col2);
   #endif
   
   if ((type1 == Gmat::REAL_TYPE) && (type2 == Gmat::REAL_TYPE))
      retval = true;
   else if ((type1 == Gmat::RMATRIX_TYPE) && (type2 == Gmat::RMATRIX_TYPE))
      if (col1 == row2)
         retval = true;
      else if ((row1 == 1 && col1 == 1) || (row2 == 1 && col2 == 1))
         retval = true;
      else
         retval = false;
   else
      retval = true;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Multiply::ValidateInputs() '%s' returning %d\n", GetName().c_str(), retval);
   #endif
   
   return retval;
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
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("\nMultiply::Evaluate() '%s' entered\n", GetName().c_str());
   #endif
   
   Integer type1, row1, col1;
   Integer type2, row2, col2;
   Real prod;
   bool error = false;
   leftNode->GetOutputInfo(type1, row1, col1);
   rightNode->GetOutputInfo(type2, row2, col2);
   
   if (type1 == Gmat::REAL_TYPE && type2 == Gmat::REAL_TYPE)
   {
      prod = leftNode->Evaluate() * rightNode->Evaluate();
   }
   else if (type1 == Gmat::REAL_TYPE && type2 == Gmat::RMATRIX_TYPE)
   {
      if (row2 == 1 && col2 == 1)
      {
         Rmatrix mat = rightNode->MatrixEvaluate();
         prod = leftNode->Evaluate() * mat(0,0);
         error = false;
      }
   }
   else if (type1 == Gmat::RMATRIX_TYPE && type2 == Gmat::REAL_TYPE)
   {
      if (row1 == 1 && col1 == 1)
      {
         Rmatrix mat = leftNode->MatrixEvaluate();
         prod = mat(0,0) * rightNode->Evaluate();
         error = false;
      }
   }
   else if (type1 == Gmat::RMATRIX_TYPE && type2 == Gmat::RMATRIX_TYPE)
   {
      if (row1 == col2)
      {
         Rmatrix mat = leftNode->MatrixEvaluate() * rightNode->MatrixEvaluate();
         prod = mat.GetElement(0,0);
         error = false;
      }
   }
   
   if (error)
   {
      throw MathException
         ("Multiply::Evaluate() row:%d * col:%d does not produce scalar\n");
   }
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Multiply::Evaluate() '%s' returning %f\n", GetName().c_str(), prod);
   #endif
   
   return prod;
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
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("\nMultiply::MatrixEvaluate() '%s' entered\n", GetName().c_str());
   #endif
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
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Multiply::MatrixEvaluate() '%s' returning %s\n", GetName().c_str(),
       prod.ToString().c_str());
   #endif
   
   return prod;
}

