//$Id$
//------------------------------------------------------------------------------
//                                  Negate
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
// Author: Linda Jun, NASA/GSFC
// Created: 2006/05/04
//
/**
 * Implements Negate class.
 */
//------------------------------------------------------------------------------

#include "Negate.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Negate()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Negate::Negate(const std::string &nomme)
   : MathFunction("Negate", nomme)
{
}


//------------------------------------------------------------------------------
// ~Negate()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Negate::~Negate()
{
}


//------------------------------------------------------------------------------
//  Negate(const Negate &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Negate object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Negate::Negate(const Negate &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Negate operation.
 *
 * @return clone of the Negate operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Negate::Clone() const
{
   return (new Negate(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Negate::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   Integer type1, row1, col1; // Left node
   
   if (!leftNode)
      throw MathException("Negate::GetOutputInfo() The left node is NULL");
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   type = type1;
   rowCount = row1;
   colCount = col1;
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Negate::ValidateInputs()
{
   if (leftNode == NULL)
      throw MathException("Negate() - Missing input arguments.\n");
   
   // it can be any type
   return true;
   //return leftNode->ValidateInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Negate of left node
 *
 */
//------------------------------------------------------------------------------
Real Negate::Evaluate()
{
   return (leftNode->Evaluate() * -1);
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix Negate::MatrixEvaluate()
{
   return (leftNode->MatrixEvaluate() * -1);
}
