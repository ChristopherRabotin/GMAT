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
#include "MessageInterface.hpp"

#define DEBUG_ORBITREAL 0

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// OrbitReal(const std::string &name, const std::string &typeStr, 
//           ParameterKey key, GmatBase *obj, const std::string &desc,
//           const std::string &unit, bool isTimeParam)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <typeStr> type of the parameter
 * @param <key> SYSTEM_PARAM or USER_PARAM
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 * @param <isTimeParam> true if time related parameter
 */
//------------------------------------------------------------------------------
OrbitReal::OrbitReal(const std::string &name, const std::string &typeStr, 
                     ParameterKey key, GmatBase *obj, const std::string &desc,
                     const std::string &unit, bool isTimeParam)
   : RealVar(name, typeStr, key, obj, desc, unit, isTimeParam)
{
   AddObject(obj);
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
   : RealVar(copy)
{
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
      RealVar::operator=(right);

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
}

//-------------------------------------
// Inherited methods from RealVar
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Real GetReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Real OrbitReal::GetReal()
{
   return mRealValue;
}

//------------------------------------------------------------------------------
// virtual Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Real OrbitReal::EvaluateReal()
{
   Evaluate();
   return mRealValue;
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//loj: 6/24/04 added
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
      ("OrbitReal::SetSolarSystem() ss=%s", ss->GetTypeName().c_str());
#endif
   
   if (OrbitData::GetRefObject("SolarSystem") == NULL)
      OrbitData::AddRefObject(ss);
   else
      OrbitData::SetRefObject(Gmat::SOLAR_SYSTEM, ss->GetName(), ss);
}

//------------------------------------------------------------------------------
// virtual Integer GetNumObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer OrbitReal::GetNumObjects() const
{
   return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
/**
 * @return reference object pointer of given object type
 */
//------------------------------------------------------------------------------
GmatBase* OrbitReal::GetObject(const std::string &objTypeName)
{
   return OrbitData::GetRefObject(objTypeName);
}

//------------------------------------------------------------------------------
// virtual bool SetObject(Gmat::ObjectType objType, const std::string &objName,
//                        GmatBase *obj
//------------------------------------------------------------------------------
/**
 * Sets reference object.
 *
 * @param <objType> object type
 * @param <objName> object name
 * @param <obj> object pointer
 *
 * @return true if the object has been set.
 */
//------------------------------------------------------------------------------
bool OrbitReal::SetObject(Gmat::ObjectType objType, const std::string &objName,
                          GmatBase *obj)
{
   //Initialize(); //loj: 4/28/04 Initialize() will be called during run setup
   //loj: 3/31/04 do not check for NULL so it can reset object
   //if (obj != NULL) 
   return OrbitData::SetRefObject(objType, objName, obj);
   //else
   //return false;
}

//------------------------------------------------------------------------------
// virtual bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds reference object.
 *
 * @param <obj> object pointer
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool OrbitReal::AddObject(GmatBase *obj)
{
   if (obj != NULL)
   {
      if (AddRefObject(obj))
         ManageObject(obj);

      return true;
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
// virtual void Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes reference objects.
 *
 * @return true if all objects are set; false otherwise
 */
//------------------------------------------------------------------------------
void OrbitReal::Initialize()
{
   InitializeRefObjects();
}

