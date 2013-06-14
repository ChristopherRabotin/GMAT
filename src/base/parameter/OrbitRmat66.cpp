//$Id$
//------------------------------------------------------------------------------
//                                  OrbitRmat66
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. 
//
// Author: Linda Jun (GSFC/NASA)
// Created: 2009.03.30
//
/**
 * Declares OrbitRmat66 class.
 */
//------------------------------------------------------------------------------

#include "OrbitRmat66.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ORBITRMAT66_REFOBJ
//#define DEBUG_ORBITRMAT66_SET

//------------------------------------------------------------------------------
// OrbitRmat66(const std::string &name, const std::string &typeStr, 
//             GmatBase *obj, const std::string &desc, const std::string &unit,
//             GmatParam::DepObject depObj, bool isSettable)
//------------------------------------------------------------------------------
OrbitRmat66::OrbitRmat66(const std::string &name, const std::string &typeStr, 
                         GmatBase *obj, const std::string &desc,
                         const std::string &unit, GmatParam::DepObject depObj,
                         bool isSettable, Gmat::ObjectType paramOwnerType)
   : Rmat66Var(name, typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
               depObj, paramOwnerType, isSettable),
     OrbitData(name, paramOwnerType)
{
   mNeedCoordSystem = true;
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// OrbitRmat66(const OrbitRmat66 &copy)
//------------------------------------------------------------------------------
OrbitRmat66::OrbitRmat66(const OrbitRmat66 &copy)
   : Rmat66Var(copy), OrbitData(copy)
{
}


//------------------------------------------------------------------------------
// OrbitRmat66& operator=(const OrbitRmat66 &right)
//------------------------------------------------------------------------------
OrbitRmat66& OrbitRmat66::operator=(const OrbitRmat66 &right)
{
   if (this != &right)
   {
      Rmat66Var::operator=(right);
      OrbitData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitRmat66()
//------------------------------------------------------------------------------
OrbitRmat66::~OrbitRmat66()
{
}


//------------------------------------------------------------------------------
// const Rmatrix& EvaluateRmatrix()
//------------------------------------------------------------------------------
const Rmatrix& OrbitRmat66::EvaluateRmatrix()
{
   Evaluate();
   return mRmat66Value;
}


//------------------------------------------------------------------------------
// virtual CoordinateSystem* GetInternalCoordSystem()
//------------------------------------------------------------------------------
CoordinateSystem* OrbitRmat66::GetInternalCoordSystem()
{
   return OrbitData::GetInternalCoordSys();
}


//------------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets SolarSystem pointer.
 */
//------------------------------------------------------------------------------
void OrbitRmat66::SetSolarSystem(SolarSystem *ss)
{
   #ifdef DEBUG_ORBITRMAT66_SET
   MessageInterface::ShowMessage
      ("OrbitRmat66::SetSolarSystem() '%s' entered, ss=<%p>'%s'\n",
       GetName().c_str(), ss, ss->GetTypeName().c_str());
   #endif
   
   if (OrbitData::GetRefObject(Gmat::SOLAR_SYSTEM, ss->GetName()) == NULL)
      OrbitData::AddRefObject(ss->GetType(), ss->GetName(), ss);
   else
      OrbitData::SetRefObject(ss, Gmat::SOLAR_SYSTEM, ss->GetName());
   
}


//------------------------------------------------------------------------------
// virtual void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets internal CoordinateSystem pointer. Assumes parameter data is in
 * this internal CoordinateSystem.
 */
//------------------------------------------------------------------------------
void OrbitRmat66::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #ifdef DEBUG_ORBITRMAT66_SET
   MessageInterface::ShowMessage
      ("OrbitRmat33::SetInternalCoordSystem() '%s' entered, cs=<%p>'%s'\n",
       GetName().c_str(), cs, cs->GetTypeName().c_str());
   #endif
   
   OrbitData::SetInternalCoordSys(cs);
}


//------------------------------------------------------------------------------
// Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer OrbitRmat66::GetNumRefObjects() const
{
   return OrbitData::GetNumRefObjects();
}


//------------------------------------------------------------------------------
// bool addRefObject(GmatBase *obj, bool replaceName)
//------------------------------------------------------------------------------
bool OrbitRmat66::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      #ifdef DEBUG_ORBITRMAT66_REFOBJ
      MessageInterface::ShowMessage
         ("OrbitRmat66::AddRefObject() obj->GetName()=%s, type=%d\n",
          obj->GetName().c_str(), obj->GetType());
      #endif
      
      return OrbitData::AddRefObject(obj->GetType(), obj->GetName(), obj,
                                     replaceName);
      
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
bool OrbitRmat66::Validate()
{
   return ValidateRefObjects(this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool OrbitRmat66::Initialize()
{
   try
   {
      InitializeRefObjects();
   }
   catch(BaseException &e)
   {
      throw GmatBaseException
         ("OrbitRmat66::Initialize() Fail to initialize Parameter:" +
          this->GetTypeName() + "\n" + e.GetFullMessage());
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type, const std::string &oldName,
//                      const std::string &newName)
//------------------------------------------------------------------------------
bool OrbitRmat66::RenameRefObject(const Gmat::ObjectType type,
                                  const std::string &oldName,
                                  const std::string &newName)
{
   return OrbitData::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
std::string OrbitRmat66::GetRefObjectName(const Gmat::ObjectType type) const
{
   std::string objName = OrbitData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("OrbitRmat66::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return objName;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& OrbitRmat66::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   return OrbitData::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
bool OrbitRmat66::SetRefObjectName(const Gmat::ObjectType type,
                                         const std::string &name)
{
   bool ret = OrbitData::SetRefObjectName(type, name);
   
   if (!ret)
      MessageInterface::ShowMessage
         ("*** Warning *** OrbitRmat66::SetRefObjectName() RefObjType:%s is not valid "
          "for ParameterName:%s\n", GmatBase::GetObjectTypeString(type).c_str(),
          this->GetName().c_str());
   
   return ret;
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* OrbitRmat66::GetRefObject(const Gmat::ObjectType type,
                                    const std::string &name)
{
   GmatBase *obj = OrbitData::GetRefObject(type, name);
   
   if (obj == NULL)
   {
      throw ParameterException
         ("OrbitRmat66::GetRefObject() Cannot find ref. object of type:" +
          GmatBase::GetObjectTypeString(type) + ", name:" + name + " in " +
          this->GetName());
   }
   
   return obj;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name)
//------------------------------------------------------------------------------
bool OrbitRmat66::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                               const std::string &name)
{
   #ifdef DEBUG_ORBITRMAT66_REFOBJ
   MessageInterface::ShowMessage
      ("OrbitRmat66::SetRefObject() setting type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif
   
   return OrbitData::SetRefObject(obj, type, name);
}

