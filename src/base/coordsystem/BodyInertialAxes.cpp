//$Header$
//------------------------------------------------------------------------------
//                                  BodyInertialAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2006.08.01
//
/**
 * Implementation of the BodyInertialAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "BodyInertialAxes.hpp"
#include "InertialAxes.hpp"
#include "TimeTypes.hpp"
#include "Rvector.hpp"
#include "RealUtilities.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateSystemException.hpp"

//#define DEBUG_BODY_INERTIAL

#ifdef DEBUG_BODY_INERTIAL
   #include "MessageInterface.hpp"
#endif


//---------------------------------
// static data
//---------------------------------

/* placeholder - may be needed later
const std::string
BodyInertialAxes::PARAMETER_TEXT[BodyInertialAxesParamCount - InertialAxesParamCount] =
{
   "",
};

const Gmat::ParameterType
BodyInertialAxes::PARAMETER_TYPE[BodyInertialAxesParamCount - InertialAxesParamCount] =
{
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  BodyInertialAxes(const std::string &itsType,
//            const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base BodyInertialAxes structures
 * (default constructor).
 *
 * @param <itsType> GMAT script string associated with this type of object.
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
BodyInertialAxes::BodyInertialAxes(const std::string &itsName) :
InertialAxes("BodyInertial",itsName)
{
   objectTypeNames.push_back("BodyInertialAxes");
   parameterCount = BodyInertialAxesParamCount;
   Real TAIModJul   = 2451544.9996274998411 - GmatTimeUtil::JD_JAN_5_1941;
   Real mjdA1       = TimeConverterUtil::Convert(TAIModJul,
                      TimeConverterUtil::TAIMJD, TimeConverterUtil::A1MJD, 
                      GmatTimeUtil::JD_JAN_5_1941); 
   epoch.Set(mjdA1);     
   
   #ifdef DEBUG_BODY_INERTIAL
      MessageInterface::ShowMessage("BodyInertialAxes created with name %s\n",
      instanceName.c_str());
   #endif
}

//---------------------------------------------------------------------------
//  BodyInertialAxes(const BodyInertialAxes &bodyInertial);
//---------------------------------------------------------------------------
/**
 * Constructs base BodyInertialAxes structures used in derived classes, by 
 * copying the input instance (copy constructor).
 *
 * @param bodyInertial  BodyInertialAxes instance to copy to create "this" 
 *                                       instance.
 */
//---------------------------------------------------------------------------
BodyInertialAxes::BodyInertialAxes(const BodyInertialAxes &bodyInertial) :
InertialAxes(bodyInertial)
{
   #ifdef DEBUG_BODY_INERTIAL
      MessageInterface::ShowMessage("BodyInertialAxes created (copied) with name %s\n",
      instanceName.c_str());
   #endif
}

//---------------------------------------------------------------------------
//  BodyInertialAxes& operator=(const BodyInertialAxes &bodyInertial)
//---------------------------------------------------------------------------
/**
 * Assignment operator for BodyInertialAxes structures.
 *
 * @param bodyInertial The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const BodyInertialAxes& BodyInertialAxes::operator=(const BodyInertialAxes &bodyInertial)
{
   if (&bodyInertial == this)
      return *this;
   InertialAxes::operator=(bodyInertial);   
   #ifdef DEBUG_BODY_INERTIAL
      MessageInterface::ShowMessage("BodyInertialAxes created (operator=) with name %s\n",
      instanceName.c_str());
   #endif
   return *this;
}
//---------------------------------------------------------------------------
//  ~BodyInertialAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
BodyInertialAxes::~BodyInertialAxes()
{
}

//---------------------------------------------------------------------------
//  bool BodyInertialAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this BodyInertialAxes.  Creates the rotation 
 * matrix, based on IAU data (except for hard-coded values for 1) Earth, 
 * which uses the FK5 value at epoch; and 2) Luna, which is based on a
 * DE405-supplied orientation at epoch).
 *
 */
//---------------------------------------------------------------------------
bool BodyInertialAxes::Initialize()
{
   InertialAxes::Initialize();
   #ifdef DEBUG_BODY_INERTIAL
      MessageInterface::ShowMessage("Initializing BodyInertialAxes with name \"%s\"\n",
      instanceName.c_str());
   #endif
   // if origin is not a CelestialNody, there is an error
   if (!(origin->IsOfType("CelestialBody")))
      throw CoordinateSystemException(
            "Improper origin set for BodyInertialAxes object.");
   if (originName == SolarSystem::EARTH_NAME)
   {
      rotMatrix.Set(1.0,0.0,0.0,
                    0.0,1.0,0.0,
                    0.0,0.0,1.0);
   }
   else if (originName == SolarSystem::MOON_NAME)
   {
      rotMatrix.Set( 0.998496505205088,  0.0499357293985327, -0.0226086714041825,
                    -0.0548154092680678, 0.909610125238044,  -0.411830900942613,
                     0.0,                0.412451018902689,   0.91097977859343   );
   }
   else
   {
      // get alpha, delta, W, Wdot at the A1Mjd time of epoch
      Rvector   coords(4);
      coords              = ((CelestialBody*)origin)->GetBodyCartographicCoordinates(epoch);
      Real      a         = GmatMathUtil::PI_OVER_TWO +    // 90 + alpha
                            GmatMathUtil::Rad(coords[0]);
      Real      b         = GmatMathUtil::PI_OVER_TWO -    // 90 - delta
                            GmatMathUtil::Rad(coords[1]);
      Rmatrix33 R3T( GmatMathUtil::Cos(a), -GmatMathUtil::Sin(a), 0.0,
                     GmatMathUtil::Sin(a),  GmatMathUtil::Cos(a), 0.0,
                                      0.0,                   0.0, 1.0);
      Rmatrix33 R1T( 1.0,                   0.0,                   0.0,
                     0.0,  GmatMathUtil::Cos(b), -GmatMathUtil::Sin(b),
                     0.0,  GmatMathUtil::Sin(b),  GmatMathUtil::Cos(b));
      rotMatrix = R3T * R1T;
   }
   // rotDotMatrix remains the zero matrix
   return true;
}


GmatCoordinate::ParameterUsage BodyInertialAxes::UsesEpoch() const
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
 * This method returns a clone of the BodyInertialAxes.
 *
 * @return clone of the BodyInertialAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* BodyInertialAxes::Clone() const
{
   return (new BodyInertialAxes(*this));
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
/*std::string BodyInertialAxes::GetParameterText(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < BodyInertialAxesParamCount)
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
/*Integer BodyInertialAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = InertialAxesParamCount; i < BodyInertialAxesParamCount; i++)
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
/*Gmat::ParameterType BodyInertialAxes::GetParameterType(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < BodyInertialAxesParamCount)
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
/*std::string BodyInertialAxes::GetParameterTypeString(const Integer id) const
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
void BodyInertialAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation)
{
   // already computed in Initialize
}
