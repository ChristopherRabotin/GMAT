//$Header$
//------------------------------------------------------------------------------
//                               Spinner
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy C. Shoan/GSFC
// Created: 2006.03.24
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class implementation for the Spinner class.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Attitude.hpp"
#include "AttitudeException.hpp"
#include "Spinner.hpp"
#include "TimeTypes.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SPINNER_INIT
//#define DEBUG_SPINNER

//---------------------------------
// static data
//---------------------------------
// none 

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Spinner(const std::string &typeStr, const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Spinner class (Constructor).
 * The default value is the (0,0,0,1) quaternion.
 */
//------------------------------------------------------------------------------
Spinner::Spinner(const std::string &itsName) : 
   Kinematic("Spinner", itsName)
{
   parameterCount = SpinnerParamCount;
   objectTypeNames.push_back("Spinner");
   attitudeModelName = "Spinner";
 }
 
 //------------------------------------------------------------------------------
//  Spinner(const Spinner &att)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Spinner class as a copy of the
 * specified Spinner class (copy constructor).
 *
 * @param <att> Spinner object to copy.
 */
//------------------------------------------------------------------------------
Spinner::Spinner(const Spinner& att) :
   Kinematic(att)
{
}
 
//------------------------------------------------------------------------------
//  Spinner& operator= (const Spinner& att)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Spinner class.
 *
 * @param att the Spinner object whose data to assign to "this"
 *            Spinner.
 *
 * @return "this" Spinner with data of input Spinner att.
 */
//------------------------------------------------------------------------------
Spinner& Spinner::operator=(const Spinner& att)
{
   Kinematic::operator=(att);
   return *this;
}


//------------------------------------------------------------------------------
//  ~Spinner()
//------------------------------------------------------------------------------
/**
 * Destructor for the Spinner class.
 */
//------------------------------------------------------------------------------
Spinner::~Spinner()
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
bool Spinner::Initialize()
{
   #ifdef DEBUG_SPINNER_INIT
   MessageInterface::ShowMessage(
   "------- Entering Spinner::Initialize, and calling Kinematic::Initialize\n");
   #endif
   Kinematic::Initialize();
   #ifdef DEBUG_SPINNER_INIT
   MessageInterface::ShowMessage(
   "------- after calling Kinematic::Initialize, angVel = %.12f %.12f %.12f\n",
   angVel[0] * GmatMathUtil::DEG_PER_RAD,angVel[1] * GmatMathUtil::DEG_PER_RAD, 
   angVel[2] * GmatMathUtil::DEG_PER_RAD);
   #endif
   Rmatrix33 RiI;
   try
   {
      // Compute the rotation matrix from inertial to Fi at the epoch time, t0
      Rvector bogus(6,100.0,200.0,300.0,400.0,500.0,600.0);
      Rvector bogus2 = refCS->FromMJ2000Eq(epoch, bogus, true);
      RiI  = (refCS->GetLastRotationMatrix()).Transpose();
   }
   catch (BaseException &be)
   {
      #ifdef DEBUG_SPINNER_INIT
      MessageInterface::ShowMessage(
      "------- error initializing rotation matrix for Spinner\n");
      #endif
   }
   #ifdef DEBUG_SPINNER_INIT
   std::stringstream RBiStream;
   RBiStream << RBi << std::endl;
   MessageInterface::ShowMessage(
   "------- RBi = %s\n", (RBiStream.str()).c_str());
   std::stringstream RiIStream;
   RiIStream << RiI << std::endl;
   MessageInterface::ShowMessage(
   "------- RiI = %s\n", (RiIStream.str()).c_str());
   #endif
   
   RB0I           = RBi * RiI;
   //angVel         = RBi * wIBi; // doesn't change  (spec mod per Steve 2006.04.05)
   
   #ifdef DEBUG_SPINNER_INIT
   std::stringstream RB0IStream;
   RB0IStream << RB0I << std::endl;
   MessageInterface::ShowMessage(
   "------- RB0I = %s\n", (RB0IStream.str()).c_str());
   std::stringstream IBBStream;
   IBBStream << angVel * GmatMathUtil::DEG_PER_RAD << std::endl;
   MessageInterface::ShowMessage(
   "------- angVel = %s\n", (IBBStream.str()).c_str());
   #endif
   
   initialwMag    = angVel.GetMagnitude();
   if (initialwMag != 0.0) initialeAxis   = angVel / initialwMag;
   else     // is this right?               
      initialeAxis[0] = initialeAxis[1] = initialeAxis[2] = 0.0;
   
   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Spinner.
 *
 * @return clone of the Spinner.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Spinner::Clone(void) const
{
   return (new Spinner(*this));
}


//---------------------------------
//  protected methods
//---------------------------------

//------------------------------------------------------------------------------
//  virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime)
//------------------------------------------------------------------------------
/**
 * This mothod computes the current CosineMatrix at the input time atTime.
 *
 * @param atTime the A1Mjd time at which to compute the attitude.
 *
 * @note This method will update the CosineMatrix parameter of the class.
 * @note This method is pure virtual and will need to be implemented in
 *       the 'leaf' classes.
 * @note This method may not need to appear here - removal probable.
 */
//------------------------------------------------------------------------------
void Spinner::ComputeCosineMatrixAndAngularVelocity(Real atTime)
{
   #ifdef DEBUG_SPINNER
   MessageInterface::ShowMessage(
   "Entering Spinner::Compute ... angVel = %.12f %.12f %.12f\n",
   angVel[0] * GmatMathUtil::DEG_PER_RAD, angVel[1] * GmatMathUtil::DEG_PER_RAD, 
   angVel[2] * GmatMathUtil::DEG_PER_RAD);
   #endif
   if (!isInitialized) Initialize();
   // now, RB0I and currentwIBB have been computed by Initialize
   
   // Calculate RBIt, where t = atTime
   Real      dt             = (atTime - epoch) * 
                              GmatTimeUtil::SECS_PER_DAY;
   // Compute the Euler angle
   Real      theEAngle      = initialwMag * dt;
   Rmatrix33 RBB0t          = EulerAxisAndAngleToDCM(
                              initialeAxis, theEAngle);
   
   cosMat                   = RBB0t * RB0I;
   // currentwIBB already computed in Initialize 
   #ifdef DEBUG_SPINNER
   MessageInterface::ShowMessage(
   "EXITING Spinner::Compute ... angVel = %.12f %.12f %.12f\n",
   angVel[0] * GmatMathUtil::DEG_PER_RAD, angVel[1] * GmatMathUtil::DEG_PER_RAD, 
   angVel[2] * GmatMathUtil::DEG_PER_RAD);
   #endif
}


//---------------------------------
//  private methods
//---------------------------------
// none 

