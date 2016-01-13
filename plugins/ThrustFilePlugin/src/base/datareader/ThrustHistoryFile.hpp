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

#ifndef ThrustHistoryFile_hpp
#define ThrustHistoryFile_hpp

#include "ThrustFileDefs.hpp"
#include "FileReader.hpp"

class ThrustHistoryFile: public FileReader
{
public:
   ThrustHistoryFile(const std::string& theName = "");
   virtual ~ThrustHistoryFile();
};

#endif /* ThrustHistoryFile_hpp */
