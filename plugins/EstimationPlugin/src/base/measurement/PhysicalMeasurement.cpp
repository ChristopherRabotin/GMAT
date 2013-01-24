//$Id: PhysicalMeasurement.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         PhysicalMeasurement
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
// Created: 2009/12/16
//
/**
 * Base class for real world measurement primitives
 */
//------------------------------------------------------------------------------


#include "PhysicalMeasurement.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "GroundstationInterface.hpp"
#include "MeasurementException.hpp"

#ifdef IONOSPHERE
#include "CoordinateConverter.hpp"
#include "Moderator.hpp"
#endif

//#define DEBUG_DERIVATIVES
//#define DEBUG_RANGE_CALC_WITH_EVENTS
//#define DEBUG_MEDIA_CORRECTION
//#define DEBUG_IONOSPHERE_MEDIA_CORRECTION

//------------------------------------------------------------------------------
// PhysicalMeasurement(const std::string &type, const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The type of the PhysicalMeasurement
 * @param nomme The measurement's name
 */
//------------------------------------------------------------------------------
PhysicalMeasurement::PhysicalMeasurement(const std::string &type,
			const std::string &nomme) :
   CoreMeasurement      (type, nomme),
   frequency            (2090659968.0)
{
   objectTypeNames.push_back("PhysicalMeasurement");

   troposphere	= NULL;
   #ifdef IONOSPHERE
      ionosphere = NULL;
   #endif
}


//------------------------------------------------------------------------------
// ~PhysicalMeasurement()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PhysicalMeasurement::~PhysicalMeasurement()
{
	if (troposphere != NULL)
		delete troposphere;

   #ifdef IONOSPHERE
	   if (ionosphere != NULL)
		   delete ionosphere;
   #endif
}


//------------------------------------------------------------------------------
// PhysicalMeasurement(const PhysicalMeasurement& pm) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param pm The PhysicalMeasurement that is getting copied here
 */
//------------------------------------------------------------------------------
PhysicalMeasurement::PhysicalMeasurement(const PhysicalMeasurement& pm) :
   CoreMeasurement      (pm),
   frequency            (pm.frequency)
{
   if (pm.troposphere != NULL)
      troposphere = new Troposphere(*(pm.troposphere));
   else
      troposphere = NULL;

   #ifdef IONOSPHERE
      if (pm.ionosphere != NULL)
         ionosphere = new Ionosphere(*(pm.ionosphere));
      else
         ionosphere = NULL;
   #endif
}


//------------------------------------------------------------------------------
// PhysicalMeasurement& operator=(const PhysicalMeasurement& pm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param pm The PhysicalMeasurement that supplies new data for this one
 *
 * @return This PhysicalMeasurement
 */
//------------------------------------------------------------------------------
PhysicalMeasurement& PhysicalMeasurement::operator=(
      const PhysicalMeasurement& pm)
{
   if (this != &pm)
   {
      CoreMeasurement::operator=(pm);

      frequency = pm.frequency;

      // Rebuild the correction models
      if (troposphere != NULL)
         delete troposphere;

      if (pm.troposphere != NULL)
         troposphere = new Troposphere(*(pm.troposphere));
      else
         troposphere = NULL;

      #ifdef IONOSPHERE
         if (ionosphere != NULL)
            delete ionosphere;

         if (pm.ionosphere != NULL)
            ionosphere = new Ionosphere(*(pm.ionosphere));
         else
            ionosphere = NULL;
      #endif
   }

   return *this;
}


//------------------------------------------------------------------------------
// void SetConstantFrequency(Real newFreq)
//------------------------------------------------------------------------------
/**
 * Sets a frequency value on the measurement
 *
 * @param newFreq The new frequency value
 */
//------------------------------------------------------------------------------
void PhysicalMeasurement::SetConstantFrequency(Real newFreq)
{
   if (newFreq > 0.0)
      frequency = newFreq;
}


//------------------------------------------------------------------------------
// Real PhysicalMeasurement::GetConstantFrequency()
//------------------------------------------------------------------------------
/**
 * Retrieves the measurement frequency
 *
 * @return The frequency
 */
//------------------------------------------------------------------------------
Real PhysicalMeasurement::GetConstantFrequency()
{
   return frequency;
}


//----------------------------------------------------------------------
// void AddCorrection(std::string& modelName)
//----------------------------------------------------------------------
/**
 * This function is used to add media correction model to the measurement
 *
 * @param nodelName	The name of media correction model involving in the
 * 						measurement
 */
//----------------------------------------------------------------------
void PhysicalMeasurement::AddCorrection(const std::string& modelName)
{
   #ifdef DEBUG_MEDIA_CORRECTION
      MessageInterface::ShowMessage("Adding correction named %s\n",
            modelName.c_str());
   #endif

	if (modelName == "HopfieldSaastamoinen")
	{
	   if (troposphere != NULL)
	      delete troposphere;

	   // Create troposphere correction model
	   troposphere = new Troposphere(modelName);

       #ifdef DEBUG_MEDIA_CORRECTION
          MessageInterface::ShowMessage("   Set as troposphere model:   troposphere(%p)\n", troposphere);
       #endif
	}
	else if (modelName == "IRI2007")							// made changes by TUAN NGUYEN
	{
      #ifdef DEBUG_MEDIA_CORRECTION
         MessageInterface::ShowMessage("   Set as ionosphere model\n");
      #endif

		// Create IRI2007 ionosphere correction model
      #ifdef IONOSPHERE
		   ionosphere = new Ionosphere(modelName);
      #else
		   MessageInterface::ShowMessage("Ionosphere IRI2007 model currently is not "
				         "available.\nIt will be be added to GMAT in a future release.\n");

		   throw new GmatBaseException("Ionosphere IRI2007 model currently is not "
		         "available.\nIt will be be added to GMAT in a future release.\n");
      #endif
	}
	else if (modelName == "None")
	{
	}
	else
	{
		MessageInterface::ShowMessage("Error: '%s' is not allowed for media correction name.\n", modelName.c_str());
		MessageInterface::ShowMessage("Only 2 media correction model names are allowed: HopfieldSaastamoinen and IRI2007\n");
		throw new MeasurementException("Error: media correction model name is not allowed\n"
			"Only 2 media correction model names are allowed: HopfieldSaastamoinen and IRI2007\n");
	}
}

//------------------------------------------------------------------------
//RealArray TroposphereCorrection(Real freq, Rvector3 rVec, Rmatrix Ro_j2k)
//------------------------------------------------------------------------
/**
 * This function is used to calculate Troposphere correction.
 *
 * @param freq		The frequency of signal	(unit: MHz)
 * @param rVec		Range vector in J2k coordinate system
 * @param Ro_j2k Rotational matrix converting j2k coordinate system to
 *                  topocentric coordinate system
 *
 */
//------------------------------------------------------------------------
RealArray PhysicalMeasurement::TroposphereCorrection(Real freq, Rvector3 rVec, Rmatrix Ro_j2k)
{
   RealArray tropoCorrection;

   if (troposphere != NULL)
   {
   	Real wavelength = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / (freq*1.0e6);
   	troposphere->SetWaveLength(wavelength);
   	Real elevationAngle = asin((Ro_j2k*rVec.GetUnitVector()).GetElement(2));
   	troposphere->SetElevationAngle(elevationAngle);
   	troposphere->SetRange(rVec.GetMagnitude()*GmatMathConstants::KM_TO_M);
   	tropoCorrection = troposphere->Correction();
//   	Real rangeCorrection = tropoCorrection[0]/GmatMathConstants::KM_TO_M;

		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("       Apply Troposphere media correction:\n");
			MessageInterface::ShowMessage("         .Wave length = %.12lf m\n", wavelength);
			MessageInterface::ShowMessage("         .Elevation angle = %.12lf degree\n", elevationAngle*GmatMathConstants::DEG_PER_RAD);
			MessageInterface::ShowMessage("         .Range correction = %.12lf m\n", tropoCorrection[0]);
		#endif
   }
   else
   {
   	tropoCorrection.push_back(0.0);
   	tropoCorrection.push_back(0.0);
   	tropoCorrection.push_back(0.0);
   }

   return tropoCorrection;
}



//------------------------------------------------------------------------
//RealArray PhysicalMeasurement::IonosphereCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch)
//------------------------------------------------------------------------
/**
 * This function is used to calculate Ionosphere correction.
 *
 * @param freq		The frequency of signal		(unit: MHz)
 * @param r1		Position of ground station	(unit: km)
 * @param r2		Position of spacecraft		(unit:km)
 * @param epoch	Time at which the signal is transmitted or received from ground station		(unit: Julian day)
 */
//------------------------------------------------------------------------
#ifdef IONOSPHERE
RealArray PhysicalMeasurement::IonosphereCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch)
{
   RealArray ionoCorrection;

   if (ionosphere != NULL)
   {
   	// 1. Set wave length:
   	Real wavelength = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / (freq*1.0e6);		// unit: meter
   	ionosphere->SetWaveLength(wavelength);

   	// 2. Set time:
   	ionosphere->SetTime(epoch);															// unit: Julian day

   	// 3. Set station and spacecraft positions:
   	GroundstationInterface* gs 	= (GroundstationInterface*)participants[0];
   	CoordinateSystem* cs = gs->GetBodyFixedCoordinateSystem();
   	Rvector inState(6, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
	Rvector outState(6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
//   	Rvector bogusOut 		= cs->FromMJ2000Eq(epoch, bogusIn, true);
//   	Rmatrix33 R_g_j2k    = cs->GetLastRotationMatrix().Transpose();
	CoordinateConverter* cv = new CoordinateConverter();
	A1Mjd time(epoch);
	CoordinateSystem* fk5cs = Moderator::Instance()->GetCoordinateSystem("EarthMJ2000Eq");
	cv->Convert(time, inState, cs, outState, fk5cs);				// convert EarthFK5 coordinate system to EarthBodyFixed coordinate system
	Rmatrix33 R_g_j2k    = cv->GetLastRotationMatrix().Transpose();
//   	MessageInterface::ShowMessage("[ %f  %f  %f\n", R_g_j2k.GetElement(0,0), R_g_j2k.GetElement(0,1), R_g_j2k.GetElement(0,2));
//   	MessageInterface::ShowMessage("  %f  %f  %f\n", R_g_j2k.GetElement(1,0), R_g_j2k.GetElement(1,1), R_g_j2k.GetElement(1,2));
//   	MessageInterface::ShowMessage("  %f  %f  %f]\n", R_g_j2k.GetElement(2,0), R_g_j2k.GetElement(2,1), R_g_j2k.GetElement(2,2));

   	ionosphere->SetStationPosition(R_g_j2k*r1);											// unit: km
   	ionosphere->SetSpacecraftPosition(R_g_j2k*r2);										// unit: km

   	// 4. Set earth radius:
	SpacePoint* earth 	= (SpacePoint*)gs->GetRefObject(Gmat::SPACE_POINT, "Earth");
   	Real earthRadius 	= earth->GetRealParameter("EquatorialRadius");
   	ionosphere->SetEarthRadius(earthRadius);											// unit: km

#ifdef DEBUG_IONOSPHERE_MEDIA_CORRECTION
	MessageInterface::ShowMessage("      *Run Ionosphere media correction for:\n");
	MessageInterface::ShowMessage("         +Earth radius = %lf km\n", earthRadius);
	MessageInterface::ShowMessage("         +Wave length = %.12lf m\n", wavelength);
	MessageInterface::ShowMessage("         +Time = %.12lf\n", epoch);
	MessageInterface::ShowMessage("         +Station location in Earth body fixed coordinate system (km):\n"
		                          "            (%s)", (R_g_j2k*r1).ToString().c_str()); 
	MessageInterface::ShowMessage("         +Spacecraft location in Earth body fixed coordinate system (km):\n"
		                          "            (%s)", (R_g_j2k*r2).ToString().c_str());
#endif

	// 5. Run ionosphere correction:
   	ionoCorrection = ionosphere->Correction();
   	Real rangeCorrection = ionoCorrection[0]*GmatMathConstants::M_TO_KM;				// unit: meter

#ifdef DEBUG_IONOSPHERE_MEDIA_CORRECTION
	MessageInterface::ShowMessage("      *Ionosphere media correction result:\n");
	MessageInterface::ShowMessage("         +Range correction = %.12lf m\n", rangeCorrection*GmatMathConstants::KM_TO_M);
#endif
   }
   else
   {
   	ionoCorrection.push_back(0.0);
   	ionoCorrection.push_back(0.0);
   	ionoCorrection.push_back(0.0);
   }

   return ionoCorrection;
}
#endif

//------------------------------------------------------------------------
// RealArray CalculateMediaCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch)
//------------------------------------------------------------------------
/**
 * This function is used to calculate media corrections.
 *
 * @param freq		The frequency of signal	(unit: MHz)
 * @param r1		Position of ground station
 * @param r2		Position of spacecraft
 * @param epoch	The time at which signal is transmitted or received from ground station
 */
//------------------------------------------------------------------------
RealArray PhysicalMeasurement::CalculateMediaCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch)
{
   #ifdef DEBUG_MEDIA_CORRECTION
      MessageInterface::ShowMessage("start PhysicalMeasurement::CalculateMediaCorrection()\n");
   #endif

   RealArray mediaCorrection;

   // 1. Run Troposphere correction:
   UpdateRotationMatrix(epoch, "o_j2k");
   Rvector3 rangeVector = r2 - r1;
   mediaCorrection = TroposphereCorrection(freq, rangeVector, R_o_j2k);

   #ifdef DEBUG_MEDIA_CORRECTION
      MessageInterface::ShowMessage(" frequency = %le MHz, epoch = %lf,     r2-r1 = ('%s')km\n", freq, epoch, (r2-r1).ToString().c_str());
	  MessageInterface::ShowMessage(" TroposhereCorrection = (%lf m,  %lf arcsec,   %le s)\n", mediaCorrection[0], mediaCorrection[1], mediaCorrection[2]);
   #endif

   #ifdef IONOSPHERE
      // 2. Run Ionosphere correction:
      RealArray ionoCorrection = this->IonosphereCorrection(freq, r1, r2, epoch);

      // 3. Combine effects:
      mediaCorrection[0] += ionoCorrection[0];
      mediaCorrection[1] += ionoCorrection[1];
      mediaCorrection[2] += ionoCorrection[2];
   #endif

   return mediaCorrection;
}


//------------------------------------------------------------------------------
// void InitializeMeasurement()
//------------------------------------------------------------------------------
/**
 * Initializes the measurement for use in estimation or simulation
 */
//------------------------------------------------------------------------------
void PhysicalMeasurement::InitializeMeasurement()
{
   CoreMeasurement::InitializeMeasurement();
}


//------------------------------------------------------------------------------
// void GetRangeDerivative(Event &ev, const Rmatrix &stmInv, Rvector &deriv,
//       bool wrtP1, Integer p1Index, Integer p2Index, bool wrtR, bool wrtV)
//------------------------------------------------------------------------------
/**
 * Calculates the range derivative of a leg of a measurement
 *
 * @param ev The event associated with the leg
 * @param stmInv The STM inverse
 * @param deriv The structure that gets filled with derivative data
 * @param wrtP1 Flag indicating if the derivative is w.r.t. participant 1 or 2
 * @param p1Index Index of the "transmitting" participant
 * @param p2Index Index of the "receiving" participant
 * @param wrtR Flag indicating if derivative with respect to position is desired
 * @param wrtV Flag indicating if derivative with respect to velocity is desired
 */
//------------------------------------------------------------------------------
void PhysicalMeasurement::GetRangeDerivative(Event &ev, const Rmatrix &stmInv,
      Rvector &deriv, bool wrtP1, Integer p1Index, Integer p2Index, bool wrtR,
      bool wrtV)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("PhysicalMeasurement::GetRangeDerivative(%s, "
            "stmInv, deriv, %d <%s>, %d <%s>, %s, %s)\n", ev.GetName().c_str(),
            p1Index, participants[p1Index]->GetName().c_str(), p2Index,
            participants[p2Index]->GetName().c_str(),
            (wrtR == true ? "true" : "false"),
            (wrtV == true ? "true" : "false"));
   #endif

   Rmatrix derivMatrix;
   if (wrtR && wrtV)
      derivMatrix.SetSize(6,6);
   else
      derivMatrix.SetSize(3,3);

   GetRangeVectorDerivative(ev, stmInv, derivMatrix, wrtP1, p1Index, p2Index,
         wrtR, wrtV);

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   Derivative matrix = \n");
      for (Integer i = 0; i < derivMatrix.GetNumRows(); ++i)
      {
         MessageInterface::ShowMessage("      [");
         for (Integer j = 0; j < derivMatrix.GetNumColumns(); ++j)
            MessageInterface::ShowMessage(" %.12lf ", derivMatrix(i,j));
         MessageInterface::ShowMessage("]\n");
      }
   #endif

   Rvector3 temp;
   Rmatrix33 mPart;
   Rvector3 unitRange = rangeVec / rangeVec.GetMagnitude();

   if (wrtR)
   {
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            mPart(i,j) = derivMatrix(i,j);

      temp = unitRange * mPart;
      for (Integer i = 0; i < 3; ++i)
         deriv[i] = temp(i);
   }
   if (wrtV)
   {
      Integer offset = (wrtR ? 3 : 0);
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            mPart(i,j) = derivMatrix(i+offset, j+offset);

      temp = unitRange * mPart;
      for (Integer i = 0; i < 3; ++i)
         deriv[i+offset] = temp(i);
   }

      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   Derivative = [");
         for (Integer i = 0; i < deriv.GetSize(); ++i)
            MessageInterface::ShowMessage(" %.12lf ", deriv[i]);
         MessageInterface::ShowMessage("]\n");
      #endif
}


//------------------------------------------------------------------------------
// void GetRangeVectorDerivative(Event &ev, const Rmatrix &stmInv,
//       Rmatrix &deriv, bool wrtP1, Integer p1Index, Integer p2Index,
//       bool wrtR, bool wrtV)
//------------------------------------------------------------------------------
/**
 * Calculates the range vector derivative of a leg of a measurement
 *
 * @param ev The event associated with the leg
 * @param stmInv The STM inverse
 * @param deriv The structure that gets filled with derivative data
 * @param wrtP1 Flag indicating if the derivative is w.r.t. participant 1 or 2
 * @param p1Index Index of the "transmitting" participant
 * @param p2Index Index of the "receiving" participant
 * @param wrtR Flag indicating if derivative with respect to position is desired
 * @param wrtV Flag indicating if derivative with respect to velocity is desired
 */
//------------------------------------------------------------------------------
void PhysicalMeasurement::GetRangeVectorDerivative(Event &ev,
      const Rmatrix &stmInv, Rmatrix &deriv, bool wrtP1, Integer p1Index,
      Integer p2Index, bool wrtR, bool wrtV)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("PhysicalMeasurement::GetRangeVector"
            "Derivative(%s, stmInv, deriv, %d <%s>, %d <%s>, %s, %s)\n",
            ev.GetName().c_str(), p1Index,
            participants[p1Index]->GetName().c_str(), p2Index,
            participants[p2Index]->GetName().c_str(),
            (wrtR == true ? "true" : "false"),
            (wrtV == true ? "true" : "false"));
   #endif

   EventData p1Data = ev.GetEventData(participants[p1Index]);
   EventData p2Data = ev.GetEventData(participants[p2Index]);

   rangeVec = p2Data.position - p1Data.position;

   // Be sure to use the correct rotation matrices, etc
   EventData dataToUse = (wrtP1 ? p1Data : p2Data);
   // p1 derivatives pick up a minus sign, handled with this variable
   Real sign = (wrtP1 ? -1.0 : 1.0);


   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("p1Position: %s\n", p1Data.position.ToString().c_str());
      MessageInterface::ShowMessage("p2Position: %s\n", p2Data.position.ToString().c_str());

      MessageInterface::ShowMessage("   Range vector: [%.12lf %.12lf %.12lf]\n",
            rangeVec[0], rangeVec[1], rangeVec[2]);
   #endif

   Rmatrix phi = dataToUse.stm * stmInv;

   Rmatrix33 A, B;
   for (Integer i = 0; i < 3; ++i)
      for (Integer j = 0; j < 3; ++j)
      {
         if (wrtR)
            A(i,j) = phi(i,j);
         if(wrtV)
            B(i,j) = phi(i,j+3);
      }

   Rmatrix33 temp;

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("Phi: %s\n", phi.ToString().c_str());
      MessageInterface::ShowMessage("  A: %s\n", A.ToString().c_str());
      MessageInterface::ShowMessage("  B: %s\n", B.ToString().c_str());
   #endif

   if (wrtR)
   {
      temp = dataToUse.rInertial2obj * A;

      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            deriv(i,j) = sign * temp(i,j);
   }
   if (wrtV)
   {
      temp = dataToUse.rInertial2obj * B;

      Integer offset = (wrtR ? 3 : 0);
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            deriv(i+offset,j+offset) = sign * temp(i,j);
   }

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   Rotation matrix for %s:\n",
            participants[p2Index]->GetName().c_str());
      for (Integer i = 0; i < 3; ++i)
      {
         MessageInterface::ShowMessage("         [");
         for (Integer j = 0; j < 3; ++j)
            MessageInterface::ShowMessage(" %.12lf ",
                  p2Data.rInertial2obj(i,j));
         MessageInterface::ShowMessage("]\n");
      }
   #endif
}

//------------------------------------------------------------------------------
// void GetInverseSTM(GmatBase *forObject, Rmatrix &stmInv)
//------------------------------------------------------------------------------
/**
 * Builds the inverse state transition matrix
 *
 * @param forObject The object supplying the STM
 * @param stmInverse The matrix that gets filled with the STM inverse
 */
//------------------------------------------------------------------------------
void PhysicalMeasurement::GetInverseSTM(GmatBase *forObject, Rmatrix &stmInv)
{
   Integer stmId;
   try
   {
      stmId = forObject->GetParameterID("CartesianX");
   }
   catch (BaseException &)
   {
      stmId = -1;
   }

   if (stmId >= 0)
   {
      stmInv = (forObject->GetParameterSTM(stmId))->Inverse();
   }
   else
   {
      // Use identity if there is no STM
      stmInv(0,0) = stmInv(1,1) = stmInv(2,2) = stmInv(3,3)
                  = stmInv(4,4) = stmInv(5,5) = 1.0;
      stmInv(0,1) = stmInv(0,2) = stmInv(0,3) = stmInv(0,4) = stmInv(0,5)
                  = stmInv(1,0) = stmInv(1,2) = stmInv(1,3) = stmInv(1,4)
                  = stmInv(1,5) = stmInv(2,0) = stmInv(2,1) = stmInv(2,3)
                  = stmInv(2,4) = stmInv(2,5) = stmInv(3,0) = stmInv(3,1)
                  = stmInv(3,2) = stmInv(3,4) = stmInv(3,5) = stmInv(4,0)
                  = stmInv(4,1) = stmInv(4,2) = stmInv(4,3) = stmInv(4,5)
                  = stmInv(5,0) = stmInv(5,1) = stmInv(5,2) = stmInv(5,3)
                  = stmInv(5,4) = 0.0;
   }

   #ifdef DEBUG_STM_INVERTER
      Rmatrix mat = *forObject->GetParameterSTM(stmId);
      MessageInterface::ShowMessage("STM:\n");
      for (Integer i = 0; i < mat.GetNumRows(); ++i)
      {
         MessageInterface::ShowMessage("   [");
         for (Integer j = 0; j < mat.GetNumColumns(); ++j)
            MessageInterface::ShowMessage(" %.12lf ", mat(i,j));
         MessageInterface::ShowMessage("]\n");
      }

      mat = stmInv;
      MessageInterface::ShowMessage("STM Inverse:\n");
      for (Integer i = 0; i < mat.GetNumRows(); ++i)
      {
         MessageInterface::ShowMessage("   [");
         for (Integer j = 0; j < mat.GetNumColumns(); ++j)
            MessageInterface::ShowMessage(" %.12lf ", mat(i,j));
         MessageInterface::ShowMessage("]\n");
      }
   #endif
}


//------------------------------------------------------------------------------
// void SetHardwareDelays()
//------------------------------------------------------------------------------
/**
 * Retrieves delay vales and passes them into the events that need them.
 *
 * The default implementation does nothing but define the interface used in
 * derived classes that override it.
 */
//------------------------------------------------------------------------------
void PhysicalMeasurement::SetHardwareDelays(bool loadEvents)
{
}
