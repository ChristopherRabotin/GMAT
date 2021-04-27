//------------------------------------------------------------------------------
//                           SurfaceHeight
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 30, 2016
/**
 * Parameter used to detect contact on the central body in a force model
 */
//------------------------------------------------------------------------------

#include "SurfaceHeight.hpp"

//------------------------------------------------------------------------------
// SurfaceHeight(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name Name of the new object
 * @param obj Object used with the new one
 */
//------------------------------------------------------------------------------
SurfaceHeight::SurfaceHeight(const std::string &name, GmatBase *obj) :
   GravReal(name, "SurfaceHeight", obj, "Surface Contact", "",
          Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// ~SurfaceHeight()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SurfaceHeight::~SurfaceHeight()
{
}

//------------------------------------------------------------------------------
// SurfaceHeight(const SurfaceHeight& sc)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param sc The object copied
 */
//------------------------------------------------------------------------------
SurfaceHeight::SurfaceHeight(const SurfaceHeight& sc) :
   GravReal       (sc)
{
}

//------------------------------------------------------------------------------
// SurfaceHeight& operator=(const SurfaceHeight& sc)
//------------------------------------------------------------------------------
/**
 * Asignment operator
 *
 * @param sc The object setting data for this one
 *
 * @return This object, set to match sc
 */
//------------------------------------------------------------------------------
SurfaceHeight& SurfaceHeight::operator =(const SurfaceHeight& sc)
{
   if (this != &sc)
   {
      GravReal::operator=(sc);
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates the parameter
 *
 * @return true if the parameter was evaluated successfully
 */
//------------------------------------------------------------------------------
bool SurfaceHeight::Evaluate()
{
   mRealValue = GravData::GetGravReal("SurfaceHeight");

   if (mRealValue == GravData::GRAV_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Makes a copu of this object
 *
 * @return The copy
 */
//------------------------------------------------------------------------------
GmatBase* SurfaceHeight::Clone() const
{
   return new SurfaceHeight(*this);
}
