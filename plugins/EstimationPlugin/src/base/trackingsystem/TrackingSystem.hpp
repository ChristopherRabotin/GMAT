//$Id: TrackingSystem.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             TrackingSystem
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2010/02/18 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the TrackingSystem class
 */
//------------------------------------------------------------------------------


#ifndef TrackingSystem_hpp
#define TrackingSystem_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"
#include "DataFile.hpp"
#include "MeasurementModel.hpp"

/**
 * Defines a tracking system used in the simulation and estimation processes
 */
class ESTIMATION_API TrackingSystem : public GmatBase
{
public:
   TrackingSystem(const std::string &type, const std::string &name);
   virtual ~TrackingSystem();
   TrackingSystem(const TrackingSystem& ts);
   TrackingSystem& operator=(const TrackingSystem& ts);

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

   // Access methods for the reference objects
   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         SetRefObjectName(const Gmat::ObjectType type,
                                         const std::string &name);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);

   // TrackingSystem Specific Methods
   virtual bool         Initialize();

   virtual UnsignedInt  GetMeasurementCount();
   virtual MeasurementModel*
                        GetMeasurement(Integer i);

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES

protected:
   /// Owned reference objects for this TrackingSystem
   StringArray                      trackingDataNames;
   /// Tracking data file or files used by this tracking system
   StringArray                      trackingFiles;
   /// Allowed measurement types for the tracking system
   IntegerArray                     allowedMeasurements;

   /// The measurements associated with this tracking system
   std::vector<MeasurementModel*>   measurements;
   /// The files used with this tracking system
   std::vector<DataFile*>           datafiles;

   /// Local storage element for ref object names
   StringArray                      refObjectList;

   /// Media correction models used in tracking system
   std::string troposphereModel;
   std::string ionosphereModel;

   /// Published parameters for the TrackingSystem objects
   enum
   {
      ADD_DATA = GmatBaseParamCount,
      FILELIST,
      TROPOSPHERE_MODEL,
      IONOSPHERE_MODEL,
      TrackingSystemParamCount,
   };

   static const std::string
      PARAMETER_TEXT[TrackingSystemParamCount - GmatBaseParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[TrackingSystemParamCount - GmatBaseParamCount];
};

#endif /* TrackingSystem_hpp */
