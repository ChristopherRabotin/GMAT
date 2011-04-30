//$Id$
//------------------------------------------------------------------------------
//                                 CSFixed
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
 * Class definition for the CSFixed (Coordinate System Fixed) attitude class.
 * 
 * @note The time passed in for the epoch or passed into the methods 
 *       should be an A1Mjd (though its type is currently Real).
 * @note Methods called to set/get parameter values will require/return angle
 *       values in degrees and rate values in degrees/sec.  All other methods 
 *       assume/expect radians and radians/sec.
 */
//------------------------------------------------------------------------------


#ifndef CSFixed_hpp
#define CSFixed_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Kinematic.hpp"
#include "Rmatrix33.hpp"

/**
 * Class used for Coordinate System Fixed Attitude classes.
 * 
 * This class represents the coordinate system fixed spacecraft attitude 
 * for GMAT.
 *  
 * @note The current builds of GMAT only contain kinematic attitude modeling.
 */
class GMAT_API CSFixed : public Kinematic 
{
public:
   // Constructor
   CSFixed(const std::string &itsName = "");
   // copy constructor
   CSFixed(const CSFixed& att);
   // operator =
   CSFixed& operator=(const CSFixed& att);
   // destructor
   virtual ~CSFixed();
   
   // Initialize the CSFixed attitude
   virtual bool Initialize();

   // inherited from GmatBase
   virtual GmatBase* Clone() const;


protected:
   enum 
   {
       CSFixedParamCount = KinematicParamCount,
   };
   
   // leave these here, in case we need them later
   //static const std::string PARAMETER_TEXT[CSFixedParamCount - KinematicParamCount];
   
   //static const Gmat::ParameterType PARAMETER_TYPE[CSFixedParamCount - KinematicParamCount];

   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime);

private:
   // Default constructor - not implemented
   //CSFixed(); // MSVC compiler gives warning: multiple default constructors specified

};
#endif /*CSFixed_hpp*/
