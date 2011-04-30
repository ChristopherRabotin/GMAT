//$Id$
//------------------------------------------------------------------------------
//                                  DegToRad
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
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void DegToRad::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
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
bool DegToRad::ValidateInputs()
{
   if (leftNode == NULL)
      throw MathException("DegToRad() - Missing input arguments.\n");
   
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
 * @return the DegToRad of left node
 *
 */
//------------------------------------------------------------------------------
Real DegToRad::Evaluate()
{
   return GmatMathUtil::DegToRad(leftNode->Evaluate());
}


//------------------------------------------------------------------------------
// Rmatrix *MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix DegToRad::MatrixEvaluate()
{
   throw MathException("MatrixEvaluate()::DegToRad returns a real value.\n");    
}
