//$Id$
//------------------------------------------------------------------------------
//                           DataWriterInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 11, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#include "DataWriterInterface.hpp"

DataWriterInterface *DataWriterInterface::instance = NULL;

DataWriterInterface* DataWriterInterface::Instance()
{
   if (instance == NULL)
      instance = new DataWriterInterface;
   return instance;
}


DataWriterInterface::DataWriterInterface()
{
}


DataWriterInterface::~DataWriterInterface()
{
}


void DataWriterInterface::RegisterWriterMaker(DataWriterMaker *newMaker)
{
   if (writerMap.find(newMaker->GetType()) == writerMap.end())
      writerMap[newMaker->GetType()] = newMaker;
}


DataWriter* DataWriterInterface::GetDataWriter(const std::string &ofType)
{
   DataWriter* retval = NULL;

   if (writerMap.find(ofType) != writerMap.end())
      retval = writerMap[ofType]->CreateDataWriter();

   return retval;
}
