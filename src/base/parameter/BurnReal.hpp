//$Id$
//------------------------------------------------------------------------------
//                                BurnReal
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
// Created: 2004/03/29
//
/**
 * Declares BurnReal class which provides base class for orbit realated Real
 * Parameters
 */
//------------------------------------------------------------------------------
#ifndef BurnReal_hpp
#define BurnReal_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "BurnData.hpp"


class GMAT_API BurnReal : public RealVar, public BurnData
{
public:

   BurnReal(const std::string &name, const std::string &typeStr,
            UnsignedInt ownerType, GmatBase *obj,
            const std::string &desc, const std::string &unit,
            GmatParam::DepObject depObj, bool isSettable);
   BurnReal(const BurnReal &copy);
   BurnReal& operator=(const BurnReal &right);
   virtual ~BurnReal();

   // methods inherited from Parameter
   virtual Real EvaluateReal();
   
   virtual Integer GetNumRefObjects() const;
   virtual CoordinateSystem* GetInternalCoordSystem();
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetInternalCoordSystem(CoordinateSystem *ss);
   virtual bool AddRefObject(GmatBase*obj, bool replaceName = false);
   virtual bool Validate();
   virtual bool Initialize();
   
   // methods inherited from GmatBase
   virtual bool RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual std::string GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type);
   virtual bool SetRefObjectName(const UnsignedInt type,
                                 const std::string &name);
   virtual GmatBase* GetRefObject(const UnsignedInt type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name = "");

protected:

};

#endif // BurnReal_hpp
