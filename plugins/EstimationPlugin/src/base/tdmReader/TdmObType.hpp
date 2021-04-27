//$Id$
//------------------------------------------------------------------------------
//                            TdmObType
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Farideh Farahnak
// Created: 2014/8/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS.
//
/**
 * TdmObType class.
 */
//------------------------------------------------------------------------------

#ifndef TdmObType_hpp
#define TdmObType_hpp

#include "estimation_defs.hpp"
#include "ObType.hpp"
#include "TdmReadWriter.hpp"
#include "TFSMagicNumbers.hpp"
#include "RampTableData.hpp"


class ESTIMATION_API TdmObType : public ObType
{
public:
   TdmObType(const std::string withName = "");
   virtual ~TdmObType();
   TdmObType(const TdmObType &tot);
   TdmObType& operator=(const TdmObType &tot);

   virtual bool Initialize();
   virtual bool Open(bool forRead = true, bool forWrite = false, bool append = false);
   virtual bool IsOpen();
   virtual bool Close();
   virtual bool Finalize();
//   virtual ObsData *ReadObservation();
   virtual ObservationData *ReadObservation();
   virtual bool AddMeasurement(MeasurementData *md);
   virtual GmatBase *Clone() const;
   virtual RampTableData *ReadRampTableData();

protected:
   /// used for low level call to Xerces libraries
   TdmReadWriter *theReadWriter;
   /// An ObservationData framework pointer,  
   /// used to hold the framework information needed for the ObservationData, 
   /// parsed from the metadata in a TDM file.
   ObservationData *theTemplate;
//   ObsData *theTemplate;
   ///  A pointer to the TFSMagicNumbers singleton, 
   /// used to retrieve type information for observations.
   TFSMagicNumbers *typeIdentifier;

private:
   /// Flag used to initialize the first time an observation is read
   bool isFirstRead;
   /// Indicates if data has been validated (and appears to GMAT as "open")
   bool tdmPassedValidation;
};

#endif
