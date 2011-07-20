//$Id:  $
//------------------------------------------------------------------------------
//                            SpecialCelestialPoint
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2011.07.15
//
/**
 * Implementation of the SpecialCelestialPoint class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"
#include "SolarSystemException.hpp"
#include "CelestialBody.hpp"
#include "SpecialCelestialPoint.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
#include "AngleUtil.hpp"
#include "StringUtil.hpp"

//#define DEBUG_CELESTIAL_POINT
//#define DEBUG_EPHEM_SOURCE
//#define DEBUG_J2000_BODY


//---------------------------------
// static data
//---------------------------------
//const std::string
//SpecialCelestialPoint::PARAMETER_TEXT[SpecialCelestialPointParamCount - CelestialBodyParamCount] =
//{
//
//};
//
//const Gmat::ParameterType
//SpecialCelestialPoint::PARAMETER_TYPE[SpecialCelestialPointParamCount - CelestialBodyParamCount] =
//{
//
//};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  SpecialCelestialPoint(std::string name)
//------------------------------------------------------------------------------
/**
* This method creates an object of the SpecialCelestialPoint class
 * (default constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               point (default is "").
 */
//------------------------------------------------------------------------------
SpecialCelestialPoint::SpecialCelestialPoint(std::string name) :
CelestialBody     ("SpecialCelestialPoint",name)
{
   objectTypeNames.push_back("SpecialCelestialPoint");
   parameterCount = SpecialCelestialPointParamCount;

   bodyType            = Gmat::SPECIAL_CELESTIAL_POINT;
   bodyNumber          = -1;
   referenceBodyNumber = -1;

   // don't need central body, but set default one to avoid error conditions
   centralBodySet      = true;
   theCentralBodyName  = "Sun";  //    N/A

   // special celestial points have no mass of their own
   mu                  = 0.0;
}

//------------------------------------------------------------------------------
//  SpecialCelestialPoint(const SpecialCelestialPoint &copy)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpecialCelestialPoint class as a copy of the
 * specified SpecialCelestialPoint class (copy constructor).
 *
 * @param <copy> SpecialCelestialPoint object to copy.
 */
//------------------------------------------------------------------------------
SpecialCelestialPoint::SpecialCelestialPoint(const SpecialCelestialPoint &copy) :
CelestialBody (copy)
{
}

//------------------------------------------------------------------------------
//  SpecialCelestialPoint& operator= (const SpecialCelestialPoint& copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the SpecialCelestialPoint class.
 *
 * @param <copy> the SpecialCelestialPoint object whose data to assign to "this"
 *            solar system.
 *
 * @return "this" SpecialCelestialPoint with data of input SpecialCelestialPoint copy.
 */
//------------------------------------------------------------------------------
SpecialCelestialPoint& SpecialCelestialPoint::operator=(const SpecialCelestialPoint &copy)
{
   if (&copy == this)
      return *this;

   CelestialBody::operator=(copy);
   return *this;
}

//------------------------------------------------------------------------------
//  ~SpecialCelestialPoint()
//------------------------------------------------------------------------------
/**
 * Destructor for the SpecialCelestialPoint class.
 */
//------------------------------------------------------------------------------
SpecialCelestialPoint::~SpecialCelestialPoint()
{
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the SpecialCelestialPoint.
 *
 * @return clone of the SpecialCelestialPoint.
 *
 */
//------------------------------------------------------------------------------
GmatBase* SpecialCelestialPoint::Clone() const
{
   return (new SpecialCelestialPoint(*this));
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
void SpecialCelestialPoint::Copy(const GmatBase* orig)
{
   operator=(*((SpecialCelestialPoint *)(orig)));
}

bool SpecialCelestialPoint::NeedsOnlyMainSPK()
{
   if (instanceName == GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME)  return true;
   return false;
}


////------------------------------------------------------------------------------
//// void SetUpBody()
////------------------------------------------------------------------------------
//void SpecialCelestialPoint::SetUpBody()
//{
//   #ifdef DEBUG_J2000_BODY
//      MessageInterface::ShowMessage("Entering SetUpBody for %s, and j2000Body is %s\n",
//            instanceName.c_str(), j2000BodyName.c_str());
//      MessageInterface::ShowMessage("Does it require the J2000 body? %s\n",
//            (RequiresJ2000Body()? "YES!" : "Nope"));
//   #endif
//   // main thing to do for now is to make sure the central body is set
//   if (!theSolarSystem)
//   {
//      std::string errmsg = "Solar System not set for special celestial point \"";
//      errmsg += instanceName + "\"\n";
//      throw SolarSystemException(errmsg);
//   }
//   if (RequiresJ2000Body() && j2000Body == NULL)
//   {
//      j2000Body = theSolarSystem->GetBody(j2000BodyName);
//   }
//   return;
//}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
bool SpecialCelestialPoint::IsParameterReadOnly(const Integer id) const
{
   // We don't want to write out any of the parameters for this object
   return true;
}

//------------------------------------------------------------------------------
//  bool SetSource(Gmat::PosVelSource pvSrc)
//------------------------------------------------------------------------------
/**
 * This method sets the position/velocity source for the body.
 *
 * @param <pcSrc> position/velocity source for the body.
 *
 * @return flag indicating success of the method.
 *
 */
//------------------------------------------------------------------------------
bool SpecialCelestialPoint::SetSource(Gmat::PosVelSource pvSrc)
{
   #ifdef DEBUG_EPHEM_SOURCE
   MessageInterface::ShowMessage
      ("SpecialCelestialPoint::SetSource() <%p> %s, Setting source to %d(%s)\n", this,
       GetName().c_str(), pvSrc, Gmat::POS_VEL_SOURCE_STRINGS[pvSrc].c_str());
   #endif

   if (pvSrc == posVelSrc) return true;

   if (pvSrc == Gmat::TWO_BODY_PROPAGATION)
   {
      MessageInterface::ShowMessage(
            "Two-Body Propagation not available for built-in SolarSystemBarycenter.  Using DE405 as source.\n");
      posVelSrc = Gmat::DE405;
   }
   else
      posVelSrc           = pvSrc;
   return true;
}

////------------------------------------------------------------------------------
////  bool SetSourceFile(PlanetaryEphem *src)
////------------------------------------------------------------------------------
///**
// * This method sets the position/velocity source file for the body.
// *
// * @param <src> position/velocity source file for the body.
// *
// * @return flag indicating success of the method.
// *
// */
////------------------------------------------------------------------------------
//bool SpecialCelestialPoint::SetSourceFile(PlanetaryEphem *src)
//{
//   #ifdef DEBUG_EPHEM_SOURCE
//   MessageInterface::ShowMessage
//      ("SpecialCelestialPoint::SetSourceFile() <%p> %s, Setting source file to %p\n",
//       this, GetName().c_str(), src);
//   #endif
//
//   theSourceFile  = src;
//   sourceFilename = theSourceFile->GetName();
//   bodyNumber     = theSourceFile->GetBodyID(instanceName);
//   return true;
//}

bool SpecialCelestialPoint::SetAllowSpice(const bool allow)
{
   allowSpice = allow;
   return true;
}


//------------------------------------------------------------------------------
//  Rvector6 ComputeTwoBody(const A1Mjd &forTime)
//------------------------------------------------------------------------------
/**
 * This method computes the position and velocity at time forTime
 *
 * @return body state (position, velocity) at time forTime, with respect to
 *         the Earth.
 *
 */
//------------------------------------------------------------------------------
Rvector6 SpecialCelestialPoint::ComputeTwoBody(const A1Mjd &forTime)
{
   Rvector6 dummy;

   std::string errmsg = "Two-Body propagation not defined for ";
   errmsg += instanceName + "\n";
   throw SolarSystemException(errmsg);

   return dummy;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

