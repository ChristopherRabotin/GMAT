//------------------------------------------------------------------------------
//                           SurfaceContact
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
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

#include "SurfaceContact.hpp"

//------------------------------------------------------------------------------
// SurfaceContact(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name Name of the new object
 * @param obj Object used with the new one
 */
//------------------------------------------------------------------------------
SurfaceContact::SurfaceContact(const std::string &name, GmatBase *obj) :
   GravReal(name, "SurfaceContact", obj, "Surface Contact", "",
          Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// ~SurfaceContact()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SurfaceContact::~SurfaceContact()
{
}

//------------------------------------------------------------------------------
// SurfaceContact(const SurfaceContact& sc)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param sc The object copied
 */
//------------------------------------------------------------------------------
SurfaceContact::SurfaceContact(const SurfaceContact& sc) :
   GravReal       (sc)
{
}

//------------------------------------------------------------------------------
// SurfaceContact& operator=(const SurfaceContact& sc)
//------------------------------------------------------------------------------
/**
 * Asignment operator
 *
 * @param sc The object setting data for this one
 *
 * @return This object, set to match sc
 */
//------------------------------------------------------------------------------
SurfaceContact& SurfaceContact::operator =(const SurfaceContact& sc)
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
bool SurfaceContact::Evaluate()
{
   mRealValue = GravData::GetGravReal("SurfaceContact");

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
GmatBase* SurfaceContact::Clone() const
{
   return new SurfaceContact(*this);
}
