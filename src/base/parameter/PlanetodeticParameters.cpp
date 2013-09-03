//$Id: PlanetodeticParameters.cpp 10206 2013-05-24 Minhee Kim $
//------------------------------------------------------------------------------
//                            PlanetodeticParameters.cpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Minhee Kim
// Created: 2013/5/24
//
/**
 * Declares Planetodetic related parameter classes.
 *   PlanetodeticRMAG, PlanetodeticLON, PlanetodeticLAT, PlanetodeticVMAG, PlanetodeticAZI, PlanetodeticHFPA, PlanetodeticState
 */
//------------------------------------------------------------------------------

#include "PlanetodeticParameters.hpp"
#include "ColorTypes.hpp"

//#define DEBUG_CARTESIAN_PARAM 1


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS

PldRMAG::PldRMAG(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "PlanetodeticRMAG", obj, "Planetodetic RMAG", "km", GmatParam::COORD_SYS, PLD_RMAG, true)
{ 
   //mDepObjectName = "EarthMJ2000Eq";
   mDepObjectName = "EarthFixed";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}

PldRMAG::PldRMAG(const PldRMAG &copy)
   : OrbitReal(copy)
{
}

PldRMAG& PldRMAG::operator=(const PldRMAG&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

PldRMAG::~PldRMAG()
{
}

bool PldRMAG::Evaluate()
{
   mRealValue = OrbitData::GetPlanetodeticReal(PLD_RMAG);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* PldRMAG::Clone(void) const
{
   return new PldRMAG(*this);
}



PldLON::PldLON(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "PlanetodeticLON", obj, "Planetodetic LON", "deg", GmatParam::COORD_SYS, PLD_LON, true)
{
   //mDepObjectName = "EarthMJ2000Eq";
   mDepObjectName = "EarthFixed";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

PldLON::PldLON(const PldLON&copy)
   : OrbitReal(copy)
{
}

PldLON& PldLON::operator=(const PldLON&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

PldLON::~PldLON()
{
}

bool PldLON::Evaluate()
{
   mRealValue = OrbitData::GetPlanetodeticReal(PLD_LON);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* PldLON::Clone(void) const
{
   return new PldLON(*this);
}



PldLAT::PldLAT(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "PlanetodeticLAT", obj, "Planetodetic LAT", "deg", GmatParam::COORD_SYS, PLD_LAT, true)
{
   //mDepObjectName = "EarthMJ2000Eq";
   mDepObjectName = "EarthFixed";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

PldLAT::PldLAT(const PldLAT &copy)
   : OrbitReal(copy)
{
}

PldLAT& PldLAT::operator=(const PldLAT&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

PldLAT::~PldLAT()
{
}

bool PldLAT::Evaluate()
{
   mRealValue = OrbitData::GetPlanetodeticReal(PLD_LAT);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* PldLAT::Clone(void) const
{
   return new PldLAT(*this);
}



PldVMAG::PldVMAG(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "PlanetodeticVMAG", obj, "Planetodetic VMAG", "km/sec", GmatParam::COORD_SYS, PLD_VMAG, true)
{
   //mDepObjectName = "EarthMJ2000Eq";
   mDepObjectName = "EarthFixed";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW32;
   #endif
}

PldVMAG::PldVMAG(const PldVMAG &copy)
   : OrbitReal(copy)
{
}

PldVMAG& PldVMAG::operator=(const PldVMAG&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

PldVMAG::~PldVMAG()
{
}

bool PldVMAG::Evaluate()
{
   mRealValue = OrbitData::GetPlanetodeticReal(PLD_VMAG);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* PldVMAG::Clone(void) const
{
   return new PldVMAG(*this);
}



PldAZI::PldAZI(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "PlanetodeticAZI", obj, "Planetodetic AZI", "deg", GmatParam::COORD_SYS, PLD_AZI, true)
{
   //mDepObjectName = "EarthMJ2000Eq";
   mDepObjectName = "EarthFixed";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

PldAZI::PldAZI(const PldAZI &copy)
   : OrbitReal(copy)
{
}

PldAZI& PldAZI::operator=(const PldAZI&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

PldAZI::~PldAZI()
{
}

bool PldAZI::Evaluate()
{
   mRealValue = OrbitData::GetPlanetodeticReal(PLD_AZI);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* PldAZI::Clone(void) const
{
   return new PldAZI(*this);
}



PldHFPA::PldHFPA(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "PlanetodeticHFPA", obj, "Planetodetic HFPA", "deg", GmatParam::COORD_SYS, PLD_HFPA, true)
{
   //mDepObjectName = "EarthMJ2000Eq";
   mDepObjectName = "EarthFixed";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

PldHFPA::PldHFPA(const PldHFPA &copy)
   : OrbitReal(copy)
{
}

PldHFPA& PldHFPA::operator=(const PldHFPA&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

PldHFPA::~PldHFPA()
{
}

bool PldHFPA::Evaluate()
{
   //mRealValue = OrbitData::GetEquinReal("EquinMlong");
   mRealValue = OrbitData::GetPlanetodeticReal(PLD_HFPA);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* PldHFPA::Clone(void) const
{
   return new PldHFPA(*this);
}



PldState::PldState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "Planetodetic", obj, "Planetodetic State Vector",
                "?", GmatParam::COORD_SYS)
{
   // Parameter member data
   //mDepObjectName = "EarthMJ2000Eq";
   mDepObjectName = "EarthFixed";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   mIsPlottable = false;
}


PldState::PldState(const PldState &copy)
   : OrbitRvec6(copy)
{
}


PldState& PldState::operator=(const PldState &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}


PldState::~PldState()
{
}


bool PldState::Evaluate()
{
   mRvec6Value = GetPlanetodeticState();
   
   #if DEBUG_CARTESIAN_PARAM
   MessageInterface::ShowMessage
      ("EquinState::Evaluate() mRvec6Value =\n%s\n",
       mRvec6Value.ToString().c_str());
   #endif
   
   return mRvec6Value.IsValid(GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
}


GmatBase* PldState::Clone(void) const
{
   return new PldState(*this);
}

