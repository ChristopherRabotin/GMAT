//$Header$
//------------------------------------------------------------------------------
//                           InertialManeuverFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the class used for maneuvers defined inertially. 
 */
//------------------------------------------------------------------------------


#include "InertialManeuverFrame.hpp"


//------------------------------------------------------------------------------
// InertialManeuverFrame()
//------------------------------------------------------------------------------
/**
 * Constructs the inertial maneuver frame (default constructor).
 */
//------------------------------------------------------------------------------
InertialManeuverFrame::InertialManeuverFrame()
{
}


//------------------------------------------------------------------------------
// ~InertialManeuverFrame()
//------------------------------------------------------------------------------
/**
 * Destroys the inertial maneuver frame (destructor).
 */
//------------------------------------------------------------------------------
InertialManeuverFrame::~InertialManeuverFrame()
{
}


//------------------------------------------------------------------------------
//  InertialManeuverFrame(const InertialManeuverFrame& imf)
//------------------------------------------------------------------------------
/**
 * Constructs the inertial maneuver frame based on another one (copy 
 * constructor).
 * 
 * @param <imf> The frame that is being copied.
 */
//------------------------------------------------------------------------------
InertialManeuverFrame::InertialManeuverFrame(const InertialManeuverFrame& imf) :
   ManeuverFrame(imf)
{
}


//------------------------------------------------------------------------------
// InertialManeuverFrame& operator=(const InertialManeuverFrame& imf)
//------------------------------------------------------------------------------
/**
 * Sets one inertial maneuver frame object to match another (assignment operator).
 * 
 * @param <imf> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
InertialManeuverFrame& InertialManeuverFrame::operator=(
      const InertialManeuverFrame& imf)
{
   if (this == &imf)
      return *this;
        
   ManeuverFrame::operator=(imf);
   
   return *this;
}

//------------------------------------------------------------------------------
//  InertialManeuverFrame* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the InertialManeuverFrame.
 *
 * @return clone of the InertialManeuverFrame.
 */
//------------------------------------------------------------------------------
InertialManeuverFrame* InertialManeuverFrame::Clone() const
{
   return (new InertialManeuverFrame(*this));
}

    
//------------------------------------------------------------------------------
// void CalculateBasis()
//------------------------------------------------------------------------------
/**
 * Calculates the orthonormal basis vectors used for the maneuver frame.
 * 
 * For the inertial frame, the basis vectors are parallel to the spacecraft 
 * reference frame vectors, so the basis matrix is the identity matrix.
 */
//------------------------------------------------------------------------------
void InertialManeuverFrame::CalculateBasis()
{
   basisMatrix[0][0] = 1.0;
   basisMatrix[1][1] = 1.0;
   basisMatrix[2][2] = 1.0;
   basisMatrix[0][1] = 0.0;
   basisMatrix[0][2] = 0.0;
   basisMatrix[1][0] = 0.0;
   basisMatrix[1][2] = 0.0;
   basisMatrix[2][0] = 0.0;
   basisMatrix[2][1] = 0.0;
}
