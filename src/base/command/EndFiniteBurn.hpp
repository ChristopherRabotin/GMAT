//$Id$
//------------------------------------------------------------------------------
//                            EndFiniteBurn
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS task
// order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2005/01/04
//
/**
 *  Definition code for the EndFiniteBurn command.
 */
//------------------------------------------------------------------------------

#ifndef EndFiniteBurn_hpp
#define EndFiniteBurn_hpp


#include "GmatCommand.hpp"
#include "FiniteBurn.hpp"
#include "Spacecraft.hpp"
#include "Thruster.hpp"
#include "FiniteThrust.hpp"


/**
 * EndFiniteBurn Class, used to turn off thrusters used in a finite burn.
 */
class GMAT_API EndFiniteBurn : public GmatCommand
{
public:
   EndFiniteBurn();
   virtual ~EndFiniteBurn();
   EndFiniteBurn(const EndFiniteBurn& endman);
   EndFiniteBurn&         operator=(const EndFiniteBurn& endman);

   virtual bool TakeAction(const std::string &action,  
                           const std::string &actionData = "");
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const ObjectTypeArray&
                       GetRefObjectTypeArray();
   virtual const StringArray&
                       GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool        SetRefObjectName(const Gmat::ObjectType type,
                                        const std::string &name);

   virtual GmatBase*    Clone() const;
   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");

   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   
   virtual bool         Initialize();
   virtual bool         Execute();

   DEFAULT_TO_NO_CLONES

protected:
   /// Name of the FiniteBurn object used to set the maneuver details
   std::string                   thrustName;
   /// The FiniteThrust that is available for the force models
   FiniteThrust                  *burnForce;

   /// Name of the FiniteBurn object used to set the maneuver details
   std::string                   burnName;
   /// The FiniteBurn object
   FiniteBurn                    *maneuver;
   /// The vector of forces managed by the Sandbox
   std::vector<PhysicalModel*>   *transientForces;
   /// The names of the spacecraft that get maneuvered
   StringArray                   satNames;
   /// The spacecraft that get maneuvered
   std::vector<Spacecraft *>     sats;
   /// The thrusters that get deactivated
   std::vector<Thruster *>       thrusters;
   
   void ValidateThrusters();
};

#endif // EndFiniteBurn_hpp
