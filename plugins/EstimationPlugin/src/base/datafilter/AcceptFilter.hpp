//$Id: AcceptFilter.hpp 1398 2017-01-03 13:36:00Z tdnguyen $
//------------------------------------------------------------------------------
//                                 AcceptFilter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2017/01/03
//
/**
 * Definition for the AcceptFilter class used to define a data 
 * filter for an Estimator
 */
//------------------------------------------------------------------------------


#ifndef AcceptFilter_hpp
#define AcceptFilter_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"
#include "DataFilter.hpp"

class ESTIMATION_API AcceptFilter : public DataFilter
{
public:
   AcceptFilter(const std::string &ofType, std::string name);
   virtual ~AcceptFilter();
   AcceptFilter(const AcceptFilter& af);
   AcceptFilter& operator=(const AcceptFilter& af);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();
   
   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);

   /// This function is used to set tracking configs to data file object in order to use for data filter
   virtual bool         SetTrackingConfigs(StringArray tkconfigs);

   virtual ObservationData* 
                        FilteringData(ObservationData* dataObject, Integer& rejectedReason, Integer obDataId = -1);

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Variables used for estimation accepted filters
   // A list of record numbers of selceted observation data
   StringArray  recNumbers;
   IntegerArray recNumRanges;
   bool         allRecNumbers;
   bool         isRecNumbersSet;

   /// Variables used for statistics accepted filters
   /// Data thinning
   std::string thinMode;
   Integer     thinningFrequency;

   /// Record count map.  It contains record count for all measurement configs
   std::map<std::string,Integer> recCountMap;

   /// Start time window map. It is used for thinMode = "Time"
   std::map<std::string,GmatEpoch>   startTimeWindowMap;

   /// List of tracking configs used for data filter
   StringArray tkConfigList;

   /// Check data thinning ratio to be reached
   bool                 IsThin(ObservationData* dataObject);
   /// Get all available thin modes
   StringArray          GetAllAvailableThinModes();


   /// Class parameter ID enumeration
   enum
   {
      THIN_MODE = DataFilterParamCount,
      THINNING_FREQUENCY,
      RECORD_NUMS,
      AcceptFilterParamCount
   };

   // Start with the parameter IDs and associates strings
   /// Strings associated with the AcceptFilter parameters
   static const std::string
                PARAMETER_TEXT[AcceptFilterParamCount - DataFilterParamCount];
   /// Types of the AcceptFilter parameters
   static const Gmat::ParameterType
                PARAMETER_TYPE[AcceptFilterParamCount - DataFilterParamCount];

private:
   bool IsInRecordNumbersList(Integer recNum);

};

#endif /* AcceptFilter_hpp */
