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
//
/**
 * Class implementation for the Nuclear Power System.
 */
//------------------------------------------------------------------------------


#include "NuclearPowerSystem.hpp"
#include "StringUtil.hpp"
#include "HardwareException.hpp"
#include "MessageInterface.hpp"
#include "Spacecraft.hpp"

//#define DEBUG_NUCLEAR

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
//  NuclearPowerSystem()
//------------------------------------------------------------------------------
/**
 * Nuclear Power System constructor.
 *
 * @param nomme Name for the power system.
 */
//------------------------------------------------------------------------------
NuclearPowerSystem::NuclearPowerSystem(const std::string &nomme) :
PowerSystem          ("NuclearPowerSystem",nomme)
{
   objectTypes.push_back(Gmat::NUCLEAR_POWER_SYSTEM);
   objectTypeNames.push_back("NuclearPowerSystem");

   parameterCount = NuclearPowerSystemParamCount;
}


//------------------------------------------------------------------------------
//  ~NuclearPowerSystem()
//------------------------------------------------------------------------------
/**
 * Nuclear Power System destructor.
 */
//------------------------------------------------------------------------------
NuclearPowerSystem::~NuclearPowerSystem()
{
}


//------------------------------------------------------------------------------
//  NuclearPowerSystem(const NuclearPowerSystem& copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * This method is called by the Clone method to replicate power systems.
 *
 * @param copy Reference to the system that gets replicated.
 */
//------------------------------------------------------------------------------
NuclearPowerSystem::NuclearPowerSystem(const NuclearPowerSystem& copy) :
   PowerSystem             (copy)
{
   parameterCount = copy.parameterCount;
}


//------------------------------------------------------------------------------
//  NuclearPowerSystem& operator=(const NuclearPowerSystem& copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * Sets the parameters for one power system equal to another's.
 *
 * @param copy Reference to the system that gets replicated.
 */
//------------------------------------------------------------------------------
NuclearPowerSystem& NuclearPowerSystem::operator=(const NuclearPowerSystem& copy)
{
   #ifdef DEBUG_NUCLEAR
      MessageInterface::ShowMessage("Calling assignment operator on %s\n",
            instanceName.c_str());
   #endif
   if (&copy != this)
   {
      PowerSystem::operator=(copy);
   }

   return *this;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the Nuclear Power System.
 */
//------------------------------------------------------------------------------
bool NuclearPowerSystem::Initialize()
{

   PowerSystem::Initialize();

   return isInitialized;
}

//---------------------------------------------------------------------------
//  GmatBase* Clone() const
//---------------------------------------------------------------------------
/**
 * Provides a clone of this object by calling the copy constructor.
 *
 * @return A GmatBase pointer to the cloned thruster.
 */
//---------------------------------------------------------------------------
GmatBase* NuclearPowerSystem::Clone() const
{
   return new NuclearPowerSystem(*this);
}



//------------------------------------------------------------------------------
//  Real GetPowerGenerated()
//------------------------------------------------------------------------------
Real NuclearPowerSystem::GetPowerGenerated() const
{
   return GetBasePower();
}


//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------
// none at this time
