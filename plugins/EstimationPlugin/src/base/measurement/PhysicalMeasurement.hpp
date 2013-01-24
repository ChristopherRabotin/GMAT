//$Id: PhysicalMeasurement.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
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


#ifndef PhysicalMeasurement_hpp
#define PhysicalMeasurement_hpp

#include "estimation_defs.hpp"
#include "CoreMeasurement.hpp"
#include "Troposphere.hpp"

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

//#define DEBUG_RANGE_CALC_WITH_EVENTS
//#define VIEW_PARTICIPANT_STATES_WITH_EVENTS

/**
 * Base class for real world measurements
 */
class ESTIMATION_API PhysicalMeasurement: public CoreMeasurement
{
public:
   PhysicalMeasurement(const std::string &type, const std::string &nomme);
   virtual ~PhysicalMeasurement();
   PhysicalMeasurement(const PhysicalMeasurement& pm);
   PhysicalMeasurement& operator=(const PhysicalMeasurement& pm);

   void        SetConstantFrequency(Real newFreq);
   Real        GetConstantFrequency();

   void 			AddCorrection(const std::string& modelName);
   RealArray	TroposphereCorrection(Real freq, Rvector3 rVec, Rmatrix Ro_j2k);

#ifdef IONOSPHERE
   RealArray	IonosphereCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch);
#endif

   RealArray	CalculateMediaCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch);



protected:
   /// Constant frequency value used in a physical measurement when needed
   Real                       frequency;		// Its unit is Hz (not MHz)
   /// Internal vector used in derivative calculations
   Rvector3                   rangeVec;

   /// media correction objects:
   Troposphere*               troposphere;
#ifdef IONOSPHERE
   Ionosphere*                ionosphere;
#endif

   /// Derived classes override this method to implement measurement
   /// calculations
   virtual bool               Evaluate(bool withEvents = false) = 0;
   virtual void               InitializeMeasurement();
   virtual void               GetRangeDerivative(Event &ev,
                                    const Rmatrix &stmInv, Rvector &deriv,
                                    bool wrtP1, Integer p1Index = 0,
                                    Integer p2Index = 1, bool wrtR = true,
                                    bool wrtV = true);
   virtual void               GetRangeVectorDerivative(Event &ev,
                                    const Rmatrix &stmInv, Rmatrix &deriv,
                                    bool wrtP1, Integer p1Index = 0,
                                    Integer p2Index = 1, bool wrtR = true,
                                    bool wrtV = true);
   virtual void               GetInverseSTM(GmatBase *forObject,
                                    Rmatrix &stmInv);
   virtual void               SetHardwareDelays(bool loadEvents = true);


   /// Enumeration defining the PhysicalMeasurement's scriptable parameters
   enum
   {
       PhysicalMeasurementParamCount = CoreMeasurementParamCount
   };

};

#endif /* PhysicalMeasurement_hpp */
