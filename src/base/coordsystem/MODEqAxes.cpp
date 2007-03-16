//$Header$
//------------------------------------------------------------------------------
//                                  MODEqAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/05/03
//
/**
 * Implementation of the MODEqAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Planet.hpp"
#include "MODEqAxes.hpp"
#include "MeanOfDateAxes.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"


//---------------------------------
// static data
//---------------------------------

/* placeholder - may be needed later
const std::string
MODEqAxes::PARAMETER_TEXT[MODEqAxesParamCount - MeanOfDateAxesParamCount] =
{
   "",
};

const Gmat::ParameterType
MODEqAxes::PARAMETER_TYPE[MODEqAxesParamCount - MeanOfDateAxesParamCount] =
{
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  MODEqAxes(const std::string &itsType,
//            const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base MODEqAxes structures
 * (default constructor).
 *
 * @param <itsType> GMAT script string associated with this type of object.
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
MODEqAxes::MODEqAxes(const std::string &itsName) :
MeanOfDateAxes("MODEq",itsName)
{
   objectTypeNames.push_back("MODEqAxes");
   parameterCount = MODEqAxesParamCount;
}

//---------------------------------------------------------------------------
//  MODEqAxes(const MODEqAxes &mod);
//---------------------------------------------------------------------------
/**
 * Constructs base MODEqAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param mod  MODEqAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
MODEqAxes::MODEqAxes(const MODEqAxes &mod) :
MeanOfDateAxes(mod)
{
}

//---------------------------------------------------------------------------
//  MODEqAxes& operator=(const MODEqAxes &mod)
//---------------------------------------------------------------------------
/**
 * Assignment operator for MODEqAxes structures.
 *
 * @param mod The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const MODEqAxes& MODEqAxes::operator=(const MODEqAxes &mod)
{
   if (&mod == this)
      return *this;
   MeanOfDateAxes::operator=(mod);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~MODEqAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
MODEqAxes::~MODEqAxes()
{
}

//---------------------------------------------------------------------------
//  bool MODEqAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this MODEqAxes.
 *
 */
//---------------------------------------------------------------------------
bool MODEqAxes::Initialize()
{
   MeanOfDateAxes::Initialize();
   //InitializeFK5(); // wcs - not needed just for precession
   return true;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MODEqAxes.
 *
 * @return clone of the MODEqAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MODEqAxes::Clone() const
{
   return (new MODEqAxes(*this));
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
/*std::string MODEqAxes::GetParameterText(const Integer id) const
{
   if (id >= MeanOfDateAxesParamCount && id < MODEqAxesParamCount)
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
/*Integer MODEqAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = MeanOfDateAxesParamCount; i < MODEqAxesParamCount; i++)
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
/*Gmat::ParameterType MODEqAxes::GetParameterType(const Integer id) const
{
   if (id >= MeanOfDateAxesParamCount && id < MODEqAxesParamCount)
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
/*std::string MODEqAxes::GetParameterTypeString(const Integer id) const
{
   return MeanOfDateAxes::PARAM_TYPE_STRING[GetParameterType(id)];
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
void MODEqAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation)
{
   
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
   Real tTDB   = (mjdTT + offset) / 36525.0;
   
   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetNutationUpdateInterval();
   else                        updateIntervalToUse = updateInterval;
//   Rmatrix33  PREC      = ComputePrecessionMatrix(tTDB, atEpoch);
   ComputePrecessionMatrix(tTDB, atEpoch);
   
   //rotMatrix = PREC.Transpose();
   rotMatrix.Set(precData[0], precData[3], precData[6],
                 precData[1], precData[4], precData[7],
                 precData[2], precData[5], precData[8]);
                 
   // rotDotMatrix is still the default zero matrix
   // (assume it is negligibly small)
   
}
