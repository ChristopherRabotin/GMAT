//$Id$
//------------------------------------------------------------------------------
//                                  Power
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
// Author: LaMont Ruley
// Created: 2006/04/20
//
/**
 * Implements Power class.
 */
//------------------------------------------------------------------------------

#include "Power.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Power()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Power::Power(const std::string &nomme)
   : MathFunction("Power", nomme)
{
}


//------------------------------------------------------------------------------
// ~Power()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Power::~Power()
{
}


//------------------------------------------------------------------------------
//  Power(const Power &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Power object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Power::Power(const Power &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Power operation.
 *
 * @return clone of the Power operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Power::Clone() const
{
   return (new Power(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Power::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   GetScalarOutputInfo(type, rowCount, colCount);
   
   #if 0
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   bool retval = false;
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   if (type1 == Gmat::REAL_TYPE)
   {
      if (type2 == Gmat::REAL_TYPE || type2 == Gmat::RMATRIX_TYPE && row2 == 1 && col2 == 1)
      {
         type = type1;
         rowCount = row1;
         colCount = col1;
         retval = true;
      }
   }
   else if (type1 == Gmat::RMATRIX_TYPE && row1 == 1 && col1 == 1)
   {
      if (type2 == Gmat::REAL_TYPE || (type2 == Gmat::RMATRIX_TYPE && row2 == 1 && col2 == 1))
      {
         type = type1;
         rowCount = row1;
         colCount = col1;
         retval = true;
      }
   }
   
   if (!retval)
      throw MathException("Input is not a scalar or 1x1 matrix, so can not do Power()");
   #endif
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Power::ValidateInputs()
{
   if (leftNode == NULL || rightNode == NULL)
      throw MathException("Power() - Missing input arguments.\n");
   
   Integer type1, row1, col1; // left node
   Integer type2, row2, col2; // right node
   bool retval = false;
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   if (type1 == Gmat::REAL_TYPE)
   {
      if (type2 == Gmat::REAL_TYPE || type2 == Gmat::RMATRIX_TYPE && row2 == 1 && col2 == 1)
         retval = true;
   }
   else if (type1 == Gmat::RMATRIX_TYPE && row1 == 1 && col1 == 1)
   {
      if (type2 == Gmat::REAL_TYPE || (type2 == Gmat::RMATRIX_TYPE && row2 == 1 && col2 == 1))
         retval = true;
   }
   else
      throw MathException("Input is not a scalar or 1x1 matrix, so can not do Power()");
   
   return retval;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return left node raised to the right node power
 *
 */
//------------------------------------------------------------------------------
Real Power::Evaluate()
{
   return GmatMathUtil::Pow(leftNode->Evaluate(), rightNode->Evaluate());
}

