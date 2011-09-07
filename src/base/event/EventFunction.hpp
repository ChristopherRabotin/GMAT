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


#ifndef EventFunction_hpp
#define EventFunction_hpp

#include "GmatBase.hpp"


class SpaceObject;


class GMAT_API EventFunction //: public GmatBase
{
public:
   EventFunction();
   virtual ~EventFunction();
   EventFunction(const EventFunction& ef);
   EventFunction& operator=(const EventFunction& ef);

   virtual Real* Evaluate() = 0;

protected:
   /// The value of the event function when last evaluated
   Real              value;
   /// The value of the event function derivative when last evaluated
   Real              derivative;
   /// SpaceObject that plays the role of “target” in the event computations.
   SpaceObject       *primary;
};

#endif /* EventFunction_hpp */
