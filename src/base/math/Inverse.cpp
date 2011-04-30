//$Id$
//------------------------------------------------------------------------------
//                                  Inverse
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "RealUtilities.hpp"
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
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Inverse::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::RMATRIX_TYPE;
   
   leftNode->GetOutputInfo(type, rowCount, colCount);

   if (rowCount == 1 && colCount == 1)
      type = Gmat::REAL_TYPE;

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
   if (leftNode == NULL)
      throw MathException("Inverse() - Missing input arguments.\n");
   
   Integer type, row, col;
   
   GetOutputInfo(type, row, col);
   
   // Only Real type and Matrix are valid. Vector is not a valid input
   if ((type == Gmat::REAL_TYPE) ||
       (type == Gmat::RMATRIX_TYPE && row == col))
      return true;
   
   return false;
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
   return GmatMathUtil::Pow(leftNode->Evaluate(), -1.0);
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
   //=======================================================
   #ifdef DEBUG_EVALUATE
   //=======================================================
   
   Rmatrix rmat = leftNode->MatrixEvaluate();
   MessageInterface::ShowMessage
      ("Inverse::MatrixEvaluate() left node =\n%s\n", rmat.ToString(12).c_str());
   try
   {
      Rmatrix result = rmat.Inverse();
      MessageInterface::ShowMessage
         ("Inverse::MatrixEvaluate() returning\n%s\n", result.ToString(12).c_str());
      return result;
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage
         ("Inverse::MatrixEvaluate() %s for %s\n", be.GetFullMessage().c_str(),
          GetName().c_str());
      throw;
   }
   
   //=======================================================
   #else
   //=======================================================
   
   return (leftNode->MatrixEvaluate()).Inverse();
   
   //=======================================================
   #endif
   //=======================================================
}


