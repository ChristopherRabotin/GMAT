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
   AddObject(obj);
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
// virtual Integer GetNumObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer TimeReal::GetNumObjects() const
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
GmatBase* TimeReal::GetObject(const std::string &objTypeName)
{
   return GetRefObject(objTypeName);
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
 *
 * @return true if the object has been set.
 */
//------------------------------------------------------------------------------
bool TimeReal::SetObject(Gmat::ObjectType objType, const std::string &objName,
                          GmatBase *obj)
{
   //Initialize(); //loj: 4/28/04 Initialize() will be called during run setup
   //loj: 3/31/04 do not check for NULL so it can reset object
   //if (obj != NULL) 
   return SetRefObject(objType, objName, obj);
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
bool TimeReal::AddObject(GmatBase *obj)
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

