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
bool DegToRad::ValidateInputs()
{
   return ValidateScalarInputs();
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

