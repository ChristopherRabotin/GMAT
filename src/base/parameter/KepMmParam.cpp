//$Header$
//------------------------------------------------------------------------------
//                              KepMmParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/06
//
/**
 * Implements Keplerian Mean Motion parameter class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "KepMmParam.hpp"

//------------------------------------------------------------------------------
// KepMmParam(const std::string &name, GmatBase *obj,
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
KepMmParam::KepMmParam(const std::string &name, GmatBase *obj,
                       const std::string &desc, const std::string &unit)
    : RealParameter(name, "KepMmParam", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// KepMmParam(const KepMmParam &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepMmParam::KepMmParam(const KepMmParam &copy)
    : RealParameter(copy)
{
}

//------------------------------------------------------------------------------
// const KepMmParam& operator=(const KepMmParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepMmParam&
KepMmParam::operator=(const KepMmParam &right)
{
    if (this != &right)
        RealParameter::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~KepMmParam()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepMmParam::~KepMmParam()
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
Real KepMmParam::EvaluateReal()
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
Integer KepMmParam::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* KepMmParam::GetObject(const std::string &objTypeName)
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
bool KepMmParam::SetObject(Gmat::ObjectType objType,
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
bool KepMmParam::AddObject(GmatBase *obj)
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
bool KepMmParam::Validate()
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
bool KepMmParam::Evaluate()
{
    mValue = GetOtherKepReal("KepMm");    
    
    if (mValue == ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}
