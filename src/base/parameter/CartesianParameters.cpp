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
#include "ColorTypes.hpp"

//#define DEBUG_CARTESIAN_PARAM 1

//1/24/05 loj: Removed AddRefObject(obj) because it is also added in OrbitReal

//==============================================================================
//                              CartX
//==============================================================================
/**
 * Implements Cartesian position X class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartX(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
CartX::CartX(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "X", obj, "Cartesian X", "Km", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mColor = GmatColor::RED32;
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
// methods inherited from Parameter
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


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* CartX::Clone(void) const
{
   return new CartX(*this);
}


//==============================================================================
//                              CartY
//==============================================================================
/**
 * Implements Cartesian position Y class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartY(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
CartY::CartY(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Y", obj, "Cartesian Y", "Km", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mColor = GmatColor::GREEN32;
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
// methods inherited from Parameter
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


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* CartY::Clone(void) const
{
   return new CartY(*this);
}


//==============================================================================
//                              CartZ
//==============================================================================
/**
 * Implements Cartesian position Z class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
CartZ::CartZ(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Z", obj, "Cartesian Z", "Km", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mColor = GmatColor::BLUE32;
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
// methods inherited from Parameter
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


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* CartZ::Clone(void) const
{
   return new CartZ(*this);
}


//==============================================================================
//                              CartVx
//==============================================================================
/**
 * Implements Cartesian velocity X class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartVx(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
CartVx::CartVx(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "VX", obj, "Cartesian VX", "Km/s", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mColor = GmatColor::RED32;
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
// methods inherited from Parameter
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


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* CartVx::Clone(void) const
{
   return new CartVx(*this);
}


//==============================================================================
//                              CartVy
//==============================================================================
/**
 * Implements Cartesian velocity Y class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartVy(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
CartVy::CartVy(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "VY", obj, "Cartesian Y", "Km/s", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mColor = GmatColor::GREEN32;
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
// methods inherited from Parameter
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


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* CartVy::Clone(void) const
{
   return new CartVy(*this);
}


//==============================================================================
//                              CartVz
//==============================================================================
/**
 * Implements Cartesian velocity Z class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CartVz(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
CartVz::CartVz(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "VZ", obj, "Cartesian VZ", "Km/s", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mColor = GmatColor::BLUE32;
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
// methods inherited from Parameter
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


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* CartVz::Clone(void) const
{
   return new CartVz(*this);
}


//==============================================================================
//                              CartState
//==============================================================================
/**
 * Implements Cartesian state class.
 *   6 elements: CartX, CartY, CartZ, CartVx, CartVy, CartVz
 */
//------------------------------------------------------------------------------


//loj: 2/16/05 changed the name "CartState" to "Cartesian"
//------------------------------------------------------------------------------
// CartState(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
CartState::CartState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "Cartesian", obj, "Cartesian State Vector",
                "Km/s", GmatParam::COORD_SYS)
{
   // Parameter member data
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mIsPlottable = false;
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
   : OrbitRvec6(copy)
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
      OrbitRvec6::operator=(right);

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


//--------------------------------------
// methods inherited from Parameter
//--------------------------------------

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
   mRvec6Value = GetCartState();
   
   #if DEBUG_CARTESIAN_PARAM
   MessageInterface::ShowMessage
      ("CartState::Evaluate() mRvec6Value =\n%s\n",
       mRvec6Value.ToString().c_str());
   #endif
   
   return mRvec6Value.IsValid(ORBIT_REAL_UNDEFINED);
}


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* CartState::Clone(void) const
{
   return new CartState(*this);
}

