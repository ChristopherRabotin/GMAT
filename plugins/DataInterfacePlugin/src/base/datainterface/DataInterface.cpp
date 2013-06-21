//$Id$
//------------------------------------------------------------------------------
//                           DataInterface
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

#include "DataInterface.hpp"
#include "InterfaceException.hpp"


const std::string
DataInterface::PARAMETER_LABEL[DataInterfaceParamCount - InterfaceParamCount] =
{
      "Format",
      "SelectedFields",
      "SupportedFields"
};

const Gmat::ParameterType
DataInterface::PARAMETER_TYPE[DataInterfaceParamCount - InterfaceParamCount] =
{
      Gmat::STRING_TYPE,
      Gmat::STRINGARRAY_TYPE,
      Gmat::STRINGARRAY_TYPE
};



DataInterface::DataInterface(const std::string &type, const std::string &name) :
   Interface         (type, name),
   readerFormat      (""),
   theReader         (NULL)
{
   // Engine required fields
   objectTypes.push_back(Gmat::INTERFACE);
   objectTypeNames.push_back("DataInterface");
}

DataInterface::~DataInterface()
{
}

DataInterface::DataInterface(const DataInterface& di) :
   Interface         (di),
   readerFormat      (di.readerFormat),
   theReader         (NULL)
{
}

DataInterface& DataInterface::operator=(const DataInterface& di)
{
   if (this != &di)
   {
      readerFormat = di.readerFormat;
      if (theReader)
         delete theReader;
      theReader = NULL;
   }

   return *this;
}

std::string DataInterface::GetParameterText(const Integer id) const
{
   if (id >= InterfaceParamCount && id < DataInterfaceParamCount)
      return PARAMETER_LABEL[id - InterfaceParamCount];
   return Interface::GetParameterText(id);
}

Integer DataInterface::GetParameterID(const std::string& str) const
{
   Integer id = -1;

   for (Integer i = InterfaceParamCount; i < DataInterfaceParamCount; i++)
   {
      if (str == PARAMETER_LABEL[i - InterfaceParamCount])
      {
         id = i;
         break;
      }
   }

   if (id != -1)
   {
      #ifdef DEBUG_DATAINTERFACE_PARAM
      MessageInterface::ShowMessage("DataInterface::GetParameterID() returning "
            "%d\n", id);
      #endif

      return id;
   }

   return Interface::GetParameterID(str);
}

Gmat::ParameterType DataInterface::GetParameterType(const Integer id) const
{
   if (id >= InterfaceParamCount && id < DataInterfaceParamCount)
      return PARAMETER_TYPE[id - InterfaceParamCount];

   return Interface::GetParameterType(id);
}

std::string DataInterface::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

bool DataInterface::IsParameterReadOnly(const Integer id) const
{
   if ((id == SUPPORTED_FIELD_NAMES) || (id == SELECTED_FIELD_NAMES))
      return true;
   return Interface::IsParameterReadOnly(id);
}

bool DataInterface::IsParameterReadOnly(const std::string& label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

std::string DataInterface::GetStringParameter(const Integer id) const
{
   if (id == FORMAT)
      return readerFormat;

   return Interface::GetStringParameter(id);
}

bool DataInterface::SetStringParameter(const Integer id,
      const std::string& value)
{
   if (id == FORMAT)
   {
      readerFormat = value;
      return true;
   }

   return Interface::SetStringParameter(id, value);
}

std::string DataInterface::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == SELECTED_FIELD_NAMES)
   {
      if (theReader)
      {
         StringArray selections = theReader->GetSelectedFieldNames();
         if ((index >= 0) && (index < (Integer)selections.size()))
            return selections[index];
         throw InterfaceException("The selection index is out of bounds in the "
               "DataReader \"" + instanceName + "\"");
      }
      throw InterfaceException("The DataReader has not been set in the "
            "DataInterface \"" + instanceName + "\"");
   }

   if (id == SUPPORTED_FIELD_NAMES)
   {
      if (theReader)
      {
         StringArray selections = theReader->GetSupportedFieldNames();
         if ((index >= 0) && (index < (Integer)selections.size()))
            return selections[index];
         throw InterfaceException("The supported field index is out of bounds "
               "in the DataReader \"" + instanceName + "\"");
      }
      throw InterfaceException("The DataReader has not been set in the "
            "DataInterface \"" + instanceName + "\"");
   }

   return Interface::GetStringParameter(id, index);
}

bool DataInterface::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   if (id == SELECTED_FIELD_NAMES)
   {
      if (theReader)
         return theReader->SetStringParameter("SelectedFieldNames", value,
               index);
      throw InterfaceException("The DataReader has not been set in the "
            "DataInterface \"" + instanceName + "\"");
   }

   if (id == SUPPORTED_FIELD_NAMES)
   {
      if (theReader)
         return theReader->SetStringParameter("SupportedFieldNames", value,
               index);
      throw InterfaceException("The DataReader has not been set in the "
            "DataInterface \"" + instanceName + "\"");
   }

   return Interface::SetStringParameter(id, value, index);
}

std::string DataInterface::GetStringParameter(const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}

bool DataInterface::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}

std::string DataInterface::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

bool DataInterface::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

const StringArray& DataInterface::GetStringArrayParameter(
      const Integer id) const
{
   if (id == SELECTED_FIELD_NAMES)
   {
      if (theReader)
         return theReader->GetSelectedFieldNames();
      throw InterfaceException("The DataReader has not been set in the "
            "DataInterface \"" + instanceName + "\"");
   }

   if (id == SUPPORTED_FIELD_NAMES)
   {
      return GetSupportedFieldNames();
   }

   return Interface::GetStringArrayParameter(id);
}

const StringArray& DataInterface::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   return Interface::GetStringArrayParameter(id, index);
}

const StringArray& DataInterface::GetStringArrayParameter(
      const std::string& label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

const StringArray& DataInterface::GetStringArrayParameter(
      const std::string& label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}

const StringArray& DataInterface::GetSupportedFieldNames() const
{
   if (theReader)
      return theReader->GetSupportedFieldNames();
   throw InterfaceException("The DataReader has not been set in the "
         "DataInterface \"" + instanceName + "\"");
}

Integer DataInterface::Open(const std::string& name)
{
   Integer retval = -1;
   return retval;
}

bool DataInterface::WasDataLoaded(const std::string& forField)
{
   return theReader->WasDataLoaded(forField);
}

//------------------------------------------------------------------------------
// const std::string GetObjectParameterName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves the parameter string for the target object
 *
 * @param forField The interface identifier for the parameter.  This is likely
 *                 to be the parameter name in many cases
 *
 * @return The parameter name
 */
//------------------------------------------------------------------------------
const std::string DataInterface::GetObjectParameterName(
      const std::string& forField)
{
   return theReader->GetObjectParameterName(forField);
}

//------------------------------------------------------------------------------
// const DataReader::readerDataType GetReaderParameterType(
//       const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Returns the data type as seen in the DataReader object.
 *
 * @param forField The interface identifier for the parameter.
 *
 * @return The readerDataType for the field.
 */
//------------------------------------------------------------------------------
const DataReader::readerDataType DataInterface::GetReaderParameterType(
      const std::string& forField)
{
   DataReader::readerDataType theType = DataReader::READER_UNKNOWN;

   if (theReader)
      theType = theReader->GetReaderDataType(forField);

   return theType;
}

//------------------------------------------------------------------------------
// Integer Close(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Closes the reader
 *
 * @param name Name of the reader to close.  This parameter is ignored for
 *             DataReader objects
 *
 * @return 0 in success, or an error code
 */
//------------------------------------------------------------------------------
Integer DataInterface::Close(const std::string& name)
{
   Integer retval = -1;
   return retval;
}

//------------------------------------------------------------------------------
// Real GetRealValue(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Pass-through function to access a read Real number
 *
 * @param forField The field identifier for the data
 *
 * @return The number
 */
//------------------------------------------------------------------------------
Real DataInterface::GetRealValue(const std::string& forField)
{
   if (!theReader)
      throw InterfaceException("The data reader has not been set");
   return theReader->GetRealValue(forField);
}

Rvector6 DataInterface::GetReal6Vector(const std::string& forField)
{
   if (!theReader)
      throw InterfaceException("The data reader has not been set");
   return theReader->GetReal6Vector(forField);
}

std::string DataInterface::GetStringValue(const std::string& forField)
{
   if (!theReader)
      throw InterfaceException("The data reader has not been set");
   return theReader->GetStringValue(forField);
}

bool DataInterface::UsesCoordinateSystem(const std::string& forField)
{
   bool retval = false;
   if (theReader)
      retval = theReader->UsesCoordinateSystem(forField);
   return retval;
}

std::string DataInterface::GetCoordinateSystemName(const std::string& forField)
{
   std::string retval = "";
   if (theReader)
      retval = theReader->GetCoordinateSystemName(forField);
   return retval;
}

bool DataInterface::UsesOrigin(const std::string& forField)
{
   bool retval = false;
   if (theReader)
      retval = theReader->UsesOrigin(forField);
   return retval;
}

std::string DataInterface::GetOriginName(const std::string& forField)
{
   std::string retval = "";
   if (theReader)
      retval = theReader->GetOriginName(forField);
   return retval;
}

bool DataInterface::UsesTimeSystem(const std::string& forField)
{
   bool retval = false;
   if (theReader)
      retval = theReader->UsesTimeSystem(forField);
   return retval;
}

std::string DataInterface::GetTimeSystemName(const std::string& forField)
{
   std::string retval = "";
   if (theReader)
      retval = theReader->GetTimeSystemName(forField);
   return retval;
}
