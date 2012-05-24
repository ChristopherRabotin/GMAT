//$Id: LineOfSight.cpp 1915 2011-11-16 19:09:31Z djconway@NDC $
//------------------------------------------------------------------------------
//                           LineOfSight
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
// Created: Nov 9, 2011
//
/**
 * Implementation of the line of sight event function
 */
//------------------------------------------------------------------------------


#include "LineOfSight.hpp"
#include "CelestialBody.hpp"
#include "SpacePoint.hpp"
#include "EventException.hpp"

//------------------------------------------------------------------------------
// LineOfSight()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
LineOfSight::LineOfSight() :
   EventFunction           ("LineOfSight"),
   secondary               (NULL),
   body                    (NULL),
   bodyRadius              (6378.1363),
   eqRadiusID              (-1)
{
}


//------------------------------------------------------------------------------
// ~LineOfSight()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
LineOfSight::~LineOfSight()
{
}


//------------------------------------------------------------------------------
// LineOfSight(const LineOfSight& los)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param los The LineOfSight providing parameters for this new one
 */
//------------------------------------------------------------------------------
LineOfSight::LineOfSight(const LineOfSight& los) :
   EventFunction           (los),
   secondary               (los.secondary),
   body                    (los.body),
   bodyRadius              (los.bodyRadius),
   eqRadiusID              (los.eqRadiusID)
{
}


//------------------------------------------------------------------------------
// LineOfSight & operator=(const LineOfSight& los)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param los The LineOfSight providing parameters for this one
 *
 * @return This LineOfSight event function, configured like los
 */
//------------------------------------------------------------------------------
LineOfSight & LineOfSight::operator=(const LineOfSight& los)
{
   if (this != &los)
   {
      EventFunction::operator=(los);

      secondary  = los.secondary;
      body       = los.body;
      bodyRadius = los.bodyRadius;
      eqRadiusID = los.eqRadiusID;
   }

   return *this;
}


//------------------------------------------------------------------------------
// void SetBody(CelestialBody *cb)
//------------------------------------------------------------------------------
/**
 * Sets the pointer to the potentially obstructing body
 *
 * @param cb The body
 */
//------------------------------------------------------------------------------
void LineOfSight::SetBody(CelestialBody *cb)
{
   body = cb;
}


//------------------------------------------------------------------------------
// void SetSecondary(SpacePoint *s)
//------------------------------------------------------------------------------
/**
 * Sets the pointer to the second end point in the LOS calculation
 *
 * @param s The endpoint
 */
//------------------------------------------------------------------------------
void LineOfSight::SetSecondary(SpacePoint *s)
{
   secondary = s;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepared the LOS for computation
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool LineOfSight::Initialize()
{
   bool retval = EventFunction::Initialize();

   if (retval)
   {
      if (secondary == NULL)
         throw EventException("Line of sight event function is missing the "
               "secondary endpoint");
      if (body == NULL)
         throw EventException("Line of sight event function is missing the "
               "potentially intervening body");

      if (eqRadiusID < 0)
         eqRadiusID = body->GetParameterID("EquatorialRadius");

      bodyRadius = body->GetRealParameter(eqRadiusID);

      #ifdef DEBUG_EVENTFUNCTIONS
         MessageInterface::ShowMessage("The %s's radius is %12.4lf; the %s's, "
               "%12.4lf\n", sol->GetName().c_str(), starRadius,
               body->GetName().c_str(), bodyRadius);
      #endif

      instanceName = primary->GetName();
      instanceName += " - ";
      instanceName += secondary->GetName();
   }

   return retval;
}


//------------------------------------------------------------------------------
// Real *Evaluate(GmatEpoch atEpoch, Real *forState)
//------------------------------------------------------------------------------
/**
 * Computes the LOS event function and its derivative
 *
 * @param atEpoch  (Optional) epoch of the computation; if omitted, epoch is
 *                 retrieved from the primary.
 * @param forState (Optional) State data of the computation; if omitted, state
 *                 is retrieved from the primary.
 *
 * @return The epoch, event function value, and event function time derivative
 */
//------------------------------------------------------------------------------
Real *LineOfSight::Evaluate(GmatEpoch atEpoch, Real *forState)
{
   #ifdef DEBUG_EVENTFUNCTIONS
      MessageInterface::ShowMessage("Evaluating Line of Sight\n   primary "
            "<%p> - secondary<%p>, body<%p>\n", primary, secondary, body);
   #endif

   // Only calculate if the reference objects are set
   if ((primary != NULL) && (body != NULL) && (secondary != NULL))
   {
      GmatEpoch now;
      Rvector3 primaryPos, primaryVel;
      Rvector3 secondaryPos, secondaryVel;

      if (atEpoch == -1.0)
      {
         now = primary->GetEpoch();
         primaryPos = primary->GetMJ2000Position(now);
         primaryVel = primary->GetMJ2000Velocity(now);
      }
      else
      {
         now = atEpoch;
         primaryPos.Set(forState[0], forState[1], forState[2]);
         primaryVel.Set(forState[3], forState[4], forState[5]);
      }

      eventData[0] = now;

      // Positions ignoring any light time corrections
      secondaryPos = secondary->GetMJ2000Position(now);
      secondaryVel = secondary->GetMJ2000Velocity(now);
      Rvector3 bodyPos = body->GetMJ2000Position(now);
      Rvector3 bodyVel = body->GetMJ2000Velocity(now);

      Rvector3 rp = primaryPos   - bodyPos;
      Rvector3 rs = secondaryPos - bodyPos;
      Rvector3 vp = primaryVel   - bodyVel;
      Rvector3 vs = secondaryVel - bodyVel;

      Real rpMag = rp.GetMagnitude();
      Real rsMag = rs.GetMagnitude();
      Real rpDotRs = rp * rs;

      Real theta1 = GmatMathUtil::ACos(bodyRadius / rpMag);
      Real theta2 = GmatMathUtil::ACos(bodyRadius / rsMag);
      Real theta  = GmatMathUtil::ACos(rpDotRs / (rsMag*rpMag));

      eventData[1] = theta1 + theta2 - theta;

      // Time derivative
      Real dTheta1, dTheta2, dTheta;
      Real rp3 = rpMag * rpMag * rpMag;
      Real rs3 = rsMag * rsMag * rsMag;
      Real rDotVp = rp * vp;
      Real rDotVs = rs * vs;

      dTheta1 = bodyRadius * (rDotVp) / (rp3 * GmatMathUtil::Sin(theta1));
      dTheta2 = bodyRadius * (rDotVs) / (rs3 * GmatMathUtil::Sin(theta2));
      dTheta  = ((vp*rs + rp*vs)/(rpMag * rsMag) -
                 (rpDotRs * rDotVp)/(rp3 * rsMag) -
                 (rpDotRs * rDotVs)/(rs3 * rpMag)) / GmatMathUtil::Sin(theta);

      eventData[2] = dTheta1 + dTheta2 - dTheta;
   }

   return eventData;
}
