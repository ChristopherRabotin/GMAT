//$Header$
//------------------------------------------------------------------------------
//                              VnbManeuverFrame 
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
 * Defines the class used for maneuvers defined in VNB coordinates. 
 */
//------------------------------------------------------------------------------


#include "VnbManeuverFrame.hpp"


//------------------------------------------------------------------------------
//  VnbManeuverFrame(void)
//------------------------------------------------------------------------------
/**
 * Constructs the VNB maneuver frame (default constructor).
 */
//------------------------------------------------------------------------------
VnbManeuverFrame::VnbManeuverFrame()
{
}


//------------------------------------------------------------------------------
//  ~VnbManeuverFrame(void)
//------------------------------------------------------------------------------
/**
 * Destroys the VNB maneuver frame (destructor).
 */
//------------------------------------------------------------------------------
VnbManeuverFrame::~VnbManeuverFrame()
{
}


//------------------------------------------------------------------------------
//  VnbManeuverFrame(const InertialManeuverFrame& vmf)
//------------------------------------------------------------------------------
/**
 * Constructs the VNB maneuver frame based on another one (copy constructor).
 * 
 * @param <imf> The frame that is being copied.
 */
//------------------------------------------------------------------------------
VnbManeuverFrame::VnbManeuverFrame(const VnbManeuverFrame& vmf) :
    ManeuverFrame       (vmf)
{
}


//------------------------------------------------------------------------------
//  VnbManeuverFrame& operator=(const InertialManeuverFrame& vmf)
//------------------------------------------------------------------------------
/**
 * Sets one VNB maneuver frame object to match another (assignment operator).
 * 
 * @param <vmf> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
VnbManeuverFrame& VnbManeuverFrame::operator=(const VnbManeuverFrame& vmf)
{
    if (this == &vmf)
        return *this;
        
    ManeuverFrame::operator=(vmf);
    return *this;
}


//------------------------------------------------------------------------------
//  void CalculateBasis(void)
//------------------------------------------------------------------------------
/**
 * Calculates the orthonormal basis vectors used for the maneuver frame.
 * 
 * For the VNB frame, the basis vectors are calculated from the spacecraft 
 * position and velocity.  This method should be generalized so that the VNB
 * calculations can use any central body in the calculations.
 * 
 * @todo Generalize the basis vector calculations for the VNB maneuver frame.
 */
//------------------------------------------------------------------------------
void VnbManeuverFrame::CalculateBasis(void)
{
    if (!position || !velocity)
        throw BurnException("VNB frame undefined -- no position/velocity data.");
        
    // "X" direction is towards velocity
    Real temp[3], mag;
    memcpy(temp, velocity, 3*sizeof(Real));
    mag = sqrt(temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2]);
    if (mag == 0.0)
        throw BurnException("VNB frame undefined for velocity = 0.0.");
    basisMatrix[0][0] = temp[0] / mag;
    basisMatrix[1][0] = temp[1] / mag;
    basisMatrix[2][0] = temp[2] / mag;
    
    // Normal direction defined by r cross v
    temp[0] = position[1]*velocity[2] - position[2]*velocity[1];
    temp[1] = position[2]*velocity[0] - position[0]*velocity[2];
    temp[2] = position[0]*velocity[1] - position[1]*velocity[0];
    mag = sqrt(temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2]);
    if (mag == 0.0)
        throw BurnException("VNB frame undefined for |r x v| = 0.0.");
    basisMatrix[0][1] = temp[0] / mag;
    basisMatrix[1][1] = temp[1] / mag;
    basisMatrix[2][1] = temp[2] / mag;

    // Binormal direction defined by v cross n
    temp[0] = basisMatrix[1][0]*basisMatrix[2][1] - 
              basisMatrix[2][0]*basisMatrix[1][1];
    temp[1] = basisMatrix[2][0]*basisMatrix[0][1] - 
              basisMatrix[0][0]*basisMatrix[2][1];
    temp[2] = basisMatrix[0][0]*basisMatrix[1][1] - 
              basisMatrix[1][0]*basisMatrix[0][1];
    mag = sqrt(temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2]);
    if (mag == 0.0)
        throw BurnException("VNB frame undefined for |v x n| = 0.0.");
    basisMatrix[0][2] = temp[0] / mag;
    basisMatrix[1][2] = temp[1] / mag;
    basisMatrix[2][2] = temp[2] / mag;
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
std::string VnbManeuverFrame::GetFrameLabel(Integer id)
{
    switch (id) {
        case 1:
            return "V";
        case 2:
            return "N";
        case 3:
            return "B";
        default:
            ;
    }
    return "Undefined";
}
