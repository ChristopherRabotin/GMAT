//$Id$
//====================================================================
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
#ifndef StructureReader_hpp
#define StructureReader_hpp
#include "gmatdefs.hpp"
#include "RgbColor.hpp"
#include "Rvector3.hpp"
#include "Surface.hpp"
#include "Structure.hpp"
//====================================================================
class StructureReader
{
public:
// --------------------------- Constructors, Assignment, Destructors
   StructureReader (const std::string& filename);
private: // Copy Protected
   StructureReader (const StructureReader&);
   StructureReader& operator= (const StructureReader&);
public:
   virtual ~StructureReader();
// --------------------------- Functions
public:
   virtual void Execute () = 0;
// --------------------------- Data Members
public:
   std::string Filename;
   Structure*  TheStructure;
// --------------------------- 
};
//====================================================================
#endif 
