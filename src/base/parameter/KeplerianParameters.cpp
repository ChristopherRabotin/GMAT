//$Header$
//------------------------------------------------------------------------------
//                             File: KeplerianParameters.cpp
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
 * Implements Keplerian related parameter classes.
 *   KepSMA, KepEcc, KepInc, KepAOP, KepRAAN, KepTA, KepMA, KepMM, KepElem
 */
//------------------------------------------------------------------------------
#include "KeplerianParameters.hpp"

//==============================================================================
//                              KepSMA
//==============================================================================
/**
 * Implements Keplerian Semimajor Axis class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepSMA(const std::string &name, GmatBase *obj,
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
KepSMA::KepSMA(const std::string &name, GmatBase *obj,
               const std::string &desc, const std::string &unit)
    : OrbitReal(name, "KepSMA", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// KepSMA(const KepSMA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepSMA::KepSMA(const KepSMA &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepSMA& operator=(const KepSMA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepSMA&
KepSMA::operator=(const KepSMA &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~KepSMA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepSMA::~KepSMA()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepSMA::Evaluate()
{
    mRealValue = OrbitData::GetKepReal("KepSMA");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

//==============================================================================
//                              KepEcc
//==============================================================================
/**
 * Implements Keplerian Eccentricity class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepEcc(const std::string &name, GmatBase *obj,
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
KepEcc::KepEcc(const std::string &name, GmatBase *obj,
               const std::string &desc, const std::string &unit)
    : OrbitReal(name, "KepEcc", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// KepEcc(const KepEcc &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepEcc::KepEcc(const KepEcc &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepEcc& operator=(const KepEcc &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepEcc&
KepEcc::operator=(const KepEcc &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~KepEcc()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepEcc::~KepEcc()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool KepEcc::Evaluate()
{
    mRealValue = OrbitData::GetKepReal("KepEcc");
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}


//==============================================================================
//                              KepInc
//==============================================================================
/**
 * Implements Keplerian Inclinatin class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepInc(const std::string &name, GmatBase *obj,
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
KepInc::KepInc(const std::string &name, GmatBase *obj,
               const std::string &desc, const std::string &unit)
    : OrbitReal(name, "KepInc", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// KepInc(const KepInc &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepInc::KepInc(const KepInc &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepInc& operator=(const KepInc &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepInc&
KepInc::operator=(const KepInc &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~KepInc()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepInc::~KepInc()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepInc::Evaluate()
{
    mRealValue = OrbitData::GetKepReal("KepInc");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

//==============================================================================
//                              KepAOP
//==============================================================================
/**
 * Implements Keplerian Argument of Periapsis class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepAOP(const std::string &name, GmatBase *obj,
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
KepAOP::KepAOP(const std::string &name, GmatBase *obj,
               const std::string &desc, const std::string &unit)
    : OrbitReal(name, "KepAOP", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// KepAOP(const KepAOP &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepAOP::KepAOP(const KepAOP &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepAOP& operator=(const KepAOP &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepAOP&
KepAOP::operator=(const KepAOP &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~KepAOP()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepAOP::~KepAOP()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepAOP::Evaluate()
{
    mRealValue = OrbitData::GetKepReal("KepAOP");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

//==============================================================================
//                              KepRAAN
//==============================================================================
/**
 * Implements Keplerian Right Ascention of Ascending Node class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepRAAN(const std::string &name, GmatBase *obj,
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
KepRAAN::KepRAAN(const std::string &name, GmatBase *obj,
                           const std::string &desc, const std::string &unit)
    : OrbitReal(name, "KepRAAN", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// KepRAAN(const KepRAAN &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepRAAN::KepRAAN(const KepRAAN &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepRAAN& operator=(const KepRAAN &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepRAAN&
KepRAAN::operator=(const KepRAAN &right)
{
    if (this != &right)
        OrbitReal::operator=(right);
    
    return *this;
}

//------------------------------------------------------------------------------
// ~KepRAAN()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepRAAN::~KepRAAN()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepRAAN::Evaluate()
{
    mRealValue = OrbitData::GetKepReal("KepRAAN");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

//==============================================================================
//                              KepTA
//==============================================================================
/**
 * Implements Keplerian True Anomaly class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepTA(const std::string &name, GmatBase *obj,
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
KepTA::KepTA(const std::string &name, GmatBase *obj,
                       const std::string &desc, const std::string &unit)
    : OrbitReal(name, "KepTA", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// KepTA(const KepTA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepTA::KepTA(const KepTA &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepTA& operator=(const KepTA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepTA&
KepTA::operator=(const KepTA &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~KepTA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepTA::~KepTA()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepTA::Evaluate()
{
    mRealValue = OrbitData::GetKepReal("KepTA");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

//==============================================================================
//                              KepMA
//==============================================================================
/**
 * Implements Keplerian Mean Anomaly class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepMA(const std::string &name, GmatBase *obj,
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
KepMA::KepMA(const std::string &name, GmatBase *obj,
             const std::string &desc, const std::string &unit)
    : OrbitReal(name, "KepMA", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// KepMA(const KepMA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepMA::KepMA(const KepMA &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepMA& operator=(const KepMA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepMA&
KepMA::operator=(const KepMA &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~KepMA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepMA::~KepMA()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepMA::Evaluate()
{
    mRealValue = OrbitData::GetKepReal("KepMA");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

//==============================================================================
//                              KepMM
//==============================================================================
/**
 * Implements Keplerian Mean Motion class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepMM(const std::string &name, GmatBase *obj,
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
KepMM::KepMM(const std::string &name, GmatBase *obj,
             const std::string &desc, const std::string &unit)
    : OrbitReal(name, "KepMM", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// KepMM(const KepMM &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepMM::KepMM(const KepMM &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepMM& operator=(const KepMM &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepMM&
KepMM::operator=(const KepMM &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~KepMM()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepMM::~KepMM()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepMM::Evaluate()
{
    mRealValue = OrbitData::GetOtherKepReal("KepMm");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}


//==============================================================================
//                              KepElem
//==============================================================================
/**
 * Implements Keplerian Elements class.
 *   6 elements: KepSMA, KepEcc, KepInc, KepRAAN, KepAOP, KepTA
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepElem(const std::string &name, GmatBase *obj,
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
KepElem::KepElem(const std::string &name, GmatBase *obj,
                 const std::string &desc, const std::string &unit)
    : Rvec6Var(name, "KepElem", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// KepElem(const KepElem &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepElem::KepElem(const KepElem &copy)
    : Rvec6Var(copy)
{
}

//------------------------------------------------------------------------------
// const KepElem& operator=(const KepElem &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepElem&
KepElem::operator=(const KepElem &right)
{
    if (this != &right)
        Rvec6Var::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~KepElem()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepElem::~KepElem()
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
Rvector6 KepElem::EvaluateRvector6()
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
Integer KepElem::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* KepElem::GetObject(const std::string &objTypeName)
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
bool KepElem::SetObject(Gmat::ObjectType objType,
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
bool KepElem::AddObject(GmatBase *obj)
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
bool KepElem::Validate()
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
bool KepElem::Evaluate()
{
    mRvec6Value.Set(OrbitData::GetKepReal("KepSMA"),
                    GetKepReal("KepEcc"),
                    GetKepReal("KepInc"),
                    GetKepReal("KepRAAN"),
                    GetKepReal("KepAOP"),
                    GetKepReal("KepTA"));

    return mRvec6Value.IsValid(ORBIT_REAL_UNDEFINED);
}
