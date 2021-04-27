//------------------------------------------------------------------------------
//                         Hull95PointObject
//------------------------------------------------------------------------------
// CSALT: Collocation Stand Alone Library and Toolkit
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Mar 13, 2017
//
/**
 * INSTRUCTIONS:
 *  - Find and Replace "Hull95" with the actual name of your test problem.
 *  - Fill in EvaluateFunctions
 *  - If applicable, fill in EvaluateJacobians
 */
//------------------------------------------------------------------------------

#include "Hull95PointObject.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------

Hull95PointObject::Hull95PointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
Hull95PointObject::Hull95PointObject(const Hull95PointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
Hull95PointObject& Hull95PointObject::operator=(const Hull95PointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
Hull95PointObject::~Hull95PointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void Hull95PointObject::EvaluateFunctions()
{
   // Extract parameter data
   Rvector stateInit     = GetInitialStateVector(0);
   Real    tInit         = GetInitialTime(0);
   Real    tFinal        = GetFinalTime(0);

   Rvector algFunctions(3, tInit, tFinal, stateInit(0));
   Rvector algFuncLower(3, 0.0, 1.0, 0.0);
   Rvector algFuncUpper(3, 0.0, 1.0, 0.0);

   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, LOWER, algFuncLower);
   SetFunctionBounds(ALGEBRAIC, UPPER, algFuncUpper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void Hull95PointObject::EvaluateJacobians()
{
}
