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

ThrustHistoryFile::ThrustHistoryFile(const std::string& theName) :
   FileReader        ("ThrustHistoryFile", theName)
{
   objectTypes.push_back(Gmat::INTERFACE);
   objectTypeNames.push_back("ThrustHistoryFile");
//   parameterCount = ImpulsiveBurnParamCount;
}

ThrustHistoryFile::~ThrustHistoryFile()
{
}

ThrustHistoryFile::ThrustHistoryFile(const ThrustHistoryFile& thf) :
   FileReader        (thf)
{
}

ThrustHistoryFile& ThrustHistoryFile::operator =(const ThrustHistoryFile& thf)
{
   if (this != &thf)
   {
      FileReader::operator =(thf);
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
