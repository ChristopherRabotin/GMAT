//$Id$
//------------------------------------------------------------------------------
//                        PointPointRangeRateAdapterKps
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Michael Barrucco, Thinking Systems, Inc.
// Created: Oct 2nd, 2014
/**
 * A measurement adapter for ranges in Km
 */
//------------------------------------------------------------------------------

#ifndef PointRangeRateAdapterKps_hpp
#define PointRangeRateAdapterKps_hpp

#include "RangeAdapterKm.hpp"
#include "SpaceObject.hpp"

/**
 * A measurement adapter for ranges in Km
 */
class ESTIMATION_API PointRangeRateAdapterKps: public RangeAdapterKm
{
public:
   PointRangeRateAdapterKps(const std::string& name);
   virtual ~PointRangeRateAdapterKps();
   PointRangeRateAdapterKps(const PointRangeRateAdapterKps& rr);
   PointRangeRateAdapterKps&      operator=(const PointRangeRateAdapterKps& rr);

   virtual GmatBase*    Clone() const;

   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);

   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);

   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);

   virtual bool         Initialize();

   // Preserve interfaces in the older measurement model code
   virtual const MeasurementData&
                        CalculateMeasurement(bool withEvents = false,
                              ObservationData* forObservation = NULL,
                              std::vector<RampTableData>* rampTB = NULL,
                              bool forSimulation = false);
   virtual const std::vector<RealArray>&
                        CalculateMeasurementDerivatives(GmatBase *obj,
                              Integer id);
   virtual bool         WriteMeasurements();
   virtual bool         WriteMeasurement(Integer id);

   // Covariance handling code
   virtual Integer      HasParameterCovariances(Integer parameterId);

   virtual Integer      GetEventCount();
   virtual void         SetCorrection(const std::string& correctionName,
         const std::string& correctionType);

   DEFAULT_TO_NO_CLONES
protected:
//   /// Doppler interval
//   Real dopplerInterval;

   /// Previous epoch
   Real _prev_epoch;
   /// Previous Range 
   Real _prev_range;
   /// Timer 
   Real _timer;

   /// Flag indicating is a current value is available
   bool valueReady;
   /// Epoch of the most recently calculated data value
   GmatEpoch lastComputedEpoch;
   /// Pointer to the spacecraft that gets propagated in this model
   SpaceObject *targetSat;
};

#endif /* PointRangeRateAdapterKps_hpp */
