//$Header$
//------------------------------------------------------------------------------
//                                OrbitReal
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/03/29
//
/**
 * Implements OrbitReal class which provides base class for orbit realated Real
 * Parameters
 */
//------------------------------------------------------------------------------

#include "OrbitReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ORBITREAL 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// OrbitReal(const std::string &name, const std::string &typeStr, 
//           GmatBase *obj, const std::string &desc,
//           const std::string &unit, GmatParam::DepObject depObj,
//           bool isSettable)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <typeStr> type of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NO_DEP)
 * @param <isSettable> true if parameter is settable, false otherwise
 */
//------------------------------------------------------------------------------
OrbitReal::OrbitReal(const std::string &name, const std::string &typeStr, 
                     GmatBase *obj, const std::string &desc,
                     const std::string &unit, GmatParam::DepObject depObj,
                     bool isSettable)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
             depObj, Gmat::SPACECRAFT, false, isSettable)
{
   mNeedCoordSystem = true;
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// OrbitReal(const OrbitReal &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OrbitReal::OrbitReal(const OrbitReal &copy)
   : RealVar(copy), OrbitData(copy)
{
   #if DEBUG_ORBITREAL
   MessageInterface::ShowMessage
      ("===> OrbitReal::OrbitReal() copy constructor called on %s\n",
       instanceName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// OrbitReal& operator=(const OrbitReal &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OrbitReal& OrbitReal::operator=(const OrbitReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      OrbitData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitReal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
OrbitReal::~OrbitReal()
{
   //MessageInterface::ShowMessage("==> OrbitReal::~OrbitReal()\n");
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Real OrbitReal::EvaluateReal()
{
//    try
//    {
      Evaluate();
//    }
//    catch (BaseException &e)
//    {
//       throw ParameterException
//          ("OrbitReal::EvaluateReal() for parameter " +  this->GetTypeName() + ":" +
//           instanceName + "\n" + e.GetDetails());
//    }
   
   return mRealValue;
}


//------------------------------------------------------------------------------
// virtual CoordinateSystem* GetInternalCoordSystem()
//------------------------------------------------------------------------------
CoordinateSystem* OrbitReal::GetInternalCoordSystem()
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
void OrbitReal::SetSolarSystem(SolarSystem *ss)
{
   #if DEBUG_ORBITREAL
      MessageInterface::ShowMessage
         ("OrbitReal::SetSolarSystem() ss=%s to %s\n", ss->GetTypeName().c_str(),
          this->GetName().c_str());
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
void OrbitReal::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #if DEBUG_ORBITREAL
      MessageInterface::ShowMessage
         ("OrbitReal::SetInternalCoordSystem() cs=%s to %s\n", cs->GetTypeName().c_str(),
          this->GetName().c_str());
   #endif
   
   OrbitData::SetInternalCoordSys(cs);
}


//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer OrbitReal::GetNumRefObjects() const
{
   return OrbitData::GetNumRefObjects();
}


//------------------------------------------------------------------------------
// virtual bool AddRefObject(GmatBase *obj, bool replaceName = false)
//------------------------------------------------------------------------------
/**
 * Adds reference object.
 *
 * @param <obj> object pointer
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool OrbitReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      #if DEBUG_ORBITREAL
      MessageInterface::ShowMessage
         ("OrbitReal::AddRefObject() obj->GetName()=%s, type=%d\n",
          obj->GetName().c_str(), obj->GetType());
      #endif
      
      if (obj->IsOfType(Gmat::CELESTIAL_BODY))
         return OrbitData::AddRefObject(Gmat::SPACE_POINT, obj->GetName(), obj,
                                        replaceName);
      else
         return OrbitData::AddRefObject(obj->GetType(), obj->GetName(), obj,
                                        replaceName);
   }
   
   return false;
}


//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
/**
 * Validates reference objects.
 *
 * @return true if all objects are set; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitReal::Validate()
{
   return ValidateRefObjects(this);
}


//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes reference objects.
 *
 * @return true if all objects are set; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitReal::Initialize()
{
   RealVar::Initialize();
   
   try
   {
      #if DEBUG_ORBITREAL
      MessageInterface::ShowMessage
         ("===> OrbitReal::Initialize() calling InitializeRefObjects() on %s\n",
          instanceName.c_str());
      #endif
      
      InitializeRefObjects();
   }
   catch(InvalidDependencyException &e)
   {
      #if DEBUG_ORBITREAL
      MessageInterface::ShowMessage
         ("OrbitReal::Initialize() Fail to initialize Parameter:%s\n",
          this->GetName().c_str());
      #endif
      
      throw ParameterException
         ("WARNING:  Incorrect parameter dependency: " + GetName() + ".\n" +
          this->GetTypeName() + e.GetMessage() + "\n");
   }
   catch(BaseException &e)
   {
      #if DEBUG_ORBITREAL
      MessageInterface::ShowMessage
         ("OrbitReal::Initialize() Fail to initialize Parameter:%s\n",
          this->GetName().c_str());
      #endif
      
      throw ParameterException
         ("WARNING:  " + e.GetMessage() + " in " + GetName() + "\n");
   }
   
   return true;
}


//-------------------------------------
// Methods inherited from GmatBase
//-------------------------------------

//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool OrbitReal::RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName)
{
   OrbitData::RenameRefObject(type, oldName, newName);
   RealVar::RenameRefObject(type, oldName, newName);
   
   return true;
}


//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string OrbitReal::GetRefObjectName(const Gmat::ObjectType type) const
{
   std::string objName = OrbitData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("OrbitReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return objName;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves reference object name array for given type. It will return all
 * object names if type is Gmat::UNKNOWN_NAME.
 *
 * @param <type> object type
 * @return reference object name.
 */
//------------------------------------------------------------------------------
const StringArray& OrbitReal::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   return OrbitData::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// virtual bool SetRefObjectName(const Gmat::ObjectType type,
//                               const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets reference object name to given object type.
 *
 * @param <type> object type
 * @param <name> object name
 *
 */
//------------------------------------------------------------------------------
bool OrbitReal::SetRefObjectName(const Gmat::ObjectType type,
                                 const std::string &name)
{
   return OrbitData::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* OrbitReal::GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name)
{
   GmatBase *obj = OrbitData::GetRefObject(type, name);
   
   if (obj == NULL)
   {
      throw ParameterException
         ("OrbitReal::GetRefObject() Cannot find ref. object of type:" +
          GmatBase::GetObjectTypeString(type) + ", name:" + name + " in " +
          this->GetName());
   }
   
   return obj;
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool OrbitReal::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name)
{
   #if DEBUG_ORBITREAL
   MessageInterface::ShowMessage
      ("OrbitReal::SetRefObject() setting type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif
   
   return OrbitData::SetRefObject(obj, type, name);
}

