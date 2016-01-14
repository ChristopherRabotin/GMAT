//$Id$
//------------------------------------------------------------------------------
//                           ThrustFileReaderFactory
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
// Created: Jan 14, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#include "ThrustFileReaderFactory.hpp"
#include "ThrustHistoryFile.hpp"
#include "MessageInterface.hpp"


ThrustFileReaderFactory::ThrustFileReaderFactory() :
   Factory           (Gmat::INTERFACE)
{
   creatables.push_back("ThrustHistoryFile");
}

ThrustFileReaderFactory::~ThrustFileReaderFactory()
{
}

ThrustFileReaderFactory::ThrustFileReaderFactory(
      const ThrustFileReaderFactory& elf) :
   Factory        (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("ThrustHistoryFile");
   }
}

ThrustFileReaderFactory& ThrustFileReaderFactory::operator =(
      const ThrustFileReaderFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("ThrustHistoryFile");
      }
   }
   return *this;
}

GmatBase* ThrustFileReaderFactory::CreateObject(const std::string& ofType,
      const std::string& withName)
{
   GmatBase *retval = NULL;

   if (ofType == "ThrustHistoryFile")
      retval = new ThrustHistoryFile(withName);

   if (retval != NULL)
   {
      if (retval->IsOfType("ThrustHistoryFile") == false)
      {
         delete retval;
         retval = NULL;
         MessageInterface::ShowMessage("The Reader Factory can only create "
               "DataReader subobjects; %s is not a DataReader\n", ofType.c_str());
      }
   }

   return retval;
}
