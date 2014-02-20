//$Id$
//------------------------------------------------------------------------------
//                                  AttitudeRvector
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
// Created: 2006/6/26
//
/**
 * Declares Attitude real data class.
 */
//------------------------------------------------------------------------------
#include "AttitudeRvector.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// AttitudeRvector(const std::string &name, const std::string &typeStr, ...)
//------------------------------------------------------------------------------
AttitudeRvector::AttitudeRvector(const std::string &name, const std::string &typeStr, 
                                 GmatBase *obj, const std::string &desc,
                                 const std::string &unit, bool isSettable,
                                 Integer size)
   : RvectorVar(name, typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
                GmatParam::OWNED_OBJ, Gmat::SPACECRAFT, false, isSettable,
                Gmat::ATTITUDE, size)
{
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// AttitudeRvector(const AttitudeRvector &copy)
//------------------------------------------------------------------------------
AttitudeRvector::AttitudeRvector(const AttitudeRvector &copy)
   : RvectorVar(copy), AttitudeData(copy)
{
}


//------------------------------------------------------------------------------
// AttitudeRvector& operator=(const AttitudeRvector &right)
//------------------------------------------------------------------------------
AttitudeRvector& AttitudeRvector::operator=(const AttitudeRvector &right)
{
   if (this != &right)
   {
      RvectorVar::operator=(right);
      AttitudeData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~AttitudeRvector()
//------------------------------------------------------------------------------
AttitudeRvector::~AttitudeRvector()
{
}


//------------------------------------------------------------------------------
// const Rvector& EvaluateRvector()
//------------------------------------------------------------------------------
const Rvector& AttitudeRvector::EvaluateRvector()
{
   Evaluate();
   return mRvectorValue;
}


//------------------------------------------------------------------------------
// Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer AttitudeRvector::GetNumRefObjects() const
{
   return AttitudeData::GetNumRefObjects();
}


//------------------------------------------------------------------------------
// bool addRefObject(GmatBase *obj, bool replaceName)
//------------------------------------------------------------------------------
bool AttitudeRvector::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      #if DEBUG_ATTITUDE_RVECTOR
      MessageInterface::ShowMessage
         ("AttitudeRvector::AddRefObject() obj->GetName()=%s, type=%d\n",
          obj->GetName().c_str(), obj->GetType());
      #endif
      
      return AttitudeData::AddRefObject(obj->GetType(), obj->GetName(), obj,
                                        replaceName);
      
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
bool AttitudeRvector::Validate()
{
   return ValidateRefObjects(this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool AttitudeRvector::Initialize()
{
   try
   {
      InitializeRefObjects();
   }
   catch(BaseException &e)
   {
      throw GmatBaseException
         ("AttitudeRvector::Initialize() Fail to initialize Parameter:" +
          this->GetTypeName() + "\n" + e.GetFullMessage());
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type, const std::string &oldName,
//                      const std::string &newName)
//------------------------------------------------------------------------------
bool AttitudeRvector::RenameRefObject(const Gmat::ObjectType type,
                                   const std::string &oldName,
                                   const std::string &newName)
{
   return AttitudeData::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
std::string AttitudeRvector::GetRefObjectName(const Gmat::ObjectType type) const
{
   std::string objName = AttitudeData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("AttitudeRvector::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return objName;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& AttitudeRvector::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   return AttitudeData::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
bool AttitudeRvector::SetRefObjectName(const Gmat::ObjectType type,
                                       const std::string &name)
{
   bool ret = AttitudeData::SetRefObjectName(type, name);
   
   if (!ret)
      MessageInterface::ShowMessage
         ("*** Warning *** AttitudeRvector::SetRefObjectName() RefObjType:%s is not valid "
          "for ParameterName:%s\n", GmatBase::GetObjectTypeString(type).c_str(),
          this->GetName().c_str());
   
   return ret;
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* AttitudeRvector::GetRefObject(const Gmat::ObjectType type,
                                        const std::string &name)
{
   GmatBase *obj = AttitudeData::GetRefObject(type, name);
   
   if (obj == NULL)
   {
      throw ParameterException
         ("AttitudeRvector::GetRefObject() Cannot find ref. object of type:" +
          GmatBase::GetObjectTypeString(type) + ", name:" + name + " in " +
          this->GetName());
   }
   
   return obj;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name)
//------------------------------------------------------------------------------
bool AttitudeRvector::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                   const std::string &name)
{
   #if DEBUG_ATTITUDE_RVECTOR
   MessageInterface::ShowMessage
      ("AttitudeRvector::SetRefObject() setting type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif
   
   return AttitudeData::SetRefObject(obj, type, name);
}

