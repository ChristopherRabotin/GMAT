//$Id$
//------------------------------------------------------------------------------
//                           DopplerAdapter
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
// Created: Sept 30, 2014
/**
 * A measurement adapter for DSN Doppler
 */
//------------------------------------------------------------------------------

#ifndef DopplerAdapter_hpp
#define DopplerAdapter_hpp

#include "RangeAdapterKm.hpp"


/**
 * A measurement adapter for DSN Doppler
 */
class ESTIMATION_API DopplerAdapter: public RangeAdapterKm
{
public:
   DopplerAdapter(const std::string& name);
   virtual ~DopplerAdapter();
   DopplerAdapter(const DopplerAdapter& da);
   DopplerAdapter&      operator=(const DopplerAdapter& da);

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
   /// Constant frequency value used in a physical measurement when needed for E path in DSNDoppler
   Real                 uplinkFreqE;                      // unit: Hz
   /// Frequency band for E path in DSNDoppler
   Integer              freqBandE;
   /// Doppler count interval
   Real                 dopplerCountInterval;
   /// Turn around ratio
   Real                 turnaround;
   /// Multiplier for S-path and E-path
   Real                 multiplierS;
   Real                 multiplierE;
   /// RangeAdapter for S-path
   RangeAdapterKm       *adapterS;


   /// Parameter IDs for the DopplerAdapter
   enum
   {
      DOPPLER_COUNT_INTERVAL = RangeAdapterKmParamCount,
      DopplerAdapterParamCount,
   };

   /// Strings describing the DopplerAdapter parameters
   static const std::string PARAMETER_TEXT[DopplerAdapterParamCount -
                                           RangeAdapterKmParamCount];
   /// Types of the DopplerAdapter parameters
   static const Gmat::ParameterType PARAMETER_TYPE[DopplerAdapterParamCount -
                                                   RangeAdapterKmParamCount];

private:
   /// MeasurementData for Start path
   MeasurementData measDataS;
   /// MeasurementData for End path
   MeasurementData measDataE;

};

#endif /* DopplerAdapter_hpp */
