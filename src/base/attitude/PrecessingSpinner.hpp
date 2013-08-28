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
 * Class definition for the PrecessingSpinner attitude class.
 * 
 * @note 
 */
//------------------------------------------------------------------------------

#ifndef PrecessingSpinner_hpp
#define PrecessingSpinner_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Kinematic.hpp"
//#include "Rmatrix33.hpp"


class GMAT_API PrecessingSpinner : public Kinematic 
{
public:
   /// Constructor
   PrecessingSpinner(const std::string &itsName = "");
   /// copy constructor
   PrecessingSpinner(const PrecessingSpinner& att);
   /// operator =
   PrecessingSpinner& operator=(const PrecessingSpinner& att);
   /// destructor
   virtual ~PrecessingSpinner();
   
   /// Initialize the Nadir attitude
   virtual bool Initialize();

   /// inherited from GmatBase
   virtual GmatBase* Clone() const;

   // Access methods derived classes can override
//   virtual std::string  GetParameterText(const Integer id) const;
//   virtual Integer      GetParameterID(const std::string &str) const;
//   virtual Gmat::ParameterType
//                        GetParameterType(const Integer id) const;
//
//   virtual Real         GetRealParameter(const Integer id) const;
//   virtual Real         SetRealParameter(const Integer id,
//                                         const Real value);


protected:
   enum 
   {
//	   NUTATION_REFERENCE_VECTOR_X = KinematicParamCount,
//	   NUTATION_REFERENCE_VECTOR_Y,
//	   NUTATION_REFERENCE_VECTOR_Z,
//	   BODY_SPIN_AXIS_X,
//	   BODY_SPIN_AXIS_Y,
//	   BODY_SPIN_AXIS_Z,
//	   INITIAL_PRECESSION_ANGLE,
//	   PRECESSION_RATE,
//	   NUTATION_ANGLE,
//	   INITIAL_SPIN_ANGLE,
//	   SPIN_RATE,
//	   PrecessingSpinnerParamCount
      PrecessingSpinnerParamCount = KinematicParamCount
   };

//   // static text and types for parameters
//   static const std::string PARAMETER_TEXT[PrecessingSpinnerParamCount - KinematicParamCount];
//
//   static const Gmat::ParameterType PARAMETER_TYPE[PrecessingSpinnerParamCount - KinematicParamCount];

   ///
//   Rvector3 NutationReferenceVector;
//   Rvector3 BodySpinAxis;
//   Real InitialPrecessionAngle;
//   Real PrecessionRate;
//   Real NutationAngle;
//   Real InitialSpinAngle;
//   Real SpinRate;
   ///
   Rvector3 xAxis;
   Rvector3 yAxis;
   Rvector3 bodySpinAxisNormalized;
   Rvector3 nutationReferenceVectorNormalized;

   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime);   

private:
   // Default constructor - not implemented
   //PrecessingSpinner(); // MSVC compiler gives warning: multiple default constructors specified

};
#endif /*PrecessingSpinner_hpp*/
