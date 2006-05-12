//------------------------------------------------------------------------------
//                                  RadToDeg
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
 * Implements RadToDeg class.
 */
//------------------------------------------------------------------------------

#include "RadToDeg.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RadToDeg()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
RadToDeg::RadToDeg(const std::string &nomme)
   : MathFunction("RadToDeg", nomme)
{
}


//------------------------------------------------------------------------------
// ~RadToDeg()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
RadToDeg::~RadToDeg()
{
}


//------------------------------------------------------------------------------
//  RadToDeg(const RadToDeg &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the RadToDeg object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
RadToDeg::RadToDeg(const RadToDeg &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the RadToDeg operation.
 *
 * @return clone of the RadToDeg operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* RadToDeg::Clone() const
{
   return (new RadToDeg(*this));
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the RadToDeg of left node
 *
 */
//------------------------------------------------------------------------------
Real RadToDeg::Evaluate()
{
   return GmatMathUtil::RadToDeg(leftNode->Evaluate());
}

//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool RadToDeg::ValidateInputs()
{
   if ( leftNode->ValidateInputs() )
   {
      try
      {
         leftNode->Evaluate();
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
void RadToDeg::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;
}

//------------------------------------------------------------------------------
// Rmatrix *MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix RadToDeg::MatrixEvaluate()
{
   throw MathException("MatrixEvaluate()::RadToDeg returns a real value.\n");    
}

