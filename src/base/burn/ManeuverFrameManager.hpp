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
 * Manager for all of the maneuver types. 
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

    const StringArray&          GetSupportedFrames(void);
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
