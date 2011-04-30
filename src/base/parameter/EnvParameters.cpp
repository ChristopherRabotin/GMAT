//$Id$
//------------------------------------------------------------------------------
//                             File: EnvParameters.cpp
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
// Created: 2004/12/10
//
/**
 * Implements Environment related parameter classes.
 *   AtmosDensity
 */
//------------------------------------------------------------------------------

#include "EnvParameters.hpp"

//==============================================================================
//                              AtmosDensity
//==============================================================================
/**
 * Implements AtmosDensity parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// AtmosDensity(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
AtmosDensity::AtmosDensity(const std::string &name, GmatBase *obj)
   : EnvReal(name, "AtmosDensity", obj, "Atmospheric Density", "Kg/m^3",
             Gmat::SPACECRAFT, GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth"); //loj: 4/7/05 Added
}


//------------------------------------------------------------------------------
// AtmosDensity(const AtmosDensity &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
AtmosDensity::AtmosDensity(const AtmosDensity &copy)
   : EnvReal(copy)
{
}


//------------------------------------------------------------------------------
// const AtmosDensity& operator=(const AtmosDensity &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const AtmosDensity&
AtmosDensity::operator=(const AtmosDensity &right)
{
   if (this != &right)
      EnvReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~AtmosDensity()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
AtmosDensity::~AtmosDensity()
{
   //MessageInterface::ShowMessage("==> AtmosDensity::~AtmosDensity()\n");
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
bool AtmosDensity::Evaluate()
{
   mRealValue = EnvData::GetEnvReal("AtmosDensity");    
   
   if (mRealValue == EnvData::ENV_REAL_UNDEFINED)
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
GmatBase* AtmosDensity::Clone(void) const
{
   return new AtmosDensity(*this);
}
