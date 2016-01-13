//------------------------------------------------------------------------------
//                           BeginFileThrust
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * Toggle command used to tell GMAT to use a thrust data file
 */
//------------------------------------------------------------------------------


#include "BeginFileThrust.hpp"

BeginFileThrust::BeginFileThrust(const std::string &name) :
   GmatCommand             ("BeginFileThrust"),
   transientForces         (NULL),
   burnForce               (NULL)
{
   instanceName = name;
}

BeginFileThrust::~BeginFileThrust()
{
}

BeginFileThrust::BeginFileThrust(const BeginFileThrust& bft) :
   GmatCommand             (bft),
   transientForces         (NULL),
   burnForce               (NULL)
{
}

BeginFileThrust& BeginFileThrust::operator =(const BeginFileThrust& bft)
{
   if (this != &bft)
   {

   }

   return *this;
}

GmatBase* BeginFileThrust::Clone() const
{
   return new BeginFileThrust(*this);
}

bool BeginFileThrust::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = false;
   return retval;
}

void BeginFileThrust::SetTransientForces(std::vector<PhysicalModel*>* tf)
{
}

bool BeginFileThrust::Initialize()
{
   bool retval = false;
   return retval;
}

bool BeginFileThrust::Execute()
{
   bool retval = false;
   return retval;
}
