//$Header$
//------------------------------------------------------------------------------
//                              VelPeriapsisParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/22
//
/**
 * Implements Velocity at Periapsis parameter class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "VelPeriapsisParam.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// VelPeriapsisParam(const std::string &name, GmatBase *obj,
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
VelPeriapsisParam::VelPeriapsisParam(const std::string &name, GmatBase *obj,
                                     const std::string &desc, const std::string &unit)
    : RealParameter(name, "VelPeriapsisParam", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// VelPeriapsisParam(const VelPeriapsisParam &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
VelPeriapsisParam::VelPeriapsisParam(const VelPeriapsisParam &copy)
    : RealParameter(copy)
{
}

//------------------------------------------------------------------------------
// const VelPeriapsisParam& operator=(const VelPeriapsisParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const VelPeriapsisParam&
VelPeriapsisParam::operator=(const VelPeriapsisParam &right)
{
    if (this != &right)
        RealParameter::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~VelPeriapsisParam()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
VelPeriapsisParam::~VelPeriapsisParam()
{
}

//-------------------------------------
// Inherited methods from RealParameter
//-------------------------------------

//------------------------------------------------------------------------------
// Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Real VelPeriapsisParam::EvaluateReal()
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
Integer VelPeriapsisParam::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* VelPeriapsisParam::GetObject(const std::string &objTypeName)
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
bool VelPeriapsisParam::SetObject(Gmat::ObjectType objType,
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
bool VelPeriapsisParam::AddObject(GmatBase *obj)
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
bool VelPeriapsisParam::Validate()
{
    return ValidateRefObjects(this);
}

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool VelPeriapsisParam::Evaluate()
{
    mValue = GetOtherKepReal("VelPeriapsis");    
    
    if (mValue == ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

