//$Header$
//------------------------------------------------------------------------------
//                                  Abs
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2006/06/27
//
/**
 * Implements Abs class.
 */
//------------------------------------------------------------------------------

#include "Abs.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Abs()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Abs::Abs(const std::string &nomme)
   : MathFunction("Abs", nomme)
{
}


//------------------------------------------------------------------------------
// ~Abs()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Abs::~Abs()
{
}


//------------------------------------------------------------------------------
//  Abs(const Abs &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Abs object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Abs::Abs(const Abs &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Abs operation.
 *
 * @return clone of the Abs operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Abs::Clone() const
{
   return (new Abs(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Abs::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   Integer type1, row1, col1; // Left node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   if (type1 != Gmat::REAL_TYPE)
      throw MathException("Left is not scalar, so cannot do Abs().\n");  
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
bool Abs::ValidateInputs()
{
   Integer type1, row1, col1; // Left node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   if (leftNode)
      leftNode->GetOutputInfo(type1, row1, col1);
   else
      throw MathException("Abs::ValidateInputs() leftNode is NULL\n");
   
   if (type1 == Gmat::REAL_TYPE)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Abs of left node
 *
 */
//------------------------------------------------------------------------------
Real Abs::Evaluate()
{
   return GmatMathUtil::Abs(leftNode->Evaluate());
}
