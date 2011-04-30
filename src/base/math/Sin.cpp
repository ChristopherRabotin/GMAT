//$Id$
//------------------------------------------------------------------------------
//                                  Sin
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
// Created: 2006/04/10
//
/**
 * Implements Sin class.
 */
//------------------------------------------------------------------------------

#include "Sin.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Sin()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Sin::Sin(const std::string &nomme)
   : MathFunction("Sin", nomme)
{
}


//------------------------------------------------------------------------------
// ~Sin()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Sin::~Sin()
{
}


//------------------------------------------------------------------------------
//  Sin(const Sin &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Sin object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Sin::Sin(const Sin &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Sin operation.
 *
 * @return clone of the Sin operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Sin::Clone() const
{
   return (new Sin(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Sin::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   Integer type1, row1, col1; // Left node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   

   if (type1 != Gmat::REAL_TYPE)
      throw MathException("Left is not scalar, so cannot do Sin().\n"); 
   else
   {
      type = type1;
      rowCount = row1;
      colCount = col1;
   }
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Sin::ValidateInputs()
{
   if (leftNode == NULL)
      throw MathException("Sin() - Missing input arguments.\n");
   
   Integer type1, row1, col1; // Left node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   if (type1 == Gmat::REAL_TYPE)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the sin of left node
 *
 */
//------------------------------------------------------------------------------
Real Sin::Evaluate()
{
   return GmatMathUtil::Sin(leftNode->Evaluate());
}
