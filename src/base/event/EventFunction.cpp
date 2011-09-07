//$Id$
//------------------------------------------------------------------------------
//                           EventFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 6, 2011
//
/**
 * Definition of the ...
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "EventFunction.hpp"

EventFunction::EventFunction() :
   value             (999.999999),
   derivative        (999.9999999),
   primary           (NULL)
{
}

EventFunction::~EventFunction()
{
}

EventFunction::EventFunction(const EventFunction& ef)
{

}

EventFunction& EventFunction::operator=(const EventFunction& ef)
{
   if (this != &ef)
   {

   }

   return *this;
}
