//$Id$
//------------------------------------------------------------------------------
//                                EnvReal
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
// Created: 2004/12/10
//
/**
 * Declares EnvReal class which provides base class for environment realated
 * Real Parameters
 */
//------------------------------------------------------------------------------
#ifndef EnvReal_hpp
#define EnvReal_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "EnvData.hpp"


class GMAT_API EnvReal : public RealVar, public EnvData
{
public:

   EnvReal(const std::string &name, const std::string &typeStr, 
           GmatBase *obj, const std::string &desc,
           const std::string &unit, Gmat::ObjectType ownerType,
           GmatParam::DepObject depObj);
   EnvReal(const EnvReal &copy);
   EnvReal& operator=(const EnvReal &right);
   virtual ~EnvReal();

   // methods inherited from Parameter
   virtual Real EvaluateReal();
   
   virtual Integer GetNumRefObjects() const;
   virtual bool AddRefObject(GmatBase *obj, bool replaceName = false);
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual bool Validate();
   virtual bool Initialize();
   
   // methods inherited from GmatBase
   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool SetRefObjectName(const Gmat::ObjectType type,
                                 const std::string &name);
   virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name = "");
protected:
   

};

#endif // EnvReal_hpp
