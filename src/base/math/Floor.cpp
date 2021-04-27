//$Id$
//------------------------------------------------------------------------------
//                                  Floor
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2016.03.08
//
/**
 * Implements Floor class.
 */
//------------------------------------------------------------------------------

#include "Floor.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Floor()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Floor::Floor(const std::string &nomme)
   : MathFunction("Floor", nomme)
{
}


//------------------------------------------------------------------------------
// ~Floor()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Floor::~Floor()
{
}


//------------------------------------------------------------------------------
//  Floor(const Floor &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Floor object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Floor::Floor(const Floor &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Floor operation.
 *
 * @return clone of the Floor operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Floor::Clone() const
{
   return (new Floor(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Floor::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
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
bool Floor::ValidateInputs()
{
   return ValidateScalarInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Floor of left node
 *
 */
//------------------------------------------------------------------------------
Real Floor::Evaluate()
{
   return GmatMathUtil::Floor(leftNode->Evaluate());
}

