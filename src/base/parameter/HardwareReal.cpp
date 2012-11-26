//$Id$
//------------------------------------------------------------------------------
//                                  HardwareReal
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun
// Created: 2009.03.20
//
/**
 * Declares BallisticMass real data class.
 */
//------------------------------------------------------------------------------

#include "HardwareReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// HardwareReal(const std::string &name, const std::string &typeStr, 
//              GmatBase *obj, const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
HardwareReal::HardwareReal(const std::string &name, const std::string &typeStr, 
                           Gmat::ObjectType ownerType, Gmat::ObjectType ownedObjType,
                           GmatBase *obj, const std::string &desc, const std::string &unit)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
             GmatParam::ATTACHED_OBJ, ownerType, false, true, true, true, ownedObjType),
     SpacecraftData(name)
{
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// HardwareReal(const HardwareReal &copy)
//------------------------------------------------------------------------------
HardwareReal::HardwareReal(const HardwareReal &copy)
   : RealVar(copy), SpacecraftData(copy)
{
}


//------------------------------------------------------------------------------
// HardwareReal& operator=(const HardwareReal &right)
//------------------------------------------------------------------------------
HardwareReal& HardwareReal::operator=(const HardwareReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      SpacecraftData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~HardwareReal()
//------------------------------------------------------------------------------
HardwareReal::~HardwareReal()
{
}


//------------------------------------------------------------------------------
// Real EvaluateReal()
//------------------------------------------------------------------------------
Real HardwareReal::EvaluateReal()
{
   Evaluate();
   return mRealValue;
}


//------------------------------------------------------------------------------
// Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer HardwareReal::GetNumRefObjects() const
{
   return SpacecraftData::GetNumRefObjects();
}


//------------------------------------------------------------------------------
// bool addRefObject(GmatBase *obj, bool replaceName)
//------------------------------------------------------------------------------
bool HardwareReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      #if DEBUG_ATTITUDEREAL
      MessageInterface::ShowMessage
         ("HardwareReal::AddRefObject() obj->GetName()=%s, type=%d\n",
          obj->GetName().c_str(), obj->GetType());
      #endif
      
      return SpacecraftData::AddRefObject(obj->GetType(), obj->GetName(), obj,
                                        replaceName);
      
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
bool HardwareReal::Validate()
{
   return ValidateRefObjects(this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool HardwareReal::Initialize()
{
   try
   {
      InitializeRefObjects();
   }
   catch(BaseException &e)
   {
      throw GmatBaseException
         ("HardwareReal::Initialize() Fail to initialize Parameter:" +
          this->GetTypeName() + "\n" + e.GetFullMessage());
   }
   
   return true;
}


//---------------------------------------------------------------------------
//  bool SetName(std::string &newName, const std;:string &oldName = "")
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool HardwareReal::SetName(const std::string &newName, const std::string &oldName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("HardwareReal::SetName() entered, newName='%s', oldName='%s'\n", newName.c_str(),
       oldName.c_str());
   #endif
   
   Parameter::SetName(newName, oldName);
   RefData::SetName(newName, oldName);
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("HardwareReal::SetName() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type, const std::string &oldName,
//                      const std::string &newName)
//------------------------------------------------------------------------------
bool HardwareReal::RenameRefObject(const Gmat::ObjectType type,
                                   const std::string &oldName,
                                   const std::string &newName)
{
   return SpacecraftData::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
std::string HardwareReal::GetRefObjectName(const Gmat::ObjectType type) const
{
   std::string objName = SpacecraftData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("HardwareReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return objName;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& HardwareReal::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   return SpacecraftData::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
bool HardwareReal::SetRefObjectName(const Gmat::ObjectType type,
                                    const std::string &name)
{
   bool ret = SpacecraftData::SetRefObjectName(type, name);
   
   if (!ret)
      MessageInterface::ShowMessage
         ("*** Warning *** HardwareReal::SetRefObjectName() RefObjType:%s is not valid "
          "for ParameterName:%s\n", GmatBase::GetObjectTypeString(type).c_str(),
          this->GetName().c_str());
   
   return ret;
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* HardwareReal::GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name)
{
   GmatBase *obj = SpacecraftData::GetRefObject(type, name);
   
   if (obj == NULL)
   {
      throw ParameterException
         ("HardwareReal::GetRefObject() Cannot find ref. object of type:" +
          GmatBase::GetObjectTypeString(type) + ", name:" + name + " in " +
          this->GetName());
   }
   
   return obj;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name)
//------------------------------------------------------------------------------
bool HardwareReal::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                const std::string &name)
{
   #if DEBUG_ATTITUDEREAL
   MessageInterface::ShowMessage
      ("HardwareReal::SetRefObject() setting type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif
   
   return SpacecraftData::SetRefObject(obj, type, name);
}

