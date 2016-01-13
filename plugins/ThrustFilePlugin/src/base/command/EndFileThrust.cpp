//------------------------------------------------------------------------------
//                           EndFileThrust
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
 * Toggle command used to tell GMAT to stop using a thrust data file
 */
//------------------------------------------------------------------------------


#include "EndFileThrust.hpp"

EndFileThrust::EndFileThrust(const std::string &name) :
   GmatCommand             ("EndFileThrust"),
   burnForce               (NULL),
   transientForces         (NULL)
{
   instanceName = name;
}

EndFileThrust::~EndFileThrust()
{
}

EndFileThrust::EndFileThrust(const EndFileThrust& eft) :
   GmatCommand             (eft),
   burnForce               (NULL),
   transientForces         (NULL)
{
}

EndFileThrust& EndFileThrust::operator =(const EndFileThrust& eft)
{
   if (this != &eft)
   {

   }

   return *this;
}

GmatBase* EndFileThrust::Clone() const
{
   return new EndFileThrust(*this);
}

bool EndFileThrust::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = false;
   return retval;
}


void EndFileThrust::SetTransientForces(std::vector<PhysicalModel*>* tf)
{
}

bool EndFileThrust::Initialize()
{
   bool retval = false;
   return retval;
}

bool EndFileThrust::Execute()
{
   bool retval = false;
   return retval;
}
