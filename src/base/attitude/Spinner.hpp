//$Header$
//------------------------------------------------------------------------------
//                                 Spinner
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
 * Class definition for the Spinner  attitude class.
 * 
 * @note The time passed in for the epoch or passed into the methods 
 *       should be an A1Mjd (though its type is currently Real).
 * @note Methods called to set/get parameter values will require/return angle
 *       values in degrees and rate values in degrees/sec.  All other methods 
 *       assume/expect radians and radians/sec.
 */
//------------------------------------------------------------------------------


#ifndef Spinner_hpp
#define Spinner_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Kinematic.hpp"

/**
 * Class used for Spinner Attitude classes.
 * 
 * This class represents the spinner spacecraft attitude 
 * for GMAT.
 *  
 * @note The current builds of GMAT only contain kinematic attitude modeling.
 */
class GMAT_API Spinner : public Kinematic 
{
public:
   // Constructor
   Spinner(const std::string &itsName = "");
   // copy constructor
   Spinner(const Spinner& att);
   // operator =
   Spinner& operator=(const Spinner& att);
   // destructor
   virtual ~Spinner();
   
   // Initialize the Spinner attitude
   virtual bool Initialize();
   
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
 
protected:
   enum 
   {
       SpinnerParamCount = KinematicParamCount,
   };
   
   // leave these here, in case we need them later
   //static const std::string PARAMETER_TEXT[SpinnerParamCount - KinematicParamCount];
   
   //static const Gmat::ParameterType PARAMETER_TYPE[SpinnerParamCount - KinematicParamCount];

   /// the rotation matrix (from inertial to Fi) at the epoch time, t0
   Rmatrix33    RB0I;
   /// the spin axis in the inertial frame,  at the epoch time, t0
   //Rvector3     wIBI; // not needed, per updated specs 2006.03.28
   /// The magnitude of the wIBI vector
   Real         initialwMag;
   /// The euler axis, as computed from initial data
   Rvector3     initialeAxis;

   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime);

private:
   // Default constructor - not implemented
   Spinner();

};
#endif /*Spinner_hpp*/
