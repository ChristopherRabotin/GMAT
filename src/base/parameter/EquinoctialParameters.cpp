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

//------------------------------------------------------------------------------
// EquinSma::EquinSma(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EquinSma::EquinSma(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "EquinSMA", obj, "Equinoctial SMA", "km", GmatParam::COORD_SYS, EQ_SMA, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}

//------------------------------------------------------------------------------
// EquinSma::EquinSma(const EquinSma &copy)
//------------------------------------------------------------------------------
EquinSma::EquinSma(const EquinSma &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// EquinSma& EquinSma::operator=(const EquinSma&right)
//------------------------------------------------------------------------------
EquinSma& EquinSma::operator=(const EquinSma&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// EquinSma::~EquinSma()
//------------------------------------------------------------------------------
EquinSma::~EquinSma()
{
}

//------------------------------------------------------------------------------
// bool EquinSma::Evaluate()
//------------------------------------------------------------------------------
bool EquinSma::Evaluate()
{
   //mRealValue = OrbitData::GetEquinReal("EquinSma");
   //why SMA? Changed to EQ_SMA (LOJ: 2012.02.08)
   //mRealValue = OrbitData::GetEquinReal(SMA);
   mRealValue = OrbitData::GetEquinReal(EQ_SMA);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* EquinSma::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EquinSma::Clone(void) const
{
   return new EquinSma(*this);
}


//------------------------------------------------------------------------------
// EquinEy::EquinEy(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EquinEy::EquinEy(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "EquinoctialH", obj, "Equinoctial Ey", "?", GmatParam::COORD_SYS, EQ_H, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

//------------------------------------------------------------------------------
// EquinEy::EquinEy(const EquinEy&copy)
//------------------------------------------------------------------------------
EquinEy::EquinEy(const EquinEy&copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// EquinEy& EquinEy::operator=(const EquinEy&right)
//------------------------------------------------------------------------------
EquinEy& EquinEy::operator=(const EquinEy&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// EquinEy::~EquinEy()
//------------------------------------------------------------------------------
EquinEy::~EquinEy()
{
}

//------------------------------------------------------------------------------
// bool EquinEy::Evaluate()
//------------------------------------------------------------------------------
bool EquinEy::Evaluate()
{
   mRealValue = OrbitData::GetEquinReal(EQ_H);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* EquinEy::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EquinEy::Clone(void) const
{
   return new EquinEy(*this);
}


//------------------------------------------------------------------------------
// EquinEx::EquinEx(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EquinEx::EquinEx(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "EquinoctialK", obj, "Equinoctial Ex", "?", GmatParam::COORD_SYS, EQ_K, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}


//------------------------------------------------------------------------------
// EquinEx::EquinEx(const EquinEx &copy)
//------------------------------------------------------------------------------
EquinEx::EquinEx(const EquinEx &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// EquinEx& EquinEx::operator=(const EquinEx&right)
//------------------------------------------------------------------------------
EquinEx& EquinEx::operator=(const EquinEx&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// EquinEx::~EquinEx()
//------------------------------------------------------------------------------
EquinEx::~EquinEx()
{
}

//------------------------------------------------------------------------------
// bool EquinEx::Evaluate()
//------------------------------------------------------------------------------
bool EquinEx::Evaluate()
{
   mRealValue = OrbitData::GetEquinReal(EQ_K);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* EquinEx::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EquinEx::Clone(void) const
{
   return new EquinEx(*this);
}



//------------------------------------------------------------------------------
// EquinNy::EquinNy(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EquinNy::EquinNy(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "EquinoctialP", obj, "Equinoctial Ny", "?", GmatParam::COORD_SYS, EQ_P, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW32;
   #endif
}

//------------------------------------------------------------------------------
// EquinNy::EquinNy(const EquinNy &copy)
//------------------------------------------------------------------------------
EquinNy::EquinNy(const EquinNy &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// EquinNy& EquinNy::operator=(const EquinNy&right)
//------------------------------------------------------------------------------
EquinNy& EquinNy::operator=(const EquinNy&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// EquinNy::~EquinNy()
//------------------------------------------------------------------------------
EquinNy::~EquinNy()
{
}

//------------------------------------------------------------------------------
// bool EquinNy::Evaluate()
//------------------------------------------------------------------------------
bool EquinNy::Evaluate()
{
   mRealValue = OrbitData::GetEquinReal(EQ_P);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* EquinNy::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EquinNy::Clone(void) const
{
   return new EquinNy(*this);
}


//------------------------------------------------------------------------------
// EquinNx::EquinNx(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EquinNx::EquinNx(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "EquinoctialQ", obj, "Equinoctial Nx", "?", GmatParam::COORD_SYS, EQ_Q, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

//------------------------------------------------------------------------------
// EquinNx::EquinNx(const EquinNx &copy)
//------------------------------------------------------------------------------
EquinNx::EquinNx(const EquinNx &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// EquinNx& EquinNx::operator=(const EquinNx&right)
//------------------------------------------------------------------------------
EquinNx& EquinNx::operator=(const EquinNx&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// EquinNx::~EquinNx()
//------------------------------------------------------------------------------
EquinNx::~EquinNx()
{
}

//------------------------------------------------------------------------------
// bool EquinNx::Evaluate()
//------------------------------------------------------------------------------
bool EquinNx::Evaluate()
{
   mRealValue = OrbitData::GetEquinReal(EQ_Q);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* EquinNx::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EquinNx::Clone(void) const
{
   return new EquinNx(*this);
}



//------------------------------------------------------------------------------
// EquinMlong::EquinMlong(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EquinMlong::EquinMlong(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MLONG", obj, "Equinoctial Mlong", "?", GmatParam::COORD_SYS, EQ_MLONG, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

//------------------------------------------------------------------------------
// EquinMlong::EquinMlong(const EquinMlong &copy)
//------------------------------------------------------------------------------
EquinMlong::EquinMlong(const EquinMlong &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// EquinMlong& EquinMlong::operator=(const EquinMlong&right)
//------------------------------------------------------------------------------
EquinMlong& EquinMlong::operator=(const EquinMlong&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// EquinMlong::~EquinMlong()
//------------------------------------------------------------------------------
EquinMlong::~EquinMlong()
{
}

//------------------------------------------------------------------------------
// bool EquinMlong::Evaluate()
//------------------------------------------------------------------------------
bool EquinMlong::Evaluate()
{
   mRealValue = OrbitData::GetEquinReal(EQ_MLONG);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* EquinMlong::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EquinMlong::Clone(void) const
{
   return new EquinMlong(*this);
}



//------------------------------------------------------------------------------
// EquinState::EquinState(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EquinState::EquinState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "Equinoctial", obj, "Equinoctial State Vector",
                "", GmatParam::COORD_SYS)
{
   // Parameter member data
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mIsPlottable = false;
}

//------------------------------------------------------------------------------
// EquinState::EquinState(const EquinState &copy)
//------------------------------------------------------------------------------
EquinState::EquinState(const EquinState &copy)
   : OrbitRvec6(copy)
{
}

//------------------------------------------------------------------------------
// EquinState& EquinState::operator=(const EquinState &right)
//------------------------------------------------------------------------------
EquinState& EquinState::operator=(const EquinState &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// EquinState::~EquinState()
//------------------------------------------------------------------------------
EquinState::~EquinState()
{
}

//------------------------------------------------------------------------------
// bool EquinState::Evaluate()
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// GmatBase* EquinState::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EquinState::Clone(void) const
{
   return new EquinState(*this);
}

