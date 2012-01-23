//$Id$
//------------------------------------------------------------------------------
//                                  Log10
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
 * Implements Log10 class.
 */
//------------------------------------------------------------------------------

#include "Log10.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Log10()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Log10::Log10(const std::string &nomme)
   : MathFunction("Log10", nomme)
{
}


//------------------------------------------------------------------------------
// ~Log10()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Log10::~Log10()
{
}


//------------------------------------------------------------------------------
//  Log10(const Log10 &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Log10 object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Log10::Log10(const Log10 &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Log10 operation.
 *
 * @return clone of the Log10 operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Log10::Clone() const
{
   return (new Log10(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Log10::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
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
bool Log10::ValidateInputs()
{
   return ValidateScalarInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the base 10 logarithm.
 *
 */
//------------------------------------------------------------------------------
Real Log10::Evaluate()
{
   return GmatMathUtil::Log10(leftNode->Evaluate());
}

