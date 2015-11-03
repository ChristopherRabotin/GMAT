//$Id$
//------------------------------------------------------------------------------
//                              FiniteBurn
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/12/20
//
/**
 * Defines the FiniteBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#ifndef FiniteBurn_hpp
#define FiniteBurn_hpp

#include "Burn.hpp"


/**
 * Class used to configure finite burns.
 */
 class GMAT_API FiniteBurn : public Burn
{
public:
   FiniteBurn(const std::string &nomme = "");
   virtual ~FiniteBurn();
   FiniteBurn(const FiniteBurn& fb);
   FiniteBurn&          operator=(const FiniteBurn& fb);
   
   // inherited methods from Burn
   virtual void         SetSpacecraftToManeuver(Spacecraft *sat);
   virtual bool         Fire(Real *burnData = NULL, Real epoch = GmatTimeConstants::MJD_OF_J2000,
                             bool backwards = false);
   
   // Inherited (GmatBase) methods
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const Integer id,
                                           const char *value,
                                           const Integer index);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id, const Real value);

   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual Gmat::ObjectType
                        GetPropertyObjectType(const Integer id) const;
   
   virtual bool         Initialize();
   
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual bool         DepletesMass();

protected:

   /// List of thrusters used in the maneuver
   StringArray             thrusterNames;
   /// List of thrusters used in the maneuver
   ObjectMap               thrusterMap;
   /// List of fuel tanks used in the maneuver (deprecated)
   StringArray             tankNames;
   /// The throttle logic algorithm - used to determine how
   // power is distributed amongst electric thrusters
   std::string             throttleLogicAlgorithm;
   /// Are the thrusters of type Electric?  If not, then they are
   /// Chemical thrusters
   bool                    isElectricBurn;
   
   bool SetThrustersFromSpacecraft();
   
   /// Published parameters for thrusters
   enum
   {
      THRUSTER = BurnParamCount,
      FUEL_TANK,
      BURN_SCALE_FACTOR,
      THROTTLE_LOGIC_ALGORITHM,
      FiniteBurnParamCount
   };
   
   /// Thruster parameter labels
   static const std::string 
                        PARAMETER_TEXT[FiniteBurnParamCount - BurnParamCount];
   /// Thruster parameter types
   static const Gmat::ParameterType 
                        PARAMETER_TYPE[FiniteBurnParamCount - BurnParamCount];
   
   /// When the thrusters are electric, we must compute the throttle logic
   // given the available thrust power.  This method will set the power
   // level on each of the electric thrusters.
   bool ComputeThrottleLogic(Real powerAvailable);

};

#endif // FiniteBurn_hpp
