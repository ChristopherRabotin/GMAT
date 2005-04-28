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
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include "RealTypes.hpp"
#include "TimeTypes.hpp"
#include "Rvector3.hpp"
#include "CoordinateSystemException.hpp"
#include "SolarSystem.hpp"

#include <iostream>
using namespace std; //***************************** for debug only 

using namespace GmatMathUtil;      // for trig functions, etc.
using namespace GmatTimeUtil;      // for SECS_PER_DAY

//#define DEBUG_ROT_MATRIX 1
//static Integer visitCount = 0;

//---------------------------------
// static data
//---------------------------------

const std::string
AxisSystem::PARAMETER_TEXT[AxisSystemParamCount - CoordinateBaseParamCount] =
{
   "Epoch",
};

const Gmat::ParameterType
AxisSystem::PARAMETER_TYPE[AxisSystemParamCount - CoordinateBaseParamCount] =
{
   Gmat::REAL_TYPE,
};


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
CoordinateBase(Gmat::AXIS_SYSTEM,itsType,itsName),
eop              (NULL),
itrf             (NULL)
{
   objectTypes.push_back(Gmat::AXIS_SYSTEM);
   objectTypeNames.push_back("AxisSystem");
   parameterCount = AxisSystemParamCount;
   rotMatrix    = Rmatrix33(false); // want zero matrix, not identity matrix
   rotDotMatrix = Rmatrix33(false); // want zero matrix, not identity matrix
   epoch = GmatTimeUtil::A1MJD_OF_J2000;
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
rotDotMatrix      (axisSys.rotDotMatrix),
epoch             (axisSys.epoch),
eop               (axisSys.eop),
itrf              (axisSys.itrf)
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
   epoch         = axisSys.epoch;
   eop           = axisSys.eop;
   itrf          = axisSys.itrf;
   Initialize();
   
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

GmatCoordinate::ParameterUsage AxisSystem::UsesEopFile() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage AxisSystem::UsesItrfFile() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage AxisSystem::UsesEpoch() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage AxisSystem::UsesPrimary() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage AxisSystem::UsesSecondary() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage AxisSystem::UsesXAxis() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage AxisSystem::UsesYAxis() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage AxisSystem::UsesZAxis() const
{
   return GmatCoordinate::NOT_USED;
}

// methods to set parameters for the AxisSystems - AxisSystems that need these
// will need to override these implementations
void AxisSystem::SetPrimaryObject(SpacePoint *prim)
{
   // default behavior is to ignore this - should I throw an exception here??
}

void AxisSystem::SetSecondaryObject(SpacePoint *second)
{
   // default behavior is to ignore this
}


//---------------------------------------------------------------------------
//  void SetEpoch(const A1Mjd &toEpoch)
//---------------------------------------------------------------------------
/**
 * Sets the epoch for the AxisSystem class.
 *
 * @param <toEpoch> epoch value to use.
 *
 * @return true if successful; false, otherwise.
 *
 */
//---------------------------------------------------------------------------
void AxisSystem::SetEpoch(const A1Mjd &toEpoch)
{
   epoch = toEpoch;
}

void AxisSystem::SetXAxis(const std::string &toValue)
{
   // default behavior is to ignore this
}

void AxisSystem::SetYAxis(const std::string &toValue)
{
   // default behavior is to ignore this
}

void AxisSystem::SetZAxis(const std::string &toValue)
{
   // default behavior is to ignore this
}

//------------------------------------------------------------------------------
//  void SetEopFile(EopFile *eopF)
//------------------------------------------------------------------------------
/**
 * Method to set EopFile for this system.
 *
 * @param eopF  the eopFile.
 *
 */
//------------------------------------------------------------------------------
void AxisSystem::SetEopFile(EopFile *eopF)
{
   eop = eopF;
}

//------------------------------------------------------------------------------
//  void SetCoefficientsFile(ItrfCoefficientsFile *itrfF)
//------------------------------------------------------------------------------
/**
 * Method to set ITRF coefficients for this system.
 *
 * @param itrfF  the ITRF coefficients file.
 *
 */
//------------------------------------------------------------------------------
void AxisSystem::SetCoefficientsFile(ItrfCoefficientsFile *itrfF)
{
   itrf = itrfF;
}

SpacePoint* AxisSystem::GetPrimaryObject() const
{
   return NULL;
}

SpacePoint* AxisSystem::GetSecondaryObject() const
{
   return NULL;
}

//---------------------------------------------------------------------------
//  A1Mjd GetEpoch() const
//---------------------------------------------------------------------------
/**
 * Returns the epoch of the AxisSystem class.
 *
 * @return epoch.
 *
 */
//---------------------------------------------------------------------------
A1Mjd AxisSystem::GetEpoch() const
{
   return epoch;
}


std::string AxisSystem::GetXAxis() const
{
   return "";
}

std::string AxisSystem::GetYAxis() const
{
   return "";
}

std::string AxisSystem::GetZAxis() const
{
   return "";
}

EopFile* AxisSystem::GetEopFile() const
{
   return eop;
}

ItrfCoefficientsFile* AxisSystem::GetItrfCoefficientsFile()
{
   return itrf;
}


//---------------------------------------------------------------------------
//  void AxisSystem::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this AxisSystem.
 *
 */
//---------------------------------------------------------------------------
bool AxisSystem::Initialize()
{
   CoordinateBase::Initialize();
   return true;
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
bool AxisSystem::RotateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
                                   Rvector &outState)
{
   CalculateRotationMatrix(epoch);
   // *********** assuming only one 6-vector for now - UPDATE LATER!!!!!!
   Rvector3 tmpPos(inState[0],inState[1], inState[2]);
   Rvector3 tmpVel(inState[3],inState[4], inState[5]);
   Rvector3 outPos = rotMatrix    * tmpPos;
   Rvector3 outVel = (rotDotMatrix * tmpPos) + (rotMatrix * tmpVel);
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
 * @param j2000Body the origin of the input EquatorAxes frame.
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::RotateFromMJ2000Eq(const A1Mjd &epoch, 
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
   Rvector3 outVel     = (tmpRotDot * tmpPos) + (tmpRot * tmpVel);
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
std::string AxisSystem::GetParameterText(const Integer id) const
{
   if (id >= CoordinateBaseParamCount && id < AxisSystemParamCount)
      return PARAMETER_TEXT[id - CoordinateBaseParamCount];
   return CoordinateBase::GetParameterText(id);
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
Integer AxisSystem::GetParameterID(const std::string &str) const
{
   for (Integer i = CoordinateBaseParamCount; i < AxisSystemParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - CoordinateBaseParamCount])
         return i;
   }
   
   return CoordinateBase::GetParameterID(str);
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
Gmat::ParameterType AxisSystem::GetParameterType(const Integer id) const
{
   if (id >= CoordinateBaseParamCount && id < AxisSystemParamCount)
      return PARAMETER_TYPE[id - CoordinateBaseParamCount];
   
   return CoordinateBase::GetParameterType(id);
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
std::string AxisSystem::GetParameterTypeString(const Integer id) const
{
   return CoordinateBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
Real AxisSystem::GetRealParameter(const Integer id) const
{
   if (id == EPOCH) return epoch.Get();  // modify later????????????
   return CoordinateBase::GetRealParameter(id);
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
Real AxisSystem::SetRealParameter(const Integer id, const Real value)
{
   if (id == EPOCH)
   {
      epoch.Set(value);
      return true;
   }
   return CoordinateBase::SetRealParameter(id,value);
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
Real AxisSystem::GetRealParameter(const std::string &label) const
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
Real AxisSystem::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


void AxisSystem::InitializeFK5()
{
   //if (originName == SolarSystem::EARTH_NAME)
   //{
      if (eop == NULL)
         throw CoordinateSystemException(
               "EOP file has not been set for " + instanceName);
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
      
      bool OK = itrf->GetNutationTerms(a, A, B, C, D, E, F);
      if (!OK) throw CoordinateSystemException("Error getting nutation data.");
      OK      = itrf->GetPlanetaryTerms(ap, Ap, Bp, Cp, Dp);
      if (!OK) throw CoordinateSystemException("Error getting planetary data.");
   //}
}   

//------------------------------------------------------------------------------
//  void ComputePrecessionMatrix(const A1Mjd &forEpoch)
//------------------------------------------------------------------------------
/**
 * This method will compute the Precession rotation matrix.
 *
 * @param forEpoch  epoch at which to compute the rotation matrix
 *
 * @note Code (for Earth) adapted from C code written by Steve Queen/ GSFC, 
 *       based on Vallado, pgs. 211- 227.  Equation references in parentheses
 *       will refer to those of the Vallado book.
 */
//------------------------------------------------------------------------------
Rmatrix33 AxisSystem::ComputePrecessionMatrix(const Real tTDB)
{
   
   #ifdef DEBUG_ROT_MATRIX
      cout << "**** tTDB = " << tTDB << endl;
   #endif
   Real tTDB2   = tTDB  * tTDB;
   Real tTDB3   = tTDB2 * tTDB;

   // Compute precession (Vallado, Eq. 3-56)
   Real zeta  = ( 2306.2181*tTDB + 0.30188*tTDB2 + 0.017998*tTDB3 )
      *RAD_PER_ARCSEC;
   Real Theta = ( 2004.3109*tTDB - 0.42665*tTDB2 - 0.041833*tTDB3 )
      *RAD_PER_ARCSEC;
   Real     z = ( 2306.2181*tTDB + 1.09468*tTDB2 + 0.018203*tTDB3 )
      *RAD_PER_ARCSEC;
   
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
   #ifdef DEBUG_ROT_MATRIX
      cout << "PREC = " << endl << PREC << endl;
   #endif
   return PREC;
}

Rmatrix33 AxisSystem::ComputeNutationMatrix(const Real tTDB, Real &dPsi,
                                            Real &longAscNodeLunar,
                                            Real &cosEpsbar)
{
   Real tTDB2   = tTDB  * tTDB;
   Real tTDB3   = tTDB2 * tTDB;
   Real tTDB4   = tTDB3 * tTDB;
   // Compute nutation - NOTE: this algorithm is bsased on the IERS 1996
   // Theory of Precession and Nutation. 
   dPsi      = 0.0;
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
   longAscNodeLunar       = 125.04455501*RAD_PER_DEG + (  -6962890.2665*tTDB 
        +  7.4722*tTDB2 + 0.007702*tTDB3 - 0.00005939*tTDB4)*RAD_PER_ARCSEC;
   
   // Now, sum using nutation coefficients  (Vallado Eq. 3-60)
   Integer i  = 0;
   Real apNut = 0.0;
   Real cosAp = 0.0;
   Real sinAp = 0.0;
   Integer nut = itrf->GetNumberOfNutationTerms();
   for (i = nut-1; i >= 0; i--)
   {
      apNut = (a.at(0)).at(i)*meanAnomalyMoon + (a.at(1)).at(i)*meanAnomalySun 
      + (a.at(2)).at(i)*argLatitudeMoon + (a.at(3)).at(i)*meanElongationSun 
      + (a.at(4)).at(i)*longAscNodeLunar;
      cosAp = Cos(apNut);
      sinAp = Sin(apNut);
      dPsi += (A[i] + B[i]*tTDB )*sinAp + E[i]*cosAp;
      dEps += (C[i] + D[i]*tTDB )*cosAp + F[i]*sinAp;
   }
   dPsi *= RAD_PER_ARCSEC;
   dEps *= RAD_PER_ARCSEC;
   
   // Compute the corrections for planetary effects on the nutation and
   // the obliquity of the ecliptic 
   // NOTE - this part is commented out for now, per Steve Hughes
   // First, compute the mean Heliocentric longitudes of the planets, and the
   // general precession in longitude
   
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
    
   // FOR NOW, SQ's code to approximate GSRF frame
   // NOTE - do we delete this when we put in the planetary stuff above?
   // offset and rate correction to approximate GCRF, Ref.[1], Eq (3-63)  - SQ
   dPsi += (-0.0431 - 0.2957*tTDB )*RAD_PER_ARCSEC;
   dEps += (-0.0051 - 0.0277*tTDB )*RAD_PER_ARCSEC;
   
   // Compute obliquity of the ecliptic (Vallado Eq. 3-52)
   Real Epsbar  = (84381.448 - 46.8150*tTDB - 0.00059*tTDB2 + 0.001813*tTDB3)
      *RAD_PER_ARCSEC;
   Real TrueOoE = Epsbar + dEps;
   
   // Compute useful trigonometric quantities
   Real cosdPsi   = Cos(dPsi);
   cosEpsbar      = Cos(Epsbar);
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
   
   #ifdef DEBUG_ROT_MATRIX
      cout << "NUT = " << endl << NUT << endl;
   #endif
   return NUT;
}

Rmatrix33 AxisSystem::ComputeSiderealTimeRotation(const Real jdTT,
                                                  const Real tUT1,
                                                  Real dPsi,
                                                  Real longAscNodeLunar,
                                                  Real cosEpsbar,
                                                  Real &cosAst,
                                                  Real &sinAst)
{
   Real tUT12    = tUT1  * tUT1;
   Real tUT13    = tUT12 * tUT1;
   
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
                     * RAD_PER_DEG;
   
   Real ThetaAst = ThetaGmst + EQequinox;
   
   //Compute useful trignonometric quantities
   cosAst = Cos(ThetaAst);
   sinAst = Sin(ThetaAst);
   
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
   
   #ifdef DEBUG_ROT_MATRIX
      cout << "ST = " << endl << ST << endl;
   #endif

   return ST;
}

Rmatrix33 AxisSystem::ComputeSiderealTimeDotRotation(const Real mjdUTC,
                                                     Real cosAst, Real sinAst,
                                                     Real &x, Real &y)
{
   // Convert to MJD UTC to use for polar motion  and LOD 
   // interpolations
   // Get the polar motion and lod data
   Real lod = 0.0;
   eop->GetPolarMotionAndLod(mjdUTC,x,y,lod);
   
   // Compute the portion that has a significant time derivative
   Real omegaE = 7.29211514670698e-05 * (1.0 - (lod / SECS_PER_DAY));
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
   
   #ifdef DEBUG_ROT_MATRIX
      cout << "x, y, lod = " << x << " " << y << " " << lod << endl;
      cout << "STderiv = " << endl << STderiv << endl;
   #endif
   
   return STderiv;
}

Rmatrix33 AxisSystem::ComputePolarMotionRotation(Real x, Real y)
{
   
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

   #ifdef DEBUG_ROT_MATRIX
      cout << "PM = " << endl << PM << endl;
   #endif

   return PM;
}


