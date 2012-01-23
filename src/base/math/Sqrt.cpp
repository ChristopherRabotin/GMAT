//$Id$
//------------------------------------------------------------------------------
//                                  Sqrt
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
// Created: 2006/04/14
//
/**
 * Implements Sqrt class.
 */
//------------------------------------------------------------------------------

#include "Sqrt.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Sqrt()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Sqrt::Sqrt(const std::string &nomme)
   : MathFunction("Sqrt", nomme)
{
}


//------------------------------------------------------------------------------
// ~Sqrt()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Sqrt::~Sqrt()
{
}


//------------------------------------------------------------------------------
//  Sqrt(const Sqrt &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Sqrt object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Sqrt::Sqrt(const Sqrt &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Sqrt operation.
 *
 * @return clone of the Sqrt operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Sqrt::Clone() const
{
   return (new Sqrt(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Sqrt::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
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
bool Sqrt::ValidateInputs()
{
   return ValidateScalarInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the sqaure root
 */
//------------------------------------------------------------------------------
Real Sqrt::Evaluate()
{
   return GmatMathUtil::Sqrt(leftNode->Evaluate());
}

