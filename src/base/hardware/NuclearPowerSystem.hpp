//$Id$
//------------------------------------------------------------------------------
//                               NuclearPowerSystem
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2014.04.28
//
/**
 * Base class definition for the NuclearPowerSystem.
 */
//------------------------------------------------------------------------------


#ifndef NuclearPowerSystem_hpp
#define NuclearPowerSystem_hpp

#include "PowerSystem.hpp"

/**
 * Basic nuclear power system model attached to Spacecraft.
 */
class GMAT_API NuclearPowerSystem : public PowerSystem
{
public:

   NuclearPowerSystem(const std::string &nomme);
   virtual ~NuclearPowerSystem();
   NuclearPowerSystem(const NuclearPowerSystem& copy);
   NuclearPowerSystem& operator=(const NuclearPowerSystem& copy);

   virtual bool         Initialize();
   virtual GmatBase*    Clone() const;

   virtual Real         GetPowerGenerated() const; // Total Power

   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Published parameters for nuclear power systems
   enum
   {
      NuclearPowerSystemParamCount = PowerSystemParamCount,
   };

};

#endif // NuclearPowerSystem_hpp
