//$Id$
//------------------------------------------------------------------------------
//                               Kinematic
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan/GSFC
// Created: 2006.03.23
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class definition for the Kinematic attitude base class.
 * 
 * @note The time passed in for the epoch or passed into the methods 
 *       should be an A1Mjd (though its type is currently Real).
 * @note Methods called to set/get parameter values will require/return angle
 *       values in degrees and rate values in degrees/sec.  All other methods 
 *       assume/expect radians and radians/sec.
 */
//------------------------------------------------------------------------------


#ifndef Kinematic_hpp
#define Kinematic_hpp

#include "Attitude.hpp"


/**
 * Base class used for Kinematic attitude classes.
 * 
 * This class is the base class for classes computing spacecraft attitude 
 * kinematically for GMAT.
 *  
 * @note The current builds of GMAT only contain kinematic attitude modeling.
 */
class GMAT_API Kinematic : public Attitude 
{
public:
   // Constructor
   Kinematic(const std::string &typeStr, const std::string &itsName = "");
   // Copy constructor
   Kinematic(const Kinematic &kin);
   // operator =
   Kinematic& operator=(const Kinematic &kin);
   // destructor
   virtual ~Kinematic();
protected:
   enum 
   {
       KinematicParamCount = AttitudeParamCount,
   };
   
   // leave these here, in case we need them later
   //static const std::string PARAMETER_TEXT[KinematicParamCount - AttitudeParamCount];
   //
   //static const Gmat::ParameterType PARAMETER_TYPE
   //             [KinematicParamCount - AttitudeParamCount];

private:
   // Default constructor - not implemented
   Kinematic();
   

};
#endif /*Kinematic_hpp*/

