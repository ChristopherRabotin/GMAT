//$Id$
//------------------------------------------------------------------------------
//                                  MJ2000EcAxes
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/01/06
//
/**
 * Implementation of the MJ2000EcAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "MJ2000EcAxes.hpp"
#include "InertialAxes.hpp"

//---------------------------------
// static data
//---------------------------------

/* placeholder - may be needed later
const std::string
MJ2000EcAxes::PARAMETER_TEXT[MJ2000EcAxesParamCount - InertialAxesParamCount] =
{
   "",
};

const Gmat::ParameterType
MJ2000EcAxes::PARAMETER_TYPE[MJ2000EcAxesParamCount - InertialAxesParamCount] =
{
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  MJ2000EcAxes(const std::string &itsType,
//               const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base MJ2000EcAxes structures
 * (default constructor).
 *
 * @param itsType GMAT script string associated with this type of object.
 * @param itsName Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
MJ2000EcAxes::MJ2000EcAxes(const std::string &itsName) :
InertialAxes("MJ2000Ec",itsName)
{
   objectTypeNames.push_back("MJ2000EcAxes");
   parameterCount = MJ2000EcAxesParamCount;
}

//---------------------------------------------------------------------------
//  MJ2000EcAxes(const MJ2000EcAxes &MJ2000);
//---------------------------------------------------------------------------
/**
 * Constructs base MJ2000EcAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param MJ2000  MJ2000EcAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
MJ2000EcAxes::MJ2000EcAxes(const MJ2000EcAxes &MJ2000) :
InertialAxes(MJ2000)
{
}

//---------------------------------------------------------------------------
//  MJ2000EcAxes& operator=(const MJ2000EcAxes &MJ2000)
//---------------------------------------------------------------------------
/**
 * Assignment operator for MJ2000EcAxes structures.
 *
 * @param MJ2000 The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const MJ2000EcAxes& MJ2000EcAxes::operator=(const MJ2000EcAxes &MJ2000)
{
   if (&MJ2000 == this)
      return *this;
   InertialAxes::operator=(MJ2000);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~MJ2000EcAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
MJ2000EcAxes::~MJ2000EcAxes()
{
}

//---------------------------------------------------------------------------
//  bool MJ2000EcAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this MJ2000EcAxes.
 *
 */
//---------------------------------------------------------------------------
bool MJ2000EcAxes::Initialize()
{
   InertialAxes::Initialize();
   // initialize the rotation matrix 
   rotMatrix(0,0) = 1.0;
   rotMatrix(0,1) = 0.0;
   rotMatrix(0,2) = 0.0;
   rotMatrix(1,0) = 0.0;
   rotMatrix(1,1) = 0.917482062076895741;
   rotMatrix(1,2) = -0.397777155914121383;
   rotMatrix(2,0) = 0.0;
   rotMatrix(2,1) = 0.397777155914121383;
   rotMatrix(2,2) = 0.917482062076895741;
   // rotDotMatrix is still the default zero matrix
   
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
GmatBase* MJ2000EcAxes::Clone() const
{
   return (new MJ2000EcAxes(*this));
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
/*std::string MJ2000EcAxes::GetParameterText(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < MJ2000EcAxesParamCount)
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
/*Integer MJ2000EcAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = InertialAxesParamCount; i < MJ2000EcAxesParamCount; i++)
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
/*Gmat::ParameterType MJ2000EcAxes::GetParameterType(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < MJ2000EcAxesParamCount)
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
/*std::string MJ2000EcAxes::GetParameterTypeString(const Integer id) const
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
 * @param atEpoch  epoch at which to compute the roration matrix
 */
//---------------------------------------------------------------------------
void MJ2000EcAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                           bool forceComputation)
{
   // already computed in Initialize;
}
