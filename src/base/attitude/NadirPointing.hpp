//$Id$
//------------------------------------------------------------------------------
//                            Nadir Pointing
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Author: Yeerang Lim/KAIST
// Created: 2013.05.09
//
/**
 * Class definition for the Nadir (Nadir Pointing) attitude class.
 * 
 * @note 
 */
//------------------------------------------------------------------------------

#ifndef Nadir_hpp
#define Nadir_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Kinematic.hpp"


class GMAT_API NadirPointing : public Kinematic 
{
public:
   /// Constructor
   NadirPointing(const std::string &itsName = "");
   /// copy constructor
   NadirPointing(const NadirPointing& att);
   /// operator =
   NadirPointing& operator=(const NadirPointing& att);
   /// destructor
   virtual ~NadirPointing();
   
   /// Initialize the Nadir attitude
   virtual bool Initialize();

   /// inherited from GmatBase
   virtual GmatBase* Clone() const;
   
protected:
   enum 
   {
      NadirPointingParamCount = KinematicParamCount
   };
   
   static const Real DENOMINATOR_TOLERANCE;

   Rmatrix33 TRIAD(Rvector3& V1, Rvector3& V2, Rvector3& W1, Rvector3& W2);

   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime);

private:
   // Default constructor - not implemented
   //NadirPointing(); // MSVC compiler gives warning: multiple default constructors specified

};
#endif /*NadirPointing_hpp*/
