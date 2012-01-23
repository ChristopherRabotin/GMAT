//$Id$
//------------------------------------------------------------------------------
//                                  Atan
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
 * Implements Atan class.
 */
//------------------------------------------------------------------------------

#include "Atan.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Atan()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Atan::Atan(const std::string &nomme)
   : MathFunction("Atan", nomme)
{
}


//------------------------------------------------------------------------------
// ~Atan()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Atan::~Atan()
{
}


//------------------------------------------------------------------------------
//  Atan(const Atan &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Atan object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Atan::Atan(const Atan &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Atan operation.
 *
 * @return clone of the Atan operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Atan::Clone() const
{
   return (new Atan(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Atan::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
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
bool Atan::ValidateInputs()
{
   return ValidateScalarInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Atan of left node
 *
 */
//------------------------------------------------------------------------------
Real Atan::Evaluate()
{
   return GmatMathUtil::ATan(leftNode->Evaluate());
}

