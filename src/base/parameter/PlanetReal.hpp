//$Id$
//------------------------------------------------------------------------------
//                                PlanetReal
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
// Created: 2004/12/13
//
/**
 * Declares PlanetReal class which provides base class for the planet
 * realated Real Parameters
 */
//------------------------------------------------------------------------------
#ifndef PlanetReal_hpp
#define PlanetReal_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "PlanetData.hpp"


class GMAT_API PlanetReal : public RealVar, public PlanetData
{
public:

   PlanetReal(const std::string &name, const std::string &typeStr, 
              GmatBase *obj, const std::string &desc,
              const std::string &unit, UnsignedInt ownerType,
              GmatParam::DepObject depObj);
   PlanetReal(const PlanetReal &copy);
   PlanetReal& operator=(const PlanetReal &right);
   virtual ~PlanetReal();

   // methods inherited from Parameter
   virtual Real EvaluateReal();

   virtual Integer GetNumRefObjects() const;
   virtual bool AddRefObject(GmatBase *obj, bool replaceName = false);
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetInternalCoordSystem(CoordinateSystem *ss);
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

#endif // PlanetReal_hpp
