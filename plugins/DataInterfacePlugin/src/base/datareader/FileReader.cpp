//$Id$
//------------------------------------------------------------------------------
//                           FileReader
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

#include "FileReader.hpp"
#include "FileUtil.hpp"



//const std::string
//FileReader::PARAMETER_LABEL[FileReaderParamCount - DataReaderParamCount] =
//{
//      "Filename",
//};
//
//const Gmat::ParameterType
//FileReader::PARAMETER_TYPE[FileReaderParamCount - DataReaderParamCount] =
//{
//      Gmat::FILENAME_TYPE,
//};



FileReader::FileReader(const std::string& theTypeName, const std::string& theName) :
   DataReader           (theTypeName, theName),
   theStream            (NULL)
{
   objectTypeNames.push_back("FileReader");
}

FileReader::~FileReader()
{
   // TODO Auto-generated destructor stub
}

FileReader::FileReader(const FileReader& fr) :
   DataReader           (fr),
   theStream            (NULL)
{

}

FileReader& FileReader::operator=(const FileReader& fr)
{
   if (this != &fr)
   {
      theStream = NULL;
   }

   return *this;
}

bool FileReader::SetStream(std::ifstream* aStream)
{
   theStream = aStream;
   return true;
}

bool FileReader::ReadLine(std::string& theLine)
{
   if (theStream->eof())
      return false;
   return GmatFileUtil::GetLine(theStream, theLine);
}
