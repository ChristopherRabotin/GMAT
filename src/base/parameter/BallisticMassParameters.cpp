//$Id$
//------------------------------------------------------------------------------
//                            File: BallisticMassParameters
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
// Created: 2009.03.20
//
/**
 * Implements BallisticMass related parameter classes.
 *    DryMass, DragCoeff, ReflectCoeff, DragArea, SRPArea, TotalMass
 */
//------------------------------------------------------------------------------

#include "BallisticMassParameters.hpp"
#include "ColorTypes.hpp"


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS

//==============================================================================
//                              DryMass
//==============================================================================
/**
 * Implements DryMass class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DryMass(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DryMass::DryMass(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DryMass", obj, "Dry Mass", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// DryMass(const DryMass &copy)
//------------------------------------------------------------------------------
DryMass::DryMass(const DryMass &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DryMass& operator=(const DryMass &right)
//------------------------------------------------------------------------------
DryMass& DryMass::operator=(const DryMass &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DryMass()
//------------------------------------------------------------------------------
DryMass::~DryMass()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DryMass::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRY_MASS);
   
    
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DryMass::Clone() const
//------------------------------------------------------------------------------
GmatBase* DryMass::Clone() const
{
   return new DryMass(*this);
}


//==============================================================================
//                              DragCoeff
//==============================================================================
/**
 * Implements DragCoeff class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DragCoeff(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DragCoeff::DragCoeff(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "Cd", obj, "Drag Coefficient", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW32;
   #endif
}


//------------------------------------------------------------------------------
// DragCoeff(const DragCoeff &copy)
//------------------------------------------------------------------------------
DragCoeff::DragCoeff(const DragCoeff &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DragCoeff& operator=(const DragCoeff &right)
//------------------------------------------------------------------------------
DragCoeff& DragCoeff::operator=(const DragCoeff &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DragCoeff()
//------------------------------------------------------------------------------
DragCoeff::~DragCoeff()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DragCoeff::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRAG_COEFF);
    
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DragCoeff::Clone() const
//------------------------------------------------------------------------------
GmatBase* DragCoeff::Clone() const
{
   return new DragCoeff(*this);
}


//==============================================================================
//                              ReflectCoeff
//==============================================================================
/**
 * Implements ReflectCoeff class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ReflectCoeff(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ReflectCoeff::ReflectCoeff(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "Cr", obj, "Reflectivity Coefficient", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}


//------------------------------------------------------------------------------
// ReflectCoeff(const ReflectCoeff &copy)
//------------------------------------------------------------------------------
ReflectCoeff::ReflectCoeff(const ReflectCoeff &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// ReflectCoeff& operator=(const ReflectCoeff &right)
//------------------------------------------------------------------------------
ReflectCoeff& ReflectCoeff::operator=(const ReflectCoeff &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ReflectCoeff()
//------------------------------------------------------------------------------
ReflectCoeff::~ReflectCoeff()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ReflectCoeff::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(REFLECT_COEFF);
    
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* ReflectCoeff::Clone() const
//------------------------------------------------------------------------------
GmatBase* ReflectCoeff::Clone() const
{
   return new ReflectCoeff(*this);
}


//==============================================================================
//                              DragArea
//==============================================================================
/**
 * Implements DragArea class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DragArea(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DragArea::DragArea(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DragArea", obj, "Drag Area", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}


//------------------------------------------------------------------------------
// DragArea(const DragArea &copy)
//------------------------------------------------------------------------------
DragArea::DragArea(const DragArea &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DragArea& operator=(const DragArea &right)
//------------------------------------------------------------------------------
DragArea& DragArea::operator=(const DragArea &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DragArea()
//------------------------------------------------------------------------------
DragArea::~DragArea()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DragArea::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRAG_AREA);
    
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DragArea::Clone() const
//------------------------------------------------------------------------------
GmatBase* DragArea::Clone() const
{
   return new DragArea(*this);
}


//==============================================================================
//                              SRPArea
//==============================================================================
/**
 * Implements SRPArea class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SRPArea(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SRPArea::SRPArea(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SRPArea", obj, "SRP Area", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::ORANGE32;
   #endif
}


//------------------------------------------------------------------------------
// SRPArea(const SRPArea &copy)
//------------------------------------------------------------------------------
SRPArea::SRPArea(const SRPArea &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SRPArea& operator=(const SRPArea &right)
//------------------------------------------------------------------------------
SRPArea& SRPArea::operator=(const SRPArea &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);
   
   return *this;
}


//------------------------------------------------------------------------------
// ~SRPArea()
//------------------------------------------------------------------------------
SRPArea::~SRPArea()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SRPArea::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SRP_AREA);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SRPArea::Clone() const
//------------------------------------------------------------------------------
GmatBase* SRPArea::Clone() const
{
   return new SRPArea(*this);
}


//==============================================================================
//                              TotalMass
//==============================================================================
/**
 * Implements TotalMass class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TotalMass(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
TotalMass::TotalMass(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "TotalMass", obj, "Total Mass", "", false)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
}


//------------------------------------------------------------------------------
// TotalMass(const TotalMass &copy)
//------------------------------------------------------------------------------
TotalMass::TotalMass(const TotalMass &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// TotalMass& operator=(const TotalMass &right)
//------------------------------------------------------------------------------
TotalMass& TotalMass::operator=(const TotalMass &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);
   
   return *this;
}


//------------------------------------------------------------------------------
// ~TotalMass()
//------------------------------------------------------------------------------
TotalMass::~TotalMass()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool TotalMass::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(TOTAL_MASS);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* TotalMass::Clone() const
//------------------------------------------------------------------------------
GmatBase* TotalMass::Clone() const
{
   return new TotalMass(*this);
}

// made changes by TUAN NGUYEN
//==============================================================================
//                              SpadDragScaleFactor
//==============================================================================
/**
* Implements SpadDragScaleFactor class.
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SpadDragScaleFactor(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SpadDragScaleFactor::SpadDragScaleFactor(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SPADDragScaleFactor", obj, "SPAD Drag Scale Factor", "")
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::BLUE32;
#endif
}


//------------------------------------------------------------------------------
// SpadDragScaleFactor(const SpadDragScaleFactor &copy)
//------------------------------------------------------------------------------
SpadDragScaleFactor::SpadDragScaleFactor(const SpadDragScaleFactor &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SpadDragScaleFactor& operator=(const SpadDragScaleFactor &right)
//------------------------------------------------------------------------------
SpadDragScaleFactor& SpadDragScaleFactor::operator=(const SpadDragScaleFactor &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SpadDragScaleFactor()
//------------------------------------------------------------------------------
SpadDragScaleFactor::~SpadDragScaleFactor()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SpadDragScaleFactor::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SPAD_DRAG_COEFF);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SpadDragScaleFactor::Clone() const
//------------------------------------------------------------------------------
GmatBase* SpadDragScaleFactor::Clone() const
{
   return new SpadDragScaleFactor(*this);
}


// made changes by TUAN NGUYEN
//==============================================================================
//                              SpadSRPScaleFactor
//==============================================================================
/**
* Implements SpadSRPScaleFactor class.
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SpadSRPScaleFactor(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SpadSRPScaleFactor::SpadSRPScaleFactor(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SPADSRPScaleFactor", obj, "SPAD SRP Scale Factor", "")
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::BLUE32;
#endif
}


//------------------------------------------------------------------------------
// SpadSRPScaleFactor(const SpadSRPScaleFactor &copy)
//------------------------------------------------------------------------------
SpadSRPScaleFactor::SpadSRPScaleFactor(const SpadSRPScaleFactor &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SpadSRPScaleFactor& operator=(const SpadSRPScaleFactor &right)
//------------------------------------------------------------------------------
SpadSRPScaleFactor& SpadSRPScaleFactor::operator=(const SpadSRPScaleFactor &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SpadSRPScaleFactor()
//------------------------------------------------------------------------------
SpadSRPScaleFactor::~SpadSRPScaleFactor()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SpadSRPScaleFactor::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SPAD_REFLECT_COEFF);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SpadSRPScaleFactor::Clone() const
//------------------------------------------------------------------------------
GmatBase* SpadSRPScaleFactor::Clone() const
{
   return new SpadSRPScaleFactor(*this);
}


