//$Id: StatisticRejectFilter.hpp 1398 2015-03-16 13:36:00Z tdnguyen $
//------------------------------------------------------------------------------
//                                 StatisticRejectFilter
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
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2015/03/16
//
/**
 * Definition for the SatisticAcceptFilter class used to define a data accepting 
 * filter for a TrackingFile
 */
//------------------------------------------------------------------------------


#ifndef StatisticRejectFilter_hpp
#define StatisticRejectFilter_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"
#include "DataFilter.hpp"

class ESTIMATION_API StatisticRejectFilter : public DataFilter
{
public:
   StatisticRejectFilter(const std::string name);
   virtual ~StatisticRejectFilter();
   StatisticRejectFilter(const StatisticRejectFilter& saf);
   StatisticRejectFilter& operator=(const StatisticRejectFilter& saf);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();
   
   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         SetStringParameter(const Integer id, const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, const std::string &value);
   virtual bool         SetStringParameter(const Integer id, const std::string &value, const Integer index);
   virtual bool         SetStringParameter(const std::string &label, const std::string &value, const Integer index);

   /// This function is used to set tracking configs to data file object in order to use for data filter
   virtual bool         SetTrackingConfigs(StringArray tkconfigs);

   virtual ObservationData* 
                        FilteringData(ObservationData* dataObject, Integer& rejectedReason);

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:

   /// List of tracking configs used for data filter
   StringArray tkConfigList;

   /// Class parameter ID enumeration
   enum
   {
      StatisticRejectFilterParamCount = DataFilterParamCount,
   };

   // Start with the parameter IDs and associates strings
   ///// Strings associated with the StatisticRejectFilter parameters
   //static const std::string
   //             PARAMETER_TEXT[StatisticRejectFilterParamCount - DataFilterParamCount];
   ///// Types of the StatisticRejectFilter parameters
   //static const Gmat::ParameterType
   //             PARAMETER_TYPE[StatisticRejectFilterParamCount - DataFilterParamCount];


};

#endif /* StatisticRejectFilter_hpp */
