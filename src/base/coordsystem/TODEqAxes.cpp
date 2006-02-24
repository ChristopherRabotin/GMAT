//$Header$
//------------------------------------------------------------------------------
//                                  TODEqAxes
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
 * Implementation of the TODEqAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Planet.hpp"
#include "TODEqAxes.hpp"
#include "TrueOfDateAxes.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"


//---------------------------------
// static data
//---------------------------------

/* placeholder - may be needed later
const std::string
TODEqAxes::PARAMETER_TEXT[TODEqAxesParamCount - TrueOfDateAxesParamCount] =
{
   "",
};

const Gmat::ParameterType
TODEqAxes::PARAMETER_TYPE[TODEqAxesParamCount - TrueOfDateAxesParamCount] =
{
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  TODEqAxes(const std::string &itsType,
//            const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base TODEqAxes structures
 * (default constructor).
 *
 * @param <itsType> GMAT script string associated with this type of object.
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
TODEqAxes::TODEqAxes(const std::string &itsName) :
TrueOfDateAxes("TODEq",itsName)
{
   objectTypeNames.push_back("TODEqAxes");
   parameterCount = TODEqAxesParamCount;
}

//---------------------------------------------------------------------------
//  TODEqAxes(const TODEqAxes &tod);
//---------------------------------------------------------------------------
/**
 * Constructs base TODEqAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param tod  TODEqAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
TODEqAxes::TODEqAxes(const TODEqAxes &tod) :
TrueOfDateAxes(tod)
{
}

//---------------------------------------------------------------------------
//  TODEqAxes& operator=(const TODEqAxes &tod)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TODEqAxes structures.
 *
 * @param tod The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const TODEqAxes& TODEqAxes::operator=(const TODEqAxes &tod)
{
   if (&tod == this)
      return *this;
   TrueOfDateAxes::operator=(tod);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~TODEqAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
TODEqAxes::~TODEqAxes()
{
}

//---------------------------------------------------------------------------
//  bool TODEqAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this TODEqAxes.
 *
 */
//---------------------------------------------------------------------------
bool TODEqAxes::Initialize()
{
   TrueOfDateAxes::Initialize();
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
 * This method returns a clone of the TODEqAxes.
 *
 * @return clone of the TODEqAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* TODEqAxes::Clone() const
{
   return (new TODEqAxes(*this));
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
/*std::string TODEqAxes::GetParameterText(const Integer id) const
{
   if (id >= TrueOfDateAxesParamCount && id < TODEqAxesParamCount)
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
/*Integer TODEqAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = TrueOfDateAxesParamCount; i < TODEqAxesParamCount; i++)
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
/*Gmat::ParameterType TODEqAxes::GetParameterType(const Integer id) const
{
   if (id >= TrueOfDateAxesParamCount && id < TODEqAxesParamCount)
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
/*std::string TODEqAxes::GetParameterTypeString(const Integer id) const
{
   return TrueOfDateAxes::PARAM_TYPE_STRING[GetParameterType(id)];
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
void TODEqAxes::CalculateRotationMatrix(const A1Mjd &atEpoch)
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
   Real jdTT  = mjdTT + GmatTimeUtil::JD_JAN_5_1941;
   // Compute Julian centuries of TDB from the base epoch (J2000) 
   Real tTDB  = (jdTT - 2451545.0) / 36525.0;
   
   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetUpdateInterval();
   else                        updateIntervalToUse = updateInterval;
//   Rmatrix33  PREC      = ComputePrecessionMatrix(tTDB, atEpoch);
//   Rmatrix33  NUT       = ComputeNutationMatrix(tTDB, atEpoch, dPsi,
//                          longAscNodeLunar, cosEpsbar);
   
   ComputePrecessionMatrix(tTDB, atEpoch);
   ComputeNutationMatrix(tTDB, atEpoch, dPsi, longAscNodeLunar, cosEpsbar);
   
   rotMatrix = PREC.Transpose() * NUT.Transpose();
    
   // rotDotMatrix is still the default zero matrix 
   // (assume it is negligibly small)
}
