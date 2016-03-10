//------------------------------------------------------------------------------
//                              Sprintf
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
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
// Created: 2016.02.17
//
/**
 * Declares Sprintf class.
 */
//------------------------------------------------------------------------------
#ifndef Sprintf_hpp
#define Sprintf_hpp

#include "StringFunction.hpp"

class GMAT_API Sprintf : public StringFunction
{
public:
   Sprintf(const std::string &name);
   virtual ~Sprintf();
   Sprintf(const Sprintf &copy);
   
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   // inherited from MathNode
   virtual bool ValidateInputs(); 
   virtual std::string EvaluateString();
};

#endif // Sprintf_hpp
