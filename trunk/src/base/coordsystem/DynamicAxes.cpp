//$Header$
//------------------------------------------------------------------------------
//                                  DynamicAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2004/12/28
//
/**
* Implementation of the DynamicAxes class.  This is the base class for those
 * AxisSystem classes that implement dynamic systems.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "DynamicAxes.hpp"

//---------------------------------
// static data
//---------------------------------

/*const std::string
DynamicAxes::PARAMETER_TEXT[DynamicAxesParamCount - AxisSystemParamCount] =
{
   "Epoch",
};

const Gmat::ParameterType
DynamicAxes::PARAMETER_TYPE[DynamicAxesParamCount - AxisSystemParamCount] =
{
   Gmat::REAL_TYPE,
   //Gmat::A1MJD_TYPE, // but no access methods available - should be TaiMjd?
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  DynamicAxes(const std::string &itsType,
//              const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base DynamicAxes structures used in derived classes
 * (default constructor).
 *
 * @param itsType GMAT script string associated with this type of object.
 * @param itsName Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for DynamicAxes.  Derived 
 *       classes must pass in the typeId and typeStr parameters.
 */
//---------------------------------------------------------------------------
DynamicAxes::DynamicAxes(const std::string &itsType,
                         const std::string &itsName) :
AxisSystem(itsType,itsName)
{
   objectTypeNames.push_back("DynamicAxes");
   parameterCount = DynamicAxesParamCount;
}

//---------------------------------------------------------------------------
//  DynamicAxes(const DynamicAxes &dyn);
//---------------------------------------------------------------------------
/**
 * Constructs base DynamicAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param dyn  DynamicAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
DynamicAxes::DynamicAxes(const DynamicAxes &dyn) :
AxisSystem(dyn)
{
}

//---------------------------------------------------------------------------
//  DynamicAxes& operator=(const DynamicAxes &dyn)
//---------------------------------------------------------------------------
/**
 * Assignment operator for DynamicAxes structures.
 *
 * @param dyn The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const DynamicAxes& DynamicAxes::operator=(const DynamicAxes &dyn)
{
   if (&dyn == this)
      return *this;
   AxisSystem::operator=(dyn);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~DynamicAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
DynamicAxes::~DynamicAxes()
{
}


//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this DynamicAxes.
 *
 */
//---------------------------------------------------------------------------
bool DynamicAxes::Initialize()
{
   return AxisSystem::Initialize();
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
