//$Header$
//------------------------------------------------------------------------------
//                                TimeReal
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/04/28
//
/**
 * Implements TimeReal class which provides base class for time realated Real
 * Parameters
 */
//------------------------------------------------------------------------------

#include "TimeReal.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// TimeReal(const std::string &name, const std::string &typeStr, 
//          ParameterKey key, GmatBase *obj, const std::string &desc,
//          const std::string &unit, bool isTimeParam)
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
TimeReal::TimeReal(const std::string &name, const std::string &typeStr, 
                   ParameterKey key, GmatBase *obj, const std::string &desc,
                   const std::string &unit, bool isTimeParam)
   : RealVar(name, typeStr, key, obj, desc, unit, isTimeParam)
{
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// TimeReal(const TimeReal &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TimeReal::TimeReal(const TimeReal &copy)
   : RealVar(copy)
{
}

//------------------------------------------------------------------------------
// TimeReal& operator=(const TimeReal &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TimeReal& TimeReal::operator=(const TimeReal &right)
{
   if (this != &right)
      RealVar::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~TimeReal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
TimeReal::~TimeReal()
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
Real TimeReal::GetReal()
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
Real TimeReal::EvaluateReal()
{
   Evaluate();
   return mRealValue;
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer TimeReal::GetNumRefObjects() const
{
   return TimeData::GetNumRefObjects();
}

//  //------------------------------------------------------------------------------
//  // GmatBase* GetObject(const std::string &objTypeName)
//  //------------------------------------------------------------------------------
//  /**
//   * @return reference object pointer of given object type
//   */
//  //------------------------------------------------------------------------------
//  GmatBase* TimeReal::GetObject(const std::string &objTypeName)
//  {
//     return TimeData::GetRefObject(objTypeName);
//  }

//  //------------------------------------------------------------------------------
//  // virtual bool SetObject(Gmat::ObjectType objType, const std::string &objName,
//  //                        GmatBase *obj
//  //------------------------------------------------------------------------------
//  /**
//   * Sets reference object.
//   *
//   * @param <objType> object type
//   * @param <objName> object name
//   *
//   * @return true if the object has been set.
//   */
//  //------------------------------------------------------------------------------
//  bool TimeReal::SetObject(Gmat::ObjectType objType, const std::string &objName,
//                            GmatBase *obj)
//  {
//     //Initialize(); //loj: 4/28/04 Initialize() will be called during run setup
//     //loj: 3/31/04 do not check for NULL so it can reset object
//     //if (obj != NULL) 
//     return TimeData::SetRefObject(objType, objName, obj);
//     //else
//     //return false;
//  }

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
bool TimeReal::AddRefObject(GmatBase *obj)
{
   if (obj != NULL)
      return TimeData::AddRefObject(obj->GetType(), obj->GetName(), obj);
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
bool TimeReal::Validate()
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
void TimeReal::Initialize()
{
   mInitialEpoch = 0.0;
   mIsInitialEpochSet = false;
   InitializeRefObjects();
}

//-------------------------------------
// Methods inherited from GmatBase
//-------------------------------------

//loj: 9/10/04 added

//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Calls TimeData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string TimeReal::GetRefObjectName(const Gmat::ObjectType type) const
{
   return TimeData::GetRefObjectName(type);
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
bool TimeReal::SetRefObjectName(const Gmat::ObjectType type,
                                 const std::string &name)
{
   TimeData::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls TimeData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* TimeReal::GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name)
{
   return TimeData::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls TimeData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool TimeReal::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name)
{
   return TimeData::SetRefObject(obj, type, name);
}

