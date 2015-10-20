//$Id$
//------------------------------------------------------------------------------
//                               ShadowState
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2014.05.06
//
/**
 * Base class definition for the ShadowState.
 */
//------------------------------------------------------------------------------


#ifndef ShadowState_hpp
#define ShadowState_hpp

#include "SolarSystem.hpp"
#include "SpacePoint.hpp"

class GMAT_API ShadowState
{
public:
   /// class methods
   ShadowState();
   ShadowState(const ShadowState &copy);
   ShadowState& operator=(const ShadowState &copy);

   virtual ~ShadowState();

   virtual void     SetSolarSystem(SolarSystem *ss);
   virtual Real     FindShadowState(bool &lit, bool &dark,
                                    const std::string &shadowModel, Real *state, Real *cbSun,
                                    Real *sunSat, Real *force, Real sunRad,
                                    Real bodyRad, Real psunrad);
protected:

   SolarSystem                  *solarSystem;
   CelestialBody                *sun;
   virtual Real   GetPercentSunInPenumbra(Real *state,
                              Real pcbrad,  Real psunrad, Real *force);
};

#endif   // ShadowState
