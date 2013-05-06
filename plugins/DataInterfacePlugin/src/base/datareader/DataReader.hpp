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

class DATAINTERFACE_API DataReader: public GmatBase
{
public:
   DataReader(const std::string& theTypeName, const std::string& theName);
   virtual ~DataReader();
   DataReader(const DataReader& dr);
   DataReader& operator=(const DataReader& dr);

   virtual const StringArray& GetSelectedFieldNames();
   virtual const StringArray& GetSupportedFieldNames();

protected:
   /// Array of the data fields the user has selected
   StringArray selectedFields;
   /// List of field names that the data reader supports
   StringArray supportedFields;


   /// Parameter IDs
   enum
   {
      SELECTED_FIELD_NAMES = GmatBaseParamCount,
      SUPPORTED_FIELD_NAMES,
      DataReaderParamCount,
   };

   /// Interface parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[DataReaderParamCount - GmatBaseParamCount];
   /// Interface parameter labels
   static const std::string PARAMETER_LABEL[DataReaderParamCount - GmatBaseParamCount];
};

#endif /* DataReader_hpp */
