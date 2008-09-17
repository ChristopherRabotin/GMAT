//$Header$
//------------------------------------------------------------------------------
//                            File: BurnParameters.cpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/05/27
//
/**
 * Implements Burn related parameter classes.
 *   DeltaVDir1, DeltaVDir2, DeltaVDir3
 */
//------------------------------------------------------------------------------

#include "BurnParameters.hpp"
#include "ColorTypes.hpp"

//#define DEBUG_BURN_PARAM 1


//==============================================================================
//                              DeltaVDir1
//==============================================================================
/**
 * Implements DeltaV first compoment.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DeltaVDir1(const std::string &name, const std::string &typeName, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <typeName> name of the parameter type
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
DeltaVDir1::DeltaVDir1(const std::string &name, const std::string &typeName,
                       GmatBase *obj)
   : BurnReal(name, typeName, Gmat::IMPULSIVE_BURN, obj,
              "ImpulsiveBurn Element1", "Km/s", GmatParam::NO_DEP, true)
{
   mColor = GmatColor::RED32;
}


//------------------------------------------------------------------------------
// DeltaVDir1(const DeltaVDir1 &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
DeltaVDir1::DeltaVDir1(const DeltaVDir1 &copy)
   : BurnReal(copy)
{
}


//------------------------------------------------------------------------------
// DeltaVDir1& operator=(const DeltaVDir1 &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
DeltaVDir1& DeltaVDir1::operator=(const DeltaVDir1 &right)
{
   if (this != &right)
      BurnReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DeltaVDir1()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
DeltaVDir1::~DeltaVDir1()
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
bool DeltaVDir1::Evaluate()
{
   mRealValue = BurnData::GetBurnReal(ELEMENT1);

   #ifdef DEBUG_BURN_EVAL
   MessageInterface::ShowMessage
      ("DeltaVDir1::Evaluate() mRealValue=%f\n", mRealValue);
   #endif
   
   if (mRealValue == BurnData::BURN_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 *
 */
//------------------------------------------------------------------------------
void DeltaVDir1::SetReal(Real val)
{
   BurnData::SetBurnReal(ELEMENT1, val);
   RealVar::SetReal(val);
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
GmatBase* DeltaVDir1::Clone(void) const
{
   return new DeltaVDir1(*this);
}


//==============================================================================
//                              DeltaVDir2
//==============================================================================
/**
 * Implements DeltaV second compoment.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DeltaVDir2(const std::string &name, const std::string &typeName, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <typeName> name of the parameter type
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
DeltaVDir2::DeltaVDir2(const std::string &name, const std::string &typeName,
                       GmatBase *obj)
   : BurnReal(name, typeName, Gmat::IMPULSIVE_BURN, obj,
              "Impulsive Burn Element2", "Km/s", GmatParam::NO_DEP, true)
{
   mColor = GmatColor::GREEN32;
}


//------------------------------------------------------------------------------
// DeltaVDir2(const DeltaVDir2 &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
DeltaVDir2::DeltaVDir2(const DeltaVDir2 &copy)
   : BurnReal(copy)
{
}


//------------------------------------------------------------------------------
// DeltaVDir2& operator=(const DeltaVDir2 &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
DeltaVDir2& DeltaVDir2::operator=(const DeltaVDir2 &right)
{
   if (this != &right)
      BurnReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DeltaVDir2()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
DeltaVDir2::~DeltaVDir2()
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
bool DeltaVDir2::Evaluate()
{
   mRealValue = BurnData::GetBurnReal(ELEMENT2);
    
   #ifdef DEBUG_BURN_EVAL
   MessageInterface::ShowMessage
      ("DeltaVDir2::Evaluate() mRealValue=%f\n", mRealValue);
   #endif
   
   if (mRealValue == BurnData::BURN_REAL_UNDEFINED)
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
GmatBase* DeltaVDir2::Clone(void) const
{
   return new DeltaVDir2(*this);
}


//==============================================================================
//                              DeltaVDir3
//==============================================================================
/**
 * Implements DeltaV third compoment.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DeltaVDir3(const std::string &name, const std::string &typeName, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <typeName> name of the parameter type
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
DeltaVDir3::DeltaVDir3(const std::string &name, const std::string &typeName,
                       GmatBase *obj)
   : BurnReal(name, typeName, Gmat::IMPULSIVE_BURN, obj,
              "Impulsive Burn Element3", "Km/s", GmatParam::NO_DEP, true)
{
   mColor = GmatColor::BLUE32;
}


//------------------------------------------------------------------------------
// DeltaVDir3(const DeltaVDir3 &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
DeltaVDir3::DeltaVDir3(const DeltaVDir3 &copy)
   : BurnReal(copy)
{
}

//------------------------------------------------------------------------------
// DeltaVDir3& operator=(const DeltaVDir3 &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
DeltaVDir3& DeltaVDir3::operator=(const DeltaVDir3 &right)
{
   if (this != &right)
      BurnReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DeltaVDir3()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
DeltaVDir3::~DeltaVDir3()
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
bool DeltaVDir3::Evaluate()
{
   mRealValue = BurnData::GetBurnReal(ELEMENT3);    

   #ifdef DEBUG_BURN_EVAL
   MessageInterface::ShowMessage
      ("DeltaVDir3::Evaluate() mRealValue=%f\n", mRealValue);
   #endif
   
   if (mRealValue == BurnData::BURN_REAL_UNDEFINED)
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
GmatBase* DeltaVDir3::Clone(void) const
{
   return new DeltaVDir3(*this);
}


