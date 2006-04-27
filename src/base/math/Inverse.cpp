//------------------------------------------------------------------------------
//                                  Inverse
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
 * Implements Inverse class.
 */
//------------------------------------------------------------------------------

#include "Inverse.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Inverse()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Inverse::Inverse(const std::string &nomme)
   : MathFunction("Inverse", nomme)
{
}


//------------------------------------------------------------------------------
// ~Inverse()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Inverse::~Inverse()
{
}


//------------------------------------------------------------------------------
//  Inverse(const Inverse &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Inverse object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Inverse::Inverse(const Inverse &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Inverse operation.
 *
 * @return clone of the Inverse operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Inverse::Clone() const
{
   return (new Inverse(*this));
}

//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Inverse of left node
 *
 */
//------------------------------------------------------------------------------
Real Inverse::Evaluate()
{
   throw MathException("Evaluate()::Inverse returns a matrix value.\n");    
}

//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Inverse::ValidateInputs()
{
   if ( leftNode->ValidateInputs() )
   {
      try
      {
         leftNode->MatrixEvaluate();
         return true;
      }
      catch (MathException &e)
      {
         return false;
      } 
   }
   else
      return false;
}

//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Inverse::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::RMATRIX_TYPE;
   rowCount = (leftNode->MatrixEvaluate()).GetNumRows();;
   colCount = (leftNode->MatrixEvaluate()).GetNumColumns();
}

//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the product of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Rmatrix Inverse::MatrixEvaluate()
{
   return (leftNode->MatrixEvaluate()).Inverse();
}


