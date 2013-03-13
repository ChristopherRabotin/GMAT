//$Id$
//------------------------------------------------------------------------------
//                               Spinner
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none 

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Spinner(const std::string &typeStr, const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Spinner class (constructor).
 */
//------------------------------------------------------------------------------
Spinner::Spinner(const std::string &itsName) : 
   Kinematic("Spinner", itsName)
{
   #ifdef DEBUG_SPINNER_INIT
   MessageInterface::ShowMessage("------- Entering Spinner (constructor)\n");
   #endif

   parameterCount    = SpinnerParamCount;
   objectTypeNames.push_back("Spinner");
   attitudeModelName     = "Spinner";
   modifyCoordSysAllowed = false;
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
   #ifdef DEBUG_SPINNER_INIT
   MessageInterface::ShowMessage("------- Entering Spinner (copy constructor)\n");
   #endif
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
 * Destroys the Spinner class (destructor).
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
      angVel[0] * GmatMathConstants::DEG_PER_RAD,angVel[1] * GmatMathConstants::DEG_PER_RAD,
      angVel[2] * GmatMathConstants::DEG_PER_RAD);
   #endif
   Rmatrix33 RiI;
   RB0I           = RBi;
   //angVel         = RBi * wIBi; // doesn't change  (spec mod per Steve 2006.04.05)
   
   #ifdef DEBUG_SPINNER_INIT
      std::stringstream RB0IStream;
      RB0IStream << RB0I << std::endl;
      MessageInterface::ShowMessage(
      "------- RB0I = %s\n", (RB0IStream.str()).c_str());
      std::stringstream IBBStream;
      IBBStream << angVel * GmatMathConstants::DEG_PER_RAD << std::endl;
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
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Spinner.
 *
 * @return clone of the Spinner.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Spinner::Clone() const
{
   return (new Spinner(*this));
}


//------------------------------------------------------------------------------
//  protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime)
//------------------------------------------------------------------------------
/**
 * This method computes the current CosineMatrix at the input time atTime.
 *
 * @param atTime the A1Mjd time at which to compute the attitude.
 *
 * @note This method will update the CosineMatrix parameter of the class.
 */
//------------------------------------------------------------------------------
void Spinner::ComputeCosineMatrixAndAngularVelocity(Real atTime)
{
   #ifdef DEBUG_SPINNER
   MessageInterface::ShowMessage(
   "Entering Spinner::Compute ... angVel = %.12f %.12f %.12f\n",
   angVel[0] * GmatMathConstants::DEG_PER_RAD, angVel[1] * GmatMathConstants::DEG_PER_RAD,
   angVel[2] * GmatMathConstants::DEG_PER_RAD);
   MessageInterface::ShowMessage("      isInitialized = %s\n", (isInitialized? "true" : "false"));
   MessageInterface::ShowMessage("      needsReinit   = %s\n", (needsReinit? "true" : "false"));
   #endif
   if (!isInitialized || needsReinit) Initialize();
   // now, RB0I and currentwIBB have been computed by Initialize
   
   // Calculate RBIt, where t = atTime
   Real      dt             = (atTime - epoch) * 
                              GmatTimeConstants::SECS_PER_DAY;
   #ifdef DEBUG_SPINNER
   MessageInterface::ShowMessage(
   "In Spinner::Compute ... atTime = %.12f and dt = %.12f\n", atTime, dt);
   #endif
   // Compute the Euler angle
   Real      theEAngle      = initialwMag * dt;
   Rmatrix33 RBB0t          = Attitude::EulerAxisAndAngleToDCM(
                              initialeAxis, theEAngle);
   
   dcm                      = RBB0t * RB0I;
   // currentwIBB already computed in Initialize 

   // save epoch and matrix
   epoch = epoch + dt/GmatTimeConstants::SECS_PER_DAY;
   RB0I  = dcm;

   #ifdef DEBUG_SPINNER
   MessageInterface::ShowMessage(
   "EXITING Spinner::Compute ... angVel = %.12f %.12f %.12f\n",
   angVel[0] * GmatMathConstants::DEG_PER_RAD, angVel[1] * GmatMathConstants::DEG_PER_RAD,
   angVel[2] * GmatMathConstants::DEG_PER_RAD);
   #endif
}


//------------------------------------------------------------------------------
//  private methods
//------------------------------------------------------------------------------
// none 
