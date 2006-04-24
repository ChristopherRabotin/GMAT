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
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Transpose of left node
 *
 */
//------------------------------------------------------------------------------
Real Transpose::Evaluate()
{
   throw MathException("Evaluate()::Transpose returns a matrix value.\n");    
}

//------------------------------------------------------------------------------
// bool EvaluateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Transpose::EvaluateInputs()
{
   if ( leftNode->EvaluateInputs() )
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
// void ReportOutputs(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Transpose::ReportOutputs(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::RMATRIX_TYPE;
   rowCount = (leftNode->MatrixEvaluate()).GetNumRows();
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
Rmatrix Transpose::MatrixEvaluate()
{
   return (leftNode->MatrixEvaluate()).Transpose();
}


