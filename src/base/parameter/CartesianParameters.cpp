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
// CartX& operator=(const CartX &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartX& CartX::operator=(const CartX &right)
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
// CartY& operator=(const CartY &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartY& CartY::operator=(const CartY &right)
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
// CartZ& operator=(const CartZ &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartZ& CartZ::operator=(const CartZ &right)
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
// CartVx& operator=(const CartVx &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartVx& CartVx::operator=(const CartVx &right)
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
// CartVy& operator=(const CartVy &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartVy& CartVy::operator=(const CartVy &right)
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
// CartVz& operator=(const CartVz &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartVz& CartVz::operator=(const CartVz &right)
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
// CartState& operator=(const CartState &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CartState& CartState::operator=(const CartState &right)
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

