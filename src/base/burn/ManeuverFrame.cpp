//$Header$
//------------------------------------------------------------------------------
//                              ManeuverFrame
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
 * Defines the base class for maneuver reference frames. 
 */
//------------------------------------------------------------------------------


#include "ManeuverFrame.hpp"


ManeuverFrame::ManeuverFrame() :
    position        (NULL),
    velocity        (NULL)
{
    basisMatrix[0][0] = basisMatrix[1][1] = basisMatrix[2][2] = 1.0;
    basisMatrix[0][1] = basisMatrix[0][2] = basisMatrix[1][0] = 
    basisMatrix[1][2] = basisMatrix[2][0] = basisMatrix[2][1] = 0.0;
}


ManeuverFrame::~ManeuverFrame()
{
}


ManeuverFrame::ManeuverFrame(const ManeuverFrame& mf)
{}

ManeuverFrame& ManeuverFrame::operator=(const ManeuverFrame& mf)
{}

void ManeuverFrame::SetState(Real *pos, Real *vel)
{
    position = pos;
    if (vel)
        velocity = vel;
    else
        velocity = pos + 3;
}


void ManeuverFrame::CalculateBasis(Real basis[3][3])
{
    CalculateBasis();
    
    Integer i, j;
    for (i = 0; i < 3; ++i)
        for (j = 0; j < 3; ++j)
           basis[i][j] = basisMatrix[i][j];
}


std::string ManeuverFrame::GetFrameLabel(Integer id)
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

