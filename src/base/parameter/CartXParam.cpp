//$Header$
//------------------------------------------------------------------------------
//                              CartXParam
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
 * Implements Cartesian Position X parameter class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "CartXParam.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// CartXParam(const std::string &name, GmatBase *obj,
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
CartXParam::CartXParam(const std::string &name, GmatBase *obj,
                       const std::string &desc, const std::string &unit)
    : RealParameter(name, "CartXParam", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// CartXParam(const CartXParam &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartXParam::CartXParam(const CartXParam &copy)
    : RealParameter(copy)
{
}

//------------------------------------------------------------------------------
// const CartXParam& operator=(const CartXParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const CartXParam&
CartXParam::operator=(const CartXParam &right)
{
    if (this != &right)
        RealParameter::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~CartXParam()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
CartXParam::~CartXParam()
{
}

//-------------------------------------
// Inherited methods from RealParameter
//-------------------------------------

//------------------------------------------------------------------------------
// Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * Evaluates and returns real value of the parameter.
 */
//------------------------------------------------------------------------------
Real CartXParam::EvaluateReal()
{
    Evaluate();
    return mValue;
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds reference objects.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool CartXParam::AddObject(GmatBase *obj)
{
    if (obj != NULL)
        return AddRefObject(obj);
    else
        return false;
}

//------------------------------------------------------------------------------
// Integer GetNumObjects() const
//------------------------------------------------------------------------------
/**
 * Retrives number of reference objects.
 */
//------------------------------------------------------------------------------
Integer CartXParam::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
/**
 * Validates reference objects.
 */
//------------------------------------------------------------------------------
bool CartXParam::Validate()
{
    return ValidateRefObjects(this);
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool CartXParam::Evaluate()
{
    mValue = GetCartReal("CartX");
    
    if (mValue == ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

