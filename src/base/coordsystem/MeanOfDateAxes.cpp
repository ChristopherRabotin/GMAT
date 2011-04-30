//$Id$
//------------------------------------------------------------------------------
//                                  MeanOfDateAxes
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/05/03
//
/**
 * Implementation of the MeanOfDateAxes class.  This is the base class for those
 * DynamicAxes classes that implement Mean Of Date axis systems.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "MeanOfDateAxes.hpp"
#include "DynamicAxes.hpp"

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  MeanOfDateAxes(const std::string &itsType,
//                 const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base MeanOfDateAxes structures used in derived classes
 * (default constructor).
 *
 * @param itsType GMAT script string associated with this type of object.
 * @param itsName Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for MeanOfDateAxes.  Derived 
 *       classes must pass in the typeId and typeStr parameters.
 */
//---------------------------------------------------------------------------
MeanOfDateAxes::MeanOfDateAxes(const std::string &itsType,
                               const std::string &itsName) :
DynamicAxes(itsType,itsName)
{
   objectTypeNames.push_back("MeanOfDateAxes");
   parameterCount = MeanOfDateAxesParamCount;
}

//---------------------------------------------------------------------------
//  MeanOfDateAxes(const MeanOfDateAxes &mod);
//---------------------------------------------------------------------------
/**
 * Constructs base MeanOfDateAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param mod  MeanOfDateAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
MeanOfDateAxes::MeanOfDateAxes(const MeanOfDateAxes &mod) :
DynamicAxes(mod)
{
}

//---------------------------------------------------------------------------
//  MeanOfDateAxes& operator=(const MeanOfDateAxes &mod)
//---------------------------------------------------------------------------
/**
 * Assignment operator for MeanOfDateAxes structures.
 *
 * @param mod The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const MeanOfDateAxes& MeanOfDateAxes::operator=(const MeanOfDateAxes &mod)
{
   if (&mod == this)
      return *this;
   DynamicAxes::operator=(mod);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~MeanOfDateAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
MeanOfDateAxes::~MeanOfDateAxes()
{
}


//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this MeanOfDateAxes.
 *
 */
//---------------------------------------------------------------------------
bool MeanOfDateAxes::Initialize()
{
   return DynamicAxes::Initialize();
}

//GmatCoordinate::ParameterUsage MeanOfDateAxes::UsesEopFile() const
//{
//   return GmatCoordinate::REQUIRED;
//}

//GmatCoordinate::ParameterUsage MeanOfDateAxes::UsesItrfFile() const
//{
//   return GmatCoordinate::REQUIRED;
//}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
