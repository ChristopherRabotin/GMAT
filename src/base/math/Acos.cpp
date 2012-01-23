//$Id$
//------------------------------------------------------------------------------
//                                  Acos
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
 * Implements Acos class.
 */
//------------------------------------------------------------------------------

#include "Acos.hpp"
#include "MessageInterface.hpp"


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Acos()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Acos::Acos(const std::string &nomme)
   : MathFunction("Acos", nomme)
{
}


//------------------------------------------------------------------------------
// ~Acos()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Acos::~Acos()
{
}


//------------------------------------------------------------------------------
//  Acos(const Acos &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Acos object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Acos::Acos(const Acos &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Acos operation.
 *
 * @return clone of the Acos operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Acos::Clone() const
{
   return (new Acos(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Acos::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
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
bool Acos::ValidateInputs()
{
   return ValidateScalarInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Acos of left node which is a scalar
 */
//------------------------------------------------------------------------------
Real Acos::Evaluate()
{
   return GmatMathUtil::ACos(leftNode->Evaluate());
}

