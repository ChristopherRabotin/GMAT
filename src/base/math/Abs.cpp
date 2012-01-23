//$Id$
//------------------------------------------------------------------------------
//                                  Abs
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
// Created: 2006/06/27
//
/**
 * Implements Abs class.
 */
//------------------------------------------------------------------------------

#include "Abs.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Abs()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Abs::Abs(const std::string &nomme)
   : MathFunction("Abs", nomme)
{
}


//------------------------------------------------------------------------------
// ~Abs()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Abs::~Abs()
{
}


//------------------------------------------------------------------------------
//  Abs(const Abs &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Abs object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Abs::Abs(const Abs &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Abs operation.
 *
 * @return clone of the Abs operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Abs::Clone() const
{
   return (new Abs(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Abs::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   GetScalarOutputInfo(type, rowCount, colCount);
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Abs::ValidateInputs()
{
   return ValidateScalarInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Abs of left node
 *
 */
//------------------------------------------------------------------------------
Real Abs::Evaluate()
{
   return GmatMathUtil::Abs(leftNode->Evaluate());
}

