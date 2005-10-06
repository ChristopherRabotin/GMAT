//$Header$
//------------------------------------------------------------------------------
//                                  TOEEcAxes
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
 * Implementation of the TOEEcAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "TOEEcAxes.hpp"
#include "InertialAxes.hpp"
#include "TimeTypes.hpp"
#include "RealUtilities.hpp"
#include "TimeSystemConverter.hpp"


//---------------------------------
// static data
//---------------------------------

/* placeholder - may be needed later
const std::string
TOEEcAxes::PARAMETER_TEXT[TOEEcAxesParamCount - InertialAxesParamCount] =
{
   "",
};

const Gmat::ParameterType
TOEEcAxes::PARAMETER_TYPE[TOEEcAxesParamCount - InertialAxesParamCount] =
{
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  TOEEcAxes(const std::string &itsType,
//            const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base TOEEcAxes structures
 * (default constructor).
 *
 * @param <itsType> GMAT script string associated with this type of object.
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
TOEEcAxes::TOEEcAxes(const std::string &itsName) :
InertialAxes("TOEEc",itsName)
{
   objectTypeNames.push_back("TOEEcAxes");
   parameterCount = TOEEcAxesParamCount;
}

//---------------------------------------------------------------------------
//  TOEEcAxes(const TOEEcAxes &toe);
//---------------------------------------------------------------------------
/**
 * Constructs base TOEEcAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param toe  TOEEcAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
TOEEcAxes::TOEEcAxes(const TOEEcAxes &toe) :
InertialAxes(toe)
{
}

//---------------------------------------------------------------------------
//  TOEEcAxes& operator=(const TOEEcAxes &toe)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TOEEcAxes structures.
 *
 * @param toe The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const TOEEcAxes& TOEEcAxes::operator=(const TOEEcAxes &toe)
{
   if (&toe == this)
      return *this;
   InertialAxes::operator=(toe);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~TOEEcAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
TOEEcAxes::~TOEEcAxes()
{
}

//---------------------------------------------------------------------------
//  bool TOEEcAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this TOEEcAxes.
 *
 */
//---------------------------------------------------------------------------
bool TOEEcAxes::Initialize()
{
   InertialAxes::Initialize();
   InitializeFK5();
   
   Real dPsi             = 0.0;
   Real longAscNodeLunar = 0.0;
   Real cosEpsbar        = 0.0;

   // convert epoch (A1 MJD) to TT MJD (for calculations)
   Real mjdTT = TimeConverterUtil::Convert(epoch.Get(),
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
   
   
   Rmatrix33  PREC      = ComputePrecessionMatrix(tTDB, epoch);
   Rmatrix33  NUT       = ComputeNutationMatrix(tTDB, epoch, dPsi,
                                                longAscNodeLunar, cosEpsbar);

   Rmatrix33 R3Psi( GmatMathUtil::Cos(-dPsi),  GmatMathUtil::Sin(-dPsi),  0.0, 
                    -GmatMathUtil::Sin(-dPsi),  GmatMathUtil::Cos(-dPsi),  0.0,
                    0.0,                       0.0,  1.0);
   
   
   rotMatrix = PREC.Transpose() * R1Eps.Transpose() * R3Psi.Transpose();

   // rotDotMatrix is still the default zero matrix
   
   return true;
}


GmatCoordinate::ParameterUsage TOEEcAxes::UsesEpoch() const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage TOEEcAxes::UsesEopFile() const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage TOEEcAxes::UsesItrfFile() const
{
   return GmatCoordinate::REQUIRED;
}



//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the TOEEcAxes.
 *
 * @return clone of the TOEEcAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* TOEEcAxes::Clone() const
{
   return (new TOEEcAxes(*this));
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
/*std::string TOEEcAxes::GetParameterText(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < TOEEcAxesParamCount)
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
/*Integer TOEEcAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = InertialAxesParamCount; i < TOEEcAxesParamCount; i++)
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
/*Gmat::ParameterType TOEEcAxes::GetParameterType(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < TOEEcAxesParamCount)
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
/*std::string TOEEcAxes::GetParameterTypeString(const Integer id) const
{
   return InertialAxes::PARAM_TYPE_STRING[GetParameterType(id)];
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
void TOEEcAxes::CalculateRotationMatrix(const A1Mjd &atEpoch)
{
   // already computed in Initialize
}
