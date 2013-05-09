//$Id$
//------------------------------------------------------------------------------
//                           DataReader
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
// Created: May 2, 2013
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef DataReader_hpp
#define DataReader_hpp

#include "DataInterfaceDefs.hpp"
#include "GmatBase.hpp"
#include "Rvector6.hpp"



class DATAINTERFACE_API DataReader: public GmatBase
{
public:
   DataReader(const std::string& theTypeName, const std::string& theName);
   virtual ~DataReader();
   DataReader(const DataReader& dr);
   DataReader& operator=(const DataReader& dr);

   // File interfaces, override for FileReaders
   virtual bool SetStream(std::ifstream* aStream);

   virtual const StringArray& GetSelectedFieldNames();
   virtual void SetSelectedFieldNames(const StringArray& selections);
   virtual const StringArray& GetSupportedFieldNames();

   virtual bool ReadData() = 0;
   virtual void ClearData();

   // Accessors; these are intentionally NOT virtual, and call virtual protected
   // methods intended for subclass overrides that actually access the data
   Real GetRealValue(const std::string& forField);
   Rvector6 GetReal6Vector(const std::string& forField);
   std::string GetStringValue(const std::string& forField);


protected:
   /// Array of the data fields the user has selected
   StringArray selectedFields;
   /// List of field names that the data reader supports
   StringArray supportedFields;
   /// Flag indicating if the data is available
   bool dataReady;
   /// Flag indicating is a new read clears previous data or overwrites it
   bool clearOnRead;
   /// Mapping for the Real data
   std::map <std::string, Real> realData;
   /// Mapping for the Rvector6 data
   std::map <std::string, Rvector6> rvector6Data;
   /// Mapping for the string data
   std::map <std::string, std::string> stringData;

   // Methods that subclasses may need to override.  Put any additional
   // processing in these methods
   virtual Real GetRData(const std::string &forField);
   virtual Rvector6 GetRVectorData(const std::string &forField);
   virtual std::string GetSData(const std::string &forField);

   /// TBD: Go we need the GmatBase parameters in this internal object?
//   /// Parameter IDs
//   enum
//   {
//      SELECTED_FIELD_NAMES = GmatBaseParamCount,
//      SUPPORTED_FIELD_NAMES,
//      DataReaderParamCount,
//   };
//
//   /// Interface parameter types
//   static const Gmat::ParameterType PARAMETER_TYPE[DataReaderParamCount - GmatBaseParamCount];
//   /// Interface parameter labels
//   static const std::string PARAMETER_LABEL[DataReaderParamCount - GmatBaseParamCount];
};

#endif /* DataReader_hpp */
