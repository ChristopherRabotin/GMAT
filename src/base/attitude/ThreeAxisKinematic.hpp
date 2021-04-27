//$Id$
//------------------------------------------------------------------------------
//                            ThreeAxisKinematic
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Author: Michael Stark / GSFC
// Created: 2019.05.xx
//
/**
 * Class definition for the ThreeAxisKinematic attitude class. This class
 * provides the capability to propagate quaternions over time using the angular
 * velocity vector
 * 
 * @note 
 */
//------------------------------------------------------------------------------

#ifndef ThreeAxis_hpp
#define ThreeAxis_hpp

#include "gmatdefs.hpp"
#include "Rmatrix.hpp"
#include "GmatBase.hpp"
#include "Kinematic.hpp"


class GMAT_API ThreeAxisKinematic : public Kinematic
{
public:
   /// Constructor
   ThreeAxisKinematic(const std::string &itsName = "");
   /// copy constructor
   ThreeAxisKinematic(const ThreeAxisKinematic& att);
   /// operator =
   ThreeAxisKinematic& operator=(const ThreeAxisKinematic& att);
   /// destructor
   virtual ~ThreeAxisKinematic();
   
   /// Initialize the Nadir attitude
   virtual bool Initialize();

   /// inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   // overrides GmatBase to allow commanding of angular velocity
   virtual bool IsParameterCommandModeSettable(const Integer id) const;

   
protected:
   enum 
   {
      ThreeAxisParamCount = KinematicParamCount
   };
   
   static const Real DENOMINATOR_TOLERANCE;
   
   /// 4x4 Identity Matrix
   Rmatrix I44;
   
   /// 4 x 4 skew symmetric matrix for propagating quaternions
   Rmatrix Omega;
   
   /// Magnitude of angular velocity vector
   Real wMag;

   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime);
   virtual void ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime);          
private:
   // Default constructor - not implemented
   //ThreeAxisKinematic(); // MSVC compiler gives warning: multiple default constructors specified

};
#endif /*ThreeAxis_hpp*/
