//$Header$
//------------------------------------------------------------------------------
//                                  TODEcAxes
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
 * Implementation of the TODEcAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Planet.hpp"
#include "TODEcAxes.hpp"
#include "TrueOfDateAxes.hpp"
#include "TimeTypes.hpp"
#include "RealUtilities.hpp"
#include "TimeSystemConverter.hpp"


//---------------------------------
// static data
//---------------------------------

/* placeholder - may be needed later
const std::string
TODEcAxes::PARAMETER_TEXT[TODEcAxesParamCount - TrueOfDateAxesParamCount] =
{
   "",
};

const Gmat::ParameterType
TODEcAxes::PARAMETER_TYPE[TODEcAxesParamCount - TrueOfDateAxesParamCount] =
{
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  TODEcAxes(const std::string &itsType,
//            const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base TODEcAxes structures
 * (default constructor).
 *
 * @param <itsType> GMAT script string associated with this type of object.
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
TODEcAxes::TODEcAxes(const std::string &itsName) :
TrueOfDateAxes("TODEc",itsName)
{
   objectTypeNames.push_back("TODEcAxes");
   parameterCount = TODEcAxesParamCount;
}

//---------------------------------------------------------------------------
//  TODEcAxes(const TODEcAxes &tod);
//---------------------------------------------------------------------------
/**
 * Constructs base TODEcAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param tod  TODEcAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
TODEcAxes::TODEcAxes(const TODEcAxes &tod) :
TrueOfDateAxes(tod)
{
}

//---------------------------------------------------------------------------
//  TODEcAxes& operator=(const TODEcAxes &tod)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TODEcAxes structures.
 *
 * @param tod The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const TODEcAxes& TODEcAxes::operator=(const TODEcAxes &tod)
{
   if (&tod == this)
      return *this;
   TrueOfDateAxes::operator=(tod);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~TODEcAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
TODEcAxes::~TODEcAxes()
{
}

//---------------------------------------------------------------------------
//  bool TODEcAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this TODEcAxes.
 *
 */
//---------------------------------------------------------------------------
bool TODEcAxes::Initialize()
{
   TrueOfDateAxes::Initialize();
   //InitializeFK5();   //wcs - moved to TrueOfDateAxes
   
   return true;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the TODEcAxes.
 *
 * @return clone of the TODEcAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* TODEcAxes::Clone() const
{
   return (new TODEcAxes(*this));
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
/*std::string TODEcAxes::GetParameterText(const Integer id) const
{
   if (id >= TrueOfDateAxesParamCount && id < TODEcAxesParamCount)
      return PARAMETER_TEXT[id - TrueOfDateAxesParamCount];
   return TrueOfDateAxes::GetParameterText(id);
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
/*Integer TODEcAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = TrueOfDateAxesParamCount; i < TODEcAxesParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - TrueOfDateAxesParamCount])
         return i;
   }
   
   return TrueOfDateAxes::GetParameterID(str);
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
/*Gmat::ParameterType TODEcAxes::GetParameterType(const Integer id) const
{
   if (id >= TrueOfDateAxesParamCount && id < TODEcAxesParamCount)
      return PARAMETER_TYPE[id - TrueOfDateAxesParamCount];
   
   return TrueOfDateAxes::GetParameterType(id);
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
/*std::string TODEcAxes::GetParameterTypeString(const Integer id) const
{
   return TrueOfDateAxes::PARAM_TYPE_STRING[GetParameterType(id)];
}
*/


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation)
//---------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000EqAxes system.
 *
 * @param atEpoch  epoch at which to compute the rotation matrix
 */
//---------------------------------------------------------------------------
void TODEcAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation)
{
   Real dPsi             = 0.0;
   Real longAscNodeLunar = 0.0;
   Real cosEpsbar        = 0.0;
   
   // convert epoch (A1 MJD) to TT MJD (for calculations)
   // 20.02.06 - arg: changed to use enum types instead of strings
//   Real mjdTT = TimeConverterUtil::Convert(atEpoch.Get(),
//                "A1Mjd", "TtMjd", GmatTimeUtil::JD_JAN_5_1941);      
   Real mjdTT = TimeConverterUtil::Convert(atEpoch.Get(),
                TimeConverterUtil::A1MJD, TimeConverterUtil::TTMJD, 
                GmatTimeUtil::JD_JAN_5_1941);      
   //Real jdTT  = mjdTT + GmatTimeUtil::JD_JAN_5_1941;
   // Compute Julian centuries of TDB from the base epoch (J2000) 
   //Real tTDB  = (jdTT - 2451545.0) / 36525.0;
   Real offset = GmatTimeUtil::JD_JAN_5_1941 - 2451545.0;
   Real tTDB  = (mjdTT + offset) / 36525.0;
   Real tTDB2 = tTDB * tTDB;
   Real tTDB3 = tTDB * tTDB2;
   
   // Vallado Eq. 3-52
   Real Epsbar  = (84381.448 - 46.8150*tTDB - 0.00059*tTDB2 + 0.001813*tTDB3)
      * GmatMathUtil::RAD_PER_ARCSEC;       
   Real R1EpsT[9] = { 1.0,                      0.0,                       0.0,
                      0.0,GmatMathUtil::Cos(Epsbar),-GmatMathUtil::Sin(Epsbar),
                      0.0,GmatMathUtil::Sin(Epsbar), GmatMathUtil::Cos(Epsbar)};
   //Rmatrix33 R1Eps( 1.0,                        0.0,                       0.0,
   //                 0.0,  GmatMathUtil::Cos(Epsbar), GmatMathUtil::Sin(Epsbar),
   //                 0.0, -GmatMathUtil::Sin(Epsbar), GmatMathUtil::Cos(Epsbar));
      
   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetNutationUpdateInterval();
   else                        updateIntervalToUse = updateInterval;
//   Rmatrix33  PREC      = ComputePrecessionMatrix(tTDB, atEpoch);
//   Rmatrix33  NUT       = ComputeNutationMatrix(tTDB, atEpoch, dPsi,
//                          longAscNodeLunar, cosEpsbar);
//   
   ComputePrecessionMatrix(tTDB, atEpoch);
   ComputeNutationMatrix(tTDB, atEpoch, dPsi, longAscNodeLunar, cosEpsbar,
                         forceComputation);
   
   Real R3PsiT[3][3] = { { GmatMathUtil::Cos(-dPsi),-GmatMathUtil::Sin(-dPsi), 0.0},
                         { GmatMathUtil::Sin(-dPsi), GmatMathUtil::Cos(-dPsi), 0.0},
                         {                      0.0,                     0.0,  1.0}};
   //Rmatrix33 R3Psi( GmatMathUtil::Cos(-dPsi),  GmatMathUtil::Sin(-dPsi),  0.0, 
   //                -GmatMathUtil::Sin(-dPsi),  GmatMathUtil::Cos(-dPsi),  0.0,
   //                                      0.0,                       0.0,  1.0);
   
   //rotMatrix = PREC.Transpose() * R1Eps.Transpose() * R3Psi.Transpose();
   Real PrecT[9] = {precData[0], precData[3], precData[6],
                    precData[1], precData[4], precData[7],
                    precData[2], precData[5], precData[8]};
   Real res[3][3], tmp[3][3];
   Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      for (Integer q = 0; q < 3; ++q)
      {
         tmp[p][q] = R1EpsT[p3]   * R3PsiT[0][q] + 
                     R1EpsT[p3+1] * R3PsiT[1][q] + 
                     R1EpsT[p3+2] * R3PsiT[2][q];
      }
   }     
    for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      for (Integer q = 0; q < 3; ++q)
      {
         res[p][q] = PrecT[p3]   * tmp[0][q] + 
                     PrecT[p3+1] * tmp[1][q] + 
                     PrecT[p3+2] * tmp[2][q];
      }
   }     
   rotMatrix.Set(res[0][0],res[0][1],res[0][2],
                 res[1][0],res[1][1],res[1][2],
                 res[2][0],res[2][1],res[2][2]); 
    
   
   // rotDotMatrix is still the default zero matrix 
   // (assume it is negligibly small)
}
