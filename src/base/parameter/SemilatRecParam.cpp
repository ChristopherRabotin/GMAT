//$Header$
//------------------------------------------------------------------------------
//                              SemilatRecParam
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
 * Implements Semilatus Rectum parameter class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "SemilatRecParam.hpp"

//------------------------------------------------------------------------------
// SemilatRecParam(const std::string &name, GmatBase *obj,
//                 const std::string &desc, const std::string &unit)
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
SemilatRecParam::SemilatRecParam(const std::string &name, GmatBase *obj,
                                 const std::string &desc, const std::string &unit)
    : RealParameter(name, "SemilatRecParam", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// SemilatRecParam(const SemilatRecParam &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
SemilatRecParam::SemilatRecParam(const SemilatRecParam &copy)
    : RealParameter(copy)
{
}

//------------------------------------------------------------------------------
// const SemilatRecParam& operator=(const SemilatRecParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const SemilatRecParam&
SemilatRecParam::operator=(const SemilatRecParam &right)
{
    if (this != &right)
        RealParameter::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~SemilatRecParam()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SemilatRecParam::~SemilatRecParam()
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
Real SemilatRecParam::EvaluateReal()
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
Integer SemilatRecParam::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* SemilatRecParam::GetObject(const std::string &objTypeName)
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
bool SemilatRecParam::SetObject(Gmat::ObjectType objType,
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
bool SemilatRecParam::AddObject(GmatBase *obj)
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
bool SemilatRecParam::Validate()
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
bool SemilatRecParam::Evaluate()
{
    mValue = GetAngularReal("SemilatRec");    
    
    if (mValue == ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}
