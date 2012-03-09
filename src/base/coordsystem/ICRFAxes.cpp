//$Id: $
//------------------------------------------------------------------------------
//                                  ICRFAxes
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Wendy C. Shoan/GSFC/GSSB
// Created: 2012.02.23
//
/**
 * Implementation of the ICRFAxes class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ICRFAxes.hpp"
#include "InertialAxes.hpp"

//#define DEBUG_CONSTRUCTION

#ifdef DEBUG_CONSTRUCTION
   #include "MessageInterface.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

/* placeholder - may be needed later
const std::string
ICRFAxes::PARAMETER_TEXT[ICRFAxesParamCount - InertialAxesParamCount] =
{
   "",
};

const Gmat::ParameterType
ICRFAxes::PARAMETER_TYPE[ICRFAxesParamCount - InertialAxesParamCount] =
{
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ICRFAxes(const std::string &itsType,
//           const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base ICRFAxes structures
 * (default constructor).
 *
 * @param itsType GMAT script string associated with this type of object.
 * @param itsName Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
ICRFAxes::ICRFAxes(const std::string &itsName) :
InertialAxes("ICRF",itsName)
{
   objectTypeNames.push_back("ICRFAxes");
   baseSystem     = "ICRF";
   parameterCount = ICRFAxesParamCount;
   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Now constructing ICRFAxes with name %s\n",
         itsName.c_str());
   #endif
}

//---------------------------------------------------------------------------
//  ICRFAxes(const ICRFAxes &icrf);
//---------------------------------------------------------------------------
/**
 * Constructs base ICRFAxes structures used in derived classes, by copying
 * the input instance (copy constructor).
 *
 * @param icrf  ICRFAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
ICRFAxes::ICRFAxes(const ICRFAxes &icrf) :
InertialAxes(icrf)
{
}

//---------------------------------------------------------------------------
//  ICRFAxes& operator=(const ICRFAxes &icrf)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ICRFAxes structures.
 *
 * @param icrf The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ICRFAxes& ICRFAxes::operator=(const ICRFAxes &icrf)
{
   if (&icrf == this)
      return *this;
   InertialAxes::operator=(icrf);
   return *this;
}
//---------------------------------------------------------------------------
//  ~ICRFAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ICRFAxes::~ICRFAxes()
{
}

//---------------------------------------------------------------------------
//  bool ICRFAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this ICRFAxes.
 *
 */
//---------------------------------------------------------------------------
bool ICRFAxes::Initialize()
{
   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Now entering ICRFAxes INIT with name %s\n",
         instanceName.c_str());
   #endif
   InertialAxes::Initialize();
   // initialize the rotation matrix to be the identity matrix
   rotMatrix(0,0) = 1.0;
   rotMatrix(1,1) = 1.0;
   rotMatrix(2,2) = 1.0;
   // rotDotMatrix is still the default zero matrix
   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Now LEAVING ICRFAxes INIT with name %s\n",
         instanceName.c_str());
   #endif

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
GmatBase* ICRFAxes::Clone() const
{
   return (new ICRFAxes(*this));
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
/*std::string ICRFAxes::GetParameterText(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < ICRFAxesParamCount)
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
/*Integer ICRFAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = InertialAxesParamCount; i < ICRFAxesParamCount; i++)
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
/*Gmat::ParameterType ICRFAxes::GetParameterType(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < ICRFAxesParamCount)
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
/*std::string ICRFAxes::GetParameterTypeString(const Integer id) const
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
 * from/to this AxisSystem to/from the ICRFAxes system.
 *
 * @param atEpoch  epoch at which to compute the roration matrix
 */
//---------------------------------------------------------------------------
void ICRFAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                           bool forceComputation)
{
   // already computed in Initialize;
}
