//$Header$
//------------------------------------------------------------------------------
//                                PlanetReal
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/12/13
//
/**
 * Implements PlanetReal class which provides base class for the planet
 * realated Real Parameters
 */
//------------------------------------------------------------------------------

#include "PlanetReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PLANET_REAL 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// PlanetReal(const std::string &name, const std::string &typeStr, 
//            GmatBase *obj, const std::string &desc,
//            const std::string &unit, Gmat::ObjectType ownerType,
//            GmatParam::DepObject depObj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <typeStr> type of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 * @param <ownerType> object type who owns this parameter as property
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NO_DEP)
 */
//------------------------------------------------------------------------------
PlanetReal::PlanetReal(const std::string &name, const std::string &typeStr, 
                       GmatBase *obj, const std::string &desc,
                       const std::string &unit, Gmat::ObjectType ownerType,
                       GmatParam::DepObject depObj)
   : RealVar(name, typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit, depObj,
             ownerType, false)
{
   mNeedCoordSystem = false;
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// PlanetReal(const PlanetReal &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
PlanetReal::PlanetReal(const PlanetReal &copy)
   : RealVar(copy)
{
}

//------------------------------------------------------------------------------
// PlanetReal& operator=(const PlanetReal &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
PlanetReal& PlanetReal::operator=(const PlanetReal &right)
{
   if (this != &right)
      RealVar::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~PlanetReal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
PlanetReal::~PlanetReal()
{
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
Real PlanetReal::EvaluateReal()
{
   Evaluate();
   return mRealValue;
}

//------------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets SolarSystem pointer.
 */
//------------------------------------------------------------------------------
void PlanetReal::SetSolarSystem(SolarSystem *ss)
{
#if DEBUG_PLANET_REAL
   MessageInterface::ShowMessage
      ("PlanetReal::SetSolarSystem() ss=%s to %s\n", ss->GetTypeName().c_str(),
       this->GetName().c_str());
#endif
   
   if (PlanetData::GetRefObject(Gmat::SOLAR_SYSTEM, ss->GetName()) == NULL)
      PlanetData::AddRefObject(ss->GetType(), ss->GetName(), ss);
   else
      PlanetData::SetRefObject(ss, Gmat::SOLAR_SYSTEM, ss->GetName());
   
}

//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer PlanetReal::GetNumRefObjects() const
{
   return PlanetData::GetNumRefObjects();
}

//------------------------------------------------------------------------------
// virtual bool AddRefObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds reference object.
 *
 * @param <obj> object pointer
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool PlanetReal::AddRefObject(GmatBase *obj)
{
   if (obj != NULL)
      return PlanetData::AddRefObject(obj->GetType(), obj->GetName(), obj);
   else
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
bool PlanetReal::Validate()
{
   return ValidateRefObjects(this);
}

//------------------------------------------------------------------------------
// virtual void Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes reference objects.
 *
 * @return true if all objects are set; false otherwise
 */
//------------------------------------------------------------------------------
void PlanetReal::Initialize()
{
   InitializeRefObjects();
}

//-------------------------------------
// Methods inherited from GmatBase
//-------------------------------------

//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool PlanetReal::RenameRefObject(const Gmat::ObjectType type,
                              const std::string &oldName,
                              const std::string &newName)
{
   return PlanetData::RenameRefObject(type, oldName, newName);
}

//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Calls PlanetData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string PlanetReal::GetRefObjectName(const Gmat::ObjectType type) const
{
   std::string objName = PlanetData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("OrbitReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return objName;
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
bool PlanetReal::SetRefObjectName(const Gmat::ObjectType type,
                                    const std::string &name)
{
   return PlanetData::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls PlanetData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* PlanetReal::GetRefObject(const Gmat::ObjectType type,
                                const std::string &name)
{
   return PlanetData::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls PlanetData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool PlanetReal::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                const std::string &name)
{
   return PlanetData::SetRefObject(obj, type, name);
}

