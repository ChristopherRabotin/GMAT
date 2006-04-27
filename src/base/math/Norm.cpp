//------------------------------------------------------------------------------
//                                  Norm
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
 * Implements Norm class.
 */
//------------------------------------------------------------------------------

#include "Norm.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Norm()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Norm::Norm(const std::string &nomme)
   : MathFunction("Norm", nomme)
{
}


//------------------------------------------------------------------------------
// ~Norm()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Norm::~Norm()
{
}


//------------------------------------------------------------------------------
//  Norm(const Norm &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Norm object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Norm::Norm(const Norm &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Norm operation.
 *
 * @return clone of the Norm operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Norm::Clone() const
{
   return (new Norm(*this));
}

//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Norm of left node
 *
 */
//------------------------------------------------------------------------------
Real Norm::Evaluate()
{
//   throw MathException("Evaluate()::Norm returns a matrix value.\n");
   Integer type, rowCount, colCount;
   leftNode->GetOutputInfo(type, rowCount, colCount);

   if(type == Gmat::RMATRIX_TYPE)
   {
      if (rowCount == 1)
      {
        return (leftNode->MatrixEvaluate()).GetRow(0).Norm();
      }
      else if (colCount == 1)
      {
        return (leftNode->MatrixEvaluate()).GetColumn(0).Norm();
      }
      else
         throw MathException(
            "Norm::Evaluate()::Can only be done on a vector.  This is a matrix.\n");
   }
   else
      throw MathException("Norm::Evaluate()::Left Node is not RMatrix type.\n");
   
//   Rmatrix matrix = leftNode->EvaluateMatrix();
}

//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Norm::ValidateInputs()
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
void Norm::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;
}

//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the product of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Rmatrix Norm::MatrixEvaluate()
{
   throw MathException("MatrixEvaluate()::Norm returns a real value.\n");    
}


