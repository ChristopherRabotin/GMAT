//$Id: Antumbra.cpp 2124 2012-02-21 19:24:34Z djconway@NDC $
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
 * Implementation of the Antumbra EventFunction
 */
//------------------------------------------------------------------------------


#include "Antumbra.hpp"
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
// Antumbra()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
Antumbra::Antumbra() :
   Eclipse("Antumbra")
{
}

//------------------------------------------------------------------------------
// ~Antumbra()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Antumbra::~Antumbra()
{
}

//------------------------------------------------------------------------------
// Antumbra(const Antumbra& u)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param u The instance being copied
 */
//------------------------------------------------------------------------------
Antumbra::Antumbra(const Antumbra& u) :
   Eclipse           (u)
{
}


//------------------------------------------------------------------------------
// Antumbra& operator=(const Antumbra& u)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param u The instance providing data for this one
 *
 * @return this instance, configured to match u
 */
//------------------------------------------------------------------------------
Antumbra& Antumbra::operator=(const Antumbra& u)
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
bool Antumbra::Initialize()
{
   return Eclipse::Initialize();
}


//------------------------------------------------------------------------------
// Real* Evaluate(GmatEpoch atEpoch, Real* forState)
//------------------------------------------------------------------------------
/**
 * Evaluates the Antumbral event function
 *
 * Antumbral event location follows the mathematics in Parker & Hughes, "A 
 * General Event Location Algorithm with Applications to Eclipse and Station 
 * Line-of-Sight," Proceeding of the AAS/AIAA Astrodynamics Specialist 
 * Conference, AAS 11-527, 2011.  
 *
 * Evaluation can follow one of two paths: 
 * (1) If the caller passes in a valid Modified Julian epoch and state, the 
 * input state and epoch data is used in the event function calculations.
 * (2) If the caller does not specify parameters, the default values trigger
 * calls into the target primary object, a Spacecraft, for the epoch and 
 * current location data.
 *
 * @param atEpoch (optional) The epoch for evaluation
 * @param forState (optional) The state for the evaluation
 *
 * @return The event function data
 */
//------------------------------------------------------------------------------
Real* Antumbra::Evaluate(GmatEpoch atEpoch, Real *forState)
{
   #ifdef DEBUG_EVENTFUNCTIONS
      MessageInterface::ShowMessage("Evaluating Antumbra\n   primary <%p>, body<%p>, "
            "sol <%p>\n", primary, body, sol);
   #endif

   // Only calculate if the reference objects are set
   if ((primary != NULL) && (body != NULL) && (sol != NULL))
   {
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
            throw EventException("Epoch passed into an Antumbra event is invalid.");
         if (forState == NULL)
            throw EventException("State passed into an Antumbra event is NULL.");
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

      // Vector from the obscuring body to the target
      Rvector3 r = primaryPos - bodyPos;
      // Vector from the obscuring body to the star
      Rvector3 sunV = starPos - bodyPos;

      Real rdotv, rmag, vmag;

      rdotv = r(0)*sunV(0) + r(1)*sunV(1) + r(2)*sunV(2);

      // rmag is the distance from the obscuring body to the target
      rmag = sqrt(r(0)*r(0) + r(1)*r(1) + r(2)*r(2));
      // vmag is the distance from the obscuring body to the star
      vmag = sqrt(sunV(0)*sunV(0) + sunV(1)*sunV(1) + sunV(2)*sunV(2));

      Real rmagvmag = rmag * vmag;

      Real pi, theta, alpha, eta; 
      pi = GmatMathConstants::PI;

      #ifdef DEBUG_EVENTFUNCTIONS
         MessageInterface::ShowMessage("Antumbra Calcs:\n   starRadius = "
               "%.12le, bodyRadius = %.12le, rmag = %.12le, vmag = %.12le\n",
               starRadius, bodyRadius, rmag, vmag);
      #endif

      if ((bodyRadius > rmag) || (starRadius > vmag))
      {
         std::stringstream errorstream;
         if (bodyRadius > rmag)
            errorstream << "ERROR: The position vector, magnitude " << rmag
                        << ", is inside of the occulting body (radius "
                        << bodyRadius << ") when calculating Antumbra\n";
         if (starRadius > vmag)
            errorstream << "ERROR: The star-body vector, magnitude " << vmag
                        << ", is inside of the star (radius "
                        << starRadius << ") when calculating Antumbra\n";
         throw EventException(errorstream.str());
      }

      theta = GmatMathUtil::ACos(rdotv / (rmagvmag));
      alpha = GmatMathUtil::ASin((starRadius - bodyRadius) / vmag);
      eta   = GmatMathUtil::ASin(bodyRadius/rmag);

      eventData[1] = pi - theta - alpha + eta;

      #ifdef DEBUG_EVENTFUNCTIONS
         MessageInterface::ShowMessage("   pi = %.12lf, theta = %.12lf"
               ", alpha = %.12lf, eta = %.12lf, value = %.12lf\n", pi, theta,
               alpha, eta, eventData[1]);
      #endif

      // Evaluate and store the derivative data
      Real dtheta, dalpha, deta;

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

      Real cosAlpha = GmatMathUtil::Cos(alpha);
      if (cosAlpha == 0.0)
         cosAlpha = 1.0e-9;
      dalpha = ((starRadius - bodyRadius) * (dSunV*sunV)) /
            (vmag*vmag2*cosAlpha);

      Real cosEta = GmatMathUtil::Cos(eta);
      if (cosEta == 0.0)
         cosEta = 1.0e-9;
      deta = -bodyRadius * dRr / (rmag2 * rmag * cosEta);

      eventData[2] = - dtheta - dalpha + deta;
   }
   else
   {
      char errordetail[128];
      sprintf(errordetail, "\n   primary <%p>, body<%p>, "
            "sol <%p>", primary, body, sol);
      std::string details = errordetail;
      throw EventException("Error calculating Antumbra; object undefined." +
            details);
   }

   return eventData;
}
