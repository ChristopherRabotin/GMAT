//------------------------------------------------------------------------------
//                           ThrustHistoryFile
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * 
 */
//------------------------------------------------------------------------------


#include "ThrustHistoryFile.hpp"
#include "MessageInterface.hpp"

const std::string ThrustHistoryFile::PARAMETER_LABEL[ThrustHistoryFileParamCount - GmatBaseParamCount] =
{
      "FileName",                         // FILENAME
      "AddThrustSegment",                 // SEGMENTS,
      "MassSource"                        // MASS_SOURCE,
};

const Gmat::ParameterType ThrustHistoryFile::PARAMETER_TYPE[ThrustHistoryFileParamCount - GmatBaseParamCount] =
{
      Gmat::FILENAME_TYPE,
      Gmat::STRINGARRAY_TYPE,
      Gmat::STRINGARRAY_TYPE
};



ThrustHistoryFile::ThrustHistoryFile(const std::string& theName) :
   FileReader        ("ThrustHistoryFile", theName),
   thrustFileName    ("")
{
   objectTypes.push_back(Gmat::INTERFACE);
   objectTypeNames.push_back("ThrustHistoryFile");
//   parameterCount = ImpulsiveBurnParamCount;
}

ThrustHistoryFile::~ThrustHistoryFile()
{
}

ThrustHistoryFile::ThrustHistoryFile(const ThrustHistoryFile& thf) :
   FileReader        (thf),
   thrustFileName    (thf.thrustFileName)
{
}

ThrustHistoryFile& ThrustHistoryFile::operator =(const ThrustHistoryFile& thf)
{
   if (this != &thf)
   {
      FileReader::operator =(thf);

      thrustFileName = thf.thrustFileName;
   }

   return *this;
}

GmatBase* ThrustHistoryFile::Clone() const
{
   return new ThrustHistoryFile(*this);
}

bool ThrustHistoryFile::RenameRefObject(Gmat::ObjectType type,
      const std::string& oldname, const std::string& newname)
{
   bool retval = false;

   return retval;
}

bool ThrustHistoryFile::ReadData()
{
   bool retval = false;

   return retval;
}

std::string ThrustHistoryFile::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ThrustHistoryFileParamCount)
      return PARAMETER_LABEL[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}

Integer ThrustHistoryFile::GetParameterID(const std::string& str) const
{
   Integer id = -1;

   for (Integer i = GmatBaseParamCount; i < ThrustHistoryFileParamCount; i++)
    {
       if (str == PARAMETER_LABEL[i - GmatBaseParamCount])
       {
          id = i;
          break;
       }
    }

    if (id != -1)
    {
       #ifdef DEBUG_BURN_PARAM
       MessageInterface::ShowMessage("Burn::GetParameterID() returning %d\n", id);
       #endif

       return id;
    }

    return GmatBase::GetParameterID(str);
}

Gmat::ParameterType ThrustHistoryFile::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ThrustHistoryFileParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}

std::string ThrustHistoryFile::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

std::string ThrustHistoryFile::GetStringParameter(const Integer id) const
{
   if (id == FILENAME)
      return thrustFileName;

   return FileReader::GetStringParameter(id);
}

bool ThrustHistoryFile::SetStringParameter(const Integer id, const char* value)
{
   MessageInterface::ShowMessage("ThrustHistoryFile::SetStringParameter(%d, %s)\n",
         id, value);

   if (id == FILENAME)
   {
      thrustFileName = value;
      return true;
   }

   if (id == SEGMENTS)
   {
      MessageInterface::ShowMessage("Block string %s\n", value);
      if (find(segmentNames.begin(), segmentNames.end(), value) == segmentNames.end())
         segmentNames.push_back(value);
      return true;
   }

   if (id == MASS_SOURCE)
   {
      MessageInterface::ShowMessage("Mass %s\n", value);
      return true;
   }

   return FileReader::SetStringParameter(id, value);
}

bool ThrustHistoryFile::SetStringParameter(const Integer id,
      const std::string& value)
{
   MessageInterface::ShowMessage("ThrustHistoryFile::SetStringParameter(%d, %s)\n",
         id, value.c_str());
   if (id == FILENAME)
   {
      thrustFileName = value;
      return true;
   }

   if (id == SEGMENTS)
   {
      MessageInterface::ShowMessage("Block string %s\n", value.c_str());
      if (find(segmentNames.begin(), segmentNames.end(), value) == segmentNames.end())
         segmentNames.push_back(value);
      return true;
   }

   if (id == MASS_SOURCE)
   {
      MessageInterface::ShowMessage("Mass %s\n", value.c_str());
      return true;
   }

   return FileReader::SetStringParameter(id, value);
}

std::string ThrustHistoryFile::GetStringParameter(const Integer id,
      const Integer index) const
{
   return FileReader::GetStringParameter(id, index);
}

bool ThrustHistoryFile::SetStringParameter(const Integer id, const char* value,
      const Integer index)
{
   MessageInterface::ShowMessage("ThrustHistoryFile::SetStringParameter(%d, %s, %d)\n",
         id, value, index);

   if (id == MASS_SOURCE)
   {
      MessageInterface::ShowMessage("Mass %s\n", value);
      return true;
   }

   return FileReader::SetStringParameter(id, value, index);
}

bool ThrustHistoryFile::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   MessageInterface::ShowMessage("ThrustHistoryFile::SetStringParameter(%d, %s, %d)\n",
         id, value.c_str(), index);


   if (id == MASS_SOURCE)
   {
      MessageInterface::ShowMessage("Mass %s\n", value.c_str());
      return true;
   }

   return FileReader::SetStringParameter(id, value, index);
}

const StringArray& ThrustHistoryFile::GetStringArrayParameter(
      const Integer id) const
{
   return FileReader::GetStringArrayParameter(id);
}

const StringArray& ThrustHistoryFile::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   return FileReader::GetStringArrayParameter(id, index);
}

std::string ThrustHistoryFile::GetStringParameter(
      const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}

bool ThrustHistoryFile::SetStringParameter(const std::string& label,
      const char* value)
{
   MessageInterface::ShowMessage("ThrustHistoryFile::SetStringParameter(%s, %s)\n", label.c_str(), value);
   return SetStringParameter(GetParameterID(label), value);
}

bool ThrustHistoryFile::SetStringParameter(const std::string& label,
      const std::string& value)
{
   MessageInterface::ShowMessage("ThrustHistoryFile::SetStringParameter(%s, %s)\n", label.c_str(), value.c_str());
   return SetStringParameter(GetParameterID(label), value);
}

std::string ThrustHistoryFile::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

bool ThrustHistoryFile::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

const StringArray& ThrustHistoryFile::GetStringArrayParameter(
      const std::string& label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

const StringArray& ThrustHistoryFile::GetStringArrayParameter(
      const std::string& label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}
