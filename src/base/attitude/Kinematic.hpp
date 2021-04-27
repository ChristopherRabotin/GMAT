//$Id$
//------------------------------------------------------------------------------
//                               Kinematic
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
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

class GMAT_API Kinematic : public Attitude 
{
public:
   /// Constructor
   Kinematic(const std::string &typeStr, const std::string &itsName = "");
   /// Copy constructor
   Kinematic(const Kinematic &att);
   /// operator =
   Kinematic& operator=(const Kinematic &att);
   /// destructor
   virtual ~Kinematic();
protected:
   enum 
   {
       KinematicParamCount = AttitudeParamCount,
   };
   
private:
   // Default constructor - not implemented
   Kinematic();
   

};
#endif /*Kinematic_hpp*/
