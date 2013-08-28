//$Id$
//------------------------------------------------------------------------------
//                             PrecessingSpinner
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Author: Yeerang Lim/KAIST
// Created: 2013.05.15
// Updated: 20130.06.26 (Jaehwan Pi/KAIST)
//
/**
 * Class implementation for the PrecessingSpinner class.
 * 
 * @note 
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Attitude.hpp"
#include "AttitudeException.hpp"
#include "MessageInterface.hpp"

#include "PrecessingSpinner.hpp"


//#define DEBUG_PRECESSING_SPINNER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
//const std::string
//PrecessingSpinner::PARAMETER_TEXT[PrecessingSpinnerParamCount - KinematicParamCount] =
//{
//	"NutationReferenceVectorX",
//	"NutationReferenceVectorY",
//	"NutationReferenceVectorZ",
//	"BodySpinAxisX",
//	"BodySpinAxisY",
//	"BodySpinAxisZ",
//	"InitialPrecessionAngle",
//	"PrecessionRate",
//	"NutationAngle",
//	"InitialSpinAngle",
//	"SpinRate"
//};
//
//const Gmat::ParameterType
//PrecessingSpinner::PARAMETER_TYPE[PrecessingSpinnerParamCount - KinematicParamCount] =
//{
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE
//};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  PrecessingSpinner(const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the PrecessingSpinner class (constructor).
 */
//------------------------------------------------------------------------------
PrecessingSpinner::PrecessingSpinner(const std::string &itsName) : 
   Kinematic("PrecessingSpinner",itsName)
{
   parameterCount = PrecessingSpinnerParamCount;
   objectTypeNames.push_back("PrecessingSpinner");
   attitudeModelName         = "PrecessingSpinner";
   setInitialAttitudeAllowed = false;
   modifyCoordSysAllowed     = false;
   // Reserve spaces to handle attribute comments for owned object
   // LOJ: 2013.03.01 for GMT-3353 FIX

//   NutationReferenceVector.Set(0,0,1);
//   BodySpinAxis.Set(0,0,1);
//   InitialPrecessionAngle = 0.0;
//   PrecessionRate = 5.0 * GmatMathConstants::RAD_PER_DEG;  // convert to radian/sec.
//   NutationAngle = 15.0 * GmatMathConstants::RAD_PER_DEG;// convert to radian
//   InitialSpinAngle = 0.0;
//   SpinRate = 10.0 * GmatMathConstants::RAD_PER_DEG;  // convert to radian/sec.

   FinalizeCreation();
}

//------------------------------------------------------------------------------
//  PrecessingSpinner(const PrecessingSpinner &att)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the PrecessingSpinner class as a copy of the
 * specified PrecessingSpinner class (copy constructor).
 *
 * @param <att> PrecessingSpinner object to copy.
 */
//------------------------------------------------------------------------------
PrecessingSpinner::PrecessingSpinner(const PrecessingSpinner& att) :
   Kinematic(att)
{
//   NutationReferenceVector = att.NutationReferenceVector ;
//   BodySpinAxis = att.BodySpinAxis ;
//   InitialPrecessionAngle = att.InitialPrecessionAngle ;
//   PrecessionRate = att.PrecessionRate ;
//   NutationAngle = att.NutationAngle ;
//   InitialSpinAngle = att.InitialSpinAngle ;
//   SpinRate = att.SpinRate ;
}
 
//------------------------------------------------------------------------------
//  PrecessingSpinner& operator= (const PrecessingSpinner& att)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the PrecessingSpinner class.
 *
 * @param att the PrecessingSpinner object whose data to assign to "this"
 *            PrecessingSpinner.
 *
 * @return "this" PrecessingSpinner with data of input PrecessingSpinner att.
 */
//------------------------------------------------------------------------------
PrecessingSpinner& PrecessingSpinner::operator=(const PrecessingSpinner& att)
{
   Kinematic::operator=(att);
//
//   NutationReferenceVector = att.NutationReferenceVector ;
//   BodySpinAxis = att.BodySpinAxis ;
//   InitialPrecessionAngle = att.InitialPrecessionAngle ;
//   PrecessionRate = att.PrecessionRate ;
//   NutationAngle = att.NutationAngle ;
//   InitialSpinAngle = att.InitialSpinAngle ;
//   SpinRate = att.SpinRate ;

   return *this;
}

//------------------------------------------------------------------------------
//  ~PrecessingSpinner()
//------------------------------------------------------------------------------
/**
 * Destroys the PrecessingSpinner class (constructor).
 */
//------------------------------------------------------------------------------
PrecessingSpinner::~PrecessingSpinner()
{
   // nothing really to do here ... la la la la la
}


//---------------------------------------------------------------------------
//  bool Initialize() 
//---------------------------------------------------------------------------
 /**
 * Initializes the attitude.
 * 
 * @return Success flag.
 *
  */
//---------------------------------------------------------------------------
bool PrecessingSpinner::Initialize()
{
   if (!Kinematic::Initialize()) return false;

   // check the norm of two vectors

   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the PrecessingSpinner.
 *
 * @return clone of the PrecessingSpinner.
 *
 */
//------------------------------------------------------------------------------
GmatBase* PrecessingSpinner::Clone() const
{
	return (new PrecessingSpinner(*this)); //???(Yeerang Lim)
}

//
//std::string PrecessingSpinner::GetParameterText(const Integer id) const
//{
//   if (id >= KinematicParamCount && id < PrecessingSpinnerParamCount)
//      return PARAMETER_TEXT[id - KinematicParamCount];
//   return Kinematic::GetParameterText(id);
//}
//
//
//Integer PrecessingSpinner::GetParameterID(const std::string &str) const
//{
//
//   for (Integer i = KinematicParamCount; i < PrecessingSpinnerParamCount; i++)
//   {
//	   if (str == PARAMETER_TEXT[i - KinematicParamCount])
//		   return i;
//   }
//
//   return Kinematic::GetParameterID(str);
//}
//
//
//Gmat::ParameterType PrecessingSpinner::GetParameterType(const Integer id) const
//{
//   if (id >= KinematicParamCount && id < PrecessingSpinnerParamCount)
//      return PARAMETER_TYPE[id - KinematicParamCount];
//
//   return Kinematic::GetParameterType(id);
//}
//
///*
//std::string PrecessingSpinner::GetParameterTypeString(const Integer id) const
//{
//   return Kinematic::PARAM_TYPE_STRING[GetParameterType(id)];
//}
//*/
//
//Real PrecessingSpinner::GetRealParameter(const Integer id) const
//{
//   if (id == NUTATION_REFERENCE_VECTOR_X)
//   {
//      return NutationReferenceVector(0);
//   }
//   else if (id == NUTATION_REFERENCE_VECTOR_Y)
//   {
//      return NutationReferenceVector(1);
//   }
//   else if (id == NUTATION_REFERENCE_VECTOR_Z)
//   {
//      return NutationReferenceVector(2);
//   }
//   else if (id == BODY_SPIN_AXIS_X)
//   {
//      return BodySpinAxis(0);
//   }
//   else if (id == BODY_SPIN_AXIS_Y)
//   {
//      return BodySpinAxis(1);
//   }
//   else if (id == BODY_SPIN_AXIS_Z)
//   {
//      return BodySpinAxis(2);
//   }
//   else if (id == INITIAL_PRECESSION_ANGLE)
//   {
//      return InitialPrecessionAngle;
//   }
//   else if (id == PRECESSION_RATE)
//   {
//      return PrecessionRate;
//   }
//   else if (id == NUTATION_ANGLE)
//   {
//      return NutationAngle;
//   }
//   else if (id == INITIAL_SPIN_ANGLE)
//   {
//      return InitialSpinAngle;
//   }
//   else if (id == SPIN_RATE)
//   {
//      return SpinRate;
//   }
//
//   return Kinematic::GetRealParameter(id);
//}
//
//
//Real PrecessingSpinner::SetRealParameter(const Integer id, const Real value)
//{
//   if (id == NUTATION_REFERENCE_VECTOR_X)
//   {
//      NutationReferenceVector(0) = value;
//	  return NutationReferenceVector(0);
//   }
//   if (id == NUTATION_REFERENCE_VECTOR_Y)
//   {
//      NutationReferenceVector(1) = value;
//	  return NutationReferenceVector(1);
//   }
//   if (id == NUTATION_REFERENCE_VECTOR_Z)
//   {
//      NutationReferenceVector(2) = value;
//	  return NutationReferenceVector(2);
//   }
//   if (id == BODY_SPIN_AXIS_X)
//   {
//      BodySpinAxis(0) = value;
//	  return BodySpinAxis(0);
//   }
//   if (id == BODY_SPIN_AXIS_Y)
//   {
//      BodySpinAxis(1) = value;
//	  return BodySpinAxis(1);
//   }
//   if (id == BODY_SPIN_AXIS_Z)
//   {
//      BodySpinAxis(2) = value;
//	  return BodySpinAxis(2);
//   }
//   if (id == INITIAL_PRECESSION_ANGLE)
//   {
//      InitialPrecessionAngle = value;
//	  return InitialPrecessionAngle;
//   }
//   if (id == PRECESSION_RATE)
//   {
//      PrecessionRate = value;
//	  return PrecessionRate;
//   }
//   if (id == NUTATION_ANGLE)
//   {
//      NutationAngle = value;
//	  return NutationAngle;
//   }
//   if (id == INITIAL_SPIN_ANGLE)
//   {
//      InitialSpinAngle = value;
//	  return InitialSpinAngle;
//   }
//   if (id == SPIN_RATE)
//   {
//      SpinRate= value;
//	  return SpinRate;
//   }
//
//   return Kinematic::SetRealParameter(id,value);
//}
//

void PrecessingSpinner::ComputeCosineMatrixAndAngularVelocity(Real atTime)
{
   if (!isInitialized || needsReinit) Initialize();

   // Error message and vector normalization
   if ( bodySpinAxis.GetMagnitude() < 1.0e-5 )
   {
	   throw AttitudeException("The size of BodySpinVector is too small");
   }else
   {
	   bodySpinAxisNormalized = bodySpinAxis / bodySpinAxis.GetMagnitude();
   }

   if ( nutationReferenceVector.GetMagnitude() < 1.0e-5 )
   {
	   throw AttitudeException("The size of NutationReferenceVector is too small");
   }else
   {
	   nutationReferenceVectorNormalized = nutationReferenceVector / nutationReferenceVector.GetMagnitude();
   }

   if ( abs(nutationAngle) < 1.0e-5 )
   {
	   MessageInterface::ShowMessage("Warning - Nutation angle is nearly zero\n");
   }

   #ifdef DEBUG_PRECESSING_SPINNER
   MessageInterface::ShowMessage(
   "Entering PrecessingSpinner::BodySpinAxis = [ %.3f %.3f %.3f]\n",
   BodySpinAxisNormalized(0), BodySpinAxisNormalized(1), BodySpinAxisNormalized(2));

   MessageInterface::ShowMessage("NutationReferenceVector = [ %.3f %.3f %.3f]\n",
   NutationReferenceVectorNormalized(0), NutationReferenceVectorNormalized(1), NutationReferenceVectorNormalized(2));

   MessageInterface::ShowMessage("InitialPrecessionAngle = %.3f , PrecessionRate = %.3f\n",
	   InitialPrecessionAngle*GmatMathConstants::DEG_PER_RAD, PrecessionRate*GmatMathConstants::DEG_PER_RAD);

   MessageInterface::ShowMessage("NutationAngle = %.3f\n" , NutationAngle*GmatMathConstants::DEG_PER_RAD);

   MessageInterface::ShowMessage("InitialSpinAngle = %.3f , SpinRate = %.3f\n",
	   InitialSpinAngle*GmatMathConstants::DEG_PER_RAD, SpinRate*GmatMathConstants::DEG_PER_RAD);
   #endif

   // Calculate RBIt, where t = atTime
   Real      dt             = (atTime - epoch) * GmatTimeConstants::SECS_PER_DAY;

   #ifdef DEBUG_PRECESSING_SPINNER
   MessageInterface::ShowMessage(
   "In PrecessingSpinner::Compute ... atTime = %.12f and dt = %.12f\n", atTime, dt);
   #endif

   // Calculate current angles   
   Real spinAngle       = ( spinRate * dt ) + initialSpinAngle;
   Real precessionAngle = ( precessionRate * dt ) + initialPrecessionAngle;

   // Rotate to align BodySpinAxis to NutationReferenceVector
   Rvector3 rvectorAlign = Cross(bodySpinAxisNormalized, nutationReferenceVectorNormalized);
   Rmatrix33 rmatrixInit(true);  // identity matrix, by default
   if(rvectorAlign.GetMagnitude()< 1.0e-5)
   {
	   // Let RmatrixInit as identity
   }
   else
   {
	   Rvector3 rvectorAlignNormalized = rvectorAlign / rvectorAlign.GetMagnitude();
	   Real angleAlign = GmatMathUtil::ACos(bodySpinAxisNormalized * nutationReferenceVectorNormalized);	//in radian
	   rmatrixInit = Attitude::EulerAxisAndAngleToDCM(rvectorAlignNormalized, angleAlign);
   }

   //Set Body 123 axis to perform 3-1-3 rotation
   xAxis.Set(1,0,0);
   yAxis.Set(0,1,0);
   Rvector3 bodyAxis1 = Cross(bodySpinAxisNormalized, xAxis);
   if (bodyAxis1.GetMagnitude()<1.0e-5)
   {
	   bodyAxis1.Set(0,1,0);
   }
   Rvector3 bodyAxis3 = bodySpinAxisNormalized;
   Rvector3 bodyAxis2 = Cross(bodyAxis3, bodyAxis1);

   bodyAxis1 = bodyAxis1 / bodyAxis1.GetMagnitude();
   bodyAxis2 = bodyAxis2 / bodyAxis2.GetMagnitude();
   bodyAxis3 = bodyAxis3 / bodyAxis3.GetMagnitude();

   //Rotate with PrecessingAngle
   Rmatrix33 rmatrixPrecession = Attitude::EulerAxisAndAngleToDCM(bodyAxis3,precessionAngle);

   //Rotate with NutationAngle
   Rmatrix33 rmatrixNutation = Attitude::EulerAxisAndAngleToDCM(bodyAxis1,nutationAngle);

   //Rotate with SpinAngle
   Rmatrix33 rmatrixSpin = Attitude::EulerAxisAndAngleToDCM(bodyAxis3,spinAngle);

   // Final rotation matrix from Inertial to Body
   dcm = rmatrixSpin * rmatrixNutation * rmatrixPrecession * rmatrixInit ;

   // Calculate angular velocity in Body 123 frame
   Real omega1 = precessionRate*GmatMathUtil::Sin(nutationAngle)*GmatMathUtil::Sin(spinAngle);
   Real omega2 = precessionRate*GmatMathUtil::Sin(nutationAngle)*GmatMathUtil::Cos(spinAngle);
   Real omega3 = precessionRate*GmatMathUtil::Cos(nutationAngle) + spinRate;

   Rvector3 omega123;
   omega123.Set(omega1, omega2, omega3);

   // Rotation matrix from Body 123 to Body xyz frame
   Rmatrix33 RBb(bodyAxis1(0), bodyAxis2(0), bodyAxis3(0), bodyAxis1(1), bodyAxis2(1), bodyAxis3(1), bodyAxis1(2), bodyAxis2(2), bodyAxis3(2));

   // Angular velocity of Body w.r.t. Inertial expressed in Body xyz frame
   Rvector3 omega_xyz = RBb * omega123;

   angVel(0) = omega_xyz(0);	//in radians
   angVel(1) = omega_xyz(1);
   angVel(2) = omega_xyz(2);
}


//------------------------------------------------------------------------------
//  private methods
//------------------------------------------------------------------------------
// none 
