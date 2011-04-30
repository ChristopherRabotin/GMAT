//$Id$
//------------------------------------------------------------------------------
//                           InertialManeuverFrame
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
// Created: 2003/12/17
//
/**
 * Defines the class used for maneuvers defined inertially. 
 */
//------------------------------------------------------------------------------


#ifndef InertialManeuverFrame_hpp
#define InertialManeuverFrame_hpp

#include "ManeuverFrame.hpp"

class GMAT_API InertialManeuverFrame : public ManeuverFrame
{
public:
   InertialManeuverFrame();
   virtual ~InertialManeuverFrame();
   InertialManeuverFrame(const InertialManeuverFrame& imf);
   InertialManeuverFrame&  operator=(const InertialManeuverFrame& imf);
 
protected:
   void                    CalculateBasis();
};

#endif // InertialManeuverFrame_hpp
