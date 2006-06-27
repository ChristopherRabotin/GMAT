//$Header$
//------------------------------------------------------------------------------
//                            File: EquinoctialParameters.cpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Daniel Hunter
// Created: 2006/6/27
//
/**
 * Implements Equinoctial related parameter classes.
 *   EquiSma, EquiEy, EquiEx, EquiNy, EquiNx, EquiMlong, EquiState
 */
//------------------------------------------------------------------------------

#include "EquinoctialParameters.hpp"
#include "ColorTypes.hpp"

//#define DEBUG_CARTESIAN_PARAM 1

EquiSma::EquiSma(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Sma", obj, "Equinoctial Sma", "?", GmatParam::COORD_SYS, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mColor = GmatColor::RED32;
}

EquiSma::EquiSma(const EquiSma &copy)
   : OrbitReal(copy)
{
}

EquiSma& EquiSma::operator=(const EquiSma&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

EquiSma::~EquiSma()
{
}

bool EquiSma::Evaluate()
{
   //mRealValue = OrbitData::GetEquiReal("EquiSma");
   mRealValue = OrbitData::GetEquiReal(SMA);
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* EquiSma::Clone(void) const
{
   return new EquiSma(*this);
}



EquiEy::EquiEy(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Ey", obj, "Equinoctial Ey", "?", GmatParam::COORD_SYS, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mColor = GmatColor::GREEN32;
}

EquiEy::EquiEy(const EquiEy&copy)
   : OrbitReal(copy)
{
}

EquiEy& EquiEy::operator=(const EquiEy&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

EquiEy::~EquiEy()
{
}

bool EquiEy::Evaluate()
{
   //mRealValue = OrbitData::GetEquiReal("EquiEy");
   mRealValue = OrbitData::GetEquiReal(EY);
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* EquiEy::Clone(void) const
{
   return new EquiEy(*this);
}



EquiEx::EquiEx(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Ex", obj, "Equinoctial Ex", "?", GmatParam::COORD_SYS, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mColor = GmatColor::BLUE32;
}

EquiEx::EquiEx(const EquiEx &copy)
   : OrbitReal(copy)
{
}

EquiEx& EquiEx::operator=(const EquiEx&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

EquiEx::~EquiEx()
{
}

bool EquiEx::Evaluate()
{
   //mRealValue = OrbitData::GetEquiReal("EquiEx");
   mRealValue = OrbitData::GetEquiReal(EX);
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* EquiEx::Clone(void) const
{
   return new EquiEx(*this);
}



EquiNy::EquiNy(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Ny", obj, "Equinoctial Ny", "?", GmatParam::COORD_SYS, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mColor = GmatColor::YELLOW32;
}

EquiNy::EquiNy(const EquiNy &copy)
   : OrbitReal(copy)
{
}

EquiNy& EquiNy::operator=(const EquiNy&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

EquiNy::~EquiNy()
{
}

bool EquiNy::Evaluate()
{
   //mRealValue = OrbitData::GetEquiReal("EquiNy");
   mRealValue = OrbitData::GetEquiReal(NY);
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* EquiNy::Clone(void) const
{
   return new EquiNy(*this);
}



EquiNx::EquiNx(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Nx", obj, "Equinoctial Nx", "?", GmatParam::COORD_SYS, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mColor = GmatColor::GREEN32;
}

EquiNx::EquiNx(const EquiNx &copy)
   : OrbitReal(copy)
{
}

EquiNx& EquiNx::operator=(const EquiNx&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

EquiNx::~EquiNx()
{
}

bool EquiNx::Evaluate()
{
   //mRealValue = OrbitData::GetEquiReal("EquiNx");
   mRealValue = OrbitData::GetEquiReal(NX);
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* EquiNx::Clone(void) const
{
   return new EquiSma(*this);
}



EquiMlong::EquiMlong(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Mlong", obj, "Equinoctial Mlong", "?", GmatParam::COORD_SYS, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mColor = GmatColor::BLUE32;
}

EquiMlong::EquiMlong(const EquiMlong &copy)
   : OrbitReal(copy)
{
}

EquiMlong& EquiMlong::operator=(const EquiMlong&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

EquiMlong::~EquiMlong()
{
}

bool EquiMlong::Evaluate()
{
   //mRealValue = OrbitData::GetEquiReal("EquiMlong");
   mRealValue = OrbitData::GetEquiReal(MLONG);
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* EquiMlong::Clone(void) const
{
   return new EquiMlong(*this);
}



EquiState::EquiState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "Equinoctial", obj, "Equinoctial State Vector",
                "Km/s", GmatParam::COORD_SYS)
{
   // Parameter member data
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mIsPlottable = false;
}


EquiState::EquiState(const EquiState &copy)
   : OrbitRvec6(copy)
{
}


EquiState& EquiState::operator=(const EquiState &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}


EquiState::~EquiState()
{
}


bool EquiState::Evaluate()
{
   mRvec6Value = GetEquiState();
   
   #if DEBUG_CARTESIAN_PARAM
   MessageInterface::ShowMessage
      ("EquiState::Evaluate() mRvec6Value =\n%s\n",
       mRvec6Value.ToString().c_str());
   #endif
   
   return mRvec6Value.IsValid(ORBIT_REAL_UNDEFINED);
}


GmatBase* EquiState::Clone(void) const
{
   return new EquiState(*this);
}

