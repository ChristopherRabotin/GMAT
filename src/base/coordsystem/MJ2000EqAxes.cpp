//$Header$
//------------------------------------------------------------------------------
//                                  MJ2000EqAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/01/05
//
/**
 * Implementation of the MJ2000EqAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "MJ2000EqAxes.hpp"
#include "InertialAxes.hpp"

//*********************//
#include <iostream>
using namespace std;

//---------------------------------
// static data
//---------------------------------

/* placeholder - may be needed later
const std::string
MJ2000EqAxes::PARAMETER_TEXT[MJ2000EqAxesParamCount - InertialAxesParamCount] =
{
   "",
};

const Gmat::ParameterType
MJ2000EqAxes::PARAMETER_TYPE[MJ2000EqAxesParamCount - InertialAxesParamCount] =
{
};
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  MJ2000EqAxes(const std::string &itsType,
//               const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base MJ2000EqAxes structures
 * (default constructor).
 *
 * @param itsType GMAT script string associated with this type of object.
 * @param itsName Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
MJ2000EqAxes::MJ2000EqAxes(const std::string &itsName) :
InertialAxes("MJ2000Eq",itsName)
{
}

//---------------------------------------------------------------------------
//  MJ2000EqAxes(const MJ2000EqAxes &mj2000);
//---------------------------------------------------------------------------
/**
 * Constructs base MJ2000EqAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param mj2000  MJ2000EqAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
MJ2000EqAxes::MJ2000EqAxes(const MJ2000EqAxes &mj2000) :
InertialAxes(mj2000)
{
}

//---------------------------------------------------------------------------
//  MJ2000EqAxes& operator=(const MJ2000EqAxes &mj2000)
//---------------------------------------------------------------------------
/**
 * Assignment operator for MJ2000EqAxes structures.
 *
 * @param mj2000 The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const MJ2000EqAxes& MJ2000EqAxes::operator=(const MJ2000EqAxes &mj2000)
{
   if (&mj2000 == this)
      return *this;
   InertialAxes::operator=(mj2000);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~MJ2000EqAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
MJ2000EqAxes::~MJ2000EqAxes()
{
}

//---------------------------------------------------------------------------
//  void MJ2000EqAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this MJ2000EqAxes.
 *
 */
//---------------------------------------------------------------------------
void MJ2000EqAxes::Initialize()
{
   InertialAxes::Initialize();
   // initialize the rotation matrix to be the identity matrix
   rotMatrix(0,0) = 1.0;
   rotMatrix(1,1) = 1.0;
   rotMatrix(2,2) = 1.0;
   // rotDotMatrix is still the default zero matrix
}

//---------------------------------------------------------------------------
//  bool RotateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                        Rvector &outState//, SpacePoint *j2000Body)
//---------------------------------------------------------------------------
/**
 * This method will rotate the input inState into the MJ2000Eq frame.
 *
 * @param epoch     the epoch at which to perform the rotation.
 * @param inState   the input state (in this AxisSystem) to be rotated.
 * @param iutState  the output state, in the MJ2000Eq AxisSystem, the result 
 *                  of rotating the input inState.
 * @param j2000Body the origin of the output MJ2000EqAxes frame. // set in object
 *
 * @return success or failure of the operation.
 */
//---------------------------------------------------------------------------
bool MJ2000EqAxes::RotateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
                              Rvector &outState) //, SpacePoint *j2000Body)
{
   cout << "---------------- In MJ2000Eq::ToMJ2000Eq ...." << endl;
   outState = inState;
   return true;
}

//---------------------------------------------------------------------------
//  bool RotateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                          Rvector &outState//, SpacePoint *j2000Body)
//---------------------------------------------------------------------------
/**
 * This method will rotate the input inState from the MJ2000Eq frame into
 * this AxisSystem.
 *
 * @param epoch     the epoch at which to perform the rotation.
 * @param inState   the input state (in MJ2000Eq AxisSystem) to be rotated.
 * @param iutState  the output state, in this AxisSystem, the result 
 *                  of rotating the input inState.
 * @param j2000Body the origin of the input MJ2000EqAxes frame.
 *
 * @return success or failure of the operation.
 */
//---------------------------------------------------------------------------
bool MJ2000EqAxes::RotateFromMJ2000Eq(const A1Mjd &epoch, 
                                      const Rvector &inState,
                                      Rvector &outState) //, SpacePoint *j2000Body)
{
   cout << "---------------- In MJ2000Eq::FromMJ2000Eq ...." << endl;
   cout << "Size of inState = " << inState.GetSize() << endl;
   cout << "Size of outState = " << outState.GetSize() << endl;
   outState = inState;
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
GmatBase* MJ2000EqAxes::Clone() const
{
   return (new MJ2000EqAxes(*this));
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
/*std::string MJ2000EqAxes::GetParameterText(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < MJ2000EqAxesParamCount)
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
/*Integer MJ2000EqAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = InertialAxesParamCount; i < MJ2000EqAxesParamCount; i++)
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
/*Gmat::ParameterType MJ2000EqAxes::GetParameterType(const Integer id) const
{
   if (id >= InertialAxesParamCount && id < MJ2000EqAxesParamCount)
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
/*std::string MJ2000EqAxes::GetParameterTypeString(const Integer id) const
{
   return InertialAxes::PARAM_TYPE_STRING[GetParameterType(id)];
}
*/


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  void CalculateRotationMatrix(SpacePoint *j2000Body)
//---------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000EqAxes system.
 *
 * @param j2000Body the origin of the MJ2000EqAxes frame.
 */
//---------------------------------------------------------------------------
void MJ2000EqAxes::CalculateRotationMatrix()
{
   // already computed in Initialize;
}
