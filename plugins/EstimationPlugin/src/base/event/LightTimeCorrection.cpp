//$Id: LightTimeCorrection.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         LightTimeCorrection
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/04
//
/**
 * Event code used to find the epoch of one endpoint on a light signal path,
 * given the other endpoint.
 */
//------------------------------------------------------------------------------


#include "LightTimeCorrection.hpp"
#include "GmatConstants.hpp"
#include "EventException.hpp"
#include "SpaceObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_LIGHTTIME
//#define DEBUG_ITERATION


//-----------------------------------------------------------------------------
// LightTimeCorrection(const std::string &name)
//-----------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name for this Event
 */
//-----------------------------------------------------------------------------
LightTimeCorrection::LightTimeCorrection(const std::string &name) :
   Event       			("LightTimeCorrection", name),
   fixedParticipant		(0),
   range                (10000.0),
   oldRange             (0.0)
{
   // Need to convert to km/s here
   lightSpeed = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M;
}


//-----------------------------------------------------------------------------
// ~LightTimeCorrection()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
LightTimeCorrection::~LightTimeCorrection()
{
}


//-----------------------------------------------------------------------------
// LightTimeCorrection(const LightTimeCorrection& ltc)
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ltc The LightTimeCorrection Event that supplies data for this new
 *            instance
 */
//-----------------------------------------------------------------------------
LightTimeCorrection::LightTimeCorrection(const LightTimeCorrection& ltc) :
   Event       			(ltc),
   lightSpeed  			(ltc.lightSpeed),
   fixedParticipant		(ltc.fixedParticipant),
   range                (ltc.range),
   oldRange             (0.0)
{
}


//-----------------------------------------------------------------------------
// LightTimeCorrection& operator=(const LightTimeCorrection& ltc)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ltc The LightTimeCorrection Event that supplies data for this
 *            instance
 *
 * @return This instance, configured to match ltc
 */
//-----------------------------------------------------------------------------
LightTimeCorrection& LightTimeCorrection::operator=(
      const LightTimeCorrection& ltc)
{
   if (this != &ltc)
   {
      Event::operator=(ltc);
      lightSpeed       = ltc.lightSpeed;
      fixedParticipant = ltc.fixedParticipant;
      stepDirection    = ltc.stepDirection;
      range            = ltc.range;
      oldRange         = 0.0;
   }

   return *this;
}


//-----------------------------------------------------------------------------
// GmatBase Clone() const
//-----------------------------------------------------------------------------
/**
 * Creates a new LightTimeCorrection Event from a GmatBase pointer to this one
 *
 * @return A copy of this Event
 */
//-----------------------------------------------------------------------------
GmatBase *LightTimeCorrection::Clone() const
{
   return new LightTimeCorrection(*this);
}


bool LightTimeCorrection::Initialize()
{
   bool retval = Event::Initialize();
   oldRange = 0.0;
   return retval;
}


//-----------------------------------------------------------------------------
// Real Evaluate()
//-----------------------------------------------------------------------------
/**
 * Calculates the value of the LightTimeCorrection event function.
 *
 * This method finds the difference between the distance light travels in the
 * estimated transit time and the physical distance between the participants.
 * If that difference is less than the tolerance of the Event, current status is
 * set to LOCATED.
 *
 * @return The value of the event function
 */
//-----------------------------------------------------------------------------
Real LightTimeCorrection::Evaluate()
{
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Evaluating light time correction; "
            "status is %d\n", status);
   #endif

   Real vals;

   oldRange = range;
   range = CalculateRange();

   // Alternative approach: Difference the range from light distance
   // // Event function: F = c[T_r - T_t^i] - |R_S(T_t^i) - R_G(T_r)|
   // vals = lightSpeed*GmatMathUtil::Abs(fixedTime-varTime) - CalculateRange();

   // Event function: F = |Range - oldRange|
   vals = GmatMathUtil::Abs(range - oldRange);  // Range diff in km

   #ifdef DEBUG_ITERATION
      MessageInterface::ShowMessage("Light time data: Range = %.12lf, "
            "oldRange = %.12lf, Difference = %.12lf %s tolerance = %.12lf\n",
            range, oldRange, GmatMathUtil::Abs(range - oldRange),
            (GmatMathUtil::Abs(vals) < tolerance ? "<" : ">"), tolerance);
   #endif

   // Located if magnitude of the event function is smaller than tolerance
   if (GmatMathUtil::Abs(vals) < tolerance)
   {
      status = LOCATED;
      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("Event located; "
               "status is %d\n", status);
      #endif


//      Real epoch;
//      if (participants[0]->IsOfType(Gmat::SPACEOBJECT))
//         epoch = ((SpaceObject*)participants[0])->GetEpoch();
//      else
//         epoch = ((SpaceObject*)participants[1])->GetEpoch();
   }

   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage(
            "   fixedTime = %.12le\n   varTime   = %.12le\n"
            "   range     = %.12le", fixedTime, varTime, CalculateRange());
      MessageInterface::ShowMessage(
            "   Light time correction event function = %.12le\n", vals);
   #endif

   return vals;
}


//------------------------------------------------------------------------------
// void FixState(GmatBase *obj, bool LockState)
//------------------------------------------------------------------------------
/**
 * Buffers a participant state
 *
 * @param obj The participant that gets buffered
 * @param lockState false if just identifying the fixed object, true if the
 *                  state should also be captured
 */
//------------------------------------------------------------------------------
void LightTimeCorrection::FixState(GmatBase *obj, bool lockState)
{
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Fixing light time participant %s\n",
            obj->GetName().c_str());
   #endif

   fixedParticipant = -1;

   if (obj == participants[0])
      fixedParticipant = 0;
   if (obj == participants[1])
      fixedParticipant = 1;

   if (fixedParticipant == -1)
      throw EventException("Attempting to fix 2-way range state for a"
            " non-participant");

   if (lockState)
   {
      if (participants[0]->IsOfType(Gmat::SPACEOBJECT))
         fixedEpoch = ((SpaceObject*)participants[0])->GetEpoch();
      else
         fixedEpoch = ((SpaceObject*)participants[1])->GetEpoch();

      positionBuffer = ((SpacePoint*)obj)->GetMJ2000Position(fixedEpoch);

      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("   At epoch %.12lf, position = %s",
               fixedEpoch, positionBuffer.ToString().c_str());
      #endif

      // Now store the other data that needs to be preserved
      StoreParticipantData(fixedParticipant, (SpacePoint *)obj, fixedEpoch);
   }
}

//------------------------------------------------------------------------------
// void FixState()
//------------------------------------------------------------------------------
/**
 * Preserves state data for later use
 *
 * The state preserved here is set for an object that has already been
 * identified by a call to FixState(GmatBase* obj, bool lockState).
 */
//------------------------------------------------------------------------------
void LightTimeCorrection::FixState()
{
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Fixing state of participant %d\n",
            fixedParticipant);
   #endif

   if (participants[0]->IsOfType(Gmat::SPACEOBJECT))
      fixedEpoch = ((SpaceObject*)participants[0])->GetEpoch();
   else
      fixedEpoch = ((SpaceObject*)participants[1])->GetEpoch();

   positionBuffer = ((SpacePoint*)participants[fixedParticipant])->
         GetMJ2000Position(fixedEpoch);

   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("   At epoch %.12lf, position = %s",
            fixedEpoch, positionBuffer.ToString().c_str());
   #endif

   // Now store the other data that needs to be preserved
   StoreParticipantData(fixedParticipant,
         ((SpacePoint*)participants[fixedParticipant]), fixedEpoch);
}


//-----------------------------------------------------------------------------
// Real CalculateRange()
//-----------------------------------------------------------------------------
/**
 * Calculates the range between two participants in the event.
 *
 * This method calculates the range vector pointing between the fixed object and
 * the propagating object, as represented in the current data of the member
 * attributes.
 *
 * @note The current method does not include corrections due to motion around
 *       the solar system barycenter.
 *
 * @return The range
 */
//-----------------------------------------------------------------------------
Real LightTimeCorrection::CalculateRange()
{
   Real range = 0.0;
   Rvector3 r1, r2, rangeVec;

   GmatEpoch t1 = -1.0, t2 = -1.0;

   if (participants.size() > 1)
   {
      if (participants[0]->IsOfType(Gmat::SPACEOBJECT))
         t1 = ((SpaceObject*)(participants[0]))->GetEpoch();
      if (participants[1]->IsOfType(Gmat::SPACEOBJECT))
         t2 = ((SpaceObject*)(participants[1]))->GetEpoch();
      else if (!participants[0]->IsOfType(Gmat::SPACEOBJECT))
         throw EventException("Neither participant in the " + typeName +
               " event is a SpaceObject, so the range cannot be calculated.");

      if (t1 == -1.0)
         t1 = t2;
      if (t2 == -1.0)
         t2 = t1;

      r1 = (fixedParticipant == 0 ? positionBuffer :
            ((SpacePoint*)participants[0])->GetMJ2000Position(t1));
      r2 = (fixedParticipant == 1 ? positionBuffer :
            ((SpacePoint*)participants[1])->GetMJ2000Position(t2));

      rangeVec = r2 - r1;
      range = rangeVec.GetMagnitude();

      // Store the rest of the data used in the other measurement calculations
      // for the moving participant.  The fixed participant was stored earlier.
      Integer index = 1 - fixedParticipant;
      StoreParticipantData(index, (SpacePoint *)participants[index],
            (index == 0 ? t1 : t2));
   }
   else
      throw EventException("Light time correction event does not have 2 "
            "participants");

   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Light time range estimate for %s at epoch "
            "[%.12lf]:\n", instanceName.c_str(), t1);
      MessageInterface::ShowMessage("   %s position: %s\n",
            participants[0]->GetName().c_str(), r1.ToString().c_str());
      MessageInterface::ShowMessage("   %s position: %s\n",
            participants[1]->GetName().c_str(), r2.ToString().c_str());
      MessageInterface::ShowMessage("   Range estimate:  %.12lf\n", range);
   #endif

	return range;
}


//-----------------------------------------------------------------------------
// void CalculateTimestepEstimate()
//-----------------------------------------------------------------------------
/**
 * Estimates the time step and epoch at which the event function is zero
 */
//-----------------------------------------------------------------------------
void LightTimeCorrection::CalculateTimestepEstimate()
{
   Real range = CalculateRange();

   // Current assumption: time tag is at receive time
   Real dt = range / lightSpeed;
   varTime = stepDirection * dt;

   estimatedEpoch = fixedTime + varTime;
}
