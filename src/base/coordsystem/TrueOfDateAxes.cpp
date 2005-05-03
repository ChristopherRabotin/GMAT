//$Header$
//------------------------------------------------------------------------------
//                                  TrueOfDateAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/05/03
//
/**
 * Implementation of the TrueOfDateAxes class.  This is the base class for those
 * DynamicAxes classes that implement True Of Date axis systems.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "TrueOfDateAxes.hpp"
#include "DynamicAxes.hpp"

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  TrueOfDateAxes(const std::string &itsType,
//                 const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base TrueOfDateAxes structures used in derived classes
 * (default constructor).
 *
 * @param itsType GMAT script string associated with this type of object.
 * @param itsName Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for TrueOfDateAxes.  Derived 
 *       classes must pass in the typeId and typeStr parameters.
 */
//---------------------------------------------------------------------------
TrueOfDateAxes::TrueOfDateAxes(const std::string &itsType,
                               const std::string &itsName) :
DynamicAxes(itsType,itsName)
{
   objectTypeNames.push_back("TrueOfDateAxes");
   parameterCount = TrueOfDateAxesParamCount;
}

//---------------------------------------------------------------------------
//  TrueOfDateAxes(const TrueOfDateAxes &tod);
//---------------------------------------------------------------------------
/**
 * Constructs base TrueOfDateAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param tod  TrueOfDateAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
TrueOfDateAxes::TrueOfDateAxes(const TrueOfDateAxes &tod) :
DynamicAxes(tod)
{
}

//---------------------------------------------------------------------------
//  TrueOfDateAxes& operator=(const TrueOfDateAxes &tod)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TrueOfDateAxes structures.
 *
 * @param tod The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const TrueOfDateAxes& TrueOfDateAxes::operator=(const TrueOfDateAxes &tod)
{
   if (&tod == this)
      return *this;
   DynamicAxes::operator=(tod);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~TrueOfDateAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
TrueOfDateAxes::~TrueOfDateAxes()
{
}


//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this TrueOfDateAxes.
 *
 */
//---------------------------------------------------------------------------
bool TrueOfDateAxes::Initialize()
{
   return DynamicAxes::Initialize();
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
