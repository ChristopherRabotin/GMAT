//$Id: ObType.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                                 ObType
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
//         Based on code by Matthew P. Wilkins, Shafer Corporation
// Created: 2009/07/06
//
/**
 * Defines the base class for observations in a data file or database.
 */
//------------------------------------------------------------------------------


#ifndef ObType_hpp
#define ObType_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"
#include "MeasurementData.hpp"
#include "ObservationData.hpp"


/**
 * The ObType classes are structures designed to contain observation data --
 * either calculated or observed -- in a single packet designed to be passed
 * between the different consumers of the contained data.  Derived classes are
 * implemented for each data file type that is supported.
 */
class ESTIMATION_API ObType : public GmatBase
{
public:
   ObType(const std::string &obType = "NotSet", const std::string &name = "");
   virtual ~ObType();
   ObType(const ObType& ot);
   ObType&           operator=(const ObType& ot);

   virtual bool      Initialize();
   virtual bool      Open(bool forRead = true, bool forWrite= false,
                          bool append = false);
   virtual bool      IsOpen();
   virtual bool      Close();
   virtual bool      Finalize();

   void              SetStreamName(std::string name);

   // Abstract methods
   /**
    * Method used to add a measurement to the stream.
    */
   virtual bool      AddMeasurement(MeasurementData *md) = 0;
   /**
    * Method used to retrieve a measurement from the stream.
    */
   virtual ObservationData *
                     ReadObservation() = 0;

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Name of the stream contining the data
   std::string       streamName;
   /// String used for header information
   std::string       header;
   /// Flag used to indicate of the stream should open for reading
   bool              openForRead;
   /// Flag used to indicate of the stream should open for writing
   bool              openForWrite;
};

#endif /* ObType_hpp */
