//$Header$
//------------------------------------------------------------------------------
//                              CartVyParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/08
//
/**
 * Implements Cartesian Velocity Y parameter class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "CartVyParam.hpp"

//------------------------------------------------------------------------------
// CartVyParam(const std::string &name, GmatBase *obj,
//            const std::string &desc, const std::string &unit)
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
CartVyParam::CartVyParam(const std::string &name, GmatBase *obj,
                         const std::string &desc, const std::string &unit)
    : RealParameter(name, "CartVyParam", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// CartVyParam(const CartVyParam &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartVyParam::CartVyParam(const CartVyParam &param)
    : RealParameter(param)
{
}

//------------------------------------------------------------------------------
// const CartVyParam& operator=(const CartVyParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const CartVyParam&
CartVyParam::operator=(const CartVyParam &right)
{
    if (this != &right)
        RealParameter::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~CartVyParam()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
CartVyParam::~CartVyParam()
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
Real CartVyParam::EvaluateReal()
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
Integer CartVyParam::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* CartVyParam::GetObject(const std::string &objTypeName)
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
bool CartVyParam::SetObject(Gmat::ObjectType objType,
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
bool CartVyParam::AddObject(GmatBase *obj)
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
bool CartVyParam::Validate()
{
    return ValidateRefObjects(this);
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool CartVyParam::Evaluate()
{
    mValue = GetCartReal("CartVy");    
    
    if (mValue == ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

