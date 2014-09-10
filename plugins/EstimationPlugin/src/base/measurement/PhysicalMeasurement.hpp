//$Id: PhysicalMeasurement.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         PhysicalMeasurement
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
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
///// TBD: Determine if there is a more generic way to add these
#include "ObservationData.hpp"         // made changes by TUAN NGUYEN
#include "RampTableData.hpp"           // made changes by TUAN NGUYEN

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

   void        SetConstantFrequency(Real newFreq, Integer index=0);                  // made changes by TUAN NGUYEN
   Real        GetConstantFrequency(Integer index=0);

   void       AddCorrection(const std::string& modelName, const std::string& mediaCorrectionType);
//   RealArray   TroposphereCorrection(Real freq, Rvector3 rVec, Rmatrix33 Ro_j2k);
   RealArray    TroposphereCorrection(Real freq, Real distance, Real elevationAngle);

#ifdef IONOSPHERE
   RealArray   IonosphereCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch);
#endif

   RealArray   CalculateMediaCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch);

///// TBD: Determine if there is a more generic way to add these
   void         SetFrequencyBand(Integer frequencyBand, Integer index=0);      // made changes by TUAN NGUYEN
   Integer      GetFrequencyBand(Integer index=0);                             // made changes by TUAN NGUYEN
   //void         SetRangeModulo(Real rangeMod);                               // made changes by TUAN NGUYEN
   //Real         GetRangeModulo();                                            // made changes by TUAN NGUYEN
   void         SetObsValue(const RealArray& value);                           // made changes by TUAN NGUYEN 
   RealArray    GetObsValue();                                                 // made changes by TUAN NGUYEN

   void         SetObservationDataRecord(ObservationData* data);               // made changes by TUAN NGUYEN 
   Integer      FrequencyBand(Real frequency);                                 // made changes by TUAN NGUYEN 

   void         SetRampTable(std::vector<RampTableData>* rt);                  // made changes by TUAN NGUYEN
   void         SetRelativityCorrection(bool useRelCorr);                      // made changes by TUAN NGUYEN
   void         SetETMinusTAICorrection(bool useETTAICorr);                    // made changes by TUAN NGUYEN


protected:
   /// Constant frequency value used in a physical measurement when needed (In DSNDoppler, it is used as uplink frequency for S path
   Real                       frequency;      // Its unit is Hz (not MHz)
///// TBD: Determine if there is a more generic way to add these
   /// Constant frequency value used in a physical measurement when needed for E path in DSNDoppler      // made changes by TUAN NGUYEN
   Real                       frequencyE;      // Its unit is Hz (not MHz)                        // made changes by TUAN NGUYEN
   /// Frequency band   (In DSNDoppler, it is used for S path)                                    // made changes by TUAN NGUYEN
   Integer                    freqBand;                                                           // made changes by TUAN NGUYEN
   /// Frequency band for E path                                                                  // made changes by TUAN NGUYEN
   Integer                    freqBandE;                                                          // made changes by TUAN NGUYEN
//   /// Range modulo                                                                             // made changes by TUAN NGUYEN
//   Real                       rangeModulo;                                                      // made changes by TUAN NGUYEN
   /// Observation value                                                                          // made changes by TUAN NGUYEN
   RealArray                  obsValue;                                                           // made changes by TUAN NGUYEN
   /// Observation data object containing an observation data record                              // made changes by TUAN NGUYEN
   ObservationData*           obsData;                                                            // made changes by TUAN NGUYEN


   /// Frequency ramp table used to calculate frequency ramp measurements                         // made changes by TUAN NGUYEN
   std::vector<RampTableData>* rampTB;                                                            // made changes by TUAN NGUYEN
   /// Flags to indicate using relativity correction and ET-TAI correction                        // made changes by TUAN NGUYEN
   bool                        useRelativityCorrection;                                           // made changes by TUAN NGUYEN
   bool                        useETminusTAICorrection;                                           // made changes by TUAN NGUYEN

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

///// TBD: Determine if there is a more generic way to add these
   Real                       GetFrequencyFromRampTable(Real epoch);         // Get frequency from ramped table for a given epoch (in A1Mjd)      // made changes by TUAN NGUYEN
   Integer                    GetUplinkBandFromRampTable(Real epoch);                         // made changes by TUAN NGUYEN
   virtual Real               IntegralRampedFrequency(Real t1, Real delta_t, Integer& err);   // made changes by TUAN NGUYEN

   /// Enumeration defining the PhysicalMeasurement's scriptable parameters
   enum
   {
       PhysicalMeasurementParamCount = CoreMeasurementParamCount
   };

};

#endif /* PhysicalMeasurement_hpp */
