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

#include "DataReader.hpp"

const std::string
DataReader::PARAMETER_LABEL[DataReaderParamCount - GmatBaseParamCount] =
{
      "SelectedFieldNames",
      "SupportedFieldNames",
};

const Gmat::ParameterType
DataReader::PARAMETER_TYPE[DataReaderParamCount - GmatBaseParamCount] =
{
      Gmat::STRINGARRAY_TYPE,
      Gmat::STRINGARRAY_TYPE,
};


DataReader::DataReader(const std::string& theTypeName, const std::string& theName) :
   GmatBase          (Gmat::DATAINTERFACE_SOURCE, theTypeName, theName)
{
   // Engine required fields
   objectTypes.push_back(Gmat::DATAINTERFACE_SOURCE);
   objectTypeNames.push_back("DataReader");
}

DataReader::~DataReader()
{
}

DataReader::DataReader(const DataReader& dr) :
   GmatBase          (dr),
   selectedFields    (dr.selectedFields),
   supportedFields   (dr.supportedFields)
{
}

DataReader& DataReader::operator=(const DataReader& dr)
{
   if (this != &dr)
   {
      selectedFields  = dr.selectedFields;
      supportedFields = dr.supportedFields;
   }

   return *this;
}

const StringArray& DataReader::GetSelectedFieldNames()
{
   return selectedFields;
}

const StringArray& DataReader::GetSupportedFieldNames()
{
   // Leaf classes fill in the field identifiers; we just return that list here
   return supportedFields;
}
