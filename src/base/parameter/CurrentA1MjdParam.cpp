//$Header$
//------------------------------------------------------------------------------
//                              CurrentA1MjdParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/12
//
/**
 * Implements Current A1Mjd parameter class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "CurrentA1MjdParam.hpp"


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// CurrentA1MjdParam(const std::string &name, GmatBase *obj,
//                   const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
CurrentA1MjdParam::CurrentA1MjdParam(const std::string &name, GmatBase *obj,
                                     const std::string &desc, const std::string &unit)
    : RealParameter(name, "CurrentA1MjdParam", SYSTEM_PARAM, obj, desc, unit, true)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// CurrentA1MjdParam::CurrentA1MjdParam(const CurrentA1MjdParam &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CurrentA1MjdParam::CurrentA1MjdParam(const CurrentA1MjdParam &param)
    : RealParameter(param)
{
}

//------------------------------------------------------------------------------
// CurrentA1MjdParam& CurrentA1MjdParam::operator=(const CurrentA1MjdParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CurrentA1MjdParam& CurrentA1MjdParam::operator=(const CurrentA1MjdParam &right)
{
    if (this != &right)
        RealParameter::operator=(right);
      
    return *this;
}

//------------------------------------------------------------------------------
// CurrentA1MjdParam::~CurrentA1MjdParam()
//------------------------------------------------------------------------------
CurrentA1MjdParam::~CurrentA1MjdParam()
{
}

//-------------------------------------
// Inherited methods from RealParameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Real CurrentA1MjdParam::EvaluateReal()
{
    Evaluate();
    return mValue;
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
Integer CurrentA1MjdParam::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* CurrentA1MjdParam::GetObject(const std::string &objTypeName)
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
 * @return true if the object has been set.
 */
//------------------------------------------------------------------------------
bool CurrentA1MjdParam::SetObject(Gmat::ObjectType objType,
                                  const std::string &objName,
                                  GmatBase *obj)
{
    if (obj != NULL)
        return SetRefObject(objType, objName, obj);
    else
        return false;
}

//------------------------------------------------------------------------------
// virtual bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds reference objects.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool CurrentA1MjdParam::AddObject(GmatBase *obj)
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
bool CurrentA1MjdParam::Validate()
{
    return ValidateRefObjects(this);
}

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool CurrentA1MjdParam::Evaluate()
{
    mValue = GetCurrentTimeReal("A1Mjd");
    
    if (mValue == TIME_REAL_UNDEFINED)
        return false;
    else
        return true;
}

