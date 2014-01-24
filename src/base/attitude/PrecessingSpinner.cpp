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
#include "AttitudeConversionUtility.hpp"

#include "PrecessingSpinner.hpp"


//#define DEBUG_PRECESSING_SPINNER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none at this time

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

//------------------------------------------------------------------------------
//  void ComputeCosineMatrixAndAngularVelocity(Real atTime)
//------------------------------------------------------------------------------
/**
 * This method computes the attitude matrix and angular velocity
 *
 */
//------------------------------------------------------------------------------
void PrecessingSpinner::ComputeCosineMatrixAndAngularVelocity(Real atTime)
{
   if (!isInitialized || needsReinit) Initialize();

   Real bsMag = bodySpinAxis.GetMagnitude();
   Real nrMag = nutationReferenceVector.GetMagnitude();

   // Error message and vector normalization
   if ( bsMag < 1.0e-5 )
   {
	   throw AttitudeException(
	         "The size of BodySpinVector for PrecessingSpinner attitude "
	         "is too small");
   }
   if ( nrMag < 1.0e-5 )
   {
      throw AttitudeException(
            "The size of NutationReferenceVector for PrecessingSpinner "
            "attitude is too small");
   }

   bodySpinAxisNormalized            = bodySpinAxis / bsMag;
   nutationReferenceVectorNormalized = nutationReferenceVector / nrMag;

   static bool nutationWarningWritten = false;
   if ( (abs(nutationAngle) < 1.0e-5 ) && !nutationWarningWritten)
   {
	   MessageInterface::ShowMessage("Warning - Nutation angle is nearly zero\n");
	   nutationWarningWritten = true;
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
   Real      dt              = (atTime - epoch) * GmatTimeConstants::SECS_PER_DAY;

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
   Real alignMag = rvectorAlign.GetMagnitude();
   if(alignMag < 1.0e-5)
   {
	   // Let RmatrixInit as identity
   }
   else
   {
	   Rvector3 rvectorAlignNormalized = rvectorAlign / alignMag;
	   Real angleAlign = GmatMathUtil::ACos(bodySpinAxisNormalized * nutationReferenceVectorNormalized);	//in radian
	   rmatrixInit = AttitudeConversionUtility::EulerAxisAndAngleToDCM(rvectorAlignNormalized, angleAlign);
   }

   //Set Body 123 axis to perform 3-1-3 rotation
   xAxis.Set(1,0,0);
   yAxis.Set(0,1,0);
   Rvector3 bodyAxis1 = Cross(bodySpinAxisNormalized, xAxis);
   if (bodyAxis1.GetMagnitude() < 1.0e-5)
   {
	   bodyAxis1.Set(0,1,0);
   }
   Rvector3 bodyAxis3 = bodySpinAxisNormalized;
   Rvector3 bodyAxis2 = Cross(bodyAxis3, bodyAxis1);

   bodyAxis1 = bodyAxis1 / bodyAxis1.GetMagnitude();
   bodyAxis2 = bodyAxis2 / bodyAxis2.GetMagnitude();
   bodyAxis3 = bodyAxis3 / bodyAxis3.GetMagnitude();

   //Rotate with PrecessingAngle
   Rmatrix33 rmatrixPrecession = AttitudeConversionUtility::EulerAxisAndAngleToDCM(bodyAxis3,precessionAngle);

   //Rotate with NutationAngle
   Rmatrix33 rmatrixNutation   = AttitudeConversionUtility::EulerAxisAndAngleToDCM(bodyAxis1,nutationAngle);

   //Rotate with SpinAngle
   Rmatrix33 rmatrixSpin       = AttitudeConversionUtility::EulerAxisAndAngleToDCM(bodyAxis3,spinAngle);

   // Final rotation matrix from Inertial to Body
   dcm = rmatrixSpin * rmatrixNutation * rmatrixPrecession * rmatrixInit ;

   // Calculate angular velocity in Body 123 frame
   Real omega1 = precessionRate*GmatMathUtil::Sin(nutationAngle)*GmatMathUtil::Sin(spinAngle);
   Real omega2 = precessionRate*GmatMathUtil::Sin(nutationAngle)*GmatMathUtil::Cos(spinAngle);
   Real omega3 = precessionRate*GmatMathUtil::Cos(nutationAngle) + spinRate;

   Rvector3 omega123(omega1, omega2, omega3);

   // Rotation matrix from Body 123 to Body xyz frame
   Rmatrix33 RBb(bodyAxis1(0), bodyAxis2(0), bodyAxis3(0),
                 bodyAxis1(1), bodyAxis2(1), bodyAxis3(1),
                 bodyAxis1(2), bodyAxis2(2), bodyAxis3(2));

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
