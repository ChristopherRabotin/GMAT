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
#include "FileUtil.hpp"

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

   // Fill in the list of supported data from the VHF
   supportedFields.clear();

   supportedFields.push_back("Epoch");
   fileStringMap["Epoch"]   = "EPOCH TIME FOR ELEMENTS";
   objectStringMap["Epoch"] = "Epoch";
   objectIDMap["Epoch"]     = -1;
   dataType["Epoch"]        = READER_TIMESTRING;

   supportedFields.push_back("CartesianState");
   fileStringMap["CartesianState"]   = "CARTESIAN COORDINATES";
   objectStringMap["CartesianState"] = "CartesianX";
   objectIDMap["CartesianState"]     = -1;
   dataType["CartesianState"]        = READER_RVECTOR6;

   supportedFields.push_back("Cr");
   fileStringMap["Cr"]   = "CSUBR";
   objectStringMap["Cr"] = "Cr";
   objectIDMap["Cr"]     = -1;
   dataType["Cr"]        = READER_REAL;

   supportedFields.push_back("X ");
   fileStringMap["X "]   = "X ";
   objectStringMap["X "] = "X";
   objectIDMap["X "]     = -1;
   dataType["X "]        = READER_SUBTYPE;

   supportedFields.push_back("Y ");
   fileStringMap["Y "]   = "Y ";
   objectStringMap["Y "] = "Y";
   objectIDMap["Y "]     = -1;
   dataType["Y "]        = READER_SUBTYPE;

   supportedFields.push_back("Z ");
   fileStringMap["Z "]   = "Z ";
   objectStringMap["Z "] = "Z";
   objectIDMap["Z "]     = -1;
   dataType["Z "]        = READER_SUBTYPE;

   supportedFields.push_back("XDOT");
   fileStringMap["XDOT"]   = "XDOT";
   objectStringMap["XDOT"] = "VX";
   objectIDMap["XDOT"]     = -1;
   dataType["XDOT"]        = READER_SUBTYPE;

   supportedFields.push_back("YDOT");
   fileStringMap["YDOT"]   = "YDOT";
   objectStringMap["YDOT"] = "VY";
   objectIDMap["YDOT"]     = -1;
   dataType["YDOT"]        = READER_SUBTYPE;

   supportedFields.push_back("ZDOT");
   fileStringMap["ZDOT"]   = "ZDOT";
   objectStringMap["ZDOT"] = "VZ";
   objectIDMap["ZDOT"]     = -1;
   dataType["ZDOT"]        = READER_SUBTYPE;
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
      FileReader::operator=(vhf);
   }

   return *this;
}


const StringArray& TcopsVHFData::GetSupportedFieldNames()
{
   return supportedFields;
}
