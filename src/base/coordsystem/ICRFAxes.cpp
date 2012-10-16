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
//         Tuan Dang Nguyen/GSFC
// Created: 2012.02.23
//
/**
 * Implementation of the ICRFAxes class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RealUtilities.hpp"
#include "ICRFAxes.hpp"
#include "InertialAxes.hpp"
#include "MessageInterface.hpp"

using namespace GmatMathUtil;      // for trig functions, etc.

//#define DEBUG_ICRFAXES_CONSTRUCTION
//#define DEBUG_ICRFAXES_CLONE
//#define DEBUG_ICRFAXES_INITIALIZE
//#define DEBUG_ICRFAXES_FIRST_CALL
//#define DEBUG_ICRFAXES_ROT_MATRIX
//#define DEBUG_ICRFAXES_RECOMPUTE
//#define DEBUG_ICRFAXES_EPOCHS

#ifdef DEBUG_ICRFAXES_FIRST_CALL
   static bool firstCallFired = false;
#endif

//---------------------------------
// static data
//---------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ICRFAxes(const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base ICRFAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
ICRFAxes::ICRFAxes(const std::string &itsName) :
   InertialAxes   ("ICRF",itsName),
   icrfFile       (NULL)
{
   objectTypeNames.push_back("ICRFAxes");
   baseSystem     = "FK5";
   parameterCount = ICRFAxesParamCount;
   #ifdef DEBUG_ICRFAXES_CONSTRUCTION
      MessageInterface::ShowMessage("Now constructing ICRFAxes with name '%s'\n",
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
   InertialAxes(icrf),
   icrfFile       (icrf.icrfFile)
{
#ifdef DEBUG_ICRFAXES_CONSTRUCTION
   MessageInterface::ShowMessage("Now copy-constructing ICRFAxes from object (%p) with name '%s'\n", &icrf,
     icrf.GetName().c_str());
#endif
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
   icrfFile 		= icrf.icrfFile;

   return *this;
}

//---------------------------------------------------------------------------
//  ~ICRFAxes()
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
 * @result success flag
 */
//---------------------------------------------------------------------------
bool ICRFAxes::Initialize()
{
   #ifdef DEBUG_ICRFAXES_INITIALIZE
      MessageInterface::ShowMessage("Initialize ICRFAxes: with name '%s'\n",
         instanceName.c_str());
   #endif

//   if (!isInitialized)
//   {
      InertialAxes::Initialize();

      // Create an ICRFFile object in order to read Euler rotation vector:
//      if (icrfFile == NULL)
      icrfFile = ICRFFile::Instance();
      icrfFile->Initialize();
      isInitialized = true;
//   }

   #ifdef DEBUG_ICRFAXES_INITIALIZE
   MessageInterface::ShowMessage("End initialize ICRFAxes: with name '%s'\n",
         instanceName.c_str());
   #endif

   return true;
}

//----------------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesEpoch() const
//----------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage ICRFAxes::UsesEpoch() const
{
   return GmatCoordinate::REQUIRED;
}


//-----------------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem) const
//-----------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage ICRFAxes::UsesEopFile(const std::string &forBaseSystem) const
{
   if (forBaseSystem == baseSystem)
      return GmatCoordinate::REQUIRED;
   return GmatCoordinate::NOT_USED;
}


//----------------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesItrfFile() const
//----------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage ICRFAxes::UsesItrfFile() const
{
   return GmatCoordinate::NOT_USED;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ICRFAxes.
 *
 * @return clone of the ICRFAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ICRFAxes::Clone() const
{
   #ifdef DEBUG_ICRFAXES_CONSTRUCTION
   MessageInterface::ShowMessage("Now clone ICRFAxes from object (%p) with name '%s'\n", this,
        this->GetName().c_str());
   #endif
   return (new ICRFAxes(*this));
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the ICRFAxes system
 *
 * @param atEpoch          epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 *
 */
//------------------------------------------------------------------------------
void ICRFAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                            bool forceComputation)
{
   Real theEpoch = atEpoch.Get();
   #ifdef DEBUG_ICRFAXES_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "Calling ICRF::CalculateRotationMatrix at epoch %18.12lf; \n", theEpoch);
   #endif

   // Specify Euler rotation vector for theEpoch:
   Real vec[3];
   icrfFile->GetICRFRotationVector(theEpoch, &vec[0], 3, 9);

   // Calculate rotation matrix based on Euler rotation vector:
   Real angle = GmatMathUtil::Sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
   Real a[3];
   a[0] = vec[0]/angle; a[1] = vec[1]/angle; a[2] = vec[2]/angle;
   Real c = GmatMathUtil::Cos(angle);
   Real s = GmatMathUtil::Sin(angle);

   // rotation matrix from FK5 to ICRF:
   Rmatrix33 rotM;
   rotM.SetElement(0,0, c+a[0]*a[0]*(1-c));
   rotM.SetElement(0,1, a[0]*a[1]*(1-c)+a[2]*s);
   rotM.SetElement(0,2, a[0]*a[2]*(1-c)-a[1]*s);
   rotM.SetElement(1,0, a[0]*a[1]*(1-c)-a[2]*s);
   rotM.SetElement(1,1, c+a[1]*a[1]*(1-c));
   rotM.SetElement(1,2, a[1]*a[2]*(1-c)+a[0]*s);
   rotM.SetElement(2,0, a[0]*a[2]*(1-c)+a[1]*s);
   rotM.SetElement(2,1, a[1]*a[2]*(1-c)-a[0]*s);
   rotM.SetElement(2,2, c+a[2]*a[2]*(1-c));

   // rotation matrix from ICRF to FK5:
   rotMatrix = rotM.Transpose();

   // rotation dot matrix from ICRF to FK5:
   for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
    	  rotDotMatrix.SetElement(i,j, 0.0);

   #ifdef DEBUG_ICRFAXES_ROT_MATRIX
      MessageInterface::ShowMessage("theEpoch  = %18.12lf\n",theEpoch);

      MessageInterface::ShowMessage("rotation vector = %18.12e %18.12e %18.12e\n", vec[0], vec[1], vec[2]);
      MessageInterface::ShowMessage("R(0,0)=%18.12e,  R(0,1)=%18.12e,  R(0,2)=%18.12e\n",rotMatrix(0,0),rotMatrix(0,1),rotMatrix(0,2));
      MessageInterface::ShowMessage("R(1,0)=%18.12e,  R(1,1)=%18.12e,  R(1,2)=%18.12e\n",rotMatrix(1,0),rotMatrix(1,1),rotMatrix(1,2));
      MessageInterface::ShowMessage("R(2,0)=%18.12e,  R(2,1)=%18.12e,  R(2,2)=%18.12e\n",rotMatrix(2,0),rotMatrix(2,1),rotMatrix(2,2));

      MessageInterface::ShowMessage("Rdot(0,0)=%18.12e,  Rdot(0,1)=%18.12e,  Rdot(0,2)=%18.12e\n",rotDotMatrix(0,0),rotDotMatrix(0,1),rotDotMatrix(0,2));
      MessageInterface::ShowMessage("Rdot(1,0)=%18.12e,  Rdot(1,1)=%18.12e,  Rdot(1,2)=%18.12e\n",rotDotMatrix(1,0),rotDotMatrix(1,1),rotDotMatrix(1,2));
      MessageInterface::ShowMessage("Rdot(2,0)=%18.12e,  Rdot(2,1)=%18.12e,  Rdot(2,2)=%18.12e\n\n\n",rotDotMatrix(2,0),rotDotMatrix(2,1),rotDotMatrix(2,2));
   #endif

   #ifdef DEBUG_ICRFAXES_FIRST_CALL
      firstCallFired = true;
      MessageInterface::ShowMessage("NOW exiting ICRFAxes::CalculateRotationMatrix ...\n");
   #endif


}

//------------------------------------------------------------------------------
//  Rmatrix33 GetRotationMatrix(const A1Mjd &atEpoch,
//                              bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the ICRFAxes system
 *
 * @param atEpoch          epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 *
 * @return rotation matrix
 */
//------------------------------------------------------------------------------
Rmatrix33 ICRFAxes::GetRotationMatrix(const A1Mjd &atEpoch, bool forceComputation)
{
	CalculateRotationMatrix(atEpoch, forceComputation);
	return rotMatrix;
}
