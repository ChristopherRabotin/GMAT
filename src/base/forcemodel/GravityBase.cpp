//$Id$
//------------------------------------------------------------------------------
//                                   GravityBase
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, Task Order 28
//
// Author:  Darrel J. Conway, Thinking Systems, Inc.
// Created: Oct 2, 2012
//
/**
 * Implementation for a base class for sophisticated gravity models.
 */
//------------------------------------------------------------------------------

#include "GravityBase.hpp"

GravityBase::GravityBase(const std::string &typeStr, const std::string &name) :
   PhysicalModel           (Gmat::PHYSICAL_MODEL, typeStr, name)
{
   objectTypeNames.push_back("GravityBase");
}

GravityBase::~GravityBase()
{
}

GravityBase::GravityBase(const GravityBase& gb) :
   PhysicalModel           (gb)
{
}

GravityBase& GravityBase::operator=(const GravityBase& gb)
{
   if (this != &gb)
   {
      PhysicalModel::operator=(gb);

      // Currently nothing to do here; fill this in when refactoring occurs
   }

   return *this;
}
