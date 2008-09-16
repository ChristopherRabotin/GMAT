//$Header$
//------------------------------------------------------------------------------
//                                  Asinh
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
 * Implements Asinh class.
 */
//------------------------------------------------------------------------------

#include "Asinh.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Asinh()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Asinh::Asinh(const std::string &nomme)
   : MathFunction("Asinh", nomme)
{
}


//------------------------------------------------------------------------------
// ~Asinh()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Asinh::~Asinh()
{
}


//------------------------------------------------------------------------------
//  Asinh(const Asinh &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Asinh object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Asinh::Asinh(const Asinh &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Asinh operation.
 *
 * @return clone of the Asinh operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Asinh::Clone() const
{
   return (new Asinh(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Asinh::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   Integer type1, row1, col1; // Left node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   if (type1 != Gmat::REAL_TYPE)
      throw MathException("Left is not scalar, so cannot do Asinh().\n");  
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
bool Asinh::ValidateInputs()
{
   Integer type1, row1, col1; // Left node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   if (leftNode)
      leftNode->GetOutputInfo(type1, row1, col1);
   else
      throw MathException("Asinh::ValidateInputs() leftNode is NULL\n");
   
   if (type1 == Gmat::REAL_TYPE)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Asinh of left node
 *
 */
//------------------------------------------------------------------------------
Real Asinh::Evaluate()
{
   return GmatMathUtil::ASinh(leftNode->Evaluate());
}
