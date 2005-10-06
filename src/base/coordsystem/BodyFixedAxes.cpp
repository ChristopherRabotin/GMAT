//$Header$
//------------------------------------------------------------------------------
//                                  BodyFixedAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/01/25
//
/**
 * Implementation of the BodyFixedAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "BodyFixedAxes.hpp"
#include "DynamicAxes.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include "RealTypes.hpp"
#include "TimeTypes.hpp"
#include "Rvector3.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateSystemException.hpp"

using namespace GmatMathUtil;      // for trig functions, etc.
using namespace GmatTimeUtil;      // for JD offsets, etc.

//---------------------------------
// static data
//---------------------------------

//const std::string
//BodyFixedAxes::PARAMETER_TEXT[BodyFixedAxesParamCount - DynamicAxesParamCount] =
//{
//};

//const Gmat::ParameterType
//BodyFixedAxes::PARAMETER_TYPE[BodyFixedAxesParamCount - DynamicAxesParamCount] =
//{
//};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  BodyFixedAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base BodyFixedAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 */
//------------------------------------------------------------------------------
BodyFixedAxes::BodyFixedAxes(const std::string &itsName) :
DynamicAxes("BodyFixed",itsName)
{
   objectTypeNames.push_back("BodyFixedAxes");
   parameterCount = BodyFixedAxesParamCount;
}


//------------------------------------------------------------------------------
//  BodyFixedAxes(const BodyFixedAxes &bfAxes);
//------------------------------------------------------------------------------
/**
 * Constructs base BodyFixedAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param bfAxes  BodyFixedAxes instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
BodyFixedAxes::BodyFixedAxes(const BodyFixedAxes &bfAxes) :
DynamicAxes(bfAxes)
{
}

//------------------------------------------------------------------------------
//  BodyFixedAxes& operator=(const BodyFixedAxes &bfAxes)
//------------------------------------------------------------------------------
/**
 * Assignment operator for BodyFixedAxes structures.
 *
 * @param bfAxes The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const BodyFixedAxes& BodyFixedAxes::operator=(const BodyFixedAxes &bfAxes)
{
   if (&bfAxes == this)
      return *this;
   DynamicAxes::operator=(bfAxes);  
   return *this;
}
//------------------------------------------------------------------------------
//  ~BodyFixedAxes(void)
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BodyFixedAxes::~BodyFixedAxes()
{
}

GmatCoordinate::ParameterUsage BodyFixedAxes::UsesEopFile() const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage BodyFixedAxes::UsesItrfFile() const
{
   return GmatCoordinate::REQUIRED;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for this BodyFixedAxes.
 *
 */
//------------------------------------------------------------------------------
bool BodyFixedAxes::Initialize()
{
   DynamicAxes::Initialize();
   if (originName == SolarSystem::EARTH_NAME) InitializeFK5();
   
   return true;
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
GmatBase* BodyFixedAxes::Clone() const
{
   return (new BodyFixedAxes(*this));
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
//std::string BodyFixedAxes::GetParameterText(const Integer id) const
//{
//   if (id >= DynamicAxesParamCount && id < BodyFixedAxesParamCount)
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
//Integer BodyFixedAxes::GetParameterID(const std::string &str) const
//{
//   for (Integer i = DynamicAxesParamCount; i < BodyFixedAxesParamCount; i++)
//   {
//      if (str == PARAMETER_TEXT[i - DynamicAxesParamCount])
//         return i;
//   }
//   
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
//Gmat::ParameterType BodyFixedAxes::GetParameterType(const Integer id) const
//{
//   if (id >= DynamicAxesParamCount && id < BodyFixedAxesParamCount)
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
//std::string BodyFixedAxes::GetParameterTypeString(const Integer id) const
//{
//   return DynamicAxes::PARAM_TYPE_STRING[GetParameterType(id)];
//}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id  ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
//std::string BodyFixedAxes::GetStringParameter(const Integer id) const
//{
//   return DynamicAxes::GetStringParameter(id);
//}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
* This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id     ID for the requested parameter.
 * @param value  string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
//bool BodyFixedAxes::SetStringParameter(const Integer id, 
//                                       const std::string &value)
//{
//   return DynamicAxes::SetStringParameter(id, value);
//}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Accessor method used to get a parameter value
 *
 * @param    label  label ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
//std::string BodyFixedAxes::GetStringParameter(const std::string &label) const
//{
//   return GetStringParameter(GetParameterID(label));
//}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
* Accessor method used to get a parameter value
 *
 * @param    label  Integer ID for the parameter
 * @param    value  The new value for the parameter
 */
//------------------------------------------------------------------------------
//bool BodyFixedAxes::SetStringParameter(const std::string &label,
//                                        const std::string &value)
//{
//   return SetStringParameter(GetParameterID(label), value);
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
void BodyFixedAxes::CalculateRotationMatrix(const A1Mjd &atEpoch) 
{
   // compute rotMatrix and rotDotMatrix
   if (originName == SolarSystem::EARTH_NAME)
   {
      Real dPsi             = 0.0;
      Real longAscNodeLunar = 0.0;
      Real cosEpsbar        = 0.0;
      Real cosAst           = 0.0;
      Real sinAst           = 0.0;
      //Real x                = 0.0;
      //Real y                = 0.0;
      // Convert to MJD UTC to use for polar motion  and LOD 
      // interpolations
      Real mjdUTC = TimeConverterUtil::Convert(atEpoch.Get(),
                    "A1Mjd", "UtcMjd", JD_JAN_5_1941);
      // convert to MJD referenced from time used in EOP file
      mjdUTC = mjdUTC + JD_JAN_5_1941 - JD_NOV_17_1858;


      // convert input time to UT1 for later use (for AST calculation)
      Real mjdUT1 = TimeConverterUtil::Convert(atEpoch.Get(),
                    "A1Mjd", "Ut1Mjd", JD_JAN_5_1941);
      Real jdUT1    = mjdUT1 + JD_JAN_5_1941; // right?
      // Compute elapsed Julian centuries (UT1)
      Real tUT1     = (jdUT1 - 2451545.0) / 36525.0;
      
      // convert input A1 MJD to TT MJD (for most calculations)
      Real mjdTT = TimeConverterUtil::Convert(atEpoch.Get(),
                   "A1Mjd", "TtMjd", JD_JAN_5_1941);      
      Real jdTT    = mjdTT + JD_JAN_5_1941; // right? 
      // Compute Julian centuries of TDB from the base epoch (J2000) 
      // NOTE - this is really TT, an approximation of TDB *********
      Real tTDB    = (jdTT - 2451545.0) / 36525.0;
 
      Rmatrix33  PREC      = ComputePrecessionMatrix(tTDB, atEpoch);
      Rmatrix33  NUT       = ComputeNutationMatrix(tTDB, atEpoch, dPsi,
                             longAscNodeLunar, cosEpsbar);
      Rmatrix33  ST        = ComputeSiderealTimeRotation(jdTT, tUT1,
                             dPsi, longAscNodeLunar, cosEpsbar,
                             cosAst, sinAst);
      Rmatrix33  STderiv   = ComputeSiderealTimeDotRotation(mjdUTC, 
                             atEpoch, cosAst, sinAst);
      Rmatrix33  PM        = ComputePolarMotionRotation(mjdUTC, atEpoch);
     
      rotMatrix    = PREC.Transpose() * (NUT.Transpose() * 
                     (ST.Transpose() * PM.Transpose()));
      Real determinant = rotMatrix.Determinant();
      if (Abs(determinant - 1.0) > DETERMINANT_TOLERANCE)
         throw CoordinateSystemException(
               "Computed rotation matrix has a determinant not equal to 1.0");
      rotDotMatrix    = PREC.Transpose() * (NUT.Transpose() * 
                        (STderiv.Transpose() * PM.Transpose()));
   }
   else
   {
      // this method will return alpha (deg), delta (deg), 
      // W (deg), and Wdot (deg/day)
      Rvector cartCoord   = ((CelestialBody*)origin)->
                              GetBodyCartographicCoordinates(atEpoch);
      Real rot1           = GmatMathUtil::PI_OVER_TWO + Rad(cartCoord(0));
      Real rot2           = GmatMathUtil::PI_OVER_TWO - Rad(cartCoord(1));
      Real W              = Rad(cartCoord(2));
      // Convert Wdot from deg/day to rad/sec
      Real Wdot           = Rad(cartCoord(3)) / SECS_PER_DAY; 
      Rmatrix33 R3left( Cos(rot1),  Sin(rot1),  0.0, 
                       -Sin(rot1),  Cos(rot1),  0.0,
                              0.0,        0.0,  1.0);
      Rmatrix33 R1middle(     1.0,        0.0,       0.0,
                              0.0,  Cos(rot2), Sin(rot2),
                              0.0, -Sin(rot2), Cos(rot2));
      Rmatrix33 R3right(   Cos(W),  Sin(W),  0.0,
                          -Sin(W),  Cos(W),  0.0,
                              0.0,     0.0,  1.0);
      rotMatrix = R3left.Transpose() * 
                 (R1middle.Transpose() * R3right.Transpose());
      Rmatrix33 Wderiv(-Wdot*Sin(W), -Wdot*Cos(W), 0.0,
                        Wdot*Cos(W), -Wdot*Sin(W), 0.0,
                                0.0,          0.0, 0.0);
      rotDotMatrix = R3left.Transpose() * 
         (R1middle.Transpose() * Wderiv);
   }
}


