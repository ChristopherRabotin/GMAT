//$Header$
//------------------------------------------------------------------------------
//                                  InertialAxes
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
* Implementation of the InertialAxes class.  This is the base class for those
 * AxisSystem classes that implement inertial systems.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "InertialAxes.hpp"

//---------------------------------
// static data
//---------------------------------

/* placeholder - may be needed later
const std::string
InertialAxes::PARAMETER_TEXT[InertialAxesParamCount - AxisSystemParamCount] =
{
   "",
};

const Gmat::ParameterType
InertialAxes::PARAMETER_TYPE[InertialAxesParamCount - AxisSystemParamCount] =
{
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  InertialAxes(const std::string &itsType,
//               const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base InertialAxes structures used in derived classes
 * (default constructor).
 *
 * @param itsType GMAT script string associated with this type of object.
 * @param itsName Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for InertialAxes.  Derived 
 *       classes must pass in the itsType and itsName parameters.
 */
//---------------------------------------------------------------------------
InertialAxes::InertialAxes(const std::string &itsType,
                           const std::string &itsName) :
AxisSystem(itsType,itsName)
{
   objectTypeNames.push_back("InertialAxes");
   parameterCount = InertialAxesParamCount;
}

//---------------------------------------------------------------------------
//  InertialAxes(const InertialAxes &inertial);
//---------------------------------------------------------------------------
/**
 * Constructs base InertialAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param inertial  InertialAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
InertialAxes::InertialAxes(const InertialAxes &inertial) :
AxisSystem(inertial)
{
}

//---------------------------------------------------------------------------
//  InertialAxes& operator=(const InertialAxes &inertial)
//---------------------------------------------------------------------------
/**
 * Assignment operator for InertialAxes structures.
 *
 * @param inertial The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const InertialAxes& InertialAxes::operator=(const InertialAxes &inertial)
{
   if (&inertial == this)
      return *this;
   AxisSystem::operator=(inertial);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~InertialAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
InertialAxes::~InertialAxes()
{
}

//---------------------------------------------------------------------------
//  bool InertialAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for the InertialAxes classes.
 *
 * @note This method is pure virtual and must be implemented by deriving 
 *       classes (at least at the leaf level).
 */
//---------------------------------------------------------------------------
bool InertialAxes::Initialize()
{
   AxisSystem::Initialize();
   return true;
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
/*std::string InertialAxes::GetParameterText(const Integer id) const
{
   if (id >= AxisSystemParamCount && id < InertialAxesParamCount)
      return PARAMETER_TEXT[id - AxisSystemParamCount];
   return AxisSystem::GetParameterText(id);
}
*/
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
/*Integer InertialAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = AxisSystemParamCount; i < InertialAxesParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - AxisSystemParamCount])
         return i;
   }
   
   return AxisSystem::GetParameterID(str);
}
*/
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
/*Gmat::ParameterType InertialAxes::GetParameterType(const Integer id) const
{
   if (id >= AxisSystemParamCount && id < InertialAxesParamCount)
      return PARAMETER_TYPE[id - AxisSystemParamCount];
   
   return AxisSystem::GetParameterType(id);
}
*/
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
/*std::string InertialAxes::GetParameterTypeString(const Integer id) const
{
   return AxisSystem::PARAM_TYPE_STRING[GetParameterType(id)];
}
*/
