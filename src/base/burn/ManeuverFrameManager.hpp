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


#ifndef ManeuverFrameManager_hpp
#define ManeuverFrameManager_hpp


#include "ManeuverFrame.hpp"
#include <list>


class ManeuverFrameManager
{
public:
	ManeuverFrameManager();
	virtual ~ManeuverFrameManager();

    StringArray&                GetSupportedFrames(void);
    ManeuverFrame*              GetFrameInstance(std::string frameType);

protected:
    /// Table of available frames
    StringArray                 frames;
    /// Current frame used for the maneuver
    ManeuverFrame               *currentFrame;
    
private:
    /// Private to prevent accidental use of copy constructor
    ManeuverFrameManager(const ManeuverFrameManager&);
    /// Private to prevent accidental use of assignment operator
    ManeuverFrameManager&       operator=(const ManeuverFrameManager&);
};

#endif // MANEUVERFRAMEMANAGER_H
