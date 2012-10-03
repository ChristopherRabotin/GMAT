//$Id: BodySpinSunAxes.cpp 10276 2012-09-17 16:41:37Z tdnguyen $
//------------------------------------------------------------------------------
//                                  BodySpinSunAxes
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
// Author: Tuan Dang Nguyen/GSFC
// Created: 2012/09/17
//
/**
 * Implementation of the BodySpinSunAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "GmatBaseException.hpp"
#include "ObjectReferencedAxes.hpp"
#include "BodySpinSunAxes.hpp"
#include "DynamicAxes.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Planet.hpp"
#include "RealUtilities.hpp"
#include "AngleUtil.hpp"
#include "GmatConstants.hpp"
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
using namespace GmatMathConstants;      // for RAD_PER_ARCSEC, etc.

#define DEBUG_CONSTRUCTION
#define DEBUG_INITIALIZE
#define DEBUG_TIME_CALC
#define DEBUG_ROT_MATRIX

//---------------------------------
// static data
//---------------------------------

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  BodySpinSunAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base BodySpinSunAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 */
//------------------------------------------------------------------------------
BodySpinSunAxes::BodySpinSunAxes(const std::string &itsName) :
ObjectReferencedAxes("BSS",itsName)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("BodySpinSunAxes::BodySpinSunAxes()   default construction.\n");
#endif

   primaryName   = SolarSystem::SUN_NAME;				// SolarSystem::EARTH_NAME;
   secondaryName = "";									// Name of the secondary has not defined yet. It will be defined in the script.
   spinaxisBF.Set(0.0, 0.0, 1.0);

   objectTypeNames.push_back("BSS");
   parameterCount = BodySpinSunAxesParamCount;
}


//------------------------------------------------------------------------------
//  BodySpinSunAxes(const BodySpinSunAxes &bss);
//------------------------------------------------------------------------------
/**
 * Constructs base BodySpinSunAxes structures used in derived classes, 
 * by copying the input instance (copy constructor).
 *
 * @param gse  BodySpinSunAxes instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
BodySpinSunAxes::BodySpinSunAxes(const BodySpinSunAxes &bss) :
ObjectReferencedAxes(bss),
spinaxisBF			(bss.spinaxisBF)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("BodySpinSunAxes::BodySpinSunAxes()   copy construction.\n");
#endif

}

//------------------------------------------------------------------------------
//  BodySpinSunAxes& operator=(const BodySpinSunAxes &bss)
//------------------------------------------------------------------------------
/**
 * Assignment operator for BodySpinSunAxes structures.
 *
 * @param bss The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const BodySpinSunAxes& BodySpinSunAxes::operator=(const BodySpinSunAxes &bss)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("BodySpinSunAxes::operator =\n");
#endif

   if (&bss == this)
      return *this;

   ObjectReferencedAxes::operator=(bss);
   spinaxisBF = bss.spinaxisBF;

   return *this;
}
//------------------------------------------------------------------------------
//  ~BodySpinSunAxes()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BodySpinSunAxes::~BodySpinSunAxes()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("BodySpinSunAxes::~BodySpinSunAxes()   destruction.\n");
#endif

}


GmatCoordinate::ParameterUsage BodySpinSunAxes::UsesXAxis() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage BodySpinSunAxes::UsesYAxis() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage BodySpinSunAxes::UsesZAxis() const
{
   return GmatCoordinate::NOT_USED;
}


GmatCoordinate::ParameterUsage BodySpinSunAxes::UsesEopFile(const std::string &forBaseSystem) const
{
   if (forBaseSystem == baseSystem)
      return GmatCoordinate::REQUIRED;
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage BodySpinSunAxes::UsesItrfFile() const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage BodySpinSunAxes::UsesNutationUpdateInterval() const
{
   //if (originName == SolarSystem::EARTH_NAME) 
      return GmatCoordinate::REQUIRED;
   //return ObjectReferencedAxes::UsesNutationUpdateInterval();
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for this BodySpinSunAxes.
 *
 */
//------------------------------------------------------------------------------
bool BodySpinSunAxes::Initialize()
{
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("BodySpinSunAxes::Initialize().\n");
#endif

   ObjectReferencedAxes::Initialize();
   //if (originName == SolarSystem::EARTH_NAME) // 2006.03.14 WCS 
   InitializeFK5();

   // secondary = origin;							// the secondary is not the same as the origin for BodySpinSun coordinate system
   spinBF = spinaxisBF.GetDataVector();
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
 * This method returns a clone of the BodySpinSunAxes.
 *
 * @return clone of the BodySpinSunAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* BodySpinSunAxes::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("BodySpinSunAxes::Clone().\n");
#endif

   return (new BodySpinSunAxes(*this));
}
   
//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


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
void BodySpinSunAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                  bool forceComputation) 
{
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("BodySpinSunAxes::CalculateRotationMatrix()  Enter\n");
   #endif

   if (primary == NULL)
   {
	   MessageInterface::ShowMessage("Error: primary object of the coordinate system is not defined...\n");
	   throw new CoordinateSystemException("Error: primary object of the coordinate system is not defined...\n");
   }
   
   if (secondary == NULL)
   {
	   MessageInterface::ShowMessage("Error: secondary object of the coordinate system is not defined...\n");
	   throw new CoordinateSystemException("Error: secondary object of the coordinate system is not defined...\n");
   }
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("primary: '%s'(%p)\n", primary->GetName().c_str(), primary);
      MessageInterface::ShowMessage("secondary: '%s'(%p)\n", secondary->GetName().c_str(), secondary);
   #endif


   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("Time conversion steps\n");
   #endif

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
   
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("   A1Mjd atEpoch = %lf\n", atEpoch.Get());
      MessageInterface::ShowMessage("         mjdUTC  = %lf\n", mjdUTC);
      MessageInterface::ShowMessage("         mjdUT1  = %lf\n", mjdUT1);
      MessageInterface::ShowMessage("         tUT1    = %lf\n", tUT1);
      MessageInterface::ShowMessage("         mjdTT   = %lf\n", mjdTT);
      MessageInterface::ShowMessage("         jdTT    = %lf\n", jdTT);
      MessageInterface::ShowMessage("         tTDB    = %lf\n\n", tTDB);
   #endif




	  
   Real fixedToMJ2000[3][3];
   Real fixedToMJ2000Dot[3][3];

   #ifdef DEBUG_ROT_MATRIX
         MessageInterface::ShowMessage("Calculate rotation matrix and dot rotation matrix from body fixed frame to MJ2000Eq\n");
   #endif
   if (((CelestialBody*)secondary)->GetRotationDataSource() != Gmat::IAU_SIMPLIFIED)
   {
      #ifdef DEBUG_ROT_MATRIX
         MessageInterface::ShowMessage("For planets which do not use IAU_SIMPLIFIED data source, GMAT calculates Precession,\n" 
		      "Nutation, side real time rotation, side real time dot rotation, and polar motion rotation matrixes:\n");
      #endif
      Real dPsi             = 0.0;
      Real longAscNodeLunar = 0.0;
      Real cosEpsbar        = 0.0;
      Real cosAst           = 0.0;
      Real sinAst           = 0.0;

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
   
      // Calculate rotation matrix: np = NUT * PREC
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
   
      // Calculate rotation matrix: ST * (NUT * PREC)
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
   
      // Calculate rotation matrix: rot = PM * (ST * (NUT * PREC))
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
   
      // Set: fixedToMJ2000 = PM * (ST * (NUT * PREC))
      for (Integer i=0;i<3;i++)
         for (Integer j=0;j<3;j++)
            fixedToMJ2000[i][j] = rot[j][i];

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
      // Calculate rotation matrix: STderiv * (NUT * PREC)
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

      // Calculate rotation matrix: PM * (STderiv * (NUT * PREC))
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

      // Set: fixedToMJ2000Dot = PM * (STderiv * (NUT * PREC))
      for (Integer i=0;i<3;i++)
         for (Integer j=0;j<3;j++)
            fixedToMJ2000Dot[i][j] = rot[j][i];
   }
   else
   {
      #ifdef DEBUG_ROT_MATRIX
         MessageInterface::ShowMessage("For planets which use IAU_SIMPLIFIED data source, GMAT calculates rotation\n" 
		      "matrix and rotation dot matrix based on alpha, delta, W, and Wdot:\n");
      #endif
	  CelestialBody* body = (CelestialBody*)secondary;
	  Rvector6 bodyorientation = body->GetOrientationParameters();
      Real t0	= body->GetRealParameter(body->GetParameterID("OrientationEpoch"));		// unit: day

	  Real now  = atEpoch.Get();									// unit: day
      Real t	= now -t0;											// unit: day
	  Real w	= bodyorientation[5]*GmatMathConstants::PI/180;		// unit: radian per day
	  Real PRA  = bodyorientation[4]*GmatMathConstants::PI/180;		// unit: radian
	  Real RA	= bodyorientation[0]*GmatMathConstants::PI/180;		// unit: radian
	  Real DEC  = bodyorientation[2]*GmatMathConstants::PI/180;		// unit: radian
      Real W = PRA + w*t;										// unit: radian
	  W = AngleUtil::PutAngleInRadRange(W,0.0,GmatMathConstants::TWO_PI);

      Rmatrix33 R3wT(cos(W), -sin(W), 0.0,
                     sin(W),  cos(W), 0.0,
                        0.0,     0.0, 1.0);
	  Rmatrix33 R3wdotT(-w*GmatTimeConstants::DAYS_PER_SEC*sin(W), -w*GmatTimeConstants::DAYS_PER_SEC*cos(W), 0.0,
		                 w*GmatTimeConstants::DAYS_PER_SEC*cos(W), -w*GmatTimeConstants::DAYS_PER_SEC*sin(W), 0.0,
						 0.0, 0.0, 0.0);

      Rmatrix33 R1T(1.0, 0.0, 0.0,
		           0.0, cos(GmatMathConstants::PI/2-DEC), -sin(GmatMathConstants::PI/2-DEC),
				   0.0, sin(GmatMathConstants::PI/2-DEC), cos(GmatMathConstants::PI/2-DEC));

	  Rmatrix33 R3T(cos(GmatMathConstants::PI/2+RA), -sin(GmatMathConstants::PI/2+RA), 0.0,
		           sin(GmatMathConstants::PI/2+RA),  cos(GmatMathConstants::PI/2+RA), 0.0,
                   0.0, 0.0, 1.0);

	  Rmatrix33 RIF = R3T*R1T*R3wT;
	  Rmatrix33 RIFdot = R3T*R1T*R3wdotT;

	  for (int i=0; i < 3; ++i)
         for (int j=0; j < 3; ++j)
		 {
            fixedToMJ2000[i][j] = RIF(i,j);
			fixedToMJ2000Dot[i][j] = RIFdot(i,j);
		 }
      

   #ifdef DEBUG_ROT_MATRIX
	  MessageInterface::ShowMessage("t0 = %lf,  now = %lf,  t = %lf\n", t0, now, t);
      MessageInterface::ShowMessage("w = %lf rad/day,   PRA = %lf rad,   W =%lf rad\n", w, PRA, W);
      MessageInterface::ShowMessage("RA = %lf rad,   DEC = %lfrad\n", RA, DEC);
      MessageInterface::ShowMessage("fixedToMJ2000 = [\n");
      for (int i = 0; i < 3; ++i)
      {
	     MessageInterface::ShowMessage("        ");
         for (int j = 0; j < 3; ++j)
         {
    	    MessageInterface::ShowMessage("%lf  ",fixedToMJ2000[i][j]);
         }
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("          ]\n");
      MessageInterface::ShowMessage("fixedToMJ2000Dot = [\n");
      for (int i = 0; i < 3; ++i)
      {
	     MessageInterface::ShowMessage("        ");
         for (int j = 0; j < 3; ++j)
         {
    	    MessageInterface::ShowMessage("%lf  ",fixedToMJ2000Dot[i][j]);
         }
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("          ]\n");
   #endif


   }


   //Rvector3 spinaxisFK5 = fixedToMJ2000 * spinaxisBF;
   Real spinaxisFK5[3];
   for (Integer p = 0; p < 3; ++p)
   {
      spinaxisFK5[p] = fixedToMJ2000[p][0]   * spinBF[0] + 
                       fixedToMJ2000[p][1]   * spinBF[1] + 
                       fixedToMJ2000[p][2]   * spinBF[2];
   }  
   

   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("Step to specify x-axis unit vector:\n");
   #endif
   
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
   Real x[3]     = {-rSun[0] / rMag, -rSun[1] / rMag, -rSun[2] / rMag}; // was rUnit
   ////Real vUnit[3] = {vSun[0] / vMag, vSun[1] / vMag, vSun[2] / vMag};

   //Rvector3 x     = rUnit;
   //Rvector3 yFK5  = Cross(spinaxisFK5,x);
   Real yFK5[3] = {spinaxisFK5[1]*x[2] - spinaxisFK5[2]*x[1],
                   spinaxisFK5[2]*x[0] - spinaxisFK5[0]*x[2],
                   spinaxisFK5[0]*x[1] - spinaxisFK5[1]*x[0]};
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
              
   //Rvector3 vR    = vSun / rMag;
   Real vR[3] = {vSun[0] / rMag, vSun[1] / rMag, vSun[2] / rMag};
   Real product = x[0] * vR[0] + x[1] * vR[1] + x[2] * vR[2];
   //Rvector3 xDot  = vR - x * (x * vR);
   Real xDot[3] = {vR[0] - x[0] * product,
                   vR[1] - x[1] * product,
                   vR[2] - x[2] * product};
   Real tmp1[3], tmp2[3];
   //Rvector3 yTmp  = Cross((fixedToMJ2000Dot * spinaxisBF), x) +
   //                 Cross((fixedToMJ2000 * spinaxisBF), xDot);
   for (Integer p = 0; p < 3; ++p)
   {
      //p3 = 3*p;
         tmp1[p] = fixedToMJ2000Dot[p][0]   * spinBF[0] + 
                   fixedToMJ2000Dot[p][1]   * spinBF[1] + 
                   fixedToMJ2000Dot[p][2]   * spinBF[2];
         tmp2[p] = fixedToMJ2000[p][0]      * spinBF[0] + 
                   fixedToMJ2000[p][1]      * spinBF[1] + 
                   fixedToMJ2000[p][2]      * spinBF[2];
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
   
  rotDotMatrix.Set(xDot[0],yDot[0],zDot[0],
                   xDot[1],yDot[1],zDot[1],
                   xDot[2],yDot[2],zDot[2]);
                   
   #ifdef DEBUG_ROT_MATRIX
//      static Integer num = 0;
//      if (num == 0)
      {
         MessageInterface::ShowMessage("****** BEGIN temporary debug ...........\n");
         MessageInterface::ShowMessage("Sun R vector(MJ2000) = %lf %lf %lf\n", rvSun[0], rvSun[1], rvSun[2]);
		 MessageInterface::ShowMessage("Sun V vector(MJ2000) = %lf %lf %lf\n", rvSun[3], rvSun[4], rvSun[5]);
         MessageInterface::ShowMessage("spin axis' unit vector (in body fixed coordinate system) = %lf %lf %lf\n", 
         spinaxisBF(0), spinaxisBF(1), spinaxisBF(2));
         MessageInterface::ShowMessage("spin axis' unit vector (in MJ2000 coordinate system) = %lf %lf %lf\n", 
         spinaxisFK5[0], spinaxisFK5[1], spinaxisFK5[2]);
         MessageInterface::ShowMessage("x-axis unit vector: x = %lf %lf %lf\n", x[0], x[1], x[2]);
         MessageInterface::ShowMessage("y-axis unit vector: y = %lf %lf %lf\n", y[0], y[1], y[2]);
         MessageInterface::ShowMessage("z-axis unit vector: z = %lf %lf %lf\n", z[0], z[1], z[2]);
         MessageInterface::ShowMessage("xDot = %le %le %le\n", xDot[0], xDot[1], xDot[2]);
         MessageInterface::ShowMessage("yDot = %le %le %le\n", yDot[0], yDot[1], yDot[2]);
         MessageInterface::ShowMessage("zDot = %le %le %le\n", zDot[0], zDot[1], zDot[2]);
         MessageInterface::ShowMessage("rotation matrix = \n%lf %lf %lf\n%lf %lf %lf\n%lf %lf %lf\n",
         rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2), rotMatrix(1,0), 
         rotMatrix(1,1), rotMatrix(1,2), rotMatrix(2,0), rotMatrix(2,1), 
         rotMatrix(2,2));
         MessageInterface::ShowMessage("rotation matrix (derivative) = \n%le %le %le\n%le %le %le\n%le %le %le\n",
         rotDotMatrix(0,0), rotDotMatrix(0,1), rotDotMatrix(0,2), rotDotMatrix(1,0), 
         rotDotMatrix(1,1), rotDotMatrix(1,2), rotDotMatrix(2,0), rotDotMatrix(2,1), 
         rotDotMatrix(2,2));
         MessageInterface::ShowMessage("****** END temporary debug ...........\n");
//         num++;
      }
   #endif

}



