//$Header$
//------------------------------------------------------------------------------
//                          ManeuverFrameManager
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/26
//
/**
 * Manager for all of the maneuver frame types. 
 * 
 * This class implements the interface to different representations of a 
 * maneuver based on classes derived from the ManeuverFrame class.  To add a new
 * frame, insert the calls for the derived classes in the three indicated 
 * places.
 */
//------------------------------------------------------------------------------


#include "ManeuverFrameManager.hpp"

// Add headers for the Maneuver frames here
#include "InertialManeuverFrame.hpp"
#include "VnbManeuverFrame.hpp"
// #include "LvlhManeuverFrame.hpp"


ManeuverFrameManager::ManeuverFrameManager() :
    currentFrame            (NULL)
{
    // Add the name of each supported frame to this list
    frames.push_back("Inertial");
    frames.push_back("VNB");
}


ManeuverFrameManager::~ManeuverFrameManager()
{
    if (currentFrame)
        delete currentFrame;
}


const StringArray& ManeuverFrameManager::GetSupportedFrames(void)
{
    return frames;
}


ManeuverFrame* ManeuverFrameManager::GetFrameInstance(std::string frameType)
{
    if (currentFrame) {
        delete currentFrame;
        currentFrame = NULL;
    }
    
    // Add constructor calls for the supported frames here
    if (frameType == "Inertial")
        currentFrame = new InertialManeuverFrame;
    if (frameType == "VNB")
        currentFrame = new VnbManeuverFrame;

    return currentFrame;
}
