//$Header$
//------------------------------------------------------------------------------
//                              CartStateParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/22
//
/**
 * Implements Spacecraft Cartesian parameter class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "CartStateParam.hpp"
#include "Rvector6.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// CartStateParam(const std::string &name, GmatBase *obj,
//                const std::string &desc, const std::string &unit)
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
CartStateParam::CartStateParam(const std::string &name, GmatBase *obj,
                               const std::string &desc, const std::string &unit)
    : Rvector6Parameter(name, "CartStateParam", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// CartStateParam(const CartStateParam &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartStateParam::CartStateParam(const CartStateParam &copy)
    : Rvector6Parameter(copy)
{
}

//------------------------------------------------------------------------------
// const CartStateParam& operator=(const CartStateParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const CartStateParam&
CartStateParam::operator=(const CartStateParam &right)
{
    if (this != &right)
        Rvector6Parameter::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~CartStateParam()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
CartStateParam::~CartStateParam()
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
Rvector6 CartStateParam::EvaluateRvector6()
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
Integer CartStateParam::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* CartStateParam::GetObject(const std::string &objTypeName)
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
bool CartStateParam::SetObject(Gmat::ObjectType objType,
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
bool CartStateParam::AddObject(GmatBase *obj)
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
bool CartStateParam::Validate()
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
bool CartStateParam::Evaluate()
{    
    mValue.Set(GetCartReal("CartX"),
               GetCartReal("CartY"),
               GetCartReal("CartZ"),
               GetCartReal("CartVx"),
               GetCartReal("CartVy"),
               GetCartReal("CartVz"));

    return mValue.IsValid(ORBIT_REAL_UNDEFINED);
}

