//------------------------------------------------------------------------------
//                                  DegToRad
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
 * Implements DegToRad class.
 */
//------------------------------------------------------------------------------

#include "DegToRad.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// DegToRad()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
DegToRad::DegToRad(const std::string &nomme)
   : MathFunction("DegToRad", nomme)
{
}


//------------------------------------------------------------------------------
// ~DegToRad()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
DegToRad::~DegToRad()
{
}


//------------------------------------------------------------------------------
//  DegToRad(const DegToRad &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the DegToRad object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
DegToRad::DegToRad(const DegToRad &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the DegToRad operation.
 *
 * @return clone of the DegToRad operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* DegToRad::Clone() const
{
   return (new DegToRad(*this));
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the DegToRad of left node
 *
 */
//------------------------------------------------------------------------------
Real DegToRad::Evaluate()
{
   return GmatMathUtil::DegToRad(leftNode->Evaluate());
}

//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool DegToRad::ValidateInputs()
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
void DegToRad::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;
}

//------------------------------------------------------------------------------
// Rmatrix *MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix DegToRad::MatrixEvaluate()
{
   throw MathException("MatrixEvaluate()::DegToRad returns a real value.\n");    
}
