//------------------------------------------------------------------------------
//                                  Atan
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2006/04/10
//
/**
 * Implements Atan class.
 */
//------------------------------------------------------------------------------

#include "Atan.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Atan()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Atan::Atan(const std::string &nomme)
   : MathFunction("Atan", nomme)
{
}


//------------------------------------------------------------------------------
// ~Atan()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Atan::~Atan()
{
}


//------------------------------------------------------------------------------
//  Atan(const Atan &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Atan object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Atan::Atan(const Atan &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Atan operation.
 *
 * @return clone of the Atan operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Atan::Clone() const
{
   return (new Atan(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Atan::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   Integer type1, row1, col1; // Left node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   if (type1 != Gmat::REAL_TYPE)
      throw MathException("Left is not scalar, so cannot do Atan().\n");  
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
bool Atan::ValidateInputs()
{
   Integer type1, row1, col1; // Left node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   if (type1 == Gmat::REAL_TYPE)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Atan of left node
 *
 */
//------------------------------------------------------------------------------
Real Atan::Evaluate()
{
   return GmatMathUtil::ATan(leftNode->Evaluate());
}
