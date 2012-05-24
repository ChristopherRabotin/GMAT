//$Id: Eclipse.hpp 2182 2012-03-01 22:20:38Z djconway@NDC $
//------------------------------------------------------------------------------
//                           Eclipse
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
 * Definition of the eclipse event function
 */
//------------------------------------------------------------------------------


#ifndef Eclipse_hpp
#define Eclipse_hpp

#include "EventLocatorDefs.hpp"
#include "EventFunction.hpp"

class CelestialBody;

/**
 * The Eclipse EventFunction base class
 *
 * This class is the base class for the Penumbra, Antumbra, and Umbra
 * EventFunctions
 */
class LOCATOR_API Eclipse : public EventFunction
{
public:
   Eclipse(const std::string &typeStr);
   virtual ~Eclipse();
   Eclipse(const Eclipse& ef);
   Eclipse&  operator=(const Eclipse& ef);

   bool SetSol(CelestialBody *sun);
   bool SetBody(CelestialBody *bod);
   virtual bool Initialize();

protected:
   /// The body casting shadows for these calculations.
   CelestialBody *body;
   /// The light source (i.e., the Sun)
   CelestialBody *sol;
   /// The radius of the star.
   Real starRadius;
   /// The radius of the occluding body.
   Real bodyRadius;

   /// ID for equatorial radius of the sun and body
   Integer eqRadiusID;
};

#endif /* Eclipse_hpp */
