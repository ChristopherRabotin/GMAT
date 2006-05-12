//------------------------------------------------------------------------------
//                                  Atan2
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2006/04/20
//
/**
 * Implements Atan2 class.
 */
//------------------------------------------------------------------------------

#include "Atan2.hpp"
#include <math.h>          // for atan2(double y, double x)
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Atan2()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Atan2::Atan2(const std::string &nomme)
   : MathFunction("Atan2", nomme)
{
}


//------------------------------------------------------------------------------
// ~Atan2()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Atan2::~Atan2()
{
}


//------------------------------------------------------------------------------
//  Atan2(const Atan2 &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Atan2 object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Atan2::Atan2(const Atan2 &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Atan2 operation.
 *
 * @return clone of the Atan2 operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Atan2::Clone() const
{
   return (new Atan2(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Atan2::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);

   if ((type1 != Gmat::REAL_TYPE) && (type2 != Gmat::REAL_TYPE))
      throw MathException("Left and Right is not scalar, so cannot do Atan2().\n");    
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
bool Atan2::ValidateInputs()
{
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);

   if ((type1 == Gmat::REAL_TYPE) && (type2 == Gmat::REAL_TYPE))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return returns the arc tangent of right node / left node 
 * in the range [-, ] radians
 *
 */
//------------------------------------------------------------------------------
Real Atan2::Evaluate()
{
   return atan2(leftNode->Evaluate(), rightNode->Evaluate());
}
