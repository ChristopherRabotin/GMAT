//$Header$
//------------------------------------------------------------------------------
//                              SphericalParameters
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/03/12
//
/**
 * Implements Spacecraft Spehrical parameter classes.
 *   SphRA, SphDec, SphElem
 */
//------------------------------------------------------------------------------
#include "SphericalParameters.hpp"


//==============================================================================
//                              SphRA
//==============================================================================
/**
 * Implements Spherical Declination parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SphRA(const std::string &name, GmatBase *obj,
//       const std::string &desc, const std::string &unit)
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
SphRA::SphRA(const std::string &name, GmatBase *obj,
             const std::string &desc, const std::string &unit)
    : OrbitReal(name, "SphRA", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// SphRA(const SphRA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
SphRA::SphRA(const SphRA &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const SphRA& operator=(const SphRA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const SphRA&
SphRA::operator=(const SphRA &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~SphRA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SphRA::~SphRA()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool SphRA::Evaluate()
{
    mRealValue = OrbitData::GetSphReal("SphRA");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}


//==============================================================================
//                              SphDec
//==============================================================================
/**
 * Implements Spherical Declination parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SphDec(const std::string &name, GmatBase *obj,
//        const std::string &desc, const std::string &unit)
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
SphDec::SphDec(const std::string &name, GmatBase *obj,
               const std::string &desc, const std::string &unit)
    : OrbitReal(name, "SphDec", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// SphDec(const SphDec &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
SphDec::SphDec(const SphDec &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const SphDec& operator=(const SphDec &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const SphDec&
SphDec::operator=(const SphDec &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~SphDec()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SphDec::~SphDec()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool SphDec::Evaluate()
{
    mRealValue = OrbitData::GetSphReal("SphDec");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}


//==============================================================================
//                              SphElem
//==============================================================================
/**
 * Implements Spherical Elements class.
 *   Elements are SphRMag, SphRA, SphDec, SphVMag, SphVRA, SphVDec
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SphElem(const std::string &name, GmatBase *obj,
//         const std::string &desc, const std::string &unit)
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
SphElem::SphElem(const std::string &name, GmatBase *obj,
                 const std::string &desc, const std::string &unit)
    : Rvec6Var(name, "SphElem", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// SphElem(const SphElem &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
SphElem::SphElem(const SphElem &copy)
    : Rvec6Var(copy)
{
}

//------------------------------------------------------------------------------
// const SphElem& operator=(const SphElem &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const SphElem&
SphElem::operator=(const SphElem &right)
{
    if (this != &right)
        Rvec6Var::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~SphElem()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SphElem::~SphElem()
{
}

//-----------------------------------------
// Inherited methods from Rvec6Var
//-----------------------------------------

//------------------------------------------------------------------------------
// Rvector6 EvaluateRvector6()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Rvector6 SphElem::EvaluateRvector6()
{
    Evaluate();
    return mRvec6Value;
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
Integer SphElem::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* SphElem::GetObject(const std::string &objTypeName)
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
bool SphElem::SetObject(Gmat::ObjectType objType,
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
bool SphElem::AddObject(GmatBase *obj)
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
bool SphElem::Validate()
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
bool SphElem::Evaluate()
{
    mRvec6Value.Set(GetSphReal("SphRMag"),
                    GetSphReal("SphRA"),
                    GetSphReal("SphDec"),
                    GetSphReal("SphVMag"),
                    GetSphReal("SphVRA"),
                    GetSphReal("SphVDec"));

    return mRvec6Value.IsValid(ORBIT_REAL_UNDEFINED);
}

