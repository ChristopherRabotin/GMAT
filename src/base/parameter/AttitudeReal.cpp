#include "AttitudeReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

AttitudeReal::AttitudeReal(const std::string &name, const std::string &typeStr, 
                   GmatBase *obj, const std::string &desc,
                   const std::string &unit, GmatParam::DepObject depObj)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
             depObj, Gmat::BURN, false)
{
   AddRefObject(obj);
}

AttitudeReal::AttitudeReal(const AttitudeReal &copy)
   : RealVar(copy), AttitudeData(copy)
{
}

AttitudeReal& AttitudeReal::operator=(const AttitudeReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      AttitudeData::operator=(right);
   }
   
   return *this;
}

AttitudeReal::~AttitudeReal()
{
}

Real AttitudeReal::EvaluateReal()
{
   Evaluate();
   return mRealValue;
}

Integer AttitudeReal::GetNumRefObjects() const
{
   return AttitudeData::GetNumRefObjects();
}

bool AttitudeReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      //loj: 4/12/05 if obj->GetType() is CELESTIAL_BODY, set as SPACE_POINT
      // since CelestialBody subtypes are not set as SPACE_POINT
      ///@todo Use IsOfType(Gmat::SPACE_POINT) when GmatBase provides this method.

      #if DEBUG_ATTITUDEREAL
      MessageInterface::ShowMessage
         ("AttitudeReal::AddRefObject() obj->GetName()=%s, type=%d\n",
          obj->GetName().c_str(), obj->GetType());
      #endif
      
      //if (obj->GetType() == Gmat::CELESTIAL_BODY)
      if (obj->IsOfType(Gmat::CELESTIAL_BODY))
         return AttitudeData::AddRefObject(Gmat::SPACE_POINT, obj->GetName(), obj,
                                        replaceName);
      else
         return AttitudeData::AddRefObject(obj->GetType(), obj->GetName(), obj,
                                        replaceName);
      
   }
   
   return false;
}

bool AttitudeReal::Validate()
{
   return ValidateRefObjects(this);
}

bool AttitudeReal::Initialize()
{
   try
   {
      InitializeRefObjects();
   }
   catch(BaseException &e)
   {
      throw GmatBaseException
         ("AttitudeReal::Initialize() Fail to initialize Parameter:" +
          this->GetTypeName() + "\n" + e.GetMessage());
   }
   
   return true;
}

bool AttitudeReal::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   return AttitudeData::RenameRefObject(type, oldName, newName);
}

std::string AttitudeReal::GetRefObjectName(const Gmat::ObjectType type) const
{
   std::string objName = AttitudeData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("AttitudeReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return objName;
}

const StringArray& AttitudeReal::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   return AttitudeData::GetRefObjectNameArray(type);
}

bool AttitudeReal::SetRefObjectName(const Gmat::ObjectType type,
                                const std::string &name)
{
   bool ret = AttitudeData::SetRefObjectName(type, name);
   
   if (!ret)
      MessageInterface::ShowMessage
         ("*** Warning *** AttitudeReal::SetRefObjectName() RefObjType:%s is not valid "
          "for ParameterName:%s\n", GmatBase::GetObjectTypeString(type).c_str(),
          this->GetName().c_str());
   
   return ret;
}

GmatBase* AttitudeReal::GetRefObject(const Gmat::ObjectType type,
                                 const std::string &name)
{
   GmatBase *obj = AttitudeData::GetRefObject(type, name);
   
   if (obj == NULL)
   {
      throw ParameterException
         ("AttitudeReal::GetRefObject() Cannot find ref. object of type:" +
          GmatBase::GetObjectTypeString(type) + ", name:" + name + " in " +
          this->GetName());
   }
   
   return obj;
}

bool AttitudeReal::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                            const std::string &name)
{
   #if DEBUG_ATTITUDEREAL
   MessageInterface::ShowMessage
      ("AttitudeReal::SetRefObject() setting type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif
   
   return AttitudeData::SetRefObject(obj, type, name);
}

