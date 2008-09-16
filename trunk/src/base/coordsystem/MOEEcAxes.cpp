//$Header$
//------------------------------------------------------------------------------
//                                  MOEEcAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/05/11
//
/**
 * Implementation of the MOEEcAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Planet.hpp"
#include "MOEEcAxes.hpp"
#include "InertialAxes.hpp"
#include "TimeTypes.hpp"
#include "RealUtilities.hpp"
#include "TimeSystemConverter.hpp"


//---------------------------------
// static data
//---------------------------------

/* placeholder - may be needed later
const std::string
MOEEcAxes::PARAMETER_TEXT[MOEEcAxesParamCount - InertialAxesParamCount] =
{
   "",
};

const Gmat::ParameterType
MOEEcAxes::PARAMETER_TYPE[MOEEcAxesParamCount - InertialAxesParamCount] =
{
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  MOEEcAxes(const std::string &itsType,
//            const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base MOEEcAxes structures
 * (default constructor).
 *
 * @param <itsType> GMAT script string associated with this type of object.
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
MOEEcAxes::MOEEcAxes(const std::string &itsName) :
InertialAxes("MOEEc",itsName)
{
   objectTypeNames.push_back("MOEEcAxes");
   parameterCount = MOEEcAxesParamCount;
}

//---------------------------------------------------------------------------
//  MOEEcAxes(const MOEEcAxes &moe);
//---------------------------------------------------------------------------
/**
 * Constructs base MOEEcAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param moe  MOEEcAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
MOEEcAxes::MOEEcAxes(const MOEEcAxes &moe) :
InertialAxes(moe)
{
}

//---------------------------------------------------------------------------
//  MOEEcAxes& operator=(const MOEEcAxes &moe)
//---------------------------------------------------------------------------
/**
 * Assignment operator for MOEEcAxes structures.
 *
 * @param moe The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const MOEEcAxes& MOEEcAxes::operator=(const MOEEcAxes &moe)
{
   if (&moe == this)
      return *this;
   InertialAxes::operator=(moe);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~MOEEcAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
MOEEcAxes::~MOEEcAxes()
{
}

//---------------------------------------------------------------------------
//  bool MOEEcAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this MOEEcAxes.
 *
 */
//---------------------------------------------------------------------------
bool MOEEcAxes::Initialize()
{
   InertialAxes::Initialize();
   //InitializeFK5(); // wcs - not needed just for precession

   // convert epoch (A1 MJD) to TT MJD (for calculations)
   // 20.02.06 - arg: changed to use enum types instead of strings
//   Real mjdTT = TimeConverterUtil::Convert(epoch.Get(),
//                 "A1Mjd", "TtMjd", GmatTimeUtil::JD_JAN_5_1941);      
   Real mjdTT = TimeConverterUtil::Convert(epoch.Get(),
                 TimeConverterUtil::A1MJD, TimeConverterUtil::TTMJD, 
                 GmatTimeUtil::JD_JAN_5_1941);      
   //Real jdTT  = mjdTT + GmatTimeUtil::JD_JAN_5_1941;
   // Compute Julian centuries of TDB from the base epoch (J2000) 
   //Real tTDB  = (jdTT - 2451545.0) / 36525.0;
   Real offset = GmatTimeUtil::JD_JAN_5_1941 - 2451545.0;
   Real tTDB  = (mjdTT +offset) / 36525.0;
   Real tTDB2 = tTDB * tTDB;
   Real tTDB3 = tTDB * tTDB2;
   
   // Vallado Eq. 3-52
   Real Epsbar  = (84381.448 - 46.8150*tTDB - 0.00059*tTDB2 + 0.001813*tTDB3)
      * GmatMathUtil::RAD_PER_ARCSEC;
   
   Real R1EpsT[3][3] = { {1.0,                      0.0,                       0.0},
                         {0.0,GmatMathUtil::Cos(Epsbar),-GmatMathUtil::Sin(Epsbar)},
                         {0.0,GmatMathUtil::Sin(Epsbar), GmatMathUtil::Cos(Epsbar)}};
   //Rmatrix33 R1Eps( 1.0,                        0.0,                       0.0,
   //                 0.0,  GmatMathUtil::Cos(Epsbar), GmatMathUtil::Sin(Epsbar),
   //                 0.0, -GmatMathUtil::Sin(Epsbar), GmatMathUtil::Cos(Epsbar));
   
   
   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetNutationUpdateInterval();
   else                        updateIntervalToUse = updateInterval;
//   Rmatrix33  PREC      = ComputePrecessionMatrix(tTDB, epoch);
   ComputePrecessionMatrix(tTDB, epoch);
   
   Real PrecT[9] = {precData[0], precData[3], precData[6],
                    precData[1], precData[4], precData[7],
                    precData[2], precData[5], precData[8]};
   
   //rotMatrix = PREC.Transpose() * R1Eps.Transpose();
   Real res[3][3];
   Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      for (Integer q = 0; q < 3; ++q)
      {
         res[p][q] = PrecT[p3]   * R1EpsT[0][q] + 
                     PrecT[p3+1] * R1EpsT[1][q] + 
                     PrecT[p3+2] * R1EpsT[2][q];
      }
   }     
   
   rotMatrix.Set(res[0][0],res[0][1],res[0][2],
                 res[1][0],res[1][1],res[1][2],
                 res[2][0],res[2][1],res[2][2]); 
   //rotMatrix = PREC.Transpose() * R1Eps.Transpose();

   // rotDotMatrix is still the default zero matrix
   
   return true;
}


GmatCoordinate::ParameterUsage MOEEcAxes::UsesEpoch() const
{
   return GmatCoordinate::REQUIRED;
}

//GmatCoordinate::ParameterUsage MOEEcAxes::UsesEopFile() const
//{
//   return GmatCoordinate::REQUIRED;
//}

//GmatCoordinate::ParameterUsage MOEEcAxes::UsesItrfFile() const
//{
//   return GmatCoordinate::REQUIRED;
//}



//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MOEEcAxes.
 *
 * @return clone of the MOEEcAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MOEEcAxes::Clone() const
{
   return (new MOEEcAxes(*this));
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
/*std::string MOEEcAxes::GetParameterText(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < MOEEcAxesParamCount)
      return PARAMETER_TEXT[id - InertialAxesParamCount];
   return InertialAxes::GetParameterText(id);
}
*/
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
/*Integer MOEEcAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = InertialAxesParamCount; i < MOEEcAxesParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - InertialAxesParamCount])
         return i;
   }
   
   return InertialAxes::GetParameterID(str);
}
*/
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
/*Gmat::ParameterType MOEEcAxes::GetParameterType(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < MOEEcAxesParamCount)
      return PARAMETER_TYPE[id - InertialAxesParamCount];
   
   return InertialAxes::GetParameterType(id);
}
*/
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
/*std::string MOEEcAxes::GetParameterTypeString(const Integer id) const
{
   return InertialAxes::PARAM_TYPE_STRING[GetParameterType(id)];
}
*/


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation = false)
//---------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000EqAxes system.
 *
 * @param atEpoch  epoch at which to compute the rotation matrix
 */
//---------------------------------------------------------------------------
void MOEEcAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation)
{
   // already computed in Initialize
}
