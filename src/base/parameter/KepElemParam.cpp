//$Header$
//------------------------------------------------------------------------------
//                              KepElemParam
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
 * Implements Spacecraft Keplerian Elements parameter class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "KepElemParam.hpp"
#include "Rvector6.hpp"


//------------------------------------------------------------------------------
// KepElemParam(const std::string &name, GmatBase *obj,
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
KepElemParam::KepElemParam(const std::string &name, GmatBase *obj,
                           const std::string &desc, const std::string &unit)
    : Rvector6Parameter(name, "KepElemParam", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// KepElemParam(const KepElemParam &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepElemParam::KepElemParam(const KepElemParam &copy)
    : Rvector6Parameter(copy)
{
}

//------------------------------------------------------------------------------
// const KepElemParam& operator=(const KepElemParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepElemParam&
KepElemParam::operator=(const KepElemParam &right)
{
    if (this != &right)
        Rvector6Parameter::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~KepElemParam()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepElemParam::~KepElemParam()
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
Rvector6 KepElemParam::EvaluateRvector6()
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
Integer KepElemParam::GetNumObjects() const
{
    return GetNumRefObjects();
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
bool KepElemParam::AddObject(GmatBase *obj)
{
    if (obj != NULL)
        return AddRefObject(obj);
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
bool KepElemParam::Validate()
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
bool KepElemParam::Evaluate()
{
    mValue.Set(GetKepReal("KepSma"),
               GetKepReal("KepEcc"),
               GetKepReal("KepInc"),
               GetKepReal("KepRaan"),
               GetKepReal("KepAop"),
               GetKepReal("KepTa"));

    return mValue.IsValid(ORBIT_REAL_UNDEFINED);
}

