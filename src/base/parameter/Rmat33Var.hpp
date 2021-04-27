//$Id$
//------------------------------------------------------------------------------
//                                  Rmat33Var
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun (GSFC/NASA)
// Created: 2009.03.30
//
/**
 * Declares base class of parameters returning Rmatrix.
 */
//------------------------------------------------------------------------------
#ifndef Rmat33Var_hpp
#define Rmat33Var_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "Rmatrix33.hpp"

class GMAT_API Rmat33Var : public Parameter
{
public:

   Rmat33Var(const std::string &name = "",
             const std::string &typeStr = "Rmat33Var",
             GmatParam::ParameterKey key = GmatParam::USER_PARAM,
             GmatBase *obj = NULL, const std::string &desc = "",
             const std::string &unit = "",
             GmatParam::DepObject depObj = GmatParam::NO_DEP,
             UnsignedInt ownerType = Gmat::UNKNOWN_OBJECT,
             bool isSettable = false);
   Rmat33Var(const Rmat33Var &copy);
   Rmat33Var& operator= (const Rmat33Var& right);
   virtual ~Rmat33Var();
   
   bool operator==(const Rmat33Var &right) const;
   bool operator!=(const Rmat33Var &right) const;
   
   virtual std::string ToString();
   
   virtual const Rmatrix& GetRmatrix() const;
   virtual void  SetRmatrix(const Rmatrix &val);
   virtual const Rmatrix& EvaluateRmatrix();
   
protected:
   
   Rmatrix33 mRmat33Value;
   
private:

};
#endif // Rmat33Var_hpp
