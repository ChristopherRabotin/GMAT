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
#include "Planet.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include "RealTypes.hpp"
#include "TimeTypes.hpp"
#include "Rvector3.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateSystemException.hpp"
#include "MessageInterface.hpp"

using namespace GmatMathUtil;      // for trig functions, etc.
using namespace GmatTimeUtil;      // for JD offsets, etc.

//#define DEBUG_FIRST_CALL

#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
#endif


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
   
   #ifdef DEBUG_FIRST_CALL
      firstCallFired = false;
   #endif
   
   precData    = PREC.GetDataVector();
   nutData     = NUT.GetDataVector();
   stData      = ST.GetDataVector();
   stDerivData = STderiv.GetDataVector();
   pmData      = PM.GetDataVector();

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
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "Calling CalculateRotationMatrix at epoch %lf; ", atEpoch.Get());
   #endif
   
   // compute rotMatrix and rotDotMatrix
   if (originName == SolarSystem::EARTH_NAME)
   {
      #ifdef DEBUG_FIRST_CALL
         if (!firstCallFired)
            MessageInterface::ShowMessage("Body is the Earth\n");
      #endif
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
 
      #ifdef DEBUG_FIRST_CALL
         if (!firstCallFired)
            MessageInterface::ShowMessage(
               "   Epoch data[mjdUTC, mjdUT1, jdUT1, tUT1, mjdTT1, jdTT, tTDB] "
               "=\n        [%.10lf %.10lf %.10lf %.10lf %.10lf %.10lf %.10lf ]\n",
               mjdUTC, mjdUT1, jdUT1, tUT1, mjdTT, jdTT, tTDB);
      #endif

      if (overrideOriginInterval) 
      {
         updateIntervalToUse = updateInterval;
         //MessageInterface::ShowMessage("Overriding origin interval .....\n");
         #ifdef DEBUG_FIRST_CALL
            if (!firstCallFired)
               MessageInterface::ShowMessage(
                  "   Overrode origin interval; set to %.12lf\n", 
                  updateIntervalToUse);
         #endif
      }
      else 
      {
         updateIntervalToUse = 
                              ((Planet*) origin)->GetUpdateInterval();
         //MessageInterface::ShowMessage("Using origin interval .....\n");
         #ifdef DEBUG_FIRST_CALL
            if (!firstCallFired)
               MessageInterface::ShowMessage(
                  "   Using body's origin interval, %.12lf\n", 
                  updateIntervalToUse);
         #endif
      }
      
      //MessageInterface::ShowMessage("Setting %4.3f as interval \n",
      //                              updateIntervalToUse);
      ComputePrecessionMatrix(tTDB, atEpoch);
      ComputeNutationMatrix(tTDB, atEpoch, dPsi, longAscNodeLunar, cosEpsbar);
      ComputeSiderealTimeRotation(jdTT, tUT1, dPsi, longAscNodeLunar, cosEpsbar,
                             cosAst, sinAst);
      ComputeSiderealTimeDotRotation(mjdUTC, atEpoch, cosAst, sinAst);
      ComputePolarMotionRotation(mjdUTC, atEpoch);
      

      Real np[3][3], rot[3][3], tmp[3][3];
      Integer p3;
      
      // NUT * PREC
      for (Integer p = 0; p < 3; ++p)
      {
         p3 = 3*p;
         for (Integer q = 0; q < 3; ++q)
         {
            np[p][q] = nutData[p3]   * precData[q]   + 
                       nutData[p3+1] * precData[q+3] + 
                       nutData[p3+2] * precData[q+6];
         }
      }     
      
      // ST * (NUT * PREC)
      for (Integer p = 0; p < 3; ++p)
      {
         p3 = 3*p;
         for (Integer q = 0; q < 3; ++q)
         {
            tmp[p][q] = stData[p3]   * np[0][q] + 
                        stData[p3+1] * np[1][q] + 
                        stData[p3+2] * np[2][q];
         }
      }     
      
      // PM * (ST * (NUT * PREC))
      for (Integer p = 0; p < 3; ++p)
      {
         p3 = 3*p;
         for (Integer q = 0; q < 3; ++q)
         {
            rot[p][q] = pmData[p3]   * tmp[0][q] + 
                        pmData[p3+1] * tmp[1][q] + 
                        pmData[p3+2] * tmp[2][q];
         }
      }

      rotMatrix.Set(rot[0][0], rot[1][0], rot[2][0],
                    rot[0][1], rot[1][1], rot[2][1],
                    rot[0][2], rot[1][2], rot[2][2]);

      Real determinant = 
              rot[0][0] * (rot[1][1] * rot[2][2] - rot[1][2]*rot[2][1]) +
              rot[0][1] * (rot[1][2] * rot[2][0] - rot[1][0]*rot[2][2]) +
              rot[0][2] * (rot[1][0] * rot[2][1] - rot[1][1]*rot[2][0]);
      if (Abs(determinant - 1.0) > DETERMINANT_TOLERANCE)
         throw CoordinateSystemException(
               "Computed rotation matrix has a determinant not equal to 1.0");

      // NUT * PREC calculated above
      // STderiv * (NUT * PREC)
      for (Integer p = 0; p < 3; ++p)
      {
         p3 = 3*p;
         for (Integer q = 0; q < 3; ++q)
         {
            tmp[p][q] = stDerivData[p3]   * np[0][q] + 
                        stDerivData[p3+1] * np[1][q] + 
                        stDerivData[p3+2] * np[2][q];
         }
      }     
      
      // PM * (STderiv * (NUT * PREC))
      for (Integer p = 0; p < 3; ++p)
      {
         p3 = 3*p;
         for (Integer q = 0; q < 3; ++q)
         {
            rot[p][q] = pmData[p3]   * tmp[0][q] + 
                        pmData[p3+1] * tmp[1][q] + 
                        pmData[p3+2] * tmp[2][q];
         }
      }
      rotDotMatrix.Set(rot[0][0], rot[1][0], rot[2][0],
                       rot[0][1], rot[1][1], rot[2][1],
                       rot[0][2], rot[1][2], rot[2][2]);



      #ifdef DEBUG_FIRST_CALL
         if (!firstCallFired) 
         {
            MessageInterface::ShowMessage("FK5 Components\n   PREC = \n");
            for (Integer m = 0; m < 3; ++m)
               MessageInterface::ShowMessage(
                  "          %20.12le %20.12le %20.12le \n", PREC(m,0), 
                  PREC(m,1), PREC(m,2));
            MessageInterface::ShowMessage("\n   NUT = \n");
            for (Integer m = 0; m < 3; ++m)
               MessageInterface::ShowMessage(
                  "          %20.12le %20.12le %20.12le \n", NUT(m,0), 
                  NUT(m,1), NUT(m,2));
            MessageInterface::ShowMessage("\n   ST = \n");
            for (Integer m = 0; m < 3; ++m)
               MessageInterface::ShowMessage(
                  "          %20.12le %20.12le %20.12le \n", ST(m,0), 
                  ST(m,1), ST(m,2));
            MessageInterface::ShowMessage("\n   STderiv = \n");
            for (Integer m = 0; m < 3; ++m)
               MessageInterface::ShowMessage(
                  "          %20.12le %20.12le %20.12le \n", STderiv(m,0), 
                  STderiv(m,1), STderiv(m,2));
            MessageInterface::ShowMessage("\n   PM = \n");
            for (Integer m = 0; m < 3; ++m)
               MessageInterface::ShowMessage(
                  "          %20.12le %20.12le %20.12le \n", PM(m,0), 
                  PM(m,1), PM(m,2));
            MessageInterface::ShowMessage("\n   rotMatrix = \n");
            for (Integer m = 0; m < 3; ++m)
               MessageInterface::ShowMessage(
                  "          %20.12le %20.12le %20.12le \n", rotMatrix(m,0), 
                  rotMatrix(m,1), rotMatrix(m,2));
         }
      #endif

   }
   else
   {
      #ifdef DEBUG_FIRST_CALL
         if (!firstCallFired)
            MessageInterface::ShowMessage("Body is %s\n", originName.c_str());
      #endif
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
   
   #ifdef DEBUG_FIRST_CALL
      firstCallFired = true;
   #endif

}


