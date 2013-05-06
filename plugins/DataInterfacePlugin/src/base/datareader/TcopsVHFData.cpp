//$Id$
//------------------------------------------------------------------------------
//                           TcopsVHFData
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
// Created: May 3, 2013
/**
 * 
 */
//------------------------------------------------------------------------------

#include "TcopsVHFData.hpp"


//const std::string
//TcopsVHFData::PARAMETER_LABEL[TVHFDataParamCount - FileReaderParamCount] =
//   {
//         "",
//   };
//
//const Gmat::ParameterType
//TcopsVHFData::PARAMETER_TYPE[TVHFDataParamCount - FileReaderParamCount] =
//   {
//         Gmat::,
//   };



TcopsVHFData::TcopsVHFData(const std::string& theTypeName,
      const std::string& theName) :
   FileReader        (theTypeName, theName)
{
   // Set up the engine accessor fields
   objectTypeNames.push_back("TcopsVHFData");


   // Fill in the list of surroted data from the VHF
   supportedFields.clear();
   supportedFields.push_back("Epoch");
   supportedFields.push_back("CartesianState");
   supportedFields.push_back("Cr");
}

TcopsVHFData::~TcopsVHFData()
{
}

TcopsVHFData::TcopsVHFData(const TcopsVHFData& vhf) :
   FileReader        (vhf)
{
}

TcopsVHFData& TcopsVHFData::operator=(const TcopsVHFData& vhf)
{
   if (this == &vhf)
   {

   }

   return *this;
}


const StringArray& TcopsVHFData::GetSupportedFieldNames()
{
   return supportedFields;
}
