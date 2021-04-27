//------------------------------------------------------------------------------
//                          BrysonDenhamPointObject
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
/**
 * INSTRUCTIONS:
 *  - Find and Replace " BrysonDenham" with the actual name of your test problem.
 */
//------------------------------------------------------------------------------
#ifndef  BrysonDenhamPointObject_hpp
#define  BrysonDenhamPointObject_hpp

#include "UserPointFunction.hpp"

/**
 *  BrysonDenhamPointObject class
 */
class  BrysonDenhamPointObject : public UserPointFunction
{
public:
    BrysonDenhamPointObject();
   virtual ~ BrysonDenhamPointObject();
    BrysonDenhamPointObject(const  BrysonDenhamPointObject &copy);
    BrysonDenhamPointObject& operator=(const  BrysonDenhamPointObject &copy);
   
   void EvaluateFunctions();
   void EvaluateJacobians();
};

#endif //  BrysonDenhamPointObject_hpp
