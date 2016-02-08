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

#ifndef ThrustSegment_hpp
#define ThrustSegment_hpp

#include "GmatBase.hpp"

class ThrustSegment : public GmatBase
{
public:
   ThrustSegment(const std::string &name);
   virtual ~ThrustSegment();
   ThrustSegment(const ThrustSegment &ts);
   ThrustSegment& operator=(const ThrustSegment &ts);

   virtual GmatBase* Clone() const;

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS
};

#endif /* ThrustSegment_hpp */
