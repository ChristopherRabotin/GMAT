//------------------------------------------------------------------------------
//                                  Determinant
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
 * Implements Determinant class.
 */
//------------------------------------------------------------------------------

#include "Determinant.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Determinant()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Determinant::Determinant(const std::string &nomme)
   : MathFunction("Determinant", nomme)
{
}


//------------------------------------------------------------------------------
// ~Determinant()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Determinant::~Determinant()
{
}


//------------------------------------------------------------------------------
//  Determinant(const Determinant &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Determinant object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Determinant::Determinant(const Determinant &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Determinant operation.
 *
 * @return clone of the Determinant operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Determinant::Clone() const
{
   return (new Determinant(*this));
}

//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Determinant of left node
 *
 */
//------------------------------------------------------------------------------
Real Determinant::Evaluate()
{
   return (leftNode->MatrixEvaluate()).Determinant();
}

//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Determinant::ValidateInputs()
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
void Determinant::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;
}

//------------------------------------------------------------------------------
// Rmatrix *MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix Determinant::MatrixEvaluate()
{
   throw MathException("MatrixEvaluate()::Determinant returns a real value.\n");    
}

