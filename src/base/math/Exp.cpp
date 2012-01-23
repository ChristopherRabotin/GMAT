//$Id$
//------------------------------------------------------------------------------
//                                  Exp
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
 * Implements Exp class.
 */
//------------------------------------------------------------------------------

#include "Exp.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Exp()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Exp::Exp(const std::string &nomme)
   : MathFunction("Exp", nomme)
{
}


//------------------------------------------------------------------------------
// ~Exp()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Exp::~Exp()
{
}


//------------------------------------------------------------------------------
//  Exp(const Exp &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Exp object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Exp::Exp(const Exp &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Exp operation.
 *
 * @return clone of the Exp operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Exp::Clone() const
{
   return (new Exp(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Exp::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
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
bool Exp::ValidateInputs()
{
   return ValidateScalarInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the exponential value
 *
 */
//------------------------------------------------------------------------------
Real Exp::Evaluate()
{
   return GmatMathUtil::Exp(leftNode->Evaluate());
}

