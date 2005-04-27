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

const std::string
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

GmatCoordinate::ParameterUsage DynamicAxes::UsesEpoch() const
{
   return GmatCoordinate::REQUIRED;
}


//---------------------------------------------------------------------------
//  void SetEpoch(const A1Mjd &toEpoch)
//---------------------------------------------------------------------------
/**
 * Sets the epoch for the DynamicAxes class.
 *
 * @param <toEpoch> epoch value to use.
 *
 * @return true if successful; false, otherwise.
 *
 */
//---------------------------------------------------------------------------
void DynamicAxes::SetEpoch(const A1Mjd &toEpoch)
{
   epoch = toEpoch;
}

//---------------------------------------------------------------------------
//  A1Mjd GetEpoch() const
//---------------------------------------------------------------------------
/**
 * Returns the epoch of the DynamicAxes class.
 *
 * @return epoch.
 *
 */
//---------------------------------------------------------------------------
A1Mjd DynamicAxes::GetEpoch() const
{
   return epoch;
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
//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string DynamicAxes::GetParameterText(const Integer id) const
{
   if (id >= AxisSystemParamCount && id < DynamicAxesParamCount)
      return PARAMETER_TEXT[id - AxisSystemParamCount];
   return AxisSystem::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer DynamicAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = AxisSystemParamCount; i < DynamicAxesParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - AxisSystemParamCount])
         return i;
   }
   
   return AxisSystem::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType DynamicAxes::GetParameterType(const Integer id) const
{
   if (id >= AxisSystemParamCount && id < DynamicAxesParamCount)
      return PARAMETER_TYPE[id - AxisSystemParamCount];
   
   return AxisSystem::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string DynamicAxes::GetParameterTypeString(const Integer id) const
{
   return AxisSystem::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the real value, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real DynamicAxes::GetRealParameter(const Integer id) const
{
   if (id == EPOCH) return epoch.Get();  // modify later????????????
   return AxisSystem::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value) 
//------------------------------------------------------------------------------
/**
 * This method sets the real value, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 * @param value to use to set the parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real DynamicAxes::SetRealParameter(const Integer id, const Real value)
{
   if (id == EPOCH)
   {
      epoch.Set(value);
      return true;
   }
   return AxisSystem::SetRealParameter(id,value);
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the real value, given the input parameter label.
 *
 * @param label label for the requested parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real DynamicAxes::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const std::string &label, const Real value) 
//------------------------------------------------------------------------------
/**
 * This method sets the real value, given the input parameter label.
 *
 * @param label label for the requested parameter.
 * @param value to use to set the parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real DynamicAxes::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

