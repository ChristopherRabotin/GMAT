//$Id$
//------------------------------------------------------------------------------
//                                  Cos
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
 * Implements Cos class.
 */
//------------------------------------------------------------------------------

#include "Cos.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Cos()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Cos::Cos(const std::string &nomme)
   : MathFunction("Cos", nomme)
{
}


//------------------------------------------------------------------------------
// ~Cos()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Cos::~Cos()
{
}


//------------------------------------------------------------------------------
//  Cos(const Cos &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Cos object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Cos::Cos(const Cos &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Cos operation.
 *
 * @return clone of the Cos operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Cos::Clone() const
{
   return (new Cos(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Cos::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
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
bool Cos::ValidateInputs()
{
   return ValidateScalarInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Cos of left node
 *
 */
//------------------------------------------------------------------------------
Real Cos::Evaluate()
{
   return GmatMathUtil::Cos(leftNode->Evaluate());
}

