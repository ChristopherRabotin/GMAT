//$Header$
//------------------------------------------------------------------------------
//                             File: BplaneParameters.cpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/06/15
//
/**
 * Declares B-Plane related parameter classes.
 *   BdotT, BdotR
 */
//------------------------------------------------------------------------------
#include "BplaneParameters.hpp"
#include "MessageInterface.hpp"

//==============================================================================
//                              BdotT
//==============================================================================
/**
 * Implements Velocity at Apoapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BdotT(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
BdotT::BdotT(const std::string &name, GmatBase *obj)
   : BplaneReal(name, "BdotT", obj, "B dot T", "", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
}


//------------------------------------------------------------------------------
// BdotT(const BdotT &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BdotT::BdotT(const BdotT &copy)
   : BplaneReal(copy)
{
}


//------------------------------------------------------------------------------
// const BdotT& operator=(const BdotT &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const BdotT&
BdotT::operator=(const BdotT &right)
{
   if (this != &right)
      BplaneReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BdotT()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BdotT::~BdotT()
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
bool BdotT::Evaluate()
{
   try
   {
      //mRealValue = BplaneData::GetBplaneReal("BdotT");
      mRealValue = BplaneData::GetBplaneReal(B_DOT_T);
   }
   catch (BaseException &e)
   {
      mRealValue = REAL_PARAMETER_UNDEFINED;
      MessageInterface::ShowMessage
         (e.GetMessage() + ",\n     so setting BdotT parameter value to: " +
          GmatRealUtil::ToString(REAL_PARAMETER_UNDEFINED) + "\n");
   }
   
   if (mRealValue == REAL_PARAMETER_UNDEFINED)
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
GmatBase* BdotT::Clone(void) const
{
   return new BdotT(*this);
}


//==============================================================================
//                              BdotR
//==============================================================================
/**
 * Implements Velocity at Periapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BdotR(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
BdotR::BdotR(const std::string &name, GmatBase *obj)
   : BplaneReal(name, "BdotR", obj, "B dot R", "", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
}


//------------------------------------------------------------------------------
// BdotR(const BdotR &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BdotR::BdotR(const BdotR &copy)
   : BplaneReal(copy)
{
}


//------------------------------------------------------------------------------
// const BdotR& operator=(const BdotR &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const BdotR&
BdotR::operator=(const BdotR &right)
{
   if (this != &right)
      BplaneReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BdotR()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BdotR::~BdotR()
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
bool BdotR::Evaluate()
{
   try
   {
      //mRealValue = BplaneData::GetBplaneReal("BdotR");
      mRealValue = BplaneData::GetBplaneReal(B_DOT_R);
   }
   catch (BaseException &e)
   {
      mRealValue = REAL_PARAMETER_UNDEFINED;
      MessageInterface::ShowMessage
         (e.GetMessage() + ",\n     so settting BdotR parameter value to: " +
          GmatRealUtil::ToString(REAL_PARAMETER_UNDEFINED) + "\n");
   }
   
   if (mRealValue == REAL_PARAMETER_UNDEFINED)
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
GmatBase* BdotR::Clone(void) const
{
   return new BdotR(*this);
}

