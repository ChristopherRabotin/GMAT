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
#include "AngleUtil.hpp"
#include "Linear.hpp"
#include "RealTypes.hpp"
#include "TimeTypes.hpp"
#include "Rvector3.hpp"
#include "MessageInterface.hpp"
#include "CoordinateSystemException.hpp"
#include "SolarSystem.hpp"

#include <iostream>
using namespace std; //***************************** for debug only 

using namespace GmatMathUtil;      // for trig functions, etc.
using namespace GmatTimeUtil;      // for SECS_PER_DAY

//#define DEBUG_ROT_MATRIX 1
//static Integer visitCount = 0;

//#define DEBUG_UPDATE
//#define DEBUG_FIRST_CALL

//#define DEBUG_ITRF_UPDATES
//#define DEBUG_CALCS
//#define DEBUG_DESTRUCTION


#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
#endif

//---------------------------------
// static data
//---------------------------------

const Real AxisSystem::DETERMINANT_TOLERANCE = 1.0e-14;
const Real AxisSystem::JD_OF_JANUARY_1_1997  = 2450449.5;  // correct????


const std::string
AxisSystem::PARAMETER_TEXT[AxisSystemParamCount - CoordinateBaseParamCount] =
{
   "Epoch",
   "UpdateInterval",
   "OverrideOriginInterval",
};

const Gmat::ParameterType
AxisSystem::PARAMETER_TYPE[AxisSystemParamCount - CoordinateBaseParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::BOOLEAN_TYPE,
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
itrf             (NULL),
epochFormat      ("A1ModJulian"),
updateInterval   (60.0), 
updateIntervalToUse    (60.0), 
overrideOriginInterval (false),
lastDPsi         (0.0),
nutationSrc      (GmatItrf::NUTATION_1980),
planetarySrc     (GmatItrf::PLANETARY_1980),
aVals            (NULL), 
apVals           (NULL),
precData         (NULL),
nutData          (NULL),
stData           (NULL),
stDerivData      (NULL),
pmData           (NULL),
AVals            (NULL),
BVals            (NULL),
CVals            (NULL),
DVals            (NULL),
EVals            (NULL),
FVals            (NULL),
ApVals           (NULL),
BpVals           (NULL),
CpVals           (NULL),
DpVals           (NULL)
{
   objectTypes.push_back(Gmat::AXIS_SYSTEM);
   objectTypeNames.push_back("AxisSystem");
   parameterCount = AxisSystemParamCount;
   rotMatrix    = Rmatrix33(false); // want zero matrix, not identity matrix
   rotDotMatrix = Rmatrix33(false); // want zero matrix, not identity matrix
   epoch        = GmatTimeUtil::A1MJD_OF_J2000;
   lastPRECEpoch       = A1Mjd(0.0); // 1941?  strange, but ...
   lastNUTEpoch        = A1Mjd(0.0); // 1941?  strange, but ...
   lastSTDerivEpoch    = A1Mjd(0.0); // 1941?  strange, but ...
   lastPMEpoch         = A1Mjd(0.0); // 1941?  strange, but ...
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
itrf              (axisSys.itrf),
epochFormat       (axisSys.epochFormat),
updateInterval    (axisSys.updateInterval),
updateIntervalToUse    (axisSys.updateIntervalToUse),
overrideOriginInterval (axisSys.overrideOriginInterval),
lastDPsi          (0.0),
nutationSrc       (GmatItrf::NUTATION_1980),
planetarySrc      (GmatItrf::PLANETARY_1980),
aVals            (NULL), 
apVals           (NULL),
precData         (NULL),
nutData          (NULL),
stData           (NULL),
stDerivData      (NULL),
pmData           (NULL),
AVals            (NULL),
BVals            (NULL),
CVals            (NULL),
DVals            (NULL),
EVals            (NULL),
FVals            (NULL),
ApVals           (NULL),
BpVals           (NULL),
CpVals           (NULL),
DpVals           (NULL)
{
   lastPRECEpoch    = A1Mjd(0.0);
   lastNUTEpoch     = A1Mjd(0.0);
   lastSTDerivEpoch = A1Mjd(0.0);
   lastPMEpoch      = A1Mjd(0.0);
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
   rotMatrix      = axisSys.rotMatrix;
   rotDotMatrix   = axisSys.rotDotMatrix;
   epoch          = axisSys.epoch;
   eop            = axisSys.eop;
   itrf           = axisSys.itrf;
   epochFormat    = axisSys.epochFormat;
   updateInterval = axisSys.updateInterval;
   updateIntervalToUse = axisSys.updateIntervalToUse;
   overrideOriginInterval = axisSys.overrideOriginInterval;
   lastPRECEpoch     = axisSys.lastPRECEpoch;
   lastNUTEpoch      = axisSys.lastNUTEpoch;   
   lastSTDerivEpoch  = axisSys.lastSTDerivEpoch;
   lastPMEpoch       = axisSys.lastPMEpoch;
   lastPREC          = axisSys.lastPREC;
   lastNUT           = axisSys.lastNUT;
   lastSTDeriv       = axisSys.lastSTDeriv;
   lastPM            = axisSys.lastPM;
   lastDPsi          = axisSys.lastDPsi;
   nutationSrc       = axisSys.nutationSrc;
   planetarySrc      = axisSys.planetarySrc;
   Initialize();
   
   return *this;
}
//---------------------------------------------------------------------------
//  ~AxisSystem()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
AxisSystem::~AxisSystem()
{
//    for (Integer i = 0; i < 5; i++)
//       delete aVals[i];
//    for (Integer i = 0; i < 10; i++)
//       delete apVals[i];
   #ifdef DEBUG_DESTRUCTION
   MessageInterface::ShowMessage("---> Entering AxisSystem destructor for %s\n",
   instanceName.c_str());
   #endif

   if (aVals != NULL)
      delete [] aVals;
   if (apVals != NULL)
      delete [] apVals;
   if (AVals != NULL) delete [] AVals;
   if (BVals != NULL) delete [] BVals;
   if (CVals != NULL) delete [] CVals;
   if (DVals != NULL) delete [] DVals;
   if (EVals != NULL) delete [] EVals;
   if (FVals != NULL) delete [] FVals;
   if (ApVals != NULL) delete [] ApVals;
   if (BpVals != NULL) delete [] BpVals;
   if (CpVals != NULL) delete [] CpVals;
   if (DpVals != NULL) delete [] DpVals;
   if (precData != NULL)    delete [] precData;
   if (nutData != NULL)     delete [] nutData;
   if (stData != NULL)      delete [] stData;
   if (stDerivData != NULL) delete [] stDerivData;
   if (pmData != NULL)      delete [] pmData;
  
//   aVals = NULL;
//   apVals = NULL;
   #ifdef DEBUG_DESTRUCTION
   MessageInterface::ShowMessage("---> LEAVING AxisSystem destructor for %s\n",
   instanceName.c_str());
   #endif
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

GmatCoordinate::ParameterUsage AxisSystem::UsesNutationUpdateInterval() const
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

void AxisSystem::SetEpochFormat(const std::string &fmt)
{
   epochFormat = fmt;
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

std::string AxisSystem::GetEpochFormat() const
{
   return epochFormat;
}

Rmatrix33 AxisSystem::GetLastRotationMatrix() const
{
   return rotMatrix;
}

void AxisSystem::GetLastRotationMatrix(Real *mat) const
{
   for (Integer i=0; i<9; i++)
      mat[i] = rotData[i];
   
}

Rmatrix33 AxisSystem::GetLastRotationDotMatrix() const
{
   return rotDotMatrix;
}

void AxisSystem::GetLastRotationDotMatrix(Real *mat) const
{
   for (Integer i=0; i<9; i++)
      mat[i] = rotDotData[i];
   
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
   #ifdef DEBUG_FIRST_CALL
      firstCallFired = false;
   #endif
   
   rotData     = rotMatrix.GetDataVector();
   rotDotData  = rotDotMatrix.GetDataVector();
   precData    = PREC.GetDataVector();
   nutData     = NUT.GetDataVector();
   stData      = ST.GetDataVector();
   stDerivData = STderiv.GetDataVector();
   pmData      = PM.GetDataVector();
   
   
   return true;
}

//------------------------------------------------------------------------------
//  bool RotateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                        Rvector &outState, bool forceComputation)
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
                                  Rvector &outState, 
                                  bool forceComputation)
{
   static Rvector3 tmpPosVecTo;
   static Rvector3 tmpVelVecTo;
   static const Real  *tmpPosTo = tmpPosVecTo.GetDataVector();
   static const Real  *tmpVelTo = tmpVelVecTo.GetDataVector();
   CalculateRotationMatrix(epoch, forceComputation);
   
   // *********** assuming only one 6-vector for now - UPDATE LATER!!!!!!
   //Rvector3 tmpPosVec(inState[0],inState[1], inState[2]);
   //Rvector3 tmpVelVec(inState[3],inState[4], inState[5]);
   tmpPosVecTo.Set(inState[0],inState[1], inState[2]);
   tmpVelVecTo.Set(inState[3],inState[4], inState[5]);
   //const Real  *inData = inState.GetDataVector();
   //const Real  *tmpPosTo = tmpPosVecTo.GetDataVector();
   //const Real  *tmpVelTo = tmpVelVecTo.GetDataVector();
   // rotMatrix * tmpPos;
   // rotDotMatrix * tmpPos + rotMatrix * tmpVel
   
   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Input vector to ToMJ2000 = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);
      MessageInterface::ShowMessage(
      "the rotation matrix is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotMatrix(0,0),rotMatrix(0,1),rotMatrix(0,2),
      rotMatrix(1,0),rotMatrix(1,1),rotMatrix(1,2),
      rotMatrix(2,0),rotMatrix(2,1),rotMatrix(2,2));
      //MessageInterface::ShowMessage(
      //   "Input vector as datavec = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      //   tmpPosTo[0], tmpPosTo[1], tmpPosTo[2], tmpVelTo[0], tmpVelTo[1], tmpVelTo[2]);
   #endif
   Real  outPos[3];
   Real  outVel[3];
   Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      outPos[p] = rotData[p3]   * tmpPosTo[0]   + 
                  rotData[p3+1] * tmpPosTo[1] + 
                  rotData[p3+2] * tmpPosTo[2];
      outVel[p] = (rotDotData[p3]    * tmpPosTo[0]   + 
                   rotDotData[p3+1]  * tmpPosTo[1] + 
                   rotDotData[p3+2]  * tmpPosTo[2])
                  +
                  (rotData[p3]    * tmpVelTo[0]   + 
                   rotData[p3+1]  * tmpVelTo[1] + 
                   rotData[p3+2]  * tmpVelTo[2]);
   }     
 
   //Rvector3 outPos_2 = rotMatrix    * tmpPosVec; // old
   //Rvector3 outVel_2 = (rotDotMatrix * tmpPosVec) + (rotMatrix * tmpVelVec);
         
   outState.Set(6,outPos[0], outPos[1], outPos[2], 
                  outVel[0], outVel[1], outVel[2]);
   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Computed Output vector in ToMJ2000 = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         outPos[0], outPos[1], outPos[2], outVel[0], outVel[1], outVel[2]);
      MessageInterface::ShowMessage(
         "Output vector from ToMJ2000 = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         outState[0], outState[1], outState[2], outState[3], outState[4], outState[5]);
   #endif

   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == 21545.0))
      {
         MessageInterface::ShowMessage(
            "RotateToMJ2000Eq check for %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   Rotation matrix = |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n",
            rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
            rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
            rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.Get());
         MessageInterface::ShowMessage(
            "   input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
         MessageInterface::ShowMessage(
            "   outpt State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            outPos[0], outPos[1], outPos[2], outVel[0], outVel[1], 
            outVel[2]);
      }
   #endif

   return true;
}

bool AxisSystem::RotateToMJ2000Eq(const A1Mjd &epoch, const Real *inState,
                                  Real *outState,
                                  bool forceComputation)
{
   CalculateRotationMatrix(epoch, forceComputation);
   
   Real pos[3] = {inState[0], inState[1], inState[2]};   
   Real vel[3] = {inState[3], inState[4], inState[5]};   
   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Input vector to ToMJ2000 = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);
      MessageInterface::ShowMessage(
      "the rotation matrix is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotMatrix(0,0),rotMatrix(0,1),rotMatrix(0,2),
      rotMatrix(1,0),rotMatrix(1,1),rotMatrix(1,2),
      rotMatrix(2,0),rotMatrix(2,1),rotMatrix(2,2));
      MessageInterface::ShowMessage(
      "the rotation matrix (as array) is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotData[0],rotData[1],rotData[2],rotData[3],rotData[4],
      rotData[5],rotData[6],rotData[7],rotData[8]);
      //MessageInterface::ShowMessage(
      //   "Input vector as datavec = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      //   tmpPosTo[0], tmpPosTo[1], tmpPosTo[2], tmpVelTo[0], tmpVelTo[1], tmpVelTo[2]);
   #endif
   //Real  outPos[3];
   //Real  outVel[3];
   Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      /*
      outPos[p] = rotData[p3]   * pos[0]   + 
                  rotData[p3+1] * pos[1] + 
                  rotData[p3+2] * pos[2];
      outVel[p] = (rotDotData[p3]    * pos[0]   + 
                   rotDotData[p3+1]  * pos[1] + 
                   rotDotData[p3+2]  * pos[2])
                  +
                  (rotData[p3]    * vel[0]   + 
                   rotData[p3+1]  * vel[1] + 
                   rotData[p3+2]  * vel[2]);
      */
      outState[p] = rotData[p3]   * pos[0]   + 
                    rotData[p3+1] * pos[1] + 
                    rotData[p3+2] * pos[2];
      outState[p+3] = (rotDotData[p3]    * pos[0]   + 
                       rotDotData[p3+1]  * pos[1] + 
                       rotDotData[p3+2]  * pos[2])
                      +
                      (rotData[p3]    * vel[0]   + 
                       rotData[p3+1]  * vel[1] + 
                       rotData[p3+2]  * vel[2]);
   }    
   
   /*
   outState[0] = outPos[0]; 
   outState[1] = outPos[1]; 
   outState[2] = outPos[2]; 
   outState[3] = outVel[0]; 
   outState[4] = outVel[1]; 
   outState[5] = outVel[2]; 
   */
   
 
   #ifdef DEBUG_CALCS
      //MessageInterface::ShowMessage(
      //   "Computed Output vector in ToMJ2000 = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      //   outPos[0], outPos[1], outPos[2], outVel[0], outVel[1], outVel[2]);
      MessageInterface::ShowMessage(
         "Output vector from ToMJ2000 = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         outState[0], outState[1], outState[2], outState[3], outState[4], outState[5]);
   #endif

   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == 21545.0))
      {
         MessageInterface::ShowMessage(
            "RotateToMJ2000Eq check for %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   Rotation matrix = |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n",
            rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
            rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
            rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.Get());
         MessageInterface::ShowMessage(
            "   input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
         //MessageInterface::ShowMessage(
         //   "   outpt State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
         //   outPos[0], outPos[1], outPos[2], outVel[0], outVel[1], 
         //   outVel[2]);
      }
   #endif

   return true;
}
//------------------------------------------------------------------------------
//  bool RotateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                          Rvector &outState, bool forceComputation)
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
                                    Rvector &outState,
                                    bool forceComputation)
{
   static Rvector3 tmpPosVec;
   static Rvector3 tmpVelVec;
   static const Real  *tmpPos = tmpPosVec.GetDataVector();
   static const Real  *tmpVel = tmpVelVec.GetDataVector();
   CalculateRotationMatrix(epoch, forceComputation);
   
   // *********** assuming only one 6-vector for now - UPDATE LATER!!!!!!
   ////Rvector3 tmpPos(inState[0],inState[1], inState[2]);
   ////Rvector3 tmpVel(inState[3],inState[4], inState[5]);
   ////Rmatrix33 tmpRot    = rotMatrix.Transpose();
   ////Rmatrix33 tmpRotDot = rotDotMatrix.Transpose();
   ////Rvector3 outPos     = tmpRot    * tmpPos ;
   ////Rvector3 outVel     = (tmpRotDot * tmpPos) + (tmpRot * tmpVel);
   tmpPosVec.Set(inState[0],inState[1], inState[2]);
   tmpVelVec.Set(inState[3],inState[4], inState[5]);

   ////Rvector3 tmpPosVec(inState[0],inState[1], inState[2]);
   ////Rvector3 tmpVelVec(inState[3],inState[4], inState[5]);
   //const Real tmpPos[3] = {inState[0], inState[1], inState[2]};
   //const Real tmpVel[3] = {inState[3], inState[4], inState[5]};
   // rotMatrix-T * tmpPos;
   // rotDotMatrix-T * tmpPos + rotMatrix-T * tmpVel
   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Input vector to FromMJ2000 = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);
      MessageInterface::ShowMessage(
      "the rotation matrix is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotMatrix(0,0),rotMatrix(0,1),rotMatrix(0,2),
      rotMatrix(1,0),rotMatrix(1,1),rotMatrix(1,2),
      rotMatrix(2,0),rotMatrix(2,1),rotMatrix(2,2));
      //MessageInterface::ShowMessage(
      //   "Input vector as datavec = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      //   tmpPos[0], tmpPos[1], tmpPos[2], tmpVel[0], tmpVel[1], tmpVel[2]);
   #endif
   Real  outPos[3];
   Real  outVel[3];
   const Real  rotDataT[9] = {rotData[0], rotData[3], rotData[6],
                              rotData[1], rotData[4], rotData[7],
                              rotData[2], rotData[5], rotData[8]};
   const Real  rotDotDataT[9] = {rotDotData[0], rotDotData[3], rotDotData[6],
                                 rotDotData[1], rotDotData[4], rotDotData[7],
                                 rotDotData[2], rotDotData[5], rotDotData[8]};
   Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      outPos[p] = rotDataT[p3]   * tmpPos[0]   + 
                  rotDataT[p3+1] * tmpPos[1] + 
                  rotDataT[p3+2] * tmpPos[2];
      outVel[p] = (rotDotDataT[p3]    * tmpPos[0]   + 
                   rotDotDataT[p3+1]  * tmpPos[1] + 
                   rotDotDataT[p3+2]  * tmpPos[2])
                  +
                  (rotDataT[p3]    * tmpVel[0]   + 
                   rotDataT[p3+1]  * tmpVel[1] + 
                   rotDataT[p3+2]  * tmpVel[2]);
   }     
   outState.Set(6,outPos[0], outPos[1], outPos[2], 
                  outVel[0], outVel[1], outVel[2]);

   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Computed Output vector in FromMJ2000 = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         outPos[0], outPos[1], outPos[2], outVel[0], outVel[1], outVel[2]);
      MessageInterface::ShowMessage(
         "Output vector from FromMJ2000 = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         outState[0], outState[1], outState[2], outState[3], outState[4], outState[5]);
   #endif
   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == 21545.0))
      {
         MessageInterface::ShowMessage(
            "RotateFromMJ2000Eq check for %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   Rotation matrix = |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n",
            rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
            rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
            rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.Get());
         MessageInterface::ShowMessage(
            "   input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
         MessageInterface::ShowMessage(
            "   outpt State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            outPos[0], outPos[1], outPos[2], outVel[0], outVel[1], 
            outVel[2]);
         firstCallFired = true;
      }
   #endif

   return true;
}

bool AxisSystem::RotateFromMJ2000Eq(const A1Mjd &epoch, 
                                    const Real *inState,
                                    Real *outState,
                                    bool forceComputation)
{
   CalculateRotationMatrix(epoch, forceComputation);
   
   Real pos[3] = {inState[0], inState[1], inState[2]};   
   Real vel[3] = {inState[3], inState[4], inState[5]};   
   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Input vector to FromMJ2000 = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);
      MessageInterface::ShowMessage(
      "the rotation matrix is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotMatrix(0,0),rotMatrix(0,1),rotMatrix(0,2),
      rotMatrix(1,0),rotMatrix(1,1),rotMatrix(1,2),
      rotMatrix(2,0),rotMatrix(2,1),rotMatrix(2,2));
      MessageInterface::ShowMessage(
      "the rotation matrix (as array) is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotData[0],rotData[1],rotData[2],rotData[3],rotData[4],
      rotData[5],rotData[6],rotData[7],rotData[8]);
   #endif
   const Real  rotDataT[9] = {rotData[0], rotData[3], rotData[6],
                              rotData[1], rotData[4], rotData[7],
                              rotData[2], rotData[5], rotData[8]};
   const Real  rotDotDataT[9] = {rotDotData[0], rotDotData[3], rotDotData[6],
                                 rotDotData[1], rotDotData[4], rotDotData[7],
                                 rotDotData[2], rotDotData[5], rotDotData[8]};
   Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      outState[p]   = rotDataT[p3]   * pos[0]   + 
                      rotDataT[p3+1] * pos[1] + 
                      rotDataT[p3+2] * pos[2];
      outState[p+3] = (rotDotDataT[p3]    * pos[0]   + 
                       rotDotDataT[p3+1]  * pos[1] + 
                       rotDotDataT[p3+2]  * pos[2])
                      +
                      (rotDataT[p3]    * vel[0]   + 
                       rotDataT[p3+1]  * vel[1] + 
                       rotDataT[p3+2]  * vel[2]);
   }     
   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Output vector from FromMJ2000 = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         outState[0], outState[1], outState[2], outState[3], outState[4], outState[5]);
   #endif
   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == 21545.0))
      {
         MessageInterface::ShowMessage(
            "RotateFromMJ2000Eq check for %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   Rotation matrix = |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n",
            rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
            rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
            rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.Get());
         MessageInterface::ShowMessage(
            "   input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
         //MessageInterface::ShowMessage(
         //   outPos[0], outPos[1], outPos[2], outVel[0], outVel[1], 
         //   outVel[2]);
         firstCallFired = true;
      }
   #endif

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


bool AxisSystem::IsParameterReadOnly(const Integer id) const
{
   if ((id == ORIGIN_NAME) || (id == J2000_BODY_NAME) ||
       // The next 2 repeat settings passed in from the CS
       (id == UPDATE_INTERVAL) || (id == OVERRIDE_ORIGIN_INTERVAL))
      return true;
   // Only access epoch if it is used
   if ((id == EPOCH) && (UsesEpoch() == GmatCoordinate::NOT_USED))
      return true;
   return CoordinateBase::IsParameterReadOnly(id);
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
   if (id == EPOCH)           return epoch.Get(); 
   if (id == UPDATE_INTERVAL) return updateInterval;
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
   if (id == UPDATE_INTERVAL)
   {
      updateInterval = value;
      //MessageInterface::ShowMessage("In AS, interval set to %f\n", value);
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

bool AxisSystem::GetBooleanParameter(const Integer id) const
{
   if (id == OVERRIDE_ORIGIN_INTERVAL) return overrideOriginInterval;
   return CoordinateBase::GetBooleanParameter(id); 
}

bool AxisSystem::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

bool AxisSystem::SetBooleanParameter(const Integer id,
                                     const bool value)
{
   if (id == OVERRIDE_ORIGIN_INTERVAL)
   {
      overrideOriginInterval = value;
      return true;
   }
   return CoordinateBase::SetBooleanParameter(id, value);
}

bool AxisSystem::SetBooleanParameter(const std::string &label,
                                     const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
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
      
      nutationSrc    = itrf->GetNutationTermsSource();
      planetarySrc   = itrf->GetPlanetaryTermsSource();
      Integer numNut = itrf->GetNumberOfNutationTerms();
      A.SetSize(numNut);   A.MakeZeroVector();
      #ifdef DEBUG_ITRF_UPDATES
      MessageInterface::ShowMessage("In Axis System, nutation source is %d\n",
                        (Integer) nutationSrc);
      //for (Integer ii = 0; ii < A.GetSize(); ii++)
      //   MessageInterface::ShowMessage("In AxisSystem, A(%d) = %f\n", ii, A(ii));
      #endif
      B.SetSize(numNut);   B.MakeZeroVector();
      C.SetSize(numNut);   C.MakeZeroVector();
      D.SetSize(numNut);   D.MakeZeroVector();
      E.SetSize(numNut);   E.MakeZeroVector();
      F.SetSize(numNut);   F.MakeZeroVector();
      
      Integer numPlan = itrf->GetNumberOfPlanetaryTerms();
      Ap.SetSize(numPlan);   Ap.MakeZeroVector();
      Bp.SetSize(numPlan);   Bp.MakeZeroVector();
      Cp.SetSize(numPlan);   Cp.MakeZeroVector();
      Dp.SetSize(numPlan);   Dp.MakeZeroVector();
      
      bool OK = itrf->GetNutationTerms(a, A, B, C, D, E, F);
      if (!OK) throw CoordinateSystemException("Error getting nutation data.");
      #ifdef DEBUG_ITRF_UPDATES
      for (Integer ii = 0; ii < A.GetSize(); ii++)
         MessageInterface::ShowMessage("In AxisSystem, after getting nutation data, A(%d) = %f\n", ii, A(ii));
      #endif
      
      aVals = new Integer[numNut * 5];
      for (Integer i = 0; i < 5; i++)
      {
         for (Integer j=0; j< numNut; j++)
         {
            // DJC changed; was this: 
            //aVals[i*5+j] = (a.at(i)).at(j);
            aVals[i*numNut+j] = (a.at(i)).at(j);
         }
      }
      
      #ifdef DEBUG_a_MATRIX
         MessageInterface::ShowMessage("aVals = \n");
         for (Integer q = 0; q < numNut; ++q)
            MessageInterface::ShowMessage("         %3d: %8d %8d %8d %8d %8d\n",
               q+1, aVals[q], aVals[q+numNut], aVals[q+2*numNut], 
               aVals[q+3*numNut], aVals[q+4*numNut]);
      #endif
      
      if (nutationSrc == GmatItrf::NUTATION_1996)
      {
         OK      = itrf->GetPlanetaryTerms(ap, Ap, Bp, Cp, Dp);
         if (!OK) throw CoordinateSystemException("Error getting planetary data.");
      
         apVals = new Integer[numPlan*10];
         for (Integer i = 0; i < 10; i++)
         {
            for (Integer j=0; j< numPlan; j++)
            {
               //apVals[i*10+j] = (ap.at(i)).at(j);
               apVals[i*numPlan+j] = (ap.at(i)).at(j);
            }
         }

         #ifdef DEBUG_Ap_MATRIX
             MessageInterface::ShowMessage("apVals = \n");
            for (Integer q = 0; q < numPlan; ++q)
                MessageInterface::ShowMessage(
                  "         %3d: %8d %8d %8d %8d %8d   %8d %8d %8d %8d %8d\n",
                  q+1, apVals[q], apVals[q+numPlan], apVals[q+2*numPlan], 
                  apVals[q+3*numPlan], apVals[q+4*numPlan], apVals[q+5*numPlan], 
                  apVals[q+6*numPlan], apVals[q+7*numPlan], apVals[q+8*numPlan], 
                  apVals[q+9*numPlan]);
         #endif
       }
      
         AVals = A.GetDataVector();
         #ifdef DEBUG_ITRF_UPDATES
         for (Integer ii = 0; ii < A.GetSize(); ii++)
            MessageInterface::ShowMessage("AVals(%d) = %f\n", ii, AVals[ii]);
         #endif
         BVals = B.GetDataVector();
         CVals = C.GetDataVector();
         DVals = D.GetDataVector();
         EVals = E.GetDataVector();
         FVals = F.GetDataVector();
         ApVals = Ap.GetDataVector();
         BpVals = Bp.GetDataVector();
         CpVals = Cp.GetDataVector();
         DpVals = Dp.GetDataVector();
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
//Rmatrix33 AxisSystem::ComputePrecessionMatrix(const Real tTDB, A1Mjd atEpoch)
void AxisSystem::ComputePrecessionMatrix(const Real tTDB, A1Mjd atEpoch)
{
   //Real dt = Abs(atEpoch.Subtract(lastPRECEpoch)) * SECS_PER_DAY;
   //if ( dt < updateIntervalToUse)
   //{
   //   #ifdef DEBUG_UPDATE
   //      cout << ">>> Using previous saved PREC values ......" << endl;
   //      cout << "lastPREC = "  << lastPREC << endl;
   //   #endif
   //   return lastPREC; 
   //}
   
   //#define DEBUG_FIRST_CALL

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   AxisSystem::ComputePrecessionMatrix(%.12lf, %.12lf)\n", tTDB, 
            atEpoch.Get());
//         InitializeFK5();
   #endif
   
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
   Real cosTheta = cos(Theta);
   Real cosz     = cos(z);
   Real coszeta  = cos(zeta);
   Real sinTheta = sin(Theta);
   Real sinz     = sin(z);
   Real sinzeta  = sin(zeta);
   
   // Compute Rotation matrix for transformations from FK5 to MOD
   // (Vallado Eq. 3-57)
   PREC.Set( cosTheta*cosz*coszeta - sinz*sinzeta,
            -sinzeta*cosTheta*cosz - sinz*coszeta,
            -sinTheta*cosz,
             sinz*cosTheta*coszeta + sinzeta*cosz,
            -sinz*sinzeta*cosTheta + cosz*coszeta,
            -sinTheta*sinz,
             sinTheta*coszeta,
            -sinTheta*sinzeta,
             cosTheta);
   //PREC(0,0) =  cosTheta*cosz*coszeta - sinz*sinzeta;
   //PREC(0,1) = -sinzeta*cosTheta*cosz - sinz*coszeta;
   //PREC(0,2) = -sinTheta*cosz;
   //PREC(1,0) =  sinz*cosTheta*coszeta + sinzeta*cosz;
   //PREC(1,1) = -sinz*sinzeta*cosTheta + cosz*coszeta;
   //PREC(1,2) = -sinTheta*sinz;
   //PREC(2,0) =  sinTheta*coszeta;
   //PREC(2,1) = -sinTheta*sinzeta;
   //PREC(2,2) =  cosTheta;
   #ifdef DEBUG_ROT_MATRIX
      cout << "PREC = " << endl << PREC << endl;
   #endif
   lastPREC = PREC;
   lastPRECEpoch = atEpoch;
//   return PREC;
}

//Rmatrix33 AxisSystem::ComputeNutationMatrix(const Real tTDB, A1Mjd atEpoch,
void AxisSystem::ComputeNutationMatrix(const Real tTDB, A1Mjd atEpoch,
                                            Real &dPsi,
                                            Real &longAscNodeLunar,
                                            Real &cosEpsbar,
                                            bool forceComputation)
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   AxisSystem::ComputeNutationMatrix(%.12lf, %.12lf, %.12lf, "
            "%.12lf, %.12lf)\n", tTDB, atEpoch.Get(), dPsi, longAscNodeLunar, 
            cosEpsbar);
   #endif

   static const Real const125 = 125.04455501*RAD_PER_DEG;
   static const Real const134 = 134.96340251*RAD_PER_DEG;
   static const Real const357 = 357.52910918*RAD_PER_DEG;
   static const Real const93  =  93.27209062*RAD_PER_DEG;
   static const Real const297 = 297.85019547*RAD_PER_DEG;

   register Real tTDB2   = tTDB  * tTDB;
   register Real tTDB3   = tTDB2 * tTDB;
   register Real tTDB4   = tTDB3 * tTDB;
   // Compute nutation - NOTE: this algorithm is based on the IERS 1996
   // Theory of Precession and Nutation. This can also be used with the 
   // 1980 Theory - the E and F terms are zero, and will fall out on the 
   // floor, roll around and under the door, down the stairs and into
   // the neighbor's yard.  It can also be used, but is not tested, 
   // with the 2000 Theory.

   // Compute values to be passed out first ... 
   longAscNodeLunar  = const125 + (  -6962890.2665*tTDB 
                       + 7.4722*tTDB2 + 0.007702*tTDB3 - 0.00005939*tTDB4)
                       * RAD_PER_ARCSEC;
   Real Epsbar       = (84381.448 - 46.8150*tTDB - 0.00059*tTDB2 
                        + 0.001813*tTDB3) * RAD_PER_ARCSEC;
   cosEpsbar         = cos(Epsbar);
   
   // if not enough time has passed, just return the last value
   Real dt = fabs(atEpoch.Subtract(lastNUTEpoch)) * SECS_PER_DAY;
   #ifdef DEBUG_UPDATE
      cout.precision(30);
      cout << "ENTERED ComputeNutation ....." << endl;
      cout << "atEpoch = " << atEpoch.Get() << endl;
      cout << "lastNUTEpoch = " << lastNUTEpoch.Get() << endl;
      cout << "dt = " << dt << endl;
      cout << "longAscNodeLunar = "  << longAscNodeLunar << endl;
      cout << "cosEpsbar = "  << cosEpsbar << endl;
   #endif
   if (( dt < updateIntervalToUse) && (!forceComputation))
   {
      #ifdef DEBUG_UPDATE
         cout << ">>> Using previous saved values ......" << endl;
         cout << "lastDPsi = "  << lastDPsi << endl;
         cout << "lastNUT = "  << lastNUT << endl;
      #endif
      dPsi = lastDPsi;

      #ifdef DEBUG_FIRST_CALL
         if (!firstCallFired)
            MessageInterface::ShowMessage(
               "   Using buffered nutation data: %.13lf = 86400*(%.12lf - %.12lf)\n",
               dt, atEpoch.Get(), lastNUTEpoch.Get());
      #endif
      
      return;
//      return lastNUT; 
   }

   #ifdef DEBUG_UPDATE
      cout << ">>> Computing brand new values ......" << endl;
   #endif
   // otherwise, need to recompute all the nutation data
   dPsi      = 0.0;
   Real dEps = 0.0;
   // First, compute useful angles (Vallado Eq. 3-54)
   // NOTE - taken from Steve Queen's code - he has apparently converted
   // the values in degrees (from Vallado Eq. 3-54) to arcsec before
   // performing these computations
   register Real meanAnomalyMoon   = const134 + (1717915923.2178*tTDB 
        + 31.8792*tTDB2 + 0.051635*tTDB3 - 0.00024470*tTDB4)*RAD_PER_ARCSEC;
   register Real meanAnomalySun    = const357 + ( 129596581.0481*tTDB 
        -  0.5532*tTDB2 - 0.000136*tTDB3 - 0.00001149*tTDB4)*RAD_PER_ARCSEC;
   register Real argLatitudeMoon   =  const93 + (1739527262.8478*tTDB 
        - 12.7512*tTDB2 + 0.001037*tTDB3 + 0.00000417*tTDB4)*RAD_PER_ARCSEC;
   register Real meanElongationSun = const297 + (1602961601.2090*tTDB 
        -  6.3706*tTDB2 + 0.006593*tTDB3 - 0.00003169*tTDB4)*RAD_PER_ARCSEC;
   
   // Now, sum using nutation coefficients  (Vallado Eq. 3-60)
   Integer i  = 0;
   Real apNut = 0.0;
   Real cosAp = 0.0;
   Real sinAp = 0.0;
   Integer nut = itrf->GetNumberOfNutationTerms();
   /*
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
    */

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   Nutation data:\n"
            "      nut               = %d\n"
            "      meanAnomalyMoon   = %.13lf\n"
            "      meanAnomalySun    = %.13lf\n"
            "      argLatitudeMoon   = %.13lf\n"
            "      meanElongationSun = %.13lf\n"
            "      longAscNodeLunar  = %.13lf\n"
            "      cosEpsbar         = %.13lf\n"
            "      tTDB              = %.13le\n"
            "      tTDB2             = %.13le\n"
            "      tTDB3             = %.13le\n"
            "      tTDB4             = %.13le\n",
            nut, meanAnomalyMoon, meanAnomalySun, argLatitudeMoon,
            meanElongationSun, longAscNodeLunar, cosEpsbar, tTDB, tTDB2, tTDB3,
            tTDB4);

      if (!firstCallFired) 
      {
         MessageInterface::ShowMessage(
            "      a[0](1st 12 els)   = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[0], aVals[1], aVals[2], aVals[3], aVals[4], aVals[5], 
            aVals[6], aVals[7], aVals[8], aVals[9], aVals[10], aVals[11]);
         MessageInterface::ShowMessage(
            "      a[1](1st 12 els)   = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*1+0], aVals[nut*1+1], aVals[nut*1+2], aVals[nut*1+3], 
            aVals[nut*1+4], aVals[nut*1+5], aVals[nut*1+6], aVals[nut*1+7], 
            aVals[nut*1+8], aVals[nut*1+9], aVals[nut*1+10], aVals[nut*1+11]);
         MessageInterface::ShowMessage(
            "      a[2](1st 12 els)   = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*2+0], aVals[nut*2+1], aVals[nut*2+2], aVals[nut*2+3], 
            aVals[nut*2+4], aVals[nut*2+5], aVals[nut*2+6], aVals[nut*2+7], 
            aVals[nut*2+8], aVals[nut*2+9], aVals[nut*2+10], aVals[nut*2+11]);
         MessageInterface::ShowMessage(
            "      a[3](1st 12 els)   = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*3+0], aVals[nut*3+1], aVals[nut*3+2], aVals[nut*3+3], 
            aVals[nut*3+4], aVals[nut*3+5], aVals[nut*3+6], aVals[nut*3+7], 
            aVals[nut*3+8], aVals[nut*3+9], aVals[nut*3+10], aVals[nut*3+11]);
         MessageInterface::ShowMessage(
            "      a[4](1st 12 els)   = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*4+0], aVals[nut*4+1], aVals[nut*4+2], aVals[nut*4+3], 
            aVals[nut*4+4], aVals[nut*4+5], aVals[nut*4+6], aVals[nut*4+7], 
            aVals[nut*4+8], aVals[nut*4+9], aVals[nut*4+10], aVals[nut*4+11]);
         MessageInterface::ShowMessage(
            "      a[*](Last els)     = [%d %d %d %d %d]\n",
            aVals[nut-1], aVals[nut+nut-1], aVals[nut*2+nut-1], 
            aVals[nut*3+nut-1], aVals[nut*4+nut-1]);
            
         MessageInterface::ShowMessage(
            "      a[0](els 51-62)    = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[51], aVals[52], aVals[53], aVals[54], aVals[55], aVals[56], 
            aVals[57], aVals[58], aVals[59], aVals[60], aVals[61], aVals[62]);
         MessageInterface::ShowMessage(
            "      a[1](els 51-62)    = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*1+51], aVals[nut*1+52], aVals[nut*1+53], aVals[nut*1+54], 
            aVals[nut*1+55], aVals[nut*1+56], aVals[nut*1+57], aVals[nut*1+58], 
            aVals[nut*1+59], aVals[nut*1+60], aVals[nut*1+61], aVals[nut*1+62]);
         MessageInterface::ShowMessage(
            "      a[2](els 51-62)    = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*2+51], aVals[nut*2+52], aVals[nut*2+53], aVals[nut*2+54], 
            aVals[nut*2+55], aVals[nut*2+56], aVals[nut*2+57], aVals[nut*2+58], 
            aVals[nut*2+59], aVals[nut*2+60], aVals[nut*2+61], aVals[nut*2+62]);
         MessageInterface::ShowMessage(
            "      a[3](els 51-62)    = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*3+51], aVals[nut*3+52], aVals[nut*3+53], aVals[nut*3+54], 
            aVals[nut*3+55], aVals[nut*3+56], aVals[nut*3+57], aVals[nut*3+58], 
            aVals[nut*3+59], aVals[nut*3+60], aVals[nut*3+61], aVals[nut*3+62]);
         MessageInterface::ShowMessage(
            "      a[4](els 51-62)    = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*4+51], aVals[nut*4+52], aVals[nut*4+53], aVals[nut*4+54], 
            aVals[nut*4+55], aVals[nut*4+56], aVals[nut*4+57], aVals[nut*4+58], 
            aVals[nut*4+59], aVals[nut*4+60], aVals[nut*4+61], aVals[nut*4+62]);
         MessageInterface::ShowMessage(
            "      a[*](Last els)    = [%d %d %d %d %d]\n",
            aVals[nut-1], aVals[nut+nut-1], aVals[nut*2+nut-1], 
            aVals[nut*3+nut-1], aVals[nut*4+nut-1]);
            
         MessageInterface::ShowMessage(
            "      A(1st 12 els)     = [%.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf]\n",
            AVals[0], AVals[1], AVals[2], AVals[3], 
            AVals[4], AVals[5], AVals[6], AVals[7], 
            AVals[8], AVals[9], AVals[10], AVals[11]);
         MessageInterface::ShowMessage(
            "      B(1st 12 els)     = [%.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf]\n",
            BVals[0], BVals[1], BVals[2], BVals[3], BVals[4], BVals[5], 
            BVals[6], BVals[7], BVals[8], BVals[9], BVals[10], BVals[11]);
         MessageInterface::ShowMessage(
            "      C(1st 12 els)     = [%.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf]\n",
            CVals[0], CVals[1], CVals[2], CVals[3], CVals[4], CVals[5], 
            CVals[6], CVals[7], CVals[8], CVals[9], CVals[10], CVals[11]);
         MessageInterface::ShowMessage(
            "      D(1st 12 els)     = [%.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf]\n",
            DVals[0], DVals[1], DVals[2], DVals[3], DVals[4], DVals[5], 
            DVals[6], DVals[7], DVals[8], DVals[9], DVals[10], DVals[11]);
         MessageInterface::ShowMessage(
            "      E(1st 12 els)     = [%.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf]\n",
            EVals[0], EVals[1], EVals[2], EVals[3], EVals[4], EVals[5], 
            EVals[6], EVals[7], EVals[8], EVals[9], EVals[10], EVals[11]);
         MessageInterface::ShowMessage(
            "      F(1st 12 els)     = [%.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf]\n",
            FVals[0], FVals[1], FVals[2], FVals[3], FVals[4], FVals[5], 
            FVals[6], FVals[7], FVals[8], FVals[9], FVals[10], FVals[11]);
      }
   #endif

   for (i = nut-1; i >= 0; i--)
   {
      //apNut = aVals[0][i]*meanAnomalyMoon + aVals[1][i]*meanAnomalySun 
      //+ aVals[2][i]*argLatitudeMoon + aVals[3][i]*meanElongationSun 
      //+ aVals[4][i]*longAscNodeLunar;
      #ifdef DEBUG_a_MATRIX
         MessageInterface::ShowMessage("   a[%d}:  %8d %8d %8d %8d %8d\n", i+1,
            aVals[i], aVals[nut*1+i], aVals[nut*2+i], aVals[nut*3+i], 
            aVals[nut*4+i]);
      #endif

      apNut = aVals[i]*meanAnomalyMoon + aVals[nut*1+i]*meanAnomalySun 
         + aVals[nut*2+i]*argLatitudeMoon + aVals[nut*3+i]*meanElongationSun 
         + aVals[nut*4+i]*longAscNodeLunar;
      
      cosAp = cos(apNut);
      sinAp = sin(apNut);
      //dPsi += (A[i] + B[i]*tTDB )*sinAp + E[i]*cosAp;
      //dEps += (C[i] + D[i]*tTDB )*cosAp + F[i]*sinAp;
      if (nutationSrc == GmatItrf::NUTATION_1980)
      {
         dPsi += (AVals[i] + BVals[i]*tTDB )*sinAp;
         dEps += (CVals[i] + DVals[i]*tTDB )*cosAp;
      }
      else // 1996 amd 200 have E and F terms
      {
         dPsi += (AVals[i] + BVals[i]*tTDB )*sinAp + EVals[i]*cosAp;
         dEps += (CVals[i] + DVals[i]*tTDB )*cosAp + FVals[i]*sinAp;
      }

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "      apNut(%3d) dPsi(%3d) dEps(%3d) = %.13le %.13le %.13le\n", i, 
            i, i, apNut, dPsi, dEps);
   #endif
   }
   
   dPsi *= RAD_PER_ARCSEC;
   dEps *= RAD_PER_ARCSEC;
   
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "      dPsi              = %.13lf\n"
            "      dEps              = %.13lf\n", dPsi, dEps);
   #endif

   // Compute the corrections for planetary effects on the nutation and
   // the obliquity of the ecliptic 
   // NOTE - this part is commented out for now, per Steve Hughes
   // First, compute the mean Heliocentric longitudes of the planets, and the
   // general precession in longitude
    Real dPsiAddend = 0.0, dEpsAddend = 0.0;
   if (nutationSrc == GmatItrf::NUTATION_1996)
   {   
   
      register Real longVenus   = (181.979800853  + 58517.8156748  * tTDB)* RAD_PER_DEG;
      register Real longEarth   = (100.466448494  + 35999.3728521  * tTDB)* RAD_PER_DEG;
      register Real longMars    = (355.433274605  + 19140.299314   * tTDB)* RAD_PER_DEG;
      register Real longJupiter = ( 34.351483900  +  3034.90567464 * tTDB)* RAD_PER_DEG;
      register Real longSaturn  = ( 50.0774713998 +  1222.11379404 * tTDB)* RAD_PER_DEG;
      register Real genPrec     = (1.39697137214 * tTDB + 0.0003086 * tTDB2)
                                 * RAD_PER_DEG;
      register Real apPlan = 0.0;
      register Real cosApP = 0.0;
      register Real sinApP = 0.0;
      register Integer nutpl = itrf->GetNumberOfPlanetaryTerms();
    
      #ifdef DEBUG_FIRST_CALL
         if (!firstCallFired)
            MessageInterface::ShowMessage(
               "      longVenus         = %.13lf\n"
               "      longEarth         = %.13lf\n"
               "      longMars          = %.13lf\n"
               "      longJupiter       = %.13lf\n"
               "      longSaturn        = %.13lf\n"
               "      genPrec           = %.13lf\n"
               "      nutpl             = %d\n",
               longVenus, longEarth, longMars, longJupiter, longSaturn, genPrec, 
               nutpl);
      #endif
     
      for (i = nutpl-1; i >= 0; i--)
      {
             apPlan = apVals[0+i]*longVenus + apVals[nutpl*1+i]*longEarth 
             + apVals[nutpl*2+i]*longMars   + apVals[nutpl*3+i]*longJupiter 
             + apVals[nutpl*4+i]*longSaturn + apVals[nutpl*5+i]*genPrec
             + apVals[nutpl*6+i]*meanElongationSun
             + apVals[nutpl*7+i]*argLatitudeMoon
             + apVals[nutpl*8+i]*meanAnomalyMoon
             + apVals[nutpl*9+i]*longAscNodeLunar;
             cosApP = cos(apPlan);
             sinApP = sin(apPlan);
             dPsiAddend += (( ApVals[i] + BpVals[i]*tTDB )*sinApP);
             dEpsAddend += (( CpVals[i] + DpVals[i]*tTDB )*cosApP);
      }
   } // end if nutationSrc == NUTATION_1996
   
    dPsi += dPsiAddend * RAD_PER_ARCSEC;
    //dEps += dPsiAddend * RAD_PER_ARCSEC;
    dEps += dEpsAddend * RAD_PER_ARCSEC;
    
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "      dPsi(0)           = %.13lf\n"
            "      dEps(0)           = %.13lf\n",
            dPsi, dEps);
   #endif
    
   // FOR NOW, SQ's code to approximate GSRF frame
   // NOTE - do we delete this when we put in the planetary stuff above?
   // offset and rate correction to approximate GCRF, Ref.[1], Eq (3-63)  - SQ
   // This is Vallado Eq. 3-62 - WCS
   // commented out per Steve Hughes 2006.05.03
   //dPsi += (-0.0431 - 0.2957*tTDB )*RAD_PER_ARCSEC;
   //dEps += (-0.0051 - 0.0277*tTDB )*RAD_PER_ARCSEC;
   
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "      dPsi(1)           = %.13lf\n"
            "      dEps(1)           = %.13lf\n",
            dPsi, dEps);
   #endif
    
   // Compute obliquity of the ecliptic (Vallado Eq. 3-52 & Eq. 3-63)
   Real TrueOoE = Epsbar + dEps;
   
   // Compute useful trigonometric quantities
   Real cosdPsi   = cos(dPsi);
   Real cosTEoE   = cos(TrueOoE);
   Real sindPsi   = sin(dPsi);
   Real sinEpsbar = sin(Epsbar);
   Real sinTEoE   = sin(TrueOoE);   
   
   // Compute Rotation matrix for transformations from MOD to TOD
   // (Vallado Eq. 3-64)
   NUT.Set( cosdPsi,
           -sindPsi*cosEpsbar,
           -sindPsi*sinEpsbar,
            sindPsi*cosTEoE, 
            cosTEoE*cosdPsi*cosEpsbar + sinTEoE*sinEpsbar,
            sinEpsbar*cosTEoE*cosdPsi - sinTEoE*cosEpsbar,
            sinTEoE*sindPsi,
            sinTEoE*cosdPsi*cosEpsbar - sinEpsbar*cosTEoE,
            sinTEoE*sinEpsbar*cosdPsi + cosTEoE*cosEpsbar);
   
   lastNUTEpoch = atEpoch;
   lastNUT      = NUT;
   lastDPsi     = dPsi; 
   
   #ifdef DEBUG_ROT_MATRIX
      cout << "atEpoch = " << endl << atEpoch.Get() << endl;
      cout << "NUT = " << endl << NUT << endl;
      cout << "longAscNodeLunar = " << endl << longAscNodeLunar << endl;
      cout << "cosEpsbar = " << endl << cosEpsbar << endl;
      cout << "dPsi = " << endl << dPsi << endl;
   #endif
//   return NUT;
}

//Rmatrix33 AxisSystem::ComputeSiderealTimeRotation(const Real jdTT,
void AxisSystem::ComputeSiderealTimeRotation(const Real jdTT,
                                                  const Real tUT1,
                                                  Real dPsi,
                                                  Real longAscNodeLunar,
                                                  Real cosEpsbar,
                                                  Real &cosAst,
                                                  Real &sinAst)
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
      {
         MessageInterface::ShowMessage("AxisSystem::ComputeSiderealTimeRotation, "
         "for object of type %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   AxisSystem::ComputeSiderealTimeRotation(%.12lf, %.12lf, %.12lf,"
            " %.12lf, %.12lf, %.12lf, %.12lf)\n", jdTT, tUT1, dPsi, 
            longAscNodeLunar, cosEpsbar, cosAst, sinAst);
      }
   #endif

   Real tUT12    = tUT1  * tUT1;
   Real tUT13    = tUT12 * tUT1;
   
   // First, compute the equation of the equinoxes
   // If the time is before January 1, 1997, don't use the last two terms
   Real term2 = 0.0;
   Real term3 = 0.0;
   if (jdTT > JD_OF_JANUARY_1_1997)
   {
      term2 = (0.00264 * sin(longAscNodeLunar))        * RAD_PER_ARCSEC;
      term3 = (0.000063 * sin(2.0 * longAscNodeLunar)) * RAD_PER_ARCSEC;
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
   //Real ThetaGmst = ((67310.54841) + 
   //                  ((876600 * 3600.0) + (8640184.812866)*tUT1 +
   //                  (0.093104)*tUT12 - (6.2e-06)*tUT13 )) * sec2deg
   //                  * RAD_PER_DEG;
   ThetaGmst = AngleUtil::PutAngleInRadRange(ThetaGmst,0.0,GmatMathUtil::TWO_PI);
   
   Real ThetaAst = ThetaGmst + EQequinox;
   
   //Compute useful trignonometric quantities
   cosAst = cos(ThetaAst);
   sinAst = sin(ThetaAst);
   
   // Compute Rotation matrix for Sidereal Time
   // (Vallado Eq. 3-64)
   ST.Set( cosAst, sinAst, 0.0,
          -sinAst, cosAst, 0.0,
              0.0,    0.0, 1.0);
   //ST(0,0) =  cosAst;
   //ST(0,1) =  sinAst;
   //ST(0,2) =  0.0;
   //ST(1,0) = -sinAst;
   //ST(1,1) =  cosAst;
   //ST(1,2) =  0.0;
   //ST(2,0) =  0.0;
   //ST(2,1) =  0.0;
   //ST(2,2) =  1.0;
   
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "      EQequinox           = %.13lf\n"
            "      ThetaGmst           = %.13lf\n"
            "      ThetaAst            = %.13lf\n",
            EQequinox, ThetaGmst, ThetaAst);
   #endif
   #ifdef DEBUG_ROT_MATRIX
      cout << "ST = " << endl << ST << endl;
   #endif

//   return ST;
}

//Rmatrix33 AxisSystem::ComputeSiderealTimeDotRotation(const Real mjdUTC, 
void AxisSystem::ComputeSiderealTimeDotRotation(const Real mjdUTC, 
                                                     A1Mjd atEpoch,
                                                     Real cosAst, Real sinAst,
                                                     bool forceComputation)
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   AxisSystem::ComputeSiderealTimeDotRotation(%.12lf, %.12lf,"
            " %.12lf, %.12lf)\n", mjdUTC, atEpoch.Get(),
            cosAst, sinAst);
   #endif

   Real dt = fabs(atEpoch.Subtract(lastSTDerivEpoch)) * SECS_PER_DAY;
   if (( dt < updateIntervalToUse) && (!forceComputation))
   {
      #ifdef DEBUG_UPDATE
         cout << ">>> Using previous saved STDeriv values ......" << endl;
         cout << "lastSTDeriv = "  << lastSTDeriv << endl;
      #endif
      return;
//      return lastSTDeriv; 
   }
   // Convert to MJD UTC to use for polar motion  and LOD 
   // interpolations
   // Get the polar motion and lod data
   Real lod = 0.0;
   Real x, y;
   eop->GetPolarMotionAndLod(mjdUTC,x,y,lod);
   
   // Compute the portion that has a significant time derivative
   Real omegaE = 7.29211514670698e-05 * (1.0 - (lod / SECS_PER_DAY));
   STderiv.Set(-omegaE * sinAst,  omegaE * cosAst, 0.0,
               -omegaE * cosAst, -omegaE * sinAst, 0.0,
                            0.0,              0.0, 0.0);
   //STderiv(0,0) = -omegaE * sinAst;
   //STderiv(0,1) =  omegaE * cosAst;
   //STderiv(0,2) =  0.0;
   //STderiv(1,0) = -omegaE * cosAst;
   //STderiv(1,1) = -omegaE * sinAst;
   //STderiv(1,2) =  0.0;
   //STderiv(2,0) =  0.0;
   //STderiv(2,1) =  0.0;
   //STderiv(2,2) =  0.0;
   
   #ifdef DEBUG_ROT_MATRIX
      cout << "lod = " << lod << endl;
      cout << "STderiv = " << endl << STderiv << endl;
   #endif
      
   lastSTDeriv      = STderiv;
   lastSTDerivEpoch = atEpoch;
   
//   return STderiv;
}

//Rmatrix33 AxisSystem::ComputePolarMotionRotation(const Real mjdUTC, A1Mjd atEpoch)
void AxisSystem::ComputePolarMotionRotation(const Real mjdUTC, A1Mjd atEpoch,
                                            bool forceComputation)
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   AxisSystem::ComputePolarMotionRotation(%.12lf, %.12lf)\n", 
            mjdUTC, atEpoch.Get());
   #endif

   Real dt = fabs(atEpoch.Subtract(lastPMEpoch)) * SECS_PER_DAY;
   if (( dt < updateIntervalToUse) && (!forceComputation))
   {
      #ifdef DEBUG_UPDATE
         cout << ">>> Using previous saved PM values ......" << endl;
         cout << "lastPM = "  << lastPM << endl;
      #endif
      return;
//      return lastPM; 
   }
   // Get the polar motion and lod data
   Real lod = 0.0;
   Real x, y;
   eop->GetPolarMotionAndLod(mjdUTC,x,y,lod);
   
   // Compute useful trigonometric quantities
   Real cosX = cos(-x * RAD_PER_ARCSEC);
   Real sinX = sin(-x * RAD_PER_ARCSEC);
   Real cosY = cos(-y * RAD_PER_ARCSEC);
   Real sinY = sin(-y * RAD_PER_ARCSEC);
   
   // Compute the polar motion rotation matrix
   PM.Set( cosX,  sinX*sinY, -sinX*cosY,
            0.0,       cosY,       sinY,
           sinX, -cosX*sinY,  cosX*cosY); 
   //PM(0,0) =  cosX;
   //PM(0,1) =  sinX*sinY;
   //PM(0,2) = -sinX*cosY;
   //PM(1,0) =  0.0;
   //PM(1,1) =  cosY;
   //PM(1,2) =  sinY;
   //PM(2,0) =  sinX;
   //PM(2,1) = -cosX*sinY;
   //PM(2,2) =  cosX*cosY;

   #ifdef DEBUG_ROT_MATRIX
      cout << "x = " << x << " and y = " << y << endl;
      cout << "PM = " << endl << PM << endl;
   #endif
      
   lastPM      = PM;
   lastPMEpoch = atEpoch;

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "      lod         = %.13lf\n"
            "      x           = %.13lf\n"
            "      y           = %.13lf\n",
            lod, x * RAD_PER_ARCSEC, y * RAD_PER_ARCSEC);
   #endif
   #ifdef DEBUG_FIRST_CALL
      firstCallFired = true;
   #endif

//   return PM;
}


