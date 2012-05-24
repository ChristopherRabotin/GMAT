//$Id: Penumbra.hpp 1914 2011-11-16 19:06:27Z djconway@NDC $
//------------------------------------------------------------------------------
//                           Penumbra
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
// Created: Sep 16, 2011
//
/**
 * Definition of the Penumbra EventFunction
 */
//------------------------------------------------------------------------------


#ifndef Penumbra_hpp
#define Penumbra_hpp

#include "EventLocatorDefs.hpp"
#include "Eclipse.hpp"

/**
 * Event function used to detect penumbral entry and exit
 */
class LOCATOR_API Penumbra: public Eclipse
{
public:
   Penumbra();
   virtual ~Penumbra();
   Penumbra(const Penumbra& u);
   Penumbra& operator=(const Penumbra& u);

   virtual bool Initialize();
   virtual Real* Evaluate(GmatEpoch atEpoch = -1.0, Real* forState = NULL);

};

#endif /* Penumbra_hpp */
