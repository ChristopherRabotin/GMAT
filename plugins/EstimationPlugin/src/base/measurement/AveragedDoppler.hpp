//$Id: AveragedDoppler.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         AveragedDoppler
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS
// Task 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 27, 2010
//
/**
 * Base class for the averaged Doppler measurements.
 */
//------------------------------------------------------------------------------


#ifndef AveragedDoppler_hpp
#define AveragedDoppler_hpp

#include "estimation_defs.hpp"
#include "PhysicalMeasurement.hpp"
#include "LightTimeCorrection.hpp"
#include "GmatConstants.hpp"       // For speed of light


/**
 * Base class for Doppler measurements based on averaging of two paths
 *
 * This class defines the structures and parameters used when calculating an
 * averaged Doppler measurement.  The structures provided here are sufficient
 * for calculations of a 2-way differenced Doppler measurement between one
 * participant and another with no intervening participants.  It also provides
 * a starting point when there are intervening parties, like in TDRSS
 * configurations.
 */
class ESTIMATION_API AveragedDoppler: public PhysicalMeasurement
{
public:
   AveragedDoppler(const std::string &type, const std::string &withName = "");
   virtual ~AveragedDoppler();
   AveragedDoppler(const AveragedDoppler& ad);
   AveragedDoppler& operator=(const AveragedDoppler& ad);

   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         GetRealParameter(const Integer id, const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         SetRealParameter(const Integer id, const Real value,
                                         const Integer row, const Integer col);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value, const Integer row,
                                         const Integer col);

   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);

   virtual bool         Initialize();
   virtual Event*       GetEvent(UnsignedInt whichOne);
   virtual bool         SetEventData(Event *locatedEvent = NULL);

   virtual const std::vector<RealArray>&
                        CalculateMeasurementDerivatives(GmatBase *obj,
                                                        Integer id) = 0;

protected:
   /// Measurement time
   GmatEpoch tm;

   // All other times are offsets from the tm epoch, measured in seconds
   /// Averaging interval
   Real interval;
   /// Time of first and second reception (post electronics)
   Real t3E[2];
   /// Delay before signal leaves transmitter
   Real t1delay[2];
   /// Time signal was transmitted from participant 1
   Real t1T[2];
   /// Transponder delay at participant 2
   Real t2delay[2];
   /// Time signal was transmitted from participant 2
   Real t2T[2];
   /// Time signal was received back at participant 1
   Real t3R[2];
   /// Final electronics delay
   Real t3delay[2];
   /// Turnaround ratio at the target spacecraft
   Real turnaround;

   // Light time events
   /// Uplink leg for the start signal
   LightTimeCorrection  uplinkLegS;
   /// Downlink leg for the start signal
   LightTimeCorrection  downlinkLegS;
   /// Uplink leg for the end signal
   LightTimeCorrection  uplinkLegE;
   /// Downlink leg for the end signal
   LightTimeCorrection  downlinkLegE;

   virtual void         InitializeMeasurement();
   virtual void         SetHardwareDelays(bool loadEvents = true);
   virtual void         SetupTimeIntervals();

   /// Enumeration defining the MeasurementModel's scriptable parameters
   enum
   {
       AveragingInterval = PhysicalMeasurementParamCount,
       AveragedDopplerParamCount
   };

   // Start with the parameter IDs and associates strings
   /// Script labels for the MeasurementModel parameters
   static const std::string
                PARAMETER_TEXT[AveragedDopplerParamCount -
                               PhysicalMeasurementParamCount];
   /// IDs for the MeasurementModel parameters
   static const Gmat::ParameterType
                PARAMETER_TYPE[AveragedDopplerParamCount -
                               PhysicalMeasurementParamCount];
};

#endif /* AveragedDoppler_hpp */
