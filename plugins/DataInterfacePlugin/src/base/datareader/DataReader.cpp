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


//const std::string
//DataReader::PARAMETER_LABEL[DataReaderParamCount - GmatBaseParamCount] =
//{
//      "SelectedFieldNames",
//      "SupportedFieldNames",
//};
//
//const Gmat::ParameterType
//DataReader::PARAMETER_TYPE[DataReaderParamCount - GmatBaseParamCount] =
//{
//      Gmat::STRINGARRAY_TYPE,
//      Gmat::STRINGARRAY_TYPE,
//};


DataReader::DataReader(const std::string& theTypeName, const std::string& theName) :
   GmatBase                (Gmat::DATAINTERFACE_SOURCE, theTypeName, theName),
   readAllSupportedFields  (true),
   dataReady               (false),
   clearOnRead             (true)
{
   // Engine required fields
   objectTypes.push_back(Gmat::DATAINTERFACE_SOURCE);
   objectTypeNames.push_back("DataReader");
}

DataReader::~DataReader()
{
}

DataReader::DataReader(const DataReader& dr) :
   GmatBase                (dr),
   selectedFields          (dr.selectedFields),
   supportedFields         (dr.supportedFields),
   readAllSupportedFields  (dr.readAllSupportedFields),
   dataReady               (false),
   clearOnRead             (dr.clearOnRead),
   fileStringMap           (dr.fileStringMap),
   objectStringMap         (dr.objectStringMap),
   objectIDMap             (dr.objectIDMap),
   dataType                (dr.dataType)
{
}

DataReader& DataReader::operator=(const DataReader& dr)
{
   if (this != &dr)
   {
      selectedFields         = dr.selectedFields;
      supportedFields        = dr.supportedFields;
      readAllSupportedFields = dr.readAllSupportedFields;
      dataReady              = false;
      clearOnRead            = dr.clearOnRead;
      fileStringMap          = dr.fileStringMap;
      objectStringMap        = dr.objectStringMap;
      objectIDMap            = dr.objectIDMap;
      dataType               = dr.dataType;
   }

   return *this;
}

const StringArray& DataReader::GetSelectedFieldNames()
{
   return selectedFields;
}

void DataReader::SetSelectedFieldNames(const StringArray& selections)
{
   selectedFields = selections;
}

const StringArray& DataReader::GetSupportedFieldNames()
{
   // Leaf classes fill in the field identifiers; we just return that list here
   return supportedFields;
}

bool DataReader::WasDataLoaded(const std::string &theField)
{
   return dataLoaded[theField];
}

void DataReader::ClearData()
{
   for (std::map<std::string, bool>::iterator i = dataLoaded.begin(); 
        i != dataLoaded.end(); ++i)
      (*i).second = false;
   dataReady = false;
}

Real DataReader::GetRealValue(const std::string& forField)
{
   Real retval = -999999.999999;

   if (dataReady)
   {
      retval = GetRData(forField);
   }

   return retval;
}

Rvector6 DataReader::GetReal6Vector(const std::string& forField)
{
   Rvector6 retval(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

   if (dataReady)
   {
      retval = GetRVectorData(forField);
   }

   return retval;
}

std::string DataReader::GetStringValue(const std::string& forField)
{
   std::string retval = "No data";

   if (dataReady)
   {
      retval = GetSData(forField);
   }

   return retval;
}

bool DataReader::UsesCoordinateSystem(const std::string& forField)
{
   return false;
}

std::string DataReader::GetCoordinateSystemName(const std::string& forField)
{
   return "";
}

bool DataReader::UsesOrigin(const std::string& forField)
{
   return false;
}

std::string DataReader::GetOriginName(const std::string& forField)
{
   return "";
}

bool DataReader::UsesTimeSystem(const std::string& forField)
{
   return false;
}

std::string DataReader::GetTimeSystemName(const std::string& forField)
{
   return "";
}

bool DataReader::SetStream(std::ifstream* aStream, const std::string &fname)
{
   return false;
}

//------------------------------------------------------------------------------
// const std::string GetObjectParameterName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Gets the object parameter name for a data field
 *
 * @param forField The field name
 *
 * @return The scripted parameter name
 */
//------------------------------------------------------------------------------
const std::string DataReader::GetObjectParameterName(
      const std::string& forField)
{
   if (objectStringMap.find(forField) != objectStringMap.end())
      return objectStringMap[forField];
   return "";
}

//------------------------------------------------------------------------------
// const readerDataType GetReaderDataType(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Returns the data type as seen in the DataReader object.
 *
 * @param forField The interface identifier for the parameter.
 *
 * @return The readerDataType for the field.
 */
//------------------------------------------------------------------------------
const DataReader::readerDataType DataReader::GetReaderDataType(
      const std::string& forField)
{
   readerDataType theType = DataReader::READER_UNKNOWN;

   if (dataType.find(forField) != dataType.end())
      theType = dataType[forField];

   return theType;
}

Real DataReader::GetRData(const std::string& forField)
{
   Real retval = -999999.999999;

   if (realData.find(forField) != realData.end())
      retval = realData[forField];

   return retval;
}

Rvector6 DataReader::GetRVectorData(const std::string& forField)
{
   Rvector6 retval(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

   if (rvector6Data.find(forField) != rvector6Data.end())
      retval = rvector6Data[forField];

   return retval;
}

//------------------------------------------------------------------------------
// std::string GetSData(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves string data
 *
 * @param forField The field that references teh string
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string DataReader::GetSData(const std::string& forField)
{
   std::string retval = "No data";

   if (stringData.find(forField) != stringData.end())
      retval = stringData[forField];

   return retval;
}
