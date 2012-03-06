//$Id$
//------------------------------------------------------------------------------
//                                  GeocentricSolarMagneticAxes
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
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/06/02
//
/**
 * Implementation of the GeocentricSolarMagneticAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "GmatBaseException.hpp"
#include "ObjectReferencedAxes.hpp"
#include "GeocentricSolarMagneticAxes.hpp"
#include "DynamicAxes.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Planet.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include "GmatConstants.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateSystemException.hpp"
#include "MessageInterface.hpp"

#include <iostream>


using namespace GmatMathUtil;      // for trig functions, etc.
using namespace GmatTimeConstants;      // for JD offsets, etc.

//#define ROT_MAT_DEBUG
//#define DEBUG_TIME_CALC
//---------------------------------
// static data
//---------------------------------
// wcs - 2006.05.01 added negative sign per Bug 260
const Real GeocentricSolarMagneticAxes::lambdaD = -70.1 * GmatMathConstants::RAD_PER_DEG;  // degrees West
const Real GeocentricSolarMagneticAxes::phiD    = 78.6 * GmatMathConstants::RAD_PER_DEG;  // degrees North


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  GeocentricSolarMagneticAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base GeocentricSolarMagneticAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 */
//------------------------------------------------------------------------------
GeocentricSolarMagneticAxes::GeocentricSolarMagneticAxes(const std::string &itsName) :
ObjectReferencedAxes("GSM",itsName)
{
   primaryName   = SolarSystem::EARTH_NAME;
   secondaryName = SolarSystem::SUN_NAME;
   ComputeDipoleEarthFixed();
   objectTypeNames.push_back("GSM");
   parameterCount = GeocentricSolarMagneticAxesParamCount;
}


//------------------------------------------------------------------------------
//  GeocentricSolarMagneticAxes(const GeocentricSolarMagneticAxes &gse);
//------------------------------------------------------------------------------
/**
 * Constructs base GeocentricSolarMagneticAxes structures used in derived classes, 
 * by copying the input instance (copy constructor).
 *
 * @param gse  GeocentricSolarMagneticAxes instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
GeocentricSolarMagneticAxes::GeocentricSolarMagneticAxes(
                             const GeocentricSolarMagneticAxes &gse) :
ObjectReferencedAxes(gse),
dipoleEF            (gse.dipoleEF)
{
}

//------------------------------------------------------------------------------
//  GeocentricSolarMagneticAxes& operator=(
//                               const GeocentricSolarMagneticAxes &gse)
//------------------------------------------------------------------------------
/**
 * Assignment operator for GeocentricSolarMagneticAxes structures.
 *
 * @param gse The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const GeocentricSolarMagneticAxes& 
GeocentricSolarMagneticAxes::operator=(const GeocentricSolarMagneticAxes &gse)
{
   if (&gse == this)
      return *this;
   ObjectReferencedAxes::operator=(gse);
   dipoleEF = gse.dipoleEF;
   return *this;
}
//------------------------------------------------------------------------------
//  ~GeocentricSolarMagneticAxes()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GeocentricSolarMagneticAxes::~GeocentricSolarMagneticAxes()
{
}


GmatCoordinate::ParameterUsage GeocentricSolarMagneticAxes::UsesXAxis() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage GeocentricSolarMagneticAxes::UsesYAxis() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage GeocentricSolarMagneticAxes::UsesZAxis() const
{
   return GmatCoordinate::NOT_USED;
}


GmatCoordinate::ParameterUsage GeocentricSolarMagneticAxes::UsesEopFile(const std::string &forBaseSystem) const
{
   if (forBaseSystem == baseSystem)
      return GmatCoordinate::REQUIRED;
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage GeocentricSolarMagneticAxes::UsesItrfFile() const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage 
GeocentricSolarMagneticAxes::UsesNutationUpdateInterval() const
{
   //if (originName == SolarSystem::EARTH_NAME) 
      return GmatCoordinate::REQUIRED;
   //return ObjectReferencedAxes::UsesNutationUpdateInterval();
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for this GeocentricSolarMagneticAxes.
 *
 */
//------------------------------------------------------------------------------
bool GeocentricSolarMagneticAxes::Initialize()
{
   ObjectReferencedAxes::Initialize();
   //if (originName == SolarSystem::EARTH_NAME) // 2006.03.14 WCS 
   InitializeFK5();
   
   dipEF = dipoleEF.GetDataVector();
   rvSun = rvSunVec.GetDataVector();

   return true;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the GeocentricSolarMagneticAxes.
 *
 * @return clone of the GeocentricSolarMagneticAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* GeocentricSolarMagneticAxes::Clone() const
{
   return (new GeocentricSolarMagneticAxes(*this));
}
   
//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void ComputeDipoleEarthFixed()
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
void GeocentricSolarMagneticAxes::ComputeDipoleEarthFixed()
{
   // wcs 2006.05.01 - removed negative signs on lambdaD per Bug 260
   dipoleEF(0) = Cos(phiD) * Cos(lambdaD);
   dipoleEF(1) = Cos(phiD) * Sin(lambdaD);
   dipoleEF(2) = Sin(phiD);
}

//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation = false)
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
void GeocentricSolarMagneticAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                  bool forceComputation) 
{
   Real dPsi             = 0.0;
   Real longAscNodeLunar = 0.0;
   Real cosEpsbar        = 0.0;
   Real cosAst           = 0.0;
   Real sinAst           = 0.0;
   //Real xVal             = 0.0;
   //Real yVal             = 0.0;
   // Convert to MJD UTC to use for polar motion  and LOD
   // interpolations
   // 20.02.06 - arg: changed to use enum types instead of strings
//   Real mjdUTC = TimeConverterUtil::Convert(atEpoch.Get(),
//                  "A1Mjd", "UtcMjd", JD_JAN_5_1941);
   Real mjdUTC = TimeConverterUtil::Convert(atEpoch.Get(),
                  TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD, 
                  JD_JAN_5_1941);
   Real offset = JD_JAN_5_1941 - JD_NOV_17_1858;
   // convert to MJD referenced from time used in EOP file
   mjdUTC = mjdUTC + offset;


   // convert input time to UT1 for later use (for AST calculation)
   // 20.02.06 - arg: changed to use enum types instead of strings
//   Real mjdUT1 = TimeConverterUtil::Convert(atEpoch.Get(),
//                  "A1Mjd", "Ut1Mjd", JD_JAN_5_1941);

   Real mjdUT1 = TimeConverterUtil::Convert(atEpoch.Get(),
                  TimeConverterUtil::A1MJD, TimeConverterUtil::UT1MJD,
                  JD_JAN_5_1941);
   //Real jdUT1    = mjdUT1 + JD_JAN_5_1941; // right?
                                             // Compute elapsed Julian centuries (UT1)
   //Real tUT1     = (jdUT1 - JD_OF_J2000) / DAYS_PER_JULIAN_CENTURY;
   offset =  JD_JAN_5_1941 - JD_OF_J2000;
   Real tUT1     = (mjdUT1 + offset) / DAYS_PER_JULIAN_CENTURY;
   

   // convert input A1 MJD to TT MJD (for most calculations)
   // 20.02.06 - arg: changed to use enum types instead of strings
//   Real mjdTT = TimeConverterUtil::Convert(atEpoch.Get(),
//                  "A1Mjd", "TtMjd", JD_JAN_5_1941);
   Real mjdTT = TimeConverterUtil::Convert(atEpoch.Get(),
                  TimeConverterUtil::A1MJD, TimeConverterUtil::TTMJD,
                  JD_JAN_5_1941);
   Real jdTT    = mjdTT + JD_JAN_5_1941; // right?
                                          // Compute Julian centuries of TDB from the base epoch (J2000)
                                          // NOTE - this is really TT, an approximation of TDB *********
   //Real tTDB    = (jdTT - JD_OF_J2000) / DAYS_PER_JULIAN_CENTURY;
   Real tTDB    = (mjdTT + offset) / DAYS_PER_JULIAN_CENTURY;

   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetNutationUpdateInterval();
   else                        updateIntervalToUse = updateInterval;

   ComputePrecessionMatrix(tTDB, atEpoch);
   ComputeNutationMatrix(tTDB, atEpoch, dPsi, longAscNodeLunar, cosEpsbar,
                         forceComputation);
   ComputeSiderealTimeRotation(jdTT, tUT1, dPsi, longAscNodeLunar, cosEpsbar,
                               cosAst, sinAst);
   ComputeSiderealTimeDotRotation(mjdUTC, atEpoch, cosAst, sinAst,
                                  forceComputation);
   ComputePolarMotionRotation(mjdUTC, atEpoch, forceComputation);

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
   
   //static Rmatrix33 fixedToMJ2000;
   
   Real fixedToMJ2000[3][3];
   for (Integer i=0;i<3;i++)
      for (Integer j=0;j<3;j++)
         fixedToMJ2000[i][j] = rot[j][i];
   //fixedToMJ2000.Set(rot[0][0], rot[1][0], rot[2][0],
   //                  rot[0][1], rot[1][1], rot[2][1],
   //                  rot[0][2], rot[1][2], rot[2][2]);

   Real determinant = 
           fixedToMJ2000[0][0] * (fixedToMJ2000[1][1] * fixedToMJ2000[2][2] 
              - fixedToMJ2000[2][1]*fixedToMJ2000[1][2]) +
           fixedToMJ2000[1][0] * (fixedToMJ2000[2][1] * fixedToMJ2000[0][2] 
              - fixedToMJ2000[0][1]*fixedToMJ2000[2][2]) +
           fixedToMJ2000[2][0] * (fixedToMJ2000[0][1] * fixedToMJ2000[1][2] 
              - fixedToMJ2000[1][1]*fixedToMJ2000[0][2]);
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
   Real fixedToMJ2000Dot[3][3];
   for (Integer i=0;i<3;i++)
      for (Integer j=0;j<3;j++)
         fixedToMJ2000Dot[i][j] = rot[j][i];
   //static Rmatrix33 fixedToMJ2000Dot;
   //fixedToMJ2000Dot.Set(rot[0][0], rot[1][0], rot[2][0],
   //                     rot[0][1], rot[1][1], rot[2][1],
   //                     rot[0][2], rot[1][2], rot[2][2]);
   //rotDotMatrix.Set(rot[0][0], rot[1][0], rot[2][0],
   //                 rot[0][1], rot[1][1], rot[2][1],
   //                 rot[0][2], rot[1][2], rot[2][2]);

   //Rmatrix33 fixedToMJ2000    = PREC.Transpose() * (NUT.Transpose() *
   //                              (ST.Transpose() * PM.Transpose()));
    //Real determinant = fixedToMJ2000.Determinant();
   //if (Abs(determinant - 1.0) > DETERMINANT_TOLERANCE)
   //   throw CoordinateSystemException(
   //         "Computed rotation matrix has a determinant not equal to 1.0");
   //Rmatrix33 fixedToMJ2000Dot = PREC.Transpose() * (NUT.Transpose() *
   //                             (STderiv.Transpose() * PM.Transpose()));


   //Rvector3 dipoleFK5 = fixedToMJ2000 * dipoleEF;
   Real dipoleFK5[3];
   //Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      //p3 = 3*p;
      dipoleFK5[p] = fixedToMJ2000[p][0]   * dipEF[0] + 
                     fixedToMJ2000[p][1]   * dipEF[1] + 
                     fixedToMJ2000[p][2]   * dipEF[2];
  }  
   
   
   rvSunVec = secondary->GetMJ2000State(atEpoch) -
              primary->GetMJ2000State(atEpoch);

   //Rvector3 rSun  = rvSun.GetR();
   //Rvector3 vSun  = rvSun.GetV();
   Real rSun[3] = {rvSun[0], rvSun[1], rvSun[2]};
   Real vSun[3] = {rvSun[3], rvSun[4], rvSun[5]};

   //Real     rMag  = rSun.GetMagnitude();
   //Real     vMag  = vSun.GetMagnitude();
   Real rMag = GmatMathUtil::Sqrt((rSun[0] * rSun[0]) + (rSun[1] * rSun[1]) +
                                  (rSun[2] * rSun[2]));
   ////Real vMag = GmatMathUtil::Sqrt((vSun[0] * vSun[0]) + (vSun[1] * vSun[1]) +
   ////                               (vSun[2] * vSun[2]));

   //Rvector3 rUnit = rSun / rMag;
   //Rvector3 vUnit = vSun / vMag;
   // unitize vectors
   Real x[3]     = {rSun[0] / rMag, rSun[1] / rMag, rSun[2] / rMag}; // was rUnit
   ////Real vUnit[3] = {vSun[0] / vMag, vSun[1] / vMag, vSun[2] / vMag};

   //Rvector3 x     = rUnit;
   //Rvector3 yFK5  = Cross(dipoleFK5,x);
   Real yFK5[3] = {dipoleFK5[1]*x[2] - dipoleFK5[2]*x[1],
                   dipoleFK5[2]*x[0] - dipoleFK5[0]*x[2],
                   dipoleFK5[0]*x[1] - dipoleFK5[1]*x[0]};
   //Real     yMag  = yFK5.GetMagnitude();
   Real     yMag  = GmatMathUtil::Sqrt((yFK5[0] * yFK5[0]) + (yFK5[1] * yFK5[1]) +
                                       (yFK5[2] * yFK5[2]));
   //Rvector3 y     = yFK5 / yMag;
   Real y[3] = {yFK5[0] / yMag, yFK5[1] / yMag, yFK5[2] / yMag};
   //Rvector3 z     = Cross(x,y);
   Real z[3] = {x[1]*y[2] - x[2]*y[1],
                x[2]*y[0] - x[0]*y[2],
                x[0]*y[1] - x[1]*y[0]};
                
   rotMatrix.Set(x[0], y[0], z[0],
                 x[1], y[1], z[1],
                 x[2], y[2], z[2]);
              
   /*
   rotMatrix(0,0) = x(0);
   rotMatrix(0,1) = y(0);
   rotMatrix(0,2) = z(0);
   rotMatrix(1,0) = x(1);
   rotMatrix(1,1) = y(1);
   rotMatrix(1,2) = z(1);
   rotMatrix(2,0) = x(2);
   rotMatrix(2,1) = y(2);
   rotMatrix(2,2) = z(2);
   */
   //Rvector3 vR    = vSun / rMag;
   Real vR[3] = {vSun[0] / rMag, vSun[1] / rMag, vSun[2] / rMag};
   Real product = x[0] * vR[0] + x[1] * vR[1] + x[2] * vR[2];
   //Rvector3 xDot  = vR - x * (x * vR);
   Real xDot[3] = {vR[0] - x[0] * product,
                   vR[1] - x[1] * product,
                   vR[2] - x[2] * product};
   Real tmp1[3], tmp2[3];
   //Rvector3 yTmp  = Cross((fixedToMJ2000Dot * dipoleEF), x) +
   //                 Cross((fixedToMJ2000 * dipoleEF), xDot);
   for (Integer p = 0; p < 3; ++p)
   {
      //p3 = 3*p;
         tmp1[p] = fixedToMJ2000Dot[p][0]   * dipEF[0] + 
                   fixedToMJ2000Dot[p][1]   * dipEF[1] + 
                   fixedToMJ2000Dot[p][2]   * dipEF[2];
         tmp2[p] = fixedToMJ2000[p][0]      * dipEF[0] + 
                   fixedToMJ2000[p][1]      * dipEF[1] + 
                   fixedToMJ2000[p][2]      * dipEF[2];
   }  
   // include division by yMag here
   Real yTmp[3] = {((tmp1[1]*x[2] - tmp1[2]*x[1]) + (tmp2[1]*xDot[2] - tmp2[2]*xDot[1])) / yMag,
                   ((tmp1[2]*x[0] - tmp1[0]*x[2]) + (tmp2[2]*xDot[0] - tmp2[0]*xDot[2])) / yMag,
                   ((tmp1[0]*x[1] - tmp1[1]*x[0]) + (tmp2[0]*xDot[1] - tmp2[1]*xDot[0])) / yMag};
   Real yVal = y[0] * yTmp[0] + y[1] * yTmp[1] + y[2] * yTmp[2];
   //Rvector3 yDot  = (yTmp / yMag) - y * (y * (yTmp / yMag));
   Real yDot[3] = {yTmp[0] - y[0] * yVal,
                   yTmp[1] - y[1] * yVal,
                   yTmp[2] - y[2] * yVal};
   //Rvector3 zDot  = Cross(xDot, y) + Cross(x, yDot);
   Real zDot[3] = {(xDot[1]*y[2] - xDot[2]*y[1]) + (x[1]*yDot[2] - x[2]*yDot[1]), 
                   (xDot[2]*y[0] - xDot[0]*y[2]) + (x[2]*yDot[0] - x[0]*yDot[2]),
                   (xDot[0]*y[1] - xDot[1]*y[0]) + (x[0]*yDot[1] - x[1]*yDot[0])};
   /*   
   rotDotMatrix(0,0) = xDot(0);
   rotDotMatrix(0,1) = yDot(0);
   rotDotMatrix(0,2) = zDot(0);
   rotDotMatrix(1,0) = xDot(1);
   rotDotMatrix(1,1) = yDot(1);
   rotDotMatrix(1,2) = zDot(1);
   rotDotMatrix(2,0) = xDot(2);
   rotDotMatrix(2,1) = yDot(2);
   rotDotMatrix(2,2) = zDot(2);
  */
  rotDotMatrix.Set(xDot[0],yDot[0],zDot[0],
                   xDot[1],yDot[1],zDot[1],
                   xDot[2],yDot[2],zDot[2]);
                   
   #ifdef ROT_MAT_DEBUG
      static Integer num = 0;
      if (num == 0)
      {
         MessageInterface::ShowMessage("****** BEGIN temporary debug ...........\n");
         MessageInterface::ShowMessage("Sun vector(MJ2000) = %f %f %f\n", 
         rvSun(0), rvSun(1), rvSun(2));
         MessageInterface::ShowMessage("Dipole (fixed) = %f %f %f\n", 
         dipoleEF(0), dipoleEF(1), dipoleEF(2));
         MessageInterface::ShowMessage("Dipole (MJ2000) = %f %f %f\n", 
         dipoleFK5(0), dipoleFK5(1), dipoleFK5(2));
         MessageInterface::ShowMessage("x = %f %f %f\n", 
         x(0), x(1), x(2));
         MessageInterface::ShowMessage("y = %f %f %f\n", 
         y(0), y(1), y(2));
         MessageInterface::ShowMessage("z = %f %f %f\n", 
         z(0), z(1), z(2));
         MessageInterface::ShowMessage("xDot = %f %f %f\n", 
         xDot(0), xDot(1), xDot(2));
         MessageInterface::ShowMessage("yDot = %f %f %f\n", 
         yDot(0), yDot(1), yDot(2));
         MessageInterface::ShowMessage("zDot = %f %f %f\n", 
         zDot(0), zDot(1), zDot(2));
         MessageInterface::ShowMessage("rotation matrix = \n%f %f %f\n%f %f %f\n%f %f %f\n",
         rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2), rotMatrix(1,0), 
         rotMatrix(1,1), rotMatrix(1,2), rotMatrix(2,0), rotMatrix(2,1), 
         rotMatrix(2,2));
         MessageInterface::ShowMessage("rotation matrix (derivative) = \n%f %f %f\n%f %f %f\n%f %f %f\n",
         rotDotMatrix(0,0), rotDotMatrix(0,1), rotDotMatrix(0,2), rotDotMatrix(1,0), 
         rotDotMatrix(1,1), rotDotMatrix(1,2), rotDotMatrix(2,0), rotDotMatrix(2,1), 
         rotDotMatrix(2,2));
         MessageInterface::ShowMessage("****** END temporary debug ...........\n");
         num++;
      }
   #endif

}


