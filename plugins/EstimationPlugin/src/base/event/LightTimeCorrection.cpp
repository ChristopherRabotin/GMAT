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
#include "RealUtilities.hpp"

//#define DEBUG_LIGHTTIME
//#define DEBUG_ITERATION
//#define DEBUG_CALCULATE_RANGE

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
   relativityCorrection = 0.0;								// made changes by TUAN NGUYEN
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
   relativityCorrection = 0.0;						// made changes by TUAN NGUYEN
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
      MessageInterface::ShowMessage("Start LightTimeCorrection::Evaluate(): Evaluating light time correction; "
            "status is %d\n", status);
   #endif

   Real vals;

   Real tolerance1 = 1.0e-10;

   oldRange = range;
   range = CalculateRange();

   // Alternative approach: Difference the range from light distance
   // // Event function: F = c[T_r - T_t^i] - |R_S(T_t^i) - R_G(T_r)|
   // vals = lightSpeed*GmatMathUtil::Abs(fixedTime-varTime) - CalculateRange();

   // Event function: F = |Range - oldRange|
   vals = GmatMathUtil::Abs(range - oldRange);  // Range diff in km

   #ifdef DEBUG_ITERATION
      MessageInterface::ShowMessage("Light time data: Range = %.12lf km, "
            "oldRange = %.12lf km, Difference = %.12lf km %s tolerance = %.12lf km\n",
            range, oldRange, GmatMathUtil::Abs(range - oldRange),
            (GmatMathUtil::Abs(vals) < tolerance ? "<" : ">"), tolerance);
   #endif

   // Located if magnitude of the event function is smaller than tolerance
   if (GmatMathUtil::Abs(vals) < max(tolerance, tolerance1*range))
   {
      status = LOCATED;
      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("Event located; "
               "status is %d\n", status);
      #endif
   }

   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage(
            "   fixedTime = %.15lf s\n   varTime   = %.15lf s\n"
            "   range     = %.12lf km\n", fixedTime, varTime, range);
      MessageInterface::ShowMessage(
            "   Light time correction event function = %.12lf km\n", vals);
   #endif

   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("End LightTimeCorrection::Evaluate(): Evaluating light time correction\n");
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
      MessageInterface::ShowMessage("Starting LightTimeCorrection(%s)::FixState(obj, lockState):  Fixing light time participant %s with lockState = %s\n",
            instanceName.c_str(), obj->GetName().c_str(), (lockState? "TRUE": "FALSE"));
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
   
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Exitting LightTimeCorrection::FixState(obj, lockState)\n"); 
   #endif
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
      MessageInterface::ShowMessage("Starting LightTimeCorrection(%s)::FixState(): Fixing state of participant %d\n",
            instanceName.c_str(), fixedParticipant);
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

   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Exitting LightTimeCorrection::FixState(): Fixing state of participant %d\n",
            fixedParticipant);
   #endif
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
 * @return The range (unit: km)
 */
//-----------------------------------------------------------------------------
Real LightTimeCorrection::CalculateRange()
{
   #ifdef DEBUG_LIGHTTIME
	  MessageInterface::ShowMessage("Start LightTimeCorrection(%s)::CalculateRange()\n", instanceName.c_str());
   #endif

   Real range = 0.0;
   Rvector3 r1, r2, rangeVec;
//   Rvector3 v2;

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
	  t1 = ((fixedParticipant == 0) ? fixedEpoch : t1);
	  t2 = ((fixedParticipant == 1) ? fixedEpoch : t2);
	  
      #ifdef DEBUG_CALCULATE_RANGE
	     MessageInterface::ShowMessage("fixedParticipant = %d\n", fixedParticipant);
      #endif

	  // Specify vector from Participant.j2000Body position to participant position: 
	  r1 = (fixedParticipant == 0 ? positionBuffer :
            ((SpacePoint*)participants[0])->GetMJ2000Position(t1));
      r2 = (fixedParticipant == 1 ? positionBuffer :
            ((SpacePoint*)participants[1])->GetMJ2000Position(t2));
	  
	  // Specify vector from solar system bary center to particpant position:						// made changes by TUAN NGUYEN
      if (solarSystem == NULL)
         throw EventException("Error: no solar system is set to solarSystem variable of "+ GetName() + " object\n");

	  SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");			// made changes by TUAN NGUYEN
	  std::string cbName1 = ((SpacePoint*)participants[0])->GetJ2000BodyName();						// made changes by TUAN NGUYEN
	  CelestialBody* cb1 = solarSystem->GetBody(cbName1);											// made changes by TUAN NGUYEN
	  Rvector3 ssb2cb1 = cb1->GetMJ2000Position(t1) - ssb->GetMJ2000Position(t1);					// made changes by TUAN NGUYEN
	  std::string cbName2 = ((SpacePoint*)participants[1])->GetJ2000BodyName();						// made changes by TUAN NGUYEN
	  CelestialBody* cb2 = solarSystem->GetBody(cbName2);											// made changes by TUAN NGUYEN
      Rvector3 ssb2cb2 = cb2->GetMJ2000Position(t2) - ssb->GetMJ2000Position(t2);					// made changes by TUAN NGUYEN

	  Rvector3 r1B = ssb2cb1 + r1;																	// made changes by TUAN NGUYEN
	  Rvector3 r2B = ssb2cb2 + r2;																	// made changes by TUAN NGUYEN

      #ifdef DEBUG_CALCULATE_RANGE
	     MessageInterface::ShowMessage("Participant1: Epoch = %.15lf CB  Pos: (%.12lf  %.12lf  %.12lf)km\n", t1, r1.GetElement(0), r1.GetElement(1), r1.GetElement(2));
	     MessageInterface::ShowMessage("Participant2: Epoch = %.15lf CB  Pos: (%.12lf  %.12lf  %.12lf)km\n", t2, r2.GetElement(0), r2.GetElement(1), r2.GetElement(2));
	     MessageInterface::ShowMessage("Participant1: Epoch = %.15lf SSB Pos: (%.12lf  %.12lf  %.12lf)km\n", t1, r1B.GetElement(0), r1B.GetElement(1), r1B.GetElement(2));
	     MessageInterface::ShowMessage("Participant2: Epoch = %.15lf SSB Pos: (%.12lf  %.12lf  %.12lf)km\n", t2, r2B.GetElement(0), r2B.GetElement(1), r2B.GetElement(2));
		 MessageInterface::ShowMessage("  ssb2cb1 (%.12lf  %.12lf  %.12lf)km\n", ssb2cb1.GetElement(0), ssb2cb1.GetElement(1), ssb2cb1.GetElement(2));
		 MessageInterface::ShowMessage("  ssb2cb2 (%.12lf  %.12lf  %.12lf)km\n", ssb2cb2.GetElement(0), ssb2cb2.GetElement(1), ssb2cb2.GetElement(2));
      #endif
      
      rangeVec = r2B - r1B;																			// made changes by TUAN NGUYEN
	  
      precisionRange = rangeVec.GetMagnitude();														// made changes by TUAN NGUYEN

	  relativityCorrection = 0.0;																	// made changes by TUAN NGUYEN
	  if (useRelativityCorrection)																	// made changes by TUAN NGUYEN
	     relativityCorrection = RelativityCorrection(r1B, r2B, t1, t2);									// made changes by TUAN NGUYEN
	  range = precisionRange + relativityCorrection;					// relativity correction was added into the range		// made changes by TUAN NGUYEN
	  
//	  MessageInterface::ShowMessage("############# Event %s: precision range = %.12lf km,      relativity correction = %.12lf km\n", GetName().c_str(), precisionRange, relativityCorrection);

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
      Rvector3 v2 = ((SpacePoint*)participants[1])->GetMJ2000Velocity(t2);
      MessageInterface::ShowMessage("Light time range estimate for %s at epoch "
            "[t1 = %.12lf,  t2 = %.12lf]:\n", instanceName.c_str(), t1, t2);
      MessageInterface::ShowMessage("   %s position: %s\n",
            participants[0]->GetName().c_str(), r1.ToString().c_str());
      MessageInterface::ShowMessage("   %s position: %s\n",
            participants[1]->GetName().c_str(), r2.ToString().c_str());
      MessageInterface::ShowMessage("   %s vilocity: %s\n",
            participants[1]->GetName().c_str(), v2.ToString().c_str());
      MessageInterface::ShowMessage("   Range estimate:  %.12lf\n", range);
	  MessageInterface::ShowMessage("   fixedParticipant = %d\n",fixedParticipant);
   #endif

   #ifdef DEBUG_LIGHTTIME
	  MessageInterface::ShowMessage("Exit LightTimeCorrection::CalculateRange()\n");
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
   #ifdef DEBUG_LIGHTTIME
	  MessageInterface::ShowMessage("Start LightTimeCorrection(%s)::CalculateTimestepEstimate()\n",instanceName.c_str());
   #endif

   Real range = CalculateRange();

   // Current assumption: time tag is at receive time
   Real dt = range / lightSpeed;			// unit of dt is second (Notice that: it is not a Julian day)
   varTime = stepDirection * dt;			// unit of varTime is second

   estimatedEpoch = fixedTime + varTime;	// unit of fixedTime and estimatedEpoch are second.

   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("varTime        = %.15lf\n", varTime);
	  MessageInterface::ShowMessage("fixedTime      = %.15lf\n", fixedTime);
	  MessageInterface::ShowMessage("estimatedEpoch = %.15lf\n", estimatedEpoch);
	  MessageInterface::ShowMessage("fixedEpoch = %.15lf\n", fixedEpoch);
	  MessageInterface::ShowMessage("End LightTimeCorrection::CalculateTimestepEstimate()\n");
   #endif

}


//--------------------------------------------------------------------------------------------
// Real LightTimeCorrection::RelativityCorrection(Rvector3 r1B, Rvector3 r2B, Real t1, Real t2)
//--------------------------------------------------------------------------------------------
/**
 * Calculate relativity correction
 *
 * @param t1     Epoch used by the first participant (unit: A1Mid)
 * @param t2     Epoch used by the second participant (unit: A1Mid)
 * @param r1B    Position of the first participant in SSB coordinate system at epoch t1 (unit:km)
 * @param r2B    Position of the second participant in SSB coordinate system at epoch t2 (unit:km)
 *
 * return range correction (unit: km)
 */
//--------------------------------------------------------------------------------------------
Real LightTimeCorrection::RelativityCorrection(Rvector3 r1B, Rvector3 r2B, Real t1, Real t2)
{
	Rvector3 P1, P2, BP1, BP2, r1, r2, r12;
	Real r1Mag, r2Mag, r12Mag;

	SpecialCelestialPoint* barycenter = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
	Rvector3 B1 = barycenter->GetMJ2000Position(t1);
	Rvector3 B2 = barycenter->GetMJ2000Position(t2);

	StringArray planetList = solarSystem->GetBodiesInUse();
	CelestialBody* sun = solarSystem->GetBody("Sun");
	Real gammar = 1.0;

	Real relCorr = 0.0;
	for(Integer i = 0; i < planetList.size(); ++i)
	{
		CelestialBody* planet = solarSystem->GetBody(planetList[i]);
		Real planetMu = planet->GetRealParameter(planet->GetParameterID("Mu"));

		P1 = planet->GetMJ2000Position(t1);				// Planet's position in SSB coordinate system at time t1
		P2 = planet->GetMJ2000Position(t2);				// Planet's position in SSB coordinate system at time t2
		BP1 = P1 - B1;									// vector from SSB to planet in  SSB coordinate system at time t1
		BP2 = P2 - B2;									// vector from SSB to planet in  SSB coordinate system at time t2

		r1 = r1B - BP1;									// position of the first participant in the local inertial coordinate system of the planet at time t1
		r2 = r2B - BP2;									// position of the first participant in the local inertial coordinate system of the planet at time t2
		r12 = r2 - r1;									// range vector in the local inertial coordinate system of the planet 
		r1Mag = r1.Norm();								// unit: km
		r2Mag = r2.Norm();								// unit: km
		r12Mag = r12.Norm();							// unit: km

		Real c = lightSpeed;							// unit: km/s
		Real term1 = (1+gammar)*planetMu/c/c;			// unit: km
		Real correction;
		if (planet == sun)
			correction = term1*GmatMathUtil::Ln((r1Mag + r2Mag + r12Mag + term1)/(r1Mag + r2Mag - r12Mag + term1));
		else
			correction = term1*GmatMathUtil::Ln((r1Mag + r2Mag + r12Mag)/(r1Mag + r2Mag - r12Mag));
		relCorr += correction;
	}

	relativityCorrection = relCorr;						// unit: Km

	return relCorr;
}


// Get relativity coorection (in Km)						// made changes by TUAN NGUYEN
Real LightTimeCorrection::GetRelativityCorrection()
{
	return relativityCorrection;
}


// Get light trip range	(in Km)								// made changes by TUAN NGUYEN
Real LightTimeCorrection::GetLightTripRange()
{
	return precisionRange;
}



//--------------------------------------------------------------------------------------------
// Rvector3 ConvertLocalGCToSBB(Real tdbMJD, Rvector3 posGEO, CelestialBody* j2kBody)			// made changes by TUAN NGUYEN
//--------------------------------------------------------------------------------------------
/**
 * Convert the local geocentric frame to solar system bary center frame					
 *
 * @param tdbMJD     A1MJD time
 * @param posGEO     position in the local geocentric frame (unit: km)
 * @param j2kBody    the body used as the origin of local geocentric frame
 *
 * return position after converted to solar system bary center frame (unit: km)
*/
//-------------------------------------------------------------------------------------------
Rvector3 LightTimeCorrection::ConvertLocalGEOCToSSB(Real tA1MJD, Rvector3 posGEO, CelestialBody* j2kBody)
{
   // Define constants
   Real Ltilde     = 1.480827e-8;            
   Real Gamma      = 1;
   Real c          = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM * GmatMathConstants::M_TO_KM;			// light speed (unit: km/s)
   Real j2kBody_mu = j2kBody->GetRealParameter(j2kBody->GetParameterID("Mu"));							// unit:km^3/s^2;        mu = 398600.4415 for Earth

   // Compute the transformation
   // Note that: function GetMJ2000State, GetMJ2000Position, and GetMJ2000Velocity accepts A1MJD epoch as input. 
   // It converts to absolute time ET and specify state of celetial body based on ET time    
   Rvector3 j2kBody_Vel = j2kBody->GetMJ2000Velocity(tA1MJD);							//[earthPos,earthVel] = GetEarthState(tdbMJD);

   Real satRadius       = posGEO.Norm();													// unit: km
   Real earthPotential  = j2kBody_mu/satRadius;												// unit: km^2/s^2
   Real posFactor       = 1 - Ltilde - Gamma*earthPotential/c/c;							// unit: no unit
   Rvector3 posSSB      = posFactor*posGEO - 0.5/c/c*(j2kBody_Vel*posGEO)*j2kBody_Vel;		// unit: km

   return posSSB;
}



/// Calculate ET - TAI at a ground station on Earth:										// made changes by TUAN NGUYEN
Real LightTimeCorrection::CalculateETMinusTAI(Real tA1MJD, GmatBase* participant)
{
   // ET minus TAI calculation.  Eq. 2-23 on p. 2-14 of Moyer

   // Specify celestial bodies and special celestial points:
   CelestialBody* sun = solarSystem->GetBody("Sun");
   CelestialBody* earth = solarSystem->GetBody("Earth");
   CelestialBody* mars = solarSystem->GetBody("Mars");
   CelestialBody* jupiter= solarSystem->GetBody("Jupiter");
   CelestialBody* saturn = solarSystem->GetBody("Saturn");
   SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
   SpecialCelestialPoint* emb = solarSystem->GetSpecialPoint("EarthMoonBarycenter");

   //Define constants
   Real c = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM * GmatMathConstants::M_TO_KM;			// light speed (unit: km/s)
   Real muEarth   = earth->GetRealParameter(earth->GetParameterID("Mu"));						// mu = 398600.4415 for Earth
   Real muSun     = sun->GetRealParameter(earth->GetParameterID("Mu"));							// mu = 132712440017.99 for Sun
   Real muJupiter = jupiter->GetRealParameter(earth->GetParameterID("Mu"));						// mu = 126712767.8578 for Jupiter
   Real muSaturn  = saturn->GetRealParameter(earth->GetParameterID("Mu"));						// mu = 37940626.061137 for Saturn
   Real muMars    = mars->GetRealParameter(earth->GetParameterID("Mu"));						// mu = 42828.314258067 for Mars

   //     1 -> Mercury          8 -> Neptune
   //     2 -> Venus            9 -> Pluto
   //     3 -> Earth           10 -> Moon
   //     4 -> Mars            11 -> Sun
   //     5 -> Jupiter         12 -> Solar system barycenter
   //     6 -> Saturn          13 -> Earth-Moon barycenter
   //     7 -> Uranus          14 -> Nutations (longitude and obliquity)

   // Specify position and velocity of celestial bodies and special celestial points
   Rvector3 sunPos = sun->GetMJ2000Position(tA1MJD);
   Rvector3 sunVel = sun->GetMJ2000Velocity(tA1MJD);
   Rvector3 earthPos = earth->GetMJ2000Position(tA1MJD);
   Rvector3 earthVel = earth->GetMJ2000Velocity(tA1MJD);
   Rvector3 marsPos = mars->GetMJ2000Position(tA1MJD);
   Rvector3 marsVel = mars->GetMJ2000Velocity(tA1MJD);
   Rvector3 jupiterPos = jupiter->GetMJ2000Position(tA1MJD);
   Rvector3 jupiterVel = jupiter->GetMJ2000Velocity(tA1MJD);
   Rvector3 saturnPos = saturn->GetMJ2000Position(tA1MJD);
   Rvector3 saturnVel = saturn->GetMJ2000Velocity(tA1MJD);

   Rvector3 emPos = emb->GetMJ2000Position(tA1MJD);
   Rvector3 emVel = emb->GetMJ2000Velocity(tA1MJD);

   //[EM_wrt_Sun_Pos, EM_wrt_Sun_Vel] = pleph(tdbMJD+2430000,13,11,1);
   //[EM_wrt_SSB_Pos, EM_wrt_SSB_Vel] = pleph(tdbMJD+2430000,13,12,1);
   //[Jup_wrt_Sun_Pos, Jup_wrt_Sun_Vel] = pleph(tdbMJD+2430000,5,11,1);
   //[Sat_wrt_Sun_Pos, Sat_wrt_Sun_Vel] = pleph(tdbMJD+2430000,6,11,1);
   //[Sun_wrt_SSB_Pos, Sun_wrt_SSB_Vel] = pleph(tdbMJD+2430000,11,12,1);
   //[E_wrt_EM_Pos, E_wrt_EM_Vel] = pleph(tdbMJD+2430000,3,13,1);
   //[E_wrt_SSB_Pos, E_wrt_SSB_Vel] = pleph(tdbMJD+2430000,3,12,1);
   //[Mars_wrt_Sun_Pos, Mars_wrt_Sun_Vel] = pleph(tdbMJD+2430000,4,11,1);

   Rvector3 Earth2GS = ((SpacePoint*)participant)->GetMJ2000Position(tA1MJD);

   Rvector3 Sun_wrt_SSB_Vel = sunVel;
   Rvector3 EM_wrt_Sun_Pos = emPos - sunPos;
   Rvector3 EM_wrt_Sun_Vel = emVel - sunVel;
   Rvector3 EM_wrt_SSB_Vel = emVel;
   Rvector3 E_wrt_SSB_Vel = earthVel;
   Rvector3 E_wrt_EM_Pos = earthPos - emPos;
   Rvector3 Jup_wrt_Sun_Pos = jupiterPos - sunPos;
   Rvector3 Jup_wrt_Sun_Vel = jupiterVel - sunVel;
   Rvector3 Sat_wrt_Sun_Pos = saturnPos - sunPos;
   Rvector3 Sat_wrt_Sun_Vel = saturnVel - sunVel;

   // Compute the transformation
   Real delta = 32.184 + (2/c/c) * (EM_wrt_Sun_Vel*EM_wrt_Sun_Pos) + 
                         (1/c/c) * (EM_wrt_SSB_Vel*E_wrt_EM_Pos) + 
                         (1/c/c) * (E_wrt_SSB_Vel*Earth2GS) + 
                (muJupiter  / (c * c * (muSun + muJupiter) )) * (Jup_wrt_Sun_Vel*Jup_wrt_Sun_Pos) + 
                (muSaturn  / (c * c * (muSun + muSaturn) )) * (Sat_wrt_Sun_Vel*Sat_wrt_Sun_Pos) + 
                (1/c/c) * (Sun_wrt_SSB_Vel*EM_wrt_Sun_Pos);

   //(muMars  / (c^2 * (muSun + muMars) )) * (Mars_wrt_Sun_Vel'*Mars_wrt_Sun_Pos);  % is this Mars term correct?

   return delta;
}


// made changes by TUAN NGUYEN
#include "MeasurementException.hpp"
Real LightTimeCorrection::ETminusTAI(Real tA1MJD, GmatBase* participant)
{
	if (((SpacePoint*) participant)->GetJ2000BodyName() != "Earth")
		throw MeasurementException("ET - TAI was not implimented for station on "+ ((SpacePoint*) participant)->GetJ2000BodyName()+"\n");

	Real TAI;
	Rvector3 posGEO = ((SpacePoint*) participant)->GetMJ2000Position(tA1MJD);
	SpacePoint* j2kBody = ((SpacePoint*) participant)->GetJ2000Body();
	Rvector3 posSSB = ConvertLocalGEOCToSSB(tA1MJD, posGEO, (CelestialBody*) j2kBody);
	Real delta_t = CalculateETMinusTAI(tA1MJD, participant);

}