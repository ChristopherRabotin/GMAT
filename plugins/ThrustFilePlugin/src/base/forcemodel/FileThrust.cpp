//------------------------------------------------------------------------------
//                           FileThrust
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#include "../../../../ThrustFilePlugin/src/base/forcemodel/FileThrust.hpp"

#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include <sstream>               // for stringstream



FileThrust::FileThrust(const std::string &name) :
   PhysicalModel           (Gmat::PHYSICAL_MODEL, "FileThrust", name),
   satCount                (0),
   cartIndex               (-1),
   fillCartesian           (true),
   mDotIndex               (-1),
   depleteMass             (false)
{
   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   objectTypeNames.push_back("FileThrust");
}

FileThrust::~FileThrust()
{
}

FileThrust::FileThrust(const FileThrust& ft) :
   PhysicalModel           (ft),
   mySpacecraft            (ft.mySpacecraft),
   satCount                (ft.satCount),
   cartIndex               (ft.cartIndex),
   fillCartesian           (ft.fillCartesian),
   mDotIndex               (ft.mDotIndex),
   depleteMass             (ft.depleteMass)
{
}

FileThrust& FileThrust::operator=(const FileThrust& ft)
{
   if (this != &ft)
   {
      PhysicalModel::operator=(ft);

      mySpacecraft = ft.mySpacecraft;
      satCount      = ft.satCount;
      cartIndex     = ft.cartIndex;
      fillCartesian = ft.fillCartesian;
      mDotIndex     = ft.mDotIndex;
      depleteMass   = ft.depleteMass;
   }

   return *this;
}

bool FileThrust::operator ==(const FileThrust& ft) const
{
   bool retval = false;
   return retval;
}

GmatBase* FileThrust::Clone() const
{
   return new FileThrust(*this);
}

void FileThrust::Clear(const Gmat::ObjectType type)
{
}

bool FileThrust::SetRefObjectName(const Gmat::ObjectType type,
      const std::string& name)
{
   bool retval = false;
   return retval;
}

const StringArray& FileThrust::GetRefObjectNameArray(
      const Gmat::ObjectType type)
{
   if (type == Gmat::SPACECRAFT)
      return mySpacecraft;

   return PhysicalModel::GetRefObjectNameArray(type);
}

bool FileThrust::SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
      const std::string& name)
{
   bool retval = false;
   return retval;
}

bool FileThrust::SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
      const std::string& name, const Integer index)
{
   bool retval = false;
   return retval;
}

bool FileThrust::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = false;
   return retval;
}

GmatBase* FileThrust::GetRefObject(const Gmat::ObjectType type,
      const std::string& name)
{
   if (type == Gmat::SPACECRAFT)
   {
      if (name == "")
         if (spacecraft.size() > 0)
            return spacecraft[0];
      for (UnsignedInt i = 0; i < spacecraft.size(); ++i)
         if (spacecraft[i]->GetName() == name)
            return spacecraft[i];
      return NULL;
   }

   return PhysicalModel::GetRefObject(type, name);
}

GmatBase* FileThrust::GetRefObject(const Gmat::ObjectType type,
      const std::string& name, const Integer index)
{
   return PhysicalModel::GetRefObject(type, name, index);
}

bool FileThrust::IsTransient()
{
   return true;
}

bool FileThrust::DepletesMass()
{
   return depleteMass;
}

bool FileThrust::Initialize()
{
   bool retval = false;
   return retval;
}

bool FileThrust::GetDerivatives(Real* state, Real dt, Integer order,
      const Integer id)
{
   bool retval = false;
   return retval;
}

Rvector6 FileThrust::GetDerivativesForSpacecraft(Spacecraft* sc)
{
   Rvector6 dv;
   return dv;
}

bool FileThrust::SupportsDerivative(Gmat::StateElementId id)
{
   bool retval = false;
   return retval;
}

bool FileThrust::SetStart(Gmat::StateElementId id, Integer index,
      Integer quantity, Integer sizeOfType)
{
   bool retval = false;
   return retval;
}
