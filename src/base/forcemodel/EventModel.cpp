//$Id$
//------------------------------------------------------------------------------
//                           EventModel
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
// Created: Sep 7, 2011
//
/**
 * Definition of the ...
 */
//------------------------------------------------------------------------------


#include "EventModel.hpp"

EventModel::EventModel(const std::string &typeStr, const std::string &nomme) :
   PhysicalModel        (Gmat::PHYSICAL_MODEL, typeStr, nomme)
{
}

EventModel::~EventModel()
{
}

EventModel::EventModel(const EventModel & em) :
   PhysicalModel        (em)
{
}

EventModel& EventModel::operator=(const EventModel & em)
{
   if (this != &em)
   {
      PhysicalModel::operator=(em);
   }

   return *this;
}


bool EventModel::Initialize()
{
   bool retval = false;

   return retval;
}

bool EventModel::GetDerivatives(Real *state, Real dt, Integer order, const Integer id)
{
   bool retval = false;

   return retval;
}


