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
#include <fstream>
#include <sstream>
#include <iomanip>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "BodyFixedAxes.hpp"
#include "DynamicAxes.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include "TimeTypes.hpp"
#include "Rvector3.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateSystemException.hpp"

using namespace GmatMathUtil;      // for trig functions, etc.
using namespace GmatTimeUtil;      // for JD offsets, etc.

// ************
//using namespace std;

//---------------------------------
// static data
//---------------------------------

const std::string
BodyFixedAxes::PARAMETER_TEXT[BodyFixedAxesParamCount - DynamicAxesParamCount] =
{
   "EopFilename",
   "CoefficientsFilename",
};

const Gmat::ParameterType
BodyFixedAxes::PARAMETER_TYPE[BodyFixedAxesParamCount - DynamicAxesParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
};

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
 * @param leapSecFile name of the file from which to get the leap
 *        second information.
 * @param itsName Optional name for the object.  Defaults to "".
 * @param leapSecFile name of the file from which to get the leap
 *        second information.
 */
//------------------------------------------------------------------------------
BodyFixedAxes::BodyFixedAxes(const std::string &itsName) :
DynamicAxes("BodyFixed",itsName),
eopFileName            (""),
itrfFileName           ("")
{
}

//------------------------------------------------------------------------------
//  BodyFixedAxes(const std::string eopFile,
//                const std::string coefFile,
//                const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base BodyFixedAxes structures
 * (constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 */
//------------------------------------------------------------------------------
BodyFixedAxes::BodyFixedAxes(const std::string eopFile,
                             const std::string coefFile,
                             const std::string &itsName) :
DynamicAxes("BodyFixed",itsName),
eopFileName            (eopFile),
itrfFileName           (coefFile)
{
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
DynamicAxes(bfAxes),
eopFileName            (bfAxes.eopFileName),
itrfFileName           (bfAxes.itrfFileName)
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
   // ********************* TBD ***********************
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

void BodyFixedAxes::SetEopFile(EopFile *eopF)
{
   eop = eopF;
}

void BodyFixedAxes::SetCoefficientsFile(ItrfCoefficientsFile *itrfF)
{
   itrf = itrfF;
}

//------------------------------------------------------------------------------
//  void BodyFixedAxes::Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for this BodyFixedAxes.
 *
 */
//------------------------------------------------------------------------------
void BodyFixedAxes::Initialize()
{
   DynamicAxes::Initialize();
   //if ((eopFileName == "") || (eop == NULL))
   if (eop == NULL)
      throw CoordinateSystemException(
            "EOP file has not been set for " + instanceName);
   //if ((itrfFileName == "") || (itrf == NULL))
   if (itrf == NULL)
      throw CoordinateSystemException(
            "Coefficient file has not been set for " + instanceName);
   Integer numNut = itrf->GetNumberOfNutationTerms();
   A.SetSize(numNut);
   B.SetSize(numNut);
   C.SetSize(numNut);
   D.SetSize(numNut);
   E.SetSize(numNut);
   F.SetSize(numNut);
   
   Integer numPlan = itrf->GetNumberOfPlanetaryTerms();
   Ap.SetSize(numPlan);
   Bp.SetSize(numPlan);
   Cp.SetSize(numPlan);
   Dp.SetSize(numPlan);
   
   //pm = eop->GetPolarMotionData();
   bool OK = itrf->GetNutationTerms(a, A, B, C, D, E, F);
   if (!OK) throw CoordinateSystemException("Error getting nutation data.");
   
   
   
   // how to check whether or not epoch has been set?
   
}

//------------------------------------------------------------------------------
//  bool RotateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                        Rvector &outState)
//------------------------------------------------------------------------------
/**
 * This method will rotate the input inState into the MJ2000Eq frame.
 *
 * @param epoch     the epoch at which to perform the rotation.
 * @param inState   the input state (in this AxisSystem) to be rotated.
 * @param iutState  the output state, in the MJ2000Eq AxisSystem, the result 
 *                  of rotating the input inState.
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool BodyFixedAxes::RotateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
                              Rvector &outState)
{
   CalculateRotationMatrix(epoch);
   // *********** assuming only one 6-vector for now - UPDATE LATER!!!!!!
   Rvector3 tmpPos(inState[0],inState[1], inState[2]);
   Rvector3 tmpVel(inState[3],inState[4], inState[5]);
   Rvector3 outPos = rotMatrix    * tmpPos;
   Rvector3 outVel = rotDotMatrix * tmpPos + rotMatrix * tmpVel;
   outState[0] = outPos[0];
   outState[1] = outPos[1];
   outState[2] = outPos[2];
   outState[3] = outVel[0];
   outState[4] = outVel[1];
   outState[5] = outVel[2];
   return true;
}

//------------------------------------------------------------------------------
//  bool RotateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                          Rvector &outState)
//------------------------------------------------------------------------------
/**
 * This method will rotate the input inState from the MJ2000Eq frame into
 * this AxisSystem.
 *
 * @param epoch     the epoch at which to perform the rotation.
 * @param inState   the input state (in MJ2000Eq AxisSystem) to be rotated.
 * @param iutState  the output state, in this AxisSystem, the result 
 *                  of rotating the input inState.
 * @param j2000Body the origin of the input BodyFixedAxes frame.
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool BodyFixedAxes::RotateFromMJ2000Eq(const A1Mjd &epoch, 
                                       const Rvector &inState,
                                       Rvector &outState)
{
   CalculateRotationMatrix(epoch);
   // *********** assuming only one 6-vector for now - UPDATE LATER!!!!!!
   Rvector3 tmpPos(inState[0],inState[1], inState[2]);
   Rvector3 tmpVel(inState[3],inState[4], inState[5]);
   Rmatrix33 tmpRot    = rotMatrix.Transpose();
   Rmatrix33 tmpRotDot = rotDotMatrix.Transpose();
   Rvector3 outPos     = tmpRot    * tmpPos ;
   Rvector3 outVel     = tmpRotDot * tmpPos + tmpRot * tmpVel;
   outState[0] = outPos[0];
   outState[1] = outPos[1];
   outState[2] = outPos[2];
   outState[3] = outVel[0];
   outState[4] = outVel[1];
   outState[5] = outVel[2];
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
std::string BodyFixedAxes::GetParameterText(const Integer id) const
{
   if (id >= DynamicAxesParamCount && id < BodyFixedAxesParamCount)
      return PARAMETER_TEXT[id - DynamicAxesParamCount];
   return DynamicAxes::GetParameterText(id);
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
Integer BodyFixedAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = DynamicAxesParamCount; i < BodyFixedAxesParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - DynamicAxesParamCount])
         return i;
   }
   
   return DynamicAxes::GetParameterID(str);
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
Gmat::ParameterType BodyFixedAxes::GetParameterType(const Integer id) const
{
   if (id >= DynamicAxesParamCount && id < BodyFixedAxesParamCount)
      return PARAMETER_TYPE[id - DynamicAxesParamCount];
   
   return DynamicAxes::GetParameterType(id);
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
std::string BodyFixedAxes::GetParameterTypeString(const Integer id) const
{
   return DynamicAxes::PARAM_TYPE_STRING[GetParameterType(id)];
}

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
std::string BodyFixedAxes::GetStringParameter(const Integer id) const
{
   if (id == EOP_FILENAME)         return eopFileName; 
   else if (id == COEFF_FILENAME)  return itrfFileName;
   return DynamicAxes::GetStringParameter(id);
}

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
bool BodyFixedAxes::SetStringParameter(const Integer id, 
                                       const std::string &value)
{
   if (id == EOP_FILENAME) 
   {
      if (eopFileName != value)
      {
         eopFileName    = value; 
         return true;
      }
   }
   if (id == COEFF_FILENAME) 
   {
      if (itrfFileName != value)
      {
         itrfFileName    = value; 
         return true;
      }
   }
   
   return DynamicAxes::SetStringParameter(id, value);
}


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
std::string BodyFixedAxes::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

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

bool BodyFixedAxes::SetStringParameter(const std::string &label,
                                        const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000Eq system.mount_nfs mabhp1.gsfc.nasa.gov:/GMAT /GMAT
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
   //cout << "OriginName = " << originName << endl;
   // compute rotMatrix and rotDotMatrix
   if (originName == SolarSystem::EARTH_NAME)
   {
      // Convert to MJD UTC to use for polar motion  and LOD 
      // interpolations
      Real mjdUTC = TimeConverterUtil::Convert(atEpoch.Get(),
                    "A1Mjd", "UtcMjd", JD_JAN_5_1941);
      // convert to MJD referenced from time used in EOP file
      mjdUTC = mjdUTC + JD_JAN_5_1941 - JD_NOV_17_1858;

      //cout << "UTC (MJD) is " << mjdUTC<< endl;

      // convert input time to UT1 for later use (for AST calculation)
      Real mjdUT1 = TimeConverterUtil::Convert(atEpoch.Get(),
                    "A1Mjd", "Ut1Mjd", JD_JAN_5_1941);
      Real jdUT1    = mjdUT1 + JD_JAN_5_1941; // right?
      // Compute elapsed Julian centuries (UT1)
      Real tUT1     = (jdUT1 - 2451545.0) / 36525.0;
      Real tUT12    = tUT1  * tUT1;
      Real tUT13    = tUT12 * tUT1;

      //cout << "UT1 (MJD) is " << mjdUT1<< endl;
      //cout << "UT1 (JD) is " << jdUT1<< endl;

      
      // convert input A1 MJD to TT MJD (for most calculations)
      Real mjdTT = TimeConverterUtil::Convert(atEpoch.Get(),
                   "A1Mjd", "TtMjd", JD_JAN_5_1941);      
      Real jdTT    = mjdTT + JD_JAN_5_1941; // right? 
      // Compute Julian centuries of TDB from the base epoch (J2000) 
      // NOTE - this is rally TT, an approximation of TDB ****************
      Real tTDB    = (jdTT - 2451545.0) / 36525.0;
      Real tTDB2   = tTDB  * tTDB;
      Real tTDB3   = tTDB2 * tTDB;
      Real tTDB4   = tTDB3 * tTDB;

      //cout << "TT (MJD) is " << mjdTT<< endl;
      //cout << "TT (JD) is " << jdTT<< endl;
      
      // Compute precession (Vallado, Eq. 3-56)
      Real zeta  = ( 2306.2181*tTDB + 0.30188*tTDB2 + 0.017998*tTDB3 )
                   *RAD_PER_ARCSEC;
      Real Theta = ( 2004.3109*tTDB - 0.42665*tTDB2 - 0.041833*tTDB3 )
                   *RAD_PER_ARCSEC;
      Real     z = ( 2306.2181*tTDB + 1.09468*tTDB2 + 0.018203*tTDB3 )
                   *RAD_PER_ARCSEC;
      //cout << "zeta is " << zeta<< endl;
      //cout << "Theta is " << Theta<< endl;
      //cout << "z is " << z<< endl;
      
      // Compute trigonometric quantities
      Real cosTheta = Cos(Theta);
      Real cosz     = Cos(z);
      Real coszeta  = Cos(zeta);
      Real sinTheta = Sin(Theta);
      Real sinz     = Sin(z);
      Real sinzeta  = Sin(zeta);
      
      // Compute Rotation matrix for transformations from FK5 to MOD
      // (Vallado Eq. 3-57)
      Rmatrix33  PREC;
      PREC(0,0) =  cosTheta*cosz*coszeta - sinz*sinzeta;
      PREC(0,1) = -sinzeta*cosTheta*cosz - sinz*coszeta;
      PREC(0,2) = -sinTheta*cosz;
      PREC(1,0) =  sinz*cosTheta*coszeta + sinzeta*cosz;
      PREC(1,1) = -sinz*sinzeta*cosTheta + cosz*coszeta;
      PREC(1,2) = -sinTheta*sinz;
      PREC(2,0) =  sinTheta*coszeta;
      PREC(2,1) = -sinTheta*sinzeta;
      PREC(2,2) =  cosTheta;
      
      //cout << "PREC has been computed................." << endl;
      //cout << "PREC = " << PREC << endl;
      
      // Compute nutation - NOTE: this algorithm is bsased on the IERS 1996
      // Theory of Precession and Nutation. 
      Real dPsi = 0.0;
      Real dEps = 0.0;
      // First, compute useful angles (Vallado Eq. 3-54)
      // NOTE - taken from Steve Queen's code - he has apparently converted
      // the values in degrees (from Vallado Eq. 3-54) to arcsec before
      // performing these computations
      Real meanAnomalyMoon   = 134.96340251*RAD_PER_DEG + (1717915923.2178*tTDB 
           + 31.8792*tTDB2 + 0.051635*tTDB3 - 0.00024470*tTDB4)*RAD_PER_ARCSEC;
      Real meanAnomalySun    = 357.52910918*RAD_PER_DEG + ( 129596581.0481*tTDB 
           +  0.5532*tTDB2 + 0.000136*tTDB3 - 0.00001149*tTDB4)*RAD_PER_ARCSEC;
      Real argLatitudeMoon   =  93.27209062*RAD_PER_DEG + (1739527262.8478*tTDB 
           - 12.7512*tTDB2 - 0.001037*tTDB3 + 0.00000417*tTDB4)*RAD_PER_ARCSEC;
      Real meanElongationSun = 297.85019547*RAD_PER_DEG + (1602961601.2090*tTDB 
           -  6.3706*tTDB2 + 0.006593*tTDB3 - 0.00003169*tTDB4)*RAD_PER_ARCSEC;
      Real longAscNodeLunar  = 125.04455501*RAD_PER_DEG + (  -6962890.2665*tTDB 
           +  7.4722*tTDB2 + 0.007702*tTDB3 - 0.00005939*tTDB4)*RAD_PER_ARCSEC;
      //cout << "meanAnomalyMoon = " << meanAnomalyMoon << endl;
      //cout << "meanAnomalySun = " << meanAnomalySun << endl;
      //cout << "argLatitudeMoon = " << argLatitudeMoon << endl;
      //cout << "meanElongationSun = " << meanElongationSun << endl;
      //cout << "longAscNodeLunar = " << longAscNodeLunar << endl;
      // Now, sum using nutation coefficients  (Vallado Eq. 3-60)
      Integer i  = 0;
      Real apNut = 0.0;
      Real cosAp = 0.0;
      Real sinAp = 0.0;
      Integer nut = itrf->GetNumberOfNutationTerms();
      //cout << "nut = " << nut << endl;
      for (i = nut-1; i >= 0; i--)
      {
         apNut = (a.at(0)).at(i)*meanAnomalyMoon + (a.at(1)).at(i)*meanAnomalySun 
         + (a.at(2)).at(i)*argLatitudeMoon + (a.at(3)).at(i)*meanElongationSun 
         + (a.at(4)).at(i)*longAscNodeLunar;
         cosAp = Cos(apNut);
         sinAp = Sin(apNut);
         dPsi += (A[i] + B[i]*tTDB )*sinAp + E[i]*cosAp;
         dEps += (C[i] + D[i]*tTDB )*cosAp + F[i]*sinAp;
         if (i > (nut-4))
         {
            //cout << "i = " << i << " and apNut = " << apNut << endl;
            //cout << "i = " << i << " and dPsi = " << dPsi << endl;
            //cout << "i = " << i << " and dEps = " << dEps << endl;
         }
      }
      dPsi *= RAD_PER_ARCSEC;
      dEps *= RAD_PER_ARCSEC;
      //cout << "At end of loop, dPsi = " << dPsi << endl;
      //cout << "At end of loop, dEps = " << dEps << endl;
      
      // Compute the corrections for planetary effects on the nutation and
      // the obliquity of the ecliptic 
      // NOTE - this part is commented out for now, per Steve Hughes
      // First, compute the mean Heliocentric longitudes of the planets, and the
      // general precession in longitude
      /*
      Real longVenus   = (181.979800853  + 58517.8156748  * tTDB)* RAD_PER_DEG;
      Real longEarth   = (100.466448494  + 35999.3728521  * tTDB)* RAD_PER_DEG;
      Real longMars    = (355.433274605  + 19140.299314   * tTDB)* RAD_PER_DEG;
      Real longJupiter = ( 34.351483900  +  3034.90567464 * tTDB)* RAD_PER_DEG;
      Real longSaturn  = ( 50.0774713998 +  1222.11379404 * tTDB)* RAD_PER_DEG;
      Real genPrec     = (1.39697137214 * tTDB + 0.0003086 * tTDB2)
                         * RAD_PER_DEG;
      Real apPlan = 0.0;
      Real cosApP = 0.0;
      Real sinApP = 0.0;
      Integer nutpl = itrf->GetNumberOfPlanetaryTerms();
      for (i = nutpl-1; i >= 0; i--)
      {
          apPlan = (ap.at(0)).at(i)*longVenus + (ap.at(1)).at(i)*longEarth 
          + (ap.at(2)).at(i)*longMars   + (ap.at(3)).at(i)*longJupiter 
          + (ap.at(4)).at(i)*longSaturn + (ap.at(5)).at(i)*genPrec
          + (ap.at(6)).at(i)*meanElongationSun*
          + (ap.at(7)).at(i)*argLatitudeMoon
          + (ap.at(8)).at(i)*meanAnomalyMoon
          + (ap.at(9)).at(i)*longAscNodeLunar;
          cosApP = Cos(apPlan);
          sinApP = Sin(apPlan);
          dPsi += (( Ap[i] + Bp[i]*tTDB )*sinApP) * RAD_PER_ARCSEC;
          dEps += (( Cp[i] + Dp[i]*tTDB )*cosApP) * RAD_PER_ARCSEC;
      }
       */
      // FOR NOW, SQ's code to approximate GSRF frame
      // NOTE - do we delete this when we put in the planetary stuff above?
      // offset and rate correction to approximate GCRF, Ref.[1], Eq (3-63)  - SQ
      dPsi += (-0.0431 - 0.2957*tTDB )*RAD_PER_ARCSEC;
      dEps += (-0.0051 - 0.0277*tTDB )*RAD_PER_ARCSEC;
      //cout << "After approximation, dPsi = " << dPsi << endl;
      //cout << "After approximation, dEps = " << dEps << endl;
      
      // Compute obliquity of the ecliptic (Vallado Eq. 3-52)
      Real Epsbar  = (84381.448 - 46.8150*tTDB - 0.00059*tTDB2 + 0.001813*tTDB3)
                     *RAD_PER_ARCSEC;
      Real TrueOoE = Epsbar + dEps;
      //cout << "Epsbar = " << Epsbar << endl;
      //cout << "TrueOoE = " << TrueOoE << endl;
      
      // Compute useful trigonometric quantities
      Real cosdPsi   = Cos(dPsi);
      Real cosEpsbar = Cos(Epsbar);
      Real cosTEoE   = Cos(TrueOoE);
      Real sindPsi   = Sin(dPsi);
      Real sinEpsbar = Sin(Epsbar);
      Real sinTEoE   = Sin(TrueOoE);   
      
      // Compute Rotation matrix for transformations from MOD to TOD
      // (Vallado Eq. 3-64)
      Rmatrix33  NUT;
      NUT(0,0) =  cosdPsi;
      NUT(0,1) = -sindPsi*cosEpsbar;
      NUT(0,2) = -sindPsi*sinEpsbar;
      NUT(1,0) =  sindPsi*cosTEoE;
      NUT(1,1) =  cosTEoE*cosdPsi*cosEpsbar + sinTEoE*sinEpsbar;
      NUT(1,2) =  sinEpsbar*cosTEoE*cosdPsi - sinTEoE*cosEpsbar;
      NUT(2,0) =  sinTEoE*sindPsi;
      NUT(2,1) =  sinTEoE*cosdPsi*cosEpsbar - sinEpsbar*cosTEoE;
      NUT(2,2) =  sinTEoE*sinEpsbar*cosdPsi + cosTEoE*cosEpsbar;
      
      //cout << "NUT has been computed................." << endl;
      //cout << "NUT = " << NUT << endl;
      // Compute Sidereal Time
      
      // First, compute the equation of the equinoxes
      // If the time is before January 1, 1997, don't use the last two terms
      Real term2 = 0.0;
      Real term3 = 0.0;
      if (jdTT > JD_OF_JANUARY_1_1997)
      {
         term2 = (0.00264 * Sin(longAscNodeLunar))        * RAD_PER_ARCSEC;
         term3 = (0.000063 * Sin(2.0 * longAscNodeLunar)) * RAD_PER_ARCSEC;
      }
      //Real EQequinox = (dPsi * cosTEoE) + term2 + term3; 
      Real EQequinox = (dPsi * cosEpsbar) + term2 + term3;
      //cout << "EQequinox = " << EQequinox << endl;
      
      // Compute Greenwich Apparent Sidereal Time from the Greenwich Mean 
      // Sidereal Time and the Equation of the equinoxes
      // (Vallado Eq. 3-45)
      // NOTE: 1 sec = 15"; 1 hour (= 15 deg) = 54000"
      Real sec2deg = 15.0 / 3600;
      Real hour2deg = 15.0;
      //Real ThetaGmst = (1.00965822615e6 + 4.746600277219299e10*tUT1 
      //                  + 1.396560*tUT12 + 9.3e-5*tUT13 )*RAD_PER_ARCSEC;
      Real ThetaGmst = ((67310.54841 * sec2deg) + 
                       ((876600 * hour2deg) + (8640184.812866 * sec2deg))*tUT1 +
                       (0.093104 * sec2deg)*tUT12 - (6.2e-06 * sec2deg)*tUT13 )
                       *RAD_PER_DEG;
      
      Real ThetaAst = ThetaGmst + EQequinox;
      //cout << "ThetaGmst = " << ThetaGmst << endl;
      //cout << "ThetaAst = " << ThetaAst << endl;
      
      //Compute useful trignonometric quantities
      Real cosAst = Cos(ThetaAst);
      Real sinAst = Sin(ThetaAst);
      
      // Compute Rotation matrix for Sidereal Time
      // (Vallado Eq. 3-64)
      Rmatrix33  ST;
      ST(0,0) =  cosAst;
      ST(0,1) =  sinAst;
      ST(0,2) =  0.0;
      ST(1,0) = -sinAst;
      ST(1,1) =  cosAst;
      ST(1,2) =  0.0;
      ST(2,0) =  0.0;
      ST(2,1) =  0.0;
      ST(2,2) =  1.0;

      //cout << "ST has been computed................." << endl;
      //cout << "ST = " << ST << endl;

      // Get the polar motion and lod data
      Real x   = 0.0;
      Real y   = 0.0;
      Real lod = 0.0;
      eop->GetPolarMotionAndLod(mjdUTC,x,y,lod);
      //cout << "Polar motion has been interpolated and the values of x, y, and lod are"
      //   << x << " " << y << " " << lod << endl;
      
      // Compute the portion that has a significant time derivative
      Real omegaE = 7.29211514670698e-05 * (1.0 - (lod / SECS_PER_DAY));
      //cout << "omegaE has been computed -------------- and = " << omegaE << endl;
      Rmatrix33 STderiv;
      STderiv(0,0) = -omegaE * sinAst;
      STderiv(0,1) =  omegaE * cosAst;
      STderiv(0,2) =  0.0;
      STderiv(1,0) = -omegaE * cosAst;
      STderiv(1,1) = -omegaE * sinAst;
      STderiv(1,2) =  0.0;
      STderiv(2,0) =  0.0;
      STderiv(2,1) =  0.0;
      STderiv(2,2) =  0.0;
      
      //cout << "STderiv has been computed................." << endl;
      //cout << "STderiv = " << STderiv << endl;

      // Compute useful trigonometric quantities
      Real cosX = Cos(-x * RAD_PER_ARCSEC);
      Real sinX = Sin(-x * RAD_PER_ARCSEC);
      Real cosY = Cos(-y * RAD_PER_ARCSEC);
      Real sinY = Sin(-y * RAD_PER_ARCSEC);

      // Compute the polar motion rotation matrix
      Rmatrix33 PM;
      PM(0,0) =  cosX;
      PM(0,1) =  sinX*sinY;
      PM(0,2) = -sinX*cosY;
      PM(1,0) =  0.0;
      PM(1,1) =  cosY;
      PM(1,2) =  sinY;
      PM(2,0) =  sinX;
      PM(2,1) = -cosX*sinY;
      PM(2,2) =  cosX*cosY;

      //cout << "PM has been computed................." << endl;
      //cout << "PM = " << PM << endl;

      
      //Rmatrix33 rotMatrixT    = (PREC * (NUT * (ST * PM)));
      //rotMatrix = rotMatrixT.Transpose();
      rotMatrix    = PREC.Transpose() * (NUT.Transpose() * 
                     (ST.Transpose() * PM.Transpose()));
      //cout << "rotMatrix computed ..........." << endl;
      //cout << "rotMatrix = " << rotMatrix << endl;

      //Rmatrix33 rotDotMatrixT = (PREC * (NUT * (STderiv * PM)));
      //rotDotMatrix = rotDotMatrixT.Transpose();
      rotDotMatrix    = PREC.Transpose() * (NUT.Transpose() * 
                        (STderiv.Transpose() * PM.Transpose()));
      //cout << "rotDotMatrix computed ..........." << endl;
      //cout << "rotDotMatrix = " << rotDotMatrix << endl;
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


