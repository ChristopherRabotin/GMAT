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
};

const Gmat::ParameterType
DataInterface::PARAMETER_TYPE[DataInterfaceParamCount - InterfaceParamCount] =
{
      Gmat::STRING_TYPE,
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

DataInterface& DataInterface::operator =(const DataInterface& di)
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
   if (id == SUPPORTED_FIELD_NAMES)
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

Integer DataInterface::Close(const std::string& name)
{
   Integer retval = -1;


   return retval;
}
