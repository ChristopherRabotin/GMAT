//$Id$
//------------------------------------------------------------------------------
//                           RangeAdapterKm
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
// Created: Feb 12, 2014
/**
 * A measurement adapter for ranges in Km
 */
//------------------------------------------------------------------------------

#ifndef RangeAdapterKm_hpp
#define RangeAdapterKm_hpp

#include "TrackingDataAdapter.hpp"


/**
 * A measurement adapter for ranges in Km
 */
class ESTIMATION_API RangeAdapterKm: public TrackingDataAdapter
{
public:
   RangeAdapterKm(const std::string& name);
   virtual ~RangeAdapterKm();
   RangeAdapterKm(const RangeAdapterKm& rak);
   RangeAdapterKm&      operator=(const RangeAdapterKm& rak);

   virtual GmatBase*    Clone() const;

   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);

   virtual bool         Initialize();

   DEFAULT_TO_NO_CLONES

   // Preserve interfaces in the older measurement model code
   virtual const MeasurementData&
                        CalculateMeasurement(bool withEvents = false,
                              ObservationData* forObservation = NULL,
                              std::vector<RampTableData>* rampTB = NULL);
   virtual const std::vector<RealArray>&
                        CalculateMeasurementDerivatives(GmatBase *obj,
                              Integer id);
   virtual bool         WriteMeasurements();
   virtual bool         WriteMeasurement(Integer id);

   // Covariance handling code
   virtual Integer      HasParameterCovariances(Integer parameterId);

   virtual Integer      GetEventCount();
//   virtual Event*       GetEvent(Integer whichOne);
//   virtual bool         SetEventData(Event *locatedEvent = NULL);

   virtual void         SetCorrection(const std::string& correctionName,
         const std::string& correctionType);

};

#endif /* RangeAdapterKm_hpp */
