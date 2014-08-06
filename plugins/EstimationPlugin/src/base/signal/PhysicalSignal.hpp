//$Id$
//------------------------------------------------------------------------------
//                           PhysicalSignal
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 9, 2014
/**
 * Class used to model instantaneous signals
 */
//------------------------------------------------------------------------------

#ifndef PhysicalSignal_hpp
#define PhysicalSignal_hpp
// Temporary removal of ionosphere from Mac and Linux
#ifdef _WIN32
#define __WIN32__
#endif

#ifdef __WIN32__
#define IONOSPHERE
#endif

#ifdef IONOSPHERE
	#include "Ionosphere.hpp"
#endif


#include "SignalBase.hpp"
#include "Troposphere.hpp"


class PropSetup;

/**
 * Signal class used for instantaneous measurements
 *
 * This class might be going away based on the evolving measurement design
 */
class ESTIMATION_API PhysicalSignal: public SignalBase
{
public:
   PhysicalSignal(const std::string &typeStr, const std::string &name = "");
   virtual ~PhysicalSignal();
   PhysicalSignal(const PhysicalSignal& gs);
   PhysicalSignal& operator=(const PhysicalSignal& gs);

   virtual GmatBase* Clone() const;

   virtual void InitializeSignal(bool chainForwards = false);

   virtual bool ModelSignal(const GmatEpoch atEpoch, bool EpochAtReceive = true);
   virtual bool ModelSignal(const GmatTime atEpoch, bool EpochAtReceive = true);			// made changes by TUAN NGUYEN
   virtual const std::vector<RealArray>&
                ModelSignalDerivative(GmatBase *obj, Integer forId);

   /// This function is used to add media correction to measurement model
   virtual void AddCorrection(const std::string& modelName,									// made changes by TUAN NGUYEN
	                          const std::string& mediaCorrectionType);						// made changes by TUAN NGUYEN

protected:
   /// Flag indicating the initialization state of the new signal elements
   bool physicalSignalInitialized;
   /// Troposphere model object
   Troposphere* troposphere;
   /// Ionosphere model object
   Ionosphere* ionosphere;
   /// Flag indicating to use relativity correction											// made changes by TUAN NGUYEN
   bool useRelativity;																		// made changes by TUAN NGUYEN
   /// Relativity correction (unit: km)														// made changes by TUAN NGUYEN
   Real relCorrection;																		// made changes by TUAN NGUYEN
   /// Flag indicating to use Et-TAI correction												// made changes by TUAN NGUYEN
   bool useETTAI;																			// made changes by TUAN NGUYEN

   bool GenerateLightTimeData(const GmatEpoch atEpoch, bool epochAtReceive);
   bool GenerateLightTimeData(const GmatTime atEpoch, bool epochAtReceive);					// made changes by TUAN NGUYEN

   /// This function is used to compute relativity correction								// made changes by TUAN NGUYEN
   Real RelativityCorrection(Rvector3 r1B, Rvector3 r2B, Real t1, Real t2);					// made changes by TUAN NGUYEN

   /// This function is used to compute Et-TAI correction									// made changes by TUAN NGUYEN
   Real ETminusTAI(Real tA1MJD, SpacePoint* participant);								    // made changes by TUAN NGUYEN

   /// These functions are used to compute midia correction
   RealArray TroposphereCorrection(Real freq, Real distance, Real elevationAngle);			// made changes by TUAN NGUYEN
   RealArray IonosphereCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch);			// made changes by TUAN NGUYEN
   RealArray CalculateMediaCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch);		// made changes by TUAN NGUYEN

};

#endif /* PhysicalSignal_hpp */
