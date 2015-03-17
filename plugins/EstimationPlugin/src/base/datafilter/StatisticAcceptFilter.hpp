//$Id: StatisticAcceptFilter.hpp 1398 2015-03-03 13:36:00Z tdnguyen $
//------------------------------------------------------------------------------
//                                 StatisticAcceptFilter
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
// Created: 2015/03/03
//
/**
 * Definition for the SatisticAcceptFilter class used to define a data accepting 
 * filter for a TrackingFile
 */
//------------------------------------------------------------------------------


#ifndef StatisticAcceptFilter_hpp
#define StatisticAcceptFilter_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"
#include "DataFilter.hpp"

class ESTIMATION_API StatisticAcceptFilter : public DataFilter
{
public:
   StatisticAcceptFilter(const std::string name);
   virtual ~StatisticAcceptFilter();
   StatisticAcceptFilter(const StatisticAcceptFilter& saf);
   StatisticAcceptFilter& operator=(const StatisticAcceptFilter& saf);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();
   
   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
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



   virtual ObservationData* 
                        FilteringData(ObservationData* dataObject);
//   virtual bool         ValidateInput();
   StringArray          GetAllAvailableThinModes();

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// A list of file names specifies data files on which the filter to be applied 
   StringArray fileNames;
   /// Data thinning
   std::string thinMode;
   Integer     thinningFrequency;

   /// Record count;
   Integer recCount;

   /// Class parameter ID enumeration
   enum
   {
      FILENAMES = DataFilterParamCount,
      THIN_MODE,
      THINNING_FREQUENCY,
      StatisticAcceptFilterParamCount
   };

   // Start with the parameter IDs and associates strings
   /// Strings associated with the StatisticAcceptFilter parameters
   static const std::string
                PARAMETER_TEXT[StatisticAcceptFilterParamCount - DataFilterParamCount];
   /// Types of the StatisticAcceptFilter parameters
   static const Gmat::ParameterType
                PARAMETER_TYPE[StatisticAcceptFilterParamCount - DataFilterParamCount];


};

#endif /* StatisticAcceptFilter_hpp */
