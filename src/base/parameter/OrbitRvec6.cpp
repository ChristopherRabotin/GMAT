//$Header$
//------------------------------------------------------------------------------
//                                OrbitRvec6
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/09/08
//
/**
 * Implements OrbitRvec6 class which provides base class for orbit realated
 * Rvector6 Parameters.
 */
//------------------------------------------------------------------------------

#include "OrbitRvec6.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ORBITRVEC6 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// OrbitRvec6(const std::string &name, const std::string &typeStr, 
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
OrbitRvec6::OrbitRvec6(const std::string &name, const std::string &typeStr, 
                       ParameterKey key, GmatBase *obj, const std::string &desc,
                       const std::string &unit, bool isTimeParam)
   : Rvec6Var(name, typeStr, key, obj, desc, unit, isTimeParam)
{
   AddObject(obj);
}

//------------------------------------------------------------------------------
// OrbitRvec6(const OrbitRvec6 &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OrbitRvec6::OrbitRvec6(const OrbitRvec6 &copy)
   : Rvec6Var(copy)
{
}

//------------------------------------------------------------------------------
// OrbitRvec6& operator=(const OrbitRvec6 &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OrbitRvec6& OrbitRvec6::operator=(const OrbitRvec6 &right)
{
   if (this != &right)
      Rvec6Var::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~OrbitRvec6()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
OrbitRvec6::~OrbitRvec6()
{
}

//-------------------------------------
// Inherited methods from Rvec6Var
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Rvector6 GetRvector6() const
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Rvector6 OrbitRvec6::GetRvector6() const
{
   return mRvec6Value;
}

//------------------------------------------------------------------------------
// virtual Rvector6 EvaluateRvector6()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Rvector6 OrbitRvec6::EvaluateRvector6()
{
   Evaluate();
   return mRvec6Value;
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets SolarSystem pointer.
 */
//------------------------------------------------------------------------------
void OrbitRvec6::SetSolarSystem(SolarSystem *ss)
{
#if DEBUG_ORBITRVEC6
   MessageInterface::ShowMessage
      ("OrbitRvec6::SetSolarSystem() ss=%s", ss->GetTypeName().c_str());
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
Integer OrbitRvec6::GetNumObjects() const
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
GmatBase* OrbitRvec6::GetObject(const std::string &objTypeName)
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
bool OrbitRvec6::SetObject(Gmat::ObjectType objType, const std::string &objName,
                          GmatBase *obj)
{
   return OrbitData::SetRefObject(objType, objName, obj);
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
bool OrbitRvec6::AddObject(GmatBase *obj)
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
bool OrbitRvec6::Validate()
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
void OrbitRvec6::Initialize()
{
   InitializeRefObjects();
}

