//$Header$
//------------------------------------------------------------------------------
//                                  AxisSystem
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2004/12/20
//
/**
 * Implementation of the AxisSystem class.  This is the base class for the
 * InertialAxes and DynamicAxes classes.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "CoordinateBase.hpp"
#include "Rmatrix33.hpp"

//---------------------------------
// static data
//---------------------------------

/*const std::string
AxisSystem::PARAMETER_TEXT[AxisSystemParamCount - CoordinateBaseParamCount] =
{
   "RotationMatrix",
   "RotationDotMatrix",
};*/

/*const Gmat::ParameterType
AxisSystem::PARAMETER_TYPE[AxisSystemParamCount - CoordinateBaseParamCount] =
{
   Gmat::RMATRIX33_TYPE,
   Gmat::RMATRIX33_TYPE,
};*/


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  AxisSystem(const std::string &itsType,
//             const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base AxisSystem structures used in derived classes
 * (default constructor).
 *
 * @param itsType GMAT script string associated with this type of object.
 * @param itsName Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for AxisSystem.  Derived 
 *       classes must pass in the itsType and itsName parameters.
 */
//---------------------------------------------------------------------------
AxisSystem::AxisSystem(const std::string &itsType,
                       const std::string &itsName) :
CoordinateBase(Gmat::AXIS_SYSTEM,itsType,itsName)//,
//rotMatrix         (NULL),
//rotDotMatrix      (NULL)
{
}

//---------------------------------------------------------------------------
//  AxisSystem(const AxisSystem &axisSys);
//---------------------------------------------------------------------------
/**
 * Constructs base AxisSystem structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param axisSys  AxisSystem instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
AxisSystem::AxisSystem(const AxisSystem &axisSys) :
CoordinateBase(axisSys),
rotMatrix         (axisSys.rotMatrix),
rotDotMatrix      (axisSys.rotDotMatrix)
{
}

//---------------------------------------------------------------------------
//  AxisSystem& operator=(const AxisSystem &axisSys)
//---------------------------------------------------------------------------
/**
 * Assignment operator for AxisSystem structures.
 *
 * @param axisSys The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const AxisSystem& AxisSystem::operator=(const AxisSystem &axisSys)
{
   if (&axisSys == this)
      return *this;
   CoordinateBase::operator=(axisSys);
   rotMatrix     = axisSys.rotMatrix;
   rotDotMatrix  = axisSys.rotDotMatrix;
   
   return *this;
}
//---------------------------------------------------------------------------
//  ~AxisSystem(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
AxisSystem::~AxisSystem()
{
}

//---------------------------------------------------------------------------
//  void AxisSystem::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this AxisSystem.
 *
 */
//---------------------------------------------------------------------------
void AxisSystem::Initialize()
{
   CoordinateBase::Initialize();
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
/*std::string AxisSystem::GetParameterText(const Integer id) const
{
   if (id >= CoordinateBaseParamCount && id < AxisSystemParamCount)
      return PARAMETER_TEXT[id - CoordinateBaseParamCount];
   return CoordinateBase::GetParameterText(id);
}*/

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
/*Integer AxisSystem::GetParameterID(const std::string &str) const
{
   for (Integer i = CoordinateBaseParamCount; i < AxisSystemParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - CoordinateBaseParamCount])
         return i;
   }
   
   return CoordinateBase::GetParameterID(str);
}*/

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
/*Gmat::ParameterType AxisSystem::GetParameterType(const Integer id) const
{
   if (id >= CoordinateBaseParamCount && id < AxisSystemParamCount)
      return PARAMETER_TYPE[id - CoordinateBaseParamCount];
   
   return CoordinateBase::GetParameterType(id);
}*/

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
/*std::string AxisSystem::GetParameterTypeString(const Integer id) const
{
   return CoordinateBase::PARAM_TYPE_STRING[GetParameterType(id)];
}*/
