//$Header$
//------------------------------------------------------------------------------
//                                  EquatorAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/02/24
//
/**
 * Implementation of the EquatorAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "EquatorAxes.hpp"
#include "DynamicAxes.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include "TimeTypes.hpp"
#include "Rvector3.hpp"
#include "CoordinateSystemException.hpp"

using namespace GmatMathUtil;      // for trig functions, etc.
using namespace GmatTimeUtil;      // for JD offsets, etc.

//---------------------------------
// static data
//---------------------------------

//const std::string
//EquatorAxes::PARAMETER_TEXT[EquatorAxesParamCount - DynamicAxesParamCount] =
//{
//};

//const Gmat::ParameterType
//EquatorAxes::PARAMETER_TYPE[EquatorAxesParamCount - DynamicAxesParamCount] =
//{
//};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  EquatorAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base EquatorAxes structures
 * (default constructor).
 *
 * @param itdName name of the EquatorAxes object.
 */
//------------------------------------------------------------------------------
EquatorAxes::EquatorAxes(const std::string &itsName) :
DynamicAxes("Equator",itsName)
{
}

//------------------------------------------------------------------------------
//  EquatorAxes(const EquatorAxes &eqAxes);
//------------------------------------------------------------------------------
/**
 * Constructs base EquatorAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param eqAxes  EquatorAxes instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
EquatorAxes::EquatorAxes(const EquatorAxes &eqAxes) :
DynamicAxes(eqAxes)
{
}

//------------------------------------------------------------------------------
//  EquatorAxes& operator=(const EquatorAxes &eqAxes)
//------------------------------------------------------------------------------
/**
 * Assignment operator for EquatorAxes structures.
 *
 * @param eqAxes The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const EquatorAxes& EquatorAxes::operator=(const EquatorAxes &eqAxes)
{
   if (&eqAxes == this)
      return *this;
   DynamicAxes::operator=(eqAxes);  
   return *this;
}
//------------------------------------------------------------------------------
//  ~EquatorAxes(void)
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
EquatorAxes::~EquatorAxes()
{
}

//------------------------------------------------------------------------------
//  void EquatorAxes::Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for this EquatorAxes.
 *
 */
//------------------------------------------------------------------------------
void EquatorAxes::Initialize()
{
   DynamicAxes::Initialize();
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Planet.
 *
 * @return clone of the Planet.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EquatorAxes::Clone() const
{
   return (new EquatorAxes(*this));
}

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
//std::string EquatorAxes::GetParameterText(const Integer id) const
//{
//   if (id >= DynamicAxesParamCount && id < EquatorAxesParamCount)
//      return PARAMETER_TEXT[id - DynamicAxesParamCount];
//   return DynamicAxes::GetParameterText(id);
//}

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
//Integer EquatorAxes::GetParameterID(const std::string &str) const
//{
//   for (Integer i = DynamicAxesParamCount; i < EquatorAxesParamCount; i++)
//   {
//      if (str == PARAMETER_TEXT[i - DynamicAxesParamCount])
//         return i;
//   }
   
//   return DynamicAxes::GetParameterID(str);
//}

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
//Gmat::ParameterType EquatorAxes::GetParameterType(const Integer id) const
//{
//   if (id >= DynamicAxesParamCount && id < EquatorAxesParamCount)
//      return PARAMETER_TYPE[id - DynamicAxesParamCount];
//   
//   return DynamicAxes::GetParameterType(id);
//}

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
//std::string EquatorAxes::GetParameterTypeString(const Integer id) const
//{
//   return DynamicAxes::PARAM_TYPE_STRING[GetParameterType(id)];
//}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000Eq system
 *
 * @param atEpoch  epoch at which to compute the rotation matrix
 *
 * @note Code (for Earth) adapted from C code written by Steve Queen/ GSFC, 
 *       based on Vallado, pgs. 211- 227.  Equation references in parentheses
 *       will refer to those of the Vallado book.
 */
//------------------------------------------------------------------------------
void EquatorAxes::CalculateRotationMatrix(const A1Mjd &atEpoch) 
{
   if (originName == SolarSystem::EARTH_NAME)
   {
      throw CoordinateSystemException("EquatorAxes not appropriate for Earth");
   }
   else
   {
      // this method will return alpha (deg), delta (deg), 
      // W (deg), and Wdot (deg/day)
      Rvector cartCoord   = ((CelestialBody*)origin)->
                              GetBodyCartographicCoordinates(atEpoch);
      Real rot1           = GmatMathUtil::PI_OVER_TWO + Rad(cartCoord(0));
      Real rot2           = GmatMathUtil::PI_OVER_TWO - Rad(cartCoord(1));
      // Don't care about W and Wdot here
      Rmatrix33 R3left( Cos(rot1),  Sin(rot1),  0.0, 
                       -Sin(rot1),  Cos(rot1),  0.0,
                              0.0,        0.0,  1.0);
      Rmatrix33 R1middle(     1.0,        0.0,       0.0,
                              0.0,  Cos(rot2), Sin(rot2),
                              0.0, -Sin(rot2), Cos(rot2));
      rotMatrix = R3left.Transpose() * R1middle.Transpose();
      //rotDotmatrix remains the zero matrix
   }
}


