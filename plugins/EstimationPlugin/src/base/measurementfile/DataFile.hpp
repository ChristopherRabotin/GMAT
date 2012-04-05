//$Id: DataFile.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                                 DataFile
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/07/16
//
/**
 * Definition for the DataFile class used in GMAT measurement simulator and
 * estimators
 */
//------------------------------------------------------------------------------


#ifndef DataFile_hpp
#define DataFile_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"
#include "ObType.hpp"
#include "MeasurementData.hpp"
#include "ObservationData.hpp"


/**
 * DataFile is the container class for measurement data streams.
 *
 * The DataFile class provides the interfaces needed to script observation data
 * into GMAT.  Instances of the class identify the type of data stream used and
 * the identifier for that stream.
 */
class ESTIMATION_API DataFile : public GmatBase
{
public:
   DataFile(const std::string name);
   virtual ~DataFile();
   DataFile(const DataFile& df);
   DataFile& operator=(const DataFile& df);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();
   virtual bool Finalize();

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

   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");

   virtual bool         SetStream(ObType *thisStream);
   virtual bool         OpenStream(bool simulate = false);
   virtual bool         IsOpen();
   virtual void         WriteMeasurement(MeasurementData* theMeas);
   virtual ObservationData*
                        ReadObservation();
   virtual bool         CloseStream();

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// The stream for this DataFile
   ObType         *theDatastream;

   /// Name of the data stream
   std::string    streamName;
   /// Text description of the observation type
   std::string    obsType;

   /// Class parameter ID enumeration
   enum
   {
       StreamName = GmatBaseParamCount,
       ObsType,
       DataFileParamCount
   };

   // Start with the parameter IDs and associates strings
   /// Strings associated with the DataFile parameters
   static const std::string
                PARAMETER_TEXT[DataFileParamCount - GmatBaseParamCount];
   /// Types of the DataFile parameters
   static const Gmat::ParameterType
                PARAMETER_TYPE[DataFileParamCount - GmatBaseParamCount];
};

#endif /* DataFile_hpp */
