//$Header$
//------------------------------------------------------------------------------
//                               CSFixed
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
 * Class implementation for the CSFixed class.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Attitude.hpp"
#include "AttitudeException.hpp"
#include "CSFixed.hpp"

//---------------------------------
// static data
//---------------------------------
// none 

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  CSFixed(const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the CSFixed class (Constructor).
 * The default value is the (0,0,0,1) quaternion.
 */
//------------------------------------------------------------------------------
CSFixed::CSFixed(const std::string &itsName) : 
   Kinematic("CSFixed",itsName)
{
   parameterCount = CSFixedParamCount;
   objectTypeNames.push_back("CSFixed");
 }
 
 //------------------------------------------------------------------------------
//  CSFixed(const CSFixed &att)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the CSFixed class as a copy of the
 * specified CSFixed class (copy constructor).
 *
 * @param <att> CSFixed object to copy.
 */
//------------------------------------------------------------------------------
CSFixed::CSFixed(const CSFixed& att) :
   Kinematic(att)
{
}
 
//------------------------------------------------------------------------------
//  CSFixed& operator= (const CSFixed& att)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the CSFixed class.
 *
 * @param att the CSFixed object whose data to assign to "this"
 *            CSFixed.
 *
 * @return "this" CSFixed with data of input CSFixed att.
 */
//------------------------------------------------------------------------------
CSFixed& CSFixed::operator=(const CSFixed& att)
{
   Kinematic::operator=(att);
   return *this;
}

//------------------------------------------------------------------------------
//  ~CSFixed()
//------------------------------------------------------------------------------
/**
 * Destructor for the CSFixed class.
 */
//------------------------------------------------------------------------------
CSFixed::~CSFixed()
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
bool CSFixed::Initialize()
{
   return Kinematic::Initialize();
   // may need to do other stuff here someday
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the CSFixed.
 *
 * @return clone of the CSFixed.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CSFixed::Clone(void) const
{
   return (new CSFixed(*this));
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
void CSFixed::ComputeCosineMatrixAndAngularVelocity(Real atTime)
{
   if (!isInitialized) 
   {
      Initialize();
   }
   // We know RBi, since it is computed on initialization
   // Get the rotation matrix from the reference coordinate system
   Rvector bogus(6,100.0,200.0,300.0,400.0,500.0,600.0);
   Rvector bogus2 = refCS->FromMJ2000Eq(atTime, bogus, true);
   Rmatrix33 RiI  = (refCS->GetLastRotationMatrix()).Transpose();
   // compute current attitude matrix
   currentRBI     = RBi * RiI;
   // Get the rotation dot matrix from the reference coordinate system
   // (it was computed on the last call to FromMJ2000Eq)
   Rmatrix33 RiIDot = (refCS->GetLastRotationDotMatrix()).Transpose();
   
   Rmatrix33 wxIBB  = - RBi * (RiIDot * (currentRBI.Transpose()));
   // set current angular velocity from this matrix
   currentwIBB(0)   = wxIBB(2,1);
   currentwIBB(1)   = wxIBB(0,2);
   currentwIBB(2)   = wxIBB(1,0);
}


//---------------------------------
//  private methods
//---------------------------------
// none 

 