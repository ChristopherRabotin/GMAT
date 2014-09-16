//$Id$
//------------------------------------------------------------------------------
//                           USNRangeAdapter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract
//
// Author: Tuan Dang Nguyen, NASA/GSFC
// Created: Aug 29, 2014
/**
 * A measurement adapter for USN ranges
 */
//------------------------------------------------------------------------------

#ifndef USNRangeAdapter_hpp
#define USNRangeAdapter_hpp

#include "RangeAdapterKm.hpp"


/**
 * A measurement adapter for USN ranges
 */
class ESTIMATION_API USNRangeAdapter: public RangeAdapterKm
{
public:
   USNRangeAdapter(const std::string& name);
   virtual ~USNRangeAdapter();
   USNRangeAdapter(const USNRangeAdapter& usn);
   USNRangeAdapter&      operator=(const USNRangeAdapter& usn);

   virtual GmatBase*    Clone() const;

   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);

   virtual bool         Initialize();

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
   virtual void         SetCorrection(const std::string& correctionName,
         const std::string& correctionType);

   DEFAULT_TO_NO_CLONES

protected:
   /// Parameter IDs for the DSNRangeAdapter
   enum
   {
      USNRangeAdapterParamCount = RangeAdapterKmParamCount,
   };

   /// Strings describing the DSNRangeAdapter parameters
   static const std::string PARAMETER_TEXT[USNRangeAdapterParamCount -
                                           RangeAdapterKmParamCount];
   /// Types of the DSNRangeAdapter parameters
   static const Gmat::ParameterType PARAMETER_TYPE[USNRangeAdapterParamCount -
                                                   RangeAdapterKmParamCount];

};

#endif /* USNRangeAdapter_hpp */
