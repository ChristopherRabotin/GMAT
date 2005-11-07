//$Header$
//------------------------------------------------------------------------------
//                                  GeocentricSolarMagneticAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
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
#include "RealTypes.hpp"
#include "TimeTypes.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateSystemException.hpp"

#include <iostream>
using namespace std; // *********************

using namespace GmatMathUtil;      // for trig functions, etc.
using namespace GmatTimeUtil;      // for JD offsets, etc.

//---------------------------------
// static data
//---------------------------------
const Real GeocentricSolarMagneticAxes::lambdaD = 70.1;  // degrees West
const Real GeocentricSolarMagneticAxes::phiD    = 78.6;  // degrees North


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


GmatCoordinate::ParameterUsage GeocentricSolarMagneticAxes::UsesEopFile() const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage GeocentricSolarMagneticAxes::UsesItrfFile() const
{
   return GmatCoordinate::REQUIRED;
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
   dipoleEF(0) = Cos(phiD) * Cos(-lambdaD);
   dipoleEF(1) = Cos(phiD) * Sin(-lambdaD);
   dipoleEF(2) = Sin(phiD);
}

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
void GeocentricSolarMagneticAxes::CalculateRotationMatrix(const A1Mjd &atEpoch) 
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

   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetUpdateInterval();
   else                        updateIntervalToUse = updateInterval;
   Rmatrix33  PREC      = ComputePrecessionMatrix(tTDB, atEpoch);
   Rmatrix33  NUT       = ComputeNutationMatrix(tTDB, atEpoch, dPsi,
                                                longAscNodeLunar, cosEpsbar);
   Rmatrix33  ST        = ComputeSiderealTimeRotation(jdTT, tUT1,
                                                      dPsi, longAscNodeLunar, 
                                                      cosEpsbar,
                                                      cosAst, sinAst);
   Rmatrix33  STderiv   = ComputeSiderealTimeDotRotation(mjdUTC, atEpoch,
                                                         cosAst, sinAst);
   Rmatrix33  PM        = ComputePolarMotionRotation(mjdUTC, atEpoch);

   Rmatrix33 fixedToMJ2000    = PREC.Transpose() * (NUT.Transpose() *
                                 (ST.Transpose() * PM.Transpose()));
   Real determinant = fixedToMJ2000.Determinant();
   if (Abs(determinant - 1.0) > DETERMINANT_TOLERANCE)
      throw CoordinateSystemException(
            "Computed rotation matrix has a determinant not equal to 1.0");
   Rmatrix33 fixedToMJ2000Dot = PREC.Transpose() * (NUT.Transpose() *
                                (STderiv.Transpose() * PM.Transpose()));


   Rvector3 dipoleFK5 = fixedToMJ2000 * dipoleEF;
   
   Rvector6 rvSun = secondary->GetMJ2000State(atEpoch) -
                    primary->GetMJ2000State(atEpoch);
   Rvector3 rSun  = rvSun.GetR();
   Rvector3 vSun  = rvSun.GetV();

   Real     rMag  = rSun.GetMagnitude();
   Real     vMag  = vSun.GetMagnitude();

   Rvector3 rUnit = rSun / rMag;
   Rvector3 vUnit = vSun / vMag;

   Rvector3 x     = rUnit;
   Rvector3 yFK5  = Cross(dipoleFK5,x);
   Real     yMag  = yFK5.GetMagnitude();
   Rvector3 y     = yFK5 / yMag;
   Rvector3 z     = Cross(x,y);

   rotMatrix(0,0) = x(0);
   rotMatrix(0,1) = y(0);
   rotMatrix(0,2) = z(0);
   rotMatrix(1,0) = x(1);
   rotMatrix(1,1) = y(1);
   rotMatrix(1,2) = z(1);
   rotMatrix(2,0) = x(2);
   rotMatrix(2,1) = y(2);
   rotMatrix(2,2) = z(2);

   Rvector3 vR    = vSun / rMag;
   Rvector3 xDot  = vR - x * (x * vR);
   Rvector3 yTmp  = Cross((fixedToMJ2000Dot * dipoleEF), x) +
                    Cross((fixedToMJ2000 * dipoleEF), xDot);
   Rvector3 yDot  = (yTmp / yMag) - y * (y * (yTmp / yMag));
   Rvector3 zDot  = Cross(xDot, y) + Cross(x, yDot);
      
   rotDotMatrix(0,0) = xDot(0);
   rotDotMatrix(0,1) = yDot(0);
   rotDotMatrix(0,2) = zDot(0);
   rotDotMatrix(1,0) = xDot(1);
   rotDotMatrix(1,1) = yDot(1);
   rotDotMatrix(1,2) = zDot(1);
   rotDotMatrix(2,0) = xDot(2);
   rotDotMatrix(2,1) = yDot(2);
   rotDotMatrix(2,2) = zDot(2);

}


