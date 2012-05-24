//$Id: Antumbra.hpp 2092 2012-02-14 00:59:40Z djconway@NDC $
//------------------------------------------------------------------------------
//                           Antumbra
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
 * Definition of the Antumbra EventFunction
 */
//------------------------------------------------------------------------------


#ifndef Antumbra_hpp
#define Antumbra_hpp

#include "EventLocatorDefs.hpp"
#include "Eclipse.hpp"

/**
 * Event function used to detect antumbral entry and exit
 */
class LOCATOR_API Antumbra: public Eclipse
{
public:
   Antumbra();
   virtual ~Antumbra();
   Antumbra(const Antumbra& u);
   Antumbra& operator=(const Antumbra& u);

   virtual bool Initialize();
   virtual Real* Evaluate(GmatEpoch atEpoch = -1.0, Real* forState = NULL);
};

#endif /* Antumbra_hpp */
