//$Id: Comet.cpp 5553 2008-06-03 16:46:30Z djcinsb $
//------------------------------------------------------------------------------
//                                  Comet
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2009.01.12
//
/**
 * Implementation of the Comet class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"
#include "SolarSystemException.hpp"
#include "CelestialBody.hpp"
#include "Comet.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
#include "AngleUtil.hpp"
#include "StringUtil.hpp"

//#define DEBUG_COMET 1

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Comet(std::string name)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Comet class
 * (default constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body (default is "").
 */
//------------------------------------------------------------------------------
Comet::Comet(std::string name) :
   CelestialBody     ("Comet",name)
{
   objectTypeNames.push_back("Comet"); 
   parameterCount      = CometParamCount;
   
   theCentralBodyName  = SolarSystem::SUN_NAME; 
   bodyType            = Gmat::COMET;
   bodyNumber          = -1;
   referenceBodyNumber = -1;
   
//   // defaults for now ...
//   Rmatrix s(5,5,
//         0.0, 0.0,             0.0,             0.0,             0.0,
//         0.0, 0.0,             0.0,             0.0,             0.0,
//         0.0, 0.0,             0.0,             0.0,             0.0,
//         0.0, 0.0,             0.0,             0.0,             0.0,
//         0.0, 0.0,             0.0,             0.0,             0.0);
//   Rmatrix c(5,5,
//         0.0, 0.0,             0.0,             0.0,             0.0,
//         0.0, 0.0,             0.0,             0.0,             0.0,
//         0.0, 0.0,             0.0,             0.0,             0.0,
//         0.0, 0.0,             0.0,             0.0,             0.0,
//         0.0, 0.0,             0.0,             0.0,             0.0);
//   sij = s;
//   cij = c;

   DeterminePotentialFileNameFromStartup();
   SaveAllAsDefault();
}

//------------------------------------------------------------------------------
//  Comet(std::string name, const std::string &cBody)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Comet class
 * (constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body.
 * @param <cBody> pointer to a central body.
 */
//------------------------------------------------------------------------------
Comet::Comet(std::string name, const std::string &cBody) :
   CelestialBody     ("Comet",name)
{
   objectTypeNames.push_back("Comet");
   parameterCount      = CometParamCount;

   theCentralBodyName  = cBody; 
   bodyType            = Gmat::COMET;
   bodyNumber          = -1;
   referenceBodyNumber = -1;

   DeterminePotentialFileNameFromStartup();
   SaveAllAsDefault();
}

//------------------------------------------------------------------------------
//  Comet(const Comet &copy)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Comet class as a copy of the
 * specified Comet class (copy constructor).
 *
 * @param <copy> Comet object to copy.
 */
//------------------------------------------------------------------------------
Comet::Comet(const Comet &copy) :
   CelestialBody (copy)
{
}

//------------------------------------------------------------------------------
//  Comet& operator= (const Comet& copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Comet class.
 *
 * @param <copy> the Comet object whose data to assign to "this"
 *            solar system.
 *
 * @return "this" Comet with data of input Comet copy.
 */
//------------------------------------------------------------------------------
Comet& Comet::operator=(const Comet &copy)
{
   if (&copy == this)
      return *this;

   CelestialBody::operator=(copy);
   return *this;
}

//------------------------------------------------------------------------------
//  ~Comet()
//------------------------------------------------------------------------------
/**
 * Destructor for the Comet class.
 */
//------------------------------------------------------------------------------
Comet::~Comet()
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Comet.
 *
 * @return clone of the Comet.
 */
//------------------------------------------------------------------------------
GmatBase* Comet::Clone() const
{
   return (new Comet(*this));
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Set this instance to match the one passed in.
 *
 * @param <orig> The object that is being copied.
 */
//---------------------------------------------------------------------------
void Comet::Copy(const GmatBase* orig)
{
   operator=(*((Comet *)(orig)));
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time
