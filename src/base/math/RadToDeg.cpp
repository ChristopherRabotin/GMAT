//$Id$
//------------------------------------------------------------------------------
//                                  RadToDeg
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
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void RadToDeg::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;
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
   if (leftNode == NULL)
      throw MathException("RadToDeg() - Missing input arguments.\n");
   
   Integer type1, row1, col1; // Left node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Only Real type is allowed
   if (type1 == Gmat::REAL_TYPE)
      return true;
   else
      return false;
   
//    if ( leftNode->ValidateInputs() )
//    {
//       try
//       {
//          leftNode->Evaluate();
//          return true;
//       }
//       catch (MathException &e)
//       {
//          return false;
//       } 
//    }
//    else
//       return false;
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
// Rmatrix *MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix RadToDeg::MatrixEvaluate()
{
   throw MathException("MatrixEvaluate()::RadToDeg returns a real value.\n");    
}

