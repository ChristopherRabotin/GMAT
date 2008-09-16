//------------------------------------------------------------------------------
//                                  Transpose
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2006/04/20
//
/**
 * Implements Transpose class.
 */
//------------------------------------------------------------------------------

#include "Transpose.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_TRANSPOSE 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Transpose()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Transpose::Transpose(const std::string &nomme)
   : MathFunction("Transpose", nomme)
{
}


//------------------------------------------------------------------------------
// ~Transpose()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Transpose::~Transpose()
{
}


//------------------------------------------------------------------------------
//  Transpose(const Transpose &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Transpose object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Transpose::Transpose(const Transpose &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Transpose operation.
 *
 * @return clone of the Transpose operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Transpose::Clone() const
{
   return (new Transpose(*this));
}

//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Transpose::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   Integer type1, row1, col1; // Left node

   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);

   if (type1 != Gmat::RMATRIX_TYPE)
      throw MathException("Left is not a matrix, so cannot do Transpose().\n"); 
   else
   {
      // output row and col is transpose of leftNode's row and col
      type = type1;
      rowCount = col1;
      colCount = row1;
   }
   
   #if DEBUG_TRANSPOSE
   MessageInterface::ShowMessage
      ("Transpose::GetOutputInfo() type=%d, rowCount=%d, colCount=%d\n",
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
bool Transpose::ValidateInputs()
{
   Integer type1, row1, col1; // Left node

   #if DEBUG_TRANSPOSE
   MessageInterface::ShowMessage
      ("Transpose::ValidateInputs() left=%s, %s\n",
       leftNode->GetTypeName().c_str(), leftNode->GetName().c_str());
   #endif
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   if (type1 == Gmat::RMATRIX_TYPE)
      return true;
   else
      return false;
   
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the product of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Rmatrix Transpose::MatrixEvaluate()
{
   return (leftNode->MatrixEvaluate()).Transpose();
}


