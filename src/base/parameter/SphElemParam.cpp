//$Header$
//------------------------------------------------------------------------------
//                              SphElemParam
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
 * Implements Spacecraft Spherical Elements parameter class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "SphElemParam.hpp"
#include "Rvector6.hpp"


//------------------------------------------------------------------------------
// SphElemParam(const std::string &name, GmatBase *obj,
//              const std::string &desc, const std::string &unit)
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
SphElemParam::SphElemParam(const std::string &name, GmatBase *obj,
                           const std::string &desc, const std::string &unit)
    : Rvector6Parameter(name, "SphElemParam", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// SphElemParam(const SphElemParam &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
SphElemParam::SphElemParam(const SphElemParam &copy)
    : Rvector6Parameter(copy)
{
}

//------------------------------------------------------------------------------
// const SphElemParam& operator=(const SphElemParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const SphElemParam&
SphElemParam::operator=(const SphElemParam &right)
{
    if (this != &right)
        Rvector6Parameter::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~SphElemParam()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SphElemParam::~SphElemParam()
{
}

//-----------------------------------------
// Inherited methods from Rvector6Parameter
//-----------------------------------------

//------------------------------------------------------------------------------
// Rvector6 EvaluateRvector6()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Rvector6 SphElemParam::EvaluateRvector6()
{
    Evaluate();
    return mValue;
}


//--------------------------------------
// Inherited methods from Parameter
//--------------------------------------

//------------------------------------------------------------------------------
// virtual Integer GetNumObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer SphElemParam::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* SphElemParam::GetObject(const std::string &objTypeName)
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
bool SphElemParam::SetObject(Gmat::ObjectType objType,
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
bool SphElemParam::AddObject(GmatBase *obj)
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
bool SphElemParam::Validate()
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
bool SphElemParam::Evaluate()
{
    mValue.Set(GetSphReal("SphRmag"),
               GetSphReal("SphRa"),
               GetSphReal("SphDec"),
               GetSphReal("SphVmag"),
               GetSphReal("SphRaV"),
               GetSphReal("SphDecV"));

    return mValue.IsValid(ORBIT_REAL_UNDEFINED);
}

