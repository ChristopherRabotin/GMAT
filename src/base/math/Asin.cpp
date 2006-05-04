//------------------------------------------------------------------------------
//                                  ASin
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
 * Implements Asin class.
 */
//------------------------------------------------------------------------------

#include "Asin.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Asin()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Asin::Asin(const std::string &nomme)
   : MathFunction("Asin", nomme)
{
}


//------------------------------------------------------------------------------
// ~Asin()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Asin::~Asin()
{
}


//------------------------------------------------------------------------------
//  Asin(const Asin &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Asin object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Asin::Asin(const Asin &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Asin operation.
 *
 * @return clone of the Asin operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Asin::Clone() const
{
   return (new Asin(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Asin::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);

   if ((type1 != type2) || (row1 != row2) || (col1 != col2))
      throw MathException("Matrixes are not the same can not add.\n");    
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
bool Asin::ValidateInputs()
{
   Integer type1, row1, col1; // Left node
//   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
//   // Get the type(Real or Matrix), # rows and # columns of the right node
//   rightNode->GetOutputInfo(type2, row2, col2);

//   if ((type1 == Gmat::REAL_TYPE) && (type2 == Gmat::REAL_TYPE))
   if (type1 == Gmat::REAL_TYPE)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Asin of left node
 *
 */
//------------------------------------------------------------------------------
Real Asin::Evaluate()
{
   return GmatMathUtil::ASin(leftNode->Evaluate());
}
