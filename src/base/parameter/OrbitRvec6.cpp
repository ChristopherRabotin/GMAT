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
   AddRefObject(obj);
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
// methods inherited from Rvec6Var
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
// methods nherited from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer OrbitRvec6::GetNumRefObjects() const
{
   return OrbitData::GetNumRefObjects();
}

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
   
   if (OrbitData::GetRefObject(Gmat::SOLAR_SYSTEM, ss->GetName()) == NULL)
      OrbitData::AddRefObject(ss->GetType(), ss->GetName(), ss);
   else
      OrbitData::SetRefObject(ss, Gmat::SOLAR_SYSTEM, ss->GetName());
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
bool OrbitRvec6::AddRefObject(GmatBase *obj)
{
   if (obj != NULL)
      return OrbitData::AddRefObject(obj->GetType(), obj->GetName(), obj);
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

//-------------------------------------
// Methods inherited from GmatBase
//-------------------------------------

//loj: 11/16/04 added
//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool OrbitRvec6::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   return OrbitData::RenameRefObject(type, oldName, newName);
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
std::string OrbitRvec6::GetRefObjectName(const Gmat::ObjectType type) const
{
   return OrbitData::GetRefObjectName(type);
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
bool OrbitRvec6::SetRefObjectName(const Gmat::ObjectType type,
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
GmatBase* OrbitRvec6::GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name)
{
   return OrbitData::GetRefObject(type, name);
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
bool OrbitRvec6::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name)
{
   return OrbitData::SetRefObject(obj, type, name);
}

