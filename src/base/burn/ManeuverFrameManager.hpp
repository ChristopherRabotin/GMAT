//$Id$
//------------------------------------------------------------------------------
//                          ManeuverFrameManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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


class GMAT_API ManeuverFrameManager
{
public:
	ManeuverFrameManager();
	virtual ~ManeuverFrameManager();

   StringArray&                GetSupportedFrames();
   ManeuverFrame*              GetFrameInstance(const std::string &frameType);

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

#endif // ManeuverFrameManager_hpp
