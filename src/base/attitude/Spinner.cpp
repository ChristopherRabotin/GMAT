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
   Kinematic::Initialize();
   // Compute the rotation matrix form inertial to Fi at the epoch time, t0
   Rvector bogus(6,100.0,200.0,300.0,400.0,500.0,600.0);
   Rvector bogus2 = refCS->FromMJ2000Eq(initialEpoch, bogus, true);
   Rmatrix33 RiI  = (refCS->GetLastRotationMatrix()).Transpose();
   RB0I           = RBi * RiI;
   currentwIBB    = RBi * wIBi; // doesn't change  (spec mod per Steve 2006.04.05)
   initialwMag    = currentwIBB.GetMagnitude();
   initialeAxis   = currentwIBB / initialwMag;
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
   if (!isInitialized) Initialize();
   // now, RB0I and currentwIBB have been computed by Initialize
   
   // Calculate RBIt, where t = atTime
   Real      dt             = (atTime - initialEpoch) * 
                              GmatTimeUtil::SECS_PER_DAY;
   // Compute the Euler angle
   Real      theEAngle      = initialwMag * dt;
   Rmatrix33 RBB0t          = EulerAxisAndAngleToDCM(
                              initialeAxis, theEAngle);
   
   currentRBI               = RBB0t * RB0I;
   // currentwIBB already computed in Initialize 
}


//---------------------------------
//  private methods
//---------------------------------
// none 

