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
//  InertialManeuverFrame(void)
//------------------------------------------------------------------------------
/**
 * Constructs the inertial maneuver frame (default constructor).
 */
//------------------------------------------------------------------------------
InertialManeuverFrame::InertialManeuverFrame()
{
}


//------------------------------------------------------------------------------
//  ~InertialManeuverFrame(void)
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
//  InertialManeuverFrame& operator=(const InertialManeuverFrame& imf)
//------------------------------------------------------------------------------
/**
 * Sets one inertial maneuver frame object to match another (assignment operator).
 * 
 * @param <imf> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
InertialManeuverFrame& InertialManeuverFrame::operator=(const InertialManeuverFrame& imf)
{
    if (this == &imf)
        return *this;
        
    ManeuverFrame::operator=(imf);
    return *this;
}

    
//------------------------------------------------------------------------------
//  void CalculateBasis(void)
//------------------------------------------------------------------------------
/**
 * Calculates the orthonormal basis vectors used for the maneuver frame.
 * 
 * For the inertial frame, the basis vectors are parallel to the spacecraft 
 * reference frame vectors, so the basis matrix is the identity matrix.
 */
//------------------------------------------------------------------------------
void InertialManeuverFrame::CalculateBasis(void)
{
    basisMatrix[0][0] = 
    basisMatrix[1][1] = 
    basisMatrix[2][2] = 1.0;
    basisMatrix[0][1] = 
    basisMatrix[0][2] = 
    basisMatrix[1][0] =
    basisMatrix[1][2] = 
    basisMatrix[2][0] = 
    basisMatrix[2][1] = 0.0;
}


//------------------------------------------------------------------------------
//  std::string GetFrameLabel(Integer id)
//------------------------------------------------------------------------------
/**
 * Gets the label for the maneuver frame principal axes.
 * 
 * @param <id> Integer ID of the axis -- 1, 2, or 3.
 * 
 * @return The label for the corresponding reference vector.
 */
//------------------------------------------------------------------------------
std::string InertialManeuverFrame::GetFrameLabel(Integer id)
{
    switch (id) {
        case 1:
            return "X";
        case 2:
            return "Y";
        case 3:
            return "Z";
        default:
            ;
    }
    return "Undefined";
}
