//$Id$
//------------------------------------------------------------------------------
//                            File: BurnParameters.cpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/05/27
//
/**
 * Implements Burn related Parameter classes.
 *   ImpBurnElements
 */
//------------------------------------------------------------------------------

#include "BurnParameters.hpp"
#include "ColorTypes.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BURN_PARAM 1


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS

//==============================================================================
//                              ImpBurnElements
//==============================================================================
/**
 * Implements ImpulsiveBurn element Parameters.
 *    Element1, Element2, Element3
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ImpBurnElements(const std::string &type, const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <type> type of the parameter
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
ImpBurnElements::ImpBurnElements(const std::string &type, const std::string &name,
                                 GmatBase *obj)
   : BurnReal(name, type, Gmat::IMPULSIVE_BURN, obj, "Impulsive Burn " + type,
              "Km/s", GmatParam::COORD_SYS, true)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
   
   // Write deprecated message for each once per GMAT session
   static bool writeDeprecatedMsg = true;

   if ((type == "V") || (type == "N") || (type == "B"))
   {
      if  (writeDeprecatedMsg)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"V\", \"N\", \"B\" are deprecated and will be "
             "removed from a future build; please use \"Element1\", \"Element2\", \"Element3\" "
             "instead.\n");
         writeDeprecatedMsg = false;
      }
   }

   if (type == "Element1" || type == "V")
      mElementId = ELEMENT1;
   else if (type == "Element2" || type == "N")
      mElementId = ELEMENT2;
   else if (type == "Element3" || type == "B")
      mElementId = ELEMENT3;
   else
      mElementId = -1;
}


//------------------------------------------------------------------------------
// ImpBurnElements(const ImpBurnElements &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ImpBurnElements::ImpBurnElements(const ImpBurnElements &copy)
   : BurnReal(copy)
{
   mElementId = copy.mElementId;
}


//------------------------------------------------------------------------------
// ImpBurnElements& operator=(const ImpBurnElements &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ImpBurnElements& ImpBurnElements::operator=(const ImpBurnElements &right)
{
   if (this != &right)
   {
      BurnReal::operator=(right);
      mElementId = right.mElementId;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~ImpBurnElements()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ImpBurnElements::~ImpBurnElements()
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
bool ImpBurnElements::Evaluate()
{
   mRealValue = BurnData::GetReal(mElementId);

   #ifdef DEBUG_BURN_EVAL
   MessageInterface::ShowMessage
      ("ImpBurnElements::Evaluate() mRealValue=%f\n", mRealValue);
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
void ImpBurnElements::SetReal(Real val)
{
   BurnData::SetReal(mElementId, val);
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
GmatBase* ImpBurnElements::Clone(void) const
{
   return new ImpBurnElements(*this);
}


