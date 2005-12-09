//$Header$
//------------------------------------------------------------------------------
//                                  MODEcAxes
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
 * Implementation of the MODEcAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Planet.hpp"
#include "MODEcAxes.hpp"
#include "MeanOfDateAxes.hpp"
#include "TimeTypes.hpp"
#include "RealUtilities.hpp"
#include "TimeSystemConverter.hpp"

//---------------------------------
// static data
//---------------------------------

/* placeholder - may be needed later
const std::string
MODEcAxes::PARAMETER_TEXT[MODEcAxesParamCount - MeanOfDateAxesParamCount] =
{
   "",
};

const Gmat::ParameterType
MODEcAxes::PARAMETER_TYPE[MODEcAxesParamCount - MeanOfDateAxesParamCount] =
{
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  MODEcAxes(const std::string &itsType,
//            const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base MODEcAxes structures
 * (default constructor).
 *
 * @param <itsType> GMAT script string associated with this type of object.
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
MODEcAxes::MODEcAxes(const std::string &itsName) :
MeanOfDateAxes("MODEc",itsName)
{
   objectTypeNames.push_back("MODEcAxes");
   parameterCount = MODEcAxesParamCount;
}

//---------------------------------------------------------------------------
//  MODEcAxes(const MODEcAxes &mod);
//---------------------------------------------------------------------------
/**
 * Constructs base MODEcAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param mod  MODEcAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
MODEcAxes::MODEcAxes(const MODEcAxes &mod) :
MeanOfDateAxes(mod)
{
}

//---------------------------------------------------------------------------
//  MODEcAxes& operator=(const MODEcAxes &mod)
//---------------------------------------------------------------------------
/**
 * Assignment operator for MODEcAxes structures.
 *
 * @param mod The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const MODEcAxes& MODEcAxes::operator=(const MODEcAxes &mod)
{
   if (&mod == this)
      return *this;
   MeanOfDateAxes::operator=(mod);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~MODEcAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
MODEcAxes::~MODEcAxes()
{
}

//---------------------------------------------------------------------------
//  bool MODEcAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this MODEcAxes.
 *
 */
//---------------------------------------------------------------------------
bool MODEcAxes::Initialize()
{
   MeanOfDateAxes::Initialize();
   InitializeFK5();
   return true;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MODEcAxes.
 *
 * @return clone of the MODEcAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MODEcAxes::Clone() const
{
   return (new MODEcAxes(*this));
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
/*std::string MODEcAxes::GetParameterText(const Integer id) const
{
   if (id >= MeanOfDateAxesParamCount && id < MODEcAxesParamCount)
      return PARAMETER_TEXT[id - MeanOfDateAxesParamCount];
   return MeanOfDateAxes::GetParameterText(id);
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
/*Integer MODEcAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = MeanOfDateAxesParamCount; i < MODEcAxesParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - MeanOfDateAxesParamCount])
         return i;
   }
   
   return MeanOfDateAxes::GetParameterID(str);
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
/*Gmat::ParameterType MODEcAxes::GetParameterType(const Integer id) const
{
   if (id >= MeanOfDateAxesParamCount && id < MODEcAxesParamCount)
      return PARAMETER_TYPE[id - MeanOfDateAxesParamCount];
   
   return MeanOfDateAxes::GetParameterType(id);
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
/*std::string MODEcAxes::GetParameterTypeString(const Integer id) const
{
   return MeanOfDateAxes::PARAM_TYPE_STRING[GetParameterType(id)];
}
*/


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch)
//---------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000EqAxes system.
 *
 * @param atEpoch  epoch at which to compute the rotation matrix
 */
//---------------------------------------------------------------------------
void MODEcAxes::CalculateRotationMatrix(const A1Mjd &atEpoch)
{
   
   // convert epoch (A1 MJD) to TT MJD (for calculations)
   Real mjdTT = TimeConverterUtil::Convert(atEpoch.Get(),
                "A1Mjd", "TtMjd", GmatTimeUtil::JD_JAN_5_1941);      
   Real jdTT  = mjdTT + GmatTimeUtil::JD_JAN_5_1941;
   // Compute Julian centuries of TDB from the base epoch (J2000) 
   Real tTDB  = (jdTT - 2451545.0) / 36525.0;
   Real tTDB2 = tTDB * tTDB;
   Real tTDB3 = tTDB * tTDB2;
   
   // Vallado Eq. 3-52
   Real Epsbar  = (84381.448 - 46.8150*tTDB - 0.00059*tTDB2 + 0.001813*tTDB3)
      * GmatMathUtil::RAD_PER_ARCSEC;

   Rmatrix33 R1Eps( 1.0,                        0.0,                       0.0,
                    0.0,  GmatMathUtil::Cos(Epsbar), GmatMathUtil::Sin(Epsbar),
                    0.0, -GmatMathUtil::Sin(Epsbar), GmatMathUtil::Cos(Epsbar));
   
   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetUpdateInterval();
   else                        updateIntervalToUse = updateInterval;
//   Rmatrix33  PREC      = ComputePrecessionMatrix(tTDB, atEpoch);
   ComputePrecessionMatrix(tTDB, atEpoch);
   
   rotMatrix = PREC.Transpose() * R1Eps.Transpose();
   // rotDotMatrix is still the default zero matrix
   // (assume it is negligibly small)
   
}
