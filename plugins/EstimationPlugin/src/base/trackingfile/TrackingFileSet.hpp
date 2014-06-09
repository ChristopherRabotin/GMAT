//$Id$
//------------------------------------------------------------------------------
//                           TrackingFileSet
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
// Created: Mar 11, 2014
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef TrackingFileSet_hpp
#define TrackingFileSet_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"
#include "MeasurementModelBase.hpp"
#include "TrackingDataAdapter.hpp"
#include "DataFile.hpp"

class SolarSystem;

class ESTIMATION_API TrackingFileSet: public MeasurementModelBase
{
public:
   TrackingFileSet(const std::string &name);
   virtual ~TrackingFileSet();
   TrackingFileSet(const TrackingFileSet& tfs);
   TrackingFileSet& operator=(const TrackingFileSet& tfs);

   virtual GmatBase* Clone() const;

   // methods overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
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

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const Integer id,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value,
                                            const Integer index);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value,
                                            const Integer index);

   DEFAULT_TO_NO_CLONES

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

   virtual void SetSolarSystem(SolarSystem *ss);
   virtual bool Initialize();
   std::vector<TrackingDataAdapter*> *GetAdapters();

protected:
   /// The collection of measurements in the set
   std::vector<TrackingDataAdapter*> measurements;
   /// The strings describing the tracking configs
   StringArray trackingConfigs;
   /// Name of the associate tracking data file
   StringArray filenames;
   /// Flag for the inclusion of light time solution
   bool useLighttime;

   /// Solar system used in the measurements
   SolarSystem *solarsystem;
   /// Pointers to ref objects that the adapters use
   ObjectArray references;
   /// Pointers to the Datafile Objects specified for this TFS
   std::vector<DataFile*> datafiles;

   /// Parameter IDs for the TrackingFileSets
   enum
   {
      TRACKINGCONFIG = GmatBaseParamCount,
      FILENAME,
      USELIGHTTIME,
      TrackingFileSetParamCount,
   };

   /// Strings describing the BatchEstimator parameters
   static const std::string PARAMETER_TEXT[TrackingFileSetParamCount -
                                           GmatBaseParamCount];
   /// Types of the BatchEstimator parameters
   static const Gmat::ParameterType PARAMETER_TYPE[TrackingFileSetParamCount -
                                                   GmatBaseParamCount];

   TrackingDataAdapter *BuildAdapter(const StringArray &strand,
         const std::string &type);
//   bool BuildDatafiles();
};

#endif /* TrackingFileSet_hpp */
