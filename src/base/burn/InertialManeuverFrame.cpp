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


InertialManeuverFrame::InertialManeuverFrame()
{
}


InertialManeuverFrame::~InertialManeuverFrame()
{
}


InertialManeuverFrame::InertialManeuverFrame(const InertialManeuverFrame& imf) :
    ManeuverFrame(imf)
{
}


InertialManeuverFrame& InertialManeuverFrame::operator=(const InertialManeuverFrame& imf)
{
    if (this == &imf)
        return *this;
        
    ManeuverFrame::operator=(imf);
    return *this;
}

    
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
