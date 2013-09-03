//$Id: DelaunayParameters.cpp 10206 2013-05-24 Minhee Kim $
//------------------------------------------------------------------------------
//                            DelaunayParameters.cpp
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
 * Implements Delaunayl related parameter classes.
 *   DelaL, DelaOrbAngMom, DelaOrbZAngMom, DelaMA, DelaAOP, DelaRAAN, DelaState
 */
//------------------------------------------------------------------------------

#include "DelaunayParameters.hpp"
#include "ColorTypes.hpp"

//#define DEBUG_CARTESIAN_PARAM 1


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS

Delal::Delal(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Delaunayl", obj, "Delaunay MA", "deg", GmatParam::COORD_SYS, DEL_DELA_SL, true)
{ 
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}

Delal::Delal(const Delal &copy)
   : OrbitReal(copy)
{
}

Delal& Delal::operator=(const Delal&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

Delal::~Delal()
{
}

bool Delal::Evaluate()
{
   mRealValue = OrbitData::GetDelaReal(DEL_DELA_SL);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* Delal::Clone(void) const
{
   return new Delal(*this);
}



Delag::Delag(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Delaunayg", obj, "Delaunay AOP", "deg", GmatParam::COORD_SYS, DEL_DELA_SG, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

Delag::Delag(const Delag&copy)
   : OrbitReal(copy)
{
}

Delag& Delag::operator=(const Delag&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

Delag::~Delag()
{
}

bool Delag::Evaluate()
{
   mRealValue = OrbitData::GetDelaReal(DEL_DELA_SG);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* Delag::Clone(void) const
{
   return new Delag(*this);
}



Delah::Delah(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Delaunayh", obj, "Delaunay RAAN", "deg", GmatParam::COORD_SYS, DEL_DELA_SH, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

Delah::Delah(const Delah &copy)
   : OrbitReal(copy)
{
}

Delah& Delah::operator=(const Delah&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

Delah::~Delah()
{
}

bool Delah::Evaluate()
{
   mRealValue = OrbitData::GetDelaReal(DEL_DELA_SH);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* Delah::Clone(void) const
{
   return new Delah(*this);
}



DelaL::DelaL(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "DelaunayL", obj, "Delaunay L", "km^2/sec", GmatParam::COORD_SYS, DEL_DELA_L, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW32;
   #endif
}

DelaL::DelaL(const DelaL &copy)
   : OrbitReal(copy)
{
}

DelaL& DelaL::operator=(const DelaL&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

DelaL::~DelaL()
{
}

bool DelaL::Evaluate()
{
   mRealValue = OrbitData::GetDelaReal(DEL_DELA_L);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* DelaL::Clone(void) const
{
   return new DelaL(*this);
}



DelaG::DelaG(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "DelaunayG", obj, "Orb Ang Mom", "km^2/sec", GmatParam::COORD_SYS, DEL_DELA_G, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

DelaG::DelaG(const DelaG &copy)
   : OrbitReal(copy)
{
}

DelaG& DelaG::operator=(const DelaG&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

DelaG::~DelaG()
{
}

bool DelaG::Evaluate()
{
   mRealValue = OrbitData::GetDelaReal(DEL_DELA_G);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* DelaG::Clone(void) const
{
   return new DelaG(*this);
}



DelaH::DelaH(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "DelaunayH", obj, "Orb Z Ang Mom", "km^2/sec", GmatParam::COORD_SYS, DEL_DELA_H, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

DelaH::DelaH(const DelaH &copy)
   : OrbitReal(copy)
{
}

DelaH& DelaH::operator=(const DelaH&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

DelaH::~DelaH()
{
}

bool DelaH::Evaluate()
{
   //mRealValue = OrbitData::GetEquinReal("EquinMlong");
   mRealValue = OrbitData::GetDelaReal(DEL_DELA_H);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* DelaH::Clone(void) const
{
   return new DelaH(*this);
}



DelaState::DelaState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "Delaunay", obj, "Delaunay State Vector",
                "?", GmatParam::COORD_SYS)
{
   // Parameter member data
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mIsPlottable = false;
}


DelaState::DelaState(const DelaState &copy)
   : OrbitRvec6(copy)
{
}


DelaState& DelaState::operator=(const DelaState &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}


DelaState::~DelaState()
{
}


bool DelaState::Evaluate()
{
   mRvec6Value = GetDelaState();
   
   #if DEBUG_CARTESIAN_PARAM
   MessageInterface::ShowMessage
      ("EquinState::Evaluate() mRvec6Value =\n%s\n",
       mRvec6Value.ToString().c_str());
   #endif
   
   return mRvec6Value.IsValid(GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
}


GmatBase* DelaState::Clone(void) const
{
   return new DelaState(*this);
}


