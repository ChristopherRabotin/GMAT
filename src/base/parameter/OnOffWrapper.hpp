//$Id$
//------------------------------------------------------------------------------
//                                  OnOffWrapper
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. 
//
// Author: Linda Jun/GSFC
// Created: 2007/07/24
//
/**
 * Declares OnOffWrapper class.
 */
//------------------------------------------------------------------------------
#ifndef OnOffWrapper_hpp
#define OnOffWrapper_hpp

#include "gmatdefs.hpp"
#include "ElementWrapper.hpp"

class GMAT_API OnOffWrapper : public ElementWrapper
{
public:

   OnOffWrapper();
   OnOffWrapper(const OnOffWrapper &copy);
   const OnOffWrapper& operator=(const OnOffWrapper &right);
   virtual ~OnOffWrapper();
   
   virtual ElementWrapper*     Clone() const;

   virtual Gmat::ParameterType GetDataType() const;
   
   virtual Real         EvaluateReal() const;
   virtual bool         SetReal(const Real val);
   
   virtual std::string  EvaluateOnOff() const;
   virtual bool         SetOnOff(const std::string &val);
   
protected:  

   // the string value of "On" or "Off"
   std::string value;
   
   virtual void         SetupWrapper();
};
#endif // OnOffWrapper_hpp
