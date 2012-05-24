//$Id: Umbra.hpp 1914 2011-11-16 19:06:27Z djconway@NDC $
//------------------------------------------------------------------------------
//                           Umbra
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
// Created: Aug 30, 2011
//
/**
 * Definition of the Umbra EventFunction
 */
//------------------------------------------------------------------------------


#ifndef Umbra_hpp
#define Umbra_hpp

#include "EventLocatorDefs.hpp"
#include "Eclipse.hpp"

/**
 * Event function used to detect penumbral entry and exit
 */
class LOCATOR_API Umbra: public Eclipse
{
public:
   Umbra();
   virtual ~Umbra();
   Umbra(const Umbra& u);
   Umbra& operator=(const Umbra& u);

   virtual bool Initialize();
   virtual Real* Evaluate(GmatEpoch atEpoch = -1.0, Real* forState = NULL);

};

#endif /* Umbra_hpp */
