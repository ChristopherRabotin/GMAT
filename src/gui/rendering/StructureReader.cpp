//$Id$
//====================================================================
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: John P. Downing/GSFC/595
// Created: 2015.08.31
/**
 * Structure Class Family, these classes implement the structure
 * reader base class.  Readers for different types of files are derived
 * from this one.
 */
//====================================================================
#include "StructureReader.hpp"
#include "MessageInterface.hpp"
//====================================================================
StructureReader::StructureReader (const std::string& filename)
// Constructor
   : Filename (filename),
     TheStructure (new Structure(""))
   {
   }
//------------------------------------------------------------------------------
StructureReader::~StructureReader()
// Destructor
   {
   if (TheStructure != 0)
      delete TheStructure;
   TheStructure = 0;
   }
//====================================================================
