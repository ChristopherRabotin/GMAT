//$Header$
//------------------------------------------------------------------------------
//                            File: CartesianParameters.cpp
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
 * Implements Cartesian related parameter classes.
 *   CartX, CartY, CartZ, CartVx, CartVy, CartVz, CartState
 */
//------------------------------------------------------------------------------

#include "CartesianParameters.hpp"


//==============================================================================
//                              CartX
//==============================================================================
/**
 * Implements Cartesian position X class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartX(const std::string &name, GmatBase *obj,
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
CartX::CartX(const std::string &name, GmatBase *obj,
             const std::string &desc, const std::string &unit)
    : OrbitReal(name, "CartX", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// CartX(const CartX &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartX::CartX(const CartX &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const CartX& operator=(const CartX &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const CartX&
CartX::operator=(const CartX &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~CartX()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
CartX::~CartX()
{
}

//  //-------------------------------------
//  // Inherited methods from RealVar
//  //-------------------------------------

//  //------------------------------------------------------------------------------
//  // virtual Real EvaluateReal()
//  //------------------------------------------------------------------------------
//  /**
//   * @return newly evaluated value of parameter
//   */
//  //------------------------------------------------------------------------------
//  Real CartX::EvaluateReal()
//  {
//      Evaluate();
//      return mRealValue;
//  }

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
bool CartX::Evaluate()
{
    mRealValue = OrbitData::GetCartReal("CartX");
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}


//==============================================================================
//                              CartY
//==============================================================================
/**
 * Implements Cartesian position Y class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartY(const std::string &name, GmatBase *obj,
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
CartY::CartY(const std::string &name, GmatBase *obj,
             const std::string &desc, const std::string &unit)
    : OrbitReal(name, "CartY", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// CartY(const CartY &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartY::CartY(const CartY &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const CartY& operator=(const CartY &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const CartY&
CartY::operator=(const CartY &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~CartY()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
CartY::~CartY()
{
}

//  //-------------------------------------
//  // Inherited methods from RealVar
//  //-------------------------------------

//  //------------------------------------------------------------------------------
//  // virtual Real EvaluateReal()
//  //------------------------------------------------------------------------------
//  /**
//   * @return newly evaluated value of parameter
//   */
//  //------------------------------------------------------------------------------
//  Real CartY::EvaluateReal()
//  {
//      Evaluate();
//      return mRealValue;
//  }


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
bool CartY::Evaluate()
{
    mRealValue = OrbitData::GetCartReal("CartY");
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}


//==============================================================================
//                              CartZ
//==============================================================================
/**
 * Implements Cartesian position Z class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartZ(const std::string &name, GmatBase *obj,
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
CartZ::CartZ(const std::string &name, GmatBase *obj,
             const std::string &desc, const std::string &unit)
    : OrbitReal(name, "CartZ", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// CartZ(const CartZ &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartZ::CartZ(const CartZ &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const CartZ& operator=(const CartZ &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const CartZ&
CartZ::operator=(const CartZ &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~CartZ()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
CartZ::~CartZ()
{
}

//  //-------------------------------------
//  // Inherited methods from RealVar
//  //-------------------------------------

//  //------------------------------------------------------------------------------
//  // virtual Real EvaluateReal()
//  //------------------------------------------------------------------------------
//  /**
//   * @return newly evaluated value of parameter
//   */
//  //------------------------------------------------------------------------------
//  Real CartZ::EvaluateReal()
//  {
//      Evaluate();
//      return mRealValue;
//  }

//--------------------------------------
// Inherited methods from Parameter
//--------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool CartZ::Evaluate()
{
    mRealValue = OrbitData::GetCartReal("CartZ");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}


//==============================================================================
//                              CartVx
//==============================================================================
/**
 * Implements Cartesian velocity X class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartVx(const std::string &name, GmatBase *obj,
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
CartVx::CartVx(const std::string &name, GmatBase *obj,
               const std::string &desc, const std::string &unit)
    : OrbitReal(name, "CartVx", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// CartVx(const CartVx &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartVx::CartVx(const CartVx &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const CartVx& operator=(const CartVx &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const CartVx&
CartVx::operator=(const CartVx &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~CartVx()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
CartVx::~CartVx()
{
}

//--------------------------------------
// Inherited methods from Parameter
//--------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool CartVx::Evaluate()
{
    mRealValue = OrbitData::GetCartReal("CartVx");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

//==============================================================================
//                              CartVy
//==============================================================================
/**
 * Implements Cartesian velocity Y class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartVy(const std::string &name, GmatBase *obj,
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
CartVy::CartVy(const std::string &name, GmatBase *obj,
               const std::string &desc, const std::string &unit)
    : OrbitReal(name, "CartVy", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// CartVy(const CartVy &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartVy::CartVy(const CartVy &param)
    : OrbitReal(param)
{
}

//------------------------------------------------------------------------------
// const CartVy& operator=(const CartVy &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const CartVy&
CartVy::operator=(const CartVy &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~CartVy()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
CartVy::~CartVy()
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
bool CartVy::Evaluate()
{
    mRealValue = OrbitData::GetCartReal("CartVy");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}


//==============================================================================
//                              CartVz
//==============================================================================
/**
 * Implements Cartesian velocity Z class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartVz(const std::string &name, GmatBase *obj,
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
CartVz::CartVz(const std::string &name, GmatBase *obj,
               const std::string &desc, const std::string &unit)
    : OrbitReal(name, "CartVz", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// CartVz(const CartVz &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartVz::CartVz(const CartVz &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const CartVz& operator=(const CartVz &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const CartVz&
CartVz::operator=(const CartVz &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~CartVz()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
CartVz::~CartVz()
{
}

//--------------------------------------
// Inherited methods from Parameter
//--------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool CartVz::Evaluate()
{
    mRealValue = OrbitData::GetCartReal("CartVz");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}


//==============================================================================
//                              RMag
//==============================================================================
/**
 * Implements Magnitude of Position.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// RMag(const std::string &name, GmatBase *obj,
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
RMag::RMag(const std::string &name, GmatBase *obj,
           const std::string &desc, const std::string &unit)
    : OrbitReal(name, "RMag", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// RMag(const RMag &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
RMag::RMag(const RMag &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const RMag& operator=(const RMag &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const RMag&
RMag::operator=(const RMag &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~RMag()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
RMag::~RMag()
{
}

//--------------------------------------
// Inherited methods from Parameter
//--------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool RMag::Evaluate()
{
    mRealValue = OrbitData::GetCartReal("RMag");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

//==============================================================================
//                              VMag
//==============================================================================
/**
 * Implements Magnitude of Velocity.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// VMag(const std::string &name, GmatBase *obj,
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
VMag::VMag(const std::string &name, GmatBase *obj,
           const std::string &desc, const std::string &unit)
    : OrbitReal(name, "VMag", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// VMag(const VMag &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
VMag::VMag(const VMag &copy)
    : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const VMag& operator=(const VMag &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const VMag&
VMag::operator=(const VMag &right)
{
    if (this != &right)
        OrbitReal::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~VMag()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
VMag::~VMag()
{
}

//--------------------------------------
// Inherited methods from Parameter
//--------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool VMag::Evaluate()
{
    mRealValue = OrbitData::GetCartReal("VMag");    
    
    if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
        return false;
    else
        return true;
}

//==============================================================================
//                              CartState
//==============================================================================
/**
 * Implements Cartesian state class.
 *   6 elements: CartX, CartY, CartZ, CartVx, CartVy, CartVz
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartState(const std::string &name, GmatBase *obj,
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
CartState::CartState(const std::string &name, GmatBase *obj,
                     const std::string &desc, const std::string &unit)
    : Rvec6Var(name, "CartState", SYSTEM_PARAM, obj, desc, unit, false)
{
    AddObject(obj);
}

//------------------------------------------------------------------------------
// CartState(const CartState &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartState::CartState(const CartState &copy)
    : Rvec6Var(copy)
{
}

//------------------------------------------------------------------------------
// const CartState& operator=(const CartState &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const CartState&
CartState::operator=(const CartState &right)
{
    if (this != &right)
        Rvec6Var::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~CartState()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
CartState::~CartState()
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
Rvector6 CartState::EvaluateRvector6()
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
Integer CartState::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* CartState::GetObject(const std::string &objTypeName)
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
bool CartState::SetObject(Gmat::ObjectType objType,
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
bool CartState::AddObject(GmatBase *obj)
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
bool CartState::Validate()
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
bool CartState::Evaluate()
{    
    mRvec6Value.Set(GetCartReal("CartX"),
                    GetCartReal("CartY"),
                    GetCartReal("CartZ"),
                    GetCartReal("CartVx"),
                    GetCartReal("CartVy"),
                    GetCartReal("CartVz"));

    return mRvec6Value.IsValid(ORBIT_REAL_UNDEFINED);
}

