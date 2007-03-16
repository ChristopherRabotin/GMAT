//$Header$
//------------------------------------------------------------------------------
//                                  TOEEqAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/04/27
//
/**
 * Implementation of the TOEEqAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Planet.hpp"
#include "TOEEqAxes.hpp"
#include "InertialAxes.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"


//#define DEBUG_TOEEQ

#ifdef DEBUG_TOEEQ
   #include "MessageInterface.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

/* placeholder - may be needed later
const std::string
TOEEqAxes::PARAMETER_TEXT[TOEEqAxesParamCount - InertialAxesParamCount] =
{
   "",
};

const Gmat::ParameterType
TOEEqAxes::PARAMETER_TYPE[TOEEqAxesParamCount - InertialAxesParamCount] =
{
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  TOEEqAxes(const std::string &itsType,
//            const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base TOEEqAxes structures
 * (default constructor).
 *
 * @param <itsType> GMAT script string associated with this type of object.
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
TOEEqAxes::TOEEqAxes(const std::string &itsName) :
InertialAxes("TOEEq",itsName)
{
   objectTypeNames.push_back("TOEEqAxes");
   parameterCount = TOEEqAxesParamCount;
}

//---------------------------------------------------------------------------
//  TOEEqAxes(const TOEEqAxes &toe);
//---------------------------------------------------------------------------
/**
 * Constructs base TOEEqAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param toe  TOEEqAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
TOEEqAxes::TOEEqAxes(const TOEEqAxes &toe) :
InertialAxes(toe)
{
}

//---------------------------------------------------------------------------
//  TOEEqAxes& operator=(const TOEEqAxes &toe)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TOEEqAxes structures.
 *
 * @param toe The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const TOEEqAxes& TOEEqAxes::operator=(const TOEEqAxes &toe)
{
   if (&toe == this)
      return *this;
   InertialAxes::operator=(toe);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~TOEEqAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
TOEEqAxes::~TOEEqAxes()
{
}

//---------------------------------------------------------------------------
//  bool TOEEqAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this TOEEqAxes.
 *
 */
//---------------------------------------------------------------------------
bool TOEEqAxes::Initialize()
{
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("Entering TOEEq::Initialize ..........\n");
   #endif
   InertialAxes::Initialize();
   InitializeFK5();
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("Completed IA:Init and InitFK5  ..........\n");
   #endif
   
   Real dPsi             = 0.0;
   Real longAscNodeLunar = 0.0;
   Real cosEpsbar        = 0.0;

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
   Real tTDB  = (mjdTT + offset) / 36525.0;
   
   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetNutationUpdateInterval();
   else                        updateIntervalToUse = updateInterval;
//   Rmatrix33  PREC      = ComputePrecessionMatrix(tTDB, epoch);
//   Rmatrix33  NUT       = ComputeNutationMatrix(tTDB, epoch, dPsi,
//                                                longAscNodeLunar, cosEpsbar);
   
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("About to call ComputePrecession/Nutation ..........\n");
   #endif
   ComputePrecessionMatrix(tTDB, epoch);
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("Called ComputePrecession ..........\n");
   #endif
   ComputeNutationMatrix(tTDB, epoch, dPsi, longAscNodeLunar, cosEpsbar, true);
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("Called ComputeNutation ..........\n");
      if (precData == NULL)
         MessageInterface::ShowMessage("precData is NULL!!!!\n");
      if (nutData == NULL)
         MessageInterface::ShowMessage("nutData is NULL!!!!\n");
   #endif
   
   Real PrecT[9] = {precData[0], precData[3], precData[6],
                    precData[1], precData[4], precData[7],
                    precData[2], precData[5], precData[8]};
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("PrecT omputed  ..........\n");
   #endif
   Real NutT[9] =  {nutData[0], nutData[3], nutData[6],
                    nutData[1], nutData[4], nutData[7],
                    nutData[2], nutData[5], nutData[8]};
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("NutT computed  ..........\n");
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
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("About to set rotMatrix ..........\n");
   #endif
   rotMatrix.Set(res[0][0],res[0][1],res[0][2],
                 res[1][0],res[1][1],res[1][2],
                 res[2][0],res[2][1],res[2][2]); 
   
   //rotMatrix = PREC.Transpose() * NUT.Transpose();
   //rotMatrix = PREC * NUT;

   // rotDotMatrix is still the default zero matrix
   
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("EXITing TOEEq::Initialize ..........\n");
   #endif
   return true;
}


GmatCoordinate::ParameterUsage TOEEqAxes::UsesEpoch() const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage TOEEqAxes::UsesEopFile() const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage TOEEqAxes::UsesItrfFile() const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage TOEEqAxes::UsesNutationUpdateInterval() const
{
   if (originName == SolarSystem::EARTH_NAME) 
      return GmatCoordinate::REQUIRED;
   return InertialAxes::UsesNutationUpdateInterval();
}



//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the TOEEqAxes.
 *
 * @return clone of the TOEEqAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* TOEEqAxes::Clone() const
{
   return (new TOEEqAxes(*this));
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
/*std::string TOEEqAxes::GetParameterText(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < TOEEqAxesParamCount)
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
/*Integer TOEEqAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = InertialAxesParamCount; i < TOEEqAxesParamCount; i++)
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
/*Gmat::ParameterType TOEEqAxes::GetParameterType(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < TOEEqAxesParamCount)
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
/*std::string TOEEqAxes::GetParameterTypeString(const Integer id) const
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
void TOEEqAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation)
{
   // already computed in Initialize
}
