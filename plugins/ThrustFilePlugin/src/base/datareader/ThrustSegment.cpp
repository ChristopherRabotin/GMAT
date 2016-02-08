//$Id$
//------------------------------------------------------------------------------
//                           ThrustSegment
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 5, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#include "ThrustSegment.hpp"

ThrustSegment::ThrustSegment(const std::string &name) :
   GmatBase       (Gmat::INTERFACE, "ThrustSegment", name)
{
}

ThrustSegment::~ThrustSegment()
{
}

ThrustSegment::ThrustSegment(const ThrustSegment& ts) :
   GmatBase          (ts)
{

}

ThrustSegment& ThrustSegment::operator =(const ThrustSegment& ts)
{
   if (this != &ts)
   {
      GmatBase::operator =(ts);
   }

   return *this;
}

GmatBase* ThrustSegment::Clone() const
{
   return new ThrustSegment(*this);
}
