//$Id$
//------------------------------------------------------------------------------
//                         DataFileAdapter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS
// Task 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 12, 2010
//
/**
 * File description here.
 */
//------------------------------------------------------------------------------


#include "DataFileAdapter.hpp"

DataFileAdapter::DataFileAdapter()
{
   // TODO Auto-generated constructor stub
}


DataFileAdapter::~DataFileAdapter()
{
   // TODO Auto-generated destructor stub
}


DataFileAdapter::DataFileAdapter(const DataFileAdapter & dfa)
{
}


DataFileAdapter& DataFileAdapter::operator =(const DataFileAdapter & dfa)
{
   if (this != &dfa)
   {

   }

   return *this;
}


ObType* DataFileAdapter::GetObTypeObject(DataFile *forFile)
{
   return NULL;
}


bool DataFileAdapter::LoadObservation(ObType & data, ObservationData & target)
{
   return false;
}


bool DataFileAdapter::LoadMeasurement(MeasurementData & data, ObType & target)
{
   return false;
}
