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

protected:
   enum 
   {
      PrecessingSpinnerParamCount = KinematicParamCount
   };

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
