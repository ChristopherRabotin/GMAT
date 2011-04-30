//$Id$
//------------------------------------------------------------------------------
//                            EquinoctialParameters.cpp
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
// Author: Daniel Hunter
// Created: 2006/6/27
//
/**
 * Implements Equinoctial related parameter classes.
 *   EquinSma, EquinEy, EquinEx, EquinNy, EquinNx, EquinMlong, EquinState
 */
//------------------------------------------------------------------------------

#include "EquinoctialParameters.hpp"
#include "ColorTypes.hpp"

//#define DEBUG_CARTESIAN_PARAM 1


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS

EquinSma::EquinSma(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Sma", obj, "Equinoctial Sma", "?", GmatParam::COORD_SYS, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}

EquinSma::EquinSma(const EquinSma &copy)
   : OrbitReal(copy)
{
}

EquinSma& EquinSma::operator=(const EquinSma&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

EquinSma::~EquinSma()
{
}

bool EquinSma::Evaluate()
{
   //mRealValue = OrbitData::GetEquinReal("EquinSma");
   mRealValue = OrbitData::GetEquinReal(SMA);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* EquinSma::Clone(void) const
{
   return new EquinSma(*this);
}



EquinEy::EquinEy(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Ey", obj, "Equinoctial Ey", "?", GmatParam::COORD_SYS, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

EquinEy::EquinEy(const EquinEy&copy)
   : OrbitReal(copy)
{
}

EquinEy& EquinEy::operator=(const EquinEy&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

EquinEy::~EquinEy()
{
}

bool EquinEy::Evaluate()
{
   //mRealValue = OrbitData::GetEquinReal("EquinEy");
   mRealValue = OrbitData::GetEquinReal(EY);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* EquinEy::Clone(void) const
{
   return new EquinEy(*this);
}



EquinEx::EquinEx(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Ex", obj, "Equinoctial Ex", "?", GmatParam::COORD_SYS, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

EquinEx::EquinEx(const EquinEx &copy)
   : OrbitReal(copy)
{
}

EquinEx& EquinEx::operator=(const EquinEx&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

EquinEx::~EquinEx()
{
}

bool EquinEx::Evaluate()
{
   //mRealValue = OrbitData::GetEquinReal("EquinEx");
   mRealValue = OrbitData::GetEquinReal(EX);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* EquinEx::Clone(void) const
{
   return new EquinEx(*this);
}



EquinNy::EquinNy(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Ny", obj, "Equinoctial Ny", "?", GmatParam::COORD_SYS, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW32;
   #endif
}

EquinNy::EquinNy(const EquinNy &copy)
   : OrbitReal(copy)
{
}

EquinNy& EquinNy::operator=(const EquinNy&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

EquinNy::~EquinNy()
{
}

bool EquinNy::Evaluate()
{
   //mRealValue = OrbitData::GetEquinReal("EquinNy");
   mRealValue = OrbitData::GetEquinReal(NY);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* EquinNy::Clone(void) const
{
   return new EquinNy(*this);
}



EquinNx::EquinNx(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Nx", obj, "Equinoctial Nx", "?", GmatParam::COORD_SYS, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

EquinNx::EquinNx(const EquinNx &copy)
   : OrbitReal(copy)
{
}

EquinNx& EquinNx::operator=(const EquinNx&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

EquinNx::~EquinNx()
{
}

bool EquinNx::Evaluate()
{
   //mRealValue = OrbitData::GetEquinReal("EquinNx");
   mRealValue = OrbitData::GetEquinReal(NX);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* EquinNx::Clone(void) const
{
   return new EquinNx(*this);
}



EquinMlong::EquinMlong(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Mlong", obj, "Equinoctial Mlong", "?", GmatParam::COORD_SYS, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

EquinMlong::EquinMlong(const EquinMlong &copy)
   : OrbitReal(copy)
{
}

EquinMlong& EquinMlong::operator=(const EquinMlong&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

EquinMlong::~EquinMlong()
{
}

bool EquinMlong::Evaluate()
{
   //mRealValue = OrbitData::GetEquinReal("EquinMlong");
   mRealValue = OrbitData::GetEquinReal(MLONG);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* EquinMlong::Clone(void) const
{
   return new EquinMlong(*this);
}



EquinState::EquinState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "Equinoctial", obj, "Equinoctial State Vector",
                "Km/s", GmatParam::COORD_SYS)
{
   // Parameter member data
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mIsPlottable = false;
}


EquinState::EquinState(const EquinState &copy)
   : OrbitRvec6(copy)
{
}


EquinState& EquinState::operator=(const EquinState &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}


EquinState::~EquinState()
{
}


bool EquinState::Evaluate()
{
   mRvec6Value = GetEquinState();
   
   #if DEBUG_CARTESIAN_PARAM
   MessageInterface::ShowMessage
      ("EquinState::Evaluate() mRvec6Value =\n%s\n",
       mRvec6Value.ToString().c_str());
   #endif
   
   return mRvec6Value.IsValid(GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
}


GmatBase* EquinState::Clone(void) const
{
   return new EquinState(*this);
}

