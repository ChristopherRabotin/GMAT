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
#include "Planet.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"
#include "Attitude.hpp"
#include "Rvector3.hpp"
#include "CoordinateSystemException.hpp"

//#define DEBUG_EQUATOR_AXES

#ifdef DEBUG_EQUATOR_AXES
   #include "MessageInterface.hpp"
#endif


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
   objectTypeNames.push_back("EquatorAxes");
   parameterCount = EquatorAxesParamCount;
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

GmatCoordinate::ParameterUsage EquatorAxes::UsesEopFile() const
{
   if (originName == SolarSystem::EARTH_NAME) return GmatCoordinate::REQUIRED;
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage EquatorAxes::UsesItrfFile() const
{
   if (originName == SolarSystem::EARTH_NAME) return GmatCoordinate::REQUIRED;
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage EquatorAxes::UsesNutationUpdateInterval() const
{
   if (originName == SolarSystem::EARTH_NAME) 
      return GmatCoordinate::REQUIRED;
   return DynamicAxes::UsesNutationUpdateInterval();
}

//------------------------------------------------------------------------------
//  bool EquatorAxes::Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for this EquatorAxes.
 *
 */
//------------------------------------------------------------------------------
bool EquatorAxes::Initialize()
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
void EquatorAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                          bool forceComputation) 
{
   #ifdef DEBUG_EQUATOR_AXES
      MessageInterface::ShowMessage("Entering Equator::Calculate with epoch = %.12f\n",
         (Real) atEpoch.Get());
   #endif
   if (originName == SolarSystem::EARTH_NAME)
   {
      Real dPsi             = 0.0;
      Real longAscNodeLunar = 0.0;
      Real cosEpsbar        = 0.0;
      
      // convert epoch (A1 MJD) to TT MJD (for calculations)
      Real mjdTT = TimeConverterUtil::Convert(atEpoch.Get(),
                   TimeConverterUtil::A1MJD, TimeConverterUtil::TTMJD, 
                   GmatTimeUtil::JD_JAN_5_1941);      
      Real offset = GmatTimeUtil::JD_JAN_5_1941 - 2451545.0;
      Real tTDB  = (mjdTT + offset) / 36525.0;
      
      if (overrideOriginInterval) updateIntervalToUse = 
                                  ((Planet*) origin)->GetUpdateInterval();
      else                        updateIntervalToUse = updateInterval;
      #ifdef DEBUG_EQUATOR_AXES
         MessageInterface::ShowMessage(
            "In Equator::Calculate tTDB = %.12f and updateIntervalToUse = %.12f\n",
            tTDB, updateIntervalToUse);
      #endif
      
      ComputePrecessionMatrix(tTDB, atEpoch);
      ComputeNutationMatrix(tTDB, atEpoch, dPsi, longAscNodeLunar, cosEpsbar,
                            forceComputation);
      
      Real PrecT[9] = {precData[0], precData[3], precData[6],
                       precData[1], precData[4], precData[7],
                       precData[2], precData[5], precData[8]};
      Real NutT[9] =  {nutData[0], nutData[3], nutData[6],
                       nutData[1], nutData[4], nutData[7],
                       nutData[2], nutData[5], nutData[8]};

      #ifdef DEBUG_EQUATOR_AXES
         MessageInterface::ShowMessage(
            "In Equator::Calculate precData = \n%.12f %.12f %.12f\n%.12f %.12f %.12f\n%.12f %.12f %.12f\n",
            precData[0],precData[1],precData[2],precData[3],precData[4],
            precData[5],precData[6],precData[7],precData[8]);
         MessageInterface::ShowMessage(
            "In Equator::Calculate nutData = \n%.12f %.12f %.12f\n%.12f %.12f %.12f\n%.12f %.12f %.12f\n",
            nutData[0],nutData[1],nutData[2],nutData[3],nutData[4],
            nutData[5],nutData[6],nutData[7],nutData[8]);
      #endif
   
      Integer p3 = 0;
      Real res[3][3];
      for (Integer p = 0; p < 3; ++p)
      {
         p3 = 3*p;
         for (Integer q = 0; q < 3; ++q)
         {
            res[p][q] = PrecT[p3]   * NutT[q]   + 
                        PrecT[p3+1] * NutT[q+3] + 
                        PrecT[p3+2] * NutT[q+6];
         }
      }     
      rotMatrix.Set(res[0][0],res[0][1],res[0][2],
                    res[1][0],res[1][1],res[1][2],
                    res[2][0],res[2][1],res[2][2]); 
      
      #ifdef DEBUG_EQUATOR_AXES
         MessageInterface::ShowMessage(
            "In Equator::rotMatrix is set to = \n%.12f %.12f %.12f\n%.12f %.12f %.12f\n%.12f %.12f %.12f\n",
            rotMatrix(0,0),rotMatrix(0,1),rotMatrix(0,2),
            rotMatrix(1,0),rotMatrix(1,1),rotMatrix(1,2),
            rotMatrix(2,0),rotMatrix(2,1),rotMatrix(2,2));
      #endif
      // rotDotMatrix is still the default zero matrix 
      // (assume it is negligibly small)
      
   }
   else if ((originName == SolarSystem::MOON_NAME) && 
           (((CelestialBody*)origin)->GetRotationDataSource() == Gmat::DE_FILE))
   {
      if (!de)
      {
         de = (DeFile*) ((CelestialBody*)origin)->GetSourceFile();
         if (!de)
         throw CoordinateSystemException(
               "No DE file specified - cannot get Moon data");
         // should we automatically switch to IAU data here?
         // De file is initialized in its constructor
      }
      bool override = ((CelestialBody*)origin)->GetOverrideTimeSystem();
      Real librationAngles[3], andRates[3];
      de->GetAnglesAndRates(atEpoch, librationAngles, andRates, override);
      rotMatrix    = (Attitude::ToCosineMatrix(librationAngles, 3, 1, 3)).Transpose();
      Real ca1    = cos(librationAngles[0]);
      Real ca2    = cos(librationAngles[1]);
      Real sa1    = sin(librationAngles[0]);
      Real sa2    = sin(librationAngles[1]);
      Real s2s1   = sa2*sa1;
      Real c2c1   = ca2*ca1;
      Real s2c1   = sa2*ca1;
      Real c2s1   = ca2*sa1;
      
      rotDotMatrix.Set(
         -andRates[0]*sa1, 
          andRates[0]*ca1, 
                      0.0,
          andRates[1]*s2s1 - andRates[0]*c2c1, 
         -andRates[1]*s2c1 - andRates[0]*c2s1,
          andRates[1]*ca2,
          andRates[1]*c2s1 + andRates[0]*s2c1,
         -andRates[1]*c2c1 + andRates[0]*s2s1,
         -andRates[1]*sa2
         );
    }
   else  // use IAU data for all other bodies, and Luna (if DE file not selected)
   {
      // this method will return alpha (deg), delta (deg), 
      // W (deg), and Wdot (deg/day)
      static Rvector cartCoord(4);  
      const Real *cartC = cartCoord.GetDataVector();
      // this method will return alpha (deg), delta (deg), 
      // W (deg), and Wdot (deg/day)
      cartCoord   = ((CelestialBody*)origin)->
                      GetBodyCartographicCoordinates(atEpoch);
      
      Real rot1           = GmatMathUtil::PI_OVER_TWO + Rad(cartC[0]);
      Real rot2           = GmatMathUtil::PI_OVER_TWO - Rad(cartC[1]);
      // Don't care about W and Wdot here
      Real R3leftT[9] =  {Cos(rot1),-Sin(rot1),0.0,
                          Sin(rot1), Cos(rot1),0.0,
                                0.0,       0.0,1.0};
      //Rmatrix33 R3left( Cos(rot1),  Sin(rot1),  0.0, 
      //                 -Sin(rot1),  Cos(rot1),  0.0,
      //                        0.0,        0.0,  1.0);
      Real R1middleT[9] =  {1.0,      0.0,       0.0,
                            0.0,Cos(rot2),-Sin(rot2),
                            0.0,Sin(rot2), Cos(rot2)};
      //Rmatrix33 R1middle(     1.0,        0.0,       0.0,
      //                        0.0,  Cos(rot2), Sin(rot2),
      //                        0.0, -Sin(rot2), Cos(rot2));
      Integer p3 = 0;
      Real    rot[3][3];
      for (Integer p = 0; p < 3; ++p)
      {
         p3 = 3*p;
         for (Integer q = 0; q < 3; ++q)
         {
            rot[p][q] = R3leftT[p3]   * R1middleT[q]   + 
                        R3leftT[p3+1] * R1middleT[q+3] + 
                        R3leftT[p3+2] * R1middleT[q+6];
         }
      }   
      rotMatrix.Set(rot[0][0], rot[0][1], rot[0][2],
                    rot[1][0], rot[1][1], rot[1][2],
                    rot[2][0], rot[2][1], rot[2][2]);  
      //rotMatrix = R3left.Transpose() * R1middle.Transpose();
      //rotDotmatrix remains the zero matrix
   }
}


