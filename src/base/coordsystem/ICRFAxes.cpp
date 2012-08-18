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
#include "RealUtilities.hpp"
#include "Planet.hpp"
#include "ICRFAxes.hpp"
#include "InertialAxes.hpp"
#include "MessageInterface.hpp"

using namespace GmatMathUtil;      // for trig functions, etc.

#define DEBUG_ICRFAXES_CONSTRUCTION
#define DEBUG_ICRFAXES_INITIALIZE
#define DEBUG_ICRFAXES_FIRST_CALL
#define DEBUG_ICRFAXES_ROT_MATRIX
#define DEBUG_ICRFAXES_RECOMPUTE
#define DEBUG_ICRFAXES_EPOCHS

#ifdef DEBUG_ICRFAXES_CONSTRUCTION
   #include "MessageInterface.hpp"
#endif

#ifdef DEBUG_ICRFAXES_FIRST_CALL
   static bool firstCallFired = false;
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
InertialAxes("ICRF",itsName),
isInitialized	(false),
icrfFile       (NULL)
{
   objectTypeNames.push_back("ICRFAxes");
   baseSystem     = "FK5";
   parameterCount = ICRFAxesParamCount;
   #ifdef DEBUG_ICRFAXES_CONSTRUCTION
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
InertialAxes(icrf),
isInitialized	(icrf.isInitialized),
icrfFile       (icrf.icrfFile)
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
   icrfFile 		= icrf.icrfFile;
   isInitialized	= icrf.isInitialized;

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
   #ifdef DEBUG_ICRFAXES_INITIALIZE
      MessageInterface::ShowMessage("Initialize ICRFAxes: with name %s\n",
         instanceName.c_str());
   #endif
   if (isInitialized) return true;

   InertialAxes::Initialize();

   // Create an ICRFFile object in order to read Euler rotation vector:
   if (icrfFile == NULL)
      icrfFile = ICRFFile::Instance();
   icrfFile->Initialize();

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
/*
void ICRFAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                           bool forceComputation)
{
	Real epoch = atEpoch.Get();
	Real vec[3];
	icrfFile->GetICRFRotationVector(epoch, &vec[0], 3, 9);
	MessageInterface::ShowMessage("epoch = %lf, rotation vector = %le %le %le\n", epoch, vec[0], vec[1], vec[2]);
}
*/

//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the ICRFAxes system
 *
 * @param atEpoch  epoch at which to compute the rotation matrix
 *
 */
//------------------------------------------------------------------------------
void ICRFAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                            bool forceComputation)
{
   #ifdef DEBUG_ICRFAXES_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "Calling CalculateRotationMatrix at epoch %lf; ", atEpoch.Get());
   #endif
   Real theEpoch = atEpoch.Get();
   #ifdef DEBUG_ICRFAXES_EPOCHS
      MessageInterface::ShowMessage("CalculateRotationMatrix(%s)   epoch = %12.10f, prevEpoch = %12.10f ...... \n",
            (coordName.c_str()), theEpoch, prevEpoch);
   #endif
   #ifdef DEBUG_ICRFAXES_RECOMPUTE
      MessageInterface::ShowMessage("Entering CalculateRotationMatrix on object %s (%p) of type %s, origin = %s\n",
            (coordName.c_str()), this, (GetTypeName()).c_str(), originName.c_str());
      MessageInterface::ShowMessage("     epoch = %12.10f, prevEpoch = %12.10f\n", theEpoch, prevEpoch);
   #endif

   // Code must check to see if we need to recompute.  Recomputation is only necessary
   // if one or more of the following conditions are true:
   // 1. the epoch is different (within machine precision) from the epoch at the last computation
   // 2. if the origin is the Earth, the nutation update interval has changed
   // 3. if the origin is Luna, the rotation data source has changed

   if ((!forceComputation)                    &&
       (originName == SolarSystem::MOON_NAME) &&
       (IsEqual(theEpoch,      prevEpoch))    &&
       (prevLunaSrc == ((CelestialBody*)origin)->GetRotationDataSource()))
      {
         #ifdef DEBUG_ICRFAXES_RECOMPUTE
            MessageInterface::ShowMessage("Don't need to recompute for Luna at this time!!\n");
         #endif
         return;
      }


   // compute rotMatrix and rotDotMatrix
//   if (originName == SolarSystem::EARTH_NAME)
//   {
      Real intervalFromOrigin = ((Planet*) origin)->GetNutationUpdateInterval();
      if ((!forceComputation)                                     &&
          (IsEqual(theEpoch,           prevEpoch)                 &&
          (IsEqual(intervalFromOrigin, prevOriginUpdateInterval)) &&
          (IsEqual(updateInterval,     prevUpdateInterval))))
      {
         #ifdef DEBUG_ICRFAXES_EPOCHS
            MessageInterface::ShowMessage("NOT recomputing\n");
         #endif
         #ifdef DEBUG_ICRFAXES_RECOMPUTE
            MessageInterface::ShowMessage("Don't need to recompute for Earth at this time!!\n");
         #endif
         return;
      }

      #ifdef DEBUG_ICRFAXES_EPOCHS
         MessageInterface::ShowMessage("RECOMPUTING!!!\n");
      #endif
      #ifdef DEBUG_ICRFAXES_RECOMPUTE
         MessageInterface::ShowMessage("   RECOMPUTING!!! - body name is EARTH\n");
      #endif
      #ifdef DEBUG_ICRFAXES_FIRST_CALL
         if (!firstCallFired)
            MessageInterface::ShowMessage("In BFA, Body is the Earth\n");
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
      rotMatrix.SetElement(0,0, c+a[0]*a[0]*(1-c));
      rotMatrix.SetElement(0,1, a[0]*a[1]*(1-c)+a[2]*s);
      rotMatrix.SetElement(0,2, a[0]*a[2]*(1-c)-a[1]*s);
      rotMatrix.SetElement(1,0, a[0]*a[1]*(1-c)-a[2]*s);
      rotMatrix.SetElement(1,1, c+a[1]*a[1]*(1-c));
      rotMatrix.SetElement(1,2, a[1]*a[2]*(1-c)+a[0]*s);
      rotMatrix.SetElement(2,0, a[0]*a[2]*(1-c)+a[1]*s);
      rotMatrix.SetElement(2,1, a[1]*a[2]*(1-c)-a[0]*s);
      rotMatrix.SetElement(2,2, c+a[2]*a[2]*(1-c));

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

      if (overrideOriginInterval)
      {
         updateIntervalToUse = updateInterval;
         //MessageInterface::ShowMessage("Overriding origin interval .....\n");
         #ifdef DEBUG_ICRFAXES_FIRST_CALL
            if (!firstCallFired)
               MessageInterface::ShowMessage(
                  "   Overrode origin interval; set to %.12lf\n",
                  updateIntervalToUse);
         #endif
      }
      else
      {
         updateIntervalToUse = intervalFromOrigin;
         //MessageInterface::ShowMessage("Using origin interval .....\n");
         #ifdef DEBUG_ICRFAXES_FIRST_CALL
            if (!firstCallFired)
               MessageInterface::ShowMessage(
                  "   Using body's origin interval, %.12lf\n",
                  updateIntervalToUse);
         #endif
      }


      // save the data to compare against next time
      prevUpdateInterval       = updateInterval;
      prevOriginUpdateInterval = intervalFromOrigin;
//   }


   // Save the epoch for comparison the next time through
   prevEpoch = theEpoch;
   #ifdef DEBUG_ICRFAXES_RECOMPUTE
      MessageInterface::ShowMessage("at the end, just set prevEpoch to %12.10f\n", prevEpoch);
   #endif
   if (originName == SolarSystem::MOON_NAME)
      prevLunaSrc = ((CelestialBody*)origin)->GetRotationDataSource();

   #ifdef DEBUG_ICRFAXES_FIRST_CALL
      firstCallFired = true;
      MessageInterface::ShowMessage("NOW exiting BFA::CalculateRotationMatrix ...\n");
   #endif

}

void  ICRFAxes::GetRotationMatrix(const A1Mjd &atEpoch, bool forceComputation)
{
	CalculateRotationMatrix(atEpoch, forceComputation);
}
