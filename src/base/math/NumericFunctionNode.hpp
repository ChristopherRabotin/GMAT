//$Id$
//------------------------------------------------------------------------------
//                                   NumericFunctionNode
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
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
// number NNG04CC06P.
//
// Author: Linda Jun
// Created: 2016.02.17
//
/**
 * Defines the numeric functions base class.
 */
//------------------------------------------------------------------------------

#ifndef NumericFunctionNode_hpp
#define NumericFunctionNode_hpp

#include "GmatBase.hpp"
#include "BuiltinFunctionNode.hpp"
#include "MathException.hpp"

class GMAT_API NumericFunctionNode : public BuiltinFunctionNode
{
public:
   NumericFunctionNode(const std::string &typeStr, const std::string &name);
   virtual ~NumericFunctionNode();
   NumericFunctionNode(const NumericFunctionNode &nfn);
   NumericFunctionNode& operator=(const NumericFunctionNode &nfn);
   
   virtual bool ValidateInputs(); 
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   
protected:
};

#endif //NumericFunctionNode_hpp
