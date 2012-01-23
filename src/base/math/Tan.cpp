//$Id$
//------------------------------------------------------------------------------
//                                  Tan
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
 * Implements Tan class.
 */
//------------------------------------------------------------------------------

#include "Tan.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Tan()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Tan::Tan(const std::string &nomme)
   : MathFunction("Tan", nomme)
{
}


//------------------------------------------------------------------------------
// ~Tan()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Tan::~Tan()
{
}


//------------------------------------------------------------------------------
//  Tan(const Tan &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Tan object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Tan::Tan(const Tan &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Tan operation.
 *
 * @return clone of the Tan operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Tan::Clone() const
{
   return (new Tan(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Tan::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
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
bool Tan::ValidateInputs()
{
   return ValidateScalarInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Tan of left node
 *
 */
//------------------------------------------------------------------------------
Real Tan::Evaluate()
{
   return GmatMathUtil::Tan(leftNode->Evaluate());
}

