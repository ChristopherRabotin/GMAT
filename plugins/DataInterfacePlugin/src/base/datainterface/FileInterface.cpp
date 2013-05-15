//$Id$
//------------------------------------------------------------------------------
//                           FileInterface
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

#include "FileInterface.hpp"
#include "FileUtil.hpp"
#include "ReaderFactory.hpp"        // Not a registered factory for now, but it
                                    // uses the factory interfaces
#include "MessageInterface.hpp"
#include "InterfaceException.hpp"


//#define DEBUG_FILEINTERFACE_PARAM
//#define DEBUG_READER_CREATION


const std::string
FileInterface::PARAMETER_LABEL[FileInterfaceParamCount - DataInterfaceParamCount] =
{
      "Filename",
};

const Gmat::ParameterType
FileInterface::PARAMETER_TYPE[FileInterfaceParamCount - DataInterfaceParamCount] =
{
      Gmat::STRING_TYPE,
};


FileInterface::FileInterface(const std::string &name) :
   DataInterface           ("FileInterface", name),
   filename                (""),
   streamIsBinary          (false)
{
   objectTypeNames.push_back("FileInterface");
   parameterCount = FileInterfaceParamCount;
}

FileInterface::~FileInterface()
{
   if (theReader != NULL)
      delete theReader;
}

FileInterface::FileInterface(const FileInterface& fi) :
   DataInterface           (fi),
   filename                (fi.filename),
   streamIsBinary          (fi.streamIsBinary)
{
}

FileInterface FileInterface::operator=(const FileInterface& fi)
{
   if (this != &fi)
   {
      DataInterface::operator =(fi);

      filename = fi.filename;
      streamIsBinary = fi.streamIsBinary;
   }
   return *this;
}

GmatBase* FileInterface::Clone() const
{
   return new FileInterface(*this);
}

std::string FileInterface::GetParameterText(const Integer id) const
{
   if (id >= DataInterfaceParamCount && id < FileInterfaceParamCount)
      return PARAMETER_LABEL[id - DataInterfaceParamCount];
   return DataInterface::GetParameterText(id);
}

Integer FileInterface::GetParameterID(const std::string& str) const
{
   Integer id = -1;

   for (Integer i = DataInterfaceParamCount; i < FileInterfaceParamCount; i++)
   {
      if (str == PARAMETER_LABEL[i - DataInterfaceParamCount])
      {
         id = i;
         break;
      }
   }

   if (id == -1)
   {
      id = DataInterface::GetParameterID(str);
   }

   #ifdef DEBUG_FILEINTERFACE_PARAM
   MessageInterface::ShowMessage("DataInterface::GetParameterID() returning "
         "%d\n", id);
   #endif
   return id;
}

Gmat::ParameterType FileInterface::GetParameterType(const Integer id) const
{
   if (id >= DataInterfaceParamCount && id < FileInterfaceParamCount)
      return PARAMETER_TYPE[id - DataInterfaceParamCount];

   return DataInterface::GetParameterType(id);
}

std::string FileInterface::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

bool FileInterface::IsParameterReadOnly(const Integer id) const
{
   return DataInterface::IsParameterReadOnly(id);
}

bool FileInterface::IsParameterReadOnly(const std::string& label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

std::string FileInterface::GetStringParameter(const Integer id) const
{
   if (id == FILENAME)
      return filename;

   return DataInterface::GetStringParameter(id);
}

bool FileInterface::SetStringParameter(const Integer id,
      const std::string& value)
{
   if (id == FILENAME)
   {
      filename = value;
      return true;
   }

   return DataInterface::SetStringParameter(id, value);
}

std::string FileInterface::GetStringParameter(const Integer id,
      const Integer index) const
{
   return DataInterface::GetStringParameter(id, index);
}

bool FileInterface::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   return DataInterface::SetStringParameter(id, value, index);
}

std::string FileInterface::GetStringParameter(const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}

bool FileInterface::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}

std::string FileInterface::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

bool FileInterface::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Sets up the reader that the interface uses and prepares for reading
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool FileInterface::Initialize()
{
   bool retval = false;

   #ifdef DEBUG_READER_CREATION
      MessageInterface::ShowMessage("Initializing the %s file interface\n",
            instanceName.c_str());
   #endif

   // Verify that the file exists
   if (GmatFileUtil::DoesFileExist(filename))
   {
      ReaderFactory rf;

      if (theReader != NULL)
         delete theReader;
      theReader = (DataReader*)rf.CreateObject(readerFormat, "");
      if (theReader != NULL)
      {
         #ifdef DEBUG_READER_CREATION
            MessageInterface::ShowMessage("A %s reader was created\n",
                  readerFormat.c_str());
         #endif
         retval = true;
      }
      else
         throw InterfaceException("The FileInterface \"" + instanceName +
               "\" was unable to create a reader for the data.");
   }
   else
      throw InterfaceException("The FileInterface \"" + instanceName +
            "\" is set to read the file \"" + filename +
            "\", but the file does not exist.");

   return retval;
}

//------------------------------------------------------------------------------
// Integer Open(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Opens the file stream so data can be read
 *
 * @param name Name for the interface (unused for FoleInterface objects)
 *
 * @return 0 on success, or an error code
 */
//------------------------------------------------------------------------------
Integer FileInterface::Open(const std::string& name)
{
   #ifdef DEBUG_READER_CREATION
      MessageInterface::ShowMessage("FileInterface::Open() called\n"
            "   theReader = <%p>\n", theReader);
   #endif

   Integer retval = -1;

   if (theReader)
   {
      if (theStream.is_open())
         throw InterfaceException("The FileInterface \"" + instanceName +
            "\" attempted to open the file \"" + filename +
            "\", but the file is already open.");

      theStream.open(filename.c_str());
      theReader->SetStream(&theStream, filename);

      retval = 0;
   }
   else
      throw InterfaceException("Cannot open the file interface");

   return retval;
}

//------------------------------------------------------------------------------
// bool LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data into the interface
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool FileInterface::LoadData()
{
   #ifdef DEBUG_READER_CREATION
      MessageInterface::ShowMessage("FileInterface::LoadData() called\n");
   #endif

   bool retval = false;

   if (theReader && theStream.is_open())
   {
      retval = theReader->ReadData();
   }

   return retval;
}

//------------------------------------------------------------------------------
// Integer Close(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Closes the FileInterface
 *
 * @param name Name for the interface (unused for FoleInterface objects)
 *
 * @return 0 on success, or an error code
 */
//------------------------------------------------------------------------------
Integer FileInterface::Close(const std::string& name)
{
   #ifdef DEBUG_READER_CREATION
      MessageInterface::ShowMessage("FileInterface::Close() called\n");
   #endif

   Integer retval = -1;

   if (theReader)
   {
      if (theStream.is_open())
         theStream.close();
      retval = 0;
   }

   return retval;
}
