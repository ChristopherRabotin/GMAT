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
   parameterCount    = SpinnerParamCount;
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
   angVel[0] * GmatMathUtil::DEG_PER_RAD,angVel[1] * GmatMathUtil::DEG_PER_RAD, 
   angVel[2] * GmatMathUtil::DEG_PER_RAD);
   #endif
   Rmatrix33 RiI;
   try
   {
      // Compute the rotation matrix from inertial to Fi at the epoch time, t0
      Rvector bogus(6,100.0,200.0,300.0,400.0,500.0,600.0);
      Rvector bogus2 = refCS->FromBaseSystem(epoch, bogus, true);  // @todo - do we need FromMJ2000Eq here?
      RiI  = (refCS->GetLastRotationMatrix()).Transpose();
   }
   catch (BaseException &)
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

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not;
 *         throws an exception if the parameter is out of the valid range
 *         of values.
 */
//---------------------------------------------------------------------------
bool Spinner::IsParameterReadOnly(const Integer id) const
{
   if (id == REFERENCE_COORDINATE_SYSTEM)
      return true;

   return Kinematic::IsParameterReadOnly(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool Spinner::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  bool  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id    ID for the requested parameter.
 * @param value string value for the requested parameter.
 *
 * @exception <AttitudeException> thrown if value is out of range
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool Spinner::SetStringParameter(const Integer id,
                                 const std::string &value)
{
   #ifdef DEBUG_SPINNER
      MessageInterface::ShowMessage(
            "Entering Spinner::SetStringParameter with id = %d, value = \"%s\", index = %d\n",
            id, value.c_str(), index);
   #endif
   if (id == REFERENCE_COORDINATE_SYSTEM)
   {
      static bool writeIgnoredMessage = true;
      if (writeIgnoredMessage)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** Setting coordinate system has no affect when attitude mode is Spinner.\n");
         writeIgnoredMessage = false;
      }
      return true;
   }
   return Kinematic::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const std::string label, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter label.
 *
 * @param label string label for the requested parameter.
 * @param value string value for the requested parameter.
 *
 * @exception <AttitudeException> thrown if value is out of range
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool Spinner::SetStringParameter(const std::string label,
                                 const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
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
   angVel[0] * GmatMathUtil::DEG_PER_RAD, angVel[1] * GmatMathUtil::DEG_PER_RAD, 
   angVel[2] * GmatMathUtil::DEG_PER_RAD);
   #endif
   if (isInitialized && needsReinit) Initialize();
   // now, RB0I and currentwIBB have been computed by Initialize
   
   // Calculate RBIt, where t = atTime
   Real      dt             = (atTime - epoch) * 
                              GmatTimeConstants::SECS_PER_DAY;
   // Compute the Euler angle
   Real      theEAngle      = initialwMag * dt;
   Rmatrix33 RBB0t          = Attitude::EulerAxisAndAngleToDCM(
                              initialeAxis, theEAngle);
   
   dcm                      = RBB0t * RB0I;
   // currentwIBB already computed in Initialize 
   #ifdef DEBUG_SPINNER
   MessageInterface::ShowMessage(
   "EXITING Spinner::Compute ... angVel = %.12f %.12f %.12f\n",
   angVel[0] * GmatMathUtil::DEG_PER_RAD, angVel[1] * GmatMathUtil::DEG_PER_RAD, 
   angVel[2] * GmatMathUtil::DEG_PER_RAD);
   #endif
}


//------------------------------------------------------------------------------
//  private methods
//------------------------------------------------------------------------------
// none 
