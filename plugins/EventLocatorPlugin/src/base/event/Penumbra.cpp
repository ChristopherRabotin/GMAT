//$Id: Penumbra.cpp 2124 2012-02-21 19:24:34Z djconway@NDC $
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
 * Implementation of the Penumbra EventFunction
 */
//------------------------------------------------------------------------------


#include "Penumbra.hpp"
#include "RealUtilities.hpp"
#include "Rvector3.hpp"
#include "Star.hpp"
#include "CelestialBody.hpp"
#include "SpaceObject.hpp"

#include <sstream>
#include "EventException.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_EVENTFUNCTIONS

//------------------------------------------------------------------------------
// Penumbra()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
Penumbra::Penumbra() :
   Eclipse("Penumbra")
{
}

//------------------------------------------------------------------------------
// ~Penumbra()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Penumbra::~Penumbra()
{
}

//------------------------------------------------------------------------------
// Penumbra(const Penumbra& u)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param u The instance being copied
 */
//------------------------------------------------------------------------------
Penumbra::Penumbra(const Penumbra& u) :
   Eclipse           (u)
{
}


//------------------------------------------------------------------------------
// Penumbra& operator=(const Penumbra& u)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param u The instance providing data for this one
 *
 * @return this instance, configured to match u
 */
//------------------------------------------------------------------------------
Penumbra& Penumbra::operator=(const Penumbra& u)
{
   if (this != &u)
   {
      Eclipse::operator=(u);
   }

   return *this;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the event function for use
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Penumbra::Initialize()
{
   bool retval = Eclipse::Initialize();
   return retval;
}


//------------------------------------------------------------------------------
// Real* Evaluate(GmatEpoch atEpoch, Real* forState)
//------------------------------------------------------------------------------
/**
 * Evaluates the Penumbral event function
 *
 * @param atEpoch (optional) The epoch for evaluation
 * @param forState (optional) The state for the evaluation
 *
 * @return The event function data
 */
//------------------------------------------------------------------------------
Real* Penumbra::Evaluate(GmatEpoch atEpoch, Real* forState)
{
   #ifdef DEBUG_EVENTFUNCTIONS
      MessageInterface::ShowMessage("Evaluating Penumbra\n   primary <%p>, "
            "body<%p>, sol <%p>\n", primary, body, sol);
   #endif

   #ifdef DEBUG_SOLVER_LOCATION
      if (forState != NULL)
         MessageInterface::ShowMessage("++Position: %.12lf [%lf %lf %lf]\n",
               atEpoch, forState[0], forState[1], forState[2]);
      else
      {
         Real t = primary->GetEpoch();
         Rvector3 pP = primary->GetMJ2000Position(t);
         MessageInterface::ShowMessage("--Position: %.12lf [%lf %lf %lf]\n", t,
               pP[0], pP[1], pP[2]);
      }
   #endif

   // Only calculate if the reference objects are set
   if ((primary != NULL) && (body != NULL) && (sol != NULL))
   {
      // Vector from the obscuring body to the target
      Rvector3 r;
      // Vector from the obscuring body to the star
      Rvector3 sunV;

      GmatEpoch now;
      Rvector3 primaryPos, primaryVel;

      if (atEpoch == -1.0)
      {
         now = primary->GetEpoch();
         primaryPos = primary->GetMJ2000Position(now);
         primaryVel = primary->GetMJ2000Velocity(now);
      }
      else
      {
         if (atEpoch < 0.0)
            throw EventException("Epoch passed into a Penumbra event is invalid.");
         if (forState == NULL)
            throw EventException("State passed into a Penumbra event is NULL.");

         now = atEpoch;

         // Offset position and velocity for the body-origin offset vector
         Rvector6 originState = origin->GetMJ2000State(now);
         primaryPos.Set(originState[0] + forState[0], 
            originState[1] + forState[1], originState[2] + forState[2]);
         primaryVel.Set(originState[3] + forState[3], 
            originState[4] + forState[4], originState[5] + forState[5]);
      }

      eventData[0] = now;

      Rvector3 starPos = sol->GetMJ2000Position(now);
      Rvector3 bodyPos = body->GetMJ2000Position(now);
      Rvector3 starVel = sol->GetMJ2000Velocity(now);
      Rvector3 bodyVel = body->GetMJ2000Velocity(now);

      r = primaryPos - bodyPos;
      sunV = starPos - bodyPos;

      Real rdotv, rmag, vmag;

      rdotv = r(0)*sunV(0) + r(1)*sunV(1) + r(2)*sunV(2);
      rmag = sqrt(r(0)*r(0) + r(1)*r(1) + r(2)*r(2));
      vmag = sqrt(sunV(0)*sunV(0) + sunV(1)*sunV(1) + sunV(2)*sunV(2));
      Real rmagvmag = rmag * vmag;

      Real pi, theta, phi, delta;
      pi = GmatMathConstants::PI;

      #ifdef DEBUG_EVENTFUNCTIONS
         MessageInterface::ShowMessage("Penumbra Calcs:\n   starRadius = %.12le, "
               "bodyRadius = %.12le, rmag = %.12le, vmag = %.12le\n",
               starRadius, bodyRadius, rmag, vmag);
      #endif

      if ((bodyRadius > rmag) || (starRadius > vmag))
      {
         std::stringstream errorstream;
         if (bodyRadius > rmag)
            errorstream << "ERROR: The position vector, magnitude " << rmag
                        << ", is inside of the occulting body (" 
                        << body->GetName() << ", radius "
                        << bodyRadius << ") when calculating Penumbra\n";
         if (starRadius > vmag)
            errorstream << "ERROR: The star-body vector, magnitude " << vmag
                        << ", is inside of the star (radius "
                        << starRadius << ") when calculating Penumbra\n";
         throw EventException(errorstream.str());
      }

      theta = GmatMathUtil::ACos(rdotv / (rmagvmag));
      phi   = GmatMathUtil::ASin((starRadius + bodyRadius) / vmag);
      delta = GmatMathUtil::ASin(bodyRadius/rmag);

      eventData[1] = pi - theta - phi - delta;

      #ifdef DEBUG_EVENTFUNCTIONS
         MessageInterface::ShowMessage("   pi = %.12lf, theta = %.12lf"
               ", phi = %.12lf, delta = %.12lf, value = %.12lf\n", pi, theta,
               phi, delta, eventData[1]);
      #endif

      // Evaluate and store the derivative data
      Real dtheta, dphi, ddelta;

      Rvector3 dSunV = starVel - bodyVel;
      Rvector3 dR    = primaryVel - bodyVel;

      Real sinTheta = GmatMathUtil::Sin(theta);
      if (sinTheta == 0.0)
         sinTheta = 1e-9;

      Real dRr = dR * r;
      Real rmag2 = rmag*rmag;
      Real vmag2 = vmag*vmag;

      dtheta = ( ((dSunV*r + sunV*dR) -
            rdotv * (dSunV*sunV) / (vmag2) -
            rdotv * (dRr) / (rmag2)) /
            (rmagvmag)) / sinTheta;

      Real cosPhi = GmatMathUtil::Cos(phi);
      if (cosPhi == 0.0)
         cosPhi = 1.0e-9;
      dphi = ((starRadius + bodyRadius) * (dSunV*sunV)) /
            (vmag*vmag2*cosPhi);

      Real cosDelta = GmatMathUtil::Cos(delta);
      if (cosDelta == 0.0)
         cosDelta = 1.0e-9;
      ddelta = -bodyRadius * dRr / (rmag2 * rmag * cosDelta);

      eventData[2] = -dtheta - dphi - ddelta;
   }
   else
   {
      char errordetail[128];
      sprintf(errordetail, "\n   primary <%p>, body<%p>, "
            "sol <%p>", primary, body, sol);
      std::string details = errordetail;
      throw EventException("Error calculating Penumbra; object undefined." +
            details);
   }

   return eventData;
}
