//$Id: TrackingData.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             TrackingData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2010/02/19 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the TrackingData class
 */
//------------------------------------------------------------------------------


#ifndef TrackingData_hpp
#define TrackingData_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"

/**
 * TrackingData ...
 */
class ESTIMATION_API TrackingData : public GmatBase
{
public:
   TrackingData(const std::string &name);
   virtual ~TrackingData();
   TrackingData(const TrackingData& td);
   TrackingData& operator=(const TrackingData& td);

   virtual GmatBase* Clone() const;

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   // Uncomment if any parameters need to be hidden
   // virtual bool         IsParameterReadOnly(const Integer id) const;
   // virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;

   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   bool Initialize();

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Owned reference objects for this TrackingSystem
   std::string          trackingType;
   /// Tracking data file or files used by this tracking system
   StringArray          participantNames;

   /// Published parameters for the TrackingSystem objects
   enum
   {
      TYPE = GmatBaseParamCount,
      PARTICIPANTS,
      TrackingDataParamCount,
   };

   static const std::string
      PARAMETER_TEXT[TrackingDataParamCount - GmatBaseParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[TrackingDataParamCount - GmatBaseParamCount];

};

#endif /* TrackingData_hpp */
