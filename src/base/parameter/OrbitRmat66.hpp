//$Id$
//------------------------------------------------------------------------------
//                                  OrbitRmat66
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
// Author: Linda Jun
// Created: 2009.03.30
//
/**
 * Declares SpacecraftState Rmatrix data class.
 */
//------------------------------------------------------------------------------
#ifndef OrbitRmat66_hpp
#define OrbitRmat66_hpp

#include "gmatdefs.hpp"
#include "Rmat66Var.hpp"
#include "OrbitData.hpp"


class GMAT_API OrbitRmat66 : public Rmat66Var, public OrbitData
{
public:
   
   OrbitRmat66(const std::string &name, const std::string &typeStr, 
               GmatBase *obj, const std::string &desc, const std::string &unit,
               GmatParam::DepObject depObj, bool isSettable = false,
               Gmat::ObjectType paramOwnerType = Gmat::SPACECRAFT);
   OrbitRmat66(const OrbitRmat66 &copy);
   OrbitRmat66& operator=(const OrbitRmat66 &right);
   virtual ~OrbitRmat66();
   
   // methods inherited from Parameter
   virtual const Rmatrix& EvaluateRmatrix();
   
   virtual Integer      GetNumRefObjects() const;
   virtual CoordinateSystem* GetInternalCoordSystem();
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetInternalCoordSystem(CoordinateSystem *ss);
   virtual bool         AddRefObject(GmatBase*obj, bool replaceName = false);
   virtual bool         Validate();
   virtual bool         Initialize();
   
   // methods inherited from GmatBase
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         SetRefObjectName(const Gmat::ObjectType type,
                                         const std::string &name);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");

protected:

};

#endif /*OrbitRmat66_hpp*/
