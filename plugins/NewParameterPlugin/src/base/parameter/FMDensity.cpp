//$Id$
//------------------------------------------------------------------------------
//                              FMDensity
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Darrel Conway
// Created: 2013/05/29
//
/**
 * Implements the Atmospheric Density Parameter class.
 */
//------------------------------------------------------------------------------

#include "FMDensity.hpp"

//==============================================================================
//                              FMDensity
//==============================================================================
/**
 * Implements FMDensity parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FMDensity(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
FMDensity::FMDensity(const std::string &name, GmatBase *obj) :
   OdeReal(name, "AtmosDensity", obj, "Atmospheric Density", "Kg/m^3",
             Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}


//------------------------------------------------------------------------------
// FMDensity(const FMDensity &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
FMDensity::FMDensity(const FMDensity &copy)
   : OdeReal(copy)
{
}


//------------------------------------------------------------------------------
// const FMDensity& operator=(const FMDensity &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const FMDensity&
FMDensity::operator=(const FMDensity &right)
{
   if (this != &right)
      OdeReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FMDensity()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
FMDensity::~FMDensity()
{
   //MessageInterface::ShowMessage("==> FMDensity::~FMDensity()\n");
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
bool FMDensity::Evaluate()
{
   mRealValue = OdeData::GetOdeReal("FMDensity");
   
   if (mRealValue == OdeData::ODE_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* FMDensity::Clone() const
{
   return new FMDensity(*this);
}
